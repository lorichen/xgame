

#ifndef __ModelManagerHelper_H__ext_
#define __ModelManagerHelper_H__ext_

//#if (TARGET_PLATFORM == PLATFORM_WIN32)
#ifdef _DEBUG
	#define	ModelManagerDllName	"xs_mod_d.dll"
#elif RELEASEDEBUG
	#define ModelManagerDllName	"xs_mod_rd.dll"
#else
	#define ModelManagerDllName	"xs_mod.dll"
#endif

//#endif


namespace xs
{

	struct IRenderSystem;
	struct IModelManager;
	struct IModelInstanceManager;

	class ModelManagerHelper
	{
	private:
		HINSTANCE m_hDll;
	public:
		typedef IModelManager* (*createModelManager)(IRenderSystem* pRenderSystem);
		typedef IModelInstanceManager* (*createModelInstanceManager)(IRenderSystem* pRenderSystem);

		ModelManagerHelper() : m_hDll(0)
		{
		}

		~ModelManagerHelper()
		{
		}
        
        void loadDLL()
		{
			if(m_hDll == 0)
			{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
				m_hDll = LoadLibrary(ModelManagerDllName);
#else
                m_hDll = dlopen(ModelManagerDllName, RTLD_NOW);
#endif
			}
		}

		void close()
		{
			if (m_hDll)
			{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
				FreeLibrary(m_hDll);
#else 
                dlclose(m_hDll);
#endif
				m_hDll = 0;
			}
		}

		IModelManager* create(IRenderSystem* pRenderSystem)
		{
			if(!m_hDll)
			{
				loadDLL();
			}

			if(!m_hDll)
				return 0;

			IModelManager *pModule = 0;

			createModelManager proc;
#if (TARGET_PLATFORM == PLATFORM_WIN32)
			proc = (createModelManager)GetProcAddress(m_hDll,"LoadModelManager");
#else
            proc = (createModelManager)getProcAddress(m_hDll,"LoadModelManager");
#endif
			if(proc)
			{
				pModule = proc(pRenderSystem);
			}

			return pModule;
		}

		IModelInstanceManager* _createModelInstanceManager(IRenderSystem *pRenderSystem)
		{
			if(!m_hDll)
			{
				loadDLL();
			}

			if(!m_hDll)
				return 0;

			IModelInstanceManager *pModule = 0;

			createModelInstanceManager proc;
#if (TARGET_PLATFORM == PLATFORM_WIN32)
			proc = (createModelInstanceManager)GetProcAddress(m_hDll,"createModelInstanceManager");
#else
            proc = (createModelInstanceManager)getProcAddress(m_hDll,"createModelInstanceManager");
#endif
			if(proc)
			{
				pModule = proc(pRenderSystem);
			}

			return pModule;
		}

	};

}




#endif