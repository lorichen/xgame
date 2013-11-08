#include "stdafx.h"
#include "ResourceMinimap.h"
#include "MinimapTile.h"
#include "IGlobalClient.h"
#include "IEntityClient.h"
#include "ISceneManager2.h"




extern RunType g_runtype;
size_t ResourceMinimap::size() const
{
	return m_pTexture ? m_pTexture->getMemorySize() : 0;
}

void ResourceMinimap::release()
{
	safeRelease(m_pTexture);
	delete this;
}

bool ResourceMinimap::load(const ulong uResID)
{
	int nGridY = uResID & 0x7FF;
	int nGridX = (uResID >> 11) & 0x7FF;
	int nMapID = (uResID >> 22);

	if (g_runtype == RUN_TYPE_GAME)
	{
		IEntityClient* pEntityClient = gGlobalClient->getEntityClient();
		if (NULL != pEntityClient)
		{
			int nCurMapID = pEntityClient->GetMapID();
			if (nMapID != nCurMapID)
			{
				return false;
			}
		}		
	}
	
	ISceneManager2* pSceneManger = gGlobalClient->getSceneManager();
	if (NULL == pSceneManger)
	{
		return false;
	}
	IMinimap* pMiniMap = pSceneManger->getMinimap();
	if (NULL == pMiniMap)
	{
		return false;
	}
	m_pMinimapTile = pMiniMap->getMiniMapTile(nGridX, nGridY);
	if (NULL == m_pMinimapTile)
	{
		return false;
	}
	m_pTexture = m_pMinimapTile->loadResource();

	return m_pTexture != 0;
}

void* ResourceMinimap::getInnerData()
{
	return (void*)m_pTexture;
}