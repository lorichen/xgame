#include "StdAfx.h"
#include "TextureManagerD3D9.h"
#include "RenderSystemD3D9.h"
#include "D3D9Device.h"
#include "TextureD3D9.h"

namespace xs
{


	TextureManagerD3D9 * TextureManagerD3D9Creater::create( RenderSystemD3D9 * pRenderSystem)
	{
		if( NULL == pRenderSystem)
			return 0;

		TextureManagerD3D9 * pMgr = new TextureManagerD3D9();
		if( NULL == pMgr)
			return 0;

		if( !pMgr->create(pRenderSystem) )
		{
			pMgr->release();
			return 0;
		}

		return pMgr;
	}

	bool TextureManagerD3D9::create(RenderSystemD3D9 * pRenderSystem)
	{
		if( NULL == pRenderSystem) 
			return false;

		m_pRenderSystem = pRenderSystem;

		return true;
	}



	void TextureManagerD3D9::release()
	{
		releaseAll();

		delete this;
	}



	const char* TextureManagerD3D9::getUniqueString()
	{
		m_mutex.Lock();
		static char szName[256];
		sprintf_s(szName,"ReTextureManagerUnique%d",m_ui32Index++);
		m_mutex.Unlock();
		return szName;
	}
	
	/** 从图像数据创建贴图,贴图是width*height的尺寸,并把image的pRect区域blt到贴图上
	@param image 图像数据
	@param pRect 图像的子区域
	@param width 宽度
	@param height 高度
	@param minFO Min Filter
	@param magFO Max Filter
	@param mipFO Mip Filter
	@param s 贴图S寻址方式
	@param t 贴图T寻址方式
	@return 贴图指针
	*/
	ITexture* TextureManagerD3D9::createTextureFromImage(																
		const Image& image,																
		const Rect* pRect,																
		uint width,uint height,														
		FilterOptions minFO,												
		FilterOptions magFO,												
		FilterOptions mipFO,													
		TextureAddressingMode s,											
		TextureAddressingMode t)
	{
		ITexture *pTexture = createEmptyTexture(width,height,image.getFormat(),minFO,magFO,mipFO,s,t);
		if(pTexture)
		{
			//TODO:mipmaps
			if(!pTexture->setSubData(0,image,pRect))
			{
				releaseTexture(pTexture);
				pTexture = 0;
			}
		}

		return pTexture;
	}

	/** 从图像数据创建贴图,贴图的大小是pRect的宽度,如果pRect则为整张图片的尺寸
	@param image 图像数据
	@param pRect 图像的子区域
	@param minFO Min Filter
	@param magFO Max Filter
	@param mipFO Mip Filter
	@param s 贴图S寻址方式
	@param t 贴图T寻址方式
	@return 贴图指针
	*/
	ITexture* TextureManagerD3D9::createTextureFromImage(																
		const Image& image,																
		const Rect* pRect,																
		FilterOptions minFO ,												
		FilterOptions magFO ,												
		FilterOptions mipFO ,													
		TextureAddressingMode s ,											
		TextureAddressingMode t)
	{
		ITexture *pTexture = createEmptyTexture(minFO,magFO,mipFO,s,t);
		if(pTexture)
		{
			if(!pTexture->loadFromImage(image,pRect))
			{
				releaseTexture(pTexture);
				pTexture = 0;
			}
		}

		return pTexture;
	}

	/** 从图像文件创建贴图,在有些显卡上支持非2^n贴图,但大多数时候用户需要使用2^N贴图
	@param name 文件名
	@param minFO Min Filter
	@param magFO Max Filter
	@param mipFO Mip Filter
	@param s 贴图S寻址方式
	@param t 贴图T寻址方式
	@return 贴图指针
	*/
	ITexture* TextureManagerD3D9::createTextureFromFile(																
		const std::string& name,																
		FilterOptions minFO,												
		FilterOptions magFO,												
		FilterOptions mipFO,													
		TextureAddressingMode s ,											
		TextureAddressingMode t)
	{
		m_mutex.Lock();
		if(names.find(name) != names.end()) 
		{
			ITexture *pTexture = names[name];
			items[pTexture]->addRef();
			m_mutex.Unlock();
			return pTexture;
		}
		m_mutex.Unlock();

		ITexture* pTexture = createEmptyTexture(minFO,magFO,mipFO,s,t,name);
		if(pTexture)
		{
			if(pTexture->loadFromFile(name))
			{
				return pTexture;
			}
			releaseTexture(pTexture);
			pTexture = 0;
		}

		return pTexture;
	}

	/** 从Raw数据创建贴图
	@param pBuffer 数据指针
	@param width pBuffer图像数据的宽度
	@param height pBuffer图像数据的高度
	@param minFO Min Filter
	@param magFO Max Filter
	@param mipFO Mip Filter
	@param s 贴图S寻址方式
	@param t 贴图T寻址方式
	@return 贴图指针
	*/
	ITexture* TextureManagerD3D9::createTextureFromRawData(															
		uchar* pBuffer,																	
		uint width,uint height,PixelFormat pf,										
		FilterOptions minFO,												
		FilterOptions magFO,												
		FilterOptions mipFO,													
		TextureAddressingMode s,											
		TextureAddressingMode t)
	{
		ITexture* pTexture = createEmptyTexture(minFO,magFO,mipFO,s,t);
		if(pTexture)
		{
			if(pTexture->loadFromRawData(pBuffer,width,height,pf))
			{
				return pTexture;
			}
			releaseTexture(pTexture);
			pTexture = 0;
		}

		return pTexture;
	}

