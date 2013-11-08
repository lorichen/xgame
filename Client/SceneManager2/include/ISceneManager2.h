
	/*! \addtogroup SceneManager
	*  场景管理
	*  @{
	*/
#ifndef __I_SceneManager2_H__
#define __I_SceneManager2_H__

#include "Tile.h"

struct IEntityFactory;
/**从地图上枚举一个区域内的所有Tile的结构体
*/
struct SnapshotTileInfo
{
	xs::Point		ptTile;
	Tile*		pTile;
};

/**从地图上枚举一个区域内的所有EntityView的结构体
*/
struct SnapshotItemInfo 
{
	xs::Point		ptTile;
	EntityView*	pEntity;
};

enum
{
	/// 是否绘制Tile格子
	eDrawGrid = 0x00000001,

	/// 是否绘制占位块
	eDrawOccupant = 0x00000002,

	/// 是否绘制地表
	eDrawGround = 0x00000004,

	/// 是否绘制排序点
	eDrawSortPoint = 0x00000008,

	/// 是否绘制锚点
	eDrawAnchor = 0x00000010,

	/// 是否绘制2D对象
	eDraw2D = 0x000000020,

	///是否绘制3D对象
	eDraw3D = 0x00000040,

	///是否绘制Grid网格(客户端管理地表和物件的最小对象)
	eDrawMapGrid = 0x00000080,

	///是否绘制服务器网格
	eDraw9Grid = 0x00000100,

    //  是否绘制实体信息：血条、蓝条和姓名；
    //  注意此处添加的处理比较简单，并没有对实体类型进行分类判断，也没有将血条、蓝条和姓名分开来
    //  进行判断，并且也没有考虑实体自身的绘制标志，相应的这些处理在策划确定实际表现后再进行对应
    //  的实现；
    eDrawEntityInfo = 0x00000200,

	//  绘制宝箱的信息：当前宝箱只有名字。
	eDrawEntityBoxInfo = 0x00000400	
};

/**地表块，256 * 256，最多4层贴图
*/
struct IGroundTile
{

	/**获得第index层贴图
	@param index 层(0-3)
	@return 贴图指针
	*/
	virtual ITexture*	getTexture(uint index) = 0;

	/**获得第index层贴图的文件名
	@param index 层
	@return 贴图文件名
	*/
	virtual const char*		getTextureFileName(uint index) = 0;

	/**设置第index层的贴图文件名
	@param index 层
	@param szFileName 贴图文件名
	@param bLoad2GPU 是否立即加载贴图
	*/
	virtual void		setTextureFileName(uint index,const char* szFileName,bool bLoad2GPU) = 0;

	/**获得第index层Alpha贴图数据
	@param index 层(0-2)
	@return 贴图数据
	*/
	virtual	uchar*		getAlphaMap(uint index) = 0;

	/**更新Alpha贴图
	@param index 层
	*/
	virtual void		updateAlphaMap(uint index) = 0;

	/**获得阴影贴图的数据
	@return 贴图数据
	*/
	virtual uchar*		getShadowMap() = 0;

	/**更新阴影贴图数据
	*/
	virtual void		updateShadowMap() = 0;

	/**获得贴图的层数
	@return 贴图层数
	*/
	virtual uint		getTextureLayerNum() = 0;

	/**设置贴图的层数
	@param num 层数(0-4)
	*/
	virtual void		setTextureLayerNum(uint num) = 0;

	/**取得包围框
	@return 包围框
	*/
	virtual xs::Rect		getRect() = 0;

	/**取得地表的X下标
	@param x X下标
	*/
	virtual int			getX() = 0;

	/**取得地表的Y下标
	@param y Y下标
	*/
	virtual int			getY() = 0;
};

/**地表
*/
struct IGround
{

	/**根据世界坐标取得地表块
	@param ptWorld 世界坐标
	@return 地表块指针
	*/
	virtual IGroundTile*	getGroundTile(const xs::Point& ptWorld) = 0;

	/**根据下标取得地表块
	@param x 地表块X下标
	@param y 地表块Y下标
	@return 地表块指针
	*/
	virtual IGroundTile*	getGroundTile(int x,int y) = 0;

	/**设置默认贴图(在没有贴图的时候)
	@param szFileName 贴图文件名
	*/
	virtual void			setDefaultTexture(const char* szFileName) = 0;

