#include "ThreadCache.h"
#include "CentralCache.h"

void* ThreadCache::Allocate(size_t size)
{
	assert(size <= MAX_BYTES);
	size_t alignSize = SizeClass::RoundUp(size);	// 实际分到的内存
	size_t index = SizeClass::Index(size);		// 桶号 @?--为什么不直接拿alignSize算桶号
	if (!_freeLists[index].Empty())
	{
		return _freeLists[index].Pop();
	}
	else
	{
		return FetchFromCentralCache(index, alignSize);
	}
}

// 整合过长的自由链表为大块span
void ThreadCache::ListTooLong(FreeList& list, size_t size)
{
	void* start = nullptr;
	void* end = nullptr;
	list.PopRange(start, end, list.MaxSize());
	CentralCache::GetInstance()->ReleaseListToSpans(start, size);
}

void ThreadCache::Deallocate(void* ptr, size_t size)
{
	assert(ptr);
	assert(size <= MAX_BYTES);
	size_t index = SizeClass::Index(size);
	_freeLists[index].Push(ptr);
	if (_freeLists[index].Size() >= _freeLists[index].MaxSize())	// 自由链表节点个数超过一次批量申请的个数就还一段list给CC
	{
		ListTooLong(_freeLists[index], size);
	}
}

void* ThreadCache::FetchFromCentralCache(size_t index, size_t size)
{
	//慢开始
	size_t bathNum = min(_freeLists[index].MaxSize(), SizeClass::NumMoveSize(size));
	if (_freeLists[index].MaxSize() == bathNum)
	{
		_freeLists[index].MaxSize() += 1;
	}

	void* start = nullptr;
	void* end = nullptr;
	size_t actualNum = CentralCache::GetInstance()->FetchRangeObj(start, end, bathNum, size);
	assert(actualNum >= 1);
	if (actualNum == 1)
	{
		start = end;
		return start;
	}
	else
	{
		_freeLists[index].PushRange(NextObj(start), end, actualNum - 1);
		return start;
	}
}
