/*******************************************************************
** 文件名:	DataPacker.h 
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

#ifndef __DATA_PACKER_H__
#define __DATA_PACKER_H__

#include "IPacker.h"
#include "Crypt.H"
#include <list>





namespace xs{


    #	define  HEAD_TYPE        WORD
	typedef ulong (*SetSessionKey_T)(uchar*, ulong);


	/**
	@name : 简单打包策略
	@brief: 打包格式: 数据长度(2Bytes) + 实际数据
	*/
	class DataPacker : public IPacker
	{


	public:

		ulong SetSessionKey(ulong id,uchar * key,ulong keylen);

		ulong Set_rc4_key(uchar keybuf[],ulong keylen);

		ulong Set_tea_key(uchar keybuf[],ulong keylen);


		ulong Set_blowfish_key(uchar keybuf[],ulong keylen);
		virtual void Pack( const char * pRawData,DWORD dwDataLen,std::vector<AsynIoBuffer> & OutBuff );

		bool UnPack( const char * pData,DWORD dwLen,std::vector<AsynIoBuffer> & OutBuff  );

		// 数据收取成功
		void UnPackOk( const char * pData,std::vector<AsynIoBuffer> & OutBuff  );

		virtual IPacker * Clone() { return new DataPacker;}

		virtual void Release() { delete this; }

		DataPacker() : m_pPendingBuff(0),m_dwPendingLen(0),m_dwRecvedPos(0),m_dwPackageLen(0)
		{
			
			m_dwCryptId=0;
			memset(key,0,sizeof(key));
			keysize=16;
			static unsigned char keybuf[16] = {
				0x92,0x11,0x23,0xC9,0xD0,0x66,0x5C,0xC6,0xD2,0xA3,0x94,0xEB,0xF6,0x79,0x33,0x48
			};
			SetSessionKey(m_dwCryptId,(uchar *)keybuf,16);
			m_pPendingBuff= (char *)&m_dwPackageLen; // 第一个包是要收包头
			m_dwPendingLen= sizeof(HEAD_TYPE);
		}
		DataPacker(DWORD CryptId,uchar *keybuf,DWORD keylen) : m_pPendingBuff(0),m_dwPendingLen(0),m_dwRecvedPos(0),m_dwPackageLen(0)
		{
			memset(key,0,sizeof(key));
			m_dwCryptId=CryptId;
			keysize=keylen;
			SetSessionKey(CryptId,keybuf,keylen);
			m_pPendingBuff= (char *)&m_dwPackageLen; // 第一个包是要收包头
			m_dwPendingLen= sizeof(HEAD_TYPE);
		}





		virtual ~DataPacker() 
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

		DWORD              m_dwCryptId;      // 算法ID
		uchar              key[64];
		uint               keysize; 

		char              *m_pPendingBuff;   // 还没有收齐的数据,可能是包长度也可能是数据
		DWORD              m_dwPendingLen;   // 本次总共需要收取的字节数
		DWORD              m_dwRecvedPos;    // 本次已经收取的字节数
		HEAD_TYPE          m_dwPackageLen;   // 包长度
		std::list<char*>   m_BuffRecycle;    // 回收临时缓冲区
	};


};

#endif//__SIMPLE_PACKER_H__