	/**取得默认贴图的指针
	@return 贴图指针
	*/
	virtual ITexture*		getDefaultTexture() = 0;

	/**取得视口在地表中的X,Y坐标
	@return 坐标
	*/
	virtual xs::Point			getViewLeftTop() = 0;

	/**取得地表块的宽度
	@return 地表块宽度
	*/
	virtual int				getGroundTileWidth() = 0;

	/**取得地表块的高度
	@return 地表块高度
	*/
	virtual int				getGroundTileHeight() = 0;

	/**取得总共的横向地表块数量
	@return 地表块数量
	*/
	virtual int				getTotalTileX() = 0;

	/**取得总共的纵向地表块数量
	@return 地表块数量
	*/
	virtual int				getTotalTileY() = 0;

	/**设置是否绘制地表块的边框
	@param bDraw 是否绘制
	*/
	virtual void			setDrawTileLine(bool bDraw) = 0;

	/**取得是否绘制地表块的边框
	@return 是否绘制
	*/
	virtual bool			isDrawTileLine() = 0;

	/**使用贴图来铺满整个地图
	@param szFilename 贴图文件名
	@return 是否成功
	@remarks 用于地图编辑器
	*/
	virtual bool			fillMap(const char* szFilename) = 0;

	/**某处地表块的资源时候已经加载
	@param x 地表块坐标X
	@param y 地表块坐标Y
	@return 是否加载
	@remarks 用于地图编辑器
	*/
	virtual bool			_isLoaded(int x,int y) = 0;

	/**加载某处地表块的资源
	@param x 地表块坐标X
	@param y 地表块坐标Y
	@remarks 用于地图编辑器
	*/
	virtual void			_load(int x,int y) = 0;

	/**卸载某处的地表块的资源
	@param x 地表块坐标X
	@param y 地表块坐标Y
	@remarks 用于地图编辑器
	*/
	virtual void			_unload(int x,int y) = 0;

    //  设置远景贴图；
    //  @param szFileName 贴图文件名；
    virtual void SetDistanceTexture(const char* szFileName) = 0;

    //  取得远景贴图的指针；
    //  @return 贴图指针；
    virtual ITexture* GetDistanceTexture() = 0;
};

class MinimapTile;
struct IMinimap
{
	/**
	*/
	virtual void drawWorldMap(IRenderSystem *pRenderSystem,const xs::Rect& rcDraw) = 0;

	/**
	*/
	virtual void draw(IRenderSystem *pRenderSystem,const xs::Rect& rcDraw) = 0;

	/**
	*/
	virtual xs::Rect getViewport() = 0;

	/** 改变视口大小
	*  倍数
	*/
	virtual bool ViewportSizeChanged(int nMultiple) = 0;

	/**
	@param x 块X下标
	@param y 块Y下标
	@return 块指针
	*/
	virtual MinimapTile*	getMiniMapTile(int x,int y) = 0;
};

enum RunType
{
	RUN_TYPE_GAME,
	RUN_TYPE_EDITOR
};

struct ISceneManager2
{
	/**
	@param ptTile Tile坐标
	@param pEntity 实体对象
	@param dwParam 附带的参数
	@return 成功为true，失败为false
	*/
	virtual bool addEntity(const xs::Point& ptTile, EntityView* pEntity, DWORD dwParam=0) = 0;

	/**
	@param ptFrom 初始Tile坐标
	@param ptTo 目的Tile坐标
	@param pEntity 实体对象
	@param dwParam 附带的参数
	@return 成功为true，失败为false
	*/
	virtual bool moveEntity(const xs::Point& ptFrom, const xs::Point& ptTo, EntityView* pEntity, DWORD dwParam=0) = 0;

	/**
	@param ptTile 实体所在的Tile坐标
	@param pEntity 实体对象
	@param dwParam 附带的参数
	@return 成功为true，失败为false
	*/
	virtual bool removeEntity(const xs::Point& ptTile, EntityView* pEntity, DWORD dwParam=0) = 0;

	/**
	@param ptTile 实体的Tile坐标
	@param pEntity 实体对象
	@return 成功为true，失败为false
	*/
	virtual bool addEntityOccupant(const xs::Point& ptTile, EntityView* pEntity) = 0;

