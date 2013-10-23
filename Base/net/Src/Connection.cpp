/*******************************************************************
** 文件名:	Connection.cpp 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	01/28/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "Connection.h"
#include "CallbackDispatcher.h"
#include "AsynIoResultImp.h"
#include "SafeReleaseHelper.h"
#include "CallbackObject.h"
#include "Reactor.h"
#include <assert.h>

#ifdef WIN32

namespace xs{

#include <Mswsock.h>

bool CConnection::Create( SHORT dwBindPort )
{
	AsynStreamSocket * device = new AsynStreamSocket;
	m_pSocket = device;

	if ( !device->Create(this))
	{
		assert(false); // 输出错误信息
		return false;
	}
	if ( !device->Bind(dwBindPort) )
	{
		assert(false); // 输出错误信息
		return false;
	}

	return true;
}

bool CConnection::Listen()
{
	assert(m_pSocket);

	if ( m_hAcceptEvent==INVALID_HANDLE_VALUE )
	{
		m_hAcceptEvent = ::CreateEvent(NULL,FALSE,FALSE,0);
		assert(m_hAcceptEvent!=INVALID_HANDLE_VALUE);
	}

	::WSAEventSelect((SOCKET)m_pSocket->GetHandle(),m_hAcceptEvent,FD_ACCEPT);
	GetAsynIoReactor()->AddEvent(m_hAcceptEvent);
	GetAsynIoReactor()->RegisterEventHandler(m_hAcceptEvent,this);

	return m_pSocket->Acceptor()->accept(this);
}


 void CConnection::ChangePacker(DWORD CryptId,uchar *key,DWORD keylen)
{

}

void CConnection::OnEvent( HANDLE event )
{
	assert(m_pSocket);
	assert(event==m_hAcceptEvent);

	// 有连接正在等待,新增一个Accept对象
	m_pSocket->Acceptor()->accept(this);
}

bool CConnection::Connect( const char * ip,unsigned short port )
{
	m_RemoteAddr.sin_family = AF_INET;
	m_RemoteAddr.sin_addr.s_addr = inet_addr( ip );
	m_RemoteAddr.sin_port = htons( port );

	assert(m_pSocket);
	return m_pSocket->Connector()->connect(&m_RemoteAddr,this);
}

bool CConnection::SendData( const char * pData,DWORD dwDataLen,DWORD dwFlag )
{
	assert(m_pSocket);
	assert(pData);
	assert(dwDataLen);

	return m_pSocket->Writer()->write((char *)pData,dwDataLen,this);
}

#if 0
bool CConnection::Disconnect()
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

	if ( m_hAcceptEvent!=INVALID_HANDLE_VALUE )
	{
		GetAsynIoReactor()->RemoveEvent(m_hAcceptEvent);
		::CloseHandle(m_hAcceptEvent);
		m_hAcceptEvent = INVALID_HANDLE_VALUE;
	}

	return true;
}
#endif

void CConnection::SetEventHandler( IConnectionEventHandler * pHandler )
{
	m_pHandler = pHandler;
}

IConnectionEventHandler * CConnection::GetEventHandler() const
{
	return m_pHandler;
}

/**
@name              : 取得对方的网络地址
@param dwRemoteIP  : 返回对方的IP [OUT]
@param wRemotePort : 返回对方的端口 [OUT]
@return            : 如果对方的地址还不确定则返回失败
*/
bool CConnection::GetRemoteAddr(DWORD &dwRemoteIP,WORD & wRemotePort)
{
	dwRemoteIP = m_RemoteAddr.sin_addr.S_un.S_addr;
	wRemotePort = m_RemoteAddr.sin_port;
	return true;
}

bool CConnection::GetRemoteAddr( const char ** ppRemoteIP,DWORD & dwRemotePort)
{
	*ppRemoteIP = inet_ntoa(m_RemoteAddr.sin_addr);
	dwRemotePort = ntohs(m_RemoteAddr.sin_port);
	return true;
}

bool  CConnection::IsPassive() const
{
	return m_bIsPassive;   
}

#if 1
void CConnection::Release()
{
	if ( m_hAcceptEvent!=INVALID_HANDLE_VALUE )
	{
		GetAsynIoReactor()->RemoveEvent(m_hAcceptEvent);
		::CloseHandle(m_hAcceptEvent);
		m_hAcceptEvent = INVALID_HANDLE_VALUE;
	}

	//crr mod 因为Connection 是AsynStreamSocket::CreateAsynIoResult的AsynIoHandler，多个完成端口线程引用了本Connection指针
	//故修改本Connection的释放时机是 当m_pSocket 的pendinglist为空,且m_iSendThreadRef ==0
	if ( m_pSocket )
	{
		// Post到工作线程来释放
		AsynIoResult * result = m_pSocket->CreateAsynIoResult(ASYN_IO_CLOSE,this);
		GetAsynIoProactor()->PostCompletion(result);
		m_pSocket->Release();
		//m_pSocket = 0;
	}
	else
	{
		delete this;
	}
}
#else
void CConnection::Release()
{
	if ( m_pSocket )
	{
		// Post到工作线程来释放
		AsynIoResult * result = m_pSocket->CreateAsynIoResult(ASYN_IO_CLOSE,this);
		GetAsynIoProactor()->PostCompletion(result);
	}else
	{
		ReleaseAux();
	}
}

void CConnection::ReleaseAux()
{
	Disconnect();

// 	if ( m_hSafeHandle!=INVALID_HANDLE )
// 	{
// 		rktCloseHandle(m_hSafeHandle);
// 		m_hSafeHandle = INVALID_HANDLE;
// 	}

	delete this;
}
#endif

