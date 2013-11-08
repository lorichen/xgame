#include "stdafx.h"
#include "MinimapEyeshot.h"
#include "MinimapTile.h"
#include "ResourceLoaderMinimap.h"
#include "IGlobalClient.h"
#include "IEntityClient.h"

extern RunType g_runtype;
#define   MINMAP_VIEW_SIZE 1025       // 小地图的视口
#define   MINMAP_VIEW_CHANGE_SIZE  257   // 小地图视口改变值
bool MinimapEyeshot::create(ISceneManager2 *pSceneManager,int mapWidth,int mapHeight,int gridWidth,int gridHeight,const xs::Point& ptCenter,xs::IResourceManager *pResourceManager,const std::string& mapName)
{
	m_pSceneManager = pSceneManager;
	m_pResourceManager = pResourceManager;
	if(m_pResourceManager)
	{
		m_pResourceManager->registerResourceLoader(typeResourceMinimap,ResourceLoaderMinimap::Instance());
	}

	m_path = "Maps\\";
	m_path += mapName;
	m_worldmap = m_path + ".dds";

	m_nMapWidth = mapWidth;
	m_nMapHeight = mapHeight;

	int mw = mapWidth / 16;	//把地图缩小到1 / 16
	int mh = mapHeight / 16;
	m_nTotalGridX = (mw + 63) / 64;	//再把地图分割为64 * 64的一份
	m_nTotalGridY = (mh + 63) / 64;

	m_nTileWidth = mapWidth / m_nTotalGridX;
	m_nTileHeight = mapHeight / m_nTotalGridY;

	m_pTile = new MinimapTile[m_nTotalGridX * m_nTotalGridY];
	for(int i = 0;i < m_nTotalGridX * m_nTotalGridY;i++)
	{
		m_pTile[i].m_pParent = this;
		m_pTile[i].m_ptCoord.x = i % m_nTotalGridX;
		m_pTile[i].m_ptCoord.y = i / m_nTotalGridX;
	}

	xs::Rect rc;
	// 需要判断是否超过地图最大边界；如果超过；需要重设中心点的位子；
	rc.left = ptCenter.x - MINMAP_VIEW_SIZE;
	if (rc.left < 0)
	{
		rc.left = 0;
	}
	rc.top = ptCenter.y - MINMAP_VIEW_SIZE;
	if (rc.top < 0)
	{
		rc.top = 0;
	}
	rc.right = ptCenter.x + MINMAP_VIEW_SIZE;
	if (rc.right > mapWidth)
	{
		rc.right = mapWidth;
		rc.left = mapWidth - MINMAP_VIEW_SIZE * 2;
	}
	rc.bottom = ptCenter.y + MINMAP_VIEW_SIZE;
	if (rc.bottom > m_nMapHeight)
	{
		rc.bottom = m_nMapHeight;
		rc.top = m_nMapHeight - MINMAP_VIEW_SIZE * 2;
	}

	if(!m_Eyeshot.create(this,mapWidth, mapHeight, m_nTileWidth, m_nTileHeight, &rc,true))
		return false;

	m_nViewWidth = rc.right - rc.left;
	m_nViewHeight = rc.bottom - rc.top;

	m_nGridWidth = m_nTileWidth;
	m_nGridHeight = m_nTileHeight;

	//加载小地图掩模
	loadMinimapMask();

	return true;
}

void MinimapEyeshot::loadMinimapMask()
{
	if(!m_pMinimapMask)
	{
		m_pMinimapMask = getRenderEngine()->getRenderSystem()->getTextureManager()->createTextureFromFile("data\\Maps\\MinimapMask.raw",FO_POINT,FO_POINT,FO_NONE,TAM_CLAMP_TO_EDGE,TAM_CLAMP_TO_EDGE);
	}
}

void MinimapEyeshot::close()
{
	if(m_pResourceManager)
	{
		m_pResourceManager->unregisterResourceLoader(typeResourceMinimap);
	}
	if (m_pWorldMap)
	{
		m_pWorldMap->finalize();
		safeDelete(m_pWorldMap);
	}
	
	safeRelease(m_pMinimapMask);
	safeDeleteArray(m_pTile);
}

void MinimapEyeshot::onFound(int nGridX,int nGridY)
{
	if(nGridX >= m_nTotalGridX || nGridX < 0 || nGridY < 0 || nGridY >= m_nTotalGridY)return;
	if(m_pResourceManager)
	{
		// 小地图资源ID由mapID、nGridX、nGridY组合产生
		ulong uResID = 0;
		if(g_runtype == RUN_TYPE_GAME)
		{
			IEntityClient* pEntityClient = gGlobalClient->getEntityClient();
			if (NULL != pEntityClient)
			{
				uResID = pEntityClient->GetMapID();
			}			
		}		
		uResID <<= 22;
		uResID |= (nGridX << 11);
		uResID |= nGridY;

		MinimapTile *pTile = &m_pTile[nGridY * m_nTotalGridX + nGridX];
		if(!pTile || pTile->getResHandle())return;
		pTile->setResHandle(m_pResourceManager->requestResource(uResID,typeResourceMinimap,false,-120));
	}
}

