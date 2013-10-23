/*******************************************************************
** 文件名:	DNS_Resolver.cpp 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	02/11/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "DNS_Resolver.h"
#include "AsynIoFrame.h"
#include <algorithm>
#include <assert.h>

bool DNS_Resolver::Create()
{
#	ifdef WIN32
	m_hDNSEvent = ::CreateEvent(0,FALSE,FALSE,0);
#	else
#	pragma WarningLn("warning!!!!! it is not windows platform,I did not know how to create event.");
#   endif//WIN32

	// 向反应器注册事件
	Reactor * g_reactor = GetAsynIoReactor();
	if ( g_reactor==0 )
	{
		ErrorLn("Get reactor failed!Please call function 'InitializeNetwork' to initialize network.");
		return false;
	}

	g_reactor->AddEvent(m_hDNSEvent);
	g_reactor->RegisterEventHandler(m_hDNSEvent,this);

    return m_QueryThread.spawn(new DNS_Query_Thread(this));
}

void DNS_Resolver::Close()
{
    if ( m_hDNSEvent )
	{
		// 从反应器移除
		Reactor * g_reactor = GetAsynIoReactor();
		if ( g_reactor )
		{
			g_reactor->RemoveEvent(m_hDNSEvent);
		}

#	ifdef WIN32
		::CloseHandle(m_hDNSEvent);
#	else
#	pragma WarningLn("warning!!!!! it is not windows platform,I did not know how to close event.");
#	endif//WIN32
		m_hDNSEvent = 0;
	}
}

bool DNS_Resolver::DNS_Query( const std::string & domain,DNS_Queryer * queryer )
{
	// 看看是否已经查询过
	HISTORY_LIST::iterator it = m_HistoryList.find(domain);
	if ( it!=m_HistoryList.end() )
	{
		const DNS_Result & result = it->second;
		
		if ( result.m_dwError==0 && !result.m_IPSet.empty() )
		{
			std::vector<const char *> ipList;
			unsigned int nIPNum = result.m_IPSet.size();
			for( int i=0;i<nIPNum;++i )
			{
				ipList.push_back(result.m_IPSet[i].c_str());
			}
            queryer->Handle_DNS_Response(domain.c_str(),&ipList[0],nIPNum);
		}
		else
		{
            queryer->Handle_DNS_Error(domain.c_str(),result.m_dwError);
		}

		return true;
	}

	// 放到查询列表
    ResGuard<Mutex>  guard(m_ResultMutex);
	m_QueryList[domain].m_QueryerList.push_back(queryer);
	return true;
}

bool DNS_Resolver::Cancel_DNS_Query( const std::string & domain,DNS_Queryer * queryer )
{
    ResGuard<Mutex>  guard(m_ResultMutex);

	std::list<DNS_Queryer *> & query_list = m_QueryList[domain].m_QueryerList;
	std::list<DNS_Queryer *>::iterator it = std::find(query_list.begin(),query_list.end(),queryer);
	if ( it!=query_list.end() )
	{
         m_QueryList[domain].m_QueryerList.erase(it);
		 return true;
	}

	return false;
}

void DNS_Resolver::OnEvent( HANDLE event )
{
     ResGuard<Mutex>  guard(m_ResultMutex);

	 while(!m_ResultList.empty())
	 {
		DNS_Result & result = m_ResultList.front();
		m_HistoryList[result.m_szDomain]=result;

		QUERY_LIST::iterator pos = m_QueryList.find(result.m_szDomain);
		if ( pos==m_QueryList.end())
		{
			m_ResultList.pop_front();
			continue;
		}

		const std::list<DNS_Queryer *> & queryer_list = pos->second.m_QueryerList;

		std::list<DNS_Queryer *>::const_iterator it = queryer_list.begin();
		for ( ;it!=queryer_list.end();++it )
		{
			if ( result.m_dwError==0 && !result.m_IPSet.empty() )
			{
				std::vector<const char *> ipList;
				unsigned int nIPNum = result.m_IPSet.size();
				for( int i=0;i<nIPNum;++i )
				{
					ipList.push_back(result.m_IPSet[i].c_str());
				}
				(*it)->Handle_DNS_Response(result.m_szDomain.c_str(),&ipList[0],nIPNum);
			}
			else
			{
				(*it)->Handle_DNS_Error(result.m_szDomain.c_str(),result.m_dwError);
			}
		}

		m_QueryList.erase(pos);
		m_ResultList.pop_front();
		continue;
	 }
}

bool DNS_Resolver::GetNextDNSQuery(std::string & domain)
{
     ResGuard<Mutex>  guard(m_ResultMutex);

	 QUERY_LIST::iterator it = m_QueryList.begin();
	 for ( ;it!=m_QueryList.end();++it )
	 {
		 if ( it->second.m_bIsQuerying==FALSE )
		 {
			 it->second.m_bIsQuerying = TRUE;
			 domain = it->first;
			 return true;
		 }
	 }

	 return false;
}

void DNS_Resolver::AddDNSResult(const std::string & domain,DWORD dwError,const std::vector<std::string> & ipset)
{
     ResGuard<Mutex>  guard(m_ResultMutex);

	 DNS_Result result;
	 result.m_szDomain = domain;
	 result.m_dwError  = dwError;
	 result.m_IPSet    = ipset;

	 m_ResultList.push_back(result);

	 SetEvent(m_hDNSEvent);
}

void DNS_Query_Thread::run()
{
	while(m_pResolver)
	{
		 std::string domain;
		 if (m_pResolver->GetNextDNSQuery(domain))
		 {
			 std::vector<std::string> ipset;
             hostent * hostinfo = gethostbyname(domain.c_str());
			 if ( hostinfo )
			 {
				 for ( int i=0;;++i )
				 {
                      void * ip = hostinfo->h_addr_list[i];
					  
					  if ( ip==0 )
						  break;

					  assert(hostinfo->h_length==sizeof(in_addr));
					  in_addr addr = *(in_addr *)ip;
					  ipset.push_back(inet_ntoa(addr));
				 }

				 m_pResolver->AddDNSResult(domain,0,ipset);
			 }else
			 {
			     m_pResolver->AddDNSResult(domain,WSAGetLastError(),ipset);
			 }

		 }else
		 {
			 Sleep(10);
		 }
	}
}