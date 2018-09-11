#include "stdafx.h"
#include <iostream>
#include <time.h>
#include <chrono>
#include <iomanip>

void TestQuickSort(int size);
int Partition(int* pArray, int lo, int hi);
void QuickSort(int* pArray, int lo, int hi);
void Swap(int* pArray, int a, int b);

int main()
{
	srand(unsigned int(time(nullptr)));
	const int startSize{ 1 };
	const int maxSize{ 1024 };
	const int sizeMultiplier{ 2 };
	std::cout << "--- Test Quick Sort ---\n\n";
	std::cout << "Size\tAverage time\n";
	for (int i{ startSize }; i <= 1024; i *= sizeMultiplier)
	{
		TestQuickSort(i);
	}
	std::cout << "\n\nPress ENTER to quit ";
	std::cin.get();
    return 0;
}

void TestQuickSort(const int size)
{
	const int iterations{ 100 };
	double averageTime{};

	for (int i{ 0 }; i < iterations; ++i)
	{
		int* pArray{ new int[size]() };
		for (int i{ 0 }; i < size; ++i)
		{
			pArray[i] = rand() % RAND_MAX;
		}

		auto start = std::chrono::steady_clock::now();
		QuickSort(pArray, 0, size - 1);
		auto end = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		averageTime += duration.count();
	}
	averageTime /= iterations;
	std::cout << size << '\t' << std::fixed << std::setprecision(16) << averageTime << '\n';
}

int Partition(int* pArray, int lo, int hi)
{
	int pivot{ pArray[hi] };
	int i{ lo - 1 };
	for (int j{ lo }; j < hi; ++j)
	{
		if (pArray[j] < pivot)
		{
			++i;
			if (pArray[i] != pArray[j])
			{
				std::swap(pArray[i], pArray[j]);
			}
		}
	}
	std::swap(pArray[i+1], pArray[hi]);
	return i + 1;
}

void QuickSort(int* pArray, int lo, int hi)
{
	if (lo < hi)
	{
		int pivot{ Partition(pArray, lo, hi) };
		QuickSort(pArray, lo, pivot - 1);
		QuickSort(pArray, pivot + 1, hi);
	}
}

void Swap(int* pArray, int a, int b)
{
	int holder{ pArray[a] };
	pArray[a] = pArray[b];
	pArray[b] = holder;
}
