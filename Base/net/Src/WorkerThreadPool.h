/*******************************************************************
** 文件名:	WorkerThreadPool.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	01/25/2008
** 版  本:	1.0
** 描  述:	工作线程池
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __WORKER_THREAD_POOL_H__
#define __WORKER_THREAD_POOL_H__

#include "singleton.h"
#include "Proactor_Win32.h"
#include "Reactor_Win32.h"
#include "Thread.h"

namespace xs{

	/**
	@name : 完成端口工作线程
	@brief: 该工作线程序组持续从Proactor中收取IO完成事件
	*/
	class ProactorWorker : public IRunnable
	{
	public:
		virtual void run()
		{
			Proactor_Win32::getInstance().HandleEventsLoop();
		}

		virtual void release()
		{
			delete this;
		}
	};

	/**
	@name : 反应器工作线程
	@brief:
	*/
	class ReactorWorker : public IRunnable
	{
	public:
		virtual void run()
		{
			Reactor_Win32::getInstance().HandleEventsLoop();
		}

		virtual void release()
		{
			delete this;
		}
	};

	/**
	@name : 工作线程序组
	@brief: 
	*/
	class WorkerThreadPool : public ThreadPool,public SingletonEx<WorkerThreadPool>
	{
	};
};

#endif//__WORKER_THREAD_POOL_H__