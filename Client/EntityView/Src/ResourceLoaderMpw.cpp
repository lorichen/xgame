#include "stdafx.h"
#include "ResourceLoaderMpw.h"
#include "ResourceMpw.h"
#include "MWDLoader.h"

//derived from IResourceLoader
IResource* ResourceLoaderMpw::loadResource(const ulong uResID)
{
	const std::string& filename = MWPFileNameMap::getInstance().find(uResID);
	ResourceMpw *pMpw = new ResourceMpw;
	if(!pMpw->load(filename))
	{
		delete pMpw;
		pMpw = 0;
	}
	return pMpw;
}

void  ResourceLoaderMpw::release()
{
}