//==========================================================================
/**
* @file	  : IMessageDispatch.h
* @author : 
* created : 2008-1-15   16:58
* purpose : 网络消息分发器
*/
//==========================================================================

#ifndef __IMESSAGEDISPATCH_H__
#define __IMESSAGEDISPATCH_H__

#include "IMessageHandler.h"

/**
	网络消息分发器负责从底层获取网络消息，然后派发到指定的模块进行处理
*/


/// 消息分发器
struct IMessageDispatch
{
	/** 分发指定的消息
	@param buf 网络层输入的消息缓冲
	@param len 缓冲区长度
	*/
	virtual void dispatchMessage(void* buf, size_t len) = 0;


	/** 注册消息处理者
	@param moduleId 模块ID，参考InputMessage.h中的枚举定义
	@param hander 消息处理者实例
	*/
	virtual void registerMessageHandler(ulong moduleId, IMessageHandler* handler) = 0;


	/** 注销消息处理者
	@param moduleId 模块ID，参考DGlobalMessage.h中的枚举定义
	*/
	virtual void unregisterMessageHandler(ulong moduleId) = 0;
};



#endif // __IMESSAGEDISPATCH_H__