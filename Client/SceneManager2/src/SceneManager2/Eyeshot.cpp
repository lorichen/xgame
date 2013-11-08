#include "stdafx.h"
#include "Eyeshot.h"

bool Eyeshot::create(IEyeshotCallback* pSink,
						  int nMapWidth,int nMapHeight,
						  int nGridWidth,int nGridHeight,
						  xs::Rect* lprcViewport,bool bDynamic)
{
	m_pScrollSheetSink = pSink,
	m_nMapWidth = nMapWidth;
	m_nMapHeight = nMapHeight;
	SetRect(&m_rcMap,0,0,nMapWidth,nMapHeight);

	m_nViewWidth = lprcViewport->right - lprcViewport->left;
	m_nViewHeight = lprcViewport->bottom - lprcViewport->top;
	m_nViewTopX = lprcViewport->left;
	m_nViewTopY = lprcViewport->top;
	SetRect(&m_rcViewportRect,m_nViewTopX,m_nViewTopY,m_nViewTopX + m_nViewWidth,m_nViewTopY + m_nViewHeight);

	m_nGridWidth = nGridWidth;
	m_nGridHeight = nGridHeight;
	SetRect(&m_rcMapGrid,0,0,(m_nMapWidth + nGridWidth - 1) / nGridWidth,(m_nMapHeight + nGridHeight - 1) / nGridHeight);

	xs::Rect rc;
	SetRect(&rc, m_nViewTopX / nGridWidth, m_nViewTopY / nGridHeight,
		(m_nViewTopX + m_nViewWidth - 1) / nGridWidth + 1, (m_nViewTopY + m_nViewHeight - 1) / nGridHeight + 1);

	//动态的话视口矩形是要根据左上和大小滚动变化的
	if(bDynamic)
		setViewTileRect(rc);
	else
		setViewTileRect	(m_rcMapGrid);

	return true;
}

bool Eyeshot::viewportSizeChanged(int nWidth, int nHeight)
{
	if(nWidth == 0 || nHeight == 0)
		return false;

	m_nViewWidth = nWidth;
	m_nViewHeight = nHeight;

	SetRect(&m_rcViewportRect,m_nViewTopX,m_nViewTopY,m_nViewTopX + m_nViewWidth,m_nViewTopY + m_nViewHeight);

	xs::Rect rcNew;
	SetRect(&rcNew,m_nViewTopX / m_nGridWidth,
		m_nViewTopY / m_nGridHeight,
		(m_nViewTopX + m_nViewWidth - 1) / m_nGridWidth + 1,
		(m_nViewTopY + m_nViewHeight - 1) / m_nGridHeight + 1);

	if(EqualRect(&rcNew,&m_rcCurViewGrid))
	{
		return true;
	}

	xs::Rect rcNewDirty = rcNew;
	InflateRect(&rcNewDirty,1,1);
	IntersectRect(&rcNewDirty,&rcNewDirty,&m_rcMapGrid);

	for(int y = m_rcLastDirty.top; y < m_rcLastDirty.bottom; y++)
	for(int x = m_rcLastDirty.left; x < m_rcLastDirty.right; x++)
	{
		xs::Point pt;
		pt.x = x; pt.y = y;
		if(!PtInRect(&rcNewDirty,pt) && !PtInRect(&rcNew,pt))
			m_pScrollSheetSink->onLost(x,y);
	}

	for(LONG y = rcNewDirty.top; y < rcNewDirty.bottom; y++)
	for(int x = rcNewDirty.left; x < rcNewDirty.right; x++)
	{
		xs::Point pt;
		pt.x = x; pt.y = y;
		if(!PtInRect(&m_rcLastDirty,pt))
			m_pScrollSheetSink->onFound(x,y);
	}

	m_rcCurViewGrid = rcNew;
	m_rcLastDirty = rcNewDirty;

	return true;
}

void Eyeshot::close()
{

}

