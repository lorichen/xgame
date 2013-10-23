/*******************************************************************
** 文件名:	net.cpp 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	01/26/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


#include "stdafx.h"
#include "net.h"
#include "ConnectionWrapper.h"
#include "CallbackDispatcher.h"
#include "SimplePacker.h"
#include "DataPacker.h"
#include "senderthread.h"
#include "WorkerThreadPool.h"
#include "UDPSocket.h"
#include "DNS_Resolver.h"
#include "HTTP_Requester.h"

#include "http.h"
#include "dns.h"

using namespace xs;

/// 通讯层是否已初试化
bool g_bInitNetwork = false;

/**
@name         : 初始化通讯层
@brief        : 初始化过程:
              : 1.初始化异步IO框架
@return       : 0 ... OK
			    -1... 异步IO框架初始化失败
*/
NET_API int  InitializeNetwork(DWORD dwThreadNum)
{
	if ( g_bInitNetwork )
		return 0;

	if ( InitializeAsynIoFrame(dwThreadNum)<0 )
		return -1;

	// 初始化全局变量
	new CallbackDispatcher;
	new SenderThread;
	new DNS_Resolver;
	new HTTP_Requester;

	if ( !DNS_Resolver::getInstance().Create() )
	{
		return -2;
	}

	// 启动发包线程
	WorkerThreadPool::getInstance().add(SenderThread::getInstancePtr());

	g_bInitNetwork = true;

	return 1;
}

/**
@name         : 释放通讯层
@brief        : 如果在程序退出时应用层还没有调用该函数,通讯层会自己释放
@brief        : 所以这个函数可以不调用
*/
NET_API int  UninitializeNetwork()
{
	if ( g_bInitNetwork )
	{
		delete HTTP_Requester::getInstancePtr();
		delete DNS_Resolver::getInstancePtr();
		delete SenderThread::getInstancePtr();
		delete CallbackDispatcher::getInstancePtr();

		UninitializeAsynIoFrame();

		g_bInitNetwork = false;
	}

	return 1;
}

/**
@name            : 获取回调内容
@brief           : 通讯层的各种事件都是在通讯线程触发的,而用户希望收到的通讯事件必须在用户线程
            	 : 这样上层代码就不需要额外处理线程同步的操作,所以在用户线程的主循环里,需要持续
 				 : 调用DispatchNetworkEvents函数来驱动通讯事件的派发,类似在主循环收取窗口消息一样
*/
NET_API void DispatchNetworkEvents()
{
	if ( !g_bInitNetwork )
	{
		InitializeNetwork();
	}

	CallbackDispatcher::getInstance().DispatchCallback();
}


/**
@name            : 创建一个TCP连接对象
@param dwBindPort: 绑定端口,传0主要用在主动方发起连接时随机选择一个可用端口
@param pHandler  : 事件处理接口指针
@brief:
*/
NET_API IConnection * RKT_CDECL CreateTCPConnection(USHORT dwBindPort,IConnectionEventHandler * pHandler,IPacker * pPacker)
{
	if ( !g_bInitNetwork )
	{
		InitializeNetwork();
	}

	CConnectionWrapper * conn = new CConnectionWrapper;
	if ( !conn->Create(dwBindPort))
	{
		assert(false);  // 错误处理
		return 0;
	}

	conn->SetEventHandler(pHandler);

	if ( pPacker==DEFAULT_PACK )
		pPacker = new DataPacker;

	if ( pPacker!=NO_PACK )
	    conn->SetPacker(pPacker);

	return conn;
}


/**
@name            : 取得网络事件的通知句柄
@brief           : 应用层可以监听这个事件然后调用DispatchNetworkEvents收取网络消息
@return          : 返回事件句柄
*/
NET_API HANDLE GetNetworkEventHandle()
{
	return CallbackDispatcher::getInstance().GetEventHandle();
}

/**
@name            : 创建一个UDP套接字对象
@param dwBindPort: 绑定端口
@param pHandler  : 事件处理接口指针
@brief:
*/
NET_API IUDPSocket * CreateUDPSocket(USHORT dwBindPort,IUDPHandler * pHandler)
{
	if ( !g_bInitNetwork )
	{
		InitializeNetwork();
	}

	UDPSocket * socket = new UDPSocket;
	if ( socket->Create(dwBindPort,pHandler) )
	{
		return socket;
	}else
	{
		delete socket;
		return 0;
	}
}

/**
@name         : 发送HTTP请求
@brief        : 使用GET方式
@param   url  : 要请求的url,带参数，例如http://www.carfax.com/register.php?username=no1
@param httpver: 要使用的http协议,目前有1.0和1.1
@param data_rng:要请求的数据段,如果为空则不指定
@param handler: 处理http响应的回调接口,为了安全起见请使用句柄,该句柄指向HTTP_Response_Handler的指针
@return       :
*/
NET_API bool HTTP_Request( const char * url,HTTP_VERSION httpver,__int64 range_begin,__int64 range_end,handle handler)
{
	return HTTP_Requester::getInstance().Request(url,httpver,range_begin,range_end,handler);
}

/**
@name         : 查询一个域名
@brief        :
@param domain : 待查询的域名
@param queryer: 查询者,用于查询成功后的通知
@note         : 在查询过程中该指针必须保持有效,如果该指针被释放,需要主动调用Cancel_DNS_Query取消查询
@return       :
*/
NET_API bool DNS_Query( const char * domain,DNS_Queryer * queryer )
{
	return DNS_Resolver::getInstance().DNS_Query(domain,queryer);
}

/**
@name         : 取消一个域名查询请求
@brief        :
@param domain : 待查询的域名
@param queryer: 查询者,用于查询成功后的通知
@return       :
*/
NET_API bool Cancel_DNS_Query( const char * domain,DNS_Queryer * queryer )
{
	return DNS_Resolver::getInstance().Cancel_DNS_Query(domain,queryer);
}