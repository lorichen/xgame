/*******************************************************************
** 文件名:	Reactor_Win32.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	01/24/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __REACTOR_WIN32_H__
#define __REACTOR_WIN32_H__

#ifdef WIN32

#include "Reactor.h"
#include "singleton.h"
#include "Thread.h"
#include "SwappingList.h"

#include <map>
#include <list>

namespace xs{

	// 轮询时间间隔
#	define  POLLING_INTERVAL     1

	/**
	@name : 反应器 (Reactor)
	@brief: 反应器就是等待多个事件然后调用相应的处理程序的这么一个装置
			Windows下用WaitForMultipleObjects实现
	@warning: 注意:
	        1.添加/删除事件需要注意多线程加锁
			2.支持多于64个事件的监听,多于64个事件时使用轮询方式检查事件
	*/
	class Reactor_Win32 : public Reactor,public EventHandler,public SingletonEx<Reactor_Win32>
	{
	public:
		/**
		@purpose    : 填加要监听的事件
		@param event: 事件句柄
		@return		  : true  ... 注册成功
		false ... 已经达到最大事件个数
		false ... 该事件不存在
		*/
		virtual bool AddEvent( HANDLE event );

		/**
		@purpose    : 移除要监听的事件
		@param event: 事件句柄
		*/
		virtual void RemoveEvent( HANDLE event );

		/**
		@purpose      : 注册一个事件 (一个Handler可以处理多个事件,一个事件可以由多个处理器处理)
		@param event  : 事件句柄
		@param handler: 事件处理程序
		@return		  : true  ... 注册成功
		false ... 已经达到最大事件个数
		false ... 该事件不存在
		*/
		virtual bool RegisterEventHandler( HANDLE event,EventHandler * handler);

		/**
		@purpose      : 取消一个事件处理器
		@note         : 该处理器不再处理这个事件，但是这个事件依旧监听，只有调用RemoveEvent才不再监听事件
		@param event  : 事件句柄
		@param handler: 事件处理程序
		@return		  :
		*/
		virtual void UnRegisterEventHandler( HANDLE event,EventHandler * handler);

		/**
		//crr add
		@name         : 在反应器内删除事件处理对象
		@brief        : RegisterEventHandler 处理事件的对象可能在反应器线程处理事件之前被删除 而引起问题
		增加接口好让在反应器线程 内删除 事件处理对象
		@note         : 
		@param event  : 事件句柄
		@param handler: 事件处理程序
		@return		  :
		*/
		virtual void SafeReleaseEventHandler(HANDLE event,EventHandler * handler);

		/**
		@purpose      : 等待事件触发并分派
		@param	wait  : 等待的毫秒数
		@return       : 等待的过程中是否出错
		*/
		virtual bool HandleEvents( DWORD wait );

		/**
		@purpose      : 循环等待/分派事件
		*/
		virtual bool HandleEventsLoop();

		/**
		@name         : 通知反应器线程退出
		@brief        : 这些函数都是多线程安全的
		*/
		virtual void NotifyExit();

		/**
		@name         : 处理自身的操作事件,用来做多线程同步
		@brief        :
		*/
		virtual void OnEvent( HANDLE event );

		virtual void Release() { delete this;}

		Reactor_Win32();

		virtual ~Reactor_Win32();

	protected:
		// 真正的处理函数,这里已经到了反应器自己的线程,没有多线程同步的问题
		bool AddEventAux( HANDLE event );
		void RemoveEventAux( HANDLE event );
		bool RegisterEventHandlerAux( HANDLE event,EventHandler * handler);
		void UnRegisterEventHandlerAux( HANDLE event,EventHandler * handler);
		void ReleaseEventHandlerAux( HANDLE event,EventHandler * handler);
		void NotifyExitAux();

	protected:
		typedef std::list<EventHandler *>     HANDLER_LIST;
		typedef std::map<HANDLE,HANDLER_LIST> HANDLERS_MAP;

		HANDLE    *  m_EventHandles;                        // 待监听的事件数组
		DWORD        m_dwEventsNum;                         // 事件个数
		DWORD        m_dwArrayCapability;                   // 数组容量
		HANDLERS_MAP m_HandlersMap;                         // 事件处理器列表

		HANDLE       m_hSelfEvent;                          // 反应器自己的维护事件
		struct       REACTOR_TASK
		{
			enum
			{
				REACTOR_ADD = 0   , 
				REACTOR_DELETE    ,
				REACTOR_REGISTER  ,
				REACTOR_UNREGISTER,
				REACTOR_RELEASE_HANDLER,
				REACTOR_EXIT      ,
			};

			int      type;          // 操作类型
			HANDLE   event;       
			EventHandler * handler;
		};

		SwappingList<REACTOR_TASK>   m_TaskList; // 任务列表
	};

};

#endif

#endif//__REACTOR_WIN32_H__