#include "stdafx.h"
#include "ResourceLoaderMinimap.h"
#include "ResourceMinimap.h"

//derived from IResourceLoader
IResource* ResourceLoaderMinimap::loadResource(const ulong uResID)
{
	ResourceMinimap *pMinimap = new ResourceMinimap;
	if(!pMinimap->load(uResID))
	{
		delete pMinimap;
		pMinimap = 0;
	}
	return pMinimap;
}

void  ResourceLoaderMinimap::release()
{
}