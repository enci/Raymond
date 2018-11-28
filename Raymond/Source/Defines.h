#pragma once

#include <cstdlib>

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

	/// Returns a random float between zero and 1
	inline float RandFloat() { return static_cast<float>((rand()) / (RAND_MAX + 1.0)); }

	/// Returns a random float between x and y
	inline float RandInRange(float x, float y) { return x + RandFloat()*(y - x); }

	/// Returns a random int between from and to
	inline int RandInRange(int from, int to) { return from + rand() % (to - from); }

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
