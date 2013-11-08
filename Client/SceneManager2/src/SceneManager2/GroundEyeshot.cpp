#include "stdafx.h"
#include "GroundEyeshot.h"
#include "lzss.h"
#include "ResourceLoaderGT.h"
#include "IGlobalClient.h"
#include "IEntityClient.h"
#include "ISchemeCenter.h"

bool GroundEyeshot::create(
						   SceneMgr *pSceneMgr,
						   int nMapWidth, 
						   int nMapHeight, 
						   int nGridWidth, 
						   int nGridHeight,
						   xs::Rect* lprcViewport,
						   xs::IResourceManager *pResourceManager,
						   xs::Stream *pMapFile,
						   const std::string& mapFilename,
						   bool bDynamic)
{
	m_bDynamic = bDynamic;
	m_pResourceManager = pResourceManager;

	m_hash = STR2ID(mapFilename.c_str());

	if(m_pResourceManager && NULL == m_ResourceLoader)
	{
		m_ResourceLoader = new ResourceLoaderGT();
		m_pResourceManager->registerResourceLoader(typeResourceGroundTile,m_ResourceLoader);
	}
	m_pSceneMgr = pSceneMgr;

	m_nMapWidth = nMapWidth;
	m_nMapHeight = nMapHeight;
	m_rcMapRect.right = nMapWidth;
	m_rcMapRect.bottom = nMapHeight;

	m_nGridWidth = nGridWidth;
	m_nGridHeight = nGridHeight;

	m_nViewWidth = lprcViewport->right - lprcViewport->left;
	m_nViewHeight = lprcViewport->bottom - lprcViewport->top;
	
	m_nTotalGridX = Ceil(m_nMapWidth, nGridWidth);
	m_nTotalGridY = Ceil(m_nMapHeight, nGridHeight);

	m_SceneCo.create(nMapWidth, nMapHeight);

	int nTotalGrid = m_nTotalGridX * m_nTotalGridY;
	m_pGroundTile = new GroundTile[nTotalGrid];
	if(m_pGroundTile == 0)
		return false;

	Info("m_pGroundTile:"<<(ulong)m_pGroundTile<<endl);

	for(int i = 0;i < nTotalGrid;i++)
	{
		m_pGroundTile[i].m_pGround = this;
		m_pGroundTile[i].m_ptCoord.x = i % m_nTotalGridX;
		m_pGroundTile[i].m_ptCoord.y = i / m_nTotalGridX;

		
	}

	if(pMapFile)
	{
		load(pMapFile);
	}

	//for(int i = 0;i < nTotalGrid;i++)
	//{
	//	//add by yhc//预加载地表,解决动态加载地表卡的问题
	//	GroundTile * pGT = &m_pGroundTile[i];
	//	if ( pGT != NULL && m_pResourceManager != NULL && m_pMapStream != NULL )
	//	{
	//		m_pMapStream->seek(m_pGroundTileTable[i]);
	//		pGT->loadAlphaMaps(m_pMapStream);
	//		pGT->setResHandle(m_pResourceManager->requestResource((m_hash ^ (ulong)pGT),typeResourceGroundTile,true));
	//	}
	//}

	xs::Rect rc = *lprcViewport;
	/*if(!m_bDynamic)
	{
		rc.left = 0;
		rc.top = 0;
		rc.right = nMapWidth;
		rc.bottom = nMapHeight;
	}*/
	m_bFirstEnterArea = true;
	if(!m_Eyeshot.create(this,nMapWidth, nMapHeight, nGridWidth, nGridHeight, &rc,m_bDynamic))
	{
		m_bFirstEnterArea =false;
		return false;
	}
	
	calcViewTileRect();

	m_groundRenderQueue.initialize();
	m_bFirstEnterArea =false;
	return true;
}

void GroundEyeshot::close()
{
	m_pMapStream = 0;
	m_groundRenderQueue.finalize();
	m_vTextures.clear();
	safeDeleteArray(m_pGroundTileTable);
	if(m_pGroundTile)
	{
		int nTotalGrid = m_nTotalGridX * m_nTotalGridY;
		for(int i = 0;i < nTotalGrid;i++)
		{
			handle h = m_pGroundTile[i].getResHandle();
			if(m_pResourceManager)
			{
				m_pResourceManager->releaseResource(h);
				m_pGroundTile[i].setResHandle(0);
			}
		}
	}

	//资源加载器不能释放，否则会造成异步加载时加载器为野指针
// 	if(m_pResourceManager)
// 	{
// 		m_pResourceManager->unregisterResourceLoader(typeResourceGroundTile);
// 		safeDelete(m_ResourceLoader);
// 	}
	safeDeleteArray(m_pGroundTile);
	
	safeRelease(m_pDefaultTexture);
}