	/**
	@param ptTile 实体的Tile坐标
	@param pEntity 实体对象
	@return 成功为true，失败为false
	*/
	virtual bool removeEntityOccupant(const xs::Point& ptTile, EntityView* pEntity) = 0;

	/**
	*/
	virtual bool createScene(int nMapWidth,int nMapHeight,xs::Rect* lprcViewport) = 0;

	/**
	*/
	virtual bool loadScene(const char* szFilename,const char* szWpFilename,xs::Rect* lprcViewport,IEntityFactory *pEntityFactory,bool bDynamic,const xs::Point* pTileCenter = 0,bool oldVersion = false) = 0;

	/**保存地图
	@param writeOccupants 是否把占位块写到地图上
	*/
	virtual bool save(const char* szFilename,bool writeOccupants = false) = 0;

	/**
	@return 场景的宽度
	*/
	virtual int getSceneWidth() const = 0;

	/**
	@return 场景的高度
	*/
	virtual int getSceneHeight() const = 0;

	/**
	@return 视口的矩形
	*/
	virtual const xs::Rect& getViewportRect() const = 0;

	/**
	@param nViewWidth 视口的宽度
	@param nViewHeight 视口的高度
	@remarks 当视口大小发生变化时调用
	*/
	virtual void onViewportSizeChanged(int nViewWidth, int nViewHeight) = 0;

	/**
	@param dx 滚动视口的x方向值
	@param dy 滚动视口的y方向值
	@return 成功为true，失败为false
	*/
	virtual bool scrollViewport(int dx, int dy) = 0;

	/**
	@param ptScreenCenter 滚动视口到以此点为中心,为屏幕坐标
	@return 成功为true，失败为false
	*/
	virtual bool scroll2Center(xs::Point ptScreenCenter) = 0;

	/**
	*/
	virtual void update(float tick,float deltaTick,IRenderSystem *pRenderSystem) = 0;

	/**
	*/
	virtual void draw(IRenderSystem* pRenderSystem,bool bWholeGround = false) = 0;

	/**
	*/
	virtual void drawTopMost(IRenderSystem* pRenderSystem) = 0;

	/** 取得当前视口左上角的世界坐标X
	@return 世界坐标X
	*/
	virtual int getViewTopLeftX() const = 0;

	/** 取得当前视口左上角的世界坐标Y
	@return 世界坐标Y
	*/
	virtual int getViewTopLeftY() const = 0;

	/**
	@param ptScreen 屏幕坐标
	@param ptTile Tile坐标
	@remarks 将屏幕坐标转换为Tile坐标
	*/
	virtual void screen2Tile(IN const xs::Point& ptScreen, OUT xs::Point& ptTile) const = 0;

	/**
	@param ptTile
	@param ptScreen
	@remarks 将Tile坐标转换为屏幕坐标
	*/
	virtual void tile2Screen(IN const xs::Point& ptTile, OUT xs::Point& ptTileCenter) const = 0;

	/**
	@param ptScreen 屏幕坐标
	@param ptWorld World坐标
	@remarks 将屏幕坐标转换为World坐标
	*/
	virtual void screen2World(IN const xs::Point& ptScreen, OUT xs::Point& ptWorld) const = 0;

	/**
	@param ptWorld World坐标
	@param ptScreen 屏幕坐标
	@remarks 将World坐标转换为屏幕坐标
	*/
	virtual void world2Screen(IN const xs::Point& ptWorld, OUT xs::Point& ptScreen) const = 0;

	/**
	@param ptWorld 世界坐标
	@param ptTile Tile坐标
	@remarks 将世界坐标转换为Tile坐标
	*/
	virtual void world2Tile(IN const xs::Point& ptWorld, OUT xs::Point& ptTile) const = 0;

	/**
	@param ptTile Tile坐标
	@param ptTileCenter 世界坐标
	@remarks 将Tile坐标转换为世界坐标
	*/
	virtual void tile2World(IN const xs::Point& ptTile, OUT xs::Point& ptTileCenter) const = 0;

	/**
	@param ptTile Tile坐标
	@return 返回Tile对象
	*/
	virtual Tile* getTile(const xs::Point& ptTile) const = 0;

