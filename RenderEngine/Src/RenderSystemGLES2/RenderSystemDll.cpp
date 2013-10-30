#include "StdAfx.h"
#include "RenderSystem.h"

#if (TARGET_PLATFORM == PLATFORM_WIN32)
#pragma comment(lib, "libEGL.lib")
#pragma comment(lib, "libGLESv2.lib")
#else

#endif

namespace xs
{

	extern "C" __declspec(dllexport) IRenderSystem* createRenderSystem(RenderEngineCreationParameters* param)
	{
		PP_BY_NAME("dllexport createRenderSystem");
		RenderSystem *pRenderSystem = new RenderSystem;
		if(pRenderSystem)
		{
			if(!pRenderSystem->create(param))
			{
				delete pRenderSystem;
				pRenderSystem = 0;
			}
		}

		return pRenderSystem;
	}
}