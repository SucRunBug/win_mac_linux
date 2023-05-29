#pragma once
#include "Common.h"

template <class T>
class ObjectPool
{
public:
	T* New()
	{
		T* obj = nullptr;	//要申请的内存
		if (_freeList != nullptr)
		{
			//从_freeList中取空间
			void* next = *(void**)_freeList;
			obj = (T*)_freeList;
			_freeList = next;
		}
		else
		{
			//从_memory中取空间
			if (_remainBytes < sizeof(T))
			{
				_remainBytes = 128 * 1024;
				//_memory = (char*)malloc(_remainBytes);
				_memory = (char*)SystemAlloc(_remainBytes >> 13);
				if (_memory == nullptr)
				{
					throw std::bad_alloc();
				}
			}
			obj = (T*)_memory;
			size_t objSize = sizeof(T) > sizeof(void*) ? sizeof(T) : sizeof(void*);
			_memory += objSize;	//解决T为char时，存不下指针
			_remainBytes -= objSize;
		}
		new(obj)T;	//定位new，显示调用T的构造
		//std::cout << "new success" << std::endl;
		return obj;
	}
	void Delete(T* obj)
	{
		obj->~T();	//显示调用T的析构
		//将回收的块头插到_freeList	[|_]->[|_]->nullptr	[指向下一块空间的指针|_]
		*(void**)obj = _freeList;	//取void*大小的空间存放下一块空间——解决32位和64位不兼容问题
		_freeList = obj;
		//std::cout << "delete success" << std::endl;
	}
private:
	char* _memory = nullptr;	//管理内存池
	void* _freeList = nullptr;	//管理回收空间
	size_t _remainBytes = 0;	//内存池剩余空间
};

