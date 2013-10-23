//==========================================================================
/**
* @f	  : Mem.h
* @author : 
* created : 2008-4-11   10:25
* purpose : 简单内存管理
*/
//==========================================================================

#ifndef RKT_MEM_H
#define RKT_MEM_H

#include "Common.h"


#ifdef MEMORY_DEBUG
#	define FILE_LINE_ARGS	,const char* f,int l
#	define FILE_LINE_PARAM	,f,l
#else
#	define FILE_LINE_ARGS
#	define FILE_LINE_PARAM
#endif

/// 内存调试标志（只有在内存调试模式下有效，即定义了 MEMORY_DEBUG）
enum MemoryDebugFlag
{
	MDF_MEMORY_LEAK			= 0x1,		/// 支持内存泄露检查（系统默认在分配器析构前检查）
	MDF_MEMORY_STAT			= 0x2,		/// 支持内存统计（系统默认在分配器析构前检查，用户可以随时检查统计数据）
	MDF_MEMORY_CRC_VERIFY	= 0x4,		/// 是否强制对内存进行CRC校验
	MDF_REPORT_TO_FILE		= 0x8,		/// 是否将报告内容写入文件
};

#ifdef __cplusplus

#ifdef USE_SYSTEM_MEMORY

#include <malloc.h>
#include <crtdbg.h>

//namespace xs {

#ifdef MEMORY_DEBUG

	inline void* mem_alloc(size_t s, const char* f, int l)
	{
#ifdef RKT_WIN32
		return _malloc_dbg(s, _NORMAL_BLOCK, f, l);
#else
		return malloc(s);
#endif
	}
	inline void* mem_realloc(void* p, size_t s, const char* f, int l)
	{
#ifdef RKT_WIN32
		return _realloc_dbg(p, s, _NORMAL_BLOCK, f, l);
#else
		return realloc(p, s);
#endif
	}
	inline void  mem_free(void* p, const char* f, int l)	{ return free(p); }

#else // !MEMORY_DEBUG

	inline void* mem_alloc(size_t s)						{ return malloc(s); }
	inline void* mem_realloc(void* p, size_t s)				{ return realloc(p, s); }
	inline void  mem_free(void* p)							{ return free(p); }

#endif // !MEMORY_DEBUG

	inline bool checkMemory(bool stopOnError)
	{
	#ifdef RKT_WIN32
		if (!_CrtCheckMemory())
		{
			if (stopOnError)
				_ASSERT(0);
			return false;
		}
	#endif
		return true;
	}
	inline bool checkMemoryBySize(bool stopOnError, size_t blockSize)	{ return true; }
	inline void alwaysCheckMemory(bool alwaysCheck)
	{
	#ifdef RKT_WIN32
		int previous = _CrtSetDbgFlag(0);
		_CrtSetDbgFlag(alwaysCheck ? (previous | _CRTDBG_CHECK_ALWAYS_DF) : (previous & ~_CRTDBG_CHECK_ALWAYS_DF));
	#endif
	}
	inline bool isAlwaysCheckMemory()
	{
	#ifdef RKT_WIN32
		int previous = _CrtSetDbgFlag(0);
		_CrtSetDbgFlag(previous);
		return (previous & _CRTDBG_CHECK_ALWAYS_DF) != 0;
	#else
		return false;
	#endif
	}

	inline ulong setMemoryDebugFlag(ulong flag)	{ return 0; }


	inline size_t getUsedMemory()	{ return 0; }
	inline size_t getFreeMemory()	{ return 0; }

	inline void reportMemoryLeak()
	{
	#ifdef RKT_WIN32
		_CrtDumpMemoryLeaks();
	#endif
	}
	inline void reportMemoryStatistics()			{ }
	inline size_t getMemorySize(void* p)			{ return 0; }
	inline bool isValidPointer(void* p)				{ return p != 0; }
	inline void setAllocBreakPoint(void* addr)		{ }

//} // namespace

#else // !USE_SYSTEM_MEMORY

