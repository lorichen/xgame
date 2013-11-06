#ifndef __SceneMgr_H__
#define __SceneMgr_H__

#include "IEntityView.h"
#include "SceneCoord.h"
#include "Tile.h"
#include <list>
#include <map>

#include "PathFinder.h"
#include "SceneBlock.h"
#include "ISceneManager2.h"
#include "CPathFinderByAStar.h"
//  默认的地图版本号；
#define ORIGIN_MAP_VERSION 0

//  支持特效缩放和旋转角度的版本的版本号；
#define SUPPORT_EFFECT_SCALE_AND_ANGLE_ADJUST_MAP_VERSION 1

#define SHIFT_WIDTH			10
#define SHIFT_HEIGHT		9
#define GRID_WIDTH			1024
#define GRID_HEIGHT			512
#define TILE_GRID_ROW		5
#define TILE_GRID_COL		3
#define OBJECT_GRID_ROW		5
#define OBJECT_GRID_COL		3

#define TILES_PREREAD_WIDTH		GRID_WIDTH * TILE_GRID_COL
#define TILES_PREREAD_HEIGHT	GRID_HEIGHT * TILE_GRID_ROW

enum
{
	mdNorth = 0,
	mdEastNorth,
	mdEast,
	mdEastSouth,
	mdSouth,
	mdWestSouth,
	mdWest,
	mdWestNorth,
	mdAll,

	mdMaxUpdateType
};

class GroundEyeshot;

struct EnumItem
{
	POINT		ptTile;
	EntityView*	pEntity;
};

struct EnumTile
{
	POINT		ptTile;
	Tile*		pTile;
};

struct BlockInfo
{
	POINT	ptLeftTop;
	int		nIndex;
};

#define MAX_EnumItem_COUNT		4096
#define MAX_SNAP_BUF_SIZE		(MAX_EnumItem_COUNT * sizeof(EnumItem))

struct SceneFileLineHeader
{
	int		nFromX;
	int		nTileCount;
};

using namespace std;
typedef list<EntityView*>  SceneEntityList;

class SceneMgr :public CPathFinderByAStar<SceneMgr>,public PathFinder<SceneMgr>
{
	ISceneManager2*	m_pSceneManager;
	//每当1024*512的大块变化的时候tile源点都会改变
	POINT	m_ptTileOrigin;

	//下面三个量都是tile相关的
	int		m_nMatrixWidth;
	int		m_nMatrixHeight;
	RECT	m_rcMapTileRect;
	//tile 对象数组
	Tile*	m_pTiles;

	//用于找到本快的地图文件偏移
	DWORD*	m_pMapTable;	// 记录1024*512宫格的数组
	
	//世界坐标的地图大小
	int		m_nMapWidth;
	int		m_nMapHeight;
	RECT	m_rcMap;
	
	//坐标转换器
	SceneCoord m_SceneCo;
	//地图文件数据流
	xs::Stream* m_pStream;
	//实体工厂
	IEntityFactory* m_pItemCF;
	
