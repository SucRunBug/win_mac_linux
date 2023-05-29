#include "CentralCache.h"

CentralCache CentralCache::_sInst;

//��Span����batchNum�����������������ж����ö��١�
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
	//�ȿ���ǰ��SpanList�Ƿ���δ��������Span
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
	//CC��Span����PC��
	list._mtx.unlock();	// �Ȱ�CC��Ͱ���⿪��������������߳��ͷ��ڴ�����������������
	PageCache::GetInstance()->_pageMtx.lock();
	Span* span = PageCache::GetInstance()->NewSpan(SizeClass::NumMovePage(size));
	span->_isUse = true;
	span->_objSize = size;
	PageCache::GetInstance()->_pageMtx.unlock();
	char* start = (char*)(span->_pageId << PAGE_SHIFT);		//����span����ڴ����ʼλ��
	size_t bytes = span->_n << PAGE_SHIFT;		//����ڴ�Ĵ�С���ֽ�����
	char* end = start + bytes;
	//�Ѵ���ڴ��г�С�飬�����ӵ�freeList��
	if (start == nullptr)
	{
		int x = 0;
	}
	span->_freeList = start;
	start += size;	//����һ�飬ȥ��ͷ������β��
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
	list._mtx.lock();	//֮ǰ��span�����з�ʱ����������������̲߳������
	list.PushFront(span);
	return span;
}

//��һ�������Ķ����ͷŵ�span���
void CentralCache::ReleaseListToSpans(void* start, size_t size)
{
	size_t index = SizeClass::Index(size);
	_spanLists[index]._mtx.lock();
	//����Ҫ�ϲ����������������ĸ�span
	while (start)
	{
		void* next = NextObj(start);
		Span* span = PageCache::GetInstance()->MapObjectToSpan(start);	// start�ǵ�ַ��ת��ҳ�ź��ҵ���Ӧ��span��ַ
		// ���ͷŶ���һ�ζε�ͷ�嵽span��
		NextObj(start) = span->_freeList;
		span->_freeList = start;
		span->_useCount--;
		if (span->_useCount == 0)	// ˵��span���зֳ�ȥ������С���ڴ涼�����ˣ����span�Ϳ����ٻ��ո�PC�ٺϲ�
		{
			_spanLists[index].Erase(span);
			span->_freeList = nullptr;
			span->_next = nullptr;
			span->_prev = nullptr;
			//�ӽ�Ͱ��
			_spanLists[index]._mtx.unlock();
			// �ӽ�PC��
			PageCache::GetInstance()->_pageMtx.lock();
			PageCache::GetInstance()->ReleaseSpanToPageCache(span);
			PageCache::GetInstance()->_pageMtx.unlock();
			_spanLists[index]._mtx.lock();
		}
		start = next;
	}
	_spanLists[index]._mtx.unlock();
}

