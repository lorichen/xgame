#include "stdafx.h"
#include "SceneMgr.h"
#include "EnumTileEntities.h"
#include "SceneManager.h"

Tile SceneMgr::m_EmptyTile;
SceneMgr::ProcScrollTilesInfo SceneMgr::m_fnScrollTileInfo[mdMaxUpdateType] =
{
	&SceneMgr::moveTop,
	&SceneMgr::moveRightTop,
	&SceneMgr::moveRight,
	&SceneMgr::moveRightDown,
	&SceneMgr::moveDown,
	&SceneMgr::moveLeftDown,
	&SceneMgr::moveLeft,
	&SceneMgr::moveleftTop,
	&SceneMgr::moveAll,
};

SceneMgr::SceneMgr()
{
	m_ptTileOrigin.x = m_ptTileOrigin.y = 0;
	m_nMatrixWidth = m_nMatrixHeight = 0;
	memset(&m_rcMapTileRect,0,sizeof(m_rcMapTileRect));
	m_pMultiValueTable = 0;

	m_pTiles = 0;
	m_pMapTable = 0;
	m_pStream = 0;
	m_pItemCF = 0;
	
	m_nMapWidth = 0;
	m_nMapHeight = 0;
	memset(&m_rcMap,0,sizeof(m_rcMap));

	m_pGround = 0;

	m_bDynamic = true;
	m_pSceneManager = 0;

	m_bWriteOccupants = false;

    m_nMapVersion = ORIGIN_MAP_VERSION;

	for(int i=0;i<MAP_MAXCOUNT;i++)
		m_VectorPreLoad[i] = false;
}

SceneMgr::~SceneMgr()
{
	close();
}
bool SceneMgr::create(GroundEyeshot *pGround,int nMapWidth, int nMapHeight, RECT& rcPreRead,ISceneManager2 *pSceneManager)
{
	m_pSceneManager = pSceneManager;
	m_pGround = pGround;

	m_nMapWidth = nMapWidth;
	m_nMapHeight = nMapHeight;
	SetRect(&m_rcMap, 0, 0, nMapWidth, nMapHeight);

	//m_bDynamic = false;

	m_SceneCo.create(nMapWidth, nMapHeight);

	m_LayerListCache.addCacheList(64);

	if(m_bDynamic)
	{
		POINT ptWorld, ptTileOrigin;
		ptWorld.x = rcPreRead.left;
		ptWorld.y = rcPreRead.top;
		m_SceneCo.pixel2Tile(ptWorld, ptTileOrigin);
		ptTileOrigin.y -= (rcPreRead.bottom - rcPreRead.top)/32 - 1;
		setOriginTile(ptTileOrigin);
	}

	if (!createMapArray(rcPreRead))
	{
		return false;
	}
	PathFinder<SceneMgr>::SetMapInfo(m_SceneCo.getMatrixWidth(), m_SceneCo.getMatrixHeight(),this);

	//如果人物为忽略阻挡则设定该地图为
		CPathFinderByAStar<SceneMgr>::SetMapInfo(m_SceneCo.getMatrixWidth(), m_SceneCo.getMatrixHeight(),this,mem_fun1(&SceneMgr::IsBlock));
	return true;
}

bool SceneMgr::createMapArray(RECT& rcPreRead)
{
	int nCols = Ceil(rcPreRead.right - rcPreRead.left, 64);
	int nRows = Ceil(rcPreRead.bottom - rcPreRead.top, 32);
	m_nMatrixWidth = nCols + nRows;
	m_nMatrixHeight = m_nMatrixWidth - 1;

	m_rcMapTileRect.left = m_rcMapTileRect.top = 0;
	m_rcMapTileRect.right = m_nMatrixWidth;
	m_rcMapTileRect.bottom = m_nMatrixHeight;

	m_pTiles = new Tile[m_nMatrixWidth * m_nMatrixHeight]();

	if(m_pTiles == 0)
		return false;

	m_pMultiValueTable = new DWORD[m_nMatrixHeight];
	if (m_pMultiValueTable == 0)
		return false;
	for (int k=0,nVal=0; k<m_nMatrixHeight; k++,nVal+=m_nMatrixWidth)
		m_pMultiValueTable[k] = nVal;

	initBlockInfo((POINT&)rcPreRead, nRows, nCols*2,nRows,nCols*2);

	return true;
}

