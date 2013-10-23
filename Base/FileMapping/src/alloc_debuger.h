// ===========================================================================
//	filename: 	alloc_debuger.h
//
//	author:		CHENTAO
//
//	created:	2008/01/16
//	
//	purpose:	内存分配诊断类
//
// ===========================================================================

#ifndef __ALLOC_DEBUGER_H__
#define __ALLOC_DEBUGER_H__

#include <map>
#include <string>
#include <iostream>

// ---------------------------------------------------------------------------
// Name	: 不启动内存调试
// Desc :
// ---------------------------------------------------------------------------

class no_alloc_debuger
{
public:
	void record(void * p,size_t size,const char * debug_info)
	{
	}

	void unrecord(void *p)
	{
	};
};

// ---------------------------------------------------------------------------
// Name	: 打印内存泄露信息 
// Desc :
// ---------------------------------------------------------------------------

class memory_leak_detector
{
protected:
	struct mem_block
	{
		size_t        size;
		std::string   debug_info; 
	};

	typedef std::map<void *,mem_block> MMAP;
	MMAP  _mem_map;

public:
	void record(void * p,size_t size,const char * debug_info)
	{
		mem_block & mem = _mem_map[p];
		mem.size = size;
		mem.debug_info = debug_info;
	}

	void unrecord(void *p)
	{
		_mem_map.erase(p);
	};

	virtual ~memory_leak_detector()
	{
		MMAP::iterator it = _mem_map.begin();
		for ( ;it!=_mem_map.end();++it )
		{
			mem_block & mem = it->second;
			
			std::cerr << "memory leaked point=" << it->first << " size=" << mem.size << " info:" << mem.debug_info;
		}
	}
};

#endif//__ALLOC_DEBUGER_H__