bool Eyeshot::moveViewportTo(int x, int y)
{
	scrollViewport(x - m_nViewTopX,y - m_nViewTopY);
	return true;
}

void Eyeshot::setViewTileRect(xs::Rect rc)
{
	IntersectRect(&rc,&rc,&m_rcMapGrid);
	m_rcCurViewGrid = rc;
	InflateRect(&rc,1,1);
	IntersectRect(&rc,&rc,&m_rcMapGrid);
	for(int y = rc.top; y < rc.bottom; y++)
		for(int x = rc.left; x < rc.right; x++)
			m_pScrollSheetSink->onFound(x,y);

	m_rcLastDirty = rc;
}

bool Eyeshot::scrollViewport(int dx, int dy)
{
	if(m_nViewTopX + dx < 0)
		dx = -m_nViewTopX;
	if(m_nViewTopX + m_nViewWidth + dx > m_nMapWidth)
		dx = m_nMapWidth - m_nViewTopX - m_nViewWidth;
	if(m_nViewTopY + dy < 0)
		dy = - m_nViewTopY;
	if(m_nViewTopY + m_nViewHeight + dy > m_nMapHeight)
		dy = m_nMapHeight - m_nViewTopY - m_nViewHeight;
	if(dx == 0 && dy == 0)
		return false;

	xs::Rect rcNew,rcOffset,rcNewDirty;
	SetRect(&rcNew,(m_nViewTopX + dx) / m_nGridWidth,(m_nViewTopY + dy) / m_nGridHeight,
		(m_nViewTopX + m_nViewWidth - 1 + dx) / m_nGridWidth + 1,
		(m_nViewTopY + m_nViewHeight - 1 + dy) / m_nGridHeight + 1);

	m_nViewTopX += dx;
	m_nViewTopY += dy;

	if(EqualRect(&rcNew,&m_rcCurViewGrid))
	{
		SetRect(&m_rcViewportRect,m_nViewTopX,m_nViewTopY,m_nViewTopX + m_nViewWidth,m_nViewTopY + m_nViewHeight);
		return true;
	}

	rcOffset = rcNew;
	OffsetRect(&rcOffset,dx > 0 ? 1 : (dx < 0 ? -1 : 0),dy > 0 ? 1 : (dy < 0 ? -1 : 0));
	UnionRect(&rcNewDirty,&rcNew,&rcOffset);
	IntersectRect(&rcNewDirty,&rcNewDirty,&m_rcMapGrid);

	SetRect(&m_rcViewportRect,m_nViewTopX,m_nViewTopY,m_nViewTopX + m_nViewWidth,m_nViewTopY + m_nViewHeight);

	for(int y = m_rcLastDirty.top; y < m_rcLastDirty.bottom; y++)
	for(int x = m_rcLastDirty.left; x < m_rcLastDirty.right; x++)
	{
		xs::Point pt;
		pt.x = x; pt.y = y;
		if(!PtInRect(&rcNewDirty,pt) && !PtInRect(&rcNew,pt))
			m_pScrollSheetSink->onLost(x,y);
	}

	for(LONG y = rcNewDirty.top; y < rcNewDirty.bottom; y++)
	for(int x = rcNewDirty.left; x < rcNewDirty.right; x++)
	{
		xs::Point pt;
		pt.x = x; pt.y = y;
		if(!PtInRect(&m_rcLastDirty,pt))
			m_pScrollSheetSink->onFound(x,y);
	}

	m_rcCurViewGrid = rcNew;
	m_rcLastDirty = rcNewDirty;
	return true;
}

bool Eyeshot::scroll2Center(const xs::Rect& rcView)
{
	int x = (rcView.left + rcView.right) / 2  - m_nViewWidth / 2;
	int y = (rcView.top + rcView.bottom) / 2 - m_nViewHeight / 2;

	return scrollViewport(x - m_nViewTopX,y - m_nViewTopY);
}
