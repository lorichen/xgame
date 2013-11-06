#include "stdafx.h"
#include "Eyeshot.h"
#include "SceneGrid.h"

SceneGrid::SceneGrid()
{
	m_pScrollSheetSink = 0;
	m_ptPlayerGrid.x = m_ptPlayerGrid.y = 0;
	memset(&m_rcMapGrid, 0, sizeof(RECT));
	memset(&m_rcPlayerGrid, 0, sizeof(RECT));
	memset(&m_rcOldPreReadGrid,0,sizeof(RECT));
	memset(&m_rcPreReadGrid, 0, sizeof(RECT));
	memset(&m_rcOldViewGrid, 0, sizeof(RECT));
	memset(&m_rcViewGrid, 0, sizeof(RECT));
	m_bDynamic = true;
	m_bFirstTimeLoad = false;
}

bool SceneGrid::create(IEyeshotCallback* pSink, int nMapWidth, int nMapHeight, 
					  POINT& ptPlayerPos,bool bDynamic)
{
	assert(pSink);
	m_pScrollSheetSink = pSink;
	m_bDynamic = bDynamic;
	
	m_rcMapGrid.left	= m_rcMapGrid.top = 0;
	m_rcMapGrid.right	= (nMapWidth + GRID_WIDTH - 1) >> SHIFT_WIDTH;
	m_rcMapGrid.bottom	= (nMapHeight + GRID_HEIGHT - 1) >> SHIFT_HEIGHT;

	m_ptPlayerGrid.x = ptPlayerPos.x >> SHIFT_WIDTH;
	m_ptPlayerGrid.y = ptPlayerPos.y >> SHIFT_HEIGHT;

	if(bDynamic)
	{
		m_rcPreReadGrid.left	= m_ptPlayerGrid.x - (TILE_GRID_COL >> 1);
		m_rcPreReadGrid.top		= m_ptPlayerGrid.y - (TILE_GRID_ROW >> 1);
		m_rcPreReadGrid.right	= m_rcPreReadGrid.left + TILE_GRID_COL;
		m_rcPreReadGrid.bottom	= m_rcPreReadGrid.top + TILE_GRID_ROW;
		limitRect(m_rcPreReadGrid, m_rcMapGrid);
		m_rcOldPreReadGrid = m_rcPreReadGrid;
	}
	else
	{
		m_rcOldPreReadGrid = m_rcPreReadGrid = m_rcMapGrid;
	}

	m_rcViewGrid.left	= m_ptPlayerGrid.x - (OBJECT_GRID_COL >> 1);
	m_rcViewGrid.top	= m_ptPlayerGrid.y - (OBJECT_GRID_ROW >> 1);
	m_rcViewGrid.right	= m_rcViewGrid.left + OBJECT_GRID_COL;
	m_rcViewGrid.bottom	= m_rcViewGrid.top + OBJECT_GRID_ROW;
	limitRect(m_rcViewGrid, m_rcMapGrid);
	m_rcOldViewGrid = m_rcViewGrid;
	resetPlayerRect();
	
	if(bDynamic)setPreReadTileRect(m_rcPreReadGrid);

	m_bFirstTimeLoad = true;
	setViewTileRect(m_rcViewGrid);
	m_bFirstTimeLoad = false;

	return true;
}

bool SceneGrid::viewportSizeChanged(int nWidth, int nHeight)
{
	return true;
}

void SceneGrid::close()
{
	//TODO
	closeViewTileRect();
	closePreReadTileRect();
}

void SceneGrid::setPreReadTileRect(RECT rc)
{
	int nDx, nDy;
	int nMoveDir = getMoveDirection((POINT&)m_rcOldPreReadGrid, (POINT&)m_rcPreReadGrid, nDx, nDy);
	m_pScrollSheetSink->onChanged(nMoveDir, nDx, nDy);
}

void SceneGrid::closePreReadTileRect()
{
}

void SceneGrid::setViewTileRect(RECT rc)
{
	for (int y=m_rcViewGrid.top; y<m_rcViewGrid.bottom; y++)
		for (int x=m_rcViewGrid.left; x<m_rcViewGrid.right; x++)
			m_pScrollSheetSink->onFound(x, y);
}

void SceneGrid::closeViewTileRect()
{
	for (int y=m_rcViewGrid.top; y<m_rcViewGrid.bottom; y++)
		for (int x=m_rcViewGrid.left; x<m_rcViewGrid.right; x++)
			m_pScrollSheetSink->onLost(x, y);
}

