# SSE Set bit count
Set bit count of a vector of length N filled with random integers using SSE implementation written in C++

Requires `g++-5` and `c++11`.
Compile with:
```
g++-5 -std=c++11 -march=native -O3 -o sse_bitcount sse_bitcount.cpp
```
Execute with:
```
./sse_bitcount [array_size]
```
