#include "StdAfx.h"
#include "RenderSystem.h"

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