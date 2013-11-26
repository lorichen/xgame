#include "StdAfx.h"
#include "TextureManager.h"
#include "Texture.h"
#include "GLPixelFormat.h"

namespace xs
{

	//--------------------------------------------------------------------
	GLint getAddressingMode(TextureAddressingMode tam)
	{
		GLint type = GL_CLAMP;
		switch(tam)
		{
		case TAM_WRAP:
			type = GL_REPEAT;
			break;
		case TAM_MIRROR:
			type = GL_MIRRORED_REPEAT;
			break;
		case TAM_CLAMP_TO_EDGE:
			type = GL_CLAMP_TO_EDGE;
			break;
		case TAM_CLAMP:
			type = GL_CLAMP;
			break;
		}

		return type;
	}
	//--------------------------------------------------------------------
	TextureAddressingMode getAddressingMode(GLint mode)
	{
		TextureAddressingMode type = TAM_WRAP;
		switch(mode)
		{
		case GL_REPEAT:
			type = TAM_WRAP;
			break;
		case GL_MIRRORED_REPEAT:
			type = TAM_MIRROR;
			break;
		case GL_CLAMP_TO_EDGE:
			type = TAM_CLAMP_TO_EDGE;
			break;
		case GL_CLAMP:
			type = TAM_CLAMP;
			break;
		}

		return type;
	}

	//---------------------------------------------------------------------
	GLuint getMagFilter(FilterOptions mMagFilter)
	{
		switch (mMagFilter)
		{
		case FO_ANISOTROPIC: // GL treats linear and aniso the same
		case FO_LINEAR:
			return GL_LINEAR;
		case FO_POINT:
		case FO_NONE:
			return GL_NEAREST;
		}

		//should never get here
		return GL_NEAREST;
	}
	//---------------------------------------------------------------------
	void getCombinedMinMipFilter(GLuint filter,FilterOptions &mMinFilter,FilterOptions &mMipFilter)
	{
		switch(filter)
		{
		case GL_LINEAR_MIPMAP_LINEAR:
			mMinFilter = FO_LINEAR;
			mMipFilter = FO_LINEAR;
			break;
		case GL_LINEAR_MIPMAP_NEAREST:
			mMinFilter = FO_LINEAR;
			mMipFilter = FO_POINT;
			break;
		case GL_LINEAR:
			mMinFilter = FO_LINEAR;
			mMipFilter = FO_NONE;
			break;
		case GL_NEAREST_MIPMAP_LINEAR:
			mMinFilter = FO_NONE;
			mMipFilter = FO_LINEAR;
			break;
		case GL_NEAREST_MIPMAP_NEAREST:
			mMinFilter = FO_NONE;
			mMipFilter = FO_POINT;
			break;
		case GL_NEAREST:
			mMinFilter = FO_NONE;
			mMipFilter = FO_NONE;
			break;
		}
	}

	//---------------------------------------------------------------------
	void getMagFilter(GLuint filter,FilterOptions &mMagFilter)
	{
		switch (filter)
		{
		case GL_LINEAR:
			mMagFilter = FO_LINEAR;
		case GL_NEAREST:
			mMagFilter = FO_NONE;
		}
	}
	//---------------------------------------------------------------------
	GLuint getCombinedMinMipFilter(FilterOptions mMinFilter,FilterOptions mMipFilter)
	{
		switch(mMinFilter)
		{
		case FO_ANISOTROPIC:
		case FO_LINEAR:
			switch(mMipFilter)
			{
			case FO_ANISOTROPIC:
			case FO_LINEAR:
				// linear min, linear mip
				return GL_LINEAR_MIPMAP_LINEAR;
			case FO_POINT:
				// linear min, point mip
				return GL_LINEAR_MIPMAP_NEAREST;
			case FO_NONE:
				// linear min, no mip
				return GL_LINEAR;
			}
			break;
		case FO_POINT:
		case FO_NONE:
			switch(mMipFilter)
			{
			case FO_ANISOTROPIC:
			case FO_LINEAR:
				// nearest min, linear mip
				return GL_NEAREST_MIPMAP_LINEAR;
			case FO_POINT:
				// nearest min, point mip
				return GL_NEAREST_MIPMAP_NEAREST;
			case FO_NONE:
				// nearest min, no mip
				return GL_NEAREST;
			}
			break;
		}

		// should never get here
		return GL_NEAREST;

	}

	void TextureManager::doDelete(ITexture* pTexture)
	{
		if(pTexture)
		{
			GLuint id = static_cast<Texture*>(pTexture)->getGLTextureID();
			glDeleteTextures(1,&id);

			delete static_cast<Texture*>(pTexture);
		}
	}

