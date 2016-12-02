# Cache-Simulation

## What is this
- This lab implements a two-level (L1 and L2) cache simulator in C++. The cache simulator will take several parameters describing the cache (block size, associativity, etc) along with a memory access trace file for an input program. 
- In this design, we use:
	- **write-back** policy for **write hit**.
	- **write no-allocate ** policy for **write miss**.
	- **round-robin** policy for eviction(every set has a counter, when reach the way-size, reset to 0).
	- L1 and L2 are **non-inclusive** cache.

## Things included in this repository
- **cacheconfig.txt**: The parameters of the L1 and L2 caches are specified in a configuration file.
	- The first line of every cache is **Block size**, it specifies the block size for the cache in *bytes*, and block size should always be a non-negative power of 2.
	- The second line is **Associativity**, it specifies the associativity of the cache. "1" implies a direct-mapped cache, while "0" implies fully-associative. Should always be a non-negative power of 2.
	- The third line is **Cache size**, it means the total size of cache data array in **KB**.
- **trace.txt**: There are two elements in each line. Access type(Read or Write) and a 32-bits address(in unsigned hexadecimal format). Two elements are separated by a single space.
- **Output file**: there should be an output file after running this program successful. Each line has two numbers (from 0 to 4), represent the **access state** of cache L1 and L2.
	- 0: No Access; 1: Read Hit; 2: Read Miss; 3: Write Hit; 4: Write Miss.


## How to run
1. Make sure these three files under the same folder: **main.cpp**, **cacheconfig.txt** and **trace.txt**.
2. `g++ main.cpp -m32 -o cachesimulator` to compile.
3. `./cachesimulator cacheconfig.txt trace.txt` to run.
4. `cat trace.txt.out` to check the output file.
