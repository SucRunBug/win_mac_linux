#pragma once
#include "Common.h"

class ThreadCache
{
public:
	// 申请空间
	void* Allocate(size_t size);
	// 释放空间 - size用于算桶号
	void Deallocate(void* ptr, size_t size);
	// 去central cache申请
	void* FetchFromCentralCache(size_t index, size_t alignSize);
	// 整合过长的自由链表为大块span
	void ListTooLong(FreeList& list, size_t size);
private:
	FreeList _freeLists[NFREE_LISTS];
};

static _declspec(thread)ThreadCache* pTLSThreaCache = nullptr;