bool GroundEyeshot::viewportSizeChanged(int nWidth, int nHeight)
{
	if(nWidth == 0 || nHeight == 0)
		return false;
	
	m_nViewWidth = nWidth;
	m_nViewHeight = nHeight;

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
	calcViewTileRect();
	return true;
}


bool GroundEyeshot::moveViewportTo(int x, int y)
{
	m_Eyeshot.moveViewportTo(x,y);
	calcViewTileRect();
	return true;
}

void GroundEyeshot::_drawTile(int i,int j)
{
	GroundTile *pTile = static_cast<GroundTile*>(getGroundTile(i,j));

	if(!pTile->isLoaded())
	{
		if(m_pGroundTileTable)
		{
			m_pMapStream->seek(m_pGroundTileTable[j * m_nTotalGridX + i]);
			pTile->load(m_pMapStream/*,true*/);
		}
		else
		{
			pTile->setLoaded(true);
		}
	}
	if(pTile->isLoaded())
	{
		m_groundRenderQueue.addTile(pTile);
	}
}

void GroundEyeshot::onDraw(bool bWholeGround)
{
	PP_BY_NAME_START("GroundEyeshot::onDraw::m_groundRenderQueue.RenderDistance");
    //  绘制远景；
    int nViewTopX = m_Eyeshot.getViewTopX();
    int nViewTopY = m_Eyeshot.getViewTopY();
    if (m_pDistanceTexture != NULL)
    {
        m_groundRenderQueue.RenderDistance(m_nMapWidth, m_nMapHeight, m_nViewWidth, m_nViewHeight, nViewTopX, nViewTopY,
            m_pDistanceTexture);
    }
	PP_BY_NAME_STOP();


	PP_BY_NAME_START("GroundEyeshot::onDraw::m_groundRenderQueue.render");
    //  绘制Tile；
    m_groundRenderQueue.reset();
	if(bWholeGround)
	{
		for(int i = 0;i < m_nTotalGridX;i++)
		for(int j = 0;j < m_nTotalGridY;j++)
		{
			GroundTile *pTile = static_cast<GroundTile*>(getGroundTile(i,j));
			m_groundRenderQueue.addTile(pTile);
		}
	}
	else
	{
		int col = (nViewTopX + m_nViewWidth - 1) / m_nGridWidth - nViewTopX / m_nGridWidth + 1;
		int row = (nViewTopY + m_nViewHeight - 1) / m_nGridHeight - nViewTopY / m_nGridHeight + 1;
		if(col == 0 || row == 0)
			return;

		int nGridTopX = nViewTopX / m_nGridWidth;
		int nGridTopY = nViewTopY / m_nGridHeight;
        //	尝试解决如下问题：将工具栏上的“显示地表线框”按钮按下后，“新建地图”功能表现不正常；
        //	此处原有实现没有进行越界保护；
        for(int i = nGridTopX; (i < nGridTopX + col) && (i < m_nTotalGridX); i++)
        {
            for(int j = nGridTopY; (j < nGridTopY + row) && (j < m_nTotalGridY); j++)
            {
                GroundTile *pTile = static_cast<GroundTile*>(getGroundTile(i,j));
                m_groundRenderQueue.addTile(pTile);
            }
        }
	}
    m_groundRenderQueue.render();
	PP_BY_NAME_STOP();

    //  绘制网格线；
	if(m_bDrawTileLine)
	{
		PP_BY_NAME_START("m_groundRenderQueue.renderTileLine");
		m_groundRenderQueue.renderTileLine(nViewTopX, nViewTopY);
		PP_BY_NAME_STOP();
	}
}

