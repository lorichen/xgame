#ifndef __Pch_H__
#define __Pch_H__

#define RE_PLATFORM_WIN32 1
#define RE_PLATFORM_LINUX 2
#define RE_PLATFORM_APPLE 3

#define RE_COMPILER_MSVC 1
#define RE_COMPILER_GNUC 2
#define RE_COMPILER_BORL 3

#define RE_ENDIAN_LITTLE 1
#define RE_ENDIAN_BIG 2

#define RE_ARCHITECTURE_32 1
#define RE_ARCHITECTURE_64 2

#if defined(_MSC_VER)
	#define RE_COMPILER RE_COMPILER_MSVC
	#define RE_COMP_VER _MSC_VER
#elif defined(__GNUC__)
	#define RE_COMPILER RE_COMPILER_GNUC
	#define RE_COMP_VER (((__GNUC__) * 100) + (__GNUC_MINOR__ * 10) + __GNUC_PATCHLEVEL__)
#elif defined(__BORLANDC__)
	#define RE_COMPILER RE_COMPILER_BORL
	#define RE_COMP_VER __BCPLUSPLUS__
#else
	#pragma error "Unknown Compiler!"
#endif

#if RE_COMPILER == RE_COMPILER_MSVC 
	#if RE_COMP_VER >= 1200
		#define FORCEINLINE __forceinline
	#endif
#else
	#define FORCEINLINE __inline
#endif

#if defined(__WIN32__) || defined(_WIN32)
	#define RE_PLATFORM RE_PLATFORM_WIN32
#elif defined(__APPLE_CC__)
	#define RE_PLATFORM RE_PLATFORM_APPLE
#else
	#define RE_PLATFORM RE_PLATFORM_LINUX
#endif

#if defined(__x86_64__)
	#define RE_ARCH_TYPE RE_ARCHITECTURE_64
#else
	#define RE_ARCH_TYPE RE_ARCHITECTURE_32
#endif

#define RE_ENDIAN	RE_ENDIAN_BIG

#include "ReMathInc.h"

// 只定义无符号的简单类型，便于书写
typedef unsigned char			uchar;
typedef unsigned short			ushort;
typedef unsigned int			uint;
typedef unsigned long			ulong;

#endif