/*******************************************************************
** 描  述:	包格式
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
//#ifdef WIN32
	#include "Stdafx.h"
//#else
//	#include "../StdAfx.h"
//#endif

#include "MpwFormat.h"


// 每个目录的散文件个数
#define DIR_MAX_FILE_COUNT				256

// 动画最长延时
#define ANI_MAX_DELAY					600000

///////////////////////////////////////////////////////////////////
// 包类型
enum
{
	PACKTYPE_UNKNOW = 0,								// 未知的
	PACKTYPE_ANI,										// 动画包
	PACKTYPE_MAX,										// 最多类
};


/** 构造函数
@param   
@param   
@return  
*/
PackFileHeader::PackFileHeader(void)
{
	memset(&m_PackFileHeader, 0, sizeof(SPackFileHeader));
	m_PackFileHeader.m_dwCode = 'WMTL';
	m_PackFileHeader.m_dwVersion = (1<<16 | 1<<0);
	m_PackFileHeader.m_dwPackType = PACKTYPE_ANI;
}

/** 构造函数
@param   
@param   
@return  
*/
bool PackFileHeader::Open(xs::Stream * pStream)
{
	memset(&m_PackFileHeader, 0, sizeof(SPackFileHeader));

	if(pStream == NULL)
	{
		return false;
	}

	// 移动末
	pStream->seek(0, SEEK_END);

	// 取得长度
	long lLen = pStream->getPosition();

	long length = pStream->getLength();

	long pl = sizeof(SPackFileHeader);

	// 看长度是否会小于包文件
	if(lLen < sizeof(SPackFileHeader))
	{
		return false;
	}

	// 移动文件首
	pStream->seek(0, SEEK_SET);

	// 读取数据
	if(!pStream->read(&m_PackFileHeader, sizeof(SPackFileHeader)))
	{
		return false;
	}

	// 数据验证
	if(m_PackFileHeader.m_dwVersion != (1<<16 | 1<<0) || m_PackFileHeader.m_dwCode != 'WMTL' || 
		m_PackFileHeader.m_dwPackType < PACKTYPE_UNKNOW || m_PackFileHeader.m_dwPackType >= PACKTYPE_MAX ||
		m_PackFileHeader.m_dwTotalFrame <= 0 || m_PackFileHeader.m_dwTotalFrame > DIR_MAX_FILE_COUNT || 
		m_PackFileHeader.m_dwPlayDelay <= 0 || m_PackFileHeader.m_dwPlayDelay > ANI_MAX_DELAY)
	{
		return false;
	}

	return true;
}

/** 设置压缩标志
@param   
@param   
@return  
*/
void PackFileHeader::SetCompressFlag(DWORD dwFlag)
{
	m_PackFileHeader.m_dwCompress = dwFlag;
}


/** 取得压缩标志
@param   
@param   
@return  
*/
DWORD PackFileHeader::GetCompressFlag(void)
{
	return m_PackFileHeader.m_dwCompress;
}

/** 设置加密标志
@param   
@param   
@return  
*/
void PackFileHeader::SetEncryptFlag(DWORD dwFlag)
{
	m_PackFileHeader.m_dwEncrypt = dwFlag;
}

/** 取得加密标志
@param   
@param   
@return  
*/
DWORD PackFileHeader::GetEncryptFlag(void)
{
	return m_PackFileHeader.m_dwEncrypt;
}

/** 设置标识符
@param   
@param   
@return  
*/
void PackFileHeader::SetPackCaption(LPCSTR pszCaption)
{
	//if(strlen(m_PackFileHeader.m_szCaption) == 0)
	//{
	//	lstrcpyn(m_PackFileHeader.m_szCaption, pszCaption, sizeof(m_PackFileHeader.m_szCaption));
	//}
}

/** 取得标识符
@param   
@param   
@return  
*/
LPCSTR PackFileHeader::GetPackCaption(void)
{
	//return m_PackFileHeader.m_szCaption;
	return "";
}

/** 设置包类型
@param   
@param   
@return  
*/
void PackFileHeader::SetPackType(DWORD dwPackType)
{
	m_PackFileHeader.m_dwPackType = dwPackType;
}

/** 获得包类型
@param   
@param   
@return  
*/
DWORD PackFileHeader::GetPackType(void)
{
	return m_PackFileHeader.m_dwPackType;
}

/** 设置后面Zip长度
@param   
@param   
@return  
*/
void PackFileHeader::SetZipLen(DWORD dwZipLen)
{
	m_PackFileHeader.m_dwZipLen = dwZipLen;
}

/** 获得后面Zip长度
@param   
@param   
@return  
*/
DWORD PackFileHeader::GetZipLen(void)
{
	return m_PackFileHeader.m_dwZipLen;
}

/** 取得结构大小
@param   
@param   
@return  
*/
int PackFileHeader::GetStructSize(void)
{
	return sizeof(m_PackFileHeader);
}

/** 取得保留字节
@param   
@param   
@return  
*/
char * PackFileHeader::GetReserve(void)
{
	//return m_PackFileHeader.m_szReserve;
	return 0;
}

/** 设置保留字节
@param   
@param   
@return  
*/
void PackFileHeader::SetReserve(char * pszReserve, int nLen)
{
	//memcpy(m_PackFileHeader.m_szReserve, pszReserve, nLen);
}

