/*******************************************************************
** 文件名:	HTTP_Connection.cpp 
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

#include "stdafx.h"
#include "AsynIoFrame.h"
#include "HTTP_Connection.h"
#include "StringHelper.h"
#include <algorithm>
#include <functional>
#include "buffer.h"
#include <assert.h>

bool HTTP_Connection::Connect( const std::string & ip,unsigned short port,HTTP_VERSION ver)
{
     if ( m_pConnection )
	 {
		 Close();
	 }

	 m_pConnection = CreateTCPConnection(0,this,NO_PACK);
	 if ( m_pConnection==0 )
		 return false;

	 m_ip = ip;
	 m_port = port;
	 m_version = ver;
     m_bConnected = false;

	 if ( !m_pConnection->Connect(m_ip.c_str(),m_port) )
		 return false;
     
	 return true;
}

bool HTTP_Connection::Request( const std::string & url,const std::string & host,__int64 range_begin,__int64 range_end,handle handler )
{
	REQUEST request;
	request.m_RangeBegin = range_begin;
	request.m_RangeEnd = range_end;
	request.m_szURL = url;
    request.m_bSended = false;
    request.m_pHander = handler;
    request.m_szServerHost = host;

	m_RequestList.push_back(request);
    
	if ( m_pConnection==0 )
	{
        Connect(m_ip,m_port,m_version);
	}

	SendAllRequest(false);

	return true;
}

void HTTP_Connection::SendAllRequest(bool resend)
{
	if ( m_pConnection==0 || m_bConnected==false )
		return;

	REQUEST_LIST::iterator it = m_RequestList.begin();
	for ( ;it!=m_RequestList.end();++it )
	{
		REQUEST & request = *it;

		if ( request.m_bSended==false || resend )
		{
			ostrbuf buff;

			if ( m_version==HTTP_VER_1_1)
			{
				buff << "GET " << request.m_szURL << " HTTP/1.1\r\n";
				buff << "Connection:Keep-Alive\r\n";
			}else
			{
				buff << "GET " << request.m_szURL << " HTTP/1.0\r\n";
				buff << "Connection:close\r\n";
			}
			
			buff << "Host:"<< request.m_szServerHost << "\r\n";
			
			buff << "Accept:*/*\r\n";
			buff << "Cache-Control: no-cache\r\n";
			buff << "User-Agent:Carfax(download engine)\r\n";
			if ( request.m_RangeBegin<request.m_RangeEnd)
			{
				buff << "Range:bytes=" << request.m_RangeBegin << "-" << request.m_RangeEnd-1 << "\r\n";
			}
            buff << "\r\n";

			m_pConnection->SendData(buff.c_str(),buff.length());
			request.m_bSended = true;
		}
	}
}

void HTTP_Connection::Close()
{
	if ( m_pHeaderBuffer )
	{
		DeallocateAsynIoBuffer(m_pHeaderBuffer);
		m_pHeaderBuffer = 0;
	}

	if ( m_pContentBuffer )
	{
		DeallocateAsynIoBuffer(m_pContentBuffer);
		m_pContentBuffer = 0;
	}

	if ( m_pConnection )
	{
		m_pConnection->Release();
		m_pConnection = 0;
	}
}

void HTTP_Connection::OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler )
{
	assert(false);
}

void HTTP_Connection::OnConnected( IConnection * conn )
{
	assert(conn==m_pConnection);

    m_bConnected = true;
	m_bServerActive = true;
	SendAllRequest(true);
}

void HTTP_Connection::OnDisconnected( IConnection * conn,DWORD reason )
{
    assert(m_pConnection==conn);

	m_bConnected = false;
	Close();

	// 如果是真正连不上则报错
	REQUEST_LIST::iterator it = m_RequestList.begin();
	for ( ;it!=m_RequestList.end(); )
	{
		REQUEST & request = *it;
		if ( request.m_bSended || !m_bServerActive)
		{
			HTTP_Response_Handler * handler = (HTTP_Response_Handler*)getHandleData(request.m_pHander);

			if ( handler )
			{
				handler->OnError(reason,request.m_szURL.c_str());
			}

			it = m_RequestList.erase(it);
			continue;
		}

		++it;
	}

	if ( !m_RequestList.empty())
	{
		// 如果是正常断开则重连
		Connect(m_ip,m_port,m_version);
	}
}

