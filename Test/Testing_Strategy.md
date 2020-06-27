# Testing and Analysis

## Testing Strategy

Here we test three aspects of our allocator to find the optimal parameters to accelerate allocation.

As is mentioned in the project requirement,  the allocator should excel in dealing with **small-scale** allocations. The maximum tuples size in the test is set to **50**, while the number of tests is set to **100,000**.

Each test and its result is included in a dependent directory, the testing framework is **test.bat**. It is easy to change parameters in the .bat file for each test case.

In each test, std::allocator is added as **control group**. After evaluating the parameters, the best one is chosen and modified in the source code for further evaluation of other parameters.

The final performance evaluation uses **test.py** for data handling. All the test results are integrated in the **performance.xlsx**.

* MAX_BYTES
  * The maximum size of block allowed to allocate in the allocator. If the required size is larger than it, **malloc()** will be used directly.
  * Obviously, too small MAX\_BYTES​ will significantly reduce efficiency and too large ones will impose burden on the system. Besides, a MAX_BYTES is sufficiently good if it is a **power of 2**. Hence, **256, 512, 1024** are chosen as the candidate value.

* ALIGIN
  * The increment of block size used in free_list, that is, the block size of free_list[$i$] should be $(i+1)\times{ALIGN}$ 
  * In this test, the smallest tuple is no smaller than int64_t. So the value of ALIGN​ is larger than 8 bytes. Since too large ones will lead to the waste of space.  **8, 16, 32** are chosen as the candidate values, for they are all **powers of 2**.

* N_CHUNK

  * In the memory-pool schema, every time some space is allocated, not only the space per ser but also a chunk of space is allocated. N_CHUNK gives a regular amount of this chunk.
  * Given that N_CHUNK in GNU 2.9 is set to be 20, we are trying to find a better N_CHUNK to cater to our application scene. Hence, numbers are 20 are chosen. They are **2, 4, 8, 16, 20, 32**.

  

After all parameters are decided, the allocator is tested with std::allocator . Tuple size is changed to observe the performance of them dealing with different scale of allocations.



## Analysis

* Result

  * MAX_SIZE=1024
  * ALIGN=16
  * N_CHUNK=16

  