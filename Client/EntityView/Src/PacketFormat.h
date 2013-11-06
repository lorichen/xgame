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
#pragma once

#include "stdio.h"

#pragma pack(1)

// 每个目录的散文件个数
#define DIR_MAX_FILE_COUNT				256

// 动画最长延时
#define ANI_MAX_DELAY					600000

// 包标识符最长字符
#define PACKCAPTION_MAX_LEN				32

// 非法坐标
#ifndef INVALID_COORDINATE
	#define INVALID_COORDINATE			-9999999
#endif

///////////////////////////////////////////////////////////////////
// 包类型
enum
{
	PACKTYPE_UNKNOW = 0,								// 未知的
	PACKTYPE_ANI,										// 动画包
	PACKTYPE_MAX,										// 最多类
};


///////////////////////////////////////////////////////////////////
// 动画包信息头
class CAniInfoHeader
{
public:
	struct SAniHeader
	{
		DWORD				m_dwVersion;				// 版本(前16字节为主版本，后16字节为副版本)
		DWORD				m_dwCode;					// 标识码		
		DWORD				m_dwCompress;				// 压缩标志
		DWORD				m_dwEncrypt;				// 加密标志
		char				m_szReserve[128];			// 保留字节

		int					m_nDrawAniPixel[2];			// 画图片左上角世界像素(X,Y)(在游戏中不起作用，只用在工具中）
		int					m_nAnchorOffset[2];			// 锚点到图片左上角的像素偏移(X,Y)
		int					m_nSortPoint1[2];			// 排序点1相对于锚点的TILE偏移(X,Y)
		int					m_nSortPoint2[2];			// 排序点2相对于锚点的TILE偏移(X,Y)
		int					m_nTileOffset[2];			// TILE的左上角相对于锚点的TILE偏移(X,Y)
		int					m_nTileW;					// X方向的占位个数
		int					m_nTileH;					// Y方向的占位个数
		int					m_iImageMaxWidth;			//图片最大宽度
		int					m_iImageMaxHeight;			//图片最大高度

		DWORD *				m_pTileFlag;				// TILE信息(此大小不算入文件包内)	

		SAniHeader(void)
		{
			memset(this, 0, sizeof(SAniHeader));

			// 标识码
			m_dwCode = 'WMTL';

			// 版本(前16字节为主版本，后16字节为副版本)
			m_dwVersion = (1 << 16 | 1 << 0);

			// 画图片左上角世界像素(X,Y)
			m_nDrawAniPixel[0] = INVALID_COORDINATE;
			m_nDrawAniPixel[1] = INVALID_COORDINATE;

			// 锚点到图片左上角的像素偏移(X,Y)
			m_nAnchorOffset[0] = INVALID_COORDINATE;
			m_nAnchorOffset[1] = INVALID_COORDINATE;

			// 排序点1相对于锚点的TILE偏移(X,Y)
			m_nSortPoint1[0] = INVALID_COORDINATE;
			m_nSortPoint1[1] = INVALID_COORDINATE;

			// 排序点2相对于锚点的TILE偏移(X,Y)
			m_nSortPoint2[0] = INVALID_COORDINATE;
			m_nSortPoint2[1] = INVALID_COORDINATE;

			// TILE的左上角相对于锚点的TILE偏移(X,Y)
			m_nTileOffset[0] = INVALID_COORDINATE;
			m_nTileOffset[1] = INVALID_COORDINATE;
		}
	};

public:
	/** 打开
	@param   pszMwdFileName ：为*.mwd文件名
	@param   
	@return  
	*/
	bool					Open(LPCSTR pszMwdFileName);

	/** 释放
	@param   
	@param   
	@return  
	*/
	void					Release(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void					Close(void);

	/** 锚点到图片左上角的偏移(X,Y)
	@param   
	@param   
	@return  
	*/
	void					SetAnchorOffset(int nX, int nY);

	/** 锚点到图片左上角的偏移(X,Y)
	@param   
	@param   
	@return  
	*/
	void					GetAnchorOffset(int &nX, int &nY);

	/** 排序点1偏移(X,Y)
	@param   
	@param   
	@return  
	*/
	void					SetSortPoint1(int nX, int nY);

	/** 排序点1偏移(X,Y)
	@param   
	@param   
	@return  
	*/
	void					GetSortPoint1(int &nX, int &nY);

	/** 排序点2偏移(X,Y)
	@param   
	@param   
	@return  
	*/
	void					SetSortPoint2(int nX, int nY);

	/** 排序点2偏移(X,Y)
	@param   
	@param   
	@return  
	*/
	void					GetSortPoint2(int &nX, int &nY);

	/**  TILE的左上角相对于锚点的TILE偏移
	@param   
	@param   
	@return  
	*/
	void					SetTileOffset(int nX, int nY);

	/**  TILE的左上角相对于锚点的TILE偏移
	@param   
	@param   
	@return  
	*/
	void					GetTileOffset(int &nX, int &nY);	

	/** Tile占位个数
	@param   
	@param   
	@return  
	*/
	void					SetTileSize(int nW, int nH);

	/** Tile占位个数
	@param   
	@param   
	@return  
	*/
	void					GetTileSize(int &nW, int &nH);

	/** 设置Tile信息
	@param   
	@param   
	@return  
	*/
	void					SetTileInfor(DWORD * pTileInfor);

	/** 取得tile信息
	@param   
	@param   
	@return  
	*/
	DWORD *					GetTileInfor(void);

	/** 画图片左上角世界像素
	@param   
	@param   
	@return  
	*/
	void					SetDrawAniPixel(int nX, int nY);

	/** 画图片左上角世界像素
	@param   
	@param   
	@return  
	*/
	void					GetDrawAniPixel(int &nX, int &nY);	

	int						GetImageMaxWidth() { return m_AniHeader.m_iImageMaxWidth;}
	int						GetImageMaxHeight() { return m_AniHeader.m_iImageMaxHeight;}

	/** 构造函数
	@param   
	@param   
	@return  
	*/
	CAniInfoHeader(void);

private:
	// 数据头
	SAniHeader				m_AniHeader;
};


#pragma pack()