#pragma once

#include <cstdlib>
#include <random>

// Define the ASSERT and DEBUG macros
#if defined(_WIN32)
#define PLATFORM_PC
#if !defined(ASSERT)
#define ASSERT _ASSERT
#endif
#if !defined(ASSERT)
#define assert _ASSERT
#endif
#ifdef _DEBUG 
#define DEBUG
#endif
#define UNUSED(x) (void)(x)
#endif

#ifdef DEBUG
#define LOGGING 1
#else
#define LOGGING 0
#endif

namespace Raymond
{
	typedef unsigned int uint;
	typedef unsigned long ulong;
	typedef unsigned short ushort;
	typedef unsigned char uchar;
	typedef unsigned long long ullong;

	inline auto RandomlySeededMersenneTwister()
	{
		// Magic number 624: The number of unsigned ints the MT uses as state
		std::vector<unsigned int> random_data(624);
		std::random_device source;
		std::generate(begin(random_data), end(random_data), [&]() {return source(); });
		std::seed_seq seeds(begin(random_data), end(random_data));
		std::mt19937 seededEngine(seeds);
		return seededEngine;
	}

	/// Returns a random float between from and to
	inline float RandInRange(float from, float to)
	{		
		static thread_local std::mt19937 generator = RandomlySeededMersenneTwister();
		std::uniform_real_distribution<float> distribution(from, to);
		return distribution(generator);
	}
	
	/// Modulo that works with negative number as well
	template <class T>
	inline T Modulo(T x, T m) { return (x % m + m) % m; }


#if LOGGING

#define LOG(macro_format, ...)										\
{																	\
	printf("[Raymond] ");											\
	printf(macro_format, ##__VA_ARGS__);							\
    const size_t macro_len = strlen(macro_format);					\
    if(macro_len > 0 && macro_format[macro_len - 1] != '\n')		\
	{																\
        printf("\n");												\
    }																\
}

#else
#define LOG(...) { }
#endif

}
