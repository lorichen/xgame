#ifndef __SceneManager_H__
#define __SceneManager_H__

#include "Eyeshot.h"
#include "SceneCoord.h"
#include "GroundEyeshot.h"
#include "SceneMgr.h"
#include "IEntityView.h"
#include "ISceneManager2.h"
#include "SceneEyeshot.h"
#include "MinimapEyeshot.h"
#include "WayPointMgr.h"
#include "PathFindAStar.h"
#include "PathFinder.h"
#include <list>

#define CMP_LT	-1
#define CMP_EQ	0
#define CMP_GT	1
//
//struct EntityViewPtr
//{
//	EntityView* pEntity;
//	int				nCount;
//	EntityViewPtr() : pEntity(0), nCount(0){}
//	bool operator == (const EntityViewPtr * const pEntity)
//	{
//		return (DWORD)pEntity == (DWORD)pEntity->pEntity;
//	}
//	bool operator == (const EntityViewPtr& item)
//	{
//		return pEntity == item.pEntity;
//	}/*
//	bool operator < (const EntityViewPtr& item)
//	{
//		return *pEntity < *item.pEntity;
//	}*/
//	EntityView* operator ->()
//	{
//		return pEntity;
//	}
//	bool isPoint()
//	{
//		return (pEntity->getSortLeft().x == pEntity->getSortRight().x && pEntity->getSortLeft().y == pEntity->getSortRight().y);
//	}
//	int compare(EntityViewPtr& item)
//	{
//		return 0;
//	}
//	bool isValid()
//	{
//		return (pEntity != 0);
//	}
//};
//
//bool operator==(const EntityViewPtr& a,const EntityViewPtr& b);

class SceneManager : public ISceneManager2
{
public:
	/**
	@param ptTile Tile坐标
	@param pEntity 实体对象
	@param dwParam 附带的参数
	@return 成功为true，失败为false
	*/
	virtual bool addEntity(const POINT& ptTile, EntityView* pEntity, DWORD dwParam=0);

	/**
	@param ptFrom 初始Tile坐标
	@param ptTo 目的Tile坐标
	@param pEntity 实体对象
	@param dwParam 附带的参数
	@return 成功为true，失败为false
	*/
	virtual bool moveEntity(const POINT& ptFrom, const POINT& ptTo, EntityView* pEntity, DWORD dwParam=0);

	/**
	@param ptTile 实体所在的Tile坐标
	@param pEntity 实体对象
	@param dwParam 附带的参数
	@return 成功为true，失败为false
	*/
	virtual bool removeEntity(const POINT& ptTile, EntityView* pEntity, DWORD dwParam=0);

	/**
	@param ptTile 实体的Tile坐标
	@param pEntity 实体对象
	@return 成功为true，失败为false
	*/
	virtual bool addEntityOccupant(const POINT& ptTile, EntityView* pEntity);

	/**
	@param ptTile 实体的Tile坐标
	@param pEntity 实体对象
	@return 成功为true，失败为false
	*/
	virtual bool removeEntityOccupant(const POINT& ptTile, EntityView* pEntity);

	virtual bool createScene(int nMapWidth,int nMapHeight,LPRECT lprcViewport);

	virtual bool loadScene(const char* szFilename,const char* szWpFilename,LPRECT lprcViewport,IEntityFactory *pEntityFactory,bool bDynamic,const POINT* pTileCenter = 0,bool oldVersion = false);

	/**
	@param pEntity 实体对象
	@return 成功为true，失败为false
	*/
	virtual bool addGlobalSceneMagic(EntityView* pEntity);

	/**
	@param pEntity 实体对象
	@return 成功为true，失败为false
	*/
	virtual bool removeGlobalSceneMagic(EntityView* pEntity);


	/**
	*/
	virtual bool save(const char* szFilename,bool writeOccupants);

	/**
	@return 场景的宽度
	*/
	virtual int getSceneWidth() const;

	/**
	@return 场景的高度
	*/
	virtual int getSceneHeight() const;

	/**
	@return 视口的矩形
	*/
	virtual const RECT& getViewportRect() const;

	/**
	@param nViewWidth 视口的宽度
	@param nViewHeight 视口的高度
	@remarks 当视口大小发生变化时调用
	*/
	virtual void onViewportSizeChanged(int nViewWidth, int nViewHeight);

	/**
	@param dx 滚动视口的x方向值
	@param dy 滚动视口的y方向值
	@return 成功为true，失败为false
	*/
	virtual bool scrollViewport(int dx, int dy);

	/**
	@param ptWorld 滚动视口到以此点为中心,世界坐标
	@return 成功为true，失败为false
	*/
	virtual bool scroll2Center(POINT ptWorld);