//namespace xs {

	//////////////////////////////////////////////////////////////////////////
	// 分配，释放相关
	RKT_API void* mem_alloc(size_t s FILE_LINE_ARGS);
	RKT_API void* mem_realloc(void* p, size_t s FILE_LINE_ARGS);
	RKT_API void  mem_free(void* p FILE_LINE_ARGS);


	//////////////////////////////////////////////////////////////////////////
	// 内存检查相关
	RKT_API bool checkMemory(bool stopOnError);
	RKT_API bool checkMemoryBySize(bool stopOnError, size_t blockSize);
	RKT_API void alwaysCheckMemory(bool alwaysCheck); // 当 MEMORY_DEBUG 定义后有效
	RKT_API bool isAlwaysCheckMemory(); // 当 MEMORY_DEBUG 定义后有效


	/// 设置内存调试标志（参考上面的枚举定义 MemoryDebugFlag）
	RKT_API ulong setMemoryDebugFlag(ulong flag);


	//////////////////////////////////////////////////////////////////////////
	// 内存统计、泄漏预报相关
	/// 获取系统中正在使用的内存数
	RKT_API size_t getUsedMemory();

	/// 获取系统中空闲的内存数
	RKT_API size_t getFreeMemory();

	/// 报告内存泄露
	RKT_API void reportMemoryLeak();

	/// 报告内存统计
	RKT_API void reportMemoryStatistics();


	//////////////////////////////////////////////////////////////////////////
	// 其他辅助函数
	RKT_API size_t getMemorySize(void* p);
	RKT_API bool isValidPointer(void* p);
	RKT_API void setAllocBreakPoint(void* addr);
//} // namespace

#endif // !USE_SYSTEM_MEMORY

#endif // __cplusplus

//////////////////////////////////////////////////////////////////////////
// 下面为重载 new, delete 操作符
#ifdef USE_SYSTEM_MEMORY

#define RKT_NEW	new

#else // !USE_SYSTEM_MEMORY

	#ifdef MEMORY_DEBUG
		// 这些文件先包含，防止new定义出错
		#include <new>

#if (TARGET_PLATFORM == PLATFORM_WIN32)
		#include <xmemory>
		#include <xtree>
#else
        #include <memory>

#endif
// 		inline void* _cdecl operator new(size_t s, const char* f, int l)		{ return mem_alloc(s, f, l); }
// 		inline void* _cdecl operator new[](size_t s, const char* f, int l)		{ return mem_alloc(s, f, l); }
// 		inline void  _cdecl operator delete(void* p, const char* f, int l)		{ mem_free(p, f, l); }
// 		inline void  _cdecl operator delete[](void* p, const char* f, int l)	{ mem_free(p, f, l); }
// 		inline void* _cdecl operator new(size_t s)								{ return mem_alloc(s, __FILE__, __LINE__); }
// 		inline void* _cdecl operator new[](size_t s)							{ return mem_alloc(s, __FILE__, __LINE__); }
// 		inline void  _cdecl operator delete(void* p)							{ mem_free(p, __FILE__, __LINE__); }
// 		inline void  _cdecl operator delete[](void* p)							{ mem_free(p, __FILE__, __LINE__); }

		#undef new
		#undef RKT_NEW
		//#define RKT_NEW				new(__FILE__, __LINE__)
		#define RKT_NEW				new
		#define malloc(s)			mem_alloc(s, __FILE__, __LINE__)
		#define calloc(n, s)		mem_alloc(s*n, __FILE__, __LINE__)
		#define realloc(p, s)		mem_realloc(p, s, __FILE__, __LINE__)
		#define free(p)				mem_free(p, __FILE__, __LINE__)

	#else // !MEMORY_DEBUG

#if (TARGET_PLATFORM == PLATFORM_WIN32)
inline void* _cdecl operator new(size_t s)								{ return mem_alloc(s); }
inline void* _cdecl operator new[](size_t s)							{ return mem_alloc(s); }
inline void  _cdecl operator delete(void* p)							{ mem_free(p); }
inline void  _cdecl operator delete[](void* p)							{ mem_free(p); }
#else
inline void*  operator new(size_t s)								{ return mem_alloc(s); }
inline void*  operator new[](size_t s)							{ return mem_alloc(s); }
inline void   operator delete(void* p)							{ mem_free(p); }
inline void   operator delete[](void* p)							{ mem_free(p); }

#endif



		#define malloc(s)			xs::mem_alloc(s)
		#define calloc(n, s)		xs::mem_alloc(s*n)
		#define realloc(p, s)		xs::mem_realloc(p, s)
		#define free(p)				xs::mem_free(p)

	#endif // !MEMORY_DEBUG

#endif // !USE_SYSTEM_MEMORY

#endif // RKT_MEM_H