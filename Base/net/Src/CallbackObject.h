/*******************************************************************
** 文件名:	CallbackObject.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	03/05/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __CALLBACK_OBJECT_H__
#define __CALLBACK_OBJECT_H__

#include "Handle.h"
#include "CallbackDispatcher.h"
#include "Connection.h"
#include "UDPSocket.h"
#include "AsynFileWrapper.h"

using namespace xs;

/**
@name : 通知Accept成功
@brief:
*/
class Callback_Accept : public CallbackDispatcher::CallbackObject
{
public:
	virtual void run()
	{
		CConnection * conn = m_hConnection;//(CConnection *)getHandleData(m_hConnection);
		CConnection * incoming = m_hIncomingConn;//(CConnection *)getHandleData(m_hIncomingConn);

		if ( conn && incoming )
		{
			IConnectionEventHandler * handler = conn->GetEventHandler();
			if ( handler )
			{
				IConnectionEventHandler * pIncomingHandler = 0;
				handler->OnAccept(incoming,&pIncomingHandler);
				assert(pIncomingHandler);
				incoming->SetEventHandler(pIncomingHandler);
				incoming->RecvNextPackage();  // 要等Handler设置好了才能收包
			}

			//m_pResult->GetAsynDevice()->DeleteAsynIoResult(m_pResult);
			conn->GetSocketDevice()->DeleteAsynIoResult(m_pResult);
		}
	}

	virtual void release()
	{	
		delete this;
	}

	Callback_Accept(CConnection * pListenConn,CConnection * pIncomingConn,AsynIoResult * pResult)
	{
		m_hConnection = pListenConn;//pListenConn->GetSafeHandle();
		m_hIncomingConn = pIncomingConn;//pIncomingConn->GetSafeHandle();
		m_pResult = pResult;
	}

protected:
	//crr 句柄包装纯粹多此一举，其非线程安全，放弃使用
	//handle        m_hConnection;
	//handle        m_hIncomingConn;

	CConnection*        m_hConnection;
	CConnection*        m_hIncomingConn;
	
	AsynIoResult   * m_pResult;
};

/**
@name : 通知连接成功
@brief:
*/
class Callback_Connect : public CallbackDispatcher::CallbackObject
{
public:
	Callback_Connect(CConnection * pConn,AsynIoResult * result)
	{
		m_hConnection = pConn;//pConn->GetSafeHandle();
		m_pResult = result;
	}

	virtual void run()
	{
		CConnection * conn = m_hConnection;//(CConnection *)getHandleData(m_hConnection);
		if ( conn )
		{

			IConnectionEventHandler * handler = conn->GetEventHandler();
			if ( handler )
			{
				handler->OnConnected(conn);
			}
			
			conn->GetSocketDevice()->DeleteAsynIoResult(m_pResult);
		}
	}

	virtual void release()
	{
		delete this;
	}

protected:
	//crr 句柄包装纯粹多此一举，其非线程安全，放弃使用
	//handle        m_hConnection;
	CConnection*    m_hConnection;

	AsynIoResult   * m_pResult;
};

/**
@name : 通知收到数据
@brief:
*/
class Callback_Recv : public CallbackDispatcher::CallbackObject
{
public:
	Callback_Recv(CConnection * pConn,LPVOID pData,DWORD dwDataLen,AsynIoResult   * pResult) : m_pData(pData),m_dwDataLen(dwDataLen)
	{
		m_hConnection = pConn;//pConn->GetSafeHandle();
		m_pResult = pResult;
	}

