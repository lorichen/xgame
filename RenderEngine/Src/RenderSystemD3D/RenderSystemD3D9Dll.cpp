#include "StdAfx.h"
#include "RenderSystemD3D9.h"

namespace xs
{
	extern "C" __declspec(dllexport) IRenderSystem* createRenderSystem(RenderEngineCreationParameters* param)
	{
		RenderSystemD3D9 *pRenderSystem = new RenderSystemD3D9;
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