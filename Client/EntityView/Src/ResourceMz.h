#ifndef __ResourceMz_H__
#define __ResourceMz_H__

#include "IResourceManager.h"

class xs::ModelNode;
class CEntity2DAni;
struct ConfigCreature;

class ResourceMz : public IResource
{
public:
	//IResourceData
	virtual size_t size() const;
	virtual void release();
	virtual void* getInnerData();
public:
	ResourceMz() : m_pNode(0){}
public:
	bool load(const std::string& name);
	bool load(const ConfigCreature* pConfig);
	void draw();
private:
	xs::ModelNode*	m_pNode;
};

#endif