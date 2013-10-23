/******************************************************************
** 文件名:	MpwDecoder.h
** 版  本:	1.0
** 描  述:	Mpw文件加载器
** 应  用:  
******************************************************************/
#pragma once
#include "MpwFormat.h"
#include "IMpwDecoder.h"
#include <string>

using namespace std;

namespace xs
{
	class _RenderEngineExport  MpwDecoder : public IMpwDecoder
	{

		typedef vector< FrameInfoHeader > TLIST_FRAME;	
		// 帧动画信息
		TLIST_FRAME					m_listFrame;

		PackFileHeader	m_packFileHeader;

		std::string		m_strMpwFileName;

		uint			m_frames;

		/************************************************************************/
		/* constructor and deconstructor                                        */
		/************************************************************************/
	public:
		MpwDecoder();
		~MpwDecoder();

		/************************************************************************/
		/* 外部调用函数定义                                                     */
		/************************************************************************/
	public:
		/** 加载mpw文件
		@param strMpwFileName
		@return true -- load success false -- load fail.
		*/
		virtual bool load(const std::string & strMpwFileName);

		/** 加载mpw文件
		@param pszMpwFileName
		@return true -- load success false -- load fail.
		*/
		virtual bool load(const char* pszMpwFileName);

		/** 得到总帧数
		*/
		virtual ulong getTotalFrame();

		/** 取得文件格式
		*/
		virtual const char* getFileFormat(void);

		/** 取得Delay
		*/
		virtual ulong getPlayDelay();

		virtual void getImageOffset(int nFrame,int &nX,int &nY);
	
		/** 取得总帧数
		@param nFrame 帧号
		@return NULL to fail.
		*/
		virtual Stream * getFrameData(int nFrame);

		/** 清除数据
		*/
		void close();

		/** 释放对象
		*/
		virtual void release() ;
	};
}

