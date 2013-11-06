#include "stdafx.h"
#include "SceneManager.h"

#define MAX_GRID_EnumItem_COUNT	1024


EntityView*	SceneEyeshot::getFirstEntity()
{
	m_it = m_EntityList.getListPtr()->begin();
	return getNextEntity();
}

EntityView*	SceneEyeshot::getNextEntity()
{
	if(m_it == m_EntityList.getListPtr()->end())return 0;

	return (*m_it++).pEntity;
}

SceneEyeshot::SceneEyeshot()
{
	m_pSceneManager = 0;
	m_EntityList.clear();
}

SceneEyeshot::~SceneEyeshot()
{
}

bool SceneEyeshot::create(int nMapWidth, int nMapHeight, POINT& ptPlayerPos, SceneManager* pSceneManager,bool bDynamic)
{
	m_pSceneManager = pSceneManager;

	m_GridMgr.create(this,nMapWidth, nMapHeight, ptPlayerPos,bDynamic);

	return true;
}

void SceneEyeshot::close()
{
	m_EntityList.clear();
	m_GridMgr.close();
}


bool SceneEyeshot::viewportSizeChanged(int nWidth,int nHeight)
{
	return m_GridMgr.viewportSizeChanged(nWidth, nHeight);
}

bool SceneEyeshot::scroll2Center(POINT& ptPlayerPos)
{
	return m_GridMgr.scroll2Center(ptPlayerPos);
}

void SceneEyeshot::onChanged(int nMoveDirection, int nDx, int nDy)	
{
	PP_BY_NAME("SceneEyeshot::onChanged");
	RECT rcPreRead;
	POINT ptTileOrigin;
	m_GridMgr.getGridRectBelowPreRead(rcPreRead);
	m_pSceneManager->getSceneCoord().pixel2Tile((const POINT&)rcPreRead, ptTileOrigin);
	ptTileOrigin.y -= (rcPreRead.bottom - rcPreRead.top)/32 - 1;

	BlockInfo BlockInfo[TILE_GRID_ROW * TILE_GRID_COL * 4];
	int nRows, nCols;
	m_GridMgr.snapshotLeftTop(nRows, nCols, BlockInfo, nMoveDirection);
	RECT& rc = m_GridMgr.getPreReadGrid();
	RECT& rc1 = m_GridMgr.getViewGrid();
	POINT ptPlayerGrid = m_GridMgr.getPlayerGrid();
	m_pSceneManager->getSceneMgr().scrollTilesInfo(BlockInfo, nRows, nCols, GRID_HEIGHT/32, GRID_WIDTH*2/64, nMoveDirection, ptTileOrigin, nDx, nDy);
}

void SceneEyeshot::onFound(int nGridX, int nGridY)
{
	PP_BY_NAME("SceneEyeshot::onFound");
	RECT rc;
	rc.left = nGridX << SHIFT_WIDTH;
	rc.top = nGridY << SHIFT_HEIGHT;
	rc.right = rc.left + GRID_WIDTH;
	rc.bottom = rc.top + GRID_HEIGHT;
	EnumItem ListBuf[MAX_GRID_EnumItem_COUNT];
	int nListCount = MAX_GRID_EnumItem_COUNT;

	int bFirstTimeLoad = m_GridMgr.isFirstTimeLoad();
	if (m_pSceneManager->enumEntityByWorldRect(rc, nListCount, (SnapshotItemInfo*)ListBuf))
	{
		for (int i=0; i<nListCount; i++)
		{
			m_pSceneManager->OnEntityEnterViewport(ListBuf[i].ptTile, ListBuf[i].pEntity);
		}
	}

	if (!m_GridMgr.isDynamic())
		return;

	EntityInfo* pInfo = m_EntityList.findFirstItem(nGridX, nGridY);
	while (pInfo != 0)
	{
		//m_pSceneManager->addEntity(pInfo->ptTile, (EntityView*)pInfo->pEntity, 1);
		
		//by yhc 修正NPC看不见的问题 
		m_pSceneManager->OnEntityEnterViewport(pInfo->ptTile, (EntityView*)pInfo->pEntity);
		//end

		pInfo = m_EntityList.findNextItem(nGridX, nGridY);
	}
}

void SceneEyeshot::onLost(int nGridX, int nGridY)
{
	PP_BY_NAME("SceneEyeshot::onLost");
	RECT rc;
	rc.left = nGridX << SHIFT_WIDTH;
	rc.top = nGridY << SHIFT_HEIGHT;
	rc.right = rc.left + GRID_WIDTH;
	rc.bottom = rc.top + GRID_HEIGHT;
	EnumItem ListBuf[MAX_GRID_EnumItem_COUNT];
	int nListCount = MAX_GRID_EnumItem_COUNT;
	if (m_pSceneManager->enumEntityByWorldRect(rc, nListCount, (SnapshotItemInfo*)ListBuf))
	{
		for (int i=0; i<nListCount; i++)
		{
			m_pSceneManager->OnEntityLeaveViewport(ListBuf[i].ptTile, ListBuf[i].pEntity);
		}
	}
}

void SceneEyeshot::AddItemToList(EntityView* pEntity, const POINT& ptTile)
{
	if(!m_pSceneManager)return;
	SceneCoord& coord = m_pSceneManager->getSceneCoord();
	POINT ptGrid, ptTileCenter;
	coord.tile2Pixel(ptTile, ptTileCenter);
	ptGrid.x = ptTileCenter.x >> SHIFT_WIDTH;
	ptGrid.y = ptTileCenter.y >> SHIFT_HEIGHT;
	if (PtInRect(&m_GridMgr.getMapGrid(), ptGrid))
	{
		m_EntityList.add(pEntity, ptTile, ptGrid);
		//_Info("m_EntityList.add OK "<<_fi("0x%08x", (ulong)pEntity)<<endl);
	}
	else
	{
		//_Info("!(PtInRect(&m_GridMgr.getMapGrid(), ptGrid) "<<_fi("0x%08x", (ulong)pEntity)<<endl);
	}
}

void SceneEyeshot::RemoveItemFromList(EntityView* pEntity, const POINT& ptTile)
{
	if(m_EntityList.remove(pEntity, ptTile))
	{
		//_Info("m_EntityList.remove OK "<<_fi("0x%08x", (ulong)pEntity)<<endl);
	}
	else
	{
		//_Info("m_EntityList.remove Fail"<<_fi("0x%08x", (ulong)pEntity)<<endl);
	}
}

void SceneEyeshot::MoveItemInList(EntityView* pEntity, const POINT& ptTileFrom, const POINT& ptTileTo)
{
	SceneCoord& coord = m_pSceneManager->getSceneCoord();
	POINT ptGridTo, ptTileCenter;
	coord.tile2Pixel(ptTileTo, ptTileCenter);
	ptGridTo.x = ptTileCenter.x >> SHIFT_WIDTH;
	ptGridTo.y = ptTileCenter.y >> SHIFT_HEIGHT;
	m_EntityList.move(pEntity, ptTileFrom, ptTileTo, ptGridTo);
}

