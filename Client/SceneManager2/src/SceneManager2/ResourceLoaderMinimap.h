#ifndef __ResourceLoaderMinimap_H__
#define __ResourceLoaderMinimap_H__

#include "IResourceManager.h"

class ResourceLoaderMinimap : public IResourceLoader
{
private:
	ResourceLoaderMinimap(){}
public:
	static IResourceLoader*	Instance()
	{
		static ResourceLoaderMinimap gt;
		return &gt;
	}
public:
	//IResourceLoader
	virtual IResource* loadResource(const ulong uResID);
	virtual void  release();
};

#endif