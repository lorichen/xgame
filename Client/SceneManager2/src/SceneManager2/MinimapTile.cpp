#include "stdafx.h"
#include "MinimapTile.h"
#include "MinimapEyeshot.h"

MinimapTile::MinimapTile()
{
	m_handle = 0;
	m_pParent = 0;
}

MinimapTile::~MinimapTile()
{
}

ITexture* MinimapTile::loadResource()
{
	const std::string& path = m_pParent->getPath();
	char filename[MAX_PATH];
	sprintf(filename,"%s\\%d_%d.dds",path.c_str(),m_ptCoord.x,m_ptCoord.y);

	m_pParent->loadMinimapMask();
	return getRenderEngine()->getRenderSystem()->getTextureManager()->createTextureFromFile(filename,FO_LINEAR,FO_LINEAR,FO_NONE,TAM_CLAMP_TO_EDGE,TAM_CLAMP_TO_EDGE);
}

ITexture* MinimapTile::getTexture()
{
	if(!m_handle || !isValidHandle(m_handle))return 0;

	IResourceNode* node = (IResourceNode*)getHandleData(m_handle);
	if (node)
	{
		IResource* res = node->getResource();
		if (res)
		{
			return (ITexture*)res->getInnerData();
		}
	}

	return 0;
}