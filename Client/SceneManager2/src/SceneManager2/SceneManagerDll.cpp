#include "stdafx.h"
#include "SceneManagerDll.h"
#include "SceneManager.h"

API_EXPORT ISceneManager2* LoadCSM2(xs::IResourceManager *pResourceManager)
{
	return new SceneManager(pResourceManager);
}