void SceneMgr::setOriginTile(POINT& ptTileOrigin)
{
	m_ptTileOrigin = ptTileOrigin;
}

void SceneMgr::scrollTilesInfo(BlockInfo* pBlockInfo, int nRows, int nCols, 
		int nTileRow, int nTileCol, int nDirection, POINT& ptNewTileOrigin, int dx, int dy)

{
	if(!m_pStream)return;
	(this->*m_fnScrollTileInfo[nDirection])(pBlockInfo, nRows, nCols, nTileRow, nTileCol, ptNewTileOrigin, dx, dy);
}

void SceneMgr::initBlockInfo(POINT& ptLeftTop, int nTileRow, int nTileCol,int realRow,int realCol)
{
	POINT ptTileLeftTop;
	m_SceneCo.pixel2Tile(ptLeftTop, ptTileLeftTop);
	POINT ptCurTile;
	for (int row=0; row<realRow; row++)
	{
		ptCurTile.x = ptTileLeftTop.x + row;
		ptCurTile.y = ptTileLeftTop.y - row;
		for (int col=0; col<realCol; col++)
		{
			POINT ptWorld;
			m_SceneCo.tile2Pixel(ptCurTile,ptWorld);
			Tile& tile = getTile(ptCurTile);
			if(row >= nTileRow || col >= nTileCol)
			{
				// by yhc 2011.5.21
				//以前动态滚动地图的情况下需要重设标志
				//现在因自动寻路需要所有阻挡信息,tile信息已经在地图加载的时候初始化好了,不需要再初始化
				//tile.setFlag(0);;
			}
			else
			{
				tile.create();
			}

			ptCurTile.x += !(col&1);
			ptCurTile.y += col&1;
		}
	}
}

void SceneMgr::loadBlockInfo(POINT& ptLeftTop, int nIndex, int nTileRow, int nTileCol)
{
	int row = nTileRow;
	int col = nTileCol;
	if (ptLeftTop.x + GRID_WIDTH > m_nMapWidth)
		nTileCol = Ceil(m_nMapWidth - ptLeftTop.x, 32);
	if (ptLeftTop.y + GRID_HEIGHT > m_nMapHeight)
		nTileRow = Ceil(m_nMapHeight - ptLeftTop.y, 32);
	initBlockInfo(ptLeftTop, nTileRow, nTileCol,row,col);
	m_pStream->seek(m_pMapTable[nIndex]);
	POINT ptTileLeftTop;
	m_SceneCo.pixel2Tile(ptLeftTop, ptTileLeftTop);
	_LoadBlock(m_pStream, ptTileLeftTop, nTileRow, nTileCol, m_pItemCF);
	m_pStream->seekToBegin();
}

void SceneMgr::copyBlockInfo(POINT& ptSrcLeftTop, POINT& ptDstLeftTop, int nTileRow, int nTileCol)
{
	POINT ptSrcTileLeftTop, ptDstTileLeftTop;
	m_SceneCo.pixel2Tile(ptSrcLeftTop, ptSrcTileLeftTop);
	m_SceneCo.pixel2Tile(ptDstLeftTop, ptDstTileLeftTop);
	int dx = ptDstTileLeftTop.x - ptSrcTileLeftTop.x;
	int dy = ptDstTileLeftTop.y - ptSrcTileLeftTop.y;
	for (int row=0; row<nTileRow; row++)
	{
		POINT ptCurTile;
		ptCurTile.x = ptSrcTileLeftTop.x + row;
		ptCurTile.y = ptSrcTileLeftTop.y - row;
		for (int col=0; col<nTileCol; col++)
		{
			getTile(ptCurTile.x + dx, ptCurTile.y + dy) = getTile(ptCurTile);
			
			ptCurTile.x += !(col&1);
			ptCurTile.y += col&1;
		}
	}
}

