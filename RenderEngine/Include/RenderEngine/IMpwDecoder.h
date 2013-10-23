/******************************************************************
** 描  述:	Mpw文件加载器接口			
** 应  用:  
******************************************************************/
#pragma once
namespace xs
{
	struct IMpwDecoder
	{
		/** 加载mpw文件
		@param strMpwFileName
		@return true -- load success false -- load fail.
		*/
		virtual bool load(const std::string & strMpwFileName) = 0;

		/** 加载mpw文件
		@param pszMpwFileName
		@return true -- load success false -- load fail.
		*/
		virtual bool load(const char* pszMpwFileName) = 0;

		/** 得到总帧数
		*/
		virtual ulong getTotalFrame() = 0;

		/** 取得文件格式
		*/
		virtual const char* getFileFormat(void) = 0;

		/** 取得Delay
		*/
		virtual ulong getPlayDelay() = 0;

		virtual void getImageOffset(int nFrame,int &nX,int &nY) = 0;

		/** 取得总帧数
		@param nFrame 帧号
		@return NULL to fail.
		*/
		virtual Stream * getFrameData(int nFrame) = 0;

		virtual void close() = 0;
		/** 释放对象
		*/
		virtual void release() = 0;
	};
}