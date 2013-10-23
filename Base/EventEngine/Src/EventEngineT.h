/*******************************************************************
** 文件名:	e:\CoGame\EventEngine\src\EventEngine\EventEngineT.h
** 版  权:	(C) 
** 
** 日  期:	2007/9/9  21:38
** 版  本:	1.0
** 描  述:	事件引擎模板
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include <hash_map>
#include <list>
#include <string>
using namespace std;
using namespace stdext;

// 发送最大层数
#define FIRE_MAX_LAYERNUM			20

// 引用最大层数
#define REF_MAX_LAYERNUM			5

// 事件key
struct __EventKey
{
	DWORD	dwSrcID;			// 发送源标识（UID中"序列号"部份）	
	BYTE	bSrcType;			// 发送源类型
	WORD	wEventID;			// 事件ID 
	BYTE	bReserve;			// 保留
};

struct SEventKey
{
	union
	{
		__EventKey			_key;
		LONGLONG			_value;
	};

	SEventKey(void)
	{
		_value = 0;
	}

	bool operator == (const SEventKey &eventkey) const
	{
		return _value == eventkey._value;
	}

	bool operator < (const SEventKey &eventkey) const
	{
		return _value < eventkey._value;
	}
};

// 特化hash函数
namespace stdext
{
	inline size_t hash_value(const SEventKey &eventkey)
	{
		DWORD k1 = (eventkey._key.dwSrcID & 0xFFFF) << 16;
		DWORD k2 = (eventkey._key.wEventID & 0xFF) << 8;
		DWORD k3 = eventkey._key.bSrcType;
		return k1 + k2 + k3;
	}
}

// 事件引擎模板
template< class TSink, class TOnEventObject >
class TEventEngine
{	
private:	
	struct SSubscibeInfo
	{
		TSink *		pSink;
		char		szDesc[32];
		int			nCallCount;
		bool		bRemoveFlag;

		SSubscibeInfo(TSink * pPrameSink, LPCSTR pDesc)
		{
			pSink = pPrameSink;		
			nCallCount = 0;
			bRemoveFlag = false;
			if(pDesc != NULL)
			{
				lstrcpyn(szDesc, pDesc, sizeof(szDesc));
			}
			else
			{
				szDesc[0] = '\0';
			}
		}

		void Add(void)
		{
			nCallCount++;
		}

		void Sub(void)
		{
			nCallCount--;
		}
	};
	// 一个事件KEY的订阅者列表
	typedef std::list< SSubscibeInfo >							TLIST_SUBSCIBEINFO;

	// 所有事件KEY的订阅者列表
	typedef stdext::hash_map<SEventKey, TLIST_SUBSCIBEINFO >	TMAP_SAFESINK;
public:
	/** 
	@param   
	@param   
	@return  
	*/
	TEventEngine()
	{
		m_nFireLayerNum = 0;
	}

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~TEventEngine()
	{
		typename TMAP_SAFESINK::iterator it = m_mapSafeSink.begin();
		for( ; it != m_mapSafeSink.end(); ++it)
		{
			TLIST_SUBSCIBEINFO * plisSubscibeInfo = &((*it).second);
			plisSubscibeInfo->clear();
		}

		m_mapSafeSink.clear();
	}

	/** 
	@param   
	@param   
	@return  
	*/
	bool Subscibe(TSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc)	
	{
		if(pSink == NULL)
		{
			return false;
		}

		// 事件KEY
		SEventKey eventkey;
		eventkey._key.wEventID = wEventID;
		eventkey._key.bSrcType = bSrcType;
		eventkey._key.dwSrcID = dwSrcID;

		// 订阅者信息
		SSubscibeInfo subscibeinfo(pSink, pszDesc);

		// 加入到订阅列表
		typename TMAP_SAFESINK::iterator it = m_mapSafeSink.find(eventkey);
		if(it == m_mapSafeSink.end())
		{
			TLIST_SUBSCIBEINFO listSubscibeInfo;
			listSubscibeInfo.push_front(subscibeinfo);

			// 加入到订阅列表					
			m_mapSafeSink.insert(TMAP_SAFESINK::value_type(eventkey, listSubscibeInfo));
		}
		else
		{
			TLIST_SUBSCIBEINFO * plistSubscibeInfo = &((*it).second);
			plistSubscibeInfo->push_front(subscibeinfo);
		}

		return true;		
	}

	/** 
	@param   
	@param   
	@return  
	*/
	bool UnSubscibe(TSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID)
	{
		if(pSink == NULL)
		{
			return false;
		}	

		SEventKey eventkey;
		eventkey._key.wEventID = wEventID;
		eventkey._key.bSrcType = bSrcType;
		eventkey._key.dwSrcID = dwSrcID;

		typename TMAP_SAFESINK::iterator it = m_mapSafeSink.find(eventkey);
		if(it != m_mapSafeSink.end())
		{
			TLIST_SUBSCIBEINFO * plistSubscibeInfo = &((*it).second);

			typename TLIST_SUBSCIBEINFO::iterator itList = plistSubscibeInfo->begin();
			for( ; itList != plistSubscibeInfo->end(); ++itList)
			{
				SSubscibeInfo * pSubscibeInfo = &(*itList);
				if(pSubscibeInfo->pSink == pSink)
				{
					if(pSubscibeInfo->nCallCount == 0)
					{
						plistSubscibeInfo->erase(itList);
					}
					else
					{
						pSubscibeInfo->bRemoveFlag = true;
					}

					break;
				}
			}				
		}

		return true;	
	}

	/** 
	@param   
	@param   
	@return  
	*/
	bool Fire(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		SEventKey eventkey;
		eventkey._key.wEventID = wEventID;
		eventkey._key.bSrcType = bSrcType;

		// 先发送有源指针的
		eventkey._key.dwSrcID = dwSrcID;
		if(eventkey._key.dwSrcID != 0)
		{
			bool bResult = Fire(eventkey, wEventID, bSrcType, dwSrcID, pszContext, nLen);
			if(!bResult) 
			{
				return false;
			}
		}
		
		// 然后发送源指针的
		eventkey._key.dwSrcID = 0;
		bool bResult = Fire(eventkey, wEventID, bSrcType, dwSrcID, pszContext, nLen);
		if(!bResult)
		{
			return false;
		}

		return true;
	}