	/**
	*/
	virtual void update(float tick,float deltaTick,IRenderSystem *pRenderSystem);

	/**
	*/
	virtual void draw(IRenderSystem* pRenderSystem,bool bWholeGround = false);

	/**
	*/
	virtual void drawTopMost(IRenderSystem* pRenderSystem);

	/** 取得当前视口左上角的世界坐标X
	@return 世界坐标X
	*/
	virtual int getViewTopLeftX() const;

	/** 取得当前视口左上角的世界坐标Y
	@return 世界坐标Y
	*/
	virtual int getViewTopLeftY() const;

	/**
	@param ptScreen 屏幕坐标
	@param ptTile Tile坐标
	@remarks 将屏幕坐标转换为Tile坐标
	*/
	virtual void screen2Tile(IN const POINT& ptScreen, OUT POINT& ptTile) const;

	/**
	@param ptTile
	@param ptScreen
	@remarks 将Tile坐标转换为屏幕坐标
	*/
	virtual void tile2Screen(IN const POINT& ptTile, OUT POINT& ptTileCenter) const;

	/**
	@param ptScreen 屏幕坐标
	@param ptWorld World坐标
	@remarks 将屏幕坐标转换为World坐标
	*/
	virtual void screen2World(IN const POINT& ptScreen, OUT POINT& ptWorld) const;

	/**
	@param ptWorld World坐标
	@param ptScreen 屏幕坐标
	@remarks 将World坐标转换为屏幕坐标
	*/
	virtual void world2Screen(IN const POINT& ptWorld, OUT POINT& ptScreen) const;

	/**
	@param ptWorld 世界坐标
	@param ptTile Tile坐标
	@remarks 将世界坐标转换为Tile坐标
	*/
	virtual void world2Tile(IN const POINT& ptWorld, OUT POINT& ptTile) const;

	/**
	@param ptTile Tile坐标
	@param ptTileCenter 世界坐标
	@remarks 将Tile坐标转换为世界坐标
	*/
	virtual void tile2World(IN const POINT& ptTile, OUT POINT& ptTileCenter) const;

	/**
	@param ptTile Tile坐标
	@return 返回Tile对象
	*/
	virtual Tile* getTile(const POINT& ptTile) const;

	/**  A* 自动寻路时 不能用getTile()去判断title的阻挡信息；因为老的加载Title的范围不够；会将Title的阻挡信息设置为true;
	// 因此暴露此方法供其他模块调用去判断Title的阻挡信息;（WZH 2010.7.26）
	@param ptTile Tile坐标
	@return 返回是否阻挡
	*/
	virtual bool IsAllBlock(const POINT& ptTile);

	/**
	@param ptScreen 屏幕坐标
	@return 根据屏幕坐标返回Tile对象
	*/
	virtual Tile* getTileFromScreen(POINT ptScreen) const;

	virtual bool isValid();
	/**
	@param ptTile Tile坐标
	@return ptTile处的Tile是否为有效的Tile
	*/
	virtual bool isValidTile(const POINT& ptTile);

	/**
	@param dwFlag 要设置的标志位
	@param dwClearFlag 清除的标志位
	@param pParam 附带的参数
	@see eDrawGrid
	*/
	virtual void setDrawFlag(DWORD dwFlag, DWORD dwClearFlag, void* pParam = 0);

	/**
	@return 标志位
	*/
	virtual DWORD getDrawFlag();

	/**
	@remarks 释放对象
	*/
	virtual void release();

	/** 取得一个矩形框内的所有Tiles
	@param rcWorld 矩形框坐标
	@param nListCount 列表大小
	@param pListBuf 取得的Tiles由此返回
	@return 成功为true，失败为false
	*/
	virtual bool enumTileByWorldRect(IN const RECT& rcWorld, IN OUT int& nListCount, OUT SnapshotTileInfo* pListBuf);

	/** 取得一个矩形框内的所有Entities
	@param rcWorld 矩形框坐标
	@param nListCount 列表大小
	@param pListBuf 取得的Entities由此返回
	@return 成功为true，失败为false
	*/
	virtual bool enumEntityByWorldRect(IN const RECT& rcWorld, IN OUT int& nListCount, OUT SnapshotItemInfo* pListBuf);

	/** 寻路(非A*)
	@param ptFrom 路径的起始点
	@param ptTo 路径的结束点
	@param ppBuffer 路径由此返回
	@param nPathLen 路径的长度由此返回
	@return 成功为true，失败为false
	*/
	virtual bool findPathEX(POINT ptFrom, POINT ptTo, POINT** ppBuffer, int& nPathLen);