void HTTP_Connection::OnError( IConnection * conn,DWORD dwErrorCode )
{
	assert(m_pConnection==conn);

	ErrorLn("Connect HTTP server failed! ErrorLn code is " << dwErrorCode << " ip=" << m_ip << " port=" << m_port );

	m_bConnected = false;
	Close();

	// 如果是真正连不上则报错
	REQUEST_LIST::iterator it = m_RequestList.begin();
	for ( ;it!=m_RequestList.end(); )
	{
		REQUEST & request = *it;
		if ( request.m_bSended || !m_bServerActive)
		{
			HTTP_Response_Handler * handler = (HTTP_Response_Handler*)getHandleData(request.m_pHander);

			if ( handler )
			{
				handler->OnError(dwErrorCode,request.m_szURL.c_str());
			}

			it = m_RequestList.erase(it);
			continue;
		}

		++it;
	}

	if ( !m_RequestList.empty())
	{
		// 如果是正常断开则重连
		Connect(m_ip,m_port,m_version);
	}
}

bool HTTP_Connection::Handle_Response_State(char * state_line,DWORD len)
{
	char * start_p = state_line;

	for ( int i=0;i<len;++i )
	{
		if ( state_line[i]==' ')
		{
			state_line[i] = 0;

			// 解释版本号
			if ( start_p==state_line )
			{
				if ( stricmp(start_p,"HTTP/1.1")==0 )
				{
					m_version = HTTP_VER_1_1;
				}else if ( stricmp(start_p,"HTTP/1.0")==0 )
				{
					m_version = HTTP_VER_1_0;
				}

				start_p = state_line+i+1;
			}
			// 解释错码
			else
			{
				DWORD dwErrorCode = 0;
				sscanf(start_p,"%d",&dwErrorCode);
				if ( dwErrorCode>=400 && dwErrorCode<600 )
				{
					assert( !m_RequestList.empty());

					REQUEST & request = m_RequestList.front();
					HTTP_Response_Handler * handler = (HTTP_Response_Handler*)getHandleData(request.m_pHander);
                    if ( handler )
					    handler->OnError(dwErrorCode,request.m_szURL.c_str());
					return false;
				}
			}
		}
	}

	return true;
}

