#include "StdAfx.h"
#include "ShaderManagerD3D9.h"
#include "RenderSystemD3D9.h"
#include "D3D9Device.h"
#include "ShaderFragmentD3D9.h"
#include "ShaderProgramD3D9.h"


namespace xs
{

	ShaderManagerD3D9 * ShaderManagerD3D9Creater::create(RenderSystemD3D9 * pRenderSystem)
	{
		if( NULL == pRenderSystem)
			return 0;

		ShaderManagerD3D9 * pMgr= new ShaderManagerD3D9();
		if( NULL == pMgr)
			return 0;

		if( !pMgr->create(pRenderSystem) )
		{
			pMgr->release();
			return 0;
		}
		else
		{
			return pMgr;
		}
	}

	ShaderManagerD3D9::ShaderManagerD3D9():
		m_pRenderSystem(0),
		m_ui32Num(0)
		/*m_pCurrentShaderProgram(0)*/
	{
		// nothing to do
	}

	ShaderManagerD3D9::~ShaderManagerD3D9()
	{
		//nothing to do
	}

	bool ShaderManagerD3D9::create(RenderSystemD3D9 * pRenderSystem)
	{
		if( NULL == pRenderSystem)
			return false;

		m_pRenderSystem = pRenderSystem;
		return true;
	}

	void ShaderManagerD3D9::release()
	{
		releaseAllShader();
		delete this;
	}

	/** 从文件中创建着色器
	@param ShaderType 着色器类型
	@param fn 文件名
	@return 着色器接口指针
	*/
	IShader* ShaderManagerD3D9::createShaderFromFile(ShaderType st,const std::string& fn)
	{
		m_mutex.Lock();
		IShader *pShader = 0;
		if(names.find(fn) != names.end()) 
		{
			pShader = names[fn];
			items[pShader]->addRef();
		}
		else
		{
			xs::CStreamHelper pStream = xs::getFileSystem()->open(fn.c_str());
			if(pStream)
			{
				uint len = pStream->getLength();
				uchar *pData = new uchar[len];
				pStream->read(pData,len);
				pShader = _createShaderFromMemory( m_pRenderSystem, fn, st, pData, len);
				if(pShader)
				{
					do_add(fn,pShader,static_cast<ShaderFragmentD3D9*>(pShader));
				}
				delete[] pData;
			}
		}

		m_mutex.Unlock();
		return pShader;
	}

	/** 从内存数据中创建着色器
	@param ShaderType 着色器类型
	@param pBuffer 数据地址
	@param len 数据长度
	@return 着色器接口指针
	@see ShaderType
	*/
	IShader* ShaderManagerD3D9::createShaderFromMemory(ShaderType st,const uchar *pBuffer,uint len)
	{
		if( NULL == pBuffer || len <= 0)
			return 0;
		m_mutex.Lock();
		std::string name( getUniqueString() );
		IShader * pShader = _createShaderFromMemory(m_pRenderSystem, name, st, pBuffer, len );
		if( pShader)
		{
			do_add( name, pShader, static_cast<ShaderFragmentD3D9 *>(pShader) );
		}
		m_mutex.Unlock();
		return pShader;
	}


	/** 销毁一个着色器
	@param IShader* 着色器指针
	*/
	void ShaderManagerD3D9::releaseShader(IShader* pShader)
	{
		m_mutex.Lock();
		del( pShader );
		m_mutex.Unlock();
	}

	/** 销毁所有的着色器
	*/
	void ShaderManagerD3D9::releaseAllShader()
	{
		m_mutex.Lock();
		std::map<IShader*,ManagedItem*>::iterator e = items.end();
		for(std::map<IShader*,ManagedItem*>::iterator b = items.begin();b != e;++b)
		{
			doDelete((*b).first);
		}
		items.clear();
		names.clear();
		m_mutex.Unlock();
	}


	void ShaderManagerD3D9::doDelete(IShader* pShader)
	{
		ShaderFragmentD3D9 * pD3DShader = static_cast<ShaderFragmentD3D9 *>(pShader);
		if( NULL != pD3DShader )
		{
			pD3DShader->release();
		}
	}

	const char* ShaderManagerD3D9::getUniqueString()
	{
		m_mutex.Lock();
		static char szName[256];	
		sprintf_s(szName,"ReShaderManagerUnique%d",m_ui32Num++);
		m_mutex.Unlock();
		return szName;
	}

	IShader * ShaderManagerD3D9::_createShaderFromMemory(
		RenderSystemD3D9 * pRenderSystem,
		const std::string & name,
		ShaderType st,
		const uchar *pBuffer,
		uint len )
	{
		if( NULL == pRenderSystem)
			return 0;

		if( NULL == pBuffer || len <= 0)
			return 0;

		ShaderFragmentD3D9 * pShader = new ShaderFragmentD3D9( name);
		if( NULL == pShader )
			return 0;

		if( !pShader->create( pRenderSystem, st, pBuffer, len) )
		{
			pShader->release();
			return 0;
		}
		else
		{
			return pShader;
		}
	}


}