/*******************************************************************
** 文件名:	e:\CoGame\EventEngine\src\EventEngine\EventEngine.h
** 版  权:	(C) 
** 
** 日  期:	2007/9/9  21:33
** 版  本:	1.0
** 描  述:	事件引擎
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include <hash_map>
#include <list>
using namespace std;

#include "IEventEngine.h"
#include "EventEngineT.h"

struct OnVoteObject
{
	bool operator() (IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		if(pSink == NULL)
		{
			return false;
		}

		return pSink->OnVote(wEventID, bSrcType, dwSrcID, pszContext, nLen);		
	}
};

struct OnExecuteObject
{
	bool operator() (IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		if(pSink == NULL)
		{
			return false;
		}

		pSink->OnExecute(wEventID, bSrcType, dwSrcID, pszContext, nLen);

		return true;
	}
};

class CEventEngine : public IEventEngine
{
	typedef TEventEngine< IEventVoteSink, OnVoteObject >		TCENTER_VOTE;

	typedef TEventEngine< IEventExecuteSink, OnExecuteObject >	TCENTER_EXECUTE;
public:
	////////////////////////IEventEngine///////////////////////////////
	/** 释放
	@param   
	@param   
	@param   
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void		Release(void);

	/** 
	@param   
	@param   
	@return  
	*/
	/** 发送投票事件   
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（UID中"序列号"部份）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度	  
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool		FireVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	/** 发送执行事件
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（UID中"序列号"部份）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度	  
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void		FireExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	/** 订阅投票事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@param   bSrcType ：发送源类型
	@param	 dwSrcID : 发送源标识（UID中"序列号"部份），如果为0，则表示订阅所有
	@param	 pDesc : 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool		Subscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc);

	/** 订阅执行事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@param   bSrcType ：发送源类型
	@param	 dwSrcID : 发送源标识（UID中"序列号"部份），如果为0，则表示订阅所有
	@param	 pDesc : 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool		Subscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc);

	/** 取消订阅投票事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@param   bSrcType ：发送源类型
	@param	 dwSrcID : 发送源标识（UID中"序列号"部份），如果为0，则表示订阅所有
	@param	 pDesc : 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool		UnSubscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID);

	/** 取消订阅执行事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@param   bSrcType ：发送源类型
	@param	 dwSrcID : 发送源标识（UID中"序列号"部份），如果为0，则表示订阅所有
	@param	 pDesc : 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool		UnSubscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID);

	////////////////////////CEventEngine///////////////////////////////
	/** 创建
	@param   
	@param   
	@return  
	*/
	bool				Create(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CEventEngine(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CEventEngine(void);	
private:	
	// 投票中心
	TCENTER_VOTE			m_VoteCenter;
	
	// 执行中心
	TCENTER_EXECUTE			m_ExecuteCenter;
};
extern CEventEngine *		g_pEventEngine;

