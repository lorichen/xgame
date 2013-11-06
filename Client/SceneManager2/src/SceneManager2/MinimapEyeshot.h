#ifndef __MinimapEyeshot_H__
#define __MinimapEyeshot_H__

#include "Eyeshot.h"
#include "ISceneManager2.h"

class MinimapTile;

class MinimapEyeshot : public IEyeshotCallback,public IMinimap
{
	virtual void drawWorldMap(IRenderSystem *pRenderSystem,const RECT& rcDraw);
	// 绘制小地图
	virtual void draw(IRenderSystem *pRenderSystem,const RECT& rcDraw);
	// 得到视口
	virtual RECT getViewport();

	/** 改变视口大小
	*/
	virtual bool ViewportSizeChanged(int nMultiple);

	/**
	@param x 块X下标
	@param y 块Y下标
	@return 块指针
	*/
	virtual MinimapTile*	getMiniMapTile(int x,int y);

public:
	// 调整视口
    bool scrollViewport(int dx, int dy);
	// 移动视口位置
	bool moveViewportTo(int x, int y,int dx, int dy);
	// 改变视口大小
	bool onViewportSizeChanged(int width,int height);
	
	const std::string& getPath()
	{
		return m_path;
	}

	// 加载遮罩图
	void loadMinimapMask();

private:
	Eyeshot	m_Eyeshot;
	std::string	m_path;
	std::string m_worldmap;
	MinimapTile*	m_pTile;
	IResourceManager*	m_pResourceManager;
	ISceneManager2*		m_pSceneManager;

	int m_nViewWidth;
	int m_nViewHeight;

	int m_nGridWidth;
	int m_nGridHeight;

	int	m_nTotalGridX;
	int	m_nTotalGridY;

	int	m_nMapWidth;
	int m_nMapHeight;

	int m_nTileWidth;
	int m_nTileHeight;

	ITexture *m_pMinimapMask;
	ImageRect	*m_pWorldMap;

public:
	void close();
	bool create(ISceneManager2 *pSceneManager,int mapWidth,int mapHeight,int gridWidth,int gridHeight,const POINT& ptCenter,xs::IResourceManager *pResourceManager,const std::string& mapName = "");

	MinimapEyeshot() : m_pResourceManager(0),m_pTile(0)
	{
		m_nViewWidth = 0;
		m_nViewHeight = 0;
		m_nGridWidth = 0;
		m_nGridHeight = 0;
		m_nTotalGridX = 0;
		m_nTotalGridY = 0;
		m_pSceneManager = 0;
		m_pMinimapMask = 0;
		m_pWorldMap = 0;
		m_pResourceManager = 0;
	}

	~MinimapEyeshot()
	{
		close();
	}

private:
	virtual void onChanged(int nMoveDirection, int nDx, int nDy){}
	virtual void onFound(int nGridX,int nGridY);
	virtual void onLost(int nGridX,int nGridY);
};

#endif