	/**  A* 自动寻路时 不能用getTile()去判断title的阻挡信息；因为老的加载Title的范围不够；会将Title的阻挡信息设置为true;
	// 因此暴露此方法供其他模块调用去判断Title的阻挡信息;（WZH 2010.7.26）
	@param ptTile Tile坐标
	@return 返回是否阻挡
	*/
	virtual bool IsAllBlock(const xs::Point& ptTile) = 0;

	/**
	@param ptScreen 屏幕坐标
	@return 根据屏幕坐标返回Tile对象
	*/
	virtual Tile* getTileFromScreen(xs::Point ptScreen) const = 0;

	/**
	@param ptTile Tile坐标
	@return ptTile处的Tile是否为有效的Tile
	*/
	virtual bool isValidTile(const xs::Point& ptTile) = 0;

	/**
	@param dwFlag 要设置的标志位
	@param dwClearFlag 清除的标志位
	@param pParam 附带的参数
	@see eDrawGrid
	*/
	virtual void setDrawFlag(DWORD dwFlag, DWORD dwClearFlag, void* pParam = 0) = 0;

	/**
	@return 标志位
	*/
	virtual DWORD getDrawFlag() = 0;

	virtual bool isValid() = 0;
	/**
	@remarks 释放对象
	*/
	virtual void release() = 0;

	/** 取得一个矩形框内的所有Tiles
	@param rcWorld 矩形框坐标
	@param nListCount 列表大小
	@param pListBuf 取得的Tiles由此返回
	@return 成功为true，失败为false
	*/
	virtual bool enumTileByWorldRect(IN const xs::Rect& rcWorld, IN OUT int& nListCount, OUT SnapshotTileInfo* pListBuf) = 0;

	/** 取得一个矩形框内的所有Entities
	@param rcWorld 矩形框坐标
	@param nListCount 列表大小
	@param pListBuf 取得的Entities由此返回
	@return 成功为true，失败为false
	*/
	virtual bool enumEntityByWorldRect(IN const xs::Rect& rcWorld, IN OUT int& nListCount, OUT SnapshotItemInfo* pListBuf) = 0;

	/** 寻路(非A*)
	@param ptFrom 路径的起始点
	@param ptTo 路径的结束点
	@param ppBuffer 路径由此返回
	@param nPathLen 路径的长度由此返回
	@return 成功为true，失败为false
	*/
	virtual bool findPathEX(xs::Point ptFrom, xs::Point ptTo, xs::Point** ppBuffer, int& nPathLen) = 0;

	/** 寻路(A*)
	@param ptFrom 路径的起始点
	@param ptTo 路径的结束点
	@param ppBuffer 路径由此返回
	@param nPathLen 路径的长度由此返回
	@return 成功为true，失败为false
	*/
	virtual bool findPath(xs::Point ptFrom, xs::Point ptTo, xs::Point** ppBuffer, int& nPathLen) = 0;

	/** 寻路(A*)
	@param ptFrom 路径的起始点
	@param ptTo 路径的结束点
	@param ppBuffer 路径由此返回
	@param nPathLen 路径的长度由此返回
	@param isNoBlock 是否忽略阻挡
	@return 成功为true，失败为false
	*/
	virtual bool findPathAStar(xs::Point ptFrom, xs::Point ptTo, xs::Point** ppBuffer, int& nPathLen,bool isNoBlock = false) = 0;


	/** 路点寻路
	@param ptFrom 路径的起始点
	@param ptTo 路径的结束点
	@param ppBuffer 路径由此返回
	@param nPathLen 路径的长度由此返回
	@return 成功为true，失败为false
	@note 寻找到的路径为所有的路径
	*/
	virtual bool findPathViaWaypoint(xs::Point ptFrom, xs::Point ptTo, xs::Point** ppBuffer, int& nPathLen) = 0;

	/**
	@param pt 屏幕坐标
	@param dwFlag 只有具有dwFlag的EntityView才会被选中
	@return 返回的EntityView
	*/
	virtual EntityView*	hitTest(const xs::Point& pt) = 0;

	/**
	@param pt 屏幕坐标
	@param dwFlag 只有具有dwFlag的EntityView才会被选中
	@param ppEntityViews 返回的EntityView*数组
	@param nNum 传入的ppEntityViews的大小，带回选中的物件的数量
	*/
	virtual void hitTest(const xs::Point& pt,EntityView** ppEntityViews,int& nNum) = 0;

