# Cache-Simulator
Developed a cache simulator with LRU and random replacement policy that takes SPEC2007 memory trace as an input, simulates the hit/miss behavior of cache memory and outputs the total number of hits, misses and evictions

## Compilation
```
g++ cache.cc -o cache
```

## Steps to run
Input
There should be four command line parameters in this order:
1. nk: the capacity of the cache in kilobytes (an int)
2. assoc: the associativity of the cache (an int)
3. blocksize: the size of a single cache block in bytes (an int)
4. repl: the replacement policy (a char); 'l' means LRU, 'r' means random.

Read traces from the standard input. Each line on the standard input will be a lowercase 'r' (for read) or 'w' (for write) followed by a space and then a 64-bit hexadecimal number giving the address of the memory access. For example, a snippet of a trace file looks like this:
r 56ecd8
r 47f639
r 7ff0001ff
w 47f63e
r 4817ef
r 7d5ab8

Output
The output should be a single line of six numbers separated by spaces. These six numbers are:
The total number of misses,
The percentage of misses (i.e. total misses divided by total accesses),
The total number of read misses,
The percentage of read misses (i.e. total read misses divided by total read accesses),
The total number of write misses,
The percentage of write misses (i.e. total write misses divided by total write accesses).


```
./cache 2048 64 64 l
55752 5.575200% 55703 5.610155% 49 0.689752%
```
That is, when the program is run specifying a 2048 kilobyte cache with 64-way associativity and 64 bytes per block with LRU replacement, and we get a 5.5752% total miss rate.