void SceneMgr::copyBlockInfoForSurface(POINT& ptSrcLeftTop, POINT& ptDstLeftTop, int nTileRow, int nTileCol)
{
	POINT ptSrcTileLeftTop, ptDstTileLeftTop;
	m_SceneCo.pixel2Tile(ptSrcLeftTop, ptSrcTileLeftTop);
	m_SceneCo.pixel2Tile(ptDstLeftTop, ptDstTileLeftTop);
	int dx = ptDstTileLeftTop.x - ptSrcTileLeftTop.x;
	int dy = ptDstTileLeftTop.y - ptSrcTileLeftTop.y;
	if (dx == 0 && dy == 0)
		return;
	for (int row=0; row<nTileRow; row++)
	{
		POINT ptCurTile;
		ptCurTile.x = ptSrcTileLeftTop.x + row;
		ptCurTile.y = ptSrcTileLeftTop.y - row;
		for (int col=0; col<nTileCol; col++)
		{
			Tile& srcTile = getTile(ptCurTile);
			DWORD dwFlag = srcTile.getFlag();
			dwFlag = srcTile.isOccupant_Surface();

			Tile& dstTile = getTile(ptCurTile.x + dx, ptCurTile.y + dy);
			DWORD dwFlagDstItem = dstTile.getFlag();
			dwFlagDstItem &= ~ Tile::tOccupant_Surface;
			dstTile.getLayerCount();
			dwFlagDstItem |= dwFlag;
			dstTile.setFlag(dwFlagDstItem);
			
			ptCurTile.x += !(col&1);
			ptCurTile.y += col&1;
		}
	}
}

void SceneMgr::close()
{
#if defined(RELEASEDEBUG) || defined(_DEBUG)
	m_mapTileInfo.clear();
#endif
	m_nMapVersion = ORIGIN_MAP_VERSION;

	SceneManager * pRealSceneMgr = static_cast<SceneManager * >( m_pSceneManager);
	SceneEntityList::iterator it   =  m_EntityList.begin();
	SceneEntityList::iterator end  =  m_EntityList.end();
	for(;it != end;)
	{
		bool ret = pRealSceneMgr->IsReferencedBySceneManager( (*it) );
		if( ret ) 
		{
			++it;
			continue;
		}

		if (m_pSceneManager->getRunType() == RUN_TYPE_GAME)
		{
			EntityView *pEntity = (*it);
			pEntity->removeFlag(flagFade);
			// 地图上的物件(主要是magicview)在切换地图，原先没有正确释放掉，这里增加flagReleaseImmediate标志，使其立即释放避免内存泄露 [5/5/2011 zgz]
			pEntity->addFlag(flagReleaseImmediate);
			pEntity->update(0,0,0);
			safeRelease((*it));
		}		
		it = m_EntityList.erase(it);
	}
	safeDeleteArray(m_pMapTable);
	safeDeleteArray(m_pMultiValueTable);
	safeDeleteArray(m_pTiles);
	if(m_pTiles)m_nMatrixWidth = 0;
	m_EntityList.clear();
	
	m_LayerListCache.close();
}


bool SceneMgr::moveEntity(const POINT& ptFrom, const POINT& ptTo, EntityView* pEntity)
{
	if (!removeEntity(ptFrom, pEntity))
		return false;
	return addEntity(ptTo, pEntity);
}

bool SceneMgr::addLayerItem(const POINT &pt, EntityView *pEntity)
{
	return addEntity(pt,pEntity);
}