	/** 创建空的贴图,未指定宽度、高度和像素格式
	@param minFO Min Filter
	@param magFO Max Filter
	@param mipFO Mip Filter
	@param s 贴图S寻址方式
	@param t 贴图T寻址方式
	@return 贴图指针
	*/
	ITexture* TextureManagerD3D9::createEmptyTexture(																	
		FilterOptions minFO ,												
		FilterOptions magFO,												
		FilterOptions mipFO ,													
		TextureAddressingMode s,											
		TextureAddressingMode t,
		const std::string& initialName)
	{
		m_mutex.Lock();

		std::string name;
		if(initialName != "")
		{
			name = initialName;
			if( names.find( name) != names.end() )
			{
				m_mutex.Unlock();
				return 0;
			}
		}
		else
			name = getUniqueString();

		TextureD3D9 * pTexture = new TextureD3D9(name, this, minFO, magFO, mipFO, s, t);
		if(!pTexture)
		{
			Error("Unable to allocate memory,TextureManager::createEmptyTexture failed!");
			m_mutex.Unlock();
			return 0;
		}

		do_add(name,pTexture,pTexture);

		m_mutex.Unlock();
		return pTexture;
	}

	/** 创建空的贴图,并指定宽度、高度和像素格式
	@param width 宽度
	@param height 高度
	@param pf 像素格式
	@param minFO Min Filter
	@param magFO Max Filter
	@param mipFO Mip Filter
	@param s 贴图S寻址方式
	@param t 贴图T寻址方式
	@return 贴图指针
	*/
	ITexture*	TextureManagerD3D9::createEmptyTexture(																	
		uint width,
		uint height,
		PixelFormat pf,										
		FilterOptions minFO,												
		FilterOptions magFO ,												
		FilterOptions mipFO ,													
		TextureAddressingMode s,											
		TextureAddressingMode t)
	{
		ITexture * pTexture = createEmptyTexture(minFO,magFO,mipFO,s,t);
		if(pTexture)
		{
			bool ret = pTexture->loadFromRawData(NULL, width, height, pf);
			if( !ret) 
			{
				pTexture->release();
				return 0;
			}
			else
			{
				return pTexture;
			}
		}
		return 0;
	}

	/** 创建空的贴图,并指定宽度、高度和像素格式。不需要等待渲染结束，这个函数会影响帧率，但是
	在渲染中调用不会锁定。
	@param width 宽度
	@param height 高度
	@param pf 像素格式
	@param minFO Min Filter
	@param magFO Max Filter
	@param mipFO Mip Filter
	@param s 贴图S寻址方式
	@param t 贴图T寻址方式
	@return 贴图指针
	*/
	ITexture*	TextureManagerD3D9::createEmptyTextureNoWait(																	
		uint width,uint height,PixelFormat pf,										
		FilterOptions minFO,												
		FilterOptions magFO,												
		FilterOptions mipFO,													
		TextureAddressingMode s,											
		TextureAddressingMode t)
	{
		TextureD3D9 * pTexture = static_cast<TextureD3D9 *>( createEmptyTexture(minFO,magFO,mipFO,s,t) );
		if(pTexture)
		{
			bool ret = pTexture->_create(D3DPOOL_MANAGED, 0,width, height, pf, TextureD3D9::CTO_NOWAIT);
			if(!ret)
			{
				pTexture->release();
				return 0;
			}
			else
			{
				return pTexture;
			}
		}
		return 0;	
	}

	/** 根据文件名取得贴图指针，也可以是自定义的贴图名称
	@param name 文件名或者自定义的贴图名称
	@return 贴图指针
	*/
	ITexture*	TextureManagerD3D9::getTexture(const std::string& name)
	{
		m_mutex.Lock();
		ITexture *pTexture = get(name);
		m_mutex.Unlock();

		return pTexture;
	}

	/** 释放贴图
	@param pTexture 贴图的指针
	*/
	void 	TextureManagerD3D9::releaseTexture(ITexture* pTexture)
	{
		m_mutex.Lock();
		del(pTexture);
		m_mutex.Unlock();
	}

	/** 根据名称释放贴图
	@param name 贴图的名称
	*/
	void TextureManagerD3D9::releaseTextureByName(const std::string& name)
	{
		m_mutex.Lock();
		delByName(name);
		m_mutex.Unlock();
	}

	/** 释放所有贴图
	*/
	void TextureManagerD3D9::releaseAll()
	{
		m_mutex.Lock();
		std::map<ITexture*,ManagedItem*>::iterator e = items.end();
		for(std::map<ITexture*,ManagedItem*>::iterator b = items.begin();b != e;++b)
		{
			doDelete((*b).first);
		}
		items.clear();
		names.clear();
		m_mutex.Unlock();
	}

	void TextureManagerD3D9::doDelete(ITexture* pTexture)
	{
		if(pTexture)
		{
			delete static_cast<TextureD3D9*>(pTexture);
		}
	}

	TextureD3D9* TextureManagerD3D9::createEmptyeTexture(
		D3DPOOL	pooltype,
		DWORD usage,
		uint width,uint height,PixelFormat pf,										
		FilterOptions minFO,												
		FilterOptions magFO,												
		FilterOptions mipFO ,													
		TextureAddressingMode s,											
		TextureAddressingMode t)
	{
		TextureD3D9 * pTexture = static_cast<TextureD3D9 *>( createEmptyTexture(minFO,magFO,mipFO,s,t) );
		if(pTexture)
		{
			bool ret = pTexture->_create(pooltype, usage,width, height, pf);
			if(!ret)
			{
				pTexture->release();
				return 0;
			}
			else
			{
				return pTexture;
			}
		}
		return 0;
		
	}
}
