#include "StdAfx.h"
#include "HighLevelShaderManager.h"
#include "HighLevelShader.h"
#include "HighLevelShaderProgram.h"

namespace xs
{
	IShader*	HighLevelShaderManager::createShaderFromFile(ShaderType st,const std::string& fn)
	{
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
				uchar *pData = new uchar[len + 1];
				pStream->read(pData,len);
				pData[len] = 0;
				pShader = _createShaderFromMemory(st,pData,len + 1,fn);
				if(pShader)
				{
					do_add(fn,pShader,static_cast<HighLevelShader*>(pShader));
				}
				delete[] pData;
			}
		}

		return pShader;
	}

	//pBuffer must be Null-terminated
	IShader*	HighLevelShaderManager::createShaderFromMemory(ShaderType st,const uchar *pBuffer,uint len)
	{
		std::string str = getUniqueString();
		IShader *pShader = _createShaderFromMemory(st,pBuffer,len,str);
		if(pShader)
		{
			do_add(str,pShader,static_cast<HighLevelShader*>(pShader));
		}

		return pShader;
	}

	void			HighLevelShaderManager::releaseShader(IShader* pShader)
	{
		del(pShader);
	}

	void			HighLevelShaderManager::releaseAllShader()
	{
		std::map<IShader*,ManagedItem*>::iterator e = items.end();
		for(std::map<IShader*,ManagedItem*>::iterator b = items.begin();b != e;++b)
		{
			doDelete((*b).first);
			delete (*b).second;
		}
		items.clear();
		names.clear();
	}

	const char* HighLevelShaderManager::getUniqueString()
	{
		static char szName[256];
		/*do
		{
			str.format("ReHighLevelShaderManagerUnique%d",m_ui32Num++);
		}
		while(names.find(str) != names.end());*/
		
		sprintf_s(szName,"ReHighLevelShaderManagerUnique%d",m_ui32Num++);

		return szName;
	}
	

	/** 释放自己
	*/
	void HighLevelShaderManager::release()
	{
		releaseAllShader();
		//因为是静态变量，所以不需要释放自己
	}

	IShader*	HighLevelShaderManager::_createShaderFromMemory(ShaderType st,const uchar* pBuffer,uint len,const std::string& name)
	{
		HighLevelShader *pShader = new HighLevelShader(name);
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

	void HighLevelShaderManager::doDelete(IShader* pShader)
	{
		if(pShader)
		{
			glDeleteObjectARB(static_cast<HighLevelShader*>(pShader)->getHandle());
			delete static_cast<HighLevelShader*>(pShader);
		}
	}

}