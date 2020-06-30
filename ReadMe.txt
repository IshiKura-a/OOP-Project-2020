OOP_Project
Author: 唐子豪 顾核金

Source:
	包括内存分配器实现MyAllocator.hpp和测试程序test.cpp
Test:
	包括三种参数测试Test_ALIGN, Test_MAX_BYTE, TEST_N_CHUNK, 可以使用test.bat测试（需要调整参数），测试结果在文件夹中。
	Test_TUPLE_SIZE为最终的性能测试，测试使用了test.py
	所有测试的结果均在Performance.xlsx中。

PROJ_OOP.exe为test.cpp编译后的程序，可以查看MyAllocator的运行速率，在test.cpp中调整参数_MY_ALLOC_可以选择编译的分配器，调整_TEST_TUPLE_SIZE_可以调整TupleSize的大小（可以从标准输入读取）。