void GroundEyeshot::save(xs::DataChunk *pDataChunk)
{
	xs::Stream *pDataStream = 0;

	HASH_MAP_NAMESPACE::hash_map<std::string,int>	mTextures;
	std::vector<std::string> vTextures;
	pDataChunk->beginChunk('MTEX',&pDataStream);
	for(int i = 0;i < m_nTotalGridX;i++)
	for(int j = 0;j < m_nTotalGridY;j++)
	{
		GroundTile *pTile = static_cast<GroundTile*>(getGroundTile(i,j));
		uint layerNum = pTile->getTextureLayerNum();
		for(uint k = 0;k < layerNum;k++)
		{
			std::string textureFileName = pTile->getTextureFileName(k);
			if(mTextures.find(textureFileName) != mTextures.end())
			{
			}
			else
			{
				vTextures.push_back(textureFileName);
				mTextures[textureFileName] = vTextures.size() - 1;
			}
		}
	}
	size_t texNum = vTextures.size();
	for(size_t size = 0;size < texNum;size++)
	{
		pDataStream->write(vTextures[size].c_str(),vTextures[size].length() + 1);
	}
	pDataChunk->endChunk();

	xs::DataChunk::stChunk *pTextureIdxChunk = pDataChunk->beginChunk('MTID',&pDataStream);
	for(int i = 0;i < m_nTotalGridX;i++)
	for(int j = 0;j < m_nTotalGridY;j++)
	{
		uint offset = 0;
		pDataStream->write(&offset,sizeof(offset));
	}
	pDataChunk->endChunk();
	uint *pTextureOffsets = (uint*)pTextureIdxChunk->m_pData;

	bool bFirstTime = true;
	uint offset = 0;
	for(int i = 0;i < m_nTotalGridX;i++)
	for(int j = 0;j < m_nTotalGridY;j++)
	{
		pDataChunk->beginChunk('MTIL',&pDataStream);
			if(bFirstTime)
			{
				offset = pTextureOffsets[j * m_nTotalGridX + i] = pDataChunk->getOffset();
				bFirstTime = false;
			}
			else
			{
				pTextureOffsets[j * m_nTotalGridX + i] = offset;
			}

			GroundTile *pTile = static_cast<GroundTile*>(getGroundTile(i,j));
			bool bLoaded = pTile->isLoaded();
			if(!bLoaded)_load(i,j);
			pTile->save(pDataStream,mTextures);
			if(!bLoaded)_unload(i,j);

			offset += pDataStream->getLength();
			offset += CHUNK_MIN_SIZE;
		pDataChunk->endChunk();
	}

    //  记录远景贴图；
    if (!m_strDistanceTextureFileName.empty())
    {
        //  这里自定义了一个Chunk名称'MDIS'，本来想参考一下其他Chunk名称的定义和使用，但是现有代码中只搜索
        //  到了对Chunk名称的使用， 而没有找到定义的地方，因此不排除'MDIS'已经被使用了的可能，这个需要在以
        //  后的开发和整理过程中留意；
        //  2009-06-17；
        pDataChunk->beginChunk('MDIS',&pDataStream);
        BYTE cbFileName = (BYTE)(m_strDistanceTextureFileName.length());
        pDataStream->write(&cbFileName, sizeof(BYTE));
        pDataStream->write(m_strDistanceTextureFileName.c_str(), cbFileName);
        pDataChunk->endChunk();
    }
}

void GroundEyeshot::getNameList(std::vector<std::string> &v,char *pFileNames,uint size)
{
	char *pName = (char*)pFileNames;
	while(pName < (char*)pFileNames + size)
	{
		std::string strName = pName;

		v.push_back(strName);
		pName += strlen(pName) + 1;
	}
}

