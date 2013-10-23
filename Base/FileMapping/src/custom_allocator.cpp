// ===========================================================================
//	filename: 	custom_allocator.cpp
//
//	author:		CHENTAO
//
//	created:	2008/01/12
//	
//	purpose:	
//
// ===========================================================================

#include <assert.h>
#include "custom_allocator.h"

//////////////////////////////////////////////////////////////////////////
void custom_allocator::set_mem_page(void * page,unsigned int page_size )
{
	MEM_BLOCK * block = (MEM_BLOCK *)page;
	block->clear();
	block->_size = page_size;

	_free_blocks.insert(block);	
}

//////////////////////////////////////////////////////////////////////////
bool custom_allocator::recover_alloc_info(void * page,unsigned int page_size)
{
	MEM_BLOCK * block = (MEM_BLOCK *)page;
	char * end = (char *)page + page_size;

	while((char*)block<end)
	{
		if ( block->is_valid()==false )
			return false;

		switch ( block->_state )
		{
		case EMPTY_BLOCK:
			_free_blocks.insert(block);
			break;
		case ALLOCATED_BLOCK:
			{
				if ( block->_name_len>0 )
				{
					// 保存名字内存映射表
					std::string mem_name((char *)block + block->_size - block->_name_len,block->_name_len);
					mem_name += '\0';
					_alloc_blocks[mem_name] = block+1;
				}else
				{
					// 没有名字时用地址做名字
					char mem_name[256];
					sprintf(mem_name,"0x%x",block);
					_alloc_blocks[mem_name] = block+1;
				}
				break;
			}
		default:
			return false;
		}

		block = (MEM_BLOCK *)((char *)block + block->_size);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
void * custom_allocator::find_mem_block(const std::string mem_name)
{
	ALLOCATED_MAP::iterator it = _alloc_blocks.find(mem_name);
	if ( it==_alloc_blocks.end() )
		return 0;

	return it->second;
}

//////////////////////////////////////////////////////////////////////////
void * custom_allocator::allocate(size_t size,const std::string * mem_name)
{
	size_t name_len  = 0;
	if ( mem_name )
	{
		name_len = mem_name->length();
	}

	size += sizeof(MEM_BLOCK) + name_len;
	size = ((size+ALIGN_SIZE)/ALIGN_SIZE)*ALIGN_SIZE;

	FREE_LIST::iterator it = _free_blocks.begin();
	for ( ;it!=_free_blocks.end();++it )
	{
		MEM_BLOCK * block = *it;
		if ( block->_size>size )
		{
			block->_size = block->_size - size;
			assert(block->_size>sizeof(MEM_BLOCK));

			MEM_BLOCK * new_block = (MEM_BLOCK*)((char *)block+block->_size);
			new_block->clear();
			new_block->_name_len = name_len;
			new_block->_size = size;
			new_block->_state=ALLOCATED_BLOCK;

			if ( mem_name )
			{
				memcpy((char*)new_block+new_block->_size-name_len,mem_name->c_str(),name_len);
			}

			return new_block+1;
		}

		if ( block->_size==size )
		{
			block->_state = ALLOCATED_BLOCK;
			block->_name_len = name_len;

			if ( mem_name )
			{
				memcpy((char*)block+block->_size-name_len,mem_name->c_str(),name_len);
			}

			_free_blocks.erase(it);
			return block+1;
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
bool custom_allocator::deallocate(void *p)
{
	MEM_BLOCK * block = (MEM_BLOCK *)p - 1;
	
	// 不是一个合法的内存块
	if ( !block->is_valid())
		return false;

	block->clear();

	FREE_LIST::iterator pos = _free_blocks.lower_bound(block);
	
	// 看看能不能和后一项合并
	bool merged = false;
	if ( pos!=_free_blocks.end() )
	{
		MEM_BLOCK * b = *pos;
		if ( (char *)block+block->_size == (char *)b )
		{
			block->_size += b->_size;
			*pos = block;
			merged = true;
		}
	}
	
	// 看看能不能和前一项合并
	if ( pos!=_free_blocks.begin())
	{
		--pos;
		MEM_BLOCK * b = *pos;
		
		if ( (char *)b+b->_size == (char *)block )
		{
			b->_size += block->_size;
			if ( merged )
				_free_blocks.erase(++pos);
			merged = true;
		}
	}
	
	if ( merged==false )
	{
		_free_blocks.insert(block);
	}
	
	return true;
}
