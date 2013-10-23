/*******************************************************************
** 文件名:	UDPSocket.cpp 
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

#include "stdafx.h"
#include "UDPSocket.h"
#include "CallbackDispatcher.h"
#include "AsynDatagramSocket.h"
#include "CallbackObject.h"
#include "SafeReleaseHelper.h"
#include "senderthread.h"

#ifdef WIN32

namespace xs{

//////////////////////////////////////////////////////////////////////////
bool UDPSocket::Create( WORD dwBindPort,IUDPHandler * pHandler)
{
	AsynDatagramSocket * device = new AsynDatagramSocket;
	m_pSocket = device;

	if ( !device->Create(dwBindPort) )
	{
		assert(false); // 输出错误信息
		return false;
	}

	m_hSafeHandle = rktCreateHandle((DWORD)this);

	SetEventHandler(pHandler);

	// 开始收数据
	if ( !m_pSocket->Reader()->read(MAX_NET_PACKAGE_SIZE,this) )
	{
		assert(false); // 输出错误信息
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool UDPSocket::Close()
{
	// closesocket调用后完成端口会将所有pending状态的相关异步io操作返回
	// 但是由于处理完成事件的工作线程不一定和调用Close的线程是同一个
	// 所以这里不能直接delete pending状态的结果对象,可选的做法有
	// 1.Post一个消息给工作线程,让工作线程去Release AsynIODevice
	// 2.在result对象中设置一个标志,延迟到该操作完成时delete它,当然要确保不要出现内存泄露
	// 使用第一种方法较为稳妥.
	if ( m_pSocket )
	{
		m_pSocket->Release();
		m_pSocket = 0;
	}
	
	if ( m_hSafeHandle!=INVALID_HANDLE )
	{
		closeHandle(m_hSafeHandle);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
void UDPSocket::HandleIoComplete( AsynIoResult * result )
{
	AsynIoDevice * device = result->GetAsynDevice();
	assert(device);
	assert(m_pHandler);

	DWORD dwError = result->GetErrorCode();

	AsynIoBuffer * pBuffer = result->GetBufferList();

	if ( result->Success() )
	{
		switch(result->GetType())
		{
		case ASYN_IO_READFROM:                
			{
				assert(pBuffer && result->GetBufferCount()==1);
				// 读出地址
				char * pData = pBuffer->buf;
				DWORD  dwDataLen = result->GetTransferredBytes();
				DWORD dwAppendLen = ((sizeof(sockaddr_in)+sizeof(int)+63)/64)*64; // 要64字节对齐
				sockaddr_in * addr = (sockaddr_in *)(pData);
				// 回调应用层
				result->SetBuffer(0,0);
				Callback_RecvFrom * callback = new Callback_RecvFrom(this,addr,pData+dwAppendLen,dwDataLen);
				CallbackDispatcher::getInstance().RequestCallback(callback);

				// 收下一个包
				m_pSocket->Reader()->read(MAX_NET_PACKAGE_SIZE,this);
			}
			break;
		case ASYN_IO_WRITETO:                   // Send OK
			break;
		default:
			break;
		}
	}else
	{
		// 收下一个包
		switch(result->GetType())
		{
		case ASYN_IO_READFROM:
			m_pSocket->Reader()->read(MAX_NET_PACKAGE_SIZE,this);
			break;
		default:
			break;
		}

		// 删除result
		device->DeleteAsynIoResult(result);	

		// 投递出错信息
		CallbackDispatcher::getInstance().RequestCallback(new Callback_UDP_Error(this,dwError));
	}
}

//////////////////////////////////////////////////////////////////////////
bool UDPSocket::SendData( const char * szRemoteIP,WORD nRemotePort,const char * pData,DWORD dwDataLen,DWORD dwFlag )
{
	char * pBuffer = 0;

	if ( dwFlag & NO_COPY )
	{
		pBuffer = (char *)pData;
	}else
	{
		pBuffer = (char *)AllocateAsynIoBuffer(dwDataLen);   // 这段内存要到发送成功才会在Result对象中销毁
		memcpy(pBuffer,pData,dwDataLen);
	}

	sockaddr_in * remote_addr = (sockaddr_in *)AllocateAsynIoBuffer(sizeof(sockaddr_in));
	remote_addr->sin_family = AF_INET;
	remote_addr->sin_port = htons( nRemotePort );
	remote_addr->sin_addr.s_addr = inet_addr(szRemoteIP);

	AsynIoBuffer buffList[2];
	buffList[0].buf = (char *)remote_addr;
	buffList[0].len = sizeof(sockaddr_in);
	buffList[1].buf = pBuffer;
	buffList[1].len = dwDataLen;

	//SenderThread::getInstance().RequestSendData(m_pSocket,this,m_hSafeHandle,buffList[0]);
	//SenderThread::getInstance().RequestSendData(m_pSocket,this,m_hSafeHandle,buffList[1]);
	SenderThread::getInstance().RequestSendData(m_pSocket,this,buffList[0]);
	SenderThread::getInstance().RequestSendData(m_pSocket,this,buffList[1]);
	return true;
}

//////////////////////////////////////////////////////////////////////////
void UDPSocket::SetEventHandler( IUDPHandler * pHandler )
{
	m_pHandler = pHandler;
}

//////////////////////////////////////////////////////////////////////////
IUDPHandler * UDPSocket::GetEventHandler() const
{
	return m_pHandler;
}

//////////////////////////////////////////////////////////////////////////
void UDPSocket::Release()
{
	delete this;
}

};

#endif//WIN32