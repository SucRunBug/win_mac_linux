#include "ConcurrentAlloc.h"
// ntimes 一轮申请和释放内存的次数
// nworks 线程数
// rounds 轮次
void BenchmarkMalloc(size_t ntimes, size_t nworks, size_t rounds)
{
	std::vector<std::thread> vthread(nworks);
	std::atomic<size_t> malloc_costtime = { 0 };
	std::atomic<size_t> free_costtime = { 0 };

	for (size_t k = 0; k < nworks; ++k)
	{
		vthread[k] = std::thread([&, k]() {
			std::vector<void*> v;
			v.reserve(ntimes);

			for (size_t j = 0; j < rounds; ++j)
			{
				size_t begin1 = clock();
				for (size_t i = 0; i < ntimes; i++)
				{
					//v.push_back(malloc(16));
					v.push_back(malloc((16 + i) % 8192 + 1));
				}
				size_t end1 = clock();

				size_t begin2 = clock();
				for (size_t i = 0; i < ntimes; i++)
				{
					free(v[i]);
				}
				size_t end2 = clock();
				v.clear();

				malloc_costtime += (end1 - begin1);
				free_costtime += (end2 - begin2);
			}
			});
	}

	for (auto& t : vthread)
	{
		t.join();
	}
	std::cout << nworks << "个线程并发执行" << rounds << "轮次，每轮次malloc " << ntimes << "次：花费：" << malloc_costtime << "ms" << std::endl;
	std::cout << nworks << "个线程并发执行" << rounds << "轮次，每轮次free " << ntimes << "次：花费：" << free_costtime << " ms" << std::endl;
	std::cout << nworks << "个线程并发执行malloc&free " << nworks * rounds * ntimes << "次，总计花费：" << malloc_costtime + free_costtime << " ms" << std::endl;
}


// 单轮次申请释放次数 线程数 轮次
void BenchmarkConcurrentMalloc(size_t ntimes, size_t nworks, size_t rounds)
{
	std::vector<std::thread> vthread(nworks);
	std::atomic<size_t> malloc_costtime = 0;
	std::atomic<size_t> free_costtime = 0;

	for (size_t k = 0; k < nworks; ++k)
	{
		vthread[k] = std::thread([&]() {
			std::vector<void*> v;
			v.reserve(ntimes);

			for (size_t j = 0; j < rounds; ++j)
			{
				size_t begin1 = clock();
				for (size_t i = 0; i < ntimes; i++)
				{
					//v.push_back(ConcurrentAlloc(16));
					v.push_back(ConcurrentAlloc((16 + i) % 8192 + 1));
				}
				size_t end1 = clock();

				size_t begin2 = clock();
				for (size_t i = 0; i < ntimes; i++)
				{
					ConcurrentFree(v[i]);
				}
				size_t end2 = clock();
				v.clear();

				malloc_costtime += (end1 - begin1);
				free_costtime += (end2 - begin2);
			}
			});
	}

	for (auto& t : vthread)
	{
		t.join();
	}

	std::cout << nworks << "个线程并发执行" << rounds << "轮次，每轮次concurrent alloc " << ntimes << "次：花费：" << malloc_costtime << "ms" << std::endl;
	std::cout << nworks << "个线程并发执行" << rounds << "轮次，每轮次concurrent dealloc " << ntimes << "次：花费：" << free_costtime << " ms" << std::endl;
	std::cout << nworks << "个线程并发执行concurrent alloc&dealloc " << nworks * rounds * ntimes << "次，总计花费：" << malloc_costtime + free_costtime << " ms" << std::endl;
}

//int main()
//{
//	size_t n = 10000;
//	std::cout << "==========================================================" << std::endl;
//	//BenchmarkConcurrentMalloc(n, 1, 10);
//	BenchmarkConcurrentMalloc(n, 4, 10);
//	std::cout << std::endl << std::endl;
//
//	BenchmarkMalloc(n, 4, 10);
//	std::cout << "==========================================================" << std::endl;
//
//	return 0;
//}