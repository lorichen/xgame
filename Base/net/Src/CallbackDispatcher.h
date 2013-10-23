/*******************************************************************
** 文件名:	CallbackDispatcher.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	01/26/2008
** 版  本:	1.0
** 描  述:	回调分发器
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __CALLBACK_DISPATCHER_H__
#define __CALLBACK_DISPATCHER_H__

#include "singleton.h"
#include "LockFree.h"
#include "AsynIoFrame.h"
#include "Connection.h"
#include "UDPSocket.h"
#include "Handle.h"
#include "Thread.h"
#include "LockFree.h"

namespace xs{

	/**
	@name : 回调分发器
	@brief: 由于存在多个工作线程序并行查询完成端口，所以完成操作可能在多个线程触发
			而应用层通常只有一个逻辑线程序，为了避免应用层编写线程同步代码，这里把
			在不同线程完成的事件统一转到用户的逻辑线程来回调
	*/
	class CallbackDispatcher : public SingletonEx<CallbackDispatcher>
	{
	public:
		/// 回调对象
		struct CallbackObject : public IRunnable
		{
			virtual void run() = 0;
			virtual void release() = 0;
		};

		// 请求一次回调
		void RequestCallback(CallbackObject * context)
		{
			m_ResultList.Add(context);
			SetEvent(m_hEventHandle);
		}

		// 用户线程序持续取回调内存
		void DispatchCallback()
		{
			CallbackObject * pCall = 0;
			while(m_ResultList.Get(pCall))
			{
				if ( pCall==0 )
				{
					break;
				}

				pCall->run();
				pCall->release();
			}
		}

		HANDLE GetEventHandle() const
		{
			return m_hEventHandle;
		}

		CallbackDispatcher()
		{
			m_hEventHandle = ::CreateEvent(0,FALSE,FALSE,0);
		}

		virtual ~CallbackDispatcher()
		{
			if ( m_hEventHandle!=INVALID_HANDLE_VALUE )
			{
				::CloseHandle(m_hEventHandle);
				m_hEventHandle = INVALID_HANDLE_VALUE;
			}
		}

	protected:
		LockFreeQueue<CallbackObject *>   m_ResultList;
		HANDLE                            m_hEventHandle;
	};
};

#endif//__CALLBACK_DISPATCHER_H__