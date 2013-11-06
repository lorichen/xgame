#ifndef __ResourceLoaderMz_H__
#define __ResourceLoaderMz_H__

#include "IResourceManager.h"

class ResourceLoaderMz : public IResourceLoader
{
private:
	ResourceLoaderMz(){}
public:
	static IResourceLoader*	Instance()
	{
		static ResourceLoaderMz mz;
		return &mz;
	}
public:
	//IResourceLoader
	virtual IResource* loadResource(const ulong uResID);
	virtual void  release();
};

#endif