#ifndef __ResourceMinimap_H__
#define __ResourceMinimap_H__

#include "IResourceManager.h"

class MinimapTile;

class ResourceMinimap : public IResource
{
public:
	//IResourceData
	virtual size_t size() const;
	virtual void release();
	virtual void* getInnerData();
public:
	ResourceMinimap() : m_pMinimapTile(0),m_pTexture(0){}
public:
	bool load(const ulong uResID);
	void draw();
private:
	MinimapTile*	m_pMinimapTile;
	ITexture*		m_pTexture;
};

#endif