bool SceneGrid::scrollViewport(int dx, int dy)
{
	bool bPreReadGridChanged = false;
	RECT rcNewPreReadGrid;

	if(m_bDynamic)
	{
		rcNewPreReadGrid = m_rcPreReadGrid;
		OffsetRect(&rcNewPreReadGrid, dx, dy);
		limitRect(rcNewPreReadGrid, m_rcMapGrid);
		bPreReadGridChanged = !EqualRect(&rcNewPreReadGrid, &m_rcPreReadGrid);
	}

	bool bViewGridChanged = false;
	RECT rcNewViewGrid;
	rcNewViewGrid = m_rcViewGrid;
	OffsetRect(&rcNewViewGrid, dx, dy);
	limitRect(rcNewViewGrid, m_rcMapGrid);
	bViewGridChanged = !EqualRect(&rcNewViewGrid, &m_rcViewGrid);
	
	if (bViewGridChanged)
	{
		POINT pt;
		for (int y=m_rcViewGrid.top; y<m_rcViewGrid.bottom; y++)
		{
			for (int x=m_rcViewGrid.left; x<m_rcViewGrid.right; x++)
			{
				pt.x = x, pt.y = y;
				if (!PtInRect(&rcNewViewGrid, pt))
					m_pScrollSheetSink->onLost(x, y);
			}
		}
	
		m_rcOldViewGrid = m_rcViewGrid;
		m_rcViewGrid = rcNewViewGrid;
		resetPlayerRect();		
	}

	if (m_bDynamic && bPreReadGridChanged)
	{
		m_rcOldPreReadGrid = m_rcPreReadGrid;
		m_rcPreReadGrid = rcNewPreReadGrid;
		int nDx, nDy;
		int nMoveDirection = getMoveDirection((POINT&)m_rcOldPreReadGrid, (POINT&)m_rcPreReadGrid, nDx, nDy);
		m_pScrollSheetSink->onChanged(nMoveDirection, nDx, nDy);
	}

	if (bViewGridChanged)
	{
		POINT pt;
		for (int y=m_rcViewGrid.top; y<m_rcViewGrid.bottom; y++)
		{ 
			for (int x=m_rcViewGrid.left; x<m_rcViewGrid.right; x++)
			{
				pt.x = x, pt.y = y;
				if (!PtInRect(&m_rcOldViewGrid, pt))
					m_pScrollSheetSink->onFound(x, y);
			}
		}

	}
	return true;
}

bool SceneGrid::scroll2Center(POINT& ptPlayerPos)
{
	POINT ptPlayerGridNew;
	ptPlayerGridNew.x = ptPlayerPos.x >> SHIFT_WIDTH;
	ptPlayerGridNew.y = ptPlayerPos.y >> SHIFT_HEIGHT;
	if (m_ptPlayerGrid.x == ptPlayerGridNew.x && m_ptPlayerGrid.y == ptPlayerGridNew.y)
		return false;
	
	//玩家的新点是否导致格子变化
	//控制在地图边沿不出现新格子
	if (PtInRect(&m_rcPlayerGrid, ptPlayerGridNew))
	{
		m_ptPlayerGrid = ptPlayerGridNew;
		return false;
	}

	int dx, dy;
	if (m_rcPlayerGrid.right - m_rcPlayerGrid.left != 1 ||
		m_rcPlayerGrid.bottom - m_rcPlayerGrid.top != 1)
	{
		dx = ptPlayerGridNew.x - ((m_rcViewGrid.right + m_rcViewGrid.left) >> 1);
		dy = ptPlayerGridNew.y - ((m_rcViewGrid.bottom + m_rcViewGrid.top) >> 1);
	}
	else
	{
		dx = ptPlayerGridNew.x - m_ptPlayerGrid.x;
		dy = ptPlayerGridNew.y - m_ptPlayerGrid.y;
	}
	m_ptPlayerGrid = ptPlayerGridNew;
	return scrollViewport(dx, dy);
}

void SceneGrid::getGridRectBelowViewport(RECT& rcCoveredRect)
{
	rcCoveredRect.left   = m_rcViewGrid.left   << SHIFT_WIDTH;
	rcCoveredRect.top	 = m_rcViewGrid.top    << SHIFT_HEIGHT;
	rcCoveredRect.right  = m_rcViewGrid.right  << SHIFT_WIDTH;
	rcCoveredRect.bottom = m_rcViewGrid.bottom << SHIFT_HEIGHT;
}

void SceneGrid::getGridRectBelowPreRead(RECT& rcCoveredRect)
{
	rcCoveredRect.left   = m_rcPreReadGrid.left   << SHIFT_WIDTH;
	rcCoveredRect.top	 = m_rcPreReadGrid.top    << SHIFT_HEIGHT;
	rcCoveredRect.right  = m_rcPreReadGrid.right  << SHIFT_WIDTH;
	rcCoveredRect.bottom = m_rcPreReadGrid.bottom << SHIFT_HEIGHT;
}