/** 文件格式
@param   
@param   
@return  
*/
void PackFileHeader::SetFileFormat(LPCSTR szFileFormat)
{
	//lstrcpyn(m_PackFileHeader.m_szFileFormat, szFileFormat, sizeof(m_PackFileHeader.m_szFileFormat));
    strncpy(m_PackFileHeader.m_szFileFormat, szFileFormat, sizeof(m_PackFileHeader.m_szFileFormat));
}

/** 文件格式
@param   
@param   
@return  
*/
LPCSTR PackFileHeader::GetFileFormat(void)
{
	return m_PackFileHeader.m_szFileFormat;
}

/** 总动画帧数
@param   
@param   
@return  
*/
void PackFileHeader::SetTotalFrame(DWORD dwTotalFrame)
{
	m_PackFileHeader.m_dwTotalFrame = dwTotalFrame;	
}

/** 总动画帧数
@param   
@param   
@return  
*/
DWORD PackFileHeader::GetTotalFrame(void)
{
	return m_PackFileHeader.m_dwTotalFrame;
}

/** 动画播放延时
@param   
@param   
@return  
*/
void PackFileHeader::SetPlayDelay(DWORD dwPlayDelay)
{
	m_PackFileHeader.m_dwPlayDelay = dwPlayDelay;
}

/** 动画播放延时
@param   
@param   
@return  
*/
DWORD PackFileHeader::GetPlayDelay(void)
{
	return m_PackFileHeader.m_dwPlayDelay;
}

/** 构造函数
@param   
@param   
@return  
*/

FrameInfoHeader::FrameInfoHeader(void)
{
	memset(&m_FrameInfoHeader, 0, sizeof(SFrameInfoHeader));
}

/** 构造函数
@param   
@param   
@return  
*/
bool FrameInfoHeader::Open(xs::Stream * pStream)
{
	memset(&m_FrameInfoHeader, 0, sizeof(SFrameInfoHeader));	

	if(pStream == NULL)
	{
		return false;
	}

	// 取得当前位置
	long lOldPos = pStream->getPosition();

	// 移动文件末
	pStream->seek(0, SEEK_END);

	// 取得文件长度
	long lNewLen = pStream->getPosition();

	int len = pStream->getLength();

	// 看文件长度是否会小于包文件
	if((lNewLen - lOldPos) <= sizeof(SFrameInfoHeader))
	{
		return false;
	}

	// 移动文件到原来位值
	pStream->seek(lOldPos, SEEK_SET);

	// 读取数据头
	if(!pStream->read(&m_FrameInfoHeader, sizeof(SFrameInfoHeader)))
	{
		return false;
	}

	if(m_FrameInfoHeader.m_nImageSize > 0)
	{
		// 取得当前位置
		lOldPos = pStream->getPosition();

		// 移动文件末
		pStream->seek(0, SEEK_END);

		// 取得文件长度
		lNewLen = pStream->getPosition();

		// 移动文件到原来位值
		pStream->seek(lOldPos, SEEK_SET);

		// 看文件长度是否会小于包文件
		if((lNewLen - lOldPos) <= sizeof(m_FrameInfoHeader.m_nImageSize))
		{
			return false;
		} 

		// 读取图片数据
		char * pImageData = new char [m_FrameInfoHeader.m_nImageSize];		
		if(!pStream->read(pImageData, m_FrameInfoHeader.m_nImageSize))
		{
			return false;
		}

		m_streamImageData.attach((uchar *)pImageData, m_FrameInfoHeader.m_nImageSize);

		pImageData = NULL;
	}

	return true;
}

/** 释放
@param   
@param   
@return  
*/
void FrameInfoHeader::Release(void)
{
	if(m_streamImageData.getBuffer() != NULL)
	{
		uchar * pBuff = m_streamImageData.detach();
		safeDeleteArray(pBuff);
	}

	delete this;
}

/** 关闭
@param   
@param   
@return  
*/
void FrameInfoHeader::Close(void)
{
	if(m_streamImageData.getBuffer() != NULL)
	{
		uchar * pBuff = m_streamImageData.detach();
		safeDeleteArray(pBuff);
	}
}

/** 设置图片大小
@param   
@param   
@return  
*/
void FrameInfoHeader::SetImageSize(int nWidth, int nHeight)
{
	m_FrameInfoHeader.m_nWidth = nWidth;
	m_FrameInfoHeader.m_nHeight = nHeight;
}

/** 取得图片大小
@param   
@param   
@return  
*/
void FrameInfoHeader::GetImageSize(int &nWidth, int &nHeight)
{
	nWidth = m_FrameInfoHeader.m_nWidth;
	nHeight = m_FrameInfoHeader.m_nHeight;
}

void FrameInfoHeader::GetImageOffset(int &nX,int &nY)
{
	nX = m_FrameInfoHeader.m_nOffsetX;
	nY = m_FrameInfoHeader.m_nOffsetY;
}

/** 取得图片数据
@param   
@param   
@return  
*/
xs::Stream * FrameInfoHeader::GetImageData(int &nImageSize)
{
	nImageSize = m_FrameInfoHeader.m_nImageSize;

	return &m_streamImageData;
}