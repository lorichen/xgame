/*******************************************************************
** 文件名:	UDPSocket.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	01/31/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __NET_UDP_SOCKET_H__
#define __NET_UDP_SOCKET_H__

#include "net.h"
#include "handle.h"
#include "AsynIoDevice.h"
#include "AsynIoFrame.h"

namespace xs{

	/**
	@name : 给应用层使用的UDP套接字对象
	@brief: 这个类对AsynDatagranSocket进行了一下包装
			主要满足以下几个需求:
			1.AsynStreamSocket是使用异步IO框架方式调用的,用户可能不太习惯
			2.一个连接建立后自动去收取数据,而不用用户显示去调
			3.因为收发数据是在通讯线程完成的,而用户需要在用户线程接收回调
			通知,否则上层要处理多线程同步逻辑会非常麻烦,所以也需要把线程
			转换一下
	*/
	class UDPSocket : public AsynIoHandler,public IUDPSocket
	{
	public:
		virtual void HandleIoComplete( AsynIoResult * result );

		virtual bool SendData( const char * szRemoteIP,WORD nRemotePort,const char * pData,DWORD dwDataLen,DWORD dwFlag=0 );

		virtual void SetEventHandler( IUDPHandler * pHandler );

		virtual IUDPHandler * GetEventHandler() const;

		virtual void Release();

		handle GetSafeHandle() const { return m_hSafeHandle; }

		bool Create( WORD dwBindPort,IUDPHandler * pHandler );

		bool Close();

		UDPSocket() : m_pSocket(0),m_pHandler(0),m_hSafeHandle(INVALID_HANDLE){}

		virtual ~UDPSocket() { Close(); }

	protected:
		AsynIoDevice             *   m_pSocket;
		IUDPHandler              *   m_pHandler;
		handle                       m_hSafeHandle;  // 自己的句柄,防止其他地方记录自己的指针不安全
	};
};

#endif//__NET_UDP_SOCKET_H__