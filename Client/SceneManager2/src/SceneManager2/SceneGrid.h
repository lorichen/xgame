#ifndef __SceneGrid_H__
#define __SceneGrid_H__

#include "SceneMgr.h"

//add by yhc 加注释
//管理场景的格子,每个格子以1024*512为大小.
//
struct IEyeshotCallback;
class SceneGrid
{
	//整个地图有多大
	RECT				m_rcMapGrid;

	//用来判断自己所在的点在不在这个矩形内,玩家的新点是否导致格子变化,控制在地图边沿不出现新格子
	RECT				m_rcPlayerGrid;
	//自己所在的格子坐标x,y
	POINT				m_ptPlayerGrid;

	//新旧范围对比可得出移动方向
	RECT				m_rcOldPreReadGrid;
	//以自己所在的格子坐标(m_ptPlayerGrid)为中心,TILE_GRID_COL*TILE_GRID_ROW大小
	//预读取格子判断该加载哪些数据等
	RECT				m_rcPreReadGrid;

	//用于判断不在旧视口内的物件
	RECT				m_rcOldViewGrid;

	//以自己所在的格子坐标(m_ptPlayerGrid)为中心,OBJECT_GRID_COL*OBJECT_GRID_ROW大小
	//视野格子主要还是为了判断物件是否可见
	RECT				m_rcViewGrid;

	IEyeshotCallback*	m_pScrollSheetSink;
	bool				m_bDynamic;
	bool				m_bFirstTimeLoad;
public:
	SceneGrid();
	~SceneGrid() {}

	bool create(IEyeshotCallback* pSink, int nMapWidth, int nMapHeight, 
		POINT& ptPlayerPos,bool bDynamic);
	void close();

	bool scrollViewport(int dx, int dy);
	bool viewportSizeChanged(int nWidth, int nHeight);
	bool scroll2Center(POINT& ptPlayerPos);

	void getGridRectBelowViewport(RECT& rcCoveredRect);
	void getGridRectBelowPreRead(RECT& rcCoveredRect);
	RECT& getPreReadGrid()			{return m_rcPreReadGrid;}
	RECT& getViewGrid()				{return m_rcViewGrid;}
	RECT& getMapGrid()				{return m_rcMapGrid;}
	POINT getPlayerGrid()			{return m_ptPlayerGrid;}
	bool	isDynamic()				{return m_bDynamic;}
	bool	isFirstTimeLoad()		{return m_bFirstTimeLoad;}
	
	void snapshotLeftTop(int& nRows, int& nCols, BlockInfo* pBlockInfo, int nMoveDirection);
	int getBlockIndex(POINT& ptLeftTop);


private:
	void setPreReadTileRect(RECT rc);
	void closePreReadTileRect();
	void setViewTileRect(RECT rc);
	void closeViewTileRect();
	int  getMoveDirection(POINT& ptFrom, POINT& ptTo, int& nDx, int& nDy);
	void limitRect(RECT& rcSmall, const RECT& rcBig);
	void resetPlayerRect();
};

#endif // __GRIDMGR_H__