	/** 寻路(A*_2)
	@param ptFrom 路径的起始点
	@param ptTo 路径的结束点
	@param ppBuffer 路径由此返回
	@param nPathLen 路径的长度由此返回
	@return 成功为true，失败为false
	*/
	virtual bool findPath(POINT ptFrom, POINT ptTo, POINT** ppBuffer, int& nPathLen);

	/** 寻路(A*)
	@param ptFrom 路径的起始点
	@param ptTo 路径的结束点
	@param ppBuffer 路径由此返回
	@param nPathLen 路径的长度由此返回
	@param isNoBlock 是否忽略阻挡
	@return 成功为true，失败为false
	*/
	virtual bool findPathAStar(POINT ptFrom, POINT ptTo, POINT** ppBuffer, int& nPathLen ,bool isNoBlock = false);

	/** 路点寻路
	@param ptFrom 路径的起始点
	@param ptTo 路径的结束点
	@param ppBuffer 路径由此返回
	@param nPathLen 路径的长度由此返回
	@return 成功为true，失败为false
	*/
	virtual bool findPathViaWaypoint(POINT ptFrom, POINT ptTo, POINT** ppBuffer, int& nPathLen);

	/**
	@param pt 屏幕坐标
	@param dwFlag 只有具有dwFlag的EntityView才会被选中
	@return 返回的EntityView
	*/
	virtual EntityView*	hitTest(const POINT& pt);
	virtual void hitTest(const POINT& pt,EntityView** ppEntityViews,int& nNum);

	/** 取得地表对象
	@return 地表对象
	*/
	virtual IGround*	getGround();

	/**取得Tile数据的宽度，用于远距离寻路
	@return 宽度
	*/
	virtual int getMapTileWidth();

	/**取得Tile数据的高度，用于远距离寻路
	@return 高度
	*/
	virtual int getMapTileHeight();

	/**绘制整个场景，用于生成小地图
	@param pRenderSystem 渲染系统
	@param flags 把实体&flags == true才绘制
	*/
	virtual void drawEntireGround(IRenderSystem *pRenderSystem);

	/**生成投影和视图矩阵
	*/
	virtual void setupMatrix(bool wholeScene = false);

	/**取得投影矩阵
	@return 投影矩阵
	*/
	virtual Matrix4	getProjectionMatrix();

	/**取得视图矩阵
	@return 视图矩阵
	*/
	virtual Matrix4 getViewMatrix();

	/**Tile坐标转换到3D坐标
	@param ptTile Tile坐标
	@param vSpace 3D世界坐标
	*/
	virtual void tile2Space(const POINT& ptTile,Vector3& vSpace);

	/**Space坐标转换到World坐标
	*/
	virtual void space2World(const Vector3& vSpace,POINT& ptWorld);

	/**World坐标转换到Space坐标
	*/
	virtual void world2Space(const POINT& ptWorld,Vector3& vSpace);

	/**Space坐标转换到Tile坐标
	*/
	virtual void space2Tile(const Vector3& vSpace,POINT& ptTile);

	virtual int tileDistance(const POINT& ptTile1, const POINT& ptTile2);

	virtual void setLightDirection(const Vector3& v);
	//振屏
	virtual void vibrate(int nVibrateRange/*振动的最大幅度,象素为单位,范围为10-40之间*/);

	/**获取小地图接口
	@see IMinimap
	*/
	virtual IMinimap* getMinimap();

	/**
	*/
	virtual EntityView*	getFirstEntity();

	virtual EntityView*	getNextEntity();

	virtual const char*	getMapFileName();

	virtual ulong test();

	virtual void setRunType(RunType rt);

    //  设置远景；
    virtual void SetDistanceTexture(const char *szFilename);

	virtual void SetAllEntityVisible();
	virtual RunType getRunType();
	virtual bool scroll2CenterByTime(POINT ptScreenCenter,ulong delta);

	//added by xxh 
	//判断EntityView是否保存在displaylist或者fadeoutlist里面
	//重新加载地图使用
	//删除实体时检查，以免使实体漏删 zgz
	virtual bool	IsReferencedBySceneManager(EntityView * pEntity);

	virtual void   LockScene();

	virtual void   UnLockScene();
private:
	DWORD				m_dwDrawFlag;
	SceneMgr		m_Map;
	SceneCoord		m_SceneCo;
	CPathFindAStar   m_PathFindAStar;
	GroundEyeshot			m_GroundEyeshot;
	SceneEyeshot			m_SceneEyeshot;
	MinimapEyeshot			m_MinimapEyeshot;
	std::string				m_strMapFilename;

