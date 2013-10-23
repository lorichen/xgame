/*******************************************************************
** 文件名:	AsynStreamSocket.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	01/24/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __ASYN_STREAM_SOCKET_H__
#define __ASYN_STREAM_SOCKET_H__

#ifdef WIN32

#include "AsynIoFrame.h"
#include "Reactor.h"
#include "AsynIoResult.h"
#include "AsynIoDevice.h"
#include "AsynIoOperation.h"
#include "Thread.h"

#include <set>

namespace xs{

class CConnection;

	/**
	@name : 异步流套接字封装(TCP)
	@brief: 
	*/
	class AsynStreamSocket : public AsynIoDevice
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		/*********************** AsynIoDevice ***********************************/

		virtual HANDLE GetHandle() const
		{
			return (HANDLE)m_hSocketHandle;
		}

		virtual AsynIoOperation_Reader * Reader() 
		{ 
			return m_pReader; 
		}

		virtual AsynIoOperation_Writer * Writer() 
		{ 
			return m_pWriter; 
		}

		virtual AsynIoOperation_Acceptor * Acceptor() 
		{ 
			return m_pAcceptor; 
		}

		virtual AsynIoOperation_Connector * Connector() 
		{ 
			return m_pConnector; 
		}

		AsynIoResult * CreateAsynIoResult(int iotype,AsynIoHandler * handler);

		virtual void DeleteAsynIoResult(AsynIoResult * result);

		virtual void Release();

		bool Create(CConnection* pConn);

		bool Bind( unsigned short port );

		bool Listen();

		bool Close();

		bool IncreaseSendRef(void);
		void DecreaseSendRef(int iCount = 1);

		AsynStreamSocket() : m_hSocketHandle(INVALID_SOCKET),
							 m_pReader(0),
							 m_pWriter(0),
							 m_pAcceptor(0),
							 m_pConnector(0),
							 m_iSendThreadRef(0),
							 m_bWaitingClosed(false)
		{
		}

		virtual ~AsynStreamSocket() { Close(); }

	protected:
		//////////////////////////////////////////////////////////////////////////
		int					 m_iSendThreadRef;//发送线程的引用次数
		bool				 m_bWaitingClosed;
		CConnection			* m_pConnection;
		SOCKET                 m_hSocketHandle;  // 套接字句柄

		AsynIoOperation_Reader *    m_pReader;   // 读取器
		AsynIoOperation_Writer *    m_pWriter;   // 写入器
		AsynIoOperation_Acceptor*   m_pAcceptor; // 接收器
		AsynIoOperation_Connector*  m_pConnector;// 连接器
		
		typedef std::set<AsynIoResult *> RESULT_LIST;
		RESULT_LIST                 m_pendingList;// 未完成的操作

		Mutex                       m_Locker;    // 线程锁
	};

	/**
	@name : 异步流套接字读操作
	@brief:
	*/
	class AsynStreamSocket_Reader : public AsynIoOperation_Reader
	{
	public:
		virtual bool read(DWORD expect_bytes,AsynIoHandler * handler);
		virtual bool read(DWORD expect_bytes,AsynIoHandler * handler, LPVOID pRecieved, DWORD dwRecievedLen, DWORD dwRestLen);
		AsynStreamSocket_Reader(AsynIoDevice* device) : m_pDevice(device){}

	protected:
		AsynIoDevice *   m_pDevice;
	};

	/**
	@name : 异步流套接字写操作
	@brief:
	*/
	class AsynStreamSocket_Writer : public AsynIoOperation_Writer
	{
	public:
		virtual bool write(const char * data,DWORD len,AsynIoHandler * handler);

		virtual bool writeEx( AsynIoBuffer * pBuffList,DWORD dwBuffCount,AsynIoHandler * handler);

		AsynStreamSocket_Writer(AsynIoDevice * device) : m_pDevice(device){}

	protected:
		AsynIoDevice *   m_pDevice;
	};

	/**
	@name : 异步流套接字接收操作
	@brief:
	*/
	class AsynStreamSocket_Acceptor : public AsynIoOperation_Acceptor
	{
	public:
		virtual bool accept(AsynIoHandler * handler);

		AsynStreamSocket_Acceptor(AsynIoDevice * device) : m_pDevice(device),m_bListen(false){}

	protected:
		AsynIoDevice *   m_pDevice;
		bool             m_bListen;
	};

	/**
	@name : 异步流套接字连接操作
	@brief:
	*/
	class AsynStreamSocket_Connector : public AsynIoOperation_Connector,public EventHandler
	{
	public:
		virtual bool connect( sockaddr_in * addr,AsynIoHandler * handler );

		virtual void OnEvent( HANDLE event );

		void cancel() { m_pHandler = 0; }

		void SafeRelease(void)
		{
			bool bReleaseFlag = false;
			do 
			{
				ResGuard<Mutex>  guard(m_ThreadRefLocker);
				m_lThreadRef --;
				if (0 >= m_lThreadRef)
				{
					bReleaseFlag = true;
				}
				

			} while (0);

			if (bReleaseFlag)
			{
				delete this;
			}
		}

		AsynStreamSocket_Connector(AsynIoDevice * device);

		virtual ~AsynStreamSocket_Connector();

	protected:
		AsynIoDevice *   m_pDevice;
		HANDLE           m_hConnectEvent;
		AsynIoHandler *  m_pHandler;

		//crr ad 因为 Connector 在connect的时候向反应器线程注册了事件，并将自己this指针作为handler
		//直接释放m_pConnector 会引起非法, 增加变量m_iThreadRef记录线程引用计数
		//只有m_iThreadRef = 0 时才真正释放
		//另外改net组件时 要特别注意m_pDevice m_pHandler这两个指针指向对象是否已经释放了凸 凸

		volatile LONG	m_lThreadRef;
		Mutex m_ThreadRefLocker;
				
	};
};

#endif//WIN32

#endif//__ASYN_STREAM_SOCKET_H__