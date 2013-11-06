#ifndef __GroundEyeshot_H__
#define __GroundEyeshot_H__

#include "GroundTile.h"
#include "Eyeshot.h"
#include "SceneCoord.h"
#include "GroundRenderQueue.h"

class SceneMgr;
class ResourceLoaderGT;

class GroundEyeshot : public IEyeshotCallback,public IGround
{
public:
	/**根据世界坐标取得地表块
	@param ptWorld 世界坐标
	@return 地表块指针
	*/
	virtual IGroundTile*	getGroundTile(const POINT& ptWorld);

	/**根据下标取得地表块
	@param x 地表块X下标
	@param y 地表块Y下标
	@return 地表块指针
	*/
	virtual IGroundTile*	getGroundTile(int x,int y);

	/**设置默认贴图(在没有贴图的时候)
	@param szFileName 贴图文件名
	*/
	virtual void			setDefaultTexture(const char* szFileName);

	/**取得默认贴图的指针
	@return 贴图指针
	*/
	virtual ITexture*		getDefaultTexture();

	/**取得视口在地表中的X,Y坐标
	@return 坐标
	*/
	virtual POINT			getViewLeftTop();

	/**取得地表块的宽度
	@return 地表块宽度
	*/
	virtual int				getGroundTileWidth();

	/**取得地表块的高度
	@return 地表块高度
	*/
	virtual int				getGroundTileHeight();

	/**取得总共的横向地表块数量
	@return 地表块数量
	*/
	virtual int				getTotalTileX();

	/**取得总共的纵向地表块数量
	@return 地表块数量
	*/
	virtual int				getTotalTileY();

	/**设置是否绘制地表块的边框
	@param bDraw 是否绘制
	*/
	virtual void			setDrawTileLine(bool bDraw);

	/**取得是否绘制地表块的边框
	@return 是否绘制
	*/
	virtual bool			isDrawTileLine();

	/**使用贴图来铺满整个地图
	@param szFilename 贴图文件名
	@return 是否成功
	@remarks 用于地图编辑器
	*/
	virtual bool			fillMap(const char* szFilename);

	/**某处地表块的资源时候已经加载
	@param x 地表块坐标X
	@param y 地表块坐标Y
	@return 是否加载
	*/
	virtual bool			_isLoaded(int x,int y);

	/**加载某处地表块的资源
	@param x 地表块坐标X
	@param y 地表块坐标Y
	@remarks 用于地图编辑器
	*/
	virtual void			_load(int x,int y);

	/**卸载某处的地表块的资源
	@param x 地表块坐标X
	@param y 地表块坐标Y
	@remarks 用于地图编辑器
	*/
	virtual void			_unload(int x,int y);

    //  设置远景贴图；
    //  @param szFileName 贴图文件名；
    virtual void SetDistanceTexture(const char* szFileName);

    //  取得远景贴图的指针；
    //  @return 贴图指针；
    virtual ITexture* GetDistanceTexture();

	int m_nMapWidth;
	int m_nMapHeight;
	RECT m_rcMapRect;
	
	int m_nViewWidth;
	int m_nViewHeight;
	
	int m_nGridWidth;
	int m_nGridHeight;
	
	int	m_nTotalGridX;
	int	m_nTotalGridY;

	RECT m_rcViewportRect;
	RECT m_rcViewportTileRect;
	
	SceneCoord	m_SceneCo;
	Eyeshot	m_Eyeshot;
	std::string	m_defaultTexture;
	ITexture*	m_pDefaultTexture;
	bool	m_bDrawTileLine;
	SceneMgr*	m_pSceneMgr;
	xs::Stream*	m_pMapStream;
	std::vector<std::string>	m_vTextures;
	DWORD*		m_pGroundTileTable;

    //  远景贴图；
    std::string	m_strDistanceTextureFileName;
    ITexture* m_pDistanceTexture;

	GroundRenderQueue		m_groundRenderQueue;

	// 运行模式
    int    m_RunType;
public:
	void drawTileLine();
	void pixel2Tile(const POINT &ptScreen, POINT &ptTile) const;
	void pixelToViewTop(const POINT &ptTile, POINT &ptTileCenter) const;
	void tile2Pixel(const POINT &ptTile, POINT &ptTileCenter) const;
	void scrollViewport(int dx, int dy);
	void onDraw(bool bWholeGround);

	void save(xs::DataChunk *pDataChunk);
	void load(xs::Stream* pMapFile);


	const RECT& getViewportRect() const
	{
		return m_rcViewportRect;
	}
	const RECT& getViewportTileRect() const
	{
		return m_rcViewportTileRect;
	}
	const SceneCoord& getSceneCoord() const
	{
		return m_SceneCo;
	}

	int getSceneWidth() const
	{
		return m_nMapWidth;
	}
	int getSceneHeight() const
	{
		return m_nMapHeight;
	}

	const RECT& getMapRect() const
	{
		return m_rcMapRect;
	}

	int getViewTopX() const
	{
		return m_Eyeshot.getViewTopX();
	}

	int getViewTopY() const
	{
		return m_Eyeshot.getViewTopY();
	}

	int getViewWidth() const
	{
		return m_nViewWidth;
	}
	int getViewHeight() const
	{
		return m_nViewHeight;
	}
	// 取得当前场景模式
	int  getRunType() const
	{
		return m_RunType;
	}
	void  setRunType(int runType) 
	{
		 m_RunType = runType;
	}
	void close();
	bool create(SceneMgr *pSceneMgr,int nMapWidth,int nMapHeight,int nGridWidth,int nGridHeight,LPRECT lprcViewport,xs::IResourceManager *pResourceManager,xs::Stream *pMapFile,const std::string& mapFilename,bool bDynamic);
	bool moveViewportTo(int x,int y);
	bool viewportSizeChanged(int nWidth,int nHeight);

	GroundEyeshot()
	{
		m_nMapWidth = 0;
		m_nMapHeight = 0;
		m_nViewWidth = 0;
		m_nViewHeight = 0;

		m_nTotalGridX = 0;
		m_nTotalGridY = 0;

		m_nGridHeight = 256;
		m_nGridWidth = 256;

		m_pGroundTile = 0;
		m_pDefaultTexture = 0;

		memset(&m_rcViewportTileRect,0,sizeof(m_rcViewportTileRect));
		memset(&m_rcMapRect,0,sizeof(m_rcMapRect));

		m_bDrawTileLine = false;
		m_pSceneMgr = 0;
		m_pMapStream = 0;
		m_pGroundTileTable = 0;

		m_pResourceManager = 0;

		m_hash = 0;

		m_ResourceLoader = 0;

        m_strDistanceTextureFileName.clear();
        m_pDistanceTexture = NULL;
		m_RunType = 0;
		m_bFirstEnterArea =false;
	}

	~GroundEyeshot()
	{
		close();
	}

private:
	virtual void onChanged(int nMoveDirection, int nDx, int nDy){}
	virtual void onFound(int nGridX,int nGridY);
	virtual void onLost(int nGridX,int nGridY);

	GroundTile*		m_pGroundTile;
	xs::IResourceManager*	m_pResourceManager;
	ulong	m_hash;
	ResourceLoaderGT*		m_ResourceLoader;

	bool	m_bDynamic;

	//add by yhc，防止地图加载时候出现花屏,在创建的时候使用同步加载
	//是否是刚进入该区域
	bool m_bFirstEnterArea;
protected:
	void calcViewTileRect();
	void getNameList(std::vector<std::string> &v,char *pFileNames,uint size);
	void _drawTile(int i,int j);
};

#endif
