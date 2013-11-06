/*******************************************************************
** 文件名:	e:\CoGame\2DEntityEditor\2DEntityEditor\PacketFormat.h
** 版  权:	(C) 
** 
** 日  期:	2007/7/24  15:54
** 版  本:	1.0
** 描  述:	包格式
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "stdafx.h"
#include "PacketFormat.h"


/** 构造函数
@param   
@param   
@return  
*/
CAniInfoHeader::CAniInfoHeader(void)
{

}

/** 打开
@param   pszMwdFileName ：为*.mwd文件名
@param   
@return  
*/
bool CAniInfoHeader::Open(LPCSTR pszMwdFileName)
{
	memset(&m_AniHeader, 0, sizeof(SAniHeader));
	if(pszMwdFileName == NULL)
	{
		return false;
	}

	xs::Stream * pStreamFile = xs::getFileSystem()->open(pszMwdFileName);
	if(pStreamFile == NULL)
	{		
		return false;
	}

	// 测算文件长度
	int nFileLen = pStreamFile->getLength();

	// 看文件长度是否会小于包文件
	if(nFileLen < sizeof(SAniHeader) - 4/*TILE信息指针长度*/)
	{
		pStreamFile->close();
		return false;
	}

	// 读取数据头
	int nDataSize = sizeof(SAniHeader) - 4/*TILE信息指针长度*/;
	if(!pStreamFile->read(&m_AniHeader, nDataSize))
	{
		pStreamFile->close();
		return false;
	}

	// 数据验证（版本号，标识码）
	if(m_AniHeader.m_dwVersion != (1 << 16 | 1 << 0) || m_AniHeader.m_dwCode != 'WMTL')
	{
		pStreamFile->close();
		return false;
	}

	// 数据验证
	if(m_AniHeader.m_nTileW < 0 || m_AniHeader.m_nTileH < 0)
	{	
		pStreamFile->close();
		return false;
	}

	// 读TILE信息
	DWORD dwTitleSize = m_AniHeader.m_nTileW * m_AniHeader.m_nTileH;
	if(dwTitleSize > 0)
	{
		// 取得当前位置
		int nCurPos = pStreamFile->getPosition();

		// 看文件长度是否会小于TITLE信息
		if((nFileLen - nCurPos) < (int)(dwTitleSize * sizeof(DWORD)))
		{
			pStreamFile->close();
			return false;
		}
	}

	m_AniHeader.m_pTileFlag = new DWORD[dwTitleSize];
	pStreamFile->read((uchar*)m_AniHeader.m_pTileFlag,dwTitleSize * sizeof(DWORD));

	// 关闭
	pStreamFile->close();

	return true;
}

/** 释放
@param   
@param   
@return  
*/
void CAniInfoHeader::Release(void)
{
	if(m_AniHeader.m_pTileFlag != NULL)
	{
		SAFE_DELETEARRAY(m_AniHeader.m_pTileFlag);
	}

	delete this;
}

/** 关闭
@param   
@param   
@return  
*/
void CAniInfoHeader::Close(void)
{
	if(m_AniHeader.m_pTileFlag != NULL)
	{
		SAFE_DELETEARRAY(m_AniHeader.m_pTileFlag);
	}
}

/** 锚点到图片左上角的偏移(X,Y)
@param   
@param   
@return  
*/
void CAniInfoHeader::SetAnchorOffset(int nX, int nY)
{
	m_AniHeader.m_nAnchorOffset[0] = nX;
	m_AniHeader.m_nAnchorOffset[1] = nY;
}

/** 锚点到图片左上角的偏移(X,Y)
@param   
@param   
@return  
*/
void CAniInfoHeader::GetAnchorOffset(int &nX, int &nY)
{
	nX = m_AniHeader.m_nAnchorOffset[0];
	nY = m_AniHeader.m_nAnchorOffset[1];
}

/** 排序点1偏移(X,Y)
@param   
@param   
@return  
*/
void CAniInfoHeader::SetSortPoint1(int nX, int nY)
{
	m_AniHeader.m_nSortPoint1[0] = nX;
	m_AniHeader.m_nSortPoint1[1] = nY;
}

/** 排序点1偏移(X,Y)
@param   
@param   
@return  
*/
void CAniInfoHeader::GetSortPoint1(int &nX, int &nY)
{
	nX = m_AniHeader.m_nSortPoint1[0];
	nY = m_AniHeader.m_nSortPoint1[1];
}

/** 排序点2偏移(X,Y)
@param   
@param   
@return  
*/
void CAniInfoHeader::SetSortPoint2(int nX, int nY)
{
	m_AniHeader.m_nSortPoint2[0] = nX;
	m_AniHeader.m_nSortPoint2[1] = nY;
}

/** 排序点2偏移(X,Y)
@param   
@param   
@return  
*/
void CAniInfoHeader::GetSortPoint2(int &nX, int &nY)
{
	nX = m_AniHeader.m_nSortPoint2[0];
	nY = m_AniHeader.m_nSortPoint2[1];
}

/**  TILE的左上角相对于锚点的TILE偏移
@param   
@param   
@return  
*/
void CAniInfoHeader::SetTileOffset(int nX, int nY)
{
	m_AniHeader.m_nTileOffset[0] = nX;
	m_AniHeader.m_nTileOffset[1] = nY;
}

/**  TILE的左上角相对于锚点的TILE偏移
@param   
@param   
@return  
*/
void CAniInfoHeader::GetTileOffset(int &nX, int &nY)
{
	nX = m_AniHeader.m_nTileOffset[0];
	nY = m_AniHeader.m_nTileOffset[1];
}

/** Tile占位个数
@param   
@param   
@return  
*/
void CAniInfoHeader::SetTileSize(int nW, int nH)
{
	m_AniHeader.m_nTileW = nW;
	m_AniHeader.m_nTileH = nH;
}

/** Tile占位个数
@param   
@param   
@return  
*/
void CAniInfoHeader::GetTileSize(int &nW, int &nH)
{
	nW = m_AniHeader.m_nTileW;
	nH = m_AniHeader.m_nTileH;
}

/** 设置Tile信息
@param   
@param   
@return  
*/
void CAniInfoHeader::SetTileInfor(DWORD * pTileInfor)
{
	if(m_AniHeader.m_pTileFlag != NULL)
	{
		SAFE_DELETEARRAY(m_AniHeader.m_pTileFlag);
	}

	if(pTileInfor != NULL)
	{
		m_AniHeader.m_pTileFlag = new DWORD [m_AniHeader.m_nTileW * m_AniHeader.m_nTileH];	
		memcpy(m_AniHeader.m_pTileFlag, pTileInfor, sizeof(DWORD) * m_AniHeader.m_nTileW * m_AniHeader.m_nTileH);
	}
}

/** 取得tile信息
@param   
@param   
@return  
*/
DWORD *	CAniInfoHeader::GetTileInfor(void)
{
	return m_AniHeader.m_pTileFlag;
}

/** 画图片左上角世界像素
@param   
@param   
@return  
*/
void CAniInfoHeader::SetDrawAniPixel(int nX, int nY)
{
	m_AniHeader.m_nDrawAniPixel[0] = nX;
	m_AniHeader.m_nDrawAniPixel[1] = nY;
}

/** 画图片左上角世界像素
@param   
@param   
@return  
*/
void CAniInfoHeader::GetDrawAniPixel(int &nX, int &nY)
{
	nX = m_AniHeader.m_nDrawAniPixel[0];
	nY = m_AniHeader.m_nDrawAniPixel[1];
}