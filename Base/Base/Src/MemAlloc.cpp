//==========================================================================
/**
* @file	  : MemAlloc.cpp
* @author : 
* created : 2008-4-11   15:40
*/
//==========================================================================

#include "stdafx.h"
#include "MemAlloc.h"


namespace xs {

	MemAllocator::MemAllocator()
	{
		memset(mSBHead, 0, sizeof(mSBHead));
		memset(mFreeNode, 0, sizeof(mFreeNode));//shit !!
#ifdef MEMORY_DEBUG
		mLMHead.prev = mLMHead.next = &mLMHead;

		mTotalAllocSize = mTotalLargeMemorySize = mTotalSmallMemorySize = 0;
		mSmallMemoryRequestCount = 0;
		mSmallMemoryRequestSize = 0;

		memset(mSmallMemoryNodeCountPeak, 0, sizeof(mSmallMemoryNodeCountPeak));
		memset(mSmallMemoryNodeCountRequest, 0, sizeof(mSmallMemoryNodeCountRequest));
		memset(mSmallMemoryNodeCountRelease, 0, sizeof(mSmallMemoryNodeCountRelease));

		mLargeMemoryNodeCountPeak = mLargeMemoryNodeCountRequest = mLargeMemoryNodeCountRelease = 0;
		mLargeMemoryNodeSizePeak = mLargeMemoryNodeSizeRequest = mLargeMemoryNodeSizeRelease = 0;
		mMaxMemoryAllocSize = 0;

		mMemoryDebugFlag = 0;//MDF_MEMORY_STAT | MDF_MEMORY_LEAK;
		mFileHandle = 0;
#endif
	}

	MemAllocator::~MemAllocator()
	{
#ifdef MEMORY_DEBUG
		if (mMemoryDebugFlag & MDF_MEMORY_STAT)
			reportMemoryStatistics();
		if (mMemoryDebugFlag & MDF_MEMORY_LEAK)
			reportMemoryLeak();
#endif
	}

	void* MemAllocator::alloc(size_t s FILE_LINE_ARGS)
	{
		MEM_LOCK;
		// 分配0字节?
		if (s == 0)
		{
			//debugBreak;
			s = 1; // C++标准:一个0字节的分配应该返回一个非空指针
		}

		if (s <= SMALL_MEMORY_SIZE) // 小内存
		{
			SM_Node* node = requestSmallMemory(s FILE_LINE_PARAM);
			if (!node) return 0;

			return node->data.ptr;
		}
		else
		{
			LM_Node* node = requestLargeMemory(s FILE_LINE_PARAM);
			if (!node)
			{
				debugBreak;
				return 0;
			}

			return node->data.ptr;
		}
		return 0;
	}

	void MemAllocator::dealloc(void* p FILE_LINE_ARGS)
	{
		MEM_LOCK;
		if (p)
		{
			if (getPtrSize(p) <= SMALL_MEMORY_SIZE)
				releaseSmallMemory(p FILE_LINE_PARAM);
			else
				releaseLargeMemory(p FILE_LINE_PARAM);
		}
	}

	void* MemAllocator::_realloc(void* p, size_t s FILE_LINE_ARGS)
	{
		void* new_ptr = alloc(s FILE_LINE_PARAM);

		if (new_ptr && p)
		{
			DataNode* new_node = ptr2DataNode(new_ptr);
			DataNode* old_node = ptr2DataNode(p);

			if (old_node && old_node->size > 0)
				memcpy(new_node->ptr, old_node->ptr, std::min(old_node->size,new_node->size));//old_node->size > new_node->size会越界写
		}

		dealloc(p FILE_LINE_PARAM);

		return new_ptr;
	}

	LM_Node* MemAllocator::requestLargeMemory(size_t s FILE_LINE_ARGS)
	{
		size_t new_size = getLM_NodeSize(s);

		LM_Node* node = (LM_Node*)sysAlloc(new_size);
		if (!node) return 0;

		node->data.size = s;
#ifdef MEMORY_DEBUG
		node->mask = '>>>>';
		node->file = f;
		node->line = l;
		node->crc = 0;
		node->prev = mLMHead.prev;
		node->next = &mLMHead;
		mLMHead.prev = node;
		node->prev->next = node;

		mTotalLargeMemorySize += s;

		mLargeMemoryNodeCountRequest++;
		if (mLargeMemoryNodeCountRequest - mLargeMemoryNodeCountRelease > mLargeMemoryNodeCountPeak)
			mLargeMemoryNodeCountPeak = mLargeMemoryNodeCountRequest - mLargeMemoryNodeCountRelease;

		mLargeMemoryNodeSizeRequest += s;
		if (mLargeMemoryNodeSizeRequest - mLargeMemoryNodeSizeRelease > mLargeMemoryNodeSizePeak)
			mLargeMemoryNodeSizePeak = mLargeMemoryNodeSizeRequest - mLargeMemoryNodeSizeRelease;

		if (s > mMaxMemoryAllocSize)
			mMaxMemoryAllocSize = s;
#endif
		return node;
	}

