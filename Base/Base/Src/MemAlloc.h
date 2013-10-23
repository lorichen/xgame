//==========================================================================
/**
* @file	  : MemAlloc.h
* @author : 
* created : 2008-4-11   15:40
* purpose : 
*/
//==========================================================================

#ifndef RKT_MEMALLOC_H
#define RKT_MEMALLOC_H

#include "Common.h"
#include "Singleton.h"
#include "Thread.h"
#include "ostrbuf.h"
#include <stdio.h>


#ifdef RKT_COMPILER_MSVC
#	pragma warning(push)
#	pragma warning(disable:4200)
#endif

#define MEMORY_THREAD

#ifdef MEMORY_THREAD
#	ifdef RKT_DEBUG
#		define MEM_LOCK		MultiThread::scopelock lock(mMutex FILE_LINE_PARAM)
#	else
#		define MEM_LOCK		MultiThread::scopelock lock(mMutex)
#	endif
#else
#	define MEM_LOCK			((void)0)
#endif

enum
{
	SB_SHIFT = 2,								/// 对齐时的右移位数（决定了尺寸对齐的大小）
	SMALL_MEMORY_SIZE =	256,					/// 小内存大小限制
	MEM_ALIGN = (1<<SB_SHIFT),					/// 内存对齐大小
	POOL_SIZE = (SMALL_MEMORY_SIZE>>SB_SHIFT),	/// 小内存池的桶数
	NODE_GROW = 32,								/// 小内存每次分配不足时，一次性递增的节点数
};

#define round(s)					( ((s) + MEM_ALIGN - 1) & ~(MEM_ALIGN - 1) )
#define getSmallBlockIndex(s)		( (round((s)) - 1) >> SB_SHIFT )
#define getSM_NodeSize(s)			( (s) + sizeof(SM_Node) )
#define getLM_NodeSize(s)			( (s) + sizeof(LM_Node) )
#define ptr2DataNode(p)				( (DataNode*) ( (uchar*)(p) - sizeof(DataNode) ) )
#define ptr2LM_Node(p)				( (LM_Node*) ( (uchar*)(p) - sizeof(LM_Node) ) )
#define ptr2SM_Node(p)				( (SM_Node*) ( (uchar*)(p) - sizeof(SM_Node) ) )
#define getPtrSize(p)				( ptr2DataNode(p)->size )

namespace xs {

	/// 数据节点，不带调试信息 4 bytes
	struct DataNode
	{
		size_t	size;		/// 用户请求分配的内存大小(即报告给用户的大小)
		char	ptr[];		/// 用户内存块指针
	};


	/// 小内存节点 24 bytes
	struct SM_Node
	{
		//crr add SM_Node * pNext;2010-06-30 
		//这里必须有一个指针，用来当truct FreeNode里面的next指针占位!!!
		SM_Node * pNext;
#ifdef MEMORY_DEBUG // 20
		ulong		mask;
		const char* file;	/// 文件
		int			line;	/// 行号
		ulong		crc;	/// crc值，用于内存校验
		size_t		state;	/// 0-未使用 1-使用中 2-空闲中
#endif
		DataNode	data;
	};


	/// 大内存节点 28 bytes
	struct LM_Node
	{
#ifdef MEMORY_DEBUG // 24
		ulong		mask;
		const char* file;	/// 文件
		int			line;	/// 行号
		ulong		crc;	/// crc值，用于内存校验
		LM_Node*	prev;	/// 前一个节点
		LM_Node*	next;	/// 后一个节点
#endif
		DataNode	data;
	};


	/// 小块 8 bytes
	struct SmallBlock
	{
		SmallBlock* prev;	/// 前一个小块
		ulong		blocks;	/// 块数

		void* data()		{ return this + 1; }
	};


	/// 空闲节点(不占用内存空间)
	struct FreeNode
	{
		FreeNode* next;	/// 下一个空闲节点
	};


	/// 内存分配器
	class MemAllocator
	{
#ifdef MEMORY_THREAD
		MultiThread::mutex	mMutex;
#endif
		SmallBlock*	mSBHead[POOL_SIZE];
		FreeNode*	mFreeNode[POOL_SIZE];

#ifdef MEMORY_DEBUG
		size_t		mTotalAllocSize;		/// 所有通过该分配器分配的内存大小，含调试信息
		size_t		mTotalLargeMemorySize;	/// 大内存实际尺寸（不含调试信息所在尺寸）
		size_t		mTotalSmallMemorySize;	/// 小内存实际尺寸（不含调试信息所在尺寸）
		ulong		mSmallMemoryRequestCount;	/// 小内存请求次数
		size_t		mSmallMemoryRequestSize;	/// 小内存请求次数对应的尺寸

		// 小内存统计
		ulong		mSmallMemoryNodeCountPeak[POOL_SIZE];		/// 小内存节点使用峰值
		ulong		mSmallMemoryNodeCountRequest[POOL_SIZE];	/// 小内存节点请求数
		ulong		mSmallMemoryNodeCountRelease[POOL_SIZE];	/// 小内存节点释放数

		// 大内存统计
		ulong		mLargeMemoryNodeCountPeak;		/// 大内存节点数峰值
		ulong		mLargeMemoryNodeCountRequest;	/// 大内存节点请求数
		ulong		mLargeMemoryNodeCountRelease;	/// 大内存节点释放数
		size_t		mLargeMemoryNodeSizePeak;		/// 大内存节点尺寸峰值
		size_t		mLargeMemoryNodeSizeRequest;	/// 大内存节点请求尺寸
		size_t		mLargeMemoryNodeSizeRelease;	/// 大内存节点释放尺寸
		size_t		mMaxMemoryAllocSize;			/// 最大的一次内存分配尺寸

		ulong		mMemoryDebugFlag;				/// 内存调试标志
		FILE*		mFileHandle;					/// 写入文件用的文件指针

		LM_Node		mLMHead;
#endif

	public:
		MemAllocator();
		~MemAllocator();

		static MemAllocator& getInstance()			{ static MemAllocator ma; return ma; }

		void* alloc(size_t s FILE_LINE_ARGS);
		void dealloc(void* p FILE_LINE_ARGS);
		void* _realloc(void* p, size_t s FILE_LINE_ARGS);

	private:
		LM_Node* requestLargeMemory(size_t s FILE_LINE_ARGS);
		void releaseLargeMemory(void* p FILE_LINE_ARGS);

		SM_Node* requestSmallMemory(size_t s FILE_LINE_ARGS);
		void releaseSmallMemory(void* p FILE_LINE_ARGS);

		SmallBlock* requestSmallBlock(SmallBlock*& head, size_t s FILE_LINE_ARGS);
		void releaseSmallBlock(SmallBlock* head FILE_LINE_ARGS);
		
		inline void* sysAlloc(size_t s);
		inline void sysFree(void* p);

		void log(ostrbuf& osb);
	public:
		ulong setMemoryDebugFlag(ulong flag);
		void reportMemoryLeak();
		void reportMemoryStatistics();

	};


} // namespace

#ifdef RKT_COMPILER_MSVC
#	pragma warning(pop)
#endif

#endif // RKT_MEMALLOC_H