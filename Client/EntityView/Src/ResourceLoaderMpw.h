#ifndef __ResourceLoaderMpw_H__
#define __ResourceLoaderMpw_H__

#include "IResourceManager.h"

class ResourceLoaderMpw : public IResourceLoader
{
private:
	ResourceLoaderMpw(){}
public:
	static IResourceLoader*	Instance()
	{
		static ResourceLoaderMpw mpw;
		return &mpw;
	}
public:
	//IResourceLoader
	virtual IResource* loadResource(const ulong uResID);
	virtual void  release();
};

#endif