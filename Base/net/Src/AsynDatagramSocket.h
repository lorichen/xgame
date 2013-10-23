/*******************************************************************
** 文件名:	AsynDatagramSocket.h 
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

#ifndef __ASYN_DATAGRAM_SOCKET_H__
#define __ASYN_DATAGRAM_SOCKET_H__

#include "AsynIoFrame.h"
#include "Reactor.h"
#include "AsynIoResult.h"
#include "AsynIoDevice.h"
#include "AsynIoOperation.h"
#include "Thread.h"

#include <set>

namespace xs{

	/**
	@name : UDP协议的套接字封装
	@brief: 
	*/
	class AsynDatagramSocket : public AsynIoDevice
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		/*********************** AsynIoDevice ***********************************/

		virtual HANDLE GetHandle() const{return (HANDLE)m_hSocketHandle;}

		virtual AsynIoOperation_Reader * Reader() { return m_pReader;}

		virtual AsynIoOperation_Writer * Writer() { return m_pWriter; }

		virtual AsynIoOperation_Acceptor * Acceptor() { return 0;}

		virtual AsynIoOperation_Connector * Connector(){ return 0;}

		AsynIoResult * CreateAsynIoResult(int iotype,AsynIoHandler * handler);

		virtual void DeleteAsynIoResult(AsynIoResult * result);

		virtual void Release();

		bool Create(unsigned short port);

		bool Close();

		AsynDatagramSocket() : m_hSocketHandle(INVALID_SOCKET),m_pReader(0),m_pWriter(0){}

		virtual ~AsynDatagramSocket() { Close(); }

	protected:
		//////////////////////////////////////////////////////////////////////////
		SOCKET                 m_hSocketHandle;  // 套接字句柄

		AsynIoOperation_Reader *    m_pReader;   // 读取器
		AsynIoOperation_Writer *    m_pWriter;   // 写入器

		typedef std::set<AsynIoResult *> RESULT_LIST;
		RESULT_LIST                 m_pendingList;// 未完成的操作

		Mutex                       m_Locker;    // 线程锁
	};

	/**
	@name : 异步流套接字读操作
	@brief:
	*/
	class AsynDatagramSocket_Reader : public AsynIoOperation_Reader
	{
	public:
		virtual bool read(DWORD expect_bytes,AsynIoHandler * handler);

		AsynDatagramSocket_Reader(AsynIoDevice* device) : m_pDevice(device){}

	protected:
		AsynIoDevice *   m_pDevice;
	};

	/**
	@name : 异步流套接字写操作
	@brief:
	*/
	class AsynDatagramSocket_Writer : public AsynIoOperation_Writer
	{
	public:
		/// UDP的设备因为发送数据时需要指定对方的地址所以实现比较特殊,默认地址(sockaddr_in)放在缓冲区的最前面
		virtual bool write(const char * data,DWORD len,AsynIoHandler * handler);

		/// UDP的设备因为发送数据时需要指定对方的地址所以实现比较特殊,默认第一个Buff用来放地址(sockaddr_in)
		virtual bool writeEx( AsynIoBuffer * pBuffList,DWORD dwBuffCount,AsynIoHandler * handler);

		AsynDatagramSocket_Writer(AsynIoDevice * device) : m_pDevice(device){}

	protected:
		AsynIoDevice *   m_pDevice;
	};
};

#endif//__ASYN_DATAGRAM_SOCKET_H__