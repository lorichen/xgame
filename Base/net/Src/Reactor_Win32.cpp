/*******************************************************************
** 文件名:	Reactor_Win32.cpp 
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

#include "stdafx.h"
#include "Reactor_Win32.h"

#include <algorithm>
#include <assert.h>

using namespace xs;

//////////////////////////////////////////////////////////////////////////
Reactor_Win32::Reactor_Win32() : m_dwEventsNum(0)
{
	m_dwArrayCapability = MAXIMUM_WAIT_OBJECTS;
	m_EventHandles = new HANDLE[m_dwArrayCapability];
	memset(m_EventHandles,0,m_dwArrayCapability*sizeof(HANDLE)); 

	m_hSelfEvent = ::CreateEvent(NULL,FALSE,FALSE,0);
	AddEventAux(m_hSelfEvent);
	RegisterEventHandlerAux(m_hSelfEvent,this);
}

//////////////////////////////////////////////////////////////////////////
Reactor_Win32::~Reactor_Win32()
{
	if ( m_EventHandles )
	{
		delete []m_EventHandles;
		m_EventHandles = 0;
	}

	if ( m_hSelfEvent!=INVALID_HANDLE_VALUE )
	{
		::CloseHandle(m_hSelfEvent);
		m_hSelfEvent = INVALID_HANDLE_VALUE;
	}
}

//////////////////////////////////////////////////////////////////////////
bool Reactor_Win32::AddEventAux( HANDLE event )
{
	// 扩容
	if ( m_dwEventsNum>=m_dwArrayCapability)
	{
		m_dwArrayCapability += MAXIMUM_WAIT_OBJECTS;
		HANDLE * newArray = new HANDLE[m_dwArrayCapability];
		memcpy(newArray,m_EventHandles,m_dwEventsNum*sizeof(HANDLE));
		delete []m_EventHandles;
		m_EventHandles = newArray;
	}

	for ( int i=0;i<m_dwEventsNum;++i )
	{
		if ( m_EventHandles[i]==event )
		{
			return false;
		}
	}	
	
	m_EventHandles[m_dwEventsNum] = event;
	m_dwEventsNum ++;
	return true;
}

//////////////////////////////////////////////////////////////////////////
void Reactor_Win32::RemoveEventAux( HANDLE event )
{
	for ( int i=0;i<m_dwEventsNum;++i )
	{
		if ( m_EventHandles[i]==event )
		{
			if ( i<m_dwEventsNum-1)
			{
				memcpy(&m_EventHandles[i],&m_EventHandles[i+1],(m_dwEventsNum-i-1)*sizeof(HANDLE));
			}

			-- m_dwEventsNum;
		}
	}

	m_HandlersMap.erase(event);
}

//////////////////////////////////////////////////////////////////////////
bool Reactor_Win32::RegisterEventHandlerAux( HANDLE event,EventHandler * handler)
{
	HANDLER_LIST & handlerList = m_HandlersMap[event];
	HANDLER_LIST::iterator it = std::find(handlerList.begin(),handlerList.end(),handler);
	if ( it!=handlerList.end() )
		return false;

	handlerList.push_back(handler);
	return true;
}

//////////////////////////////////////////////////////////////////////////
void Reactor_Win32::UnRegisterEventHandlerAux( HANDLE event,EventHandler * handler)
{
	HANDLER_LIST & handlerList = m_HandlersMap[event];
	HANDLER_LIST::iterator it = std::find(handlerList.begin(),handlerList.end(),handler);
	if ( it!=handlerList.end() )
	{
		handlerList.erase(it);
	}
}

//////////////////////////////////////////////////////////////////////////
void Reactor_Win32::ReleaseEventHandlerAux( HANDLE event,EventHandler * handler)
{
	HANDLERS_MAP::iterator eventit = m_HandlersMap.find(event);
	if (eventit == m_HandlersMap.end())
	{
		return;
	}
	HANDLER_LIST & handlerList = eventit->second;
	HANDLER_LIST::iterator it = std::find(handlerList.begin(),handlerList.end(),handler);
	if ( it!=handlerList.end() )
	{
		(*it)->Release();
		handlerList.erase(it);
	}
}
//////////////////////////////////////////////////////////////////////////
void Reactor_Win32::NotifyExitAux()
{
	::ExitThread(0);
}

//////////////////////////////////////////////////////////////////////////
bool Reactor_Win32::HandleEvents( DWORD wait )
{
	if ( m_dwEventsNum==0 )
	{
		::Sleep(1);
		return true;
	}

	// 如果事件个数大于64就应该是polling模式,这时候不应该是INFINITE的等待时间
	if ( m_dwEventsNum>MAXIMUM_WAIT_OBJECTS)
	{
		assert( wait!=INFINITE);
	}

	DWORD offset = 0;

	// 开始轮询
	do 
	{
		DWORD dwWaitNum = min(m_dwEventsNum-offset,MAXIMUM_WAIT_OBJECTS);
		DWORD result = ::WaitForMultipleObjects(dwWaitNum,m_EventHandles+offset,FALSE,wait);

		if ( result==WAIT_TIMEOUT )
		{
			offset += dwWaitNum;
			continue;
		}

		if ( result==WAIT_FAILED )
		{
			ErrorLn("WaitForMultipleObjects WAIT_FAILED");
			OnEvent(m_hSelfEvent);
			continue;       // 应用层删除事件,因为Reactor的线程可能还在等待,所以由于事件句柄已释放,这里可能会产生一个错误,待删除后就会正常了
			assert(false);  // 什么地方出错了？
			return false;
		}

		DWORD dwIndex = result - WAIT_OBJECT_0;
		if ( dwIndex>=0 && dwIndex<dwWaitNum)
		{
			//crr remove this,一个事件发生后，只处理这个事件发生的事情
			//OnEvent(m_hSelfEvent);

			// 派发给事件处理器
			HANDLER_LIST & handlerList = m_HandlersMap[m_EventHandles[dwIndex+offset]];
			HANDLER_LIST::iterator it = handlerList.begin();
			for ( ;it!=handlerList.end();++it )
			{
				EventHandler * handler = *it;
				handler->OnEvent(m_EventHandles[dwIndex+offset]);
			}	

			// 如果是第一个事件表示有事件新增或减少,需要重新监听一下
			if (dwIndex+offset==0)
			{
				return true;
			}
		}else
		{
			assert(false);  // 奇怪的返回值
			return false;
		}

		//继续检查下一组
		offset += dwWaitNum;
	}while(offset<m_dwEventsNum);

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Reactor_Win32::HandleEventsLoop()
{
	// 大于64个事件自动转为轮询模式
	while(HandleEvents(m_dwEventsNum>MAXIMUM_WAIT_OBJECTS ? POLLING_INTERVAL : INFINITE))
	{
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Reactor_Win32::AddEvent( HANDLE event )
{
	REACTOR_TASK task;
	task.event = event;
	task.type  = REACTOR_TASK::REACTOR_ADD;
	task.handler = 0;

	m_TaskList.Add(task);
	SetEvent(m_hSelfEvent);
	return true;
}

//////////////////////////////////////////////////////////////////////////
void Reactor_Win32::RemoveEvent( HANDLE event )
{
	REACTOR_TASK task;
	task.event = event;
	task.type  = REACTOR_TASK::REACTOR_DELETE;
	task.handler = 0;

	m_TaskList.Add(task);
	SetEvent(m_hSelfEvent);
	return ;
}

//////////////////////////////////////////////////////////////////////////
bool Reactor_Win32::RegisterEventHandler( HANDLE event,EventHandler * handler)
{
	REACTOR_TASK task;
	task.event = event;
	task.type  = REACTOR_TASK::REACTOR_REGISTER;
	task.handler = handler;

	m_TaskList.Add(task);
	SetEvent(m_hSelfEvent);
	return true;
}

//////////////////////////////////////////////////////////////////////////
void Reactor_Win32::UnRegisterEventHandler( HANDLE event,EventHandler * handler)
{
	REACTOR_TASK task;
	task.event = event;
	task.type  = REACTOR_TASK::REACTOR_UNREGISTER;
	task.handler = handler;

	m_TaskList.Add(task);
	SetEvent(m_hSelfEvent);
	return ;
}
//////////////////////////////////////////////////////////////////////////
void Reactor_Win32::SafeReleaseEventHandler(HANDLE event,xs::EventHandler * handler)
{
	REACTOR_TASK task;
	task.event = event;
	task.type  = REACTOR_TASK::REACTOR_RELEASE_HANDLER;
	task.handler = handler;

	m_TaskList.Add(task);
	SetEvent(m_hSelfEvent);
	return ;
}
//////////////////////////////////////////////////////////////////////////
void Reactor_Win32::NotifyExit()
{
	REACTOR_TASK task;
	task.event = 0;
	task.type  = REACTOR_TASK::REACTOR_EXIT;
	task.handler = 0;

	m_TaskList.Add(task);
	SetEvent(m_hSelfEvent);
	return ;
}

//////////////////////////////////////////////////////////////////////////
void Reactor_Win32::OnEvent( HANDLE event )
{
	REACTOR_TASK task;
	while(m_TaskList.Get(task))
	{
		switch(task.type)
		{
		case REACTOR_TASK::REACTOR_ADD:
			AddEventAux(task.event);
			break;
		case REACTOR_TASK::REACTOR_DELETE:
			RemoveEventAux(task.event);
			break;
		case REACTOR_TASK::REACTOR_REGISTER:
			RegisterEventHandlerAux(task.event,task.handler);
			break;
		case REACTOR_TASK::REACTOR_UNREGISTER:
			UnRegisterEventHandlerAux(task.event,task.handler);
			break;
		case REACTOR_TASK::REACTOR_RELEASE_HANDLER:
			ReleaseEventHandlerAux(task.event, task.handler);
			break;
		case REACTOR_TASK::REACTOR_EXIT:
			NotifyExitAux();
			break;
		default:
			assert(false);
			break;
		}
	}
}