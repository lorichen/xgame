//==========================================================================
/**
* @file	  : Handle.cpp
* @author : 
* created : 2008-1-12   11:10
*/
//==========================================================================

#include "stdafx.h"
#include "Handle.h"
#include "Api.h"
#include "Thread.h"
#include "Trace.h"

#define new RKT_NEW

#define GROW_COUNT		512							/// 现有句柄使用完毕后的增长增量

namespace xs {

	ulong				g_handlesAllocated = 0;		/// 已经分配的句柄数
	ulong				g_firstFree = 1;			/// 第一个空闲句柄索引,0索引的句柄为系统使用，用于标记句柄

//#ifdef RKT_HANDLE_THREAD_SAFE
	Mutex		g_handleMutex;						/// 句柄互斥体
#	define handleLock		ResGuard<>		handle_lock(g_handleMutex) /// 句柄锁
//#else
//#	define handleLock
//#endif

#ifdef RKT_DEBUG
	ulong				g_handleCount = 0;			/// 实际使用的句柄数
	ulong				g_maxCreateCount = 0;		/// 句柄的最大原地创建数
#endif

	/** 句柄表入口*/
	typedef struct tagHandleEntry
	{
		handle	h;				/// 句柄
		ulong	userData;		/// 用户数据
#ifdef RKT_DEBUG
		const char* file;		/// 文件名
		uint	line;			/// 行号
#endif
	} HandleEntry, *PHandleEntry;

	PHandleEntry		g_aHandleEntry = NULL;		/// 句柄表入口地址



	// handle的各部分信息描述
	//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
	//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
	//  +------------------------+--------------------------------------+
	//  |      Create Count      |                Index                 |
	//  +------------------------+--------------------------------------+

#define HANDLE_MASK_COUNT		0xFFF00000
#define HANDLE_SHIFT_COUNT		20
#define HANDLE_MASK_INDEX		0x000FFFFF
#define HANDLE_SHIFT_INDEX		0

#define countFromHandle(h)		(ulong)(((ulong)(h) & HANDLE_MASK_COUNT) >> HANDLE_SHIFT_COUNT)
#define indexFromHandle(h)		(ulong)(((ulong)(h) & HANDLE_MASK_INDEX))

#define handleFromCount(h)		(((ulong)(h)) << HANDLE_SHIFT_COUNT)
#define handleFromIndex(h)		(((ulong)(h)))


	bool _isValidHandle(handle h)
	{
		return (indexFromHandle(h) > 0 && indexFromHandle(h) < g_handlesAllocated &&
			g_aHandleEntry[indexFromHandle(h)].h == h);
	}


#ifdef RKT_DEBUG
	RKT_API handle createHandle(ulong userData, const char* file, uint line)
#else
	RKT_API handle createHandle(ulong userData)
#endif
	{
		handleLock;

		handle h;
		ulong i, nextFree;
		PHandleEntry phe;

		if (g_firstFree >= g_handlesAllocated)
		{
#ifdef RKT_DEBUG // check max index
			if (g_handlesAllocated + GROW_COUNT >= 1024*1024)
			{
				Error("createHandle, max index overflow!"<<endl);
				return INVALID_HANDLE;
			}
#endif
			if (g_handlesAllocated == 0)
			{
				g_aHandleEntry = (PHandleEntry) new uchar[sizeof(HandleEntry) * GROW_COUNT];
				if (g_aHandleEntry == NULL)
					return INVALID_HANDLE;
			}
			else
			{
				PHandleEntry newHandleEntry = (PHandleEntry) new uchar[sizeof(HandleEntry) * (g_handlesAllocated + GROW_COUNT)];
				if (newHandleEntry == NULL)
					return INVALID_HANDLE;
				memcpy(newHandleEntry, g_aHandleEntry, sizeof(HandleEntry) * g_handlesAllocated);
				delete[] g_aHandleEntry;
				g_aHandleEntry = newHandleEntry;
			}

			i = g_handlesAllocated;
			g_handlesAllocated += GROW_COUNT;
			phe = &g_aHandleEntry[i];
			while (i < g_handlesAllocated)
			{
				phe->h = 0;
				phe->userData = ++i; // 下一个空闲索引
				phe++;
			}
		}

		ulong nextCreateCount = countFromHandle(g_aHandleEntry[g_firstFree].h);
		nextCreateCount ++;
		h = g_aHandleEntry[g_firstFree].h = (handle) (handleFromCount(nextCreateCount) | handleFromIndex(g_firstFree));

		//#ifdef RKT_DEBUG // check create count max
		//	if (countFromHandle(h) <= nextCreateCount)
		//		Warning("createHandle(0x%08x), create counts overflow!", (ulong)h);
		//#endif

		nextFree = (ulong)g_aHandleEntry[g_firstFree].userData;
		g_aHandleEntry[g_firstFree].userData = userData;

#ifdef RKT_DEBUG
		g_aHandleEntry[g_firstFree].file = file;
		g_aHandleEntry[g_firstFree].line = line;
#endif

		g_firstFree = nextFree;

#ifdef RKT_DEBUG
		g_handleCount ++;
		if (nextCreateCount > g_maxCreateCount)
			g_maxCreateCount = nextCreateCount;
#endif

		return h;
	}


#ifdef RKT_DEBUG
	RKT_API bool closeHandle(handle h, const char* file, uint line)
#else
	RKT_API bool closeHandle(handle h)
#endif
	{
		handleLock;

		if (!_isValidHandle(h))
		{
#ifdef RKT_DEBUG
			Error(file<<"("<<line<<"): Error: closeHandle("<<(uint)h<<"), handle is invalid"<<endl);
#else
			Error("Error: closeHandle(<<(uint)h<<), handle is invalid"<<endl);
#endif
			return false;
		}	

		register ulong i;

		i = indexFromHandle(h);
		g_aHandleEntry[i].h = (handle) handleFromCount(countFromHandle(h));
		g_aHandleEntry[i].userData = g_firstFree;
		g_firstFree = i;

#ifdef RKT_DEBUG
		g_aHandleEntry[i].file = NULL;
		g_aHandleEntry[i].line = 0xFFFFFFFF;
		g_handleCount --;
#endif

		return true;
	}


