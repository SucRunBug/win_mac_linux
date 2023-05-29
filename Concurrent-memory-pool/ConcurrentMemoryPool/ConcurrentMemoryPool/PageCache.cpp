#include "PageCache.h"

PageCache PageCache::_sInst;

Span* PageCache::NewSpan(size_t k)
{
	assert(k > 0);
	if (k > NPAGES - 1)	// 超过128页，去堆
	{
		void* ptr = SystemAlloc(k);
		// Span* span = new Span;
		Span* span = _spanPool.New();
		span->_pageId = (PAGE_ID)ptr >> PAGE_SHIFT;
		span->_n = k;
		// _idSpanMap[span->_pageId] = span;
		_idSpanMap.set(span->_pageId, span);
		return span;
	}
	//看当前桶有无span
	if (!_spanLists[k].Empty())
	{
		Span* kSpan = _spanLists[k].PopFront();
		for (PAGE_ID i = 0; i < kSpan->_n; ++i)
		{
			// _idSpanMap[kSpan->_pageId + i] = kSpan;
			_idSpanMap.set(kSpan->_pageId + i, kSpan);
		}
		return kSpan;
	}
	//检查后面桶有无span，有：切分；无：向系统要
	for (size_t i = k + 1; i < NPAGES; ++i)
	{
		if (!_spanLists[i].Empty())
		{
			//切分成两部分：k号桶所对应大小的k页span，剩下的i-k页span挂在i-k页号桶上
			Span* nSpan = _spanLists[i].PopFront();
			//Span* kSpan = new Span;
			Span* kSpan = _spanPool.New();
			//在nSpan上利用PageId切一个k页的span
			kSpan->_pageId = nSpan->_pageId;
			kSpan->_n = k;
			nSpan->_pageId += k;
			nSpan->_n -= k;
			_spanLists[nSpan->_n].PushFront(nSpan);	// 挂回 
			// 存储nSpan的首位页号与nSpan的映射，方便PC回收内存进行的合并查找
			//_idSpanMap[nSpan->_pageId] = nSpan;
			_idSpanMap.set(nSpan->_pageId, nSpan);
			//_idSpanMap[nSpan->_pageId + nSpan->_n - 1] = nSpan;	// @? 方便向左找
			_idSpanMap.set(nSpan->_pageId + nSpan->_n - 1, nSpan);
			// 建立id和span映射，便于CC回收小块内存时找到对应span
			for (PAGE_ID i = 0; i < kSpan->_n; ++i)
			{
				//_idSpanMap[kSpan->_pageId + i] = kSpan;
				_idSpanMap.set(kSpan->_pageId + i, kSpan);
			}
			return kSpan;
		}
	}
	//找堆要128页的span
	//Span* bigSpan = new Span;
	Span* bigSpan = _spanPool.New();
	void* ptr = SystemAlloc(NPAGES - 1);
	bigSpan->_pageId = (PAGE_ID)ptr >> PAGE_SHIFT;	//地址转换页号除以8k
	bigSpan->_n = NPAGES - 1;
	_spanLists[bigSpan->_n].PushFront(bigSpan);
	return NewSpan(k);
}
//将记录页号与span映射的map对象转化为span返回
Span* PageCache::MapObjectToSpan(void* obj)
{
	PAGE_ID id = (PAGE_ID)obj >> PAGE_SHIFT;
	/*std::unique_lock<std::mutex> lock(_pageMtx);
	auto ret = _idSpanMap.find(id);
	if (ret != _idSpanMap.end())
	{
		return ret->second;
	}
	else
	{
		assert(false);
		return nullptr;
	}*/
	auto ret = (Span*)_idSpanMap.get(id);
	assert(ret != nullptr);
	return ret;
}

// 释放span到PC，合并前后相邻的span，缓解内存碎片
void PageCache::ReleaseSpanToPageCache(Span* span)
{
	if (span->_n > NPAGES - 1)	// 属于堆
	{
		void* ptr = (void*)(span->_pageId << PAGE_SHIFT);
		SystemFree(ptr);
		//delete span;
		_spanPool.Delete(span);
		return;
	}
	// 合并左边span
	while (true)
	{
		PAGE_ID prevId = span->_pageId - 1;
		//auto ret = _idSpanMap.find(prevId);
		//// 左边（前面）页号不存在时
		//if (ret == _idSpanMap.end())
		//{
		//	break;
		//}
		auto ret = (Span*)_idSpanMap.get(prevId);
		if (ret == nullptr) // 左边（前面）页号不存在时
		{
			break;
		}
		// 左边span在使用
		Span* prevSpan = ret;
		if (prevSpan->_isUse == true)
		{
			break;
		}
		// 合并出超过128页的span，无法管理，就不合并
		if (prevSpan->_n + span->_n > NPAGES - 1)
		{
			break;
		}
		// 满足条件，向左合并
		span->_pageId = prevSpan->_pageId;
		span->_n += prevSpan->_n;
		_spanLists[prevSpan->_n].Erase(prevSpan);
		//delete prevSpan;	// @? 上面一行代码是删掉PC中的相应span，方便更大块的span转移到下面。  
		_spanPool.Delete(prevSpan);
	}
	// 向后合并
	while (true)
	{
		PAGE_ID nextId = span->_pageId + span->_n;
		/*auto ret = _idSpanMap.find(nextId);
		if (ret == _idSpanMap.end())
		{
			break;
		}*/
		auto ret = (Span*)_idSpanMap.get(nextId);
		if (ret == nullptr)
		{
			break;
		}
		Span* nextSpan = ret;
		if (nextSpan->_isUse == true)
		{
			break;
		}
		if (span->_n + nextSpan->_n > NPAGES - 1)
		{
			break;
		}

		span->_n += nextSpan->_n;
		_spanLists[nextSpan->_n].Erase(nextSpan);
		//delete nextSpan;
		_spanPool.Delete(nextSpan);
	}
	_spanLists[span->_n].PushFront(span);
	span->_isUse = false;
	/*_idSpanMap[span->_pageId] = span;
	_idSpanMap[span->_pageId + span->_n - 1] = span;*/
	_idSpanMap.set(span->_pageId, span);
	_idSpanMap.set(span->_pageId + span->_n - 1, span);
}
