#ifndef __EnumTileEntities_H__
#define __EnumTileEntities_H__

#include "SceneMgr.h"

//add by yhc
// 枚举范围内的实体或tile,
//枚举时遍历范围内所有的tile
//这个枚举的效率不太高,入参EnumItem和EnumItem数组,这两个临时变量开销有点大
//在服务器端调用频很频繁可考虑直接优化掉临时变量

class EnumRect
{
public:
	EnumRect()
	{
		m_nArrayWidth = 0;
	}
	bool create(int nWidth)
	{
		m_nArrayWidth = nWidth;
		return true;
	}
	bool isValidCo(int x,int y)
	{
		return x >= 0 && x < m_nArrayWidth && x >= 0 && x < m_nArrayWidth;
	}
	void EnumTileArea(const xs::Rect& rcTileArea);
	enum {retFalse = 0,retTrue = 1,retStop = 2};
	virtual int OnFoundTile(int x,int y) = 0;

private:
	int m_nArrayWidth;
};

class EnumEntityByWorldRect : public EnumRect
{
public:
	EnumEntityByWorldRect(SceneMgr* pMap, const xs::Rect &rcWorld, 
		int nMaxItemCount, EnumItem* pListBuf)
	{
		create(pMap->getMapArrayWidth());
		m_pMap = pMap;
		m_nMaxItemCount = nMaxItemCount;
		m_pListBuf = pListBuf;
		m_nFoundCount = 0;
		EnumTileArea(pMap->getSceneCoord().pixelRectToAreaTileRect(rcWorld));
	}
	virtual int OnFoundTile(int x, int y)
	{
		Tile& tile = m_pMap->getTile(x,y);
		if(!tile.isValid())
		{
			return retStop;
		}
		int nItemCount = tile.getLayerCount();
		for (int tt=0; tt<nItemCount; tt++)
		{
			if (m_nFoundCount >= m_nMaxItemCount)
			{
				return retStop;
			}
			m_pListBuf[m_nFoundCount].ptTile.x = x;
			m_pListBuf[m_nFoundCount].ptTile.y = y;
			m_pListBuf[m_nFoundCount].pEntity = tile.getLayer(tt);
			m_nFoundCount++;
		}
		return retTrue;
	}
public:
	int			m_nMaxItemCount;
	int			m_nFoundCount;
	EnumItem*	m_pListBuf;
private:
	SceneMgr*	m_pMap;
};

class EnumTileByWorldRect : public EnumRect
{
public:
	EnumTileByWorldRect(SceneMgr* pMap, const xs::Rect &rcWorld,
		int nMaxTileCount, EnumTile* pListBuf)
	{
		create(pMap->getMapArrayWidth());
		m_pMap = pMap;
		m_nMaxTileCount = nMaxTileCount;
		m_pListBuf = pListBuf;
		m_nFoundCount = 0;
		EnumTileArea(pMap->getSceneCoord().pixelRectToAreaTileRect(rcWorld));
	}
	virtual int OnFoundTile(int x, int y)
	{
		Tile& tile = m_pMap->getTile(x, y);
		if(!tile.isValid())
		{
			return retStop;
		}
		if (m_nFoundCount >= m_nMaxTileCount)
		{
			return retStop;
		}
		m_pListBuf[m_nFoundCount].ptTile.x = x;
		m_pListBuf[m_nFoundCount].ptTile.y = y;
		m_pListBuf[m_nFoundCount].pTile = &tile;
		m_nFoundCount++;

		return retTrue;
	}
public:
	int			m_nMaxTileCount;
	int			m_nFoundCount;
	EnumTile*	m_pListBuf;
private:
	SceneMgr*	m_pMap;
};

#endif