	ITexture*	TextureManager::createTextureFromImage(
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

	ITexture*	TextureManager::createTextureFromImage(
		const Image& image,
		const Rect* pRect,
		FilterOptions minFO,
		FilterOptions magFO,
		FilterOptions mipFO,
		TextureAddressingMode s,
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

	ITexture*	TextureManager::createTextureFromFile(
		const std::string& name,
		FilterOptions min,
		FilterOptions mag,
		FilterOptions mip,
		TextureAddressingMode s,
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

		ITexture* pTexture = createEmptyTexture(min,mag,mip,s,t,name);
		if(pTexture)
		{
			if(pTexture->loadFromFile(name))
			{
				//commentted by xxh 这里不需要再设置了，而且会出现同时访问临界区的异常
				//names[name] = pTexture;
				//items[pTexture] = static_cast<Texture*>(pTexture);

				return pTexture;
			}
			releaseTexture(pTexture);
			pTexture = 0;
		}

		return pTexture;
	}

	ITexture*	TextureManager::createEmptyTexture(
		FilterOptions min,
		FilterOptions mag,
		FilterOptions mip,
		TextureAddressingMode s,
		TextureAddressingMode t,
		const std::string& initialName)
	{
		m_mutex.Lock();

		std::string name;
		if(initialName != "")
			name = initialName;
		else
			name = getUniqueString();

		Texture *pTexture = new Texture(name,this);
		if(!pTexture)
		{
			Error("Unable to allocate memory,TextureManager::loadRawData failed!");
			m_mutex.Unlock();
			return 0;
		}

		GLuint id;
		glGenTextures(1,&id);
		GLenum err = glGetError();
		const uchar *pTRACE0_ERRORS = gluErrorString(err);
		glBindTexture(GL_TEXTURE_2D,id);

		pTexture->setGLTextureID(id);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,getCombinedMinMipFilter(min,mip));
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,getMagFilter(mag));

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getAddressingMode(s));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getAddressingMode(t));

		do_add(name,pTexture,pTexture);

		m_mutex.Unlock();
		return pTexture;
	}

	ITexture*	TextureManager::createEmptyTextureNoWait(																	
		uint width,uint height,PixelFormat pf,										
		FilterOptions minFO,												
		FilterOptions magFO,												
		FilterOptions mipFO,													
		TextureAddressingMode s ,											
		TextureAddressingMode t)
	{
		return createEmptyTexture(width, height, pf, minFO, magFO, mipFO, s, t);
	}

	ITexture*	TextureManager::createEmptyTexture(
		uint width,uint height,PixelFormat pf,
		FilterOptions min,
		FilterOptions mag,
		FilterOptions mip,
		TextureAddressingMode s,
		TextureAddressingMode t)//这个函数绝不会将压缩纹理上传到显卡，因而比较安全
	{
		ITexture *pTexture = createEmptyTexture(min,mag,mip,s,t);
		if(pTexture)
		{
			uint numBytes = width * height * PixelUtil::getNumElemBytes(pf);
			uchar *pData = new uchar[numBytes];
			memset(pData,0,numBytes);
			//modified by xxh 20091029 由于压缩纹理要同步到渲染线程去做，所以要修改这里，把纹理加载给texture自己处理，而且这段代码没有设置mip
			//commented by xxh
			/*glBindTexture(GL_TEXTURE_2D,static_cast<Texture*>(pTexture)->getGLTextureID());
			glTexImage2D(GL_TEXTURE_2D,0,GLPixelUtil::getClosestGLInternalFormat(pf),width,height,0,GLPixelUtil::getGLOriginFormat(pf),GLPixelUtil::getGLOriginDataType(pf),pData);
			delete[] pData;

			Texture *pTex = static_cast<Texture*>(pTexture);
			pTex->setWidth(width);
			pTex->setHeight(height);
			pTex->setPixelFormat(pf);
			*/
			Texture * pTex = static_cast<Texture *>(pTexture);
			pTex->loadFromRawData(pData,width, height,pf);
			delete [] pData;
			pData = 0;
		}

		return pTexture;
	}
	ITexture*	TextureManager::createTextureFromRawData(
		uchar* pBuffer,uint width,uint height,PixelFormat pf,
		FilterOptions min,
		FilterOptions mag,
		FilterOptions mip,
		TextureAddressingMode s,
		TextureAddressingMode t)
	{
		ITexture* pTexture = createEmptyTexture(min,mag,mip,s,t);
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

	ITexture*	TextureManager::getTexture(const std::string& name)
	{
		m_mutex.Lock();
		ITexture *pTexture = get(name);
		m_mutex.Unlock();

		return pTexture;
	}

	void	TextureManager::releaseTexture(ITexture* pTexture)
	{
		m_mutex.Lock();
		del(pTexture);
		m_mutex.Unlock();
	}

	void	TextureManager::releaseTextureByName(const std::string& name)
	{
		m_mutex.Lock();
		delByName(name);
		m_mutex.Unlock();
	}

	void	TextureManager::releaseAll()
	{
		m_mutex.Lock();
		std::map<ITexture*,ManagedItem*>::iterator e = items.end();
		for(std::map<ITexture*,ManagedItem*>::iterator b = items.begin();b != e;++b)
		{
			doDelete((*b).first);
			//delete (*b).second;
		}
		items.clear();
		names.clear();
		m_mutex.Unlock();
	}

	const char* TextureManager::getUniqueString()
	{
		m_mutex.Lock();
		static char szName[256];
		/*do
		{
			str.format("ReTextureManagerUnique%d",m_ui32Num++);
		}while(names.find(str) != names.end());*/

		sprintf_s(szName,"ReTextureManagerUnique%d",m_ui32Num++);
		m_mutex.Unlock();

		return szName;
	}
}