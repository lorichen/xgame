// ===========================================================================
//	filename: 	sharemem_allocator.cpp
//
//	author:		CHENTAO
//
//	created:	2008/01/12
//	
//	purpose:	
//
// ===========================================================================

#include "sharemem_allocator.h"

/// 加载共享内存
bool filemapping_allocator::create( const std::string & file_name,const std::string & share_name,size_t page_size,bool recover )
{
	_file_handle = ::CreateFile(file_name.c_str(),
								GENERIC_WRITE|GENERIC_READ,
								FILE_SHARE_READ,
								0,
								OPEN_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								0);
	
	if ( INVALID_HANDLE_VALUE==_file_handle )
		return false;
	
	_mapping_handle = ::CreateFileMapping(_file_handle,
								0,
								PAGE_READWRITE,
								0,
								page_size,
								share_name.c_str());
	
	if ( _mapping_handle==0 )
		return false;
	
	_mapping_addr = ::MapViewOfFile(_mapping_handle,
								FILE_MAP_ALL_ACCESS,
								0,
								0,
								page_size);
	
	if ( _mapping_addr==0 )
		return false;

	_mapping_size = page_size;

	if ( recover )
	{
		if ( !recover_alloc_info(_mapping_addr,_mapping_size) )
			return false;
	}else
	{
		set_mem_page(_mapping_addr,_mapping_size);
	}

	return true;
}

/// 释放共享内存
void filemapping_allocator::close()
{
	if ( _mapping_addr )
	{
		::UnmapViewOfFile(_mapping_addr);
		_mapping_addr = 0;
		_mapping_size = 0;
	}

	if ( _mapping_handle )
	{
		::CloseHandle(_mapping_handle);
		_mapping_handle = 0;
	}

	if ( _file_handle )
	{
		::CloseHandle(_file_handle);
		_file_handle = 0;
	}
}