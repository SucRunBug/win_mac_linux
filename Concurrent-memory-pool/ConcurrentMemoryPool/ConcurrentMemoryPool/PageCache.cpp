#include "PageCache.h"

PageCache PageCache::_sInst;

Span* PageCache::NewSpan(size_t k)
{
	assert(k > 0);
	if (k > NPAGES - 1)	// ����128ҳ��ȥ��
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
	//����ǰͰ����span
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
	//������Ͱ����span���У��з֣��ޣ���ϵͳҪ
	for (size_t i = k + 1; i < NPAGES; ++i)
	{
		if (!_spanLists[i].Empty())
		{
			//�зֳ������֣�k��Ͱ����Ӧ��С��kҳspan��ʣ�µ�i-kҳspan����i-kҳ��Ͱ��
			Span* nSpan = _spanLists[i].PopFront();
			//Span* kSpan = new Span;
			Span* kSpan = _spanPool.New();
			//��nSpan������PageId��һ��kҳ��span
			kSpan->_pageId = nSpan->_pageId;
			kSpan->_n = k;
			nSpan->_pageId += k;
			nSpan->_n -= k;
			_spanLists[nSpan->_n].PushFront(nSpan);	// �һ� 
			// �洢nSpan����λҳ����nSpan��ӳ�䣬����PC�����ڴ���еĺϲ�����
			//_idSpanMap[nSpan->_pageId] = nSpan;
			_idSpanMap.set(nSpan->_pageId, nSpan);
			//_idSpanMap[nSpan->_pageId + nSpan->_n - 1] = nSpan;	// @? ����������
			_idSpanMap.set(nSpan->_pageId + nSpan->_n - 1, nSpan);
			// ����id��spanӳ�䣬����CC����С���ڴ�ʱ�ҵ���Ӧspan
			for (PAGE_ID i = 0; i < kSpan->_n; ++i)
			{
				//_idSpanMap[kSpan->_pageId + i] = kSpan;
				_idSpanMap.set(kSpan->_pageId + i, kSpan);
			}
			return kSpan;
		}
	}
	//�Ҷ�Ҫ128ҳ��span
	//Span* bigSpan = new Span;
	Span* bigSpan = _spanPool.New();
	void* ptr = SystemAlloc(NPAGES - 1);
	bigSpan->_pageId = (PAGE_ID)ptr >> PAGE_SHIFT;	//��ַת��ҳ�ų���8k
	bigSpan->_n = NPAGES - 1;
	_spanLists[bigSpan->_n].PushFront(bigSpan);
	return NewSpan(k);
}
//����¼ҳ����spanӳ���map����ת��Ϊspan����
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

// �ͷ�span��PC���ϲ�ǰ�����ڵ�span�������ڴ���Ƭ
void PageCache::ReleaseSpanToPageCache(Span* span)
{
	if (span->_n > NPAGES - 1)	// ���ڶ�
	{
		void* ptr = (void*)(span->_pageId << PAGE_SHIFT);
		SystemFree(ptr);
		//delete span;
		_spanPool.Delete(span);
		return;
	}
	// �ϲ����span
	while (true)
	{
		PAGE_ID prevId = span->_pageId - 1;
		//auto ret = _idSpanMap.find(prevId);
		//// ��ߣ�ǰ�棩ҳ�Ų�����ʱ
		//if (ret == _idSpanMap.end())
		//{
		//	break;
		//}
		auto ret = (Span*)_idSpanMap.get(prevId);
		if (ret == nullptr) // ��ߣ�ǰ�棩ҳ�Ų�����ʱ
		{
			break;
		}
		// ���span��ʹ��
		Span* prevSpan = ret;
		if (prevSpan->_isUse == true)
		{
			break;
		}
		// �ϲ�������128ҳ��span���޷������Ͳ��ϲ�
		if (prevSpan->_n + span->_n > NPAGES - 1)
		{
			break;
		}
		// ��������������ϲ�
		span->_pageId = prevSpan->_pageId;
		span->_n += prevSpan->_n;
		_spanLists[prevSpan->_n].Erase(prevSpan);
		//delete prevSpan;	// @? ����һ�д�����ɾ��PC�е���Ӧspan�����������spanת�Ƶ����档  
		_spanPool.Delete(prevSpan);
	}
	// ���ϲ�
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
