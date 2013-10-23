/*******************************************************************
** 文件名:	e:\CoGame\EventEngine\include\IEventEngine.h
** 版  权:	(C) 
** 
** 日  期:	2007/9/9  20:58
** 版  本:	1.0
** 描  述:	事件引擎
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

namespace xs {
/*! \addtogroup EventEngine
*  事件引擎
*  @{
*/

/// 投票事件sink
struct IEventVoteSink 
{
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  如果返回false : 则中断执行，否则继续向下执行
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool		OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen) = 0;	
};

/// 执行事件sink
struct IEventExecuteSink 
{
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void		OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen) = 0;
};

/// 事件引擎
struct IEventEngine
{
	/** 释放
	@param   
	@param   
	@param   
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void		Release(void) = 0;
	
	/** 发送投票事件   
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度	  
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool		FireVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen) = 0;

	/** 发送执行事件
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度	  
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void		FireExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen) = 0;

	/** 订阅投票事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@param   bSrcType ：发送源类型
	@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
	@param	 pDesc : 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool		Subscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc) = 0;

	/** 订阅执行事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@param   bSrcType ：发送源类型
	@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
	@param	 pDesc : 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool		Subscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc) = 0;

	/** 取消订阅投票事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@param   bSrcType ：发送源类型
	@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
	@param	 pDesc : 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool		UnSubscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID) = 0;

	/** 取消订阅执行事件
	@param   pSink : 回调sink
	@param   wEventID : 事件ID
	@param   bSrcType ：发送源类型
	@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
	@param	 pDesc : 订阅者描述，用来调试程序
	@return  成功返回true,失败返回false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool		UnSubscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(EVENTENGINE_STATIC_LIB)	/// 静态库版本
	#	pragma comment(lib, MAKE_LIB_NAME(xs_eem))
	extern "C" IEventEngine * CreateEventEngine(void);
	#	define	CreateEventEngineProc	LoadEEM
#else													/// 动态库版本
	typedef  IEventEngine * (RKT_CDECL *procCreateEventEngine)(void);
	#	define	CreateEventEngineProc	DllApi<procCreateEventEngine>::load(MAKE_DLL_NAME(xs_eem), "LoadEEM")
#endif

}//namespace xs*/
