#ifndef __ResourceMpw_H__
#define __ResourceMpw_H__

#include "IResourceManager.h"
#include "RenderEngine/Entity2DAniPack.h"

class ResourceMpw : public IResource
{
public:
	//IResourceData
	virtual size_t size() const;
	virtual void release();
	virtual void* getInnerData();
public:
	ResourceMpw() : m_pMpw(0){}
public:
	bool load(const std::string& name);
	void draw();
private:
	Entity2DAniPack*	m_pMpw;
};

#endif