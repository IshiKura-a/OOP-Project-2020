#include <iostream>
#include <random>
#include <ctime>
#include <iomanip>
#include <vector>
#include "MyAllocator.hpp"

// paramter used to choice allocator
#define _MY_ALLOC_ 0
#define _TEST_TUPLE_SIZE_ 1
#if _MY_ALLOC_

template<class T>
using MyAllocator = my::MyAlloc<T>;

#else

template<class T>
using MyAllocator = std::allocator<T>;

#endif
using Point2D = std::pair<int, int>;

// For we need to test small cases, each vector has size
// no more than 50. All the data are subject to uniform
// distribution.
const int TestSize = 100000;
const int PickSize = 1000;

#if not _TEST_TUPLE_SIZE_
const int TupleSize = 50;
#endif

int main()
{
#if _TEST_TUPLE_SIZE_
    int TupleSize;
    std::cin >> TupleSize;
#endif // _TEST_TUPLE_SIZE_

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, TestSize);
    std::uniform_int_distribution<> dis_size(1, TupleSize);

    // Record time.
    clock_t begin, end;
    begin = clock();
    // vector creation
    using IntVec = std::vector<int, MyAllocator<int>>;
    std::vector<IntVec, MyAllocator<IntVec>> vecints(TestSize);
    for (int i = 0; i < TestSize; i++)
        vecints[i].resize(dis_size(gen));

    using PointVec = std::vector<Point2D, MyAllocator<Point2D>>;
    std::vector<PointVec, MyAllocator<PointVec>> vecpts(TestSize);
    for (int i = 0; i < TestSize; i++)
        vecpts[i].resize(dis_size(gen));

    // vector resize
    for (int i = 0; i < PickSize; i++)
    {
        int idx = dis(gen) - 1;
        int size = dis_size(gen);
        vecints[idx].resize(size);
        vecpts[idx].resize(size);
    }
    end = clock();

    std::cout << "Run-time:" << std::scientific << (end - begin) << " Ticks\n";
    // vector element assignment, used to debug
#if _DEBUG
    {
        int val = 10;
        int idx1 = dis(gen) - 1;
        int idx2 = vecints[idx1].size() / 2;
        vecints[idx1][idx2] = val;
        if (vecints[idx1][idx2] == val)
            std::cout << "correct assignment in vecints: " << idx1 << std::endl;
        else
            std::cout << "incorrect assignment in vecints: " << idx1 << std::endl;
    }
    {
        Point2D val(11, 15);
        int idx1 = dis(gen) - 1;
        int idx2 = vecpts[idx1].size() / 2;
        vecpts[idx1][idx2] = val;
        if (vecpts[idx1][idx2] == val)
            std::cout << "correct assignment in vecpts: " << idx1 << std::endl;
        else
            std::cout << "incorrect assignment in vecpts: " << idx1 << std::endl;
    }
#endif

#if _DEBUG
    system("PAUSE");
#endif
    return 0;
}
