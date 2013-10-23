/*******************************************************************
** 文件名:	ConnectionWrapper.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	01/29/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __NET_CONNECTION_WRAPPER_H__
#define __NET_CONNECTION_WRAPPER_H__

#include "Connection.h"
#include "IPacker.h"
#include "SenderThread.h"
#include "dns.h"

namespace xs{

	/**
	@name : Connection包装器
	@brief: 对Connection再包装一层,用来支持一些扩展功能,例如:
			1.组包功能
			2.加密功能
			3.支持域名
	*/
	class CConnectionWrapper : public CConnection,public IConnectionEventHandler,public DNS_Queryer
	{
	public:
		virtual bool SendData( const char * pData,DWORD dwDataLen,DWORD dwFlag );

		virtual void SetEventHandler( IConnectionEventHandler * pHandler );
	
		virtual bool Connect( const char * ip,unsigned short port );

		virtual void OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler );

		virtual void OnConnected( IConnection * conn );

		virtual void OnDisconnected( IConnection * conn,DWORD reason );

		virtual void OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen );

		virtual void OnError( IConnection * conn,DWORD dwErrorCode );

		virtual void Release();

		virtual void Handle_DNS_Response( const char * szDomain,const char ** ppIPList,unsigned int nIPNum);

		virtual void Handle_DNS_Error( const char * szDomain,DWORD dwErrorCode );

		virtual bool Create( SHORT dwBindPort );

		// 设置组包方法
		void SetPacker(IPacker * packer);

		virtual void  ChangePacker(DWORD CryptId,uchar *key,DWORD keylen) ;

		/**
		@name         : 克隆一个和自己同样类型的连接,Accept时需要用到
		@brief        : 需要克隆的信息包括: Socket选项设置,组包方式,加密方式等
		@return       :
		*/
		virtual CConnection * Clone();

		CConnectionWrapper() : m_pRealHandler(0),m_pPacker(0),m_dwSenderID(0){}
	private:
		virtual ~CConnectionWrapper();

	protected:
		IConnectionEventHandler  *       m_pRealHandler; // 用户真正的处理接口
		IPacker                  *       m_pPacker;      // 组包接口
		DWORD                            m_dwSenderID;   // 发送ID,由发送线程分配
	};
};

#endif//__NET_CONNECTION_WRAPPER_H__