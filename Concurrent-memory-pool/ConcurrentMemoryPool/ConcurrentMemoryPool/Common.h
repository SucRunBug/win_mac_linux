#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <algorithm>
#include <unordered_map>

#include <assert.h>
#ifdef _WIN32
#include <windows.h>
#elif
// linux
#endif 


static const size_t MAX_BYTES = 256 * 1024;		// thread cache最大空间
static const size_t NFREE_LISTS = 208;		//最多208个桶
static const size_t NPAGES = 129;	//PageCache中128个桶（0号桶不用）
static const size_t PAGE_SHIFT = 13;	//2^13代表8KB，表示一页的大小

#ifdef _WIN64
typedef unsigned long long PAGE_ID;
#elif _WIN32 
typedef size_t PAGE_ID;
#endif // _WIN64
//Linux 下可扩展

inline static void* SystemAlloc(size_t kPage)
{
#ifdef _WIN32
	void* ptr = VirtualAlloc(0, kPage << 13, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else 
	// linux下brk，mmap等
#endif // _WIN32
	if (ptr == nullptr)
	{
		throw std::bad_alloc();
	}
	return ptr;
}

inline static void SystemFree(void* ptr)
{
#ifdef _WIN32
	VirtualFree(ptr, 0, MEM_RELEASE);
#else 
	// sbrk unmmap等
#endif // _WIN32
}

static void*& NextObj(void* obj)
{
	return *(void**)obj;
}

// 自由链表管理释放的内存块
class FreeList
{
public:
	void Push(void* obj)
	{
		//头插
		NextObj(obj) = _freeList;
		_freeList = obj;
		++_size;

	}
	void* Pop()
	{
		//头删
		assert(_freeList != nullptr);
		void* obj = _freeList;
		_freeList = NextObj(obj);
		--_size;
		return obj;
	}
	bool Empty()
	{
		return _freeList == nullptr;
	}
	size_t& MaxSize()
	{
		return _maxSize;
	}
	size_t Size()
	{
		return _size;
	}
	//插入多块空间到桶中
	void PushRange(void* start, void* end, size_t n)
	{
		NextObj(end) = _freeList;
		_freeList = start;
		////////////////////////////
		int i = 0;
		void* cur = start;
		while (cur)
		{
			cur = NextObj(cur);
			++i;
		}
		if (n != i)
		{
			int x = 0;
		}
		///////////////////////////
		_size += n;
	}
	void PopRange(void*& start, void*& end, size_t n)
	{
		assert(n <= _size);
		start = _freeList;
		end = start;
		for (size_t i = 0; i < n - 1; ++i)
		{
			/*if (end == nullptr)
			{
				int x = 0;
			}*/
			end = NextObj(end);
		}
		_freeList = NextObj(end);
		NextObj(end) = nullptr;
		_size -= n;
	}
private:
	void* _freeList = nullptr;
	size_t _maxSize = 1;
	size_t _size = 0;
};

class SizeClass
{
public:
	//计算申请空间
	static inline size_t _RoundUp(size_t bytes, size_t alignNum)
	{
		//将bytes进位到对齐数所在位数，再保留其最高位
		return ((bytes + (alignNum - 1)) & ~(alignNum - 1));
	}
	static inline size_t RoundUp(size_t size)	// 计算实际申请的空间，可能留有内碎片
	{
		if (size <= 128)
		{
			return _RoundUp(size, 8);
		}
		else if (size <= 1024)
		{
			return _RoundUp(size, 16);
		}
		else if (size <= 1024 * 8)
		{
			return _RoundUp(size, 128);
		}
		else if (size <= 1024 * 64)
		{
			return _RoundUp(size, 1024);
		}
		else if (size <= 1024 * 256)
		{
			return _RoundUp(size, 1024 * 8);
		}
		else
		{
			return _RoundUp(size, 1 << PAGE_SHIFT);	// 和上一个条件的区别是偏移量可以修改
			/*assert(false);
			return -1;*/
		}
	}
	//计算桶号
	static inline size_t _Index(size_t bytes, size_t align_shift)
	{
		//先计算是第几个桶，再减去1算出桶号
		return ((bytes + (1 << align_shift) - 1) >> align_shift) - 1;
	}
	static inline size_t Index(size_t bytes)
	{
		assert(bytes <= MAX_BYTES);
		static int group_array[4] = { 16, 56, 56, 56 };		// 各自代表各个阶段的桶的数量
		if (bytes <= 128)
		{
			return _Index(bytes, 3);
		}
		else if (bytes <= 1024)
		{
			return _Index(bytes, 4) + group_array[0];
		}
		else if (bytes <= 1024 * 8)
		{
			return _Index(bytes, 7) + group_array[0] + group_array[1];
		}
		else if (bytes <= 1024 * 64)
		{
			return _Index(bytes, 10) + group_array[0] + group_array[1] + group_array[2];
		}
		else if (bytes <= 1024 * 256)
		{
			return _Index(bytes, 13) + group_array[0] + group_array[1] + group_array[2] + group_array[3];
		}
		else
		{
			assert(false);
			return -1;
		}
	}
	//TC从CC获取多少个小对象
	static size_t NumMoveSize(size_t size)
	{
		assert(size > 0);
		int num = MAX_BYTES / size;
		if (num < 2)
		{
			num = 2;
		}
		else if (num > 512)
		{
			num = 512;
		}
		return num;
	}
	//计算实际需要多少页
	static size_t NumMovePage(size_t size)
	{
		size_t num = NumMoveSize(size);
		size_t npage = num * size;	//实际申请空间
		npage >>= PAGE_SHIFT;	//除以8KB，获得页数
		if (npage == 0)
			npage = 1;
		return npage;
	}
};

//位于CentralCache中，管理多个连续页的大块内存
struct Span
{
	PAGE_ID _pageId = 0;	//大块内存起始页的页号
	size_t _n = 0;	//页数
	Span* _next = nullptr;
	Span* _prev = nullptr;
	size_t _useCount = 0;	//切好的小块内存，分配给thread cache的计数
	void* _freeList = nullptr;	//管理切好的小块内存的自由链表
	bool _isUse = false;	//是否在被使用
	size_t _objSize = 0;	// 切好的小对象的大小
};

//带头双向循环链表
class SpanList
{
public:
	SpanList()
	{
		_head = new Span;
		_head->_next = _head;
		_head->_prev = _head;
	}
	void Insert(Span* pos, Span* newSpan)
	{
		assert(pos);
		assert(newSpan);
		Span* prev = pos->_prev;
		prev->_next = newSpan;
		newSpan->_prev = prev;
		newSpan->_next = pos;
		pos->_prev = newSpan;
	}
	void Erase(Span* pos)
	{
		assert(pos);
		assert(pos != _head);
		//if (pos == _head)
		//{
		//	int x = 0;
		//}
		Span* prev = pos->_prev;
		Span* next = pos->_next;
		prev->_next = next;
		next->_prev = prev;
	}
	//用于遍历SpanList的仿造迭代器
	Span* Begin()
	{
		return _head->_next;
	}
	Span* End()
	{
		return _head;
	}
	void PushFront(Span* span)
	{
		Insert(Begin(), span);
	}
	bool Empty()
	{
		return _head->_next == _head;
	}
	Span* PopFront()
	{
		Span* front = _head->_next;
		Erase(front);	//未Del
		return front;
	}
private:
	Span* _head;
public:
	std::mutex _mtx;	//桶锁
};