	typedef list<EntityView*> DisplayList;
	typedef list<EntityView*>::iterator DisplayListPtr;
	typedef list<EntityView*>::reverse_iterator DisplayListReversePtr;
	DisplayList			m_DisplayList;
	DisplayList			m_FadeOutList;
	DisplayListPtr		m_iteratorCurItem;

	DisplayList			m_VisibleListGround;
	DisplayList			m_VisibleListObj;	
	DisplayList			m_VisibleListMultiOcc;
	DisplayList			m_VisibleListGlobalSceneMagic;// 全局场景特效列表，例如下雨、下雪 [4/19/2011 zgz]

	POINT				m_ptSave;
	xs::MemoryStream		m_mapStream;
	WayPointMgr			m_WayPointManager;

	Matrix4				m_mtxProjection;
	Matrix4				m_mtxView;

	Vector3				m_lightDirection;
	uint				m_numModels;
public:
	typedef list<EntityView*> LogicList;
	typedef list<EntityView*>::iterator LogicListPtr;
	LogicList			m_LogicItemList;
	bool					m_bOldVersion;

	//调试变量
	std::vector<DWORD>	m_flagsD;
	std::vector<DWORD>	m_flagsF;
public:
	bool isItemInViewArea(const POINT &ptTile, EntityView *pItemView);
	bool isItemInLogicArea(const POINT &ptTile, EntityView *pItemView);
	bool isItemVisible(const POINT& ptTile,EntityView *pItemView);
	bool createGroundEyeshot(xs::Stream* pMapFile,LPRECT lprcViewport, IEntityFactory* pEntityFactory,bool bDynamic);
	void createSceneCo(int nMapWidth,int nMapHeight);

	SceneMgr& getSceneMgr()
	{
		return m_Map;
	}

	SceneCoord& getSceneCoord()
	{
		return m_SceneCo;
	}

	SceneManager(xs::IResourceManager *pResourceManager);
	~SceneManager();
	void close();

	virtual int OnDrawSortPoint(const POINT& ptTile, EntityView *pEntity);
	virtual int OnDrawAnchor(const POINT& ptTile, EntityView *pEntity);
	virtual int OnDrawOccupant(const POINT& ptTile,Tile* pTile,void* pParam);
	virtual void OnEntityEnterViewport(const POINT& ptTile,EntityView* pEntity);
	virtual void OnEntityLeaveViewport(const POINT& ptTile,EntityView* pEntity);

	

private:
	void AddItemToDisplayList(EntityView* pEntity);
	void RemoveItemFromDisplayList(EntityView* pEntity);
	void AddItemToFadeOutList(EntityView* pEntity);
	void RemoveItemFromFadeOutList(EntityView* pEntity);

	void updateDisplayList(DisplayList& list,float tick,float deltaTick,IRenderSystem *pRenderSystem);
	void updateGlobalSceneMagic(float tick, float deltaTick, IRenderSystem* pRenderSystem);
	void drawGlobalSceneMagic(IRenderSystem* pRenderSystem); 
	//振屏控制变量
	//振动最大幅度
	int m_nVibrateRange;
	//本次振动的累计时间
	int m_nVibrateTime;
	//本次振动是否完成
	bool m_bVibrateFinish;
public:
	bool IsIntersect(EntityView& item1, EntityView& item2, RECT& rc);
	int cmpPP(EntityView& item1, EntityView& item2);
	int cmpPL(EntityView& item1, EntityView& item2);
	int cmpLL(EntityView& item1, EntityView& item2);
	int compare(EntityView& item1, EntityView& item2);
	void addSortingEntitiesAll(EntityView* pEntity, DisplayList& _DisplayList);
	void addSortingEntities(EntityView* pEntity, DisplayList& _DisplayList);
	void sortVisibleEntities();


private:
	ImageRect*			m_pOccupantImageRect;
	xs::IResourceManager*	m_pResourceManager;

private:
	IShaderProgram	*	m_pShadowProgram;//用于渲染影子


	// add by zjp;用来渲染技能格子
public:
	void ShowSkillTile(const POINT& ptTile);
	void ClearSkillTile();
private:
	list<POINT>			m_ListSkillTile;

	// 用于地表加载与场景释放间的同步 [4/15/2011 zgz]
	xs::Mutex			m_mutex;
};

// 场景锁辅助类
class CSceneLock
{
public:
	CSceneLock();
	~CSceneLock();
private:
	CSceneLock(const CSceneLock& rhs);
	CSceneLock& operator=(const CSceneLock& rhs);
};
#endif