void HTTP_Connection::OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen )
{
    assert(m_pConnection==conn);

	if ( m_RequestList.empty())
		return;

	REQUEST & request = m_RequestList.front();
	char * p = (char *)pData;

	// 收取未完成的HTTP头
	if ( m_pHeaderBuffer!=0 )
	{
		char * pHeader = (char*)AllocateAsynIoBuffer(m_nHeaderLen+dwDataLen);
		memcpy(pHeader,m_pHeaderBuffer,m_nHeaderLen);
		memcpy(pHeader+m_nHeaderLen,pData,dwDataLen);

		DeallocateAsynIoBuffer(m_pHeaderBuffer);
		m_nHeaderLen = 0;
		OnRecv(conn,pHeader,m_nHeaderLen+dwDataLen);
		DeallocateAsynIoBuffer(pHeader);
		return;
	}

	// 收取上次未完成的包
	if ( m_nContentLen-m_nCurrentLen>0 )
	{
		int nMoveLen = min(m_nContentLen-m_nCurrentLen,dwDataLen);
		memcpy(m_pContentBuffer+m_nCurrentLen,pData,nMoveLen);
		m_nCurrentLen+=nMoveLen;
		p += nMoveLen;
		dwDataLen -= nMoveLen;

		if ( m_nContentLen==m_nCurrentLen )
		{
			HTTP_Response_Handler * handler = (HTTP_Response_Handler*)getHandleData(request.m_pHander);
			if ( handler )
			{
				handler->OnResponse(m_pContentBuffer,m_nContentLen,m_HttpHeader,request.m_szURL.c_str());
			}

			m_RequestList.pop_front();

			m_nContentLen = 0;
			m_nCurrentLen = 0;

			DeallocateAsynIoBuffer(m_pContentBuffer);
			m_pContentBuffer = 0;
		}else
		{
			return;
		}
	}
    
	// 收取新的数据包
	m_HttpHeader.clear();

	if ( p==0 || dwDataLen==0 )
		return;

	int key_pos = 0;
	int key_end = 0;
	int val_pos = 0;
	int val_end = 0;
	int entity_pos = -1;

	for ( int i=0;i<dwDataLen;++i )
	{
		if (p[i]=='\n')
		{
            val_end = i;
			
			// 第一行 HTTP/1.1 200 OK
			if ( key_pos==0 )
			{
				if (!Handle_Response_State(p,i))
					return;
			}
			// 后面是实体域了
			else if( val_pos==0 )
			{
				entity_pos = i+1;
				break;
			}
			// 把头域存储起来
			else
			{
				std::string key(p+key_pos,key_end-key_pos);
				std::string val(p+val_pos,val_end-val_pos);
				key = StringHelper::toLower(key);
				val = StringHelper::toLower(val);
				StringHelper::replace(val,'\r',' ');
				StringHelper::replace(val,'\n',' ');
				StringHelper::replace(val,'\n',' ');
				StringHelper::trim(key," ");
				StringHelper::trim(val," ");

				HTTP_Response_Header::value_type entry(key,val);
				m_HttpHeader.insert(entry);
			}

			key_pos = i+1;
			key_end = i+1;
			val_pos = 0;
			val_end = 0;
		}

		if ( p[i]==':' && val_pos==0 )
		{
			val_pos = i+1;
			key_end = i;
		}
	}

	// 包头都没收完
	if ( entity_pos<0 )
	{
		assert(m_pHeaderBuffer==0);
		assert(m_nHeaderLen==0);

		m_nHeaderLen = dwDataLen;
		m_pHeaderBuffer = (char *)AllocateAsynIoBuffer(m_nHeaderLen);
		memcpy(m_pHeaderBuffer,pData,m_nHeaderLen);
		return;
	}

	// 是否重定向
	HTTP_Response_Header::iterator it = m_HttpHeader.find("location");
	if ( it!=m_HttpHeader.end())
	{
		HTTP_Response_Handler * handler = (HTTP_Response_Handler*)getHandleData(request.m_pHander);
		if ( handler )
		{
			handler->OnLocation(it->second.c_str(),request.m_szURL.c_str());
		}		
		return ;
	}

	// 是否存在长度
	it = m_HttpHeader.find("content-length");
	if ( it!=m_HttpHeader.end())
	{
		int nContentLen = 0;
		sscanf( it->second.c_str(),"%d",&nContentLen);

		if ( nContentLen<=dwDataLen-entity_pos)
		{
			HTTP_Response_Handler * handler = (HTTP_Response_Handler*)getHandleData(request.m_pHander);
			if ( handler )
			{
				handler->OnResponse(p+entity_pos,nContentLen,m_HttpHeader,request.m_szURL.c_str());
			}

			m_RequestList.pop_front();

			int nLeaveLen = dwDataLen-entity_pos-nContentLen;
			if ( nLeaveLen>0 )
			{
				OnRecv(conn,p+entity_pos+nContentLen,nLeaveLen);
			}
		}else
		{
			assert(m_pContentBuffer==0);
			assert(m_nContentLen==0);
			assert(m_nCurrentLen==0);

			m_nContentLen = nContentLen;
			m_nCurrentLen = dwDataLen-entity_pos;
			m_pContentBuffer = (char *)AllocateAsynIoBuffer(m_nContentLen);
			memcpy(m_pContentBuffer,p+entity_pos,m_nCurrentLen);
		}
		return ;
	}

	// 暂不支持chunk数据的读取
	HTTP_Response_Handler * handler = (HTTP_Response_Handler*)getHandleData(request.m_pHander);
	if ( handler )
	{
		handler->OnError(-8,request.m_szURL.c_str());
	}

	m_RequestList.pop_front();
}

