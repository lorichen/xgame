//==========================================================================
/**
* @f	  : Mem.cpp
* @author : 
* created : 2008-4-11   10:25
*/
//==========================================================================

#include "stdafx.h"
#include "Mem.h"
#include "MemAlloc.h"

//#define USE_MEM_ALLOCATOR


#ifndef USE_SYSTEM_MEMORY


	RKT_API void* mem_alloc(size_t s FILE_LINE_ARGS)
	{
#ifdef USE_MEM_ALLOCATOR
		return xs::MemAllocator::getInstance().alloc(s FILE_LINE_PARAM);
#else
#undef malloc
		return malloc(s);
#endif
	}


	RKT_API void* mem_realloc(void* p, size_t s FILE_LINE_ARGS)
	{
#ifdef USE_MEM_ALLOCATOR
		return xs::MemAllocator::getInstance()._realloc(p,s FILE_LINE_PARAM);
#else
#undef realloc
		return realloc(p,s);
#endif
	}


	RKT_API void mem_free(void* p FILE_LINE_ARGS)
	{
#ifdef USE_MEM_ALLOCATOR
		xs::MemAllocator::getInstance().dealloc(p FILE_LINE_PARAM);
#else
#undef free
		free(p);
#endif
	}

	RKT_API bool checkMemory(bool stopOnError)
	{
		return true;
	}

	RKT_API bool checkMemoryBySize(bool stopOnError, size_t blockSize)
	{
		return true;
	}
	
	RKT_API void alwaysCheckMemory(bool alwaysCheck) // 当 MEMORY_DEBUG 定义后有效
	{

	}

	RKT_API bool isAlwaysCheckMemory() // 当 MEMORY_DEBUG 定义后有效
	{
		return true;
	}

	RKT_API ulong setMemoryDebugFlag(ulong flag)
	{
#ifdef USE_MEM_ALLOCATOR
		return xs::MemAllocator::getInstance().setMemoryDebugFlag(flag);
#else
		return 0;
#endif
	}

	RKT_API size_t getUsedMemory()
	{
		return 0;
	}

	RKT_API size_t getFreeMemory()
	{
		return 0;
	}

	RKT_API void reportMemoryLeak()
	{
#ifdef USE_MEM_ALLOCATOR
		xs::MemAllocator::getInstance().reportMemoryLeak();
#endif
	}

	RKT_API void reportMemoryStatistics()
	{
#ifdef USE_MEM_ALLOCATOR
		xs::MemAllocator::getInstance().reportMemoryStatistics();
#endif
	}

	RKT_API size_t getMemorySize(void* p)
	{
		return 0;
	}

	RKT_API bool isValidPointer(void* p)
	{
		return true;
	}

	RKT_API void setAllocBreakPoint(void* addr)
	{

	}

#endif // USE_RKT_MEMORY