void MinimapEyeshot::onLost(int nGridX,int nGridY)
{
	if(nGridX >= m_nTotalGridX || nGridX < 0 || nGridY < 0 || nGridY >= m_nTotalGridY)return;
	if(m_pResourceManager)
	{
		MinimapTile *pTile = &m_pTile[nGridY * m_nTotalGridX + nGridX];
		if(!pTile)return;
		handle h = pTile->getResHandle();
		if(!h)return;
		pTile->setResHandle(0);
		m_pResourceManager->releaseResource(h);
	}
}

bool MinimapEyeshot::scrollViewport(int dx, int dy)
{
	return m_Eyeshot.scrollViewport(dx,dy);
}

xs::Rect MinimapEyeshot::getViewport()
{
	return m_Eyeshot.getViewportRect();
}

/** 改变视口大小
*/
bool MinimapEyeshot::ViewportSizeChanged(int nMultiple)
{
	int nViewWidth = m_Eyeshot.getViewWidth();
	int nViewHeight = m_Eyeshot.getViewHeight();
	nViewWidth += MINMAP_VIEW_CHANGE_SIZE * nMultiple;
	nViewHeight += MINMAP_VIEW_CHANGE_SIZE * nMultiple;
	return onViewportSizeChanged(nViewWidth,nViewHeight);
}
void MinimapEyeshot::drawWorldMap(IRenderSystem *pRenderSystem,const xs::Rect& rcDraw)
{
	if(!m_pWorldMap)
	{
		m_pWorldMap = new ImageRect;
		if(m_pWorldMap)
		{
			m_pWorldMap->initialize(pRenderSystem,m_worldmap);
		}
	}
	if(m_pWorldMap)
	{
		Vector3 scale = Vector3((rcDraw.right - rcDraw.left) / (float)m_pWorldMap->getWidth(),(rcDraw.bottom - rcDraw.top) / (float)m_pWorldMap->getHeight(),1.0f);
		Matrix4 mtxScale = Matrix4::IDENTITY;
		mtxScale.setScale(scale);
		Matrix4 mtxWorld = pRenderSystem->getWorldMatrix();
		pRenderSystem->setWorldMatrix(mtxWorld * mtxScale);
		m_pWorldMap->render();
		pRenderSystem->setWorldMatrix(mtxWorld);
	}
}