	void MemAllocator::releaseLargeMemory(void* p FILE_LINE_ARGS)
	{
		LM_Node* node = ptr2LM_Node(p);
#ifdef MEMORY_DEBUG
		node->prev->next = node->next;
		node->next->prev = node->prev;
		mLargeMemoryNodeCountRelease++;
		mLargeMemoryNodeSizeRelease += node->data.size;
#endif
		sysFree(node);
	}

	SM_Node* MemAllocator::requestSmallMemory(size_t s FILE_LINE_ARGS)
	{
		int i = getSmallBlockIndex(s);

		if (!mFreeNode[i])
		{
			ulong blockSize = getSM_NodeSize(round(s));	// 每个块的大小
			ulong memSize = NODE_GROW * blockSize;		// 整个块的大小
			SmallBlock* newBlock = requestSmallBlock(mSBHead[i], memSize FILE_LINE_PARAM); // 分配新的数据链
			FreeNode* node = (FreeNode*)newBlock->data(); // 定位到数据区
			(uchar*&)node += memSize - blockSize; // 定位到最后一个节点

			// 各节点建立链接关系
			for (int j=NODE_GROW-1; j>=0; j--, (uchar*&)node-=blockSize)
			{
				node->next = mFreeNode[i];
				mFreeNode[i] = node;
#ifdef MEMORY_DEBUG
				((SM_Node*)node)->state = 0;
#endif
			}
		}

		Assert(mFreeNode[i]);

		SM_Node* node = (SM_Node*)mFreeNode[i];
		mFreeNode[i] = mFreeNode[i]->next;

		node->data.size = s;
#ifdef MEMORY_DEBUG
		node->mask = '<<<<';
		node->file = f;
		node->line = l;
		node->crc = 0;
		node->state = 1;

		mTotalSmallMemorySize += s;
		mSmallMemoryRequestCount++;
		mSmallMemoryRequestSize += s;

		mSmallMemoryNodeCountRequest[i]++;
		if (mSmallMemoryNodeCountRequest[i] - mSmallMemoryNodeCountRelease[i] > mSmallMemoryNodeCountPeak[i])
			mSmallMemoryNodeCountPeak[i] = mSmallMemoryNodeCountRequest[i] - mSmallMemoryNodeCountRelease[i];
#endif
		return node;
	}

	void MemAllocator::releaseSmallMemory(void* p FILE_LINE_ARGS)
	{
		SM_Node* node = ptr2SM_Node(p);
		int i = getSmallBlockIndex(node->data.size);

#ifdef MEMORY_DEBUG
		node->state = 2;
		mTotalSmallMemorySize -= getPtrSize(p);
		mSmallMemoryNodeCountRelease[i]++;
#endif

		FreeNode* freeNode = (FreeNode*)node;
		freeNode->next = mFreeNode[i];
		mFreeNode[i] = freeNode;
	}

	SmallBlock* MemAllocator::requestSmallBlock(SmallBlock*& head, size_t s FILE_LINE_ARGS)
	{
		SmallBlock* p = (SmallBlock*)sysAlloc(sizeof(SmallBlock) + s);
		p->prev = head;
		return head = p;
	}

	void MemAllocator::releaseSmallBlock(SmallBlock* head FILE_LINE_ARGS)
	{
		SmallBlock* p = head;
		while (p)
		{
			uchar* bytes = (uchar*)p;
			SmallBlock* prev = p->prev;
			dealloc(bytes FILE_LINE_PARAM);
			p = prev;
		}
	}
	
	ulong MemAllocator::setMemoryDebugFlag(ulong flag)
	{
		ulong old = mMemoryDebugFlag;
		mMemoryDebugFlag = flag;
		return old;
	}

	void* MemAllocator::sysAlloc(size_t s)
	{
		s = round(s);
#ifdef MEMORY_DEBUG
		mTotalAllocSize += s;
#endif
#undef malloc
		return malloc(s);
	}

	void MemAllocator::sysFree(void* p)
	{
#undef free
		free(p);
	}

} // namespace