/*******************************************************************
** 文件名:	FileMapping.h
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include <map>
#include <string>

// 导出宏定义
#ifndef FM_API
#	ifdef FM_EXPORTS
#		define FM_API		extern "C" __declspec(dllexport)
#	else
#		define FM_API		extern "C" __declspec(dllimport)
#	endif
#endif//NET_API

namespace xs
{
	/**
	@purpose         : 创建映射文件，初始化共享内存
	@param	file_name: 映射文件在磁盘上的位置
	@param share_name: 多个进程共享内存时使用的名字
	@param page_size : 内存映射总共有多大
	@param recover   : 是否从内存中恢复曾经分配的对象 
	*/
	FM_API bool InitializeFileMapping(const std::string & file_name,const std::string & share_name,size_t page_size,bool recover);

	/**
	@purpose         : 取得共享内存映射的地址
	@return          :
	*/
	FM_API void * GetFileMappingAddr();

	/**
	@purpose         : 在共享内存区块中分配一个可用内存
	@param   size    : 要分配的大小
	@param   name    : 给该块内存命令.默认为空,这个参数可以用来调试内存,也可以用来在当机恢复时找回需要的数据
	@return          : 返回内存地址
	*/
	FM_API void * ShareMemAlloc( int size,const char * name=0 );

	/**
	@purpose         : 释放先前从共享内存中分配的内存
	@param   p       : 分配地址
	*/
	FM_API void ShareMemDealloc( void * p );

	/**
	@purpose         : 查找一块已分配的内存
	@param   name    : 内存名称
	@return          : 返回内存地址
	*/
	FM_API void * FindRecoveredMemory(const char * name);

	/**
	@purpose         : 取得所有的已恢复的内存块
	*/
	FM_API bool GetAllRecoveredMemory(std::map<std::string,void *> & mem_list);
}