	virtual void run()
	{
		CConnection * conn = m_hConnection;//(CConnection *)getHandleData(m_hConnection);
		if ( conn )
		{

			IConnectionEventHandler * handler = conn->GetEventHandler();
			if ( handler )
			{
				//存在连包情况
				DWORD dwRestDataLen = m_dwDataLen;
				LPVOID pCurPacket = m_pData;


				while (dwRestDataLen > 0)
				{
					//handler->OnRecv(conn,m_pData,m_dwDataLen);
					//默认采用的SimplePacker 会在开头两个字节记录本次发送数据的大小
					
					if (dwRestDataLen < sizeof WORD)
					{
						ErrorLn("Callback_Recv包未接收完整 未接受simple packer的头2个字节");
						break;
					}
					WORD wCurPacketSize = *(WORD*)pCurPacket + sizeof WORD;	
					if (dwRestDataLen < wCurPacketSize)
					{
						ErrorLn("Callback_Recv包未接收完整 大小< " << wCurPacketSize);
						break;
					}
					
					handler->OnRecv(conn,pCurPacket,wCurPacketSize);
					
					pCurPacket = (LPVOID)((char*)pCurPacket + wCurPacketSize);
					dwRestDataLen -= wCurPacketSize;

				}

				
			}

			conn->GetSocketDevice()->DeleteAsynIoResult(m_pResult);
		}

		DeallocateAsynIoBuffer(m_pData);     // 释放result分配的内存

	}

	virtual void release()
	{
		delete this;
	}

protected:
	//handle        m_hConnection;
	CConnection*	m_hConnection;
	LPVOID        m_pData;
	DWORD         m_dwDataLen;

	AsynIoResult   * m_pResult;
};

/**
@name : 通知UDP收到数据包
@brief:
*/
class Callback_RecvFrom : public CallbackDispatcher::CallbackObject
{
public:
	Callback_RecvFrom(UDPSocket * sock,sockaddr_in * addr,LPVOID pData,DWORD dwDataLen) : m_pData(pData),m_dwDataLen(dwDataLen)
	{
		m_hSocket = sock;//sock->GetSafeHandle();
		m_addr = addr;
	}

	virtual void run()
	{
		UDPSocket * sock = m_hSocket;//(UDPSocket *)getHandleData(m_hSocket);
		if ( sock )
		{
			IUDPHandler * handler = sock->GetEventHandler();
			if ( handler )
			{
				handler->OnRecv(sock,m_pData,m_dwDataLen,inet_ntoa(m_addr->sin_addr),ntohs(m_addr->sin_port));
			}
		}

		DeallocateAsynIoBuffer(m_addr);     // 释放result分配的内存,注意result分配的内存其实是连接addr一起分配的,所以这里传入addr的指针
	}

	virtual void release()
	{
		delete this;
	}

protected:
	//handle        m_hSocket;
	UDPSocket *		m_hSocket;
	sockaddr_in * m_addr;
	LPVOID        m_pData;
	DWORD         m_dwDataLen;
};

/**
@name : 通知TCP连接错误
@brief:
*/
class Callback_TCP_Error : public CallbackDispatcher::CallbackObject
{
public:
	Callback_TCP_Error(CConnection * pConn,DWORD dwErrorCode, AsynIoResult   * pResult) : m_dwErrorCode(dwErrorCode)
	{
		m_hConnection = pConn;//pConn->GetSafeHandle();
		m_pResult = pResult;
	}

	virtual void run()
	{
		CConnection * conn = m_hConnection;//(CConnection *)getHandleData(m_hConnection);
		if ( conn )
		{
			
			IConnectionEventHandler * handler = conn->GetEventHandler();
			if ( handler )
			{
				handler->OnError(conn,m_dwErrorCode);
			}
			conn->GetSocketDevice()->DeleteAsynIoResult(m_pResult);
		}		
	}

	virtual void release()
	{
		delete this;
	}

protected:
	//handle        m_hConnection;
	CConnection*	m_hConnection;
	DWORD         m_dwErrorCode;

	AsynIoResult   * m_pResult;
};

