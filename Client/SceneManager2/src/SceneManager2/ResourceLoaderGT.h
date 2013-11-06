#ifndef __ResourceLoaderGT_H__
#define __ResourceLoaderGT_H__

#include "IResourceManager.h"

class ResourceLoaderGT : public IResourceLoader
{
public:
	ResourceLoaderGT(){}
public:
	//IResourceLoader
	virtual IResource* loadResource(const ulong uResID);
	virtual void  release();

	// 加载地表资源时地图ID作为参数的一部分，这里不需要地图文件名的hash值
	//ulong _hash;
};

#endif