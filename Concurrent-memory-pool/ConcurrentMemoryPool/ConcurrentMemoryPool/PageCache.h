#pragma once
#pragma once
#include "Common.h"
#include "ObjectPool.h"
#include "PageMap.h"

class PageCache
{
public:
	static PageCache* GetInstance()
	{
		return &_sInst;
	}
	//获取一个K页的span
	Span* NewSpan(size_t k);
	//将记录页号与span映射的map对象转化为span返回
	Span* MapObjectToSpan(void* obj);
	// 释放span到PC，合并相邻span
	void ReleaseSpanToPageCache(Span* span);
private:
	PageCache() {}
	PageCache(const PageCache&) = delete;
public:
	std::mutex _pageMtx;
private:
	SpanList _spanLists[NPAGES];
	static PageCache _sInst;
	//std::unordered_map<PAGE_ID, Span*> _idSpanMap;	// 页号映射span地址
	TCMalloc_PageMap1<32 - PAGE_SHIFT> _idSpanMap;		// 页号映射span地址
	ObjectPool<Span> _spanPool;	// 使用定长内存池替代new
};
