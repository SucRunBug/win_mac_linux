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
	//��ȡһ��Kҳ��span
	Span* NewSpan(size_t k);
	//����¼ҳ����spanӳ���map����ת��Ϊspan����
	Span* MapObjectToSpan(void* obj);
	// �ͷ�span��PC���ϲ�����span
	void ReleaseSpanToPageCache(Span* span);
private:
	PageCache() {}
	PageCache(const PageCache&) = delete;
public:
	std::mutex _pageMtx;
private:
	SpanList _spanLists[NPAGES];
	static PageCache _sInst;
	//std::unordered_map<PAGE_ID, Span*> _idSpanMap;	// ҳ��ӳ��span��ַ
	TCMalloc_PageMap1<32 - PAGE_SHIFT> _idSpanMap;		// ҳ��ӳ��span��ַ
	ObjectPool<Span> _spanPool;	// ʹ�ö����ڴ�����new
};
