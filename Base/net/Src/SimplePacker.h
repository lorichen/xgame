/*******************************************************************
** 文件名:	SimplePacker.h 
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

#ifndef __SIMPLE_PACKER_H__
#define __SIMPLE_PACKER_H__

#include "IPacker.h"
#include <list>

namespace xs{

#	define  HEAD_TYPE        WORD

	/**
	@name : 简单打包策略
	@brief: 打包格式: 数据长度(2Bytes) + 实际数据
	*/
	class SimplePacker : public IPacker
	{
	public:
		virtual void Pack( const char * pRawData,DWORD dwDataLen,std::vector<AsynIoBuffer> & OutBuff )
		{
			char * pHeadBuff = (char *)AllocateAsynIoBuffer(sizeof(HEAD_TYPE));
			*(HEAD_TYPE *)pHeadBuff = (HEAD_TYPE)dwDataLen;

			AsynIoBuffer buff;

			buff.buf = pHeadBuff;
			buff.len = sizeof(HEAD_TYPE);

			OutBuff.push_back(buff);

			buff.buf = (char *)pRawData;
			buff.len = dwDataLen;

			OutBuff.push_back(buff);
		}

		bool UnPack( const char * pData,DWORD dwLen,std::vector<AsynIoBuffer> & OutBuff  )
		{
			RecycleBuffer();

			while(dwLen>0)
			{
				if ( m_dwPendingLen>MAX_NET_PACKAGE_SIZE )
					return false;

				DWORD dwNeedLen = m_dwPendingLen-m_dwRecvedPos;
				if ( dwLen >= dwNeedLen )
				{
					if ( m_dwRecvedPos!=0 )
					{
						memcpy(m_pPendingBuff+m_dwRecvedPos,pData,dwNeedLen);
						UnPackOk(m_pPendingBuff,OutBuff);
					}else
					{
						UnPackOk(pData,OutBuff);
 					}
					dwLen -= dwNeedLen;
					pData += dwNeedLen;
				}else
				{
					if ( m_pPendingBuff==0 )  // 如果缓冲区还没准备好则先准备缓冲区
					{
						m_pPendingBuff = (char *)AllocateAsynIoBuffer(m_dwPendingLen);
					}
					memcpy(m_pPendingBuff+m_dwRecvedPos,pData,dwLen);
					m_dwRecvedPos += dwLen;
					dwLen = 0;
				}
			}

			return true;
		}

		// 数据收取成功
		void UnPackOk( const char * pData,std::vector<AsynIoBuffer> & OutBuff  )
		{
			// 当前是在收取包头
			if ( m_pPendingBuff==(char*)&m_dwPackageLen )
			{
				m_dwPendingLen = *(HEAD_TYPE*)pData;  // 开始收取包体
				m_pPendingBuff = 0;
				m_dwRecvedPos  = 0;
			}
			// 当前是在收取包体
			else 
			{
				AsynIoBuffer buff;                // 返回给应用层
				buff.buf = (char *)pData;
				buff.len = m_dwPendingLen;

				OutBuff.push_back(buff);

				if ( m_pPendingBuff )             // 开始收取包头
				{
					m_BuffRecycle.push_back(m_pPendingBuff);
				}

				m_pPendingBuff= (char *)&m_dwPackageLen;
				m_dwRecvedPos = 0;
				m_dwPendingLen= sizeof(HEAD_TYPE);
			}
		}

		virtual IPacker * Clone() { return new SimplePacker;}

		virtual void Release() { delete this; }

		SimplePacker() : m_pPendingBuff(0),m_dwPendingLen(0),m_dwRecvedPos(0),m_dwPackageLen(0)
		{
			m_pPendingBuff= (char *)&m_dwPackageLen; // 第一个包是要收包头
			m_dwPendingLen= sizeof(HEAD_TYPE);
		}

		virtual ~SimplePacker() 
		{
			RecycleBuffer();

			m_dwPendingLen = 0;
			m_dwRecvedPos  = 0;
			m_dwPackageLen = 0;
		}

		inline void RecycleBuffer()
		{
			if ( !m_BuffRecycle.empty() )
			{
				std::list<char*>::iterator it = m_BuffRecycle.begin();
				for ( ;it!=m_BuffRecycle.end();++it )
				{
					DeallocateAsynIoBuffer(*it);
				}

				m_BuffRecycle.clear();
			}
		}

	protected:
		char       *       m_pPendingBuff;   // 还没有收齐的数据,可能是包长度也可能是数据
		DWORD              m_dwPendingLen;   // 本次总共需要收取的字节数
		DWORD              m_dwRecvedPos;    // 本次已经收取的字节数
		HEAD_TYPE          m_dwPackageLen;   // 包长度
		std::list<char*>   m_BuffRecycle;    // 回收临时缓冲区
	};
};

#endif//__SIMPLE_PACKER_H__