/*******************************************************************
** 文件名:	Connection.h 
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

#ifndef __NET_CONNECTION_H__
#define __NET_CONNECTION_H__

#include "net.h"
#include "AsynStreamSocket.h"
#include "Handle.h"

#include <assert.h>

namespace xs{

	/**
	@name : 给应用层使用的连接对象
	@brief: 这个类对AsynStreamSocket进行了一下包装
	主要满足以下几个需求:
			1.AsynStreamSocket是使用异步IO框架方式调用的,用户可能不太习惯
			2.一个连接建立后自动去收取数据,而不用用户显示去调
			3.因为收发数据是在通讯线程完成的,而用户需要在用户线程接收回调
			通知,否则上层要处理多线程同步逻辑会非常麻烦,所以也需要把线程
			转换一下
			4.在一个并发连接非常频繁的系统中,没有只有一个Accept操作在等待是
			远远不够的,所以这里注册FD_ACCEPT事件,一旦该事件触发就新增一个等
			待的Accept操作
	*/
	class CConnection : public AsynIoHandler,public EventHandler,public IConnection
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		virtual bool Listen();

		virtual bool Connect( const char * ip,unsigned short port );

		virtual bool SendData( const char * pData,DWORD dwDataLen,DWORD dwFlag );

		//virtual bool Disconnect();

		virtual void SetEventHandler( IConnectionEventHandler * pHandler );

		virtual IConnectionEventHandler * GetEventHandler() const;


		virtual bool GetRemoteAddr(DWORD &dwRemoteIP,WORD & wRemotePort);

		virtual bool GetRemoteAddr( const char ** ppRemoteIP,DWORD & dwRemotePort);

		virtual bool  IsPassive() const;

		virtual void Release();

		virtual void HandleIoComplete( AsynIoResult * result );

		virtual void OnEvent( HANDLE event );

		virtual void ChangePacker(DWORD CryptId,uchar *key,DWORD keylen);

		//////////////////////////////////////////////////////////////////////////
		virtual bool Create( SHORT dwBindPort );

		/**
		@name         : 克隆一个和自己同样类型的连接,Accept时需要用到
		@brief        : 需要克隆的信息包括: Socket选项设置,组包方式,加密方式等
		@return       :
		*/
		virtual CConnection * Clone() { return new CConnection; }

		virtual void SetSocketDevice(AsynIoDevice * device) {m_pSocket=device;}

		virtual AsynIoDevice* GetSocketDevice(void) {return m_pSocket;}

		// 收取下一个包
		bool RecvNextPackage();

		//crr add,某个tcp包没有接收完整，接收剩下的数据
		bool RecvThisPackageRestData(LPVOID pRecieved, DWORD dwRecievLen, DWORD dwRestLen);


		//crr mod 因为Connection 是AsynStreamSocket::CreateAsynIoResult的AsynIoHandler，多个完成端口线程引用了本Connection指针
		//故修改本Connection的释放时机是 当m_pSocket 的pendinglist为空时
		//void ReleaseAux();

		
		//handle GetSafeHandle() const { return m_hSafeHandle; }

		CConnection() : m_pSocket(0),m_pHandler(0),m_hAcceptEvent(INVALID_HANDLE_VALUE)//,m_hSafeHandle(INVALID_HANDLE)
		{
			m_bIsPassive  = false;
			//m_hSafeHandle = rktCreateHandle((ulong)this);
			memset(&m_RemoteAddr,0,sizeof(m_RemoteAddr));
		}

	protected:
		//除了AsynStreamSocket，禁止其他类显式调用delete
		friend AsynStreamSocket;
		virtual ~CConnection()
		{
			//Disconnect();
		}

	protected:
		AsynIoDevice             *   m_pSocket;
		IConnectionEventHandler  *   m_pHandler;
		HANDLE                       m_hAcceptEvent;
		//crr mod, 定义的句柄包装,纯粹多此一举，而且其非线程安全，放弃使用
		//handle                       m_hSafeHandle;        // 自己的句柄,防止其他地方记录自己的指针不安全
		bool                         m_bIsPassive;         // 是否是被动连接
		sockaddr_in                  m_RemoteAddr;         // 对方地址
	};
};

#endif//__NET_CONNECTION_H__