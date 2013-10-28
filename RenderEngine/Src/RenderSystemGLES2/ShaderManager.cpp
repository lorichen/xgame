#include "StdAfx.h"
#include "ShaderManager.h"
#include "Shader.h"
#include "ShaderProgram.h"

namespace xs
{
	IShader*	ShaderManager::createShaderFromFile(ShaderType st,const std::string& fn)
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
				pShader = _createShaderFromMemory(st,pData,len,fn);
				if(pShader)
				{
					do_add(fn,pShader,static_cast<Shader*>(pShader));
				}
				delete[] pData;
			}
		}

		m_mutex.Unlock();
		return pShader;
	}

	IShader*	ShaderManager::createShaderFromMemory(ShaderType st,const uchar *pBuffer,uint len)
	{
		m_mutex.Lock();
		std::string str = getUniqueString();
		IShader *pShader = _createShaderFromMemory(st,pBuffer,len,str);
		if(pShader)
		{
			do_add(str,pShader,static_cast<Shader*>(pShader));
		}
		m_mutex.Unlock();

		return pShader;
	}

	void			ShaderManager::releaseShader(IShader* pShader)
	{
		m_mutex.Lock();
		del(pShader);
		m_mutex.Unlock();
	}

	void			ShaderManager::releaseAllShader()
	{
		m_mutex.Lock();
		std::map<IShader*,ManagedItem*>::iterator e = items.end();
		for(std::map<IShader*,ManagedItem*>::iterator b = items.begin();b != e;++b)
		{
			doDelete((*b).first);
			delete (*b).second;
		}
		items.clear();
		names.clear();
		m_mutex.Unlock();
	}

	/** 释放自己
	*/
	void ShaderManager::release()
	{
		releaseAllShader();
		//不释放自己，因为自己是静态对象.
	}

	const char* ShaderManager::getUniqueString()
	{
		static char szName[256];	
		m_mutex.Lock();	
		sprintf_s(szName,"ReShaderManagerUnique%d",m_ui32Num++);
		m_mutex.Unlock();
		return szName;
	}

	GLenum getGLShaderType(ShaderType st)
	{
		switch(st)
		{
		case ST_VERTEX_PROGRAM:
			return GL_VERTEX_PROGRAM_ARB;
		case ST_FRAGMENT_PROGRAM:
			return GL_FRAGMENT_PROGRAM_ARB;
		}

		return GL_VERTEX_PROGRAM_ARB;
	}
	

	IShader*	ShaderManager::_createShaderFromMemory(ShaderType st,const uchar* pBuffer,uint len,const std::string& name)
	{
		Shader *pShader = new Shader(name);
		if(pShader)
		{
			if(!pShader->create(st,pBuffer,len))
			{
				delete pShader;
				pShader = 0;
			}
		}

		return pShader;
	}

	void ShaderManager::doDelete(IShader* pShader)
	{
		if(pShader)
		{
			GLuint id = static_cast<Shader*>(pShader)->getGLShaderID();
			glDeleteProgramsARB(1,&id);
			delete static_cast<Shader*>(pShader);
		}
	}

}