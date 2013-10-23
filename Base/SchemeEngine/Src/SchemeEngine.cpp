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
#include "stdafx.h"
#include "SchemeEngine.h"

CSchemeEngine *	g_pSchemeEngine = NULL;
/** 构造函数
@param   
@param   
@return  
*/
CSchemeEngine::CSchemeEngine(void)
{
	m_hashSchemeSink.clear();

	m_nCallNum = 0;

	m_bIsChange = false;

	m_listDynamicScheme.clear();
}

/** 析构函数
@param   
@param   
@return  
*/
CSchemeEngine::~CSchemeEngine(void)
{
	
}

/** 释放配置引擎
@param   
@param   
@param   
@return  
@note     
@warning 
@retval buffer 
*/	
void CSchemeEngine::Release(void)
{
	m_hashSchemeSink.clear();

	delete this;
	//全局变量是魔鬼!
	assert (this == g_pSchemeEngine);
	g_pSchemeEngine = NULL; 
}

/** 载入配置,会自动注册sink
@param   szFileName：配置文件名
@param   pSink：配置更新回调接口
@param   
@return  成功返回true，否则为false
@note     
@warning 
@retval buffer 
*/
bool CSchemeEngine::LoadScheme(const char * pszFileName, ISchemeUpdateSink * pSink, bool bDynamic)
{
	if(pszFileName == NULL || pSink == NULL || strlen(pszFileName) <= 4)
	{
		return false;
	}

	// 注意为了提高效率和避免太多麻烦，这里不能被嵌套调用，也就是在回调中再装载配置.
	if(m_nCallNum > 0)
	{
		memcpy(NULL, NULL, -1);
	}

	char szFullFileName[MAX_PATH] = { 0 };
	strncpy(szFullFileName, pszFileName, sizeof(szFullFileName));

	// 将文件名全部转成大写
	strlwr(szFullFileName);

	// 注册sink
	if(!RegisterSink(szFullFileName, pSink, bDynamic))
	{
		return false;
	}

	// 载入脚本
	CCsvReader csvreader;
	TiXmlDocument tixmldocument;
	bool bIsCsv = false;
	if(!__Load(szFullFileName, &csvreader, &tixmldocument, bIsCsv))
	{
		UnregisterSink(szFullFileName, pSink);
		return false;
	}

	//  计数加一
	m_nCallNum++;

	// 调sink
	THASH_SCHEMESINK::iterator it = m_hashSchemeSink.find(szFullFileName);
	if(it != m_hashSchemeSink.end())
	{
		SSINK * pSSink = &((*it).second);
		if(pSSink != NULL)
		{
			TLIST_SINK::iterator itList = pSSink->listSink.begin();
			for( ; itList != pSSink->listSink.end(); ++itList)
			{
				ISchemeUpdateSink * __pSink = (*itList);
				if(bIsCsv)
				{
					if(!__pSink->OnSchemeLoad(&csvreader, pszFileName))
					{
						// 计数减一
						m_nCallNum--;

						return false;
					}
				}
				else
				{
					if(!__pSink->OnSchemeLoad(&tixmldocument, pszFileName))
					{
						// 计数减一
						m_nCallNum--;

						return false;
					}
				}
			}
		}
	}	

	// 计数减一
	m_nCallNum--;

	// 修改了
	m_bIsChange = true;

	return true;
}

/** 更新配置
@param   szFileName：配置文件名
@param   
@param   
@return  成功返回true，否则为false
@note     
@warning 
@retval buffer 
*/
bool CSchemeEngine::UpdateScheme(const char * pszFileName)
{
	if(pszFileName == NULL)
	{
		return false;
	}

	// 不能被嵌套调用，也就是在回调中再装载配置.
	if(m_nCallNum > 0)
	{
		memcpy(NULL, NULL, -1);
	}

	char szFullFileName[MAX_PATH] = { 0 };
	strncpy(szFullFileName, pszFileName, sizeof(szFullFileName));

	// 将文件名全部转成大写
	strlwr(szFullFileName);

	// 看是否有此sink
	THASH_SCHEMESINK::iterator it = m_hashSchemeSink.find(szFullFileName);
	if(it == m_hashSchemeSink.end())
	{
		return false;
	}

	SSINK * pSSink = &((*it).second);
	if(pSSink == NULL || !pSSink->bDynamic)
	{
		return false;
	}

	// 载入脚本
	CCsvReader csvreader;
	TiXmlDocument tixmldocument;
	bool bIsCsv = false;
	if(!__Load(szFullFileName, &csvreader, &tixmldocument, bIsCsv))
	{
		return false;
	}

	//  计数加一
	m_nCallNum++;

	// 调sink
	TLIST_SINK::iterator itList = pSSink->listSink.begin();
	for( ; itList != pSSink->listSink.end(); ++itList)
	{
		ISchemeUpdateSink * __pSink = (*itList);
		if(bIsCsv)
		{
			if(!__pSink->OnSchemeUpdate(&csvreader, pszFileName))
			{
				// 计数减一
				m_nCallNum--;

				return false;
			}
		}
		else
		{
			if(!__pSink->OnSchemeUpdate(&tixmldocument, pszFileName))
			{
				// 计数减一
				m_nCallNum--;

				return false;
			}
		}
	}

	// 计数减一
	m_nCallNum--;

	return true;
}

