#ifndef __ResourceGT_H__
#define __ResourceGT_H__

#include "IResourceManager.h"
#include "GroundTile.h"

class GroundTile;

class ResourceGT : public IResource
{
public:
	//IResourceData
	virtual size_t size() const;
	virtual void release();
	virtual void* getInnerData();

public:
	ResourceGT()
	{
	}

public:
	bool load(const ulong uResID);
	void draw();

private:
	GroundTileResource	m_res;
	size_t				m_size;
	// 加载地表资源时地图ID作为参数的一部分，这里不需要地图文件名的hash值
	//ulong				_hash;

private:
	void loadAllTextures(GroundTile *pTile);
	void loadTextures(GroundTile *pTile);
	void releaseResource();
};

#endif