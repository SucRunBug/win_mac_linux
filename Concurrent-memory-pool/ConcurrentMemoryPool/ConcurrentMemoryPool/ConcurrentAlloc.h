#pragma once
#include "Common.h"
#include "ThreadCache.h"
#include "PageCache.h"

static void* ConcurrentAlloc(size_t size)
{
	if (size > MAX_BYTES)
	{
		size_t alignSize = SizeClass::RoundUp(size);
		size_t kpage = alignSize >> PAGE_SHIFT;
		PageCache::GetInstance()->_pageMtx.lock();
		Span* span = PageCache::GetInstance()->NewSpan(kpage);
		PageCache::GetInstance()->_pageMtx.unlock();
		void* ptr = (void*)(span->_pageId << PAGE_SHIFT);
		return ptr;
	}
	else
	{
		// 通过TLS，每个线程无锁地获取自己独立的ThreadCache对象 
		if (pTLSThreaCache == nullptr)
		{
			static ObjectPool<ThreadCache> tcPool;
			//pTLSThreaCache = new ThreadCache;
			pTLSThreaCache = tcPool.New();
		}
		// std::cout << std::this_thread::get_id() << " : " << pTLSThreaCache << std::endl;
		return pTLSThreaCache->Allocate(size);
	}

}

static void* ConcurrentFree(void* ptr)
{
	Span* span = PageCache::GetInstance()->MapObjectToSpan(ptr);
	size_t size = span->_objSize;
	if (size > MAX_BYTES)
	{
		Span* span = PageCache::GetInstance()->MapObjectToSpan(ptr);
		PageCache::GetInstance()->_pageMtx.lock();
		PageCache::GetInstance()->ReleaseSpanToPageCache(span);
		PageCache::GetInstance()->_pageMtx.unlock();
	}
	else
	{
		assert(pTLSThreaCache);
		pTLSThreaCache->Deallocate(ptr, size);
	}
	return nullptr;
}
