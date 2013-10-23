/*******************************************************************
** 描  述:	资源包（*.mpk)
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "MpwFormat.h"
#include "IMpwDecoder.h"
#include <string>
using namespace std;

namespace xs
{
	class MpwFrame;

	class _RenderEngineExport Entity2DAniPack
	{
	public:
		struct SIMAGERES
		{
			// 图片资源
			MpwFrame *		pMpwFrame;
			int				m_nOffsetX;
			int				m_nOffsetY;

			SIMAGERES(void)
			{
				memset(this, 0, sizeof(SIMAGERES));
			}
		};
		typedef list< SIMAGERES >	TLIST_IMAGERES;		
	public:
		///////////////////////////CEntity2DAni////////////////////////////
		/** 释放
		@param   
		@param   
		@return  
		*/
		virtual void				release(void);

		/** 打开
		@param   
		@param   
		@return  
		*/
		virtual int					Open(IRenderSystem * pRenderSystem, char * pszPathFileName);


		/** 显示
		@param   
		@param   
		@return  
		*/
		virtual void				Draw(float alpha = 1.0f);	

		/** 显示哪一帧
		@param   
		@param   
		@return  
		*/
		virtual void				Draw(int nFrame);

		virtual size_t				getMemorySize();

		/** 构造
		@param   
		@param   
		@return  
		*/
		Entity2DAniPack();


		/** 析构
		@param   
		@param   
		@return  
		*/
		virtual ~Entity2DAniPack(void);

	public:
		// 所有目录下图片
		TLIST_IMAGERES				m_listImageRes;

		// 当前指向的图片资源替代器
		TLIST_IMAGERES::iterator	m_itlistImageRes;
	
		// 当前播的何帧
		int							m_nPlayFrame;	

		// 路径名
		string						m_stringPath;

		// 上次播放的时间
		DWORD						m_dwLastTickCount;

		// 渲染系统
		IRenderSystem *				m_pRenderSystem;

		// 图片张数
		DWORD						m_dwImageCount;
		// 路径名
		string						m_stringPackName;

		IMpwDecoder					*m_pMpwDecoder;

		size_t						m_memorySize;

	};
}