/*******************************************************************
** 文件名:	AsynIoFrame.cpp 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	01/25/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "AsynIoFrame.h"
#include "Proactor_Win32.h"
#include "Reactor_Win32.h"
#include "WorkerThreadPool.h"
#include "safereleasehelper.h"
#include <list>

bool g_bInitializeAsynIoFrame = false;

/// 初始化异步IO框架
NET_API int InitializeAsynIoFrame(DWORD dwProactorThreadNum)
{
	// 不要重复调用
	if ( g_bInitializeAsynIoFrame )
	{
		return 1;
	}

	// 初始化SOCKET
#	ifdef WIN32
		WORD wVersionRequested;
		WSADATA wsaData;
		int err;

		wVersionRequested = MAKEWORD( 2, 2 );

		err = WSAStartup( wVersionRequested, &wsaData );
		if ( err != 0 ) {
			return -3;
		}
#	endif

	// 初始化全局变量
	new Proactor_Win32;
	new Reactor_Win32;
	new WorkerThreadPool;

	// 启动前摄器
	if ( !Proactor_Win32::getInstance().Create(0) )
		return -1;

	// 启动工作线程
	for ( int i=0;i<dwProactorThreadNum;++i )
	{
		WorkerThreadPool::getInstance().add(new ProactorWorker);
	}

	// 启动反应器线程
	WorkerThreadPool::getInstance().add(new ReactorWorker);

	g_bInitializeAsynIoFrame = true;

	return 1;
}

/**
@name         : 释放异步IO框架
@brief        :
*/
NET_API int UninitializeAsynIoFrame()
{
	if ( g_bInitializeAsynIoFrame )
	{
		delete WorkerThreadPool::getInstancePtr();
		delete Proactor_Win32::getInstancePtr();
		delete Reactor_Win32::getInstancePtr();

#	ifdef WIN32
		WSACleanup();
#	endif

		g_bInitializeAsynIoFrame = false;
	}

	return 1;
}

/**
@name         : 取得用于异步IO的前摄器指针
@brief        : 其他有些地方可以用得到
*/
NET_API Proactor * GetAsynIoProactor()
{
	return Proactor_Win32::getInstancePtr();
}

/**
@name         : 取得用于异步IO的反应器指针
@brief        : 其他有些地方可以用得到
*/
NET_API Reactor * GetAsynIoReactor()
{
	return Reactor_Win32::getInstancePtr();
}

/**
@name         : 创建一个反应器
@brief        : 通过GetAsynIoReactor获得的反应器是在IO线程监听的,如果
              : 你需要监听事件又不想在IO线程可以自己创建一个Reactor去监听
*/
NET_API Reactor * CreateReactor()
{
	return new Reactor_Win32;
}

/**
@name         : 分配一段内存,用于异步IO
@brief        : 应用层提前分配好可以减少一次多余的内存拷贝，提高效率
@param dwSize : 需要分配的内存大小
@return       :
*/
NET_API void * AllocateAsynIoBuffer(DWORD dwSize)
{
	return malloc(dwSize);
}

/**
@name         : 释放一段用于异步IO的内存
@brief        : 应用层提前分配好可以减少一次多余的内存拷贝，提高效率
@param   buf  : 指向需要释放的内存块的指针
@return       :
*/
NET_API void   DeallocateAsynIoBuffer(void * buf)
{
	free(buf);
}