/** 取消注册配置回调接口
@param   szFileName：配置文件名
@param   pSink：配置更新回调接口
@param   
@return  成功返回true，否则为false
@note     
@warning 
@retval buffer 
*/
bool CSchemeEngine::UnregisterSink(const char * szFileName, ISchemeUpdateSink * pSink)
{
	char szFullFileName[MAX_PATH] = { 0 };
	strncpy(szFullFileName, szFileName, sizeof(szFullFileName));

	// 将文件名全部转成大写
	strlwr(szFullFileName);
	// 看是否已存在
	THASH_SCHEMESINK::iterator it = m_hashSchemeSink.find(szFullFileName);
	if(it != m_hashSchemeSink.end())
	{
		SSINK * pSchemeSink = &((*it).second);
		pSchemeSink->listSink.remove(pSink);
	}

	return true;
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeEngine::__Load(const char * szFileName, CCsvReader * pCSVReader, TiXmlDocument * pTiXmlDocument, bool &bIsCsv)
{
	int nLen = strlen(szFileName);
	if(nLen <= 4)
	{
		return false;
	}

	const char * pExternName = szFileName + nLen - 3;
	if(stricmp(pExternName, "CSV") == 0)	
	{	
		bIsCsv = true;
		return __LoadCSV(szFileName, false, pCSVReader);		
	}
	else if(stricmp(pExternName, "CSC") == 0)
	{
		bIsCsv = true;
		return __LoadCSV(szFileName, true, pCSVReader);		
	}
	else if(stricmp(pExternName, "XML") == 0)
	{
		bIsCsv = false;
		return __LoadXML(szFileName, false, pTiXmlDocument);		
	}
	else if(stricmp(pExternName, "XMC") == 0)
	{
		bIsCsv = false;
		return __LoadXML(szFileName, true, pTiXmlDocument);		
	}	

	return false;
}

/** 载入CSV配置
@param   
@param   
@return  
*/
bool CSchemeEngine::__LoadCSV(const char * szFileName, bool bEncrypt, CCsvReader * pCSVReader)
{
	if(szFileName == NULL || pCSVReader == NULL)
	{
		return false;
	}

	return pCSVReader->Open(szFileName, bEncrypt);
}

/** 载入XML配置
@param   
@param   
@return  
*/
bool CSchemeEngine::__LoadXML(const char * szFileName, bool bEncrypt, TiXmlDocument * pTiXmlDocument)
{
	if(szFileName == NULL || pTiXmlDocument == NULL)
	{
		return false;
	}

	// 取得文件系统
	FileSystem * pFileSystem = getFileSystem();
	if(pFileSystem == NULL)
	{
		return false;
	}

	// 读取文件
	CStreamHelper stream = pFileSystem->open(szFileName);
	if(stream.isNull())
	{
		return false;
	}

	int nFileLength = stream->getLength();	

	char * pFileBuffer = new char [nFileLength + 1];
	if(!stream->read(pFileBuffer, nFileLength))
	{
		return false;
	}
	stream.close();
	
	// 解密了文件XML
	if(bEncrypt)
	{
		if(!makeMap((uchar *)pFileBuffer, nFileLength, 'LAND'))
		{
			return false;
		}
	}	

	// 交给TiXmlDocument去解析
	pTiXmlDocument->Parse(pFileBuffer);

	delete [] pFileBuffer;
	pFileBuffer = NULL;

	return true;
}

/** 注册sink
@param   
@param   
@return  
*/
bool CSchemeEngine::RegisterSink(const char * szFileName, ISchemeUpdateSink * pSink, bool bDynamic)
{
	if(szFileName == NULL || pSink == NULL)
	{
		return false;
	}
	string stringFileName = szFileName;

	// 看是否已存在
	THASH_SCHEMESINK::iterator it = m_hashSchemeSink.find(stringFileName);
	if(it != m_hashSchemeSink.end())
	{
		SSINK * pSchemeSink = &((*it).second);
		pSchemeSink->listSink.push_back(pSink);
		pSchemeSink->listSink.unique();
	}
	else
	{
		SSINK sink;
		sink.bDynamic = bDynamic;
		strncpy(sink.szFileName, szFileName, sizeof(sink.szFileName));
		sink.listSink.push_back(pSink);

		// 添加到LIST中去
		//string stringFileName = szFileName;
		m_hashSchemeSink[stringFileName] = sink;
	}

	return true;
}

/** 取得所有可动态更新的配置
@param   
@param   
@return  
*/
list<string> * CSchemeEngine::GetDynamicSchemeFileName(void)
{
	if(!m_bIsChange)
	{
		return &m_listDynamicScheme;
	}
	m_bIsChange = false;

	// 可动态更新的配置
	m_listDynamicScheme.clear();

	// 遍历
	THASH_SCHEMESINK::iterator it = m_hashSchemeSink.begin();
	for( ; it != m_hashSchemeSink.end(); ++it)
	{
		SSINK * pSchemeSink = &((*it).second);
		if(pSchemeSink != NULL && pSchemeSink->bDynamic)
		{
			string str = pSchemeSink->szFileName;
			m_listDynamicScheme.push_back(str);
		}
	}

	return &m_listDynamicScheme;
}

/** 输出函数
@param   
@param
@return  
*/
API_EXPORT ISchemeEngine * LoadSEM(void)
{
	// 如果创建了，则直接返回
	if(g_pSchemeEngine != NULL)
	{
		return g_pSchemeEngine;
	}

	// 实例化声音引擎
	g_pSchemeEngine = new CSchemeEngine();
	if(g_pSchemeEngine == NULL)
	{
		return NULL;
	}	

	return g_pSchemeEngine;
}