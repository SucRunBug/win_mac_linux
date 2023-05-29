#pragma once
#include "Common.h"

template <class T>
class ObjectPool
{
public:
	T* New()
	{
		T* obj = nullptr;	//Ҫ������ڴ�
		if (_freeList != nullptr)
		{
			//��_freeList��ȡ�ռ�
			void* next = *(void**)_freeList;
			obj = (T*)_freeList;
			_freeList = next;
		}
		else
		{
			//��_memory��ȡ�ռ�
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
			_memory += objSize;	//���TΪcharʱ���治��ָ��
			_remainBytes -= objSize;
		}
		new(obj)T;	//��λnew����ʾ����T�Ĺ���
		//std::cout << "new success" << std::endl;
		return obj;
	}
	void Delete(T* obj)
	{
		obj->~T();	//��ʾ����T������
		//�����յĿ�ͷ�嵽_freeList	[|_]->[|_]->nullptr	[ָ����һ��ռ��ָ��|_]
		*(void**)obj = _freeList;	//ȡvoid*��С�Ŀռ�����һ��ռ䡪�����32λ��64λ����������
		_freeList = obj;
		//std::cout << "delete success" << std::endl;
	}
private:
	char* _memory = nullptr;	//�����ڴ��
	void* _freeList = nullptr;	//������տռ�
	size_t _remainBytes = 0;	//�ڴ��ʣ��ռ�
};