void CConnection::HandleIoComplete( AsynIoResult * result )
{
	AsynIoDevice * device = result->GetAsynDevice();
	assert(device);
	assert(m_pHandler);

	DWORD dwError = result->GetErrorCode();

	if ( result->Success() )
	{
		switch(result->GetType())
		{
		case ASYN_IO_ACCEPT:                  // Incoming Connection
			{
				AsynStreamSocketAcceptResult * pAcceptResult = (AsynStreamSocketAcceptResult *)result;
				AsynIoDevice * incomingDevice = pAcceptResult->GetIncomingSocket();
				assert(incomingDevice);
				CConnection * incoming = this->Clone();
				incoming->m_bIsPassive = true;
				int nRemoteAddrLen = 0;
				incoming->SetSocketDevice(incomingDevice);
				pAcceptResult->GetRemoteAddr(incoming->m_RemoteAddr,nRemoteAddrLen);
				// 继续accept下一个
				device->Acceptor()->accept(this);
				//crr 出于线程安全放到执行CallBack的线程中去DeleteAsynIoResult
				// 删除result
				//device->DeleteAsynIoResult(result);
				// 通知应用层
				CallbackDispatcher::getInstance().RequestCallback(new Callback_Accept(this,incoming,result));
				break;
			}
		case ASYN_IO_CONNECT:                // Connect OK
			RecvNextPackage();
			// 删除result
			//device->DeleteAsynIoResult(result);
			// 通知应用层
			CallbackDispatcher::getInstance().RequestCallback(new Callback_Connect(this,result));
			break;
		case ASYN_IO_READ:                   // Data Recieve
			{
				AsynIoBuffer * pBuffer = result->GetBufferList();
				assert(pBuffer && result->GetBufferCount()==1);
				LPVOID pData = pBuffer->buf;
				DWORD dwTransferedBytes = result->GetTransferredBytes();

				if (dwTransferedBytes>0)
				{
					result->SetBuffer(0,0);          // Detach The Buff



					//crr mod,默认采用的SimplePacker 会在开头两个字节记录本次发送数据的大小
					//检查包接收是否完整，注意：有可能存在连包情况
					
					//上一次接收到的包的开头部分
					dwTransferedBytes += result->GetLastRecievedLen();

					WORD wCurPacketSize = 0;
					DWORD dwRestDataLen = dwTransferedBytes;
					char* pCurPacket = (char*)pData;
					while (true)
					{
						if (dwRestDataLen < sizeof WORD)
						{
							//ErrorLn("包未接收完整 未接受simple packer的头2个字节");
							break;
						}
						wCurPacketSize = *(WORD*)pCurPacket + sizeof WORD;	
						if (dwRestDataLen < wCurPacketSize)
						{
							//ErrorLn("包未接收完整 大小< " << wCurPacketSize);
							break;
						}

						pCurPacket = pCurPacket + wCurPacketSize;
						dwRestDataLen -= wCurPacketSize;
						if (0 == dwRestDataLen)
						{
							break;
						}
					}
					if (dwRestDataLen == 0)
					{
						//单个包或多个连着的包接收完整
						// 收下一个包
						RecvNextPackage();
						// 删除result
						//device->DeleteAsynIoResult(result);
						// 通知应用层
						CallbackDispatcher::getInstance().RequestCallback(new Callback_Recv(this,pData,dwTransferedBytes, result));

					}
					else
					{
						//最后一个包未接收完整
						// 把这个包接收完整
						RecvThisPackageRestData((LPVOID)pCurPacket, dwRestDataLen,wCurPacketSize);
						// 删除result
						//device->DeleteAsynIoResult(result);
						// 通知应用层
						CallbackDispatcher::getInstance().RequestCallback(new Callback_Recv(this,pData,dwTransferedBytes - dwRestDataLen,result));
					}

				}else
				{
					// 删除result
					//device->DeleteAsynIoResult(result);
					// 通知应用层
					CallbackDispatcher::getInstance().RequestCallback(new Callback_TCP_Error(this,WSAECONNRESET, result));
				}
			}
			break;
		case ASYN_IO_WRITE:                  // Send OK
				// 删除result
				device->DeleteAsynIoResult(result);
			break;
		case ASYN_IO_CLOSE:
			//crr 这里释放掉connection后，那些Callback 中引用到本this会出错
			device->DeleteAsynIoResult(result);
			//CallbackDispatcher::getInstance().RequestCallback(new Callback_Release(this,WSAECONNRESET));
			//ReleaseAux();
			break;
		default:
			// 删除result
			device->DeleteAsynIoResult(result);
			break;
		}
	}else
	{
		// 投递出错信息
		// 删除result
		//device->DeleteAsynIoResult(result);
		CallbackDispatcher::getInstance().RequestCallback(new Callback_TCP_Error(this,dwError, result));
	}

	// 删除result(不能在这里删,要提前删,因为callback后该连接可能已经delete了)
	// device->DeleteAsynIoResult(result);
}

// 收取下一个包
bool CConnection::RecvNextPackage()
{
	assert(m_pSocket);
	m_pSocket->Reader()->read(MAX_NET_PACKAGE_SIZE,this);
	return true;
}


//crr add,某个tcp包没有接收完整，接收剩下的数据
bool CConnection::RecvThisPackageRestData(LPVOID pRecieved, DWORD dwRecievLen, DWORD dwRestLen)
{	
	assert(m_pSocket);
	((AsynStreamSocket_Reader *)(m_pSocket->Reader()))->read(MAX_NET_PACKAGE_SIZE,this, pRecieved, dwRecievLen, dwRestLen);
	return true;
}

};

#endif//WIN32