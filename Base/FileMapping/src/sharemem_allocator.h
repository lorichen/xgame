// ===========================================================================
//	filename: 	sharemem_allocator.h
//
//	author:		CHENTAO
//
//	created:	2008/01/12
//	
//	purpose:	映射文件内存分配器
//
// ===========================================================================

#ifndef  __sharemem_allocator_h__
#define  __sharemem_allocator_h__

#include "custom_allocator.h"
#include "custom_allocable.h"
#include <windows.h>
#include "Singleton.h"
using namespace xs;

// ---------------------------------------------------------------------------
// Name	: 映射文件内存分配器
// Desc : 使用文件映射的方式共享内存，然后把内存分配到共享内存里
// ---------------------------------------------------------------------------
class filemapping_allocator : public custom_allocator,public Singleton<filemapping_allocator>
{
public:
	/**
	@purpose         : 创建映射文件，初始化共享内存
	@param	file_name: 映射文件在磁盘上的位置
	@param share_name: 多个进程共享内存时使用的名字
	@param page_size : 内存映射总共有多大
	@param recover   : 是否从内存中恢复曾经分配的对象 
	@return		     : 初始化成功返回true,否则返回false
	*/
	
	bool create( const std::string & file_name,const std::string & share_name,size_t page_size,bool recover );

	void close();

	filemapping_allocator() : _file_handle(0),_mapping_handle(0),_mapping_addr(0),_mapping_size(0)
	{
	}

	virtual ~filemapping_allocator() 
	{
		close();
	}

public:
	HANDLE         _file_handle;
	HANDLE         _mapping_handle;
	void   *       _mapping_addr;
	size_t         _mapping_size;
};


// ---------------------------------------------------------------------------
// Name	: 共享内存分配器
// Desc : 
//        1.遵循stl标准,可以传给标准容器,例如std::list<T,sharemem_allocator<T>)
//        2.如果需要接管一个类的内存分配,可以申明如下 class A : public custom_allocable<A,sharemem_allocator<A>>
// ---------------------------------------------------------------------------
template<typename T>
class sharemem_allocator : public std::allocator<T>
{
public:
	pointer allocate(size_type _N, const void * _Name)
	{
		// 符合c++标准
		if ( _N==0 )
			_N = 1;

		while(1)
		{
			pointer ret = (pointer)filemapping_allocator::get_instance()->allocate(_N,(const std::string *)_Name);
			if ( ret )
				return ret;

			new_handler global_handler = set_new_handler(0);
			set_new_handler(global_handler);
			if ( global_handler )
				global_handler();
			else
				throw(std::bad_alloc());
		}

		return 0;
	}
	
	void deallocate(void _FARQ *_P, size_type)
	{
		// 符合c++标准
		if ( _P==0 )
			return;

		if ( !filemapping_allocator::get_instance()->deallocate(_P) )
			throw(std::bad_alloc());
	}
};

//// 方便书写
//template<typename T>
//class share_alloc : public custom_allocable<T,sharemem_allocator<T> // ? vc6编译器一定要多一个> 才能识别，真让人哭笑不得
//{
//};


#endif// __sharemem_allocator_h__