void GroundEyeshot::load(xs::Stream* pMapFile)
{
	m_pMapStream = pMapFile;
	pMapFile->seekToBegin();

	m_vTextures.clear();
	safeDeleteArray(m_pGroundTileTable);

    m_strDistanceTextureFileName.clear();

	xs::DataChunk chunk;
	xs::DataChunk::stChunk *pChunk = chunk.beginChunk(pMapFile);
	while(pChunk)
	{
		switch(pChunk->m_ui32Type)
		{
		case 'MTEX':
			{
				// 纹理列表
				getNameList(m_vTextures,(char*)pChunk->m_pData,pChunk->m_ui32DataSize);
			}
			break;
		case 'MTID':
			{
				// 对应的纹理ID
				m_pGroundTileTable = new DWORD[m_nTotalGridX * m_nTotalGridY];
				memcpy(m_pGroundTileTable,pChunk->m_pData,pChunk->m_ui32DataSize);
			}
			break;

        case 'MDIS':
            {
				// 远景图片
                ASSERT(pChunk->m_pData != NULL);
                BYTE cbFileNameLength = *((BYTE*)(pChunk->m_pData));
                char* szFileName = new char[cbFileNameLength + 1];
                memcpy(szFileName, (BYTE*)(pChunk->m_pData) + sizeof(BYTE), cbFileNameLength);
                szFileName[cbFileNameLength] = 0;
                SetDistanceTexture(szFileName);

                delete[] szFileName;
            }
            break;
		}
		pChunk = chunk.nextChunk(pMapFile);
	}

	for(int i = 0;i < m_nTotalGridX;i++)
	for(int j = 0;j < m_nTotalGridY;j++)
	{
		GroundTile *pTile = static_cast<GroundTile*>(getGroundTile(i,j));
		m_pMapStream->seek(m_pGroundTileTable[j * m_nTotalGridX + i]);
		pTile->loadBasicInfo(m_pMapStream,m_vTextures);
	}
}

void GroundEyeshot::onFound(int nGridX,int nGridY)
{
	GroundTile *pGT = static_cast<GroundTile*>(getGroundTile(nGridX,nGridY));
	if(!pGT)return;
	
	//Info("Found!"<<nGridY<<","<<nGridX<<","<<(ulong)pGT<<endl);

	// 地表资源ID由mapID、nGridX、nGridY组合产生
	ulong uResID = 0;
	if(getRunType() == RUN_TYPE_GAME)
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

	if(m_pResourceManager)
	{
		if(m_pMapStream)
		{
			m_pMapStream->seek(m_pGroundTileTable[nGridY * m_nTotalGridX + nGridX]);
			pGT->loadAlphaMaps(m_pMapStream);
			
			//PP_BY_NAME_START("同步加载地表");
			//一定不能在主线程里面做加载和解码
			//add by yhc，防止地图加载时候出现花屏,在创建的时候使用同步加载
			if(m_bFirstEnterArea)
				pGT->setResHandle(m_pResourceManager->requestResource(uResID,typeResourceGroundTile,true));
			else
				pGT->setResHandle(m_pResourceManager->requestResource(uResID,typeResourceGroundTile,false,10));//by yhc 地表的优先级高
			//PP_BY_NAME_STOP();
			//是否播放区域的音效
			if(getRunType() == RUN_TYPE_EDITOR)
			{
				return ;
			}
		}
		else
		{
			//地图编辑器创建新地图的时候，m_pMapStream == 0
			//ulong lst = ((IGlobalClient*)::xs::getGlobal())->getTimeStamp();
			//一定不能在主线程里面做加载和解码
			if(m_bFirstEnterArea)
				pGT->setResHandle(m_pResourceManager->requestResource(uResID,typeResourceGroundTile,true));
			else
				pGT->setResHandle(m_pResourceManager->requestResource(uResID,typeResourceGroundTile,false,10));//by yhc 地表的优先级高 
			//pGT->setResHandle(m_pResourceManager->requestResource((m_hash ^ (ulong)pGT),typeResourceGroundTile,true));
			//ulong lend = lst - ((IGlobalClient*)::xs::getGlobal())->getTimeStamp();
		}
	}
}

void GroundEyeshot::onLost(int nGridX,int nGridY)
{
	//Info("Lost!"<<nGridY<<","<<nGridX<<endl);

	GroundTile *pGT = static_cast<GroundTile*>(getGroundTile(nGridX,nGridY));
	if(!pGT)return;

	if(m_pResourceManager && m_bDynamic)
	{
		//不释放了
		m_pResourceManager->releaseResource(pGT->getResHandle());
		pGT->setResHandle(0);
	}
}

void GroundEyeshot::scrollViewport(int dx, int dy)
{
	//Info("ScrollViewport:"<<dx<<","<<dy<<endl);

	//滚动的范围过大，认为是第一次进入本区域，防止花屏用同步加载
	if(dy>m_nGridWidth||dy<-m_nGridWidth || dy>m_nGridHeight||dy<-m_nGridHeight )
		m_bFirstEnterArea = true;
	if (!m_Eyeshot.scrollViewport(dx,dy))
	{
		m_bFirstEnterArea = false;
		return;
	}
	calcViewTileRect();
	m_bFirstEnterArea = false;
}

