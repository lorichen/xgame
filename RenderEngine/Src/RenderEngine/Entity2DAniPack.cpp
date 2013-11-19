/*******************************************************************
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "Stdafx.h"

#include "MpwFormat.h"
#include "Mpwdecoder.h"
#include "Entity2DAniPack.h"
#include "MpwFrame.h"

namespace xs
{
	/** 构造
	@param   
	@param   
	@return  
	*/
	Entity2DAniPack::Entity2DAniPack()
	{
		m_listImageRes.clear();

		m_dwLastTickCount = 0;

		m_dwImageCount = 0;

		m_pRenderSystem = NULL;

		m_stringPath.clear();

		m_itlistImageRes = m_listImageRes.end();

		m_nPlayFrame = -1;

		// 渲染系统
		m_pRenderSystem = 0;

		m_pMpwDecoder = new MpwDecoder;

		m_memorySize = 0;
	}


	/** 析构
	@param   
	@param   
	@return  
	*/
	Entity2DAniPack::~Entity2DAniPack(void)
	{

	}


	/** 释放
	@param   
	@param   
	@return  
	*/
	void Entity2DAniPack::release(void)
	{
	
		safeRelease(m_pMpwDecoder);
		// 释放
		TLIST_IMAGERES::iterator itI = m_listImageRes.begin();
		for( ; itI != m_listImageRes.end(); ++itI)
		{
			SIMAGERES * pImageRes = &(*itI);

			if(pImageRes->pMpwFrame != NULL)
			{
				pImageRes->pMpwFrame->finalize();
			}			

			safeDelete(pImageRes->pMpwFrame);
		}

		m_listImageRes.clear();

		delete this;
	}


	/** 打开
	@param   
	@param   
	@return 1 : 失败
	2 ：文件数超过
	3 ：未找到文件
	4 ：读取资源失败
	5 : 非法文件格式
	6 : 解压失败
	*/
	int Entity2DAniPack::Open(IRenderSystem * pRenderSystem, char * pszPathFileName)
	{
		if(pRenderSystem == NULL || pszPathFileName == NULL)
		{
			Info("Entity2DAniPack::Open Param list is empty");
			return 1;
		}
		m_stringPackName = pszPathFileName;

		m_pRenderSystem = pRenderSystem;
		if(!m_pMpwDecoder->load(pszPathFileName))
			return 1;
		int nTotal = m_pMpwDecoder->getTotalFrame();
		m_dwImageCount = 0;
		for(int nFrame = 0 ; nFrame < nTotal ; nFrame++)
		{
			SIMAGERES imageres;			
			xs::Stream * pStream = m_pMpwDecoder->getFrameData(nFrame);
			if(pStream != NULL)
			{
				imageres.pMpwFrame = new MpwFrame();
				if( ! imageres.pMpwFrame->initialize(m_pRenderSystem,
						pStream,
						m_pMpwDecoder->getFileFormat(), true) )
				{
					safeDelete(imageres.pMpwFrame);
					return 1;
				}
			}
			m_dwImageCount++;

			int nX,nY;
			m_pMpwDecoder->getImageOffset(nFrame,nX,nY);
			imageres.m_nOffsetX = nX;
			imageres.m_nOffsetY = nY;
			m_listImageRes.push_back(imageres);
		}		
		m_pMpwDecoder->close();

		m_memorySize = 0;

		TLIST_IMAGERES::iterator begin = m_listImageRes.begin();
		TLIST_IMAGERES ::iterator end = m_listImageRes.end();
		while(begin != end)
		{
			m_memorySize += (*begin).pMpwFrame->getMemorySize();
			++begin;
		}

		// 替代器指向LIST头
		m_itlistImageRes = m_listImageRes.begin();
		m_nPlayFrame = 0;
		return 0;
	}


	/** 显示
	@param   
	@param   
	@return  
	*/
	void Entity2DAniPack::Draw(float alpha)
	{
		if(m_dwImageCount <= 0)
			return;

		if(m_dwImageCount > 1)
		{
			// 初始化
			if(m_dwLastTickCount == 0)
			{
				m_dwLastTickCount = getTickCount();
			}

			// 图片翻转
			DWORD dwTickCount = getTickCount();
			if((dwTickCount - m_dwLastTickCount) >= m_pMpwDecoder->getPlayDelay())
			{
				m_itlistImageRes++;
				m_nPlayFrame++;
				if(m_itlistImageRes == m_listImageRes.end())
				{
					m_itlistImageRes = m_listImageRes.begin();
					m_nPlayFrame = 0;
				}

				m_dwLastTickCount = dwTickCount;
			}
		}

		// 显示
		if((*m_itlistImageRes).pMpwFrame != NULL)
		{
			(*m_itlistImageRes).pMpwFrame->render(alpha);
		}
	}

	size_t	Entity2DAniPack::getMemorySize()
	{
		return m_memorySize;
	}

	/** 显示哪一帧
	@param   
	@param   
	@return  
	*/
	void Entity2DAniPack::Draw(int nFrame)
	{
		if(nFrame < 0 || nFrame >= (int)m_dwImageCount)
		{
			return;
		}

		if(m_nPlayFrame != nFrame)
		{
			int nMargin = nFrame - m_nPlayFrame;
			advance(m_itlistImageRes, nMargin);	
			m_nPlayFrame += nMargin;
		}

		// 显示
		if((*m_itlistImageRes).pMpwFrame != NULL)
		{
			(*m_itlistImageRes).pMpwFrame->render();
		}	
	}
}