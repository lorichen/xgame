/*******************************************************************
** 文件名:	HTTP_Requester.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	02/10/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include "singleton.h"
#include "DNS_Resolver.h"
#include "HTTP_Connection.h"
#include "Handle.h"

#include <string>
#include <list>
#include <map>

/**
@name : 封装HTTP请求功能
@brief: GET方式
*/
class HTTP_Requester : public DNS_Queryer,public SingletonEx<HTTP_Requester>
{
public:
	//////////////////////////////////////////////////////////////////////////
    /**
    @name         : 发送HTTP请求
    @brief        : 使用GET方式
    @param   url  : 要请求的url,带参数，例如http://www.carfax.com/register.php?username=no1
	@param httpver: 要使用的http协议,目前有1.0和1.1
	@param data_rng:要请求的数据段,如果为空则不指定
	@param handler: 处理http响应的回调接口,为了安全起见请使用句柄,该句柄指向HTTP_Response_Handler的指针
    @return       :
    */
	bool Request( const std::string & url,HTTP_VERSION httpver,__int64 range_begin,__int64 range_end,handle handler);

	//////////////////////////////////////////////////////////////////////////
	/*********************** DNS_Queryer ************************************/
	virtual void Handle_DNS_Response( const char * szDomain,const char ** ppIPList,unsigned int nIPNum);

	virtual void Handle_DNS_Error( const char * szDomain,DWORD dwErrorCode );

	HTTP_Requester();

	virtual ~HTTP_Requester();

protected:
	//////////////////////////////////////////////////////////////////////////
	struct REQUEST
	{
		std::string    m_ServerHost;      // 服务器域名
		std::string    m_ServerIP;        // 服务器IP
		unsigned short m_ServerPort;      // 服务器端口
		std::string    m_szURL;           // 请求的URL
		HTTP_VERSION   m_Version;         // 请求的版本号
		__int64        m_RangeBegin;	  // 请求的分块信息
		__int64        m_RangeEnd;		  // 请求的分块信息
		handle         m_pHandler;        // 响应处理器
	};

	typedef std::list<REQUEST> REQUEST_LIST;
	typedef std::map<std::string,REQUEST_LIST> DNS_QUERYING_LIST;

	typedef std::list<HTTP_Connection *> CONNECTION_POOL;

	/**
	@name         : 从URL中取得主机名 
	@brief        : 例如:url=http://www.carfax.com/register.php?username=aaa 返回www.carfax.com
	@param  url   : 传入url地址
	@param  host  : 返回主机名
	@param  port  : 返回服务器端口
	@param trim_url:返回裁剪过后的url
	@return       : 如果url格式不合法则返回false	*/
	bool  GetURLHostName( const std::string & url,std::string & host,unsigned short & port,std::string & trim_url );

	bool SendHTTPRequest(const REQUEST & request);

protected:
	CONNECTION_POOL              m_ConnectionPool;    // 正在连接的服务器列表
	DNS_QUERYING_LIST            m_DNSQueryingList;   // 等待DNS请求的列表
};

#endif//__HTTP_REQUEST_H__