private:
	/** 
	@param   
	@param   
	@return  
	*/
	bool Fire(SEventKey &eventkey, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		m_nFireLayerNum++;

		if(m_nFireLayerNum >= FIRE_MAX_LAYERNUM)
		{
			Error("事件服务器非法！死循环调用 EVENTID = " << wEventID << "SRCTYPE ＝ " << bSrcType << endl);
			memcpy(NULL, NULL, -1);
			m_nFireLayerNum--;
			return false;
		}

		typename TMAP_SAFESINK::iterator itMap = m_mapSafeSink.find(eventkey);
		if(itMap != m_mapSafeSink.end())
		{
			TLIST_SUBSCIBEINFO * plistSubscibeInfo = &((*itMap).second);
			
			typename TLIST_SUBSCIBEINFO::iterator itList = plistSubscibeInfo->begin();
			for( ; itList != plistSubscibeInfo->end(); )
			{
				SSubscibeInfo * pSubscibeInfo = &(*itList);

				if(pSubscibeInfo->nCallCount >= REF_MAX_LAYERNUM)
				{
					Error("事件服务器内同一事件循环调用！严重问题！EVENTID = " << wEventID << "描述 = " << pSubscibeInfo->szDesc << endl);
					m_nFireLayerNum--;
					return false;
				}

				if(!pSubscibeInfo->bRemoveFlag)
				{					
					bool bResult = false;

					// 非发布版本不捕获异常，让问题尽快暴露 [4/21/2011 zgz]
#if  !defined(RELEASEDEBUG) && !defined(_DEBUG)
					try
#endif
					{
						pSubscibeInfo->Add();
						bResult = m_FireEventObject(pSubscibeInfo->pSink, wEventID, bSrcType, dwSrcID, pszContext, nLen);
						pSubscibeInfo->Sub();						
					}
#if  !defined(RELEASEDEBUG) && !defined(_DEBUG)
					catch(...)
					{
						Error("事件服务器非法！EVENTID = " << wEventID << "描述 = " << pSubscibeInfo->szDesc << endl);
						return false;
					}
#endif

					if(pSubscibeInfo->bRemoveFlag && pSubscibeInfo->nCallCount == 0)
					{
						itList = plistSubscibeInfo->erase(itList);
					}					
					else
					{
						++itList;
					}

					if(!bResult) 
					{
						m_nFireLayerNum--;
						return false;
					}
				}
				else
				{
					if(pSubscibeInfo->nCallCount == 0)				
					{
						itList = plistSubscibeInfo->erase(itList);
					}
					else
					{
						++itList;
					}
				}
			}
		}

		m_nFireLayerNum--;

		return true;
	}
	
private:
	// 事件发送对像
	TOnEventObject		m_FireEventObject;

	// 事件对像列表
	TMAP_SAFESINK		m_mapSafeSink;

	// 发送层数
	int					m_nFireLayerNum;	
};