void GroundEyeshot::calcViewTileRect()
{
	SetRect(&m_rcViewportRect,m_Eyeshot.getViewTopX(),m_Eyeshot.getViewTopY(),
		m_Eyeshot.getViewTopX() + m_Eyeshot.getViewWidth(),
		m_Eyeshot.getViewTopY() + m_Eyeshot.getViewHeight());

	m_rcViewportTileRect = m_SceneCo.pixelRectToAreaTileRect(m_rcViewportRect);
}

void GroundEyeshot::pixelToViewTop(const xs::Point &ptTile, xs::Point &ptTileCenter) const
{
	ptTileCenter = ptTile;
	ptTileCenter.x -= m_Eyeshot.getViewTopX();
	ptTileCenter.y -= m_Eyeshot.getViewTopY();
}

void GroundEyeshot::tile2Pixel(const xs::Point &ptTile, xs::Point &ptTileCenter) const
{
	m_SceneCo.tile2Pixel(ptTile,ptTileCenter);
	ptTileCenter.x -= m_Eyeshot.getViewTopX();
	ptTileCenter.y -= m_Eyeshot.getViewTopY();
}

void GroundEyeshot::pixel2Tile(const xs::Point &ptScreen, xs::Point &ptTile) const
{
	xs::Point ptWorld;
	ptWorld.x = ptScreen.x + m_Eyeshot.getViewTopX();
	ptWorld.y = ptScreen.y + m_Eyeshot.getViewTopY();
	m_SceneCo.pixel2Tile(ptWorld,ptTile);
}

void GroundEyeshot::drawTileLine()
{
	int nViewTopX = m_Eyeshot.getViewTopX();
	int nViewTopY = m_Eyeshot.getViewTopY();
	xs::Point ktl(nViewTopX / 64,nViewTopY / 32);

	int col = m_Eyeshot.getViewWidth() / 64 + 2;
	int row = m_Eyeshot.getViewHeight() / 32 + 2;
	int w =  col * 64;
	int h = col * 32;

	IRenderEngine *pRenderEngine = getRenderEngine();
	IRenderSystem *pRenderSystem = pRenderEngine->getRenderSystem();
	ColorValue color = pRenderSystem->getColor();
	pRenderSystem->beginPrimitive(PT_LINES);
	// 绿色
	pRenderSystem->setColor(ColorValue(0,0.48f,0));

	int x = -(nViewTopX - ktl.x * 64);
	int y = -(nViewTopY - ktl.y * 32) - h + 16;
	for(int i = 0; i < col; i++)
	{
		xs::Point ptFrom (x,y);
		xs::Point ptTo (x + w, y + h);
		pRenderSystem->sendVertex(Vector3(ptFrom.x,ptFrom.y,-0.998));
		pRenderSystem->sendVertex(Vector3(ptTo.x,ptTo.y,-0.998));
		y += 32;
	}

	for(int i = 0; i < row; i++)
	{
		xs::Point ptFrom (x,y);
		xs::Point ptTo (x + w, y + h);

		pRenderSystem->sendVertex(Vector3(ptFrom.x,ptFrom.y,-0.998));
		pRenderSystem->sendVertex(Vector3(ptTo.x,ptTo.y,-0.998));
		ptFrom.x = x; ptFrom.y = y;
		ptTo.x = x + w; ptTo.y = y - h;

		pRenderSystem->sendVertex(Vector3(ptFrom.x,ptFrom.y,-0.998));
		pRenderSystem->sendVertex(Vector3(ptTo.x,ptTo.y,-0.998));

		y += 32;
	}

	for(int i = 0; i < col; i++)
	{
		xs::Point ptFrom (x,y);
		xs::Point ptTo (x + w, y - h);

		pRenderSystem->sendVertex(Vector3(ptFrom.x,ptFrom.y,-0.998));
		pRenderSystem->sendVertex(Vector3(ptTo.x,ptTo.y,-0.998));

		y += 32;
	}
	pRenderSystem->endPrimitive();
	pRenderSystem->setColor(color);
}

IGroundTile* GroundEyeshot::getGroundTile(const xs::Point& ptWorld)
{
	if(PtInRect(&m_rcMapRect,ptWorld))
	{
		int nGridX = ptWorld.x / m_nGridWidth;
		int nGridY = ptWorld.y / m_nGridHeight;
		int nIdx = m_nTotalGridX * nGridY + nGridX;
	
		return &m_pGroundTile[nIdx];
	}

	return 0;
}

