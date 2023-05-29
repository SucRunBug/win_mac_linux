#pragma once
#include "Common.h"
#include "PageCache.h"

class CentralCache
{
public:
	static CentralCache* GetInstance()
	{
		return &_sInst;
	}
	//从CC获取一定数量的对象给TC
	size_t FetchRangeObj(void*& start, void*& end, size_t n, size_t size);
	//获取一个非空Span
	static Span* GetOneSpan(SpanList& list, size_t size);
	//将一定数量的对象释放到span跨度
	void ReleaseListToSpans(void* start, size_t size);
private:
	CentralCache()
	{}
	CentralCache(const CentralCache&) = delete;
private:
	SpanList _spanLists[NFREE_LISTS];
	static CentralCache _sInst;
};