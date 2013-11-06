#include "stdafx.h"
#include "RenderEngine/Entity2DAniPack.h"
#include "ResourceMz.h"

void ResourceMz::release()
{
	safeRelease(m_pNode);
	delete this;
}

bool ResourceMz::load(const std::string& name)
{
	m_pNode = ModelNodeCreater::create(name.c_str());
	return m_pNode != 0;
}

bool ResourceMz::load(const ConfigCreature* pConfig)
{
	m_pNode = ModelNodeCreater::create((const ConfigModelNode*)pConfig);
	return m_pNode != 0;
}

size_t ResourceMz::size() const
{
	return m_pNode ? m_pNode->getMemorySize() : 0;
}

void* ResourceMz::getInnerData()
{
	return (void*)m_pNode;
}