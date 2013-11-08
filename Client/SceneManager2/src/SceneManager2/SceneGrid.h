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
	xs::Rect				m_rcMapGrid;

	//用来判断自己所在的点在不在这个矩形内,玩家的新点是否导致格子变化,控制在地图边沿不出现新格子
	xs::Rect				m_rcPlayerGrid;
	//自己所在的格子坐标x,y
	xs::Point				m_ptPlayerGrid;

	//新旧范围对比可得出移动方向
	xs::Rect				m_rcOldPreReadGrid;
	//以自己所在的格子坐标(m_ptPlayerGrid)为中心,TILE_GRID_COL*TILE_GRID_ROW大小
	//预读取格子判断该加载哪些数据等
	xs::Rect				m_rcPreReadGrid;

	//用于判断不在旧视口内的物件
	xs::Rect				m_rcOldViewGrid;

	//以自己所在的格子坐标(m_ptPlayerGrid)为中心,OBJECT_GRID_COL*OBJECT_GRID_ROW大小
	//视野格子主要还是为了判断物件是否可见
	xs::Rect				m_rcViewGrid;

	IEyeshotCallback*	m_pScrollSheetSink;
	bool				m_bDynamic;
	bool				m_bFirstTimeLoad;
public:
	SceneGrid();
	~SceneGrid() {}

	bool create(IEyeshotCallback* pSink, int nMapWidth, int nMapHeight, 
		xs::Point& ptPlayerPos,bool bDynamic);
	void close();

	bool scrollViewport(int dx, int dy);
	bool viewportSizeChanged(int nWidth, int nHeight);
	bool scroll2Center(xs::Point& ptPlayerPos);

	void getGridRectBelowViewport(xs::Rect& rcCoveredRect);
	void getGridRectBelowPreRead(xs::Rect& rcCoveredRect);
	xs::Rect& getPreReadGrid()			{return m_rcPreReadGrid;}
	xs::Rect& getViewGrid()				{return m_rcViewGrid;}
	xs::Rect& getMapGrid()				{return m_rcMapGrid;}
	xs::Point getPlayerGrid()			{return m_ptPlayerGrid;}
	bool	isDynamic()				{return m_bDynamic;}
	bool	isFirstTimeLoad()		{return m_bFirstTimeLoad;}
	
	void snapshotLeftTop(int& nRows, int& nCols, BlockInfo* pBlockInfo, int nMoveDirection);
	int getBlockIndex(xs::Point& ptLeftTop);


private:
	void setPreReadTileRect(xs::Rect rc);
	void closePreReadTileRect();
	void setViewTileRect(xs::Rect rc);
	void closeViewTileRect();
	int  getMoveDirection(xs::Point& ptFrom, xs::Point& ptTo, int& nDx, int& nDy);
	void limitRect(xs::Rect& rcSmall, const xs::Rect& rcBig);
	void resetPlayerRect();
};

#endif // __GRIDMGR_H__

