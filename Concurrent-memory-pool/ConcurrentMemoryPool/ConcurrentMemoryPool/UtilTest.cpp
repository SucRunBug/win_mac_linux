#include "ObjectPool.h"
#include "ConcurrentAlloc.h"

void Alloc1()
{
	for (size_t i = 0; i < 5; ++i)
	{
		void* ptr = ConcurrentAlloc(6);
	}
}

void Alloc2()
{
	for (size_t i = 0; i < 5; ++i)
	{
		void* ptr = ConcurrentAlloc(7);
	}
}

void TLSTest()
{
	std::thread t1(Alloc1);
	std::thread t2(Alloc2);
	t1.join();
	t2.join();
}

void TestConcurrentAlloc1()
{
	void* p1 = ConcurrentAlloc(6);
	void* p2 = ConcurrentAlloc(8);
	void* p3 = ConcurrentAlloc(1);
	void* p4 = ConcurrentAlloc(7);
	void* p5 = ConcurrentAlloc(8);
	void* p6 = ConcurrentAlloc(7);
	void* p7 = ConcurrentAlloc(8);
	std::cout << p1 << std::endl;
	std::cout << p2 << std::endl;
	std::cout << p3 << std::endl;
	std::cout << p4 << std::endl;
	std::cout << p5 << std::endl;
	std::cout << p6 << std::endl;
	std::cout << p7 << std::endl;
	ConcurrentFree(p1);
	ConcurrentFree(p2);
	ConcurrentFree(p3);
	ConcurrentFree(p4);
	ConcurrentFree(p5);
	ConcurrentFree(p6);
	ConcurrentFree(p7);
}

void TestConcurrentAlloc2()
{
	for (size_t i = 0; i < 1024; ++i)
	{
		void* p1 = ConcurrentAlloc(6);
		std::cout << p1 << std::endl;
	}
	void* p2 = ConcurrentAlloc(8);
	std::cout << p2 << std::endl;
}

// 用于测试地址映射页号的正确性
void TestAddressShift()
{
	PAGE_ID id1 = 2000, id2 = 2001;
	char* p1 = (char*)(id1 << PAGE_SHIFT);
	char* p2 = (char*)(id2 << PAGE_SHIFT);
	while (p1 < p2)
	{
		std::cout << (void*)p1 << ":" << ((PAGE_ID)p1 >> PAGE_SHIFT) << std::endl;
		p1 += 8;
	}
}

// 多线程测试 - 同一个桶冲突和不同桶测试
void MultiThreadAlloc1()
{
	std::vector<void*> v;
	for (size_t i = 0; i < 7; ++i)
	{
		void* ptr = ConcurrentAlloc(6);
		v.push_back(ptr);
	}
	for (auto e : v)
	{
		ConcurrentFree(e);
	}
}

void MultiThreadAlloc2()
{
	std::vector<void*> v;
	for (size_t i = 0; i < 7; ++i)
	{
		void* ptr = ConcurrentAlloc(16);
	}
	for (auto e : v)
	{
		ConcurrentFree(e);
	}
}

void TestMultiThread()
{
	std::thread t1(MultiThreadAlloc1);

	std::thread t2(MultiThreadAlloc2);
	t1.join();
	t2.join();
}

void BigAlloc()
{
	void* p1 = ConcurrentAlloc(257 * 1024);	// 257kb
	ConcurrentFree(p1);
	void* p2 = ConcurrentAlloc(129 * 8 * 1024);	// 129page >1mb
	ConcurrentFree(p2);
}

int main()
{
	TLSTest();
	std::cout << sizeof(void*) << std::endl;
	TestConcurrentAlloc1();
	TestConcurrentAlloc2();
	TestAddressShift();

	// 回收测试
	TestConcurrentAlloc1();
	
	// 多线程测试
	TestMultiThread();

	// 大于256KB内存申请测试
	BigAlloc();
	return 0;
}