	RKT_API ulong getHandleData(handle h)
	{
		handleLock;

		if (!_isValidHandle(h))
			return 0;

		register ulong userData;

		userData = g_aHandleEntry[indexFromHandle(h)].userData;

		return userData;
	}


	RKT_API bool setHandleData(handle h, ulong userData)
	{
		handleLock;

		if (!_isValidHandle(h))
			return false;

		g_aHandleEntry[indexFromHandle(h)].userData = userData;
		//register ulong old = g_aHandleEntry[indexFromHandle(h)].userData;
		//InterlockedCompareExchange((long*)&g_aHandleEntry[indexFromHandle(h)].userData, (long)userData, (long)old);

		return true;
	}


	RKT_API bool isValidHandle(handle h)
	{
		handleLock;

		return _isValidHandle(h);
	}


	RKT_API void dumpHandleTable()
	{
		handleLock;

#ifdef RKT_DEBUG
		Info("BEGIN dumpHandleTable() ************************\n");
		Info("Alloc Handle Count: "<<g_handlesAllocated<<", must <= "<<indexFromHandle(0xffffffff)<<endl);
		Info("Max Create Count: "<<g_maxCreateCount<<", must <= "<<countFromHandle(0xffffffff)<<endl);
		Info("Use Handle Count: "<<g_handleCount<<endl);
		Info("Free Handle Count: "<<(g_handlesAllocated - g_handleCount)<<endl);
		Info("Handle leak:"<<endl);

		for (uint i = 1; i < g_handlesAllocated; i++) // index 0 for system handle flags
		{
			if (_isValidHandle(g_aHandleEntry[i].h))
			{
				Info(g_aHandleEntry[i].file<<"("<<g_aHandleEntry[i].line
					<<"): handle leak(handle:"<<(uint)g_aHandleEntry[i].h
					<<", userData:"<<g_aHandleEntry[i].userData<<")"<<endl);
			}
		}
		Info("END dumpHandleTable() **************************\n");
#else
		Info("Warning:dumpHandleTable() only use in RKT_DEBUG mode!");
#endif
	}


	/** 销毁句柄表
	*/
	void destroyHandleTable()
	{
		handleLock;

		if (g_aHandleEntry)
		{
			delete[] g_aHandleEntry;
			g_aHandleEntry = NULL;
		}
	}
/*
#ifdef RKT_DEBUG
	class DumpHandle_Dummy
	{
	public:
		DumpHandle_Dummy();
		~DumpHandle_Dummy()
		{
			dumpHandleTable();
		}
	} gHandleDummy;
#endif*/

} // namespace