void MinimapEyeshot::draw(IRenderSystem *pRenderSystem,const xs::Rect& rcDraw)
{
	if(!m_pSceneManager)
		return;

	int nViewTopX = m_Eyeshot.getViewTopX();
	int nViewTopY = m_Eyeshot.getViewTopY();
	int col = (m_nViewWidth + (m_nGridWidth - 1)) / m_nGridWidth + 1;
	int row = (m_nViewHeight + (m_nGridHeight - 1)) / m_nGridHeight + 1;
	if(col == 0 || row == 0)
		return;

	xs::Rect rc = m_Eyeshot.getViewportRect();

	int nGridTopX = nViewTopX / m_nGridWidth;
	int nGridTopY = nViewTopY / m_nGridHeight;

	// 预防数组越界；导致崩溃
	int MaxGridTopX = nGridTopX + col;
	int MaxGridTopY =  nGridTopY + row;
	//if (MaxGridTopX > m_nTotalGridX )
	//{
	//	MaxGridTopX = m_nTotalGridX;
	//}
	//if (MaxGridTopY > m_nTotalGridY)
	//{
	//	MaxGridTopY = m_nTotalGridY;
	//}

	Matrix4 mtxWorld = pRenderSystem->getWorldMatrix();
	Matrix4 mtxScale = Matrix4::IDENTITY;
	mtxScale.setScale(Vector3((rcDraw.right - rcDraw.left) / (float)(rc.right - rc.left),(rcDraw.bottom - rcDraw.top) / (float)(rc.bottom - rc.top),1.0f));
	Matrix4 mtxTranslation;
	mtxTranslation.makeTrans(rcDraw.left,rcDraw.top,0);
	pRenderSystem->setWorldMatrix(mtxWorld * mtxTranslation * mtxScale);

	SceneBlendFactor src,dst;
	pRenderSystem->getSceneBlending(src,dst);
	pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);
	//pRenderSystem->setAlphaCheckEnabled(true);
	//pRenderSystem->setAlphaFunction(CMPF_GREATER,(uchar)0x7F);

	//设置多重纹理混合
	pRenderSystem->setTexture(1,0);
	pRenderSystem->setTexture(0,0);
	TextureStageOperator colorOP, alphaOP;
	TextureStageArgument colorArg0, colorArg1, alphaArg0, alphaArg1;
	pRenderSystem->getTextureStageStatus(1, colorOP, colorArg0, colorArg1, alphaOP, alphaArg0, alphaArg1);
	pRenderSystem->setTextureStageStatus(1, TSO_REPLACE,  TSA_PREVIOUS, TSA_TEXTURE, TSO_MODULATE, TSA_TEXTURE, TSA_PREVIOUS);

	for(int i = nGridTopX;i < MaxGridTopX;i++)
	{

		for(int j = nGridTopY;j < MaxGridTopY;j++)
		{
			 // 此处添加下标判断；防止数组越界；
			if ((j * m_nTotalGridX + i) > (m_nTotalGridX * m_nTotalGridY - 1) )
			{
				continue ;
			}
			MinimapTile *pTile = &m_pTile[j * m_nTotalGridX + i];
			
			if (pTile)
			{
				ITexture *pTexture = pTile->getTexture();
				if(pTexture)
				{
					xs::Rect rcTile;
					rcTile.left = i * m_nTileWidth;
					rcTile.top = j * m_nTileHeight;
					rcTile.right = (i + 1) * m_nTileWidth;
					rcTile.bottom = (j + 1) * m_nTileHeight;
					xs::Rect rcDest;
					if(IntersectRect(&rcDest,&rcTile,&rc))
					{
						float minx,miny,maxx,maxy;
						minx = (rcDest.left - rcTile.left) / (float)m_nTileWidth;
						miny = (rcDest.top - rcTile.top) / (float)m_nTileHeight;
						maxx = (rcDest.right - rcTile.left) / (float)m_nTileWidth;
						maxy = (rcDest.bottom - rcTile.top) / (float)m_nTileHeight;

						float _minx,_miny,_maxx,_maxy;
						_minx = (rcDest.left - rc.left) / (float)(rc.right - rc.left);
						_miny = (rcDest.top - rc.top) / (float)(rc.bottom - rc.top);
						_maxx = (rcDest.right - rc.left) / (float)(rc.right - rc.left);
						_maxy = (rcDest.bottom - rc.top) / (float)(rc.bottom - rc.top);
						OffsetRect(&rcDest,-rc.left,-rc.top);
						pRenderSystem->setTexture(0,pTexture);
						pRenderSystem->setTexture(1,m_pMinimapMask);
						pRenderSystem->beginPrimitive(PT_QUADS);
						pRenderSystem->setMultiTexcoord(0,Vector2(minx,miny));
						pRenderSystem->setMultiTexcoord(1,Vector2(_minx,_miny));
						pRenderSystem->sendVertex(Vector2(rcDest.left,rcDest.top));
						pRenderSystem->setMultiTexcoord(0,Vector2(minx,maxy));
						pRenderSystem->setMultiTexcoord(1,Vector2(_minx,_maxy));
						pRenderSystem->sendVertex(Vector2(rcDest.left,rcDest.bottom));
						pRenderSystem->setMultiTexcoord(0,Vector2(maxx,maxy));
						pRenderSystem->setMultiTexcoord(1,Vector2(_maxx,_maxy));
						pRenderSystem->sendVertex(Vector2(rcDest.right,rcDest.bottom));
						pRenderSystem->setMultiTexcoord(0,Vector2(maxx,miny));
						pRenderSystem->setMultiTexcoord(1,Vector2(_maxx,_miny));
						pRenderSystem->sendVertex(Vector2(rcDest.right,rcDest.top));
						pRenderSystem->endPrimitive();
					}
				}
			}
		}
	}

	pRenderSystem->setTextureStageStatus(1, colorOP, colorArg0, colorArg1, alphaOP, alphaArg0, alphaArg1);
	pRenderSystem->setSceneBlending(src,dst);
	pRenderSystem->setTexture(0,0);
	pRenderSystem->setTexture(1,0);
	pRenderSystem->setWorldMatrix(mtxWorld);
}

bool MinimapEyeshot::onViewportSizeChanged(int nWidth, int nHeight)
{
	if(nWidth == 0 || nHeight == 0)
		return false;

	int nViewTopX = m_Eyeshot.getViewTopX();
	int nViewTopY = m_Eyeshot.getViewTopY();

	int _nViewTopX = nViewTopX;
	int _nViewTopY = nViewTopY;
	if(nViewTopX + nWidth > m_nMapWidth)
	{
		_nViewTopX = m_nMapWidth - nWidth;
	}
	if(nViewTopY + nHeight > m_nMapHeight)
	{
		_nViewTopY = m_nMapHeight - nHeight;
	}
	m_Eyeshot.scrollViewport(_nViewTopX - nViewTopX,_nViewTopY - nViewTopY);
	m_Eyeshot.viewportSizeChanged(nWidth, nHeight);

	m_nViewHeight = nHeight;
	m_nViewWidth = nWidth;

	return true;
}
bool  MinimapEyeshot::moveViewportTo(int x, int y,int dx, int dy)
{
	int nTopX = x - m_nViewWidth/2;
	int nTopY = y - m_nViewHeight/2;
	xs::Rect rc = m_Eyeshot.getViewportRect();
	if (((nTopX + dx ) == rc.left) && ((nTopY + dy ) == rc.top))
	{
		return false;
	}
	return m_Eyeshot.moveViewportTo(nTopX,nTopY);
}


MinimapTile* MinimapEyeshot::getMiniMapTile(int x,int y)
{
	if(x >= m_nTotalGridX || y < 0 || y < 0 || y >= m_nTotalGridY)
		return NULL;
	return  &m_pTile[y * m_nTotalGridX + x];
}