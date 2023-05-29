#pragma once
#include "Common.h"

class ThreadCache
{
public:
	// ����ռ�
	void* Allocate(size_t size);
	// �ͷſռ� - size������Ͱ��
	void Deallocate(void* ptr, size_t size);
	// ȥcentral cache����
	void* FetchFromCentralCache(size_t index, size_t alignSize);
	// ���Ϲ�������������Ϊ���span
	void ListTooLong(FreeList& list, size_t size);
private:
	FreeList _freeLists[NFREE_LISTS];
};

static _declspec(thread)ThreadCache* pTLSThreaCache = nullptr;
