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
#include "stdafx.h"
#include "EventEngine.h"

CEventEngine *	g_pEventEngine = NULL;
/** 
@param   
@param   
@return  
*/
CEventEngine::CEventEngine(void)
{

}

/** 
@param   
@param   
@return  
*/
CEventEngine::~CEventEngine(void)
{

}

/** 释放
@param   
@param   
@param   
@return  
@note     
@warning 
@retval buffer 
*/
void CEventEngine::Release(void)
{
	delete this;
	//全局变量是魔鬼!
	assert(this == g_pEventEngine);
	g_pEventEngine = NULL;
}

/** 创建
@param   
@param   
@return  
*/
bool CEventEngine::Create(void)
{
	return true;
}

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
bool CEventEngine::FireVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	return m_VoteCenter.Fire(wEventID, bSrcType, dwSrcID, pszContext, nLen);
}

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
void CEventEngine::FireExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	static Timer per4Timer;
	per4Timer.reset();

	m_ExecuteCenter.Fire(wEventID, bSrcType, dwSrcID, pszContext, nLen);

	if( per4Timer.getTime() > 30 ) //lilinll77
		ErrorLn("事件引擎执行时间过长,EventID ="<<wEventID<<",SrcType ="<<bSrcType);
}

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
bool CEventEngine::Subscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc)
{
	return m_VoteCenter.Subscibe(pSink, wEventID, bSrcType, dwSrcID, pszDesc);
}

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
bool CEventEngine::Subscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc)
{
	return m_ExecuteCenter.Subscibe(pSink, wEventID, bSrcType, dwSrcID, pszDesc);
}

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
bool CEventEngine::UnSubscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID)
{
	return m_VoteCenter.UnSubscibe(pSink, wEventID, bSrcType, dwSrcID);
}

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
bool CEventEngine::UnSubscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID)
{
	return m_ExecuteCenter.UnSubscibe(pSink, wEventID, bSrcType, dwSrcID);
}

/** 输出函数
@param   
@param   
@return  
*/
API_EXPORT IEventEngine * LoadEEM(void)
{
	// 如果创建了，则直接返回
	if(g_pEventEngine != NULL)
	{
		return g_pEventEngine;
	}

	// 实例化
	g_pEventEngine = new CEventEngine();
	if(!g_pEventEngine)
	{
		return NULL;
	}

	// 创建
	if(!g_pEventEngine->Create())
	{
		SAFE_DELETE(g_pEventEngine);

		return NULL;
	}

	return g_pEventEngine;
}
