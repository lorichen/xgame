/*******************************************************************
** 文件名:	e:\CoGame\SchemeEngine\src\SchemeEngine\SchemeEngine.h
** 版  权:	(C) 
** 
** 日  期:	2007/6/24  1:56
** 版  本:	1.0
** 描  述:	配置引擎
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "../include/tinyxml.h"
#include "CSVReader.h"
#include <string>
#include <list>
#include <vector>
#include <hash_map>
using namespace std;
using namespace stdext;

class CSchemeEngine : public ISchemeEngine
{
	typedef list< ISchemeUpdateSink * >		TLIST_SINK;
	struct SSINK
	{
		char		szFileName[MAX_PATH];
		bool		bDynamic;
		TLIST_SINK	listSink;		
	};
	
	typedef hash_map< string, SSINK >		THASH_SCHEMESINK;
public:
	//////////////////////////// ISchemeEngine ////////////////////////
	///////////////////////////////////////////////////////////////////
	/** 释放配置引擎
	@param   
	@param   
	@param   
	@return  
	@note     
	@warning 
	@retval buffer 
	*/	
	virtual void			Release(void);

	/** 载入配置,会自动注册sink
	@param   szFileName：配置文件名
	@param   pSink：配置更新回调接口
	@param   bDynamic:是否支持动态更新
	@return  成功返回true，否则为false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			LoadScheme(const char * pszFileName, ISchemeUpdateSink * pSink, bool bDynamic = true);

	/** 更新配置
	@param   szFileName：配置文件名
	@param   
	@param   
	@return  成功返回true，否则为false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			UpdateScheme(const char * pszFileName);

	/** 取消注册配置回调接口
	@param   szFileName：配置文件名
	@param   pSink：配置更新回调接口
	@param   
	@return  成功返回true，否则为false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			UnregisterSink(const char * pszFileName, ISchemeUpdateSink * pSink);

	/** 取得所有可动态更新的配置
	@param   
	@param   
	@return  
	*/
	virtual list<string> *	GetDynamicSchemeFileName(void);

	//////////////////////////// CSchemeEngine ////////////////////////
	///////////////////////////////////////////////////////////////////
	/** 构造函数
	@param   
	@param   
	@return  
	*/
	CSchemeEngine(void);

	/** 析构函数
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeEngine(void);

private:
	/** 注册sink
	@param   
	@param   
	@return  
	*/
	bool					RegisterSink(const char * szFileName, ISchemeUpdateSink * pSink, bool bDynamic);

	/** 载入配置
	@param   
	@param   
	@return  
	*/
	bool					__Load(const char * szFileName, CCsvReader * pCSVReader, TiXmlDocument * pTiXmlDocument, bool &bIsCsv);

	/** 载入CSV配置
	@param   
	@param   
	@return  
	*/
	bool					__LoadCSV(const char * szFileName, bool bEncrypt, CCsvReader * pCSVReader);

	/** 载入XML配置
	@param   
	@param   
	@return  
	*/
	bool					__LoadXML(const char * szFileName, bool bEncrypt, TiXmlDocument * pTiXmlDocument);

private:
	// 回调接口
	THASH_SCHEMESINK		m_hashSchemeSink;

	// 调用次数
	int						m_nCallNum;

	// 是否修改了
	bool					m_bIsChange;

	// 可动态更新的配置
	list<string>			m_listDynamicScheme;
};