/**
@name : 通知UDP连接错误
@brief:
*/
class Callback_UDP_Error : public CallbackDispatcher::CallbackObject
{
public:
	Callback_UDP_Error(UDPSocket * sock,DWORD dwErrorCode) : m_dwErrorCode(dwErrorCode)
	{
		m_hSocket = sock->GetSafeHandle();
	}

	virtual void run()
	{
		UDPSocket * sock = (UDPSocket *)getHandleData(m_hSocket);
		if ( sock )
		{
			IUDPHandler * handler = sock->GetEventHandler();
			if ( handler )
			{
				handler->OnError(sock,m_dwErrorCode);
			}
		}
	}

	virtual void release()
	{
		delete this;
	}

protected:
	handle        m_hSocket;
	DWORD         m_dwErrorCode;
};

/**
@name : 文件读取成功
@brief:
*/
class Callback_Read : public CallbackDispatcher::CallbackObject
{
public:
	Callback_Read(AsynFileWrapper * file,unsigned __int64 pos,LPVOID pData,DWORD dwDataLen) : m_Position(pos),m_pData(pData),m_dwDataLen(dwDataLen)
	{
		m_hFile = file->GetSafeHandle();
	}

	virtual void run()
	{
		AsynFileWrapper * file = (AsynFileWrapper *)getHandleData(m_hFile);
		if ( file )
		{
			IAsynFileHandler * handler = file->GetEventHandler();
			if ( handler )
			{
				handler->HandleReadSuccess(m_Position,(const char *)m_pData,m_dwDataLen);
			}
		}

		DeallocateAsynIoBuffer(m_pData);     // 释放result分配的内存
	}

	virtual void release()
	{
		delete this;
	}

protected:
	handle        m_hFile;
	LPVOID        m_pData;
	DWORD         m_dwDataLen;
	unsigned __int64 m_Position;
};

/**
@name : 文件错误
@brief:
*/
class Callback_Read_Error : public CallbackDispatcher::CallbackObject
{
public:
	Callback_Read_Error(AsynFileWrapper * file,DWORD dwErrorCode,unsigned __int64 pos,DWORD len) : m_dwReadLen(len),m_Position(pos),m_dwErrorCode(dwErrorCode)
	{
		m_hFile = file->GetSafeHandle();
	}

	virtual void run()
	{
		AsynFileWrapper * file = (AsynFileWrapper *)getHandleData(m_hFile);
		if ( file )
		{
			IAsynFileHandler * handler = file->GetEventHandler();
			if ( handler )
			{
				handler->HandleReadFailed(m_dwErrorCode,m_Position,m_dwReadLen);
			}
		}
	}

	virtual void release()
	{
		delete this;
	}

protected:
	handle        m_hFile;
	DWORD         m_dwErrorCode;
	DWORD         m_dwReadLen;
	unsigned __int64 m_Position;
};

/**
@name : 文件写入完成
@brief:
*/
class Callback_Write : public CallbackDispatcher::CallbackObject
{
public:
	Callback_Write(AsynFileWrapper * file,DWORD dwError,unsigned __int64 pos,DWORD dwDataLen) : m_Position(pos),m_dwError(dwError),m_dwDataLen(dwDataLen)
	{
		m_hFile = file->GetSafeHandle();
	}

	virtual void run()
	{
		AsynFileWrapper * file = (AsynFileWrapper *)getHandleData(m_hFile);
		if ( file )
		{
			IAsynFileHandler * handler = file->GetEventHandler();
			if ( handler )
			{
				if ( m_dwError==ERROR_SUCCESS )
				{
					handler->HandleWriteSuccess(m_Position,m_dwDataLen);
				}else
				{
					handler->HandleWriteFailed(m_dwError,m_Position,m_dwDataLen);
				}
			}
		}
	}

	virtual void release()
	{
		delete this;
	}

protected:
	handle        m_hFile;
	DWORD         m_dwError;
	DWORD         m_dwDataLen;
	unsigned __int64 m_Position;
};

#endif//__CALLBACK_OBJECT_H__