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
	//��CC��ȡһ�������Ķ����TC
	size_t FetchRangeObj(void*& start, void*& end, size_t n, size_t size);
	//��ȡһ���ǿ�Span
	static Span* GetOneSpan(SpanList& list, size_t size);
	//��һ�������Ķ����ͷŵ�span���
	void ReleaseListToSpans(void* start, size_t size);
private:
	CentralCache()
	{}
	CentralCache(const CentralCache&) = delete;
private:
	SpanList _spanLists[NFREE_LISTS];
	static CentralCache _sInst;
};