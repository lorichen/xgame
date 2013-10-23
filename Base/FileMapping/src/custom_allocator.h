// ===========================================================================
//	filename: 	custom_allocator.h
//
//	author:		CHENTAO
//
//	created:	2008/01/12
//	
//	purpose:	自定义内存分配管理器
//
// ===========================================================================

#ifndef  __custom_allocator_h__
#define  __custom_allocator_h__

#include <map>
#include <set>
#include <string>

#define  ALIGN_SIZE          64             // 64字节对齐
#define  EMPTY_BLOCK         0x00           // 空块标志
#define  ALLOCATED_BLOCK     0xAD           // 空块标志

// ---------------------------------------------------------------------------
// Name	: 自定义内存分配管理器
// Desc :
// 功能 :
//        1.维护一大段内存区域，以后每次分配/释放内存从该区域分配
//        2.支持将这段大内存序列化，以后载入时可以还原上次分配过的对象
// ---------------------------------------------------------------------------

class custom_allocator
{
public:
	//////////////////////////////////////////////////////////////////////////
	struct MEM_BLOCK
	{
		unsigned int  _size;            // 内存块大小
		unsigned char _state;           // 内存块状态,是否已分配
		unsigned char _name_len;        // 内存标识,名字
		unsigned char _verify[2];       // 校验是否是一个合法的内存块结构

		void clear() 
		{
			_verify[0]='M';
			_verify[1]='B';
			_state = EMPTY_BLOCK;
			_name_len = 0;
		}
		bool is_valid() const { return _verify[0]=='M' && _verify[1]=='B'; } 
	};
	typedef  std::set<MEM_BLOCK *> FREE_LIST;
	typedef  std::map<std::string,void *> ALLOCATED_MAP;

	/**
	@purpose          : 设置内存页面，以后每次分配/释放内存从该页面分配
	@param  page      : 内存页面地址
	@param  page_size : 内存页面长度
	*/ 
	void set_mem_page(void * page,unsigned int page_size );

	/**
	@purpose          : 设置内存页面，并恢复页面上的内存分配情况
	@param  page      : 内存页面地址
	@param  page_size : 内存页面长度
	@param  recover_map:返回已分配的内存及其标识
	@return		      : 是否恢复成功
	*/
	bool recover_alloc_info(void * page,unsigned int page_size);

	/**
	@purpose          : 查找一块已分配的内存 (目前只能查找从内存页面中恢复的内存)
	@param	 mem_name : 内存标识，名称
	@return		      : 如果找到则返回改内存的地址，否则返回0
	*/
	void * find_mem_block(const std::string mem_name);

	/**
	@purpose          : 分配一块指定大小的内存
	@param	size      : 需要分配多大的内存
	@param mem_name   : 给分配的这段内存命一个名字，用于调试或恢复内存
	@return		      : 如果分配成功则返回分配的内存地址，否则返回null
	*/
	void * allocate(size_t size,const std::string * mem_name = 0);

	/**
	@purpose         : 释放一块已分配的内存
	@param	p	     : 要释放的内存地址
	@return		     : 如果是曾经分配过的内存，则释放成功，否则释放失败
	*/
	bool deallocate(void *p);

protected:
	FREE_LIST            _free_blocks;

public:
	ALLOCATED_MAP        _alloc_blocks;
};

#endif// __custom_allocator_h__
