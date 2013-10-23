/*******************************************************************
** 文件名:	AsynIoResultImp.h 
** 版  权:	(C) 深圳未名网络技术有限公司 2008 - All Rights Reserved
** 创建人:	achen
** 日  期:	02/03/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __ASYN_IO_RESULT_IMP_H__
#define __ASYN_IO_RESULT_IMP_H__

#include "AsynIoResult.h"
#include "AsynIoDevice.h"

#ifdef WIN32

#include <Mswsock.h>

namespace xs{

	/**
	@name : TCP Accept操作的返回结果
	@brief: 1.记录Accept到的新连接的指针
	        2.维护Accept操作收到的对方地址等信息
	*/
	class AsynStreamSocketAcceptResult : public AsynIoResultImp
	{
	public:
		enum
		{
			SOCKET_ADDRESS_LEN = (sizeof(sockaddr_in)+16) ,  // 网络地址长度
			ACCEPT_BUFFER_LEN  = SOCKET_ADDRESS_LEN * 2   ,  // 接收缓冲区的长度
		};

		/**
		@name         : 取得接收缓冲区地址
		@brief        : 接收缓冲区完成后的内容 = 收到的第一块数据 + 自己的地址 + 对方的地址
		              : 目前我们调用AcceptEx时默认不接收第一块数据,所以其长度为0
		*/ 
		char * GetAcceptBuffer() { return m_AcceptBuffer; };

		/// 取得接收缓冲区长度
		DWORD GetAcceptBufferLen() const { return ACCEPT_BUFFER_LEN; }

		/// 取得远程主机地址
		bool GetRemoteAddr( sockaddr_in & remoteAddr,int & nAddLen )
		{
			sockaddr * pLocalAddr = 0;
			int nLocalAddrLen = 0;
			sockaddr * pRemoteAddr = 0;
			int nRemoteAddrLen = 0;

			::GetAcceptExSockaddrs(m_AcceptBuffer,0,SOCKET_ADDRESS_LEN,SOCKET_ADDRESS_LEN,&pLocalAddr,&nLocalAddrLen,&pRemoteAddr,&nRemoteAddrLen);

			if ( pRemoteAddr==0 || nRemoteAddrLen==0 )
				return false;
			
			memcpy(&remoteAddr,pRemoteAddr,nRemoteAddrLen);
			nAddLen = nRemoteAddrLen;
			return true;
		}

		/// 设置新连入的连接指针
		void SetIncomingSocket(AsynIoDevice * socket) { m_pIncomingSocket=socket;}

		/// 获取新连入的连接指针
		AsynIoDevice  *   GetIncomingSocket() const { return m_pIncomingSocket; }

		AsynStreamSocketAcceptResult() : m_pIncomingSocket(0){}

		AsynStreamSocketAcceptResult(AsynIoHandler * pHandler) : AsynIoResultImp(ASYN_IO_ACCEPT,pHandler) , m_pIncomingSocket(0){}

	protected:
		AsynIoDevice  *   m_pIncomingSocket;
		char              m_AcceptBuffer[ACCEPT_BUFFER_LEN];
	};
};

#endif//WIN32

#endif//__ASYN_IO_RESULT_IMP_H__