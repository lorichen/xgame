//==========================================================================
/**
* @file	  : IChatClient.h
* @author : 
* created : 2008-3-6   11:20
* purpose : 聊天模块客户端接口定义
*/
//==========================================================================

#ifndef __ICHATCLIENT_H__
#define __ICHATCLIENT_H__

#include "ChatMsgDef.h"
#include "UserInfoDef.h"
#include <list>

/// 聊天客户端
struct IChatClient
{
	/** 发送聊天消息
	@param channelType	频道类型，参考ChatChannelType定义
	@param msg			要发送的聊天内容
    @param msg			要发送的聊天时是否有装备属性 
	@param target		私聊时的目标数据库ID，非私聊时，该参数没有意义，可填0
	*/
	virtual void sendChatMessage(ulong channelType, const char* msg, const char* goodsTip, dbid target = 0,bool bOpen = false,ulong ulDuration = 0) const = 0;

	/** 发送聊天消息
	@param channelType	频道类型，参考ChatChannelType定义
	@param msg			要发送的聊天内容
	@param msg			要发送的聊天时是否有装备属性
	@param target		私聊时的目标数据库ID，非私聊时，该参数没有意义，可填0
	*/
	virtual void sendChatMessageByName(ulong channelType, const char* msg,const char* goodsTip,const char* name,ulong ulDuration = 0) const = 0;

	/** 发送订阅消息配置
	@param channelType	频道类型，参考ChatChannelType定义
	@param bSelected    是否被选中		
	*/
	virtual void sendChatConfig(ulong channelType, bool bSelected) = 0;

	/** 更新泡泡位置
	@param
	@param 
	@param 
	*/
	virtual void UpdatePaoPaoPlace(int nLeft,int nBottom,LONGLONG uid) = 0;

	/** Close泡泡
	@param
	@param 
	@param 
	*/
	virtual void ClosePaoPaoFadeOutScreen(LONGLONG uid) = 0;

	/** Show泡泡
	@param
	@param 
	@param 
	*/
	virtual void ShowPaoPaoFadeInScreen(LONGLONG uid) = 0;


	/** 添加系统信息
	@param pos	系统信息显示位置
	@param tips 提示信息
	@param userData 用户数据(不关心时可以填0)
	*/
	virtual void addSystemInfo(ulong pos, const char* tips, ulong userData = 0, bool bErrMsg = false) const = 0;

	/** 添加系统信息
	@param pos	系统信息显示位置
	@param tips 提示信息
	@param userData 用户数据(不关心时可以填0)
	*/
	virtual void addSystembroadcastInfo(ulong pos, ulong channelType,const char* tips, ulong userData = 0) const = 0;

	/** 屏蔽玩家
	@param dbidPerson	玩家PID
	*/
	virtual void shieldPerson(dbid dbidPerson, bool bShield, const string& name) = 0;

	/** 是否被屏蔽
	@param dbidPerson	玩家PID
	*/
	virtual bool isShield(dbid dbidPerson) = 0;

	virtual std::list<dbid> GetShieldList() = 0;

	virtual int GetShieldPlayerOnlineNum() = 0;

	virtual int GetShieldPlayerNum() = 0;

	virtual SShieldRecord GetShieldRecord(unsigned long dbidPerson) = 0;

	virtual		void  FilterChatMessage(char* lpchat) = 0; 

	virtual void release() = 0;

	virtual void Reset() = 0;

	// 移除聊天泡泡列表内的内容
	virtual void RemoveChatPaoPaoList(int nPaoPaoID) = 0;
};


#if defined(_LIB) || defined(CHATCLIENT_STATIC_LIB)/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(xs_cct))
extern "C" IChatClient* LoadCCT(void);
#	define	CreateChatClientProc	LoadCCT
#	define  CloseChatClient()
#else /// 动态库版本
typedef IChatClient* (RKT_CDECL *procCreateChatClient)(void);
#	define	CreateChatClientProc	DllApi<procCreateChatClient>::load(MAKE_DLL_NAME(xs_cct), "LoadCCT")
#	define  CloseChatClient()		DllApi<procCreateChatClient>::freelib()
#endif


#endif // __ICHATCLIENT_H__