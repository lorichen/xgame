#include "StdAfx.h"
#include "ModelManager.h"
#include "BoneAnimationSetManager.h"

namespace xs
{
	extern "C" __declspec(dllexport) IModelManager* LoadModelManager(IRenderSystem* pRenderSystem)
	{
		BoneAnimationSetManager::getInstance();
		return new ModelManager(pRenderSystem);
	}
}