int SceneGrid::getMoveDirection(POINT& ptFrom, POINT& ptTo, int& nDx, int& nDy)
{
	nDx = ptTo.x - ptFrom.x;
	nDy = ptTo.y - ptFrom.y;
	if (nDx > TILE_GRID_COL || nDx < -TILE_GRID_COL || nDy > TILE_GRID_ROW || nDy < -TILE_GRID_ROW)
		return mdAll;
	if (nDx > 0)
	{
		if (nDy > 0)		return mdEastSouth;
		else if (nDy < 0)	return mdEastNorth;
		else				return mdEast;
	}
	else if (nDx < 0)
	{
		if (nDy > 0)		return mdWestSouth;
		else if (nDy < 0)	return mdWestNorth;
		else				return mdWest;
	}
	else
	{
		if (nDy > 0)		return mdSouth;
		else if (nDy < 0)	return mdNorth;
	}
	return mdAll;
}

void SceneGrid::snapshotLeftTop(int& nRows, int& nCols, BlockInfo* pBlockInfo, int nMoveDirection)
{
	RECT rcUnion;
	if (nMoveDirection == mdAll)
		rcUnion = m_rcPreReadGrid;
	else
		UnionRect(&rcUnion, &m_rcOldPreReadGrid, &m_rcPreReadGrid);
	POINT ptLeftTop;
	ptLeftTop.x  = rcUnion.left << SHIFT_WIDTH;
	ptLeftTop.y	 = rcUnion.top << SHIFT_HEIGHT;
	nRows = rcUnion.bottom - rcUnion.top;
	nCols = rcUnion.right - rcUnion.left;
	
	int x, y = ptLeftTop.y;
	int nIdx = rcUnion.top * m_rcMapGrid.right + rcUnion.left;
	for (int row=0,i=0; row<nRows; row++,y+=GRID_HEIGHT,nIdx+=m_rcMapGrid.right)
	{
		x = ptLeftTop.x;
		for (int col=0; col<nCols; col++,x+=GRID_WIDTH,i++)
		{
			pBlockInfo[i].ptLeftTop.x = x;
			pBlockInfo[i].ptLeftTop.y = y;
			pBlockInfo[i].nIndex = nIdx + col;
		}
	}
}

int SceneGrid::getBlockIndex(POINT& ptLeftTop)
{
	int nGridX = ptLeftTop.x >> SHIFT_WIDTH;
	int nGridY = ptLeftTop.y >> SHIFT_HEIGHT;
	return nGridY * m_rcMapGrid.right + nGridX;
}

void SceneGrid::limitRect(RECT& rcSmall, const RECT& rcBig)
{
	if (rcSmall.left < rcBig.left)
	{
		rcSmall.right += rcBig.left - rcSmall.left;
		rcSmall.left = rcBig.left;
		
		if (rcSmall.right > rcBig.right)
			rcSmall.right = rcBig.right;
	}
	if (rcSmall.top < rcBig.top)
	{
		rcSmall.bottom += rcBig.top - rcSmall.top;
		rcSmall.top = rcBig.top;

		if (rcSmall.bottom > rcBig.bottom)
			rcSmall.bottom = rcBig.bottom;
	}
	if (rcSmall.right > rcBig.right)
	{
		rcSmall.left -= rcSmall.right - rcBig.right;
		rcSmall.right = rcBig.right;
		
		if (rcSmall.left < rcBig.left)
			rcSmall.left = rcBig.left;
	}
	if (rcSmall.bottom > rcBig.bottom)
	{
		rcSmall.top -= rcSmall.bottom - rcBig.bottom;
		rcSmall.bottom = rcBig.bottom;
		
		if (rcSmall.top < rcBig.top)
			rcSmall.top = rcBig.top;
	}
}

void SceneGrid::resetPlayerRect()
{
	int dx = 0, dy = 0;
	if (m_rcViewGrid.left <= m_rcMapGrid.left)
		dx = - ((m_rcViewGrid.left + m_rcViewGrid.right) >> 1);
	if (m_rcViewGrid.top <= m_rcMapGrid.top)
		dy = - ((m_rcViewGrid.top + m_rcViewGrid.bottom) >> 1);
	if (m_rcViewGrid.right >= m_rcMapGrid.right)
		dx = ((m_rcViewGrid.right - m_rcViewGrid.left) >> 1);
	if (m_rcViewGrid.bottom >= m_rcMapGrid.bottom)
		dy = ((m_rcViewGrid.bottom - m_rcViewGrid.top) >> 1);

	m_rcPlayerGrid.left = (m_rcViewGrid.left + m_rcViewGrid.right) >> 1;
	m_rcPlayerGrid.top = (m_rcViewGrid.top + m_rcViewGrid.bottom) >> 1;
	m_rcPlayerGrid.right = m_rcPlayerGrid.left + 1;
	m_rcPlayerGrid.bottom = m_rcPlayerGrid.top + 1;
	if (dx < 0)
		m_rcPlayerGrid.left += dx;
	if (dx > 0)
		m_rcPlayerGrid.right += dx;
	if (dy < 0)
		m_rcPlayerGrid.top += dy;
	if (dy > 0)
		m_rcPlayerGrid.bottom += dy;
}