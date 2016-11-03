# SSE Set bit count with sideways addition
Set bit count of a vector of length N filled with random integers using the SSE implementation of the sideway addition written in C++

Requires `g++-5` and `c++11`.
Compile with:
```
g++-5 -std=c++11 -march=native -O3 -o sse_bitcount sse_bitcount.cpp
```
Execute with:
```
./sse_bitcount [vector_size]
```
