#ifndef __ModelManagerHelper_H__
#define __ModelManagerHelper_H__

#ifdef _DEBUG
	#define	ModelManagerDllName	"xs_mod_d.dll"
#elif RELEASEDEBUG
	#define ModelManagerDllName	"xs_mod_rd.dll"
#else
	#define ModelManagerDllName	"xs_mod.dll"
#endif

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

		void close()
		{
			if (m_hDll)
			{
				FreeLibrary(m_hDll);
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
			proc = (createModelManager)GetProcAddress(m_hDll,"LoadModelManager");	
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
			proc = (createModelInstanceManager)GetProcAddress(m_hDll,"createModelInstanceManager");	
			if(proc)
			{
				pModule = proc(pRenderSystem);
			}

			return pModule;
		}

		void loadDLL()
		{
			if(m_hDll == 0)
			{
				m_hDll = LoadLibrary(ModelManagerDllName);
			}
		}
	};

}

#endif