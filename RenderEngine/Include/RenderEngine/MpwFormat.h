/*******************************************************************
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

// 包标识符最长字符
#define __PACKCAPTION_MAX_LEN				32
///////////////////////////////////////////////////////////////////
// 包文件头
class PackFileHeader	
{
	struct SPackFileHeader
	{
		DWORD				m_dwVersion;				// 版本(前16字节为主版本，后16字节为副版本)
		//char				m_szCaption[__PACKCAPTION_MAX_LEN];// 标识符
		DWORD				m_dwCode;					// 标识码
		DWORD				m_dwPackType;				// 包类型
		DWORD				m_dwCompress;				// 压缩标志（压缩只会压缩包文件头后的数据）
		DWORD				m_dwEncrypt;				// 加密标志（加密只会加密包文件头后的数据）
		
		char				m_szFileFormat[8];			// 文件格式
		DWORD				m_dwTotalFrame;				// 总共动画帧数
		DWORD				m_dwPlayDelay;				// 动画播放延时

		DWORD				m_dwZipLen;					// 后面Zip长度
		//char				m_szReserve[128];			// 保留字节 m_ptViewPortOffset(在游戏中不起作用，只用在工具中）

		SPackFileHeader(void)
		{
			memset(this, 0, sizeof(SPackFileHeader));

			// 特殊初始化
			m_dwPlayDelay = 30;
		}
	};
public:
	/** 打开 
	@param   
	@param   
	@return  
	*/
	bool					Open(xs::Stream * pStream);

	/** 设置压缩标志
	@param   
	@param   
	@return  
	*/
	void					SetCompressFlag(DWORD dwFlag);

	/** 取得压缩标志
	@param   
	@param   
	@return  
	*/
	DWORD					GetCompressFlag(void);

	/** 设置加密标志
	@param   
	@param   
	@return  
	*/
	void					SetEncryptFlag(DWORD dwFlag);

	/** 取得加密标志
	@param   
	@param   
	@return  
	*/
	DWORD					GetEncryptFlag(void);

	/** 设置标识符
	@param   
	@param   
	@return  
	*/
	void					SetPackCaption(LPCSTR pszCaption);

	/** 取得标识符
	@param   
	@param   
	@return  
	*/
	LPCSTR					GetPackCaption(void);

	/** 设置包类型
	@param   
	@param   
	@return  
	*/
	void					SetPackType(DWORD dwPackType);

	/** 获得包类型
	@param   
	@param   
	@return  
	*/
	DWORD					GetPackType(void);	

	/** 设置后面Zip长度
	@param   
	@param   
	@return  
	*/
	void					SetZipLen(DWORD dwZipLen);

	/** 获得后面Zip长度
	@param   
	@param   
	@return  
	*/
	DWORD					GetZipLen(void);	

	/** 取得结构大小
	@param   
	@param   
	@return  
	*/
	int						GetStructSize(void);

	/** 取得保留字节
	@param   
	@param   
	@return  
	*/
	char *					GetReserve(void);

	/** 设置保留字节
	@param   
	@param   
	@return  
	*/
	void					SetReserve(char * pszReserve, int nLen);

	/** 文件格式
	@param   
	@param   
	@return  
	*/
	void					SetFileFormat(LPCSTR szFileFormat);

	/** 文件格式
	@param   
	@param   
	@return  
	*/
	LPCSTR					GetFileFormat(void);

	/** 总动画帧数
	@param   
	@param   
	@return  
	*/
	void					SetTotalFrame(DWORD dwTotalFrame);

	/** 总动画帧数
	@param   
	@param   
	@return  
	*/
	DWORD					GetTotalFrame(void);

	/** 动画播放延时
	@param   
	@param   
	@return  
	*/
	void					SetPlayDelay(DWORD dwPlayDelay);

	/** 动画播放延时
	@param   
	@param   
	@return  
	*/
	DWORD					GetPlayDelay(void);

	/** 构造函数
	@param   
	@param   
	@return  
	*/
	PackFileHeader(void);	

private:
	// 包文件头
	SPackFileHeader			m_PackFileHeader;
};

///////////////////////////////////////////////////////////////////
// 动画包每一帧数据
class FrameInfoHeader
{
private:
	struct SFrameInfoHeader
	{
		int					m_nImageSize;				// 图片数据大小
		short				m_nWidth;					// 图片宽
		short				m_nHeight;					// 图片高
		short				m_nOffsetX;
		short				m_nOffsetY;

		SFrameInfoHeader(void)
		{
			memset(this, 0, sizeof(SFrameInfoHeader));
		}
	};	

public:
	/** 打开
	@param   
	@param   
	@return  
	*/
	bool					Open(xs::Stream * pStream);

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

	/** 设置图片大小
	@param   
	@param   
	@return  
	*/
	void					SetImageSize(int nWidth, int nHeight);

	/** 取得图片大小
	@param   
	@param   
	@return  
	*/
	void					GetImageSize(int &nWidth, int &nHeight);

	void					GetImageOffset(int &nX,int &nY);

	/** 取得图片数据
	@param   
	@param   
	@return  
	*/
	xs::Stream *			GetImageData(int &nImageSize);

	/** 构造函数
	@param   
	@param   
	@return  
	*/
	FrameInfoHeader(void);

private:
	// 数据头
	SFrameInfoHeader		m_FrameInfoHeader;

	// 图片数据
	xs::MemoryStream		m_streamImageData;

public:
	FrameInfoHeader(const FrameInfoHeader& f)
	{
		*this = f;
	}
};

#pragma pack()