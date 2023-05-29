#include "ThreadCache.h"
#include "CentralCache.h"

void* ThreadCache::Allocate(size_t size)
{
	assert(size <= MAX_BYTES);
	size_t alignSize = SizeClass::RoundUp(size);	// ʵ�ʷֵ����ڴ�
	size_t index = SizeClass::Index(size);		// Ͱ�� @?--Ϊʲô��ֱ����alignSize��Ͱ��
	if (!_freeLists[index].Empty())
	{
		return _freeLists[index].Pop();
	}
	else
	{
		return FetchFromCentralCache(index, alignSize);
	}
}

// ���Ϲ�������������Ϊ���span
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
	if (_freeLists[index].Size() >= _freeLists[index].MaxSize())	// ��������ڵ��������һ����������ĸ����ͻ�һ��list��CC
	{
		ListTooLong(_freeLists[index], size);
	}
}

void* ThreadCache::FetchFromCentralCache(size_t index, size_t size)
{
	//����ʼ
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
