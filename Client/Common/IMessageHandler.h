//==========================================================================
/**
* @file	  : IMessageHandler.h
* @author : 
* created : 2008-1-15   16:51
* purpose : 网络消息处理者
*/
//==========================================================================

#ifndef __IMESSAGEHANDLER_H__
#define __IMESSAGEHANDLER_H__

#include "DGlobalMessage.h"


/// 网络消息处理者
struct IMessageHandler
{
	/** 消息处理
	@param msg 网络消息
	*/
	virtual void onMessage(ulong actionId, SGameMsgHead* head, void* data, size_t len) = 0;
};


#endif // __IMESSAGEHANDLER_H__