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


static const size_t MAX_BYTES = 256 * 1024;		// thread cache���ռ�
static const size_t NFREE_LISTS = 208;		//���208��Ͱ
static const size_t NPAGES = 129;	//PageCache��128��Ͱ��0��Ͱ���ã�
static const size_t PAGE_SHIFT = 13;	//2^13����8KB����ʾһҳ�Ĵ�С

#ifdef _WIN64
typedef unsigned long long PAGE_ID;
#elif _WIN32 
typedef size_t PAGE_ID;
#endif // _WIN64
//Linux �¿���չ

inline static void* SystemAlloc(size_t kPage)
{
#ifdef _WIN32
	void* ptr = VirtualAlloc(0, kPage << 13, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else 
	// linux��brk��mmap��
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
	// sbrk unmmap��
#endif // _WIN32
}

static void*& NextObj(void* obj)
{
	return *(void**)obj;
}

// ������������ͷŵ��ڴ��
class FreeList
{
public:
	void Push(void* obj)
	{
		//ͷ��
		NextObj(obj) = _freeList;
		_freeList = obj;
		++_size;

	}
	void* Pop()
	{
		//ͷɾ
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
	//������ռ䵽Ͱ��
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
	//��������ռ�
	static inline size_t _RoundUp(size_t bytes, size_t alignNum)
	{
		//��bytes��λ������������λ�����ٱ��������λ
		return ((bytes + (alignNum - 1)) & ~(alignNum - 1));
	}
	static inline size_t RoundUp(size_t size)	// ����ʵ������Ŀռ䣬������������Ƭ
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
			return _RoundUp(size, 1 << PAGE_SHIFT);	// ����һ��������������ƫ���������޸�
			/*assert(false);
			return -1;*/
		}
	}
	//����Ͱ��
	static inline size_t _Index(size_t bytes, size_t align_shift)
	{
		//�ȼ����ǵڼ���Ͱ���ټ�ȥ1���Ͱ��
		return ((bytes + (1 << align_shift) - 1) >> align_shift) - 1;
	}
	static inline size_t Index(size_t bytes)
	{
		assert(bytes <= MAX_BYTES);
		static int group_array[4] = { 16, 56, 56, 56 };		// ���Դ�������׶ε�Ͱ������
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
	//TC��CC��ȡ���ٸ�С����
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
	//����ʵ����Ҫ����ҳ
	static size_t NumMovePage(size_t size)
	{
		size_t num = NumMoveSize(size);
		size_t npage = num * size;	//ʵ������ռ�
		npage >>= PAGE_SHIFT;	//����8KB�����ҳ��
		if (npage == 0)
			npage = 1;
		return npage;
	}
};

//λ��CentralCache�У�����������ҳ�Ĵ���ڴ�
struct Span
{
	PAGE_ID _pageId = 0;	//����ڴ���ʼҳ��ҳ��
	size_t _n = 0;	//ҳ��
	Span* _next = nullptr;
	Span* _prev = nullptr;
	size_t _useCount = 0;	//�кõ�С���ڴ棬�����thread cache�ļ���
	void* _freeList = nullptr;	//�����кõ�С���ڴ����������
	bool _isUse = false;	//�Ƿ��ڱ�ʹ��
	size_t _objSize = 0;	// �кõ�С����Ĵ�С
};

//��ͷ˫��ѭ������
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
	//���ڱ���SpanList�ķ��������
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
		Erase(front);	//δDel
		return front;
	}
private:
	Span* _head;
public:
	std::mutex _mtx;	//Ͱ��
};