void GroundEyeshot::setDefaultTexture(const char* szFileName)
{
	m_defaultTexture = szFileName;
	if(m_pDefaultTexture)
	{
		m_pDefaultTexture->release();
	}
	m_pDefaultTexture = getRenderEngine()->getRenderSystem()->getTextureManager()->createTextureFromFile(szFileName,FO_POINT,FO_POINT,FO_NONE,TAM_WRAP,TAM_WRAP);
}

ITexture* GroundEyeshot::getDefaultTexture()
{
	return m_pDefaultTexture;
}

xs::Point GroundEyeshot::getViewLeftTop()
{
	xs::Point pt (m_Eyeshot.getViewTopX(),m_Eyeshot.getViewTopY());
	return pt;
}

int	GroundEyeshot::getGroundTileWidth()
{
	return m_nGridWidth;
}

int	GroundEyeshot::getGroundTileHeight()
{
	return m_nGridHeight;
}

int	GroundEyeshot::getTotalTileX()
{
	return m_nTotalGridX;
}

int GroundEyeshot::getTotalTileY()
{
	return m_nTotalGridY;
}

IGroundTile*	GroundEyeshot::getGroundTile(int x,int y)
{
	//如果传进来的值；
	int nTotal = y * m_nTotalGridX + x;
	if (nTotal < 0 || nTotal >= m_nTotalGridX * m_nTotalGridY)
	{
		return NULL;
	}
	//
	return &m_pGroundTile[y * m_nTotalGridX + x];
}

void			GroundEyeshot::setDrawTileLine(bool bDraw)
{
	m_bDrawTileLine = bDraw;
}

bool			GroundEyeshot::isDrawTileLine()
{
	return m_bDrawTileLine;
}

bool			GroundEyeshot::fillMap(const char* szFilename)
{
	bool bFilled = false;
	for(int i = 0;i < m_nTotalGridX;i++)
	for(int j = 0;j < m_nTotalGridY;j++)
	{
		GroundTile *pTile = static_cast<GroundTile*>(getGroundTile(i,j));
		if(pTile->getTextureLayerNum() == 0)
		{
			pTile->setTextureLayerNum(1);
			pTile->setTextureFileName(0,szFilename,false);
			bFilled = true;
		}
	}

	return bFilled;
}

bool			GroundEyeshot::_isLoaded(int x,int y)
{
	IGroundTile *pTile = getGroundTile(x,y);
	return static_cast<GroundTile*>(pTile)->isLoaded();
}

void			GroundEyeshot::_load(int x,int y)
{
	IGroundTile *pTile = getGroundTile(x,y);
	if(m_pGroundTileTable)
	{
		m_pMapStream->seek(m_pGroundTileTable[y * m_nTotalGridX + x]);
		static_cast<GroundTile*>(pTile)->load(m_pMapStream/*,false*/);
	}
	else
	{
		static_cast<GroundTile*>(pTile)->setLoaded(true);
	}
}

void			GroundEyeshot::_unload(int x,int y)
{
	IGroundTile *pTile = getGroundTile(x,y);
	pTile->setTextureLayerNum(0);
	static_cast<GroundTile*>(pTile)->setLoaded(false);
}

//  设置远景贴图；
//  @param szFileName 贴图文件名；
void GroundEyeshot::SetDistanceTexture(const char* szFileName)
{
    if (szFileName == NULL)
    {
        ASSERT(false);
        return;
    }

    m_strDistanceTextureFileName = szFileName;
    if(m_pDistanceTexture != NULL)
    {
        m_pDistanceTexture->release();
        m_pDistanceTexture = NULL;
    }
    if (!m_strDistanceTextureFileName.empty())
    {
        m_pDistanceTexture = getRenderEngine()->getRenderSystem()->getTextureManager()->createTextureFromFile(m_strDistanceTextureFileName,
            FO_POINT, FO_POINT, FO_NONE,TAM_WRAP, TAM_WRAP);
        ASSERT(m_pDistanceTexture != NULL);
    }
}

//  取得远景贴图的指针；
//  @return 贴图指针；
ITexture* GroundEyeshot::GetDistanceTexture()
{
    return m_pDistanceTexture;
}
