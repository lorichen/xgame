#include "stdafx.h"
#include "ResourceLoaderGT.h"
#include "ResourceGT.h"

//derived from IResourceLoader
IResource* ResourceLoaderGT::loadResource(const ulong uResID)
{
	ResourceGT *pGT = new ResourceGT();
	if(!pGT->load(uResID))
	{
		delete pGT;
		pGT = 0;
	}
	return pGT;
}

void  ResourceLoaderGT::release()
{
}