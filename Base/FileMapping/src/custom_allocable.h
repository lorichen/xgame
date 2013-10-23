// ===========================================================================
//	filename: 	custom_allocable.h
//
//	author:		CHENTAO
//
//	created:	2008/01/11
//	
//	purpose:	自定义分配对象
//
// ===========================================================================

#ifndef  __custom_allocable_h__
#define  __custom_allocable_h__

#include <new>
#include <string>
#include "alloc_debuger.h"

// ---------------------------------------------------------------------------
// Name	: 自定义分配对象
// Desc : 这个模版实现了一种类，它的内存分配是由用户自定义的
// param: 
//        ALLOC         : 内存分配器
//        ALLOC_DEBUGER : 内存分配诊断类 
// ---------------------------------------------------------------------------
template
<
    typename T,
	typename ALLOC = std::allocator<T>,
	typename ALLOC_DEBUGER = no_alloc_debuger
>
class custom_allocable
{
public:
	//static void * operator new(size_t size)
	//{
	//	ALLOC alloc;
	//	return alloc.allocate(size,0);
	//}

	static void operator delete(void * p)
	{
		ALLOC alloc;
		alloc.deallocate(p,0);

#		ifdef DEBUG_NEW
			ALLOC_DEBUGER alloc_debuger;
			alloc_debuger.unrecord(p);
#		endif
	}

	//static void* operator new [](size_t size)
	//{
	//	ALLOC alloc;
	//	return alloc.allocate(size,0);
	//}

	static void operator delete[](void *p,size_t size)
	{
		ALLOC alloc;
		alloc.deallocate(p,size);

#		ifdef DEBUG_NEW
			ALLOC_DEBUGER alloc_debuger;
			alloc_debuger.unrecord(p);
#		endif
	}

	// 用到了MFC则重载一下Debug new
#	ifdef DEBUG_NEW
	static void * operator new(size_t n, const char * file, int line)
	{	
		void * p = operator new(n);

		ALLOC_DEBUGER alloc_debuger;
		alloc_debuger.record(p,n,file);

		return p;
	}
	
	static void * operator new[](size_t n, const char * file, int line)
	{	
		void * p = operator new[](n);

		ALLOC_DEBUGER alloc_debuger;
		alloc_debuger.record(p,n,file);

		return p;
	}

#	endif
	//static void * operator new(size_t size,const std::string name)
	//{
	//	ALLOC alloc;
	//	return alloc.allocate(size,&name);
	//}

};

#endif// __custom_allocable_h__