	VariableLengthArrayCache<DWORD,EntityView*> m_LayerListCache;
	DWORD* m_pMultiValueTable;
	SceneEntityList m_EntityList;
	typedef void (SceneMgr::*ProcScrollTilesInfo)(BlockInfo* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	static ProcScrollTilesInfo m_fnScrollTileInfo[mdMaxUpdateType];

	GroundEyeshot*		m_pGround;

	bool m_bDynamic;

	SceneBlock		m_MapBlockBuffer;

	//保存地图时候是否把占位块直接存储到地图上
	bool			m_bWriteOccupants;
	
	////标志，防止特效往预加载列表里面重复加
	bool	m_VectorPreLoad[MAP_MAXCOUNT];
private:
	static Tile m_EmptyTile;
public:
	bool IsBlock(POINT ptPos)
	{
		Tile& tile = getTile(ptPos.x, ptPos.y);
		return (!tile.isValid() || tile.isBlock());
	}
	// 自动寻路搜索的范围为整图的阻挡信息
	bool  IsAllBlock(POINT ptPos)
	{
		Tile& tile = getTileArray(ptPos.x, ptPos.y);
		return (!tile.isValid() || tile.isBlock());
	}

	/** 判断一个tile是否无效，给搜路模板使用
	@param   
	@param   
	@return  
	*/
	bool IsInValidTile(POINT ptTile)
	{
		Tile& tile = getTileArray(ptTile.x, ptTile.y);
		if(!tile.isValid())
		{
			return true;
		}
		return false;
	}
	bool isMovableForMultiOcc(EntityView* pEntity, POINT ptFrom, POINT ptTo);	
	int getSceneWidth()const		{return m_nMapWidth;}
	int getSceneHeight()const		{return m_nMapHeight;}
	void moveTop(BlockInfo* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	void moveRightTop(BlockInfo* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	void moveRight(BlockInfo* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	void moveRightDown(BlockInfo* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	void moveDown(BlockInfo* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	void moveLeftDown(BlockInfo* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	void moveLeft(BlockInfo* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	void moveleftTop(BlockInfo* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	void moveAll(BlockInfo* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, POINT& ptNewTileOrigin, int dx, int dy);
	void scrollTilesInfo(BlockInfo* pBlockInfo, int nRows, int nCols, int nTileRow, int nTileCol, int nDirection, POINT& ptNewTileOrigin, int dx, int dy);
	void initBlockInfo(POINT& ptLeftTop, int nTileRow, int nTileCol,int realRow,int realCol);
	void loadBlockInfo(POINT& ptLeftTop, int nIndex, int nTileRow, int nTileCol);
	void copyBlockInfo(POINT& ptSrcLeftTop, POINT& ptDstLeftTop, int nTileRow, int nTileCol);
	void copyBlockInfoForSurface(POINT& ptSrcLeftTop, POINT& ptDstLeftTop, int nTileRow, int nTileCol);
	void setOriginTile(POINT& ptTileOrigin);
	POINT GetOriginTile()		{return m_ptTileOrigin;}
	bool isValid(){	return m_pTiles != 0;}
	bool AdjustTileRect(RECT &rc);
	bool IsTileInMap(int tx,int ty) const
	{
		POINT ptOff = {tx - m_ptTileOrigin.x, ty - m_ptTileOrigin.y};
		return (ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight);
	}
	bool IsTileInMap(const POINT& ptTile) const
	{
		POINT ptOff = {ptTile.x - m_ptTileOrigin.x, ptTile.y - m_ptTileOrigin.y};
		return (ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight);
	}
	bool isValidTile(const POINT& ptTile)
	{
		POINT ptOff = {ptTile.x - m_ptTileOrigin.x, ptTile.y - m_ptTileOrigin.y};
		if (ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight)
			return m_pTiles[m_pMultiValueTable[ptOff.y] + ptOff.x].isValid();
		else
			return false;
	}
	bool isValidTile(int tx,int ty)
	{
		POINT ptOff = {tx - m_ptTileOrigin.x, ty - m_ptTileOrigin.y};
		if (ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight)
			return m_pTiles[m_pMultiValueTable[ptOff.y] + ptOff.x].isValid();
		else
			return false;
	}

	Tile& getTile(const POINT& ptTile) const
	{
		if (NULL == m_pTiles || NULL == m_pMultiValueTable)
		{
			return m_EmptyTile;
		}

		POINT ptOff = {ptTile.x - m_ptTileOrigin.x, ptTile.y - m_ptTileOrigin.y};
		if (ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight)
			return m_pTiles[m_pMultiValueTable[ptOff.y] + ptOff.x];
		else
			return m_EmptyTile;
	}

	Tile& getTile(int tx, int ty) const
	{
		if (NULL == m_pTiles || NULL == m_pMultiValueTable)
		{
			return m_EmptyTile;
		}

		POINT ptOff = {tx - m_ptTileOrigin.x, ty - m_ptTileOrigin.y};
		if (ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight)
			return m_pTiles[m_pMultiValueTable[ptOff.y] + ptOff.x];
		else
			return m_EmptyTile;
	}

	// 自动寻路搜索的范围为整图的阻挡信息
	Tile& getTileArray(int tx, int ty) const
	{
		POINT ptOff = {tx, ty};
		if (ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight)
			return m_pTiles[m_pMultiValueTable[ptOff.y] + ptOff.x];
		else
			return m_EmptyTile;
	}
	/*inline Tile& _GetTile(const POINT& ptTile) const
	{
		POINT ptOff = {ptTile.x - m_ptTileOrigin.x, ptTile.y - m_ptTileOrigin.y};
		assert(ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight);
		return m_pTiles[m_pMultiValueTable[ptOff.y] + ptOff.x];
	}

	inline Tile& _GetTile(int tx, int ty) const
	{
		POINT ptOff = {tx - m_ptTileOrigin.x, ty - m_ptTileOrigin.y};
		assert(ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight);
		return m_pTiles[m_pMultiValueTable[ptOff.y] + ptOff.x];
	}*/

	Tile* getTilePtr(const POINT& ptTile) const
	{
		POINT ptOff = {ptTile.x - m_ptTileOrigin.x, ptTile.y - m_ptTileOrigin.y};
		if (ptOff.x >= 0 && ptOff.x < m_nMatrixWidth && ptOff.y >= 0 && ptOff.y < m_nMatrixHeight)
		{
			Tile* pTile = &m_pTiles[m_pMultiValueTable[ptOff.y] + ptOff.x];
			if (pTile->isValid())
				return pTile;
		}
		return 0;
	}

	inline Tile* getMapArray()						{return m_pTiles;}
	int getMapArrayWidth() const					{return m_nMatrixWidth;}
	int getMapArrayHeight() const					{return m_nMatrixHeight;}
	const SceneCoord& getSceneCoord() const	{return m_SceneCo;}
	SceneCoord GetSceneCoordNormal() {return m_SceneCo;}
	SceneBlock*	getMapBlockBuffer(){return &m_MapBlockBuffer;}

	bool create(GroundEyeshot *pGround,int nMapWidth, int nMapHeight, RECT& rcPreRead,ISceneManager2 *pSceneManager);
	void close();

	bool save(xs::DataChunk* pDataChunk,bool writeOccupants);
	bool load(GroundEyeshot *pGround,xs::Stream* pStream,IEntityFactory* pEntityFactory, LPRECT pViewport,const POINT* pTileCenter,bool bDynamic,ISceneManager2 *pSceneManager);

	virtual bool addEntity(const POINT& ptTile, EntityView* pEntity);
	virtual bool moveEntity(const POINT& ptFrom, const POINT& ptTo, EntityView* pEntity);
	virtual bool removeEntity(const POINT& ptTile, EntityView* pEntity);

	virtual bool addEntityOccupant(const POINT& ptTile, EntityView* pEntity);
	virtual bool removeEntityOccupant(const POINT& ptTile, EntityView* pEntity);

	bool addLayerItem(const POINT &pt, EntityView *pEntity);

	bool snapshotRectItem(const RECT& rc,/*in out*/int& nListCount,/*out*/EnumItem* pListBuf);
	bool enumEntityByWorldRect(const RECT &rcWorld, /*in out*/int& nListCount,/*out*/EnumItem* pListBuf);
	bool enumTileByWorldRect(const RECT &rcWorld, /*in out*/int& nListCount,/*out*/EnumTile* pListBuf);

	SceneMgr();
	virtual ~SceneMgr();
	void viewRectToPreReadRect(RECT& rcView, RECT& rcPreRead, int nMapWidth, int nMapHeight);
	
private:
	bool _LoadBlock(xs::Stream* pStream, POINT ptTileLeftTop, int nTileRow, int nTileCol, IEntityFactory* pEntityFactory);
	bool _LoadTileInfo(xs::Stream* pStream, Tile* pTile, POINT& ptTile, IEntityFactory* pEntityFactory);
	bool _LoadTileInfoOld(xs::Stream* pStream, Tile* pTile, POINT& ptTile, IEntityFactory* pEntityFactory);
	bool _SaveBlock(xs::Stream* pStream, POINT ptTileLeftTop, int nTileRow, int nTileCol);
	bool _SaveMultiEmptyTileInfo(xs::Stream* pStream, int nCount);
	bool _SaveMultiTileInfo(xs::Stream* pStream, POINT* ptTileList, int nCount);
	bool _SaveTileInfo(xs::Stream* pStream, Tile* pTile);
	void addMultiOccupantValue(POINT pt, EntityView *pEntity);
	void removeMultiOccupantValue(POINT pt, EntityView *pEntity);
	bool testIntersect(POINT pt, EntityView* pEntity);
	
	//add by yhc
	//获取本地图的特效信息
	bool GetBlockEf(xs::Stream* pStream, POINT ptTileLeftTop, int nTileRow, int nTileCol);
	bool GetTileEf(xs::Stream* pStream, Tile* pTile, POINT& ptTile);
protected:
	bool createMapArray(RECT& rcPreRead);
	
protected:
    //  版本号；
    int m_nMapVersion;


#if defined(RELEASEDEBUG) || defined(_DEBUG)
private:
	void AddTileInfo(POINT ptTile, string strFileName);

	typedef list<string> TLIST_NAME;
	typedef map<DWORD, TLIST_NAME> TMAP_TILEINFO;
	TMAP_TILEINFO m_mapTileInfo;
#endif

};


#endif