	/** 取得地表对象
	@return 地表对象
	*/
	virtual IGround*	getGround() = 0;

	/**取得Tile数据的宽度，用于远距离寻路
	@return 宽度
	*/
	virtual int getMapTileWidth() = 0;

	/**取得Tile数据的高度，用于远距离寻路
	@return 高度
	*/
	virtual int getMapTileHeight() = 0;

	/**绘制整个地表，用于生成小地图
	@param pRenderSystem 渲染系统
	*/
	virtual void drawEntireGround(IRenderSystem *pRenderSystem) = 0;

	/**生成投影和视图矩阵
	*/
	virtual void setupMatrix(bool wholeScene = false) = 0;

	/**取得投影矩阵
	@return 投影矩阵
	*/
	virtual Matrix4	getProjectionMatrix() = 0;

	/**取得视图矩阵
	@return 视图矩阵
	*/
	virtual Matrix4 getViewMatrix() = 0;

	/**Tile坐标转换到3D坐标
	@param ptTile Tile坐标
	@param vSpace 3D世界坐标
	*/
	virtual void tile2Space(const xs::Point& ptTile,Vector3& vSpace) = 0;

	/**Space坐标转换到World坐标
	*/
	virtual void space2World(const Vector3& vSpace,xs::Point& ptWorld) = 0;

	/**World坐标转换到Space坐标
	*/
	virtual void world2Space(const xs::Point& ptWorld,Vector3& vSpace) = 0;

	/**Space坐标转换到Tile坐标
	*/
	virtual void space2Tile(const Vector3& vSpace,xs::Point& ptTile) = 0;

	/**ptTile1和ptTile2之间的距离
	*/
	virtual int tileDistance(const xs::Point& ptTile1, const xs::Point& ptTile2) = 0;

	/**设置光线的方向,用来生成影子
	*/
	virtual void setLightDirection(const Vector3& v) = 0;

	/**获取小地图接口
	@see IMinimap
	*/
	virtual IMinimap* getMinimap() = 0;

	/**
	*/
	virtual EntityView*	getFirstEntity() = 0;

	virtual EntityView*	getNextEntity() = 0;

	virtual const char*	getMapFileName() = 0;

	virtual ulong test() = 0;

	virtual void setRunType(RunType rt) = 0;

    //  设置远景；
    virtual void SetDistanceTexture(const char *szFilename) = 0;

	//提供给编辑器使用
	//使所有的物件可见
	virtual void SetAllEntityVisible() = 0;
	virtual RunType getRunType() = 0;
	/**
	@param ptScreenCenter 根据时间渐变的滚动视口到以此点为中心,为屏幕坐标
	@return 成功为true，失败为false
	*/
	virtual bool scroll2CenterByTime(xs::Point ptScreenCenter,ulong delta) = 0;

	virtual void vibrate(int nVibrateRange/*振动的最大幅度,象素为单位,范围为10-40之间*/) = 0;

	virtual bool	IsReferencedBySceneManager(EntityView * pEntity) = 0;

	virtual void ShowSkillTile(const xs::Point& ptTile) = 0;
	virtual void ClearSkillTile() = 0;

	// 锁定场景，用于地表加载与场景释放间的同步 [4/15/2011 zgz]
	virtual void   LockScene() = 0;
	
	// 解锁场景，用于地表加载与场景释放间的同步 [4/15/2011 zgz]
	virtual void   UnLockScene() = 0;
};

#if defined(_LIB) || defined(SCENEMANAGER_STATIC_LIB)/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(xs_csm))
extern "C" ISceneManager2* LoadCSM2(xs::IResourceManager *pResourceManager);
#	define	CreateSceneManagerProc	LoadCSM2)
extern "C" ISceneManager2* LoadCSM2(xs::IResourceManager *pResourceManager);
#	define	CreateSceneManagerProc	LoadCSM2
#else /// 动态库版本
typedef ISceneManager2* (RKT_CDECL *procCreateSceneManager)(xs::IResourceManager *pResourceManager);
#	define	CreateSceneManagerProc	DllApi<procCreateSceneManager>::load(MAKE_DLL_NAME(xs_csm), "LoadCSM2")
#	define  CloseSceneManagerProc()	DllApi<procCreateSceneManager>::free()
#endif


#endif

	/** @} */