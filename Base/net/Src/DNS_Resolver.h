/*******************************************************************
** 文件名:	DNS_Resolver.h 
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

#ifndef __DNS_RESOLVER_H__
#define __DNS_RESOLVER_H__

#include "dns.h"
#include "Thread.h"
#include "Reactor.h"
#include "Singleton.h"
using namespace xs;

#include <list>
#include <map>

/**
@name : DNS解答者 (域名查询器)
@brief: 查询域名的过程是异步的,要等DNS服务器返回
*/
class DNS_Resolver : public EventHandler,public SingletonEx<DNS_Resolver>
{
public:
	/**
	@name         : 查询一个域名
	@brief        :
	@param domain : 待查询的域名
	@param queryer: 查询者,用于查询成功后的通知
	@note         : 在查询过程中该指针必须保持有效,如果该指针被释放,需要主动调用Cancel_DNS_Query取消查询
	@return       :
	*/
	virtual bool DNS_Query( const std::string & domain,DNS_Queryer * queryer );

	/**
	@name         : 取消一个域名查询请求
	@brief        :
	@param domain : 待查询的域名
	@param queryer: 查询者,用于查询成功后的通知
	@return       :
	*/
	virtual bool Cancel_DNS_Query( const std::string & domain,DNS_Queryer * queryer );

	/**
	@name         : 响应域名查询成功时触发的事件
	*/
	virtual void OnEvent( HANDLE event );


	virtual void Release() { delete this; }

	/**
	@name         : 获取下一个查询请求
	@brief        : 查询线程在空闲时调用,需要加锁
	@param  domain: 要查询的线程
	@return       : 如果没有请求了返回false
	*/
    bool GetNextDNSQuery(std::string & domain);

    /**
    @name         : 填加DNS服务器返回的结果
    @brief        : 因为是在域名查询线程返回的所以要加锁
	@param domain : 要查询的域名
    @param dwError: 错误码,例如域名不存在
	@param ipset  : 域名对应的IP列表
    @return       :
    */
	void AddDNSResult(const std::string & domain,DWORD dwError,const std::vector<std::string> & ipset);

	bool Create();

	void Close();

	DNS_Resolver() : m_hDNSEvent(0){}

	virtual ~DNS_Resolver() { Close();}

protected:
	struct DNS_Result  // DNS返回的结果
	{
		DWORD                    m_dwError;
		std::string              m_szDomain;
		std::vector<std::string> m_IPSet;
	};

	struct DNS_Queryer_List
	{
		std::list<DNS_Queryer*>  m_QueryerList;  
		BOOL                     m_bIsQuerying;
		DNS_Queryer_List() : m_bIsQuerying(FALSE){}
	};

	typedef   std::list<DNS_Result>   RESULT_LIST;
    typedef   std::map<std::string,DNS_Result>  HISTORY_LIST;
	typedef   std::map<std::string,DNS_Queryer_List> QUERY_LIST;

	HANDLE       m_hDNSEvent;          // DNS查询成功触发的事件
	RESULT_LIST  m_ResultList;         // 查询结果列表
	HISTORY_LIST m_HistoryList;        // 查询历史列表，避免重复查询
    QUERY_LIST   m_QueryList;          // 查询请求列表
    Mutex        m_ResultMutex;        // 结果列表锁
	Thread       m_QueryThread;        // 查询线程
};

/**
@name : 域名查询线程
@brief: 
*/
class DNS_Query_Thread : public IRunnable
{
public:
	virtual void run();
	virtual void release(){ delete this;}

	DNS_Query_Thread(DNS_Resolver * resolver) : m_pResolver(resolver){}

protected:
	DNS_Resolver   *    m_pResolver;
};

#endif//__DNS_RESOLVER_H__