bool SceneMgr::addEntity(const POINT& ptTile, EntityView* pEntity)
{
	if (pEntity == 0)
		return 0;

	Tile& tile = getTile(ptTile);
	if (!tile.isValid())
	{
		POINT pt;
		m_SceneCo.tile2Pixel(ptTile, pt);
		int nGridX = pt.x >> SHIFT_WIDTH;
		int nGridY = pt.y >> SHIFT_HEIGHT;
		_Info("addEntity:fail"<<_fi("0x%08x", (ulong)pEntity)<<endl);
		return false;
	}

	if (tile.getMapEntity() == 0)
	{
		_Info("addEntity:ok"<<_fi("0x%08x", (ulong)pEntity)<<endl);
		tile.setOneLayer(pEntity);
	}
	else
	{ 
		if (tile.isMapEntityValid())
		{
			EntityView *pFirst = tile.getMapEntity();
			tile.setNoLayer();
			if (!m_LayerListCache.addData(tile.getLayerListPointer(), pFirst))
			{
				_Info("addEntity:fail"<<_fi("0x%08x", (ulong)pEntity)<<endl);
				return false;
			}
		}

		if (!m_LayerListCache.addData(tile.getLayerListPointer(), pEntity))
		{
			_Info("addEntity:fail"<<_fi("0x%08x", (ulong)pEntity)<<endl);
			return false;
		}
		else
		{
			_Info("addEntity:ok"<<_fi("0x%08x", (ulong)pEntity)<<endl);
		}
	}

	if (pEntity->getOccupantTileList() == 0)
	{
		//by yhc,2010.3.28,去掉物件锚点的阻挡信息 
		//tile.set_Occupant_Entity(pEntity->getOccupantValue());
	}
	else
		addMultiOccupantValue(ptTile, pEntity);

	pEntity->setTile(ptTile);
	pEntity->addFlag(flagInMap);

	_Info("addEntity:ok"<<_fi("0x%08x", (ulong)pEntity)<<endl);
	return true;
}

bool SceneMgr::removeEntity(const POINT& ptTile, EntityView* pEntity)
{
	//zgz 由于物件和地表一样，进入ViewPort时会重新加载，所以此处需要删除
	if (NULL != m_pSceneManager && m_pSceneManager->getRunType() == RUN_TYPE_GAME)
	{		
		m_EntityList.remove(pEntity);
	}
	
	Tile& tile = getTile(ptTile);
	if (!tile.isValid())
	{
		POINT ptOff (ptTile.x - m_ptTileOrigin.x, ptTile.y - m_ptTileOrigin.y);
		if (ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight)
		{
			POINT pt, ptLeftTop, ptTileLeftTop = m_ptTileOrigin;
			m_SceneCo.tile2Pixel(ptTile, pt);
			ptTileLeftTop.y -= TILES_PREREAD_HEIGHT / 32 - 1;
			m_SceneCo.tile2Pixel(ptTileLeftTop, ptLeftTop);
			RECT rcPreRead;
			rcPreRead.left = ptLeftTop.x;
			rcPreRead.top = ptLeftTop.y;
			rcPreRead.right = rcPreRead.left + TILES_PREREAD_WIDTH;
			rcPreRead.bottom = rcPreRead.top + TILES_PREREAD_HEIGHT;
			if (PtInRect(&rcPreRead, pt))
			{
				int nGridX = pt.x >> SHIFT_WIDTH;
				int nGridY = pt.y >> SHIFT_HEIGHT;
				_Info("removeEntity:fail"<<_fi("0x%08x", (ulong)pEntity)<<endl);
				return false;
			}
			_Info("removeEntity:ok"<<_fi("0x%08x", (ulong)pEntity)<<endl);
			return true;
		}
		_Info("removeEntity:ok"<<_fi("0x%08x", (ulong)pEntity)<<endl);
		return true;
	}

	if (tile.getMapEntity() == 0)
	{
		POINT pt;
		m_SceneCo.tile2Pixel(ptTile, pt);
		int nGridX = pt.x >> SHIFT_WIDTH;
		int nGridY = pt.y >> SHIFT_HEIGHT;

		//_Info("removeEntity Failed:"<<_fi("0x%08x", (ulong)pEntity)<<" tile.getMapEntity() == 0"<<endl);
		_Info("removeEntity:fail"<<_fi("0x%08x", (ulong)pEntity)<<endl);
		return false;
	}
	else if (tile.isMapEntityValid())
	{
		if (tile.getMapEntity() != pEntity)
		{
			//_Info("removeEntity Failed:"<<_fi("0x%08x", (ulong)pEntity)<<" tile.getMapEntity() != pEntity"<<endl);
			_Info("removeEntity:fail"<<_fi("0x%08x", (ulong)pEntity)<<endl);
			return false;
		}
		_Info("removeEntity:ok"<<_fi("0x%08x", (ulong)pEntity)<<endl);
		tile.setNoLayer();
	}
	else
	{
		if (!m_LayerListCache.removeData(tile.getLayerListPointer(), pEntity))
		{
			//_Info("removeEntity Failed:"<<_fi("0x%08x", (ulong)pEntity)<<" m_LayerListCache.removeData"<<endl);
			_Info("removeEntity:fail"<<_fi("0x%08x", (ulong)pEntity)<<endl);
			return false;
		}
		else
		{
			_Info("removeEntity:ok"<<_fi("0x%08x", (ulong)pEntity)<<endl);
		}
	}
	
	if (pEntity->getOccupantTileList() == 0)
	{
		tile.removeEntityOccupant(pEntity->getOccupantValue());
	}
	else
	{
		removeMultiOccupantValue(ptTile, pEntity);
	}

	pEntity->removeFlag(flagInMap);
	_Info("removeEntity:ok"<<_fi("0x%08x", (ulong)pEntity)<<endl);
	return true;
}

bool SceneMgr::addEntityOccupant(const POINT& ptTile, EntityView* pEntity)
{
	if(pEntity == 0)
		return false;

	Tile& tile = getTile(ptTile);
	if (!tile.isValid())
		return false;

	if (pEntity->getOccupantTileList() == 0)
		tile.set_Occupant_Entity(pEntity->getOccupantValue());
	else
		addMultiOccupantValue(ptTile, pEntity);

	return true;
}

bool SceneMgr::removeEntityOccupant(const POINT& ptTile, EntityView* pEntity)
{
	if(pEntity == 0)
		return false;

	Tile& tile = getTile(ptTile);
	if (!tile.isValid())
		return false;

	if (pEntity->getOccupantTileList() == 0)
		tile.removeEntityOccupant(pEntity->getOccupantValue());
	else
		removeMultiOccupantValue(ptTile, pEntity);
	
	return true;
}

void SceneMgr::addMultiOccupantValue(POINT pt, EntityView *pEntity)
{
	OccupantTileList* pList = pEntity->getOccupantTileList();
	pt.x += pList->getParamData().nOffAnchorX;
	pt.y += pList->getParamData().nOffAnchorY;
	int sw = pList->getParamData().wWidth;
	int sh = pList->getParamData().wHeight;
	if(pList->getCount() == 0)
	{
		int xFrom = pt.x;
		for(int y = sh; y > 0; y--,pt.y++)
		{
			for(int x = sw; x > 0 ;x--,pt.x++)
			{
				Tile& tile = getTile(pt);
				if(tile.isValid())
				{
					//if(pt.x==633 && pt.y==438)
					//	int i=0;
					tile.set_Occupant_Entity();
				}
			}
			pt.x = xFrom;
		}
	}
	else
	{
		char* pL = (char*)pList->getDataBuffer();;
		int xFrom = pt.x;
		for(int y = sh; y > 0; y--,pt.y++)
		{
			char*p = pL;
			for(int x = sw; x > 0 ;x--,p++,pt.x++)
				if(*p)
				{
					Tile& tile = getTile(pt);

					//if(pt.x==633 && pt.y==438)
					//	int i=0;

					if(tile.isValid())
						tile.set_Occupant_Entity_Byte(*p);
				}
			pt.x = xFrom;
			pL += sw;
		}
	}
}

void SceneMgr::removeMultiOccupantValue(POINT pt, EntityView *pEntity)
{
	OccupantTileList* pList = pEntity->getOccupantTileList();
	pt.x += pList->getParamData().nOffAnchorX;
	pt.y += pList->getParamData().nOffAnchorY;
	int sw = pList->getParamData().wWidth;
	int sh = pList->getParamData().wHeight;
	if(pList->getCount() == 0)
	{
		int xFrom = pt.x;
		for(int y = sh; y > 0; y--,pt.y++)
		{
			for(int x = sw; x > 0 ;x--,pt.x++)
			{
				Tile& tile = getTile(pt);
				if(tile.isValid())
				{
					tile.clear_Occupant_Entity_Byte();
					tile.onUpdateItems();
				}
			}
			pt.x = xFrom;
		}
	}
	else
	{
		char* pL = (char*)pList->getDataBuffer();;
		int xFrom = pt.x;
		for(int y = sh; y > 0; y--,pt.y++)
		{
			char*p = pL;
			for(int x = sw; x > 0 ;x--,p++,pt.x++)
				if(*p)
				{
					Tile& tile = getTile(pt);
					if(tile.isValid())
					{
						tile.clear_Occupant_Entity_Byte(*p);
						tile.onUpdateItems();
					}
				}
			pt.x = xFrom;
			pL += sw;
		}
	}
}

bool SceneMgr::testIntersect(POINT pt, EntityView* pEntity)
{
	OccupantTileList* pList = pEntity->getOccupantTileList();
	pt.x += pList->getParamData().nOffAnchorX; 
	pt.y += pList->getParamData().nOffAnchorY;
	int sw = pList->getParamData().wWidth;
	int sh = pList->getParamData().wHeight;
	if(pList->getCount() == 0)
	{
		int xFrom = pt.x;
		for(int y = sh; y > 0; y--,pt.y++)
		{
			for(int x = sw; x > 0 ;x--,pt.x++)
			{
				Tile& tile = getTile(pt);
				if(tile.isValid() &&tile.isBlock())
					return false;
			}
			pt.x = xFrom;
		}
	}
	else
	{
		char* pL = (char*)pList->getDataBuffer();;
		int xFrom = pt.x;
		for(int y = sh; y > 0; y--,pt.y++)
		{
			char*p = pL;
			for(int x = sw; x > 0 ;x--,p++,pt.x++)
				if(*p)
				{
					Tile& tile = getTile(pt);
					if(tile.isValid() &&tile.isBlock())
						return false;
				}
				pt.x = xFrom;
				pL += sw;
		}
	}
	return true;
}

bool SceneMgr::isMovableForMultiOcc(EntityView* pEntity, POINT ptFrom, POINT ptTo)
{
	OccupantTileList* pList = pEntity->getOccupantTileList();
	POINT pt = ptFrom;
	pt.x += pList->getParamData().nOffAnchorX; 
	pt.y += pList->getParamData().nOffAnchorY;
	int sw = pList->getParamData().wWidth;
	int sh = pList->getParamData().wHeight;
	int dx = ptTo.x - ptFrom.x;
	int dy = ptTo.y - ptFrom.y;

	char* pL = (char*)pList->getDataBuffer();
	int nOffset = sw*dy + dx;
	for (; pt.y<ptTo.y; pt.y++)
	{
		char* p = pL;
		for (; pt.x<ptTo.x; pt.x++,p++)
		{
			if (*p)
			{
				Tile& tile = getTile(pt.x + dx, pt.y + dy);
				if (tile.isValid() && tile.isBlock() && *(pL+nOffset) == 0)
					return false;
			}
		}
		pL += sw;
	}
	return true;
}

bool SceneMgr::AdjustTileRect(RECT &rc)
{
	if(rc.right < 0 || rc.left >= m_nMatrixWidth || rc.bottom < 0 || rc.top < 0 || rc.top >= m_nMatrixWidth)
	{
		return false;
	}
	if(rc.left < 0)
	{
		rc.right +=  rc.left / 2;
		rc.bottom +=  rc.left / 2;
		rc.left = 0;
	}
	if(rc.left + rc.right > m_nMatrixWidth)
		rc.right = 	m_nMatrixWidth - rc.left;

	if(rc.top + rc.right > m_nMatrixWidth)
		rc.right = m_nMatrixWidth - rc.top;
	if(rc.top - rc.bottom + 1 < 0)
		rc.bottom = rc.top + 1;

	if(rc.left + rc.right + rc.bottom - 1 > m_nMatrixWidth)
		rc.bottom = m_nMatrixWidth - rc.left - rc.right + 1;
	
	return true;
}

bool SceneMgr::snapshotRectItem(const RECT& rc,int& nListCount,EnumItem* pListBuf)
{
	if(pListBuf == 0 || nListCount < 1)
	{
		return false;
	}
	RECT rcArea;
	if(!IntersectRect(&rcArea,&rc,&m_rcMapTileRect))
		return false;
	int nFoundCount = 0;

	int txEnd = rcArea.right;
	int tyEnd = rcArea.bottom;
	Tile* pTileLeft = &m_pTiles[rcArea.top * m_nMatrixWidth + rcArea.left];
	Tile* pTile = pTileLeft;
	for(int ty = rcArea.top; ty < tyEnd; ty++)
	{
		for(int tx = rcArea.left; tx < txEnd; tx++,pTile++)
		{
			int nCount = pTile->getLayerCount();
			if(nCount)
			{
				for(int i = 0; i < nCount; i++)
				{
					if(nFoundCount >= nListCount)
					{
						goto END_SnapshotRectItem;
					}
					pListBuf[nFoundCount].ptTile.x = tx;
					pListBuf[nFoundCount].ptTile.y = ty;
					pListBuf[nFoundCount].pEntity = pTile->getLayer(i);
					nFoundCount++;
				}
			}
		}
		pTileLeft += m_nMatrixWidth;
		pTile = pTileLeft;
	}

END_SnapshotRectItem:
	nListCount = nFoundCount;
	return true;
}



bool SceneMgr::enumEntityByWorldRect(const RECT &rcWorld,int& nListCount, EnumItem* pListBuf)
{
	if (pListBuf == 0 || nListCount < 1)
	{
		return false;
	}
	EnumEntityByWorldRect snap(this, rcWorld, nListCount, pListBuf);
	nListCount = snap.m_nFoundCount;
	return nListCount > 0;
}

bool SceneMgr::enumTileByWorldRect(const RECT &rcWorld,int& nListCount,EnumTile* pListBuf)
{
	if (pListBuf == 0 || nListCount < 1)
	{
		return false;
	}
	EnumTileByWorldRect snap(this, rcWorld, nListCount, pListBuf);
	nListCount = snap.m_nFoundCount;
	return nListCount > 0;
}

#if defined(RELEASEDEBUG) || defined(_DEBUG)
void SceneMgr::AddTileInfo(POINT ptTile, string strFileName)
{
	DWORD dwTile = ptTile.x;
	dwTile <<= 16;
	dwTile |= ptTile.y;

	TMAP_TILEINFO::iterator iter = m_mapTileInfo.find(dwTile);
	if (iter == m_mapTileInfo.end())
	{
		TLIST_NAME listName;
		listName.push_back(strFileName);
		m_mapTileInfo.insert(pair<DWORD, TLIST_NAME>(dwTile, listName));
	}
	else
	{
		TLIST_NAME& listName = (*iter).second;
		listName.push_back(strFileName);

		
		ErrorLn("有物件重复加载!");		
		Error( m_pSceneManager->getMapFileName() );
		char buf[32];
		sprintf_s(buf, 32, "坐标 x:%d y:%d", ptTile.x, ptTile.y);
		ErrorLn(buf);
		for (TLIST_NAME::iterator itName = listName.begin(); itName != listName.end(); ++itName)
		{
			ErrorLn((*itName).c_str());
		}
	}
}
#endif