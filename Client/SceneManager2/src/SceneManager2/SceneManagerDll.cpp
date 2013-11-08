#include "stdafx.h"
#include "SceneManagerDll.h"
#include "SceneManager.h"

extern "C" __declspec(dllexport) ISceneManager2* LoadCSM2(xs::IResourceManager *pResourceManager)
{
	return new SceneManager(pResourceManager);
}
