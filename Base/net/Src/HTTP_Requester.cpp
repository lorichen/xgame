/*******************************************************************
** 文件名:	HTTP_Requester.cpp 
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

#include "stdafx.h"
#include "HTTP_Requester.h"
#include "buffer.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////////
HTTP_Requester::HTTP_Requester()
{
}

HTTP_Requester::~HTTP_Requester()
{
	CONNECTION_POOL::iterator it = m_ConnectionPool.begin();
	for ( ;it!=m_ConnectionPool.end();++it )
	{
		delete *it;
	}

	m_ConnectionPool.clear();
}

bool HTTP_Requester::Request( const std::string & url,HTTP_VERSION httpver,__int64 range_begin,__int64 range_end,handle handler)
{
	 std::string host;
	 std::string trim_url;
	 unsigned short port = 0;

	 if ( !GetURLHostName(url,host,port,trim_url))
	 {
		 ErrorLn("Get host name from url failed! Please ensure format of the url is valid!" << url);
		 return false;
	 }

	 REQUEST request;
	 request.m_szURL   = trim_url;
	 request.m_Version = httpver;
	 request.m_RangeBegin = range_begin;
	 request.m_RangeEnd = range_end;
	 request.m_ServerPort = port;
     request.m_pHandler = handler;
     request.m_ServerHost=host;

	 // 是域名
     if ( isalpha(host.at(0)))
	 {
		 m_DNSQueryingList[host].push_back(request);

		 if ( m_DNSQueryingList[host].size()==1 )
			 DNS_Resolver::getInstance().DNS_Query(host,this);
	 }else
	 {
		 request.m_ServerIP = host;
		 if ( inet_addr(request.m_ServerIP.c_str())==INADDR_NONE)
		 {
			 ErrorLn("Get host name from url failed! Please ensure format of the url is valid!" << url);
			 return false;
		 }

         SendHTTPRequest(request);
	 }

	 return true;
}

bool HTTP_Requester::SendHTTPRequest(const REQUEST & request)
{
	CONNECTION_POOL::iterator it = m_ConnectionPool.begin();
	for ( ;it!=m_ConnectionPool.end();++it )
	{
		HTTP_Connection * conn = *it;
		if ( conn->GetServerIP()==request.m_ServerIP && 
			 conn->GetServerPort()==request.m_ServerPort )
		{
            return conn->Request(request.m_szURL,request.m_ServerHost,request.m_RangeBegin,request.m_RangeEnd,request.m_pHandler);
		}
	}

	HTTP_Connection * conn = new HTTP_Connection;
	if ( !conn->Connect(request.m_ServerIP,request.m_ServerPort,request.m_Version) )
		return false;

	conn->Request(request.m_szURL,request.m_ServerHost,request.m_RangeBegin,request.m_RangeEnd,request.m_pHandler);

	m_ConnectionPool.push_back(conn);

	return true;
}

bool HTTP_Requester::GetURLHostName( const std::string & url,std::string & host,unsigned short & port,std::string & trim_url )
{
	size_t start = 0;
	size_t end = url.size();
    port = 80;

	trim_url = "/";

	int pos = url.find("://");
	if ( pos>=0 && pos<end )
	{
       start = pos+3;
	}

	pos = url.find('/',start);
	if ( pos>=0 && pos<end )
	{
		trim_url = url.substr(pos,end-pos);
		end = pos;
	}

	host = url.substr(start,end-start);
	pos  = host.find(':');
	if ( pos>=0 && pos < host.size() )
	{
		std::string port_str = host.substr(pos+1);
		unsigned long port_value = 0;
		sscanf(port_str.c_str(),"%d",&port_value);
		port = port_value;
		host = host.substr(0,pos);
	}

	pos = host.find('.');
	return pos >=0 && host.size();
}

//////////////////////////////////////////////////////////////////////////
/*********************** DNS_Queryer ************************************/
void HTTP_Requester::Handle_DNS_Response( const char * szDomain,const char ** ppIPList,unsigned int nIPNum)
{
	if ( nIPNum==0 || ppIPList==0 )
		return ;

    REQUEST_LIST & request_list = m_DNSQueryingList[szDomain];

	REQUEST_LIST::iterator it = request_list.begin();
	for ( ;it!=request_list.end();++it )
	{
		REQUEST & request = *it;
		request.m_ServerIP = ppIPList[0];

	    SendHTTPRequest(*it);
	}

	m_DNSQueryingList.erase(szDomain);
}

void HTTP_Requester::Handle_DNS_Error( const char * szDomain,DWORD dwErrorCode )
{
	REQUEST_LIST & request_list = m_DNSQueryingList[szDomain];

	REQUEST_LIST::iterator it = request_list.begin();
	for ( ;it!=request_list.end();++it )
	{
		HTTP_Response_Handler * handler = (HTTP_Response_Handler*)getHandleData(it->m_pHandler);
		if ( handler )
		     handler->OnError(dwErrorCode,it->m_szURL.c_str());
	}

	m_DNSQueryingList.erase(szDomain);
}