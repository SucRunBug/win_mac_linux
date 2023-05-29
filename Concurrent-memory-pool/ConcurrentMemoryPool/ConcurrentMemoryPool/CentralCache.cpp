#include "CentralCache.h"

CentralCache CentralCache::_sInst;

//从Span中拿batchNum个对象。若不够，则有多少拿多少。
size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t batchNum, size_t size)
{
	size_t index = SizeClass::Index(size);
	_spanLists[index]._mtx.lock();
	Span* span = GetOneSpan(_spanLists[index], size);
	assert(span);
	assert(span->_freeList);
	start = span->_freeList;
	end = start;
	size_t i = 0;
	size_t actualNum = 1;
	while (i < batchNum - 1 && NextObj(end) != nullptr)
	{
		end = NextObj(end);
		++i;
		++actualNum;
	}
	span->_freeList = NextObj(end);
	NextObj(end) = nullptr;
	span->_useCount += actualNum;
	//////////////////////////
	//int j = 0;
	//void* cur = start;
	//while (cur)
	//{
	//	cur = NextObj(cur);
	//	j++;
	//}
	//if (actualNum != j)
	//{
	//	int x = 0;
	//}
	//////////////////////////
	_spanLists[index]._mtx.unlock();

	return actualNum;
}

Span* CentralCache::GetOneSpan(SpanList& list, size_t size)
{
	//先看当前的SpanList是否还有未分配对象的Span
	Span* it = list.Begin();
	while (it != list.End())
	{
		if (it->_freeList != nullptr)
		{
			return it;
		}
		else
		{
			it = it->_next;
		}
	}
	//CC无Span，向PC拿
	list._mtx.unlock();	// 先把CC的桶锁解开，这样如果其他线程释放内存对象回来，不会阻塞
	PageCache::GetInstance()->_pageMtx.lock();
	Span* span = PageCache::GetInstance()->NewSpan(SizeClass::NumMovePage(size));
	span->_isUse = true;
	span->_objSize = size;
	PageCache::GetInstance()->_pageMtx.unlock();
	char* start = (char*)(span->_pageId << PAGE_SHIFT);		//计算span大块内存的起始位置
	size_t bytes = span->_n << PAGE_SHIFT;		//大块内存的大小（字节数）
	char* end = start + bytes;
	//把大块内存切成小块，并连接到freeList上
	if (start == nullptr)
	{
		int x = 0;
	}
	span->_freeList = start;
	start += size;	//先切一块，去做头，方便尾插
	void* tail = span->_freeList;
	while (start < end)
	{
		NextObj(tail) = start;
		tail = NextObj(tail);
		start += size;
	}
	NextObj(tail) = nullptr;
	////////////////////////////////////////
	//int j = 0;
	//void* cur = span->_freeList;
	//while (cur)
	//{
	//	cur = NextObj(cur);
	//	j++;
	//}
	//if (j != (bytes / size))
	//{
	//	int x = 0;
	//}
	///////////////////////////////////////
	list._mtx.lock();	//之前对span进行切分时，无需加锁，其他线程不会访问
	list.PushFront(span);
	return span;
}

//将一定数量的对象释放到span跨度
void CentralCache::ReleaseListToSpans(void* start, size_t size)
{
	size_t index = SizeClass::Index(size);
	_spanLists[index]._mtx.lock();
	//计算要合并的自由链表属于哪个span
	while (start)
	{
		void* next = NextObj(start);
		Span* span = PageCache::GetInstance()->MapObjectToSpan(start);	// start是地址，转成页号后找到对应的span地址
		// 将释放对象一段段地头插到span里
		NextObj(start) = span->_freeList;
		span->_freeList = start;
		span->_useCount--;
		if (span->_useCount == 0)	// 说明span的切分出去的所有小块内存都回来了，这个span就可以再回收给PC再合并
		{
			_spanLists[index].Erase(span);
			span->_freeList = nullptr;
			span->_next = nullptr;
			span->_prev = nullptr;
			//加解桶锁
			_spanLists[index]._mtx.unlock();
			// 加解PC锁
			PageCache::GetInstance()->_pageMtx.lock();
			PageCache::GetInstance()->ReleaseSpanToPageCache(span);
			PageCache::GetInstance()->_pageMtx.unlock();
			_spanLists[index]._mtx.lock();
		}
		start = next;
	}
	_spanLists[index]._mtx.unlock();
}

