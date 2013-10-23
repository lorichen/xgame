/*******************************************************************
** 文件名:	HTTP_Connection.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	02/12/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __HTTP_CONNECTION_H__
#define __HTTP_CONNECTION_H__

#include "net.h"
#include "Handle.h"
using namespace xs;

#include <list>

#include "http.h"

/**
@name : 维护一条HTTP联接
@brief: 维护这样一个逻辑:
        如果是1.0版本的HTTP协议，每次发出一个请求就重新建立一次连接
        如果是1.1版本的HTTP协议，同一条连接上可以发送多个请求
*/
class HTTP_Connection : public IConnectionEventHandler
{
public:
	//////////////////////////////////////////////////////////////////////////
	virtual void OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler );

	virtual void OnConnected( IConnection * conn );

	virtual void OnDisconnected( IConnection * conn,DWORD reason );

	virtual void OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen );

	virtual void OnError( IConnection * conn,DWORD dwErrorCode );

	const std::string & GetServerIP() const { return m_ip;}

	unsigned short GetServerPort() const { return m_port;}

	HTTP_VERSION GetServerVersion() const { return m_version; }

	bool Connect( const std::string & ip,unsigned short port,HTTP_VERSION ver);

	bool Request( const std::string & url,const std::string & host,__int64 range_begin,__int64 range_end,handle hander);

	void Close();

	HTTP_Connection() : m_port(0),
		                m_version(HTTP_VER_1_1),
						m_bConnected(false),
						m_pConnection(0),
						m_bServerActive(false),
						m_pContentBuffer(0),
						m_nContentLen(0),
						m_nCurrentLen(0),
						m_pHeaderBuffer(0),
						m_nHeaderLen(0)
	{
	}

	virtual ~HTTP_Connection()
	{
		Close();
	}

protected:
	/**
	@name         : 将所有没发送的请求全部发送出去
	@brief        :
	@param resend : 是否将没成功返回的重新发送
	@return       :
	*/
	void SendAllRequest(bool resend);

	/**
	@name         : 处理HTTP响应状态行
	@brief        : 通常为 HTTP/1.1 200 OK
	*/
	bool Handle_Response_State(char * state_line,DWORD len );

protected:
	//////////////////////////////////////////////////////////////////////////
	struct REQUEST
	{
		std::string             m_szServerHost; // 服务器主机名
		std::string             m_szURL;		// 请求的URL
		__int64                 m_RangeBegin;	// 请求的分块信息
		__int64                 m_RangeEnd;		// 请求的分块信息
		handle                  m_pHander;		// 响应处理器
		bool                    m_bSended;		// 该请求是否已经发送
	};
	typedef std::list<REQUEST> REQUEST_LIST;

	std::string              m_ip;                // 服务器IP
	unsigned short           m_port;              // 服务器端口
	HTTP_VERSION             m_version;           // HTTP版本号
	IConnection      *       m_pConnection;       // 当前和服务器保持的连接
	REQUEST_LIST             m_RequestList;       // 请求列表
	bool                     m_bConnected;        // 是否已连接
	bool                     m_bServerActive;     // 服务器是否可连接
	HTTP_Response_Header     m_HttpHeader;        // 接收到的HTTP头
	char             *       m_pHeaderBuffer;     // HTTP头缓冲区
	int                      m_nHeaderLen;        // 缓存的http头长度
	char             *       m_pContentBuffer;    // 网页内容缓冲区
	int                      m_nContentLen;       // 内网长度
	int                      m_nCurrentLen;       // 当场收到的长度
};

#endif//__HTTP_CONNECTION_H__