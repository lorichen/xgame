
#ifndef __RenderSystemHelper_H__
#define __RenderSystemHelper_H__

#include "IRenderSystem.h"

namespace xs{

	class RenderSystemHelper
	{
	private:
		HINSTANCE m_hDll;
		IRenderSystem *m_pModule;
	public:
		typedef IRenderSystem* (*createRenderSystem)(RenderEngineCreationParameters*);
		typedef void (*closeRenderSystem)();

		RenderSystemHelper() : m_hDll(0)
		{
		}

		~RenderSystemHelper()
		{
		}

		void close()
		{
			if(m_hDll)
			{
				FreeLibrary(m_hDll);
				m_hDll = 0;
			}
		}
		IRenderSystem* operator->()
		{
			return m_pModule;
		}

		bool isValid()
		{
			return m_pModule != 0;
		}

		IRenderSystem* create(const char* renderSystemDllName,RenderEngineCreationParameters *param)
		{
			if(!m_hDll)
			{
				loadDLL(renderSystemDllName);
			}

			if(!m_hDll)
				return 0;

			m_pModule = 0;

			createRenderSystem proc;
			proc = (createRenderSystem)GetProcAddress(m_hDll, "createRenderSystem");	
			if(proc)
			{
				m_pModule = proc(param);
			}

			return m_pModule;
		}

		void loadDLL(const char* renderSystemDllName)
		{
			if(m_hDll == 0)
			{
				m_hDll = LoadLibrary(renderSystemDllName);
			}
		}
	};

}

#endif