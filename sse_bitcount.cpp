#include <iostream>
#include <chrono>
#include <cstdlib>

#include "xmmintrin.h"
#include "smmintrin.h"
#include "emmintrin.h"

int main (int argc, char* argv[]) {

	if (argc != 2) {
		std::cout << " !! Error !! " << std::endl;
		std::cout << " !! Usage: !! " << std::endl;
		std::cout << "\t ./sse_bitcount <vector_size> " << std::endl;
		return 1;
	}

	long size = atol(argv[1]);
	std::cout << "Number of elements: " << size << std::endl;

	/* Create vector long <vector_size> */
	int* buf = (int*) _mm_malloc(size*sizeof(int), 16);
	if (!buf) {
		std::cout << " !! Error while allocating memeory" << std::endl;
		return 1;
	}

	std::cout << "Size in MB " << size*sizeof(int)/1000/1000 << std::endl;

	/* Fill the vector with random integers */
	for (long i=0; i<size; i++) {
		buf[i] = rand();
	}
	
	auto begin = std::chrono::high_resolution_clock::now();

	// Count set bits with sideways addition
	__m128i sse_bitcount = _mm_set1_epi32(0);
	
	float* p_sse = (float*) buf;
	for (long i=0; i<size; i+=4) {
		__m128i copy	= _mm_castps_si128(_mm_load_ps(p_sse));
		
		//x = (x & 10101010101010101010101010101010) + (x >> 1 & 01010101010101010101010101010101)
		copy = _mm_add_epi32(
					_mm_srl_epi32(
						_mm_castps_si128(_mm_and_ps(
							_mm_castsi128_ps(copy),
							_mm_castsi128_ps(_mm_set1_epi32(0xAAAAAAAA))
							)
						), 
						_mm_set_epi32(0,0,0,1)
					),
					_mm_castps_si128(_mm_and_ps(
						_mm_castsi128_ps(copy),
						_mm_castsi128_ps(_mm_set1_epi32(0x55555555))
					))
				);
		
		//x = (x & 11001100110011001100110011001100) + (x >> 2 & 00110011001100110011001100110011)
		copy = _mm_add_epi32(
					_mm_srl_epi32(
						_mm_castps_si128(_mm_and_ps(
							_mm_castsi128_ps(copy),
							_mm_castsi128_ps(_mm_set1_epi32(0xCCCCCCCC))
							)
						), 
						_mm_set_epi32(0,0,0,2)
					),
					_mm_castps_si128(_mm_and_ps(
						_mm_castsi128_ps(copy),
						_mm_castsi128_ps(_mm_set1_epi32(0x33333333))
					))
				);
			
		// x = (x & 11110000111100001111000011110000) + (x >> 4 & 00001111000011110000111100001111)
		copy = _mm_add_epi32(
					_mm_srl_epi32(
						_mm_castps_si128(_mm_and_ps(
							_mm_castsi128_ps(copy),
							_mm_castsi128_ps(_mm_set1_epi32(0xF0F0F0F0))
							)
						), 
						_mm_set_epi32(0,0,0,4)
					),
					_mm_castps_si128(_mm_and_ps(
						_mm_castsi128_ps(copy),
						_mm_castsi128_ps(_mm_set1_epi32(0x0F0F0F0F))
					))
				);
		
		//x = (x & 11111111000000001111111100000000) + (x >> 8 & 00000000111111110000000011111111)
		copy = _mm_add_epi32(
					_mm_srl_epi32(
						_mm_castps_si128(_mm_and_ps(
							_mm_castsi128_ps(copy),
							_mm_castsi128_ps(_mm_set1_epi32(0xFF00FF00))
							)
						), 
						_mm_set_epi32(0,0,0,8)
					),
					_mm_castps_si128(_mm_and_ps(
						_mm_castsi128_ps(copy),
						_mm_castsi128_ps(_mm_set1_epi32(0x00FF00FF))
					))
				);
		
		// x = (x & 11111111111111100000000000000000) + (x >> 16 & 00000000000000001111111111111111)
		copy = _mm_add_epi32(
					_mm_srl_epi32(
						_mm_castps_si128(_mm_and_ps(
							_mm_castsi128_ps(copy),
							_mm_castsi128_ps(_mm_set1_epi32(0xFFFF0000))
							)
						), 
						_mm_set_epi32(0,0,0,16)
					),
					_mm_castps_si128(_mm_and_ps(
						_mm_castsi128_ps(copy),
						_mm_castsi128_ps(_mm_set1_epi32(0x0000FFFF))
					))
				);

		sse_bitcount = _mm_add_epi32(sse_bitcount, copy);

		p_sse+=4; // Salto di 4 interi
	}

	// Store data
	int tmp[4];
	_mm_storeu_ps((float*) tmp, _mm_castsi128_ps(sse_bitcount));
	long count = tmp[0] + tmp[1] + tmp[2] + tmp[3];

	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin);

	std::cout << "Bit count: " << count << std::endl;
	std::cout << "Time     : " << elapsed.count() << "ms." << std::endl;
	std::cout << std::endl;

	_mm_free(buf);
	return 0;
} 
