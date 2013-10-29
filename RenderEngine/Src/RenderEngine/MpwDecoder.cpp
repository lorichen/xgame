/******************************************************************
** 版  本:	1.0
** 描  述:	Mpw文件加载器			
** 应  用:  
******************************************************************/

#include "Stdafx.h"

#include "MpwFormat.h"
#include "MpwDecoder.h"
namespace xs
{
	MpwDecoder::MpwDecoder()
	{
		m_frames = 0;
		m_listFrame.clear();
	}

	MpwDecoder::~MpwDecoder()
	{
		close();
	}

	void MpwDecoder::close()
	{
		// 释放
		TLIST_FRAME::iterator itF = m_listFrame.begin();
		for( ; itF != m_listFrame.end(); ++itF)
		{
			FrameInfoHeader * pHeader = &(*itF);
			// 释放内存
			pHeader->Close();
		}
		m_listFrame.clear();
	}

	bool MpwDecoder::load(const char* pszMpwFileName)
	{
		//Info("mpw:"<<pszMpwFileName<<endl);
		m_strMpwFileName = pszMpwFileName;
		return load(m_strMpwFileName);
	}

	bool MpwDecoder::load(const std::string & strMpwFileName)
	{
		m_strMpwFileName = strMpwFileName;
		if(m_strMpwFileName.empty())
		{
			Error("RenderEngine::MpwDecoder::loader,strMpwFileName is empty,"<<strMpwFileName.c_str()<<endl);
			return false;
		}
		//清除
		close();

		// 得到mwd路径
		string stringTempMpwFileName = strMpwFileName;
		// 打开mpw文件，是否存在
		CStreamHelper pStreamMpw = xs::getFileSystem()->open(stringTempMpwFileName.c_str());
		if(pStreamMpw == NULL)
		{
			Error("RenderEngine::MpwLoader::loader open mpw fail."<<stringTempMpwFileName.c_str()<<" not exist."<<endl);
			return false;
		}

		/////////////////////////////// 读包头文件，然后再判定是否加密或压缩
		if(!m_packFileHeader.Open(pStreamMpw))
		{	
			pStreamMpw->close();
			Error("RenderEngine::MpwLoader::loader Invalid Pack File Header."<<strMpwFileName.c_str()<<endl);
			return false;
		}

		// 看是否加密或者压缩了
		bool  bCompressFlag = m_packFileHeader.GetCompressFlag() != 0 ? true : false;
		DWORD dwZipLen = m_packFileHeader.GetZipLen();
		if(bCompressFlag && dwZipLen <= 0)
		{
			pStreamMpw->close();
			Error("RenderEngine::MpwLoader::loader Invalid Zip code.\n");
			return false;
		}

		// 偱环读取动画帧
		m_listFrame.clear();
		for(DWORD i = 0; i < m_packFileHeader.GetTotalFrame(); i++)
		{
			FrameInfoHeader frameinfo;
			if(!frameinfo.Open(pStreamMpw))
			{
				pStreamMpw->close();
				Error("RenderEngine::MpwLoader::loader open frame error.");
				return false;
			}
			m_listFrame.push_back(frameinfo);
		}
		pStreamMpw->close();

		m_frames = m_listFrame.size();
		return true;
	}

	/** 得到总帧数
	*/
	ulong MpwDecoder::getTotalFrame()
	{
		if(m_frames)
			return m_packFileHeader.GetTotalFrame();
		return 0;
	}

	/** 取得文件格式
	*/
	const char* MpwDecoder::getFileFormat(void)
	{
		if(m_frames)
			return m_packFileHeader.GetFileFormat();
		return 0;
	}

	/** 取得Delay
	*/
	ulong MpwDecoder::getPlayDelay()
	{
		if(m_frames)
			return m_packFileHeader.GetPlayDelay();
		return 0;
	}

	void MpwDecoder::getImageOffset(int nFrame,int &nX,int &nY)
	{
		if(nFrame >= (int)(m_frames))
		{
			nX = 0;
			nY = 0;
			return;
		}
		
		m_listFrame[nFrame].GetImageOffset(nX,nY);
	}

	/** 取得总帧数
	@param nFrame 帧号
	@return NULL to fail.
	*/
	Stream * MpwDecoder::getFrameData(int nFrame)
	{
		if(nFrame >= (int)(m_frames))
			return NULL;
		int nImageSize = 0;
		return m_listFrame[nFrame].GetImageData(nImageSize);
	}

	void MpwDecoder::release() 
	{
		close();
		delete this;
	}
}