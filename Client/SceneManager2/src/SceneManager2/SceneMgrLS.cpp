#include "stdafx.h"
#include "SceneMgr.h"
#include "SceneManager.h"
#include "IGlobalClient.h"

#include "IEntityClient.h"
#include "IProgressManager.h"

extern RunType g_runtype;
/************************************************************************/
/* SceneMgrLS means SceneMgr Load and Save implementation       */
/************************************************************************/

void SceneMgr::viewRectToPreReadRect(xs::Rect& rcView, xs::Rect& rcPreRead, int nMapWidth, int nMapHeight)
{
	rcPreRead.left = (rcView.right + rcView.left) / 2 - (TILES_PREREAD_WIDTH) / 2;
	rcPreRead.top = (rcView.bottom + rcView.top) / 2 - (TILES_PREREAD_HEIGHT) / 2;
	if (rcPreRead.left < 0)
		rcPreRead.left = 0;
	if (rcPreRead.top < 0)
		rcPreRead.top = 0;
	if (rcPreRead.left > nMapWidth - TILES_PREREAD_WIDTH)
		rcPreRead.left = nMapWidth - TILES_PREREAD_WIDTH;
	if (rcPreRead.top > nMapHeight - TILES_PREREAD_HEIGHT)
		rcPreRead.top = nMapHeight - TILES_PREREAD_HEIGHT;
	int nGridX = rcPreRead.left >> SHIFT_WIDTH;
	int nGridY = rcPreRead.top >> SHIFT_HEIGHT;
	rcPreRead.left = nGridX << SHIFT_WIDTH;
	rcPreRead.top = nGridY << SHIFT_HEIGHT;
	// 先右移再左移，保证能被2的SHIFT_WIDTH次幂整除
	rcPreRead.right = rcPreRead.left + TILES_PREREAD_WIDTH;
	rcPreRead.bottom = rcPreRead.top + TILES_PREREAD_HEIGHT;
}

bool SceneMgr::save(xs::DataChunk* pDataChunk,bool writeOccupants)
{
	int nGridWidth = GRID_WIDTH;
	int nGridHeight = GRID_HEIGHT;

	if(!isValid())
	{
		return false;
	}

	int nGridRow = Ceil(m_nMapHeight, nGridHeight);
	int nGridCol = Ceil(m_nMapWidth, nGridWidth);

	uint *pOffsetsData = 0;
	xs::Stream *pDataStream = 0;

    //  地图版本信息；
    pDataChunk->beginChunk('MVER',&pDataStream);
    m_nMapVersion = SUPPORT_EFFECT_SCALE_AND_ANGLE_ADJUST_MAP_VERSION;
    pDataStream->write(&m_nMapVersion, sizeof(m_nMapVersion));
    pDataChunk->endChunk();

	pDataChunk->beginChunk('MINF',&pDataStream);
	pDataStream->write(&m_nMapWidth,sizeof(m_nMapWidth));
	pDataStream->write(&m_nMapHeight,sizeof(m_nMapHeight));
	pDataChunk->endChunk();

	xs::DataChunk::stChunk *pIdxChunk = pDataChunk->beginChunk('MIDX',&pDataStream);
	for(int row = 0;row < nGridRow;row++)
	for(int col = 0;col < nGridCol;col++)
	{
		uint offset = 0;
		pDataStream->write(&offset,sizeof(offset));
	}
	pDataChunk->endChunk();
	pOffsetsData = (uint*)pIdxChunk->m_pData;

	m_bWriteOccupants = writeOccupants;
	int nTileRow = nGridHeight / 32;
	int nTileCol = nGridWidth / (64 / 2);
	xs::Point ptTileLeftTop;
	xs::Point ptLeftTop;
	for(int row = 0;row < nGridRow;row++)
	for(int col = 0;col < nGridCol;col++)
	{
		pDataChunk->beginChunk('MDAT',&pDataStream);
			pOffsetsData[row * nGridCol + col] = pDataChunk->getOffset();

			ptLeftTop.x = col * nGridWidth;
			ptLeftTop.y = row * nGridHeight;

			m_SceneCo.pixel2Tile(ptLeftTop, ptTileLeftTop);
			_SaveBlock(pDataStream,ptTileLeftTop,nTileRow,nTileCol);

		pDataChunk->endChunk();
	}
	m_bWriteOccupants = false;

	m_pGround->save(pDataChunk);

	SceneBlock	mb;
	int nTileWidth = m_SceneCo.getMatrixWidth();
	int nTileHeight = m_SceneCo.getMatrixHeight();

	mb.SetMapSize(nTileWidth, nTileHeight);

	for (int row = 0; row < nTileHeight; ++row)
	{
		for (int col = 0; col < nTileWidth; ++col)
		{
			xs::Point ptTile (col, row);
			Tile *pTile = &getTile(ptTile);
			if (!pTile->isValid())
			{
				mb.SetBlock(col, row, true);
			}
			else
			{
				mb.SetBlock(col, row, pTile->isBlock() ? true: false);
			}

		}
	}			

	pDataChunk->beginChunk('MWPT',&pDataStream);
	mb.Save(pDataStream);
	pDataChunk->endChunk();

	return true;
}

bool SceneMgr::load(GroundEyeshot *pGround,xs::Stream* pStream,IEntityFactory* pEntityFactory,xs::Rect* pViewport,const xs::Point* pTileCenter,bool bDynamic,ISceneManager2 *pSceneManager)
{
	//总共进度为6

    if (pSceneManager == NULL)
    {
        ASSERT(false);
        return false;
    }

	IProgressManager * pProMgr = gGlobalClient->getProgressManager();

	//关闭地图的进度为1
	if(pProMgr)
		pProMgr->AdvanceLoadingScenceProgressLength(1);
	close();

	m_pSceneManager = pSceneManager;
	m_bDynamic = bDynamic;
	m_pStream = pStream;
	m_pItemCF = pEntityFactory;
	int nMapWidth,nMapHeight;

	int nGridRow;
	int nGridCol;


	//读文件的进度为1
	if(pProMgr)
		pProMgr->AdvanceLoadingScenceProgressLength(1);

	xs::DataChunk chunk;
	xs::DataChunk::stChunk *pChunk = chunk.beginChunk(pStream);
	while(pChunk)
	{
		switch(pChunk->m_ui32Type)
		{
        case 'MVER':
            {
                xs::MemoryStream stream((uchar*)pChunk->m_pData,pChunk->m_ui32DataSize);
                stream.read(&m_nMapVersion, sizeof(m_nMapVersion));
            }
            break;

		case 'MINF':
			{
				xs::MemoryStream stream((uchar*)pChunk->m_pData,pChunk->m_ui32DataSize);
				stream.read(&nMapWidth,sizeof(nMapWidth));
				stream.read(&nMapHeight,sizeof(nMapHeight));

				nGridRow = (nMapHeight - 1) / GRID_HEIGHT + 1;
				nGridCol = (nMapWidth - 1) / GRID_WIDTH + 1;

				static_cast<SceneManager*>(m_pSceneManager)->createSceneCo(nMapWidth,nMapHeight);
			}
			break;
		case 'MIDX':
			{
				// 初始化记录宫格信息的数组
				m_pMapTable = new DWORD[nGridRow * nGridCol];
				memcpy(m_pMapTable,pChunk->m_pData,pChunk->m_ui32DataSize);
			}
			break;
		case 'MWPT':
			{
				xs::MemoryStream stream((uchar*)pChunk->m_pData,pChunk->m_ui32DataSize);
				m_MapBlockBuffer.Load(&stream);
			}
			break;
		}
		pChunk = chunk.nextChunk(pStream);
	}

	xs::Rect rcRect (0,0,nMapWidth,nMapHeight);

	if(pTileCenter)
	{
		int height = (pViewport->bottom - pViewport->top) >> 1;
		int width = (pViewport->right - pViewport->left) >> 1;
		pViewport->left = -width;
		pViewport->top = -height;
		pViewport->bottom = height;
		pViewport->right = width;
		xs::Point ptWorld;
		m_pSceneManager->tile2World(*pTileCenter,ptWorld);
		//  offsetRece 表示以后边的点为中心点调整传入矩形的坐标，下边分别对每条边进行调整，
		//保证矩形落在地图（Rect{0,0,nMapWidth,nMapHeight}）区域内
		OffsetRect(pViewport,ptWorld.x,ptWorld.y);	
		if(pViewport->left < 0)
		{
			OffsetRect(pViewport,-pViewport->left,0);
		}
		if(pViewport->top < 0)
		{
			OffsetRect(pViewport,0,-pViewport->top);
		}
		if(pViewport->right > nMapWidth)
		{
			OffsetRect(pViewport,-(pViewport->right - nMapWidth),0);
		}
		if(pViewport->bottom > nMapHeight)
		{
			OffsetRect(pViewport,0,-(pViewport->bottom - nMapHeight));
		}
	}
	xs::Rect* lprcViewport = pViewport;
	// 地图和当前场景是否有交叉
	if(!IntersectRect(&rcRect,&rcRect,lprcViewport))
	{
		ErrorLn("!IntersectRect(&rcRect,&rcRect,lprcViewport)");
		return false;
	}

	xs::Rect rcTilesPreRead;
	// 此处将动态加载的条件取消，加载整张地图的阻挡信息；
	//if(m_bDynamic)
	//	viewRectToPreReadRect(*lprcViewport, rcTilesPreRead, nMapWidth, nMapHeight);
	//else
		SetRect(&rcTilesPreRead, 0, 0, nMapWidth, nMapHeight);

	//构建地图的进度为2
	if(pProMgr)
		pProMgr->AdvanceLoadingScenceProgressLength(2);

	if (!create(pGround,nMapWidth,nMapHeight,rcTilesPreRead,m_pSceneManager))
	{
		return false;
	}

	//构建地表观察器的进度为1
	if(pProMgr)
		pProMgr->AdvanceLoadingScenceProgressLength(1);

    if (!static_cast<SceneManager*>(m_pSceneManager)->createGroundEyeshot(pStream, pViewport, pEntityFactory, bDynamic))
    {
        ASSERT(false);
        return false;
    }


 /*  if(!bDynamic)
	{*/
		// note by zjp；
		// 下面载入整张地图的tile信息
		int nTileRow = GRID_HEIGHT / 32;
		int nTileCol = GRID_WIDTH / (64 / 2);
		xs::Point ptTileLeftTop;
		xs::Point ptLeftTop;
		for(int row = 0;row < nGridRow;row++)
		{
			for(int col = 0;col < nGridCol;col++)
			{
				pStream->seek(m_pMapTable[row*nGridCol+col]);

				ptLeftTop.x = col * GRID_WIDTH;
				ptLeftTop.y = row * GRID_HEIGHT;

				m_SceneCo.pixel2Tile(ptLeftTop, ptTileLeftTop);
				if (!_LoadBlock(pStream, ptTileLeftTop, nTileRow, nTileCol, pEntityFactory))
					return false;
			}
		}	
	//}

	//预加载模型的进度为1
	if(pProMgr)
		pProMgr->AdvanceLoadingScenceProgressLength(1);

	IEntityClient * pIEntityClient = ((IGlobalClient*)::xs::getGlobal())->getEntityClient();
	if(pIEntityClient)
	{
		//还没有往预加载列表里面添加的
		if(!m_VectorPreLoad[pIEntityClient->GetMapID()])
		{
			int nTileRow = GRID_HEIGHT / 32;
			int nTileCol = GRID_WIDTH / (64 / 2);

			xs::Point ptTileLeftTop;
			xs::Point ptLeftTop;
			for(int row = 0;row < nGridRow;row++)
			{
				for(int col = 0;col < nGridCol;col++)
				{
					pStream->seek(m_pMapTable[row*nGridCol+col]);

					ptLeftTop.x = col * GRID_WIDTH;
					ptLeftTop.y = row * GRID_HEIGHT;

					m_SceneCo.pixel2Tile(ptLeftTop, ptTileLeftTop);
					if (!GetBlockEf(pStream, ptTileLeftTop, nTileRow, nTileCol))
						return false;
				}
			}
			m_VectorPreLoad[pIEntityClient->GetMapID()] = true;
		}
	}
		
	return true;
}

bool SceneMgr::GetTileEf(xs::Stream* pStream, Tile* pTile, xs::Point& ptTile)
{
	if(ptTile.x == 82 && ptTile.y == 200)
	 int i=0;
	WORD nFlags = 0;
	pStream->read((char*)&nFlags, sizeof(nFlags));
	DWORD dwFlags = (DWORD)nFlags;
    //  这个处理会导致客户端和服务端对地表占位的判断处理不一致，此处暂时将其屏蔽，
    //  进一步观察效果后再进行相应处理；
	//dwFlags &= Tile::tOccupant_Surface;
	dwFlags |= pTile->getFlag();
	pTile->setFlag(dwFlags);

	BYTE nItemCount = 0;
	pStream->read((char*)&nItemCount, sizeof(nItemCount));

	for (int i=0; i<nItemCount; i++)
	{
		uchar entityType;
		pStream->read(&entityType,sizeof(entityType));
		uchar resType;
		pStream->read(&resType,sizeof(resType));
		uchar length;
		pStream->read(&length,sizeof(length));

		char szFilename[256];
		pStream->read(szFilename,length);
		MemoryStream ms;
		ms.write(&length,sizeof(length));
		ms.write(szFilename,length);
		xs::Point pointAnchorOffset;
		pStream->read((void*)&pointAnchorOffset, sizeof(xs::Point));
		ms.seekToBegin();

		//
		//char szResID[256];
		//uchar ucResIDLength;
		//pStream->read(&ucResIDLength, sizeof(uchar));
		//pStream->read(szResID, ucResIDLength);
		switch (entityType)
		{
		case typeEffect:
			{
				/*szResID[ucResIDLength] = 0;*/
				int nResID = *(int*)szFilename/*szResID*/;

				//预加载模型文件
				IEntityClient * pIEntityClient = ((IGlobalClient*)::xs::getGlobal())->getEntityClient();
				if(pIEntityClient)
				{
					DWORD dwMapID = pIEntityClient->GetMapID();
					pIEntityClient->AddEffectToList(dwMapID,nResID);
				}
				//mConfig = ConfigCreatures::Instance()->getCreature(creatureId);
			}
			break;
		}

		if ((m_nMapVersion >= SUPPORT_EFFECT_SCALE_AND_ANGLE_ADJUST_MAP_VERSION) && (entityType == typeEffect))
		{
			float fScale = 1;
			pStream->read((void*)&fScale, sizeof(float));
			ulong ulAngle = 0;
			pStream->read((void*)&ulAngle, sizeof(ulong));
		}

	}

	return true;
}
bool SceneMgr::GetBlockEf(xs::Stream* pStream, xs::Point ptTileLeftTop, int nTileRow, int nTileCol)
{
	xs::Point ptCurTile = ptTileLeftTop;
	bool bIsEmptyTile = false;
	int nCount = 0;
	Tile* pTile = 0;

	for (int row=0; row<nTileRow; row++)
	{
		ptCurTile.x = ptTileLeftTop.x + row;
		ptCurTile.y = ptTileLeftTop.y - row;
		for (int col=0; col<nTileCol; col++)
		{
			if (nCount == 0)
			{
				BYTE btyData;
				pStream->read(&btyData, sizeof(btyData));
				if (btyData & 0x80)
				{
					bIsEmptyTile = false;
					nCount = btyData - 128 + 1;
				}
				else
				{
					bIsEmptyTile = true;
					nCount = btyData + 1;
				}
			}
			pTile = getTilePtr(ptCurTile);
			if (pTile)
			{
				if (!bIsEmptyTile)
				{
					GetTileEf(pStream, pTile, ptCurTile);
				}
				nCount --;
			}
			
			ptCurTile.x += !(col&1);
			ptCurTile.y += col&1;
		}
	}

	return true;
}
bool SceneMgr::_LoadBlock(xs::Stream* pStream, xs::Point ptTileLeftTop, 
							   int nTileRow, int nTileCol, IEntityFactory* pEntityFactory)
{
	xs::Point ptCurTile = ptTileLeftTop;
	bool bIsEmptyTile = false;
	int nCount = 0;
	Tile* pTile = 0;

	for (int row=0; row<nTileRow; row++)
	{
		ptCurTile.x = ptTileLeftTop.x + row;
		ptCurTile.y = ptTileLeftTop.y - row;
		for (int col=0; col<nTileCol; col++)
		{
			if (nCount == 0)
			{
				BYTE btyData;
				pStream->read(&btyData, sizeof(btyData));
				if (btyData & 0x80)
				{
					bIsEmptyTile = false;
					nCount = btyData - 128 + 1;
				}
				else
				{
					bIsEmptyTile = true;
					nCount = btyData + 1;
				}
			}
			pTile = getTilePtr(ptCurTile);
			if (pTile)
			{
				if (!bIsEmptyTile)
				{
					if(!static_cast<SceneManager*>(m_pSceneManager)->m_bOldVersion)
					{
						_LoadTileInfo(pStream, pTile, ptCurTile, pEntityFactory);
					}
					else
					{
						_LoadTileInfoOld(pStream, pTile, ptCurTile, pEntityFactory);
					}
				}
				nCount --;
			}
			
			ptCurTile.x += !(col&1);
			ptCurTile.y += col&1;
		}
	}

	return true;
}

bool SceneMgr::_LoadTileInfoOld(xs::Stream* pStream, Tile* pTile, xs::Point& ptTile, IEntityFactory* pEntityFactory)
{
	WORD nFlags = 0;
	pStream->read((char*)&nFlags, sizeof(nFlags));
	DWORD dwFlags = (DWORD)nFlags;
	dwFlags &= Tile::tOccupant_Surface;
	dwFlags |= pTile->getFlag();
	pTile->setFlag(dwFlags);

	BYTE nItemCount = 0;
	pStream->read((char*)&nItemCount, sizeof(nItemCount));

	int seek = pStream->getPosition();
	for (int i=0; i<nItemCount; i++)
	{
		BYTE nClassID = 0;
		if (!pStream->read((char*)&nClassID, sizeof(nClassID)))
			break;

		int nBeginOff = pStream->getPosition();

		struct EntityMpwCreateParams
		{
			char filename[64];
			DWORD reserved;
			EntityMpwCreateParams()
			{
				memset(this, 0, sizeof(EntityMpwCreateParams));
			}
		};

		if (pEntityFactory == 0)
		{
			pStream->seek(sizeof(EntityMpwCreateParams),SEEK_CUR);
		}
		else
		{
			EntityMpwCreateParams param;
			pStream->read(&param,sizeof(param));
			MemoryStream ms;
			uchar length = strlen(param.filename);
			if(length == 0)continue;
			ms.write(&length,sizeof(length));
			ms.write(param.filename,length);
			ms.seekToBegin();

			EntityView* pEntity = pEntityFactory->createEntity((ulong)typeStatic, (ulong)xs::typeResourcePic,&ms,ms.getLength());
#ifdef MEM_LEAK_TEST
			gGlobalClient->CreateEntityView(pEntity, MemType_MapStatic);		
#endif
			if (pEntity == 0)
			{
				if (pStream->getPosition() != (nBeginOff + sizeof(EntityMpwCreateParams)))
				{
					pStream->seek(nBeginOff + sizeof(EntityMpwCreateParams));
				}
				continue;
			}
			else
			{
				if(g_runtype == RUN_TYPE_EDITOR)
					pEntity->removeFlag(flagFade);
				pEntity->addFlag(flagNoLogic | flagMapItem);

				if (!addEntity(ptTile, pEntity))
					pEntity->release();
				else
				{
					m_EntityList.push_back(pEntity);
					pEntity->onAddEntity();
#if defined(RELEASEDEBUG) || defined(_DEBUG)
					// 用于测试物件是否重复加载
					//this->AddTileInfo(ptTile, param.filename);
#endif
				}
			}
		}
	}
	return true;

}

bool SceneMgr::_LoadTileInfo(xs::Stream* pStream, Tile* pTile, 
								  xs::Point& ptTile, IEntityFactory* pEntityFactory)
{
	WORD nFlags = 0;
	pStream->read((char*)&nFlags, sizeof(nFlags));
	DWORD dwFlags = (DWORD)nFlags;
    //  这个处理会导致客户端和服务端对地表占位的判断处理不一致，此处暂时将其屏蔽，
    //  进一步观察效果后再进行相应处理；
	//dwFlags &= Tile::tOccupant_Surface;
	dwFlags |= pTile->getFlag();


	//add by yhc, 找到了导致阻挡多出来的问题,是已经存到tile信息里面导致的
	//if(ptTile.x==633 && ptTile.y==438)
	//	int i=0;
	//end 

	pTile->setFlag(dwFlags);

	//add by yhc, 找到了导致阻挡多出来的问题,是已经存到tile信息里面导致的
	//把以前存乱的阻挡去掉,只有物件创建才会有这个物件阻挡,tile里面没有这个阻挡
	//if(m_pSceneManager&&m_pSceneManager->getRunType() == RUN_TYPE_EDITOR)
	//	pTile->clear_GroundBlock_Entity();
	//end 

	BYTE nItemCount = 0;
	pStream->read((char*)&nItemCount, sizeof(nItemCount));

	for (int i=0; i<nItemCount; i++)
	{
		uchar entityType;
		pStream->read(&entityType,sizeof(entityType));
		uchar resType;
		pStream->read(&resType,sizeof(resType));
		uchar length;
		pStream->read(&length,sizeof(length));
		if (pEntityFactory == 0)
		{
			pStream->seek(length,SEEK_CUR);
		}
		else
		{
			char szFilename[256] = {0};
			pStream->read(szFilename,length);
			MemoryStream ms;
			ms.write(&length,sizeof(length));
			ms.write(szFilename,length);
			xs::Point pointAnchorOffset;
			pStream->read((void*)&pointAnchorOffset, sizeof(xs::Point));
			ms.seekToBegin();
			
            //
			EntityView* pEntity = pEntityFactory->createEntity((ulong)entityType, (ulong)resType,&ms,ms.getLength());
#ifdef MEM_LEAK_TEST
			if (entityType == typeEffect)
			{
				gGlobalClient->CreateEntityView(pEntity, MemType_MapEffect);
			}
			else if (entityType == typeStatic)
			{
				gGlobalClient->CreateEntityView(pEntity, MemType_MapStatic);
			}
			else
			{
				breakable;
			}
#endif
			if (pEntity)
			{
				if(g_runtype == RUN_TYPE_EDITOR)
					pEntity->removeFlag(flagFade);
				pEntity->addFlag(flagNoLogic | flagMapItem);
				if (!addEntity(ptTile, pEntity))
					pEntity->release();
				else
				{
					m_EntityList.push_back(pEntity);
					pEntity->onAddEntity();
#if defined(RELEASEDEBUG) || defined(_DEBUG)
					// 用于测试物件是否重复加载
					//this->AddTileInfo(ptTile, szFilename);
#endif
				}

                //  锚点偏移的处理；
                pEntity->SetAnchorOffset(pointAnchorOffset);

                if ((m_nMapVersion >= SUPPORT_EFFECT_SCALE_AND_ANGLE_ADJUST_MAP_VERSION) && (entityType == typeEffect))
                {
                    float fScale = 1;
                    pStream->read((void*)&fScale, sizeof(float));
                    pEntity->setScale(fScale);
                    ulong ulAngle = 0;
                    pStream->read((void*)&ulAngle, sizeof(ulong));
                    pEntity->setAngle(ulAngle);
                }
			}
		}
	}

	return true;
}

bool SceneMgr::_SaveBlock(xs::Stream* pStream, xs::Point ptTileLeftTop, int nTileRow, int nTileCol)
{
	xs::Point ptCurTile = ptTileLeftTop;
	bool bIsEmptyTile = false;
	int nCount = 0;
	Tile* pTile=0;
	xs::Point ptTileList[130];
	for (int row=0; row<nTileRow; row++)
	{
		ptCurTile.x = ptTileLeftTop.x + row;
		ptCurTile.y = ptTileLeftTop.y - row;
		for (int col=0; col<nTileCol; col++)
		{
			pTile = getTilePtr(ptCurTile);
			if (pTile)
			{
				if (pTile->isMostTileValue())
				{
					if (bIsEmptyTile)
					{
						nCount++;
						if (nCount >= 128)
						{
							_SaveMultiEmptyTileInfo(pStream, nCount);
							nCount = 0;
						}
					}
					else
					{
						_SaveMultiTileInfo(pStream, ptTileList, nCount);
						nCount = 1;
					}
					bIsEmptyTile = true;
				}
				else
				{
					if (bIsEmptyTile)
					{
						_SaveMultiEmptyTileInfo(pStream, nCount);
						nCount = 0;
						ptTileList[nCount++] = ptCurTile;
					}
					else
					{
						ptTileList[nCount] = ptCurTile;
						nCount ++;
						if (nCount >= 128)
						{
							_SaveMultiTileInfo(pStream, ptTileList, nCount);
							nCount = 0;
						}
					}
					bIsEmptyTile = false;
				}
			}
			
			ptCurTile.x += !(col&1);
			ptCurTile.y += col&1;
		}
	}
	
	if (bIsEmptyTile)
		_SaveMultiEmptyTileInfo(pStream, nCount);
	else
		_SaveMultiTileInfo(pStream, ptTileList, nCount);
	return true;
}

bool SceneMgr::_SaveMultiEmptyTileInfo(xs::Stream* pStream, int nCount)
{
	if (nCount == 0)
		return true;
	if (nCount > 128)
	{
		return false;
	}

	BYTE bytCount = nCount-1;
	pStream->write((char*)&bytCount, sizeof(bytCount));
	return true;
}

bool SceneMgr::_SaveMultiTileInfo(xs::Stream* pStream, xs::Point* ptTileList, int nCount)
{
	if (nCount == 0)
		return true;
	if (nCount > 128)
	{
		return false;
	}

	BYTE bytCount = nCount-1 + 128;
	pStream->write((char*)&bytCount,sizeof(bytCount));

	for (int i=0; i<nCount; i++)
	{
		Tile* pTile = getTilePtr(ptTileList[i]);
		if (pTile)
			if (!_SaveTileInfo(pStream, pTile))
				return false;
	}
	return true;
}

bool SceneMgr::_SaveTileInfo(xs::Stream* pStream, Tile* pTile)
{
    if ((pStream == NULL) || (pTile == NULL))
    {
        ASSERT(false);
        return false;
    }

    //  如果Tile中只存在一个笼子类型的实体，则不进行处理：该类型实体是用于生成配置文件而放置的（比如怪物生成文件）；
    BYTE nItemCount = pTile->getLayerCount();
    if (nItemCount == 1)
    {
        EntityView* pEntity = pTile->getLayer(0);
        if (pEntity == NULL)
        {
            ASSERT(false);
            return false;
        }
        BYTE nType = (BYTE)pEntity->getEntityType();
        if (nType == typeCage)
        {
            return true;
        }
    }

    //  去除3d实体的占位信息：这个实现可以根据策划需求来进行调整；
	DWORD dwFlags = pTile->getFlag();
	WORD nFlags = (WORD)dwFlags;
	/*
	bool has3D = false;
	for (int i = 0; i < nItemCount; i++)
	{
		EntityView* pEntity = pTile->getLayer(i);
        if (pEntity == NULL)
        {
            ASSERT(false);
            continue;
        }
		if (static_cast<EntityView*>(pEntity)->getFlag() & flag3D)
		{
			has3D = true;
			break;
		}
	}
	if (has3D)
    {
        nFlags &= ~(Tile::tOccupant_Entity | Tile::tIsMapEntity);
    }*/

    //  如果需要把占位块直接存储到地图上
    if (m_bWriteOccupants)
    {
        //  把物件占位块当作地表占位块来处理
        //  其实客户端地表上也保存了一套物件占位数据，只是在加载的时候去掉了
    }

    //  记录Tile的占位标记；
    pStream->write((char*)&nFlags, sizeof(nFlags));

    //  记录实体数目；
	pStream->write((char*)&nItemCount, sizeof(nItemCount));

    //  记录实体信息；
    for (int i = 0; i < nItemCount; i++)
	{
		EntityView* pEntity = pTile->getLayer(i);
        if (pEntity == NULL)
        {
            ASSERT(false);
            continue;
        }
        BYTE nType = (BYTE)pEntity->getEntityType();
        pStream->write((char*)&nType, sizeof(nType));
        uchar resType = (uchar)pEntity->getResType();
        pStream->write(&resType,sizeof(resType));
        if (!pEntity->onSave(pStream))
        {
            return false;
        }
	}

	return true;
}

void SceneMgr::moveTop(BlockInfo* pBlockInfo, int nRows, int nCols, 
				 int nTileRow, int nTileCol, xs::Point& ptNewTileOrigin, int dx, int dy)
{
	int row,col;
	int kx = nCols;
	int ky = nRows+dy;
	int dy_nCols = dy*nCols;
	int row_nCols = (ky-1)*nCols;
	for (row=ky-1; row>=(-dy); row--,row_nCols-=nCols)
	{
		for (col=0; col<nCols; col++)
		{
			copyBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols-dy_nCols+col].ptLeftTop, nTileRow, nTileCol);
		}
	}

	setOriginTile(ptNewTileOrigin);

	row_nCols = 0;
	for (row=0; row<(-dy); row++,row_nCols+=nCols)
	{
		for (col=0; col<nCols; col++)
		{
			loadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
}

void SceneMgr::moveRightTop(BlockInfo* pBlockInfo, int nRows, int nCols, 
				int nTileRow, int nTileCol, xs::Point& ptNewTileOrigin, int dx, int dy)
{
	int row,col;
	int kx = nCols-dx;
	int ky = nRows+dy;
	int dy_nCols = dy*nCols;
	int row_nCols;
	for (col=dx; col<kx; col++)
	{
		row_nCols = -dy_nCols;
		for (row=-dy; row<ky; row++,row_nCols+=nCols)
		{
			copyBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols-dy_nCols+(col-dx)].ptLeftTop, nTileRow, nTileCol);
		}
	}
	
	setOriginTile(ptNewTileOrigin);
	
	for (col=kx; col<nCols; col++)
	{
		row_nCols = 0;
		for (row=0; row<ky; row++,row_nCols+=nCols)
		{
			loadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}

	row_nCols = 0;
	for (row=0; row<(-dy); row++,row_nCols+=nCols)
	{
		for (col=dx; col<kx; col++)
		{
			loadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
}

void SceneMgr::moveRight(BlockInfo* pBlockInfo, int nRows, int nCols, 
				int nTileRow, int nTileCol, xs::Point& ptNewTileOrigin, int dx, int dy)
{
	int row,col;
	int kx = nCols-dx;
	int ky = nRows;
	int row_nCols = 0;
	for (col=dx; col<kx; col++)
	{
		row_nCols = 0;
		for (row=0; row<ky; row++,row_nCols+=nCols)
		{
			copyBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+(col-dx)].ptLeftTop, nTileRow, nTileCol);
		}
	}
	
	setOriginTile(ptNewTileOrigin);
	
	for (col=kx; col<nCols; col++)
	{
		row_nCols = 0;
		for (row=0; row<ky; row++,row_nCols+=nCols)
		{
			loadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
}

void SceneMgr::moveRightDown(BlockInfo* pBlockInfo, int nRows, int nCols,
				int nTileRow, int nTileCol, xs::Point& ptNewTileOrigin, int dx, int dy)
{
	int row,col;
	int kx = nCols - dx;
	int ky = nRows - dy;
	int dy_nCols = dy*nCols;
	int row_nCols;
	for (col=dx; col<kx; col++)
	{
		row_nCols = dy_nCols;
		for (row=dy; row<ky; row++,row_nCols+=nCols)
		{
			copyBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols-dy_nCols+(col-dx)].ptLeftTop, nTileRow, nTileCol);
		}
	}
	
	setOriginTile(ptNewTileOrigin);
	
	for (col=kx; col<nCols; col++)
	{
		row_nCols = dy_nCols;
		for (row=dy; row<nRows; row++,row_nCols+=nCols)
		{
			loadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
	
	row_nCols = ky*nCols;
	for (row=ky; row<nRows; row++,row_nCols+=nCols)
	{
		for (col=dx; col<kx; col++)
		{
			loadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
}
void SceneMgr::moveDown(BlockInfo* pBlockInfo, int nRows, int nCols, 
				int nTileRow, int nTileCol, xs::Point& ptNewTileOrigin, int dx, int dy)
{
	int row,col;
	int kx = nCols;
	int ky = nRows - dy;
	int dy_nCols = dy*nCols;
	int row_nCols = dy_nCols;
	for (row=dy; row<ky; row++,row_nCols+=nCols)
	{
		for (col=0; col<nCols; col++)
		{
			copyBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols-dy_nCols+col].ptLeftTop, nTileRow, nTileCol);
		}
	}

	setOriginTile(ptNewTileOrigin);

	row_nCols = ky*nCols;
	for (row=ky; row<nRows; row++,row_nCols+=nCols)
	{
		for (col=0; col<nCols; col++)
		{
			loadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
}

void SceneMgr::moveLeftDown(BlockInfo* pBlockInfo, int nRows, int nCols, 
				int nTileRow, int nTileCol, xs::Point& ptNewTileOrigin, int dx, int dy)
{
	int row,col;
	int kx = nCols + dx;
	int ky = nRows - dy;
	int dy_nCols = dy*nCols;
	int row_nCols;
	for (col=kx-1; col>=(-dx); col--)
	{
		row_nCols = dy_nCols;
		for (row=dy; row<ky; row++,row_nCols+=nCols)
		{
			copyBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols-dy_nCols+(col+(-dx))].ptLeftTop, nTileRow, nTileCol);
		}
	}
	
	setOriginTile(ptNewTileOrigin);
	
	for (col=0; col<(-dx); col++)
	{
		row_nCols = dy_nCols;
		for (row=dy; row<nRows; row++,row_nCols+=nCols)
		{
			loadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
	
	row_nCols = ky*nCols;
	for (row=ky; row<nRows; row++,row_nCols+=nCols)
	{
		for (col=(-dx); col<kx; col++)
		{
			loadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
}

void SceneMgr::moveLeft(BlockInfo* pBlockInfo, int nRows, int nCols, 
				int nTileRow, int nTileCol, xs::Point& ptNewTileOrigin, int dx, int dy)
{
	int row,col;
	int kx = nCols + dx;
	int ky = nRows;
	int row_nCols = 0;
	for (col=kx-1; col>=(-dx); col--)
	{
		row_nCols = 0;
		for (row=0; row<nRows; row++,row_nCols+=nCols)
		{
			copyBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+(col+(-dx))].ptLeftTop, nTileRow, nTileCol);
		}
	}
	
	setOriginTile(ptNewTileOrigin);
	
	for (col=0; col<(-dx); col++)
	{
		row_nCols = 0;
		for (row=0; row<nRows; row++,row_nCols+=nCols)
		{
			loadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
}
void SceneMgr::moveleftTop(BlockInfo* pBlockInfo, int nRows, int nCols, 
				int nTileRow, int nTileCol, xs::Point& ptNewTileOrigin, int dx, int dy)
{
	int row,col;
	int kx = nCols + dx;
	int ky = nRows + dy;
	int dy_nCols = dy*nCols;
	int row_nCols;
	for (col=kx-1; col>=(-dx); col--)
	{
		row_nCols = -dy_nCols;
		for (row=(-dy); row<ky; row++,row_nCols+=nCols)
		{
			copyBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols-dy_nCols+(col+(-dx))].ptLeftTop, nTileRow, nTileCol);
		}
	}
	
	setOriginTile(ptNewTileOrigin);
	
	for (col=0; col<(-dx); col++)
	{
		row_nCols = 0;
		for (row=0; row<ky; row++,row_nCols+=nCols)
		{
			loadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
	
	row_nCols = 0;
	for (row=0; row<(-dy); row++,row_nCols+=nCols)
	{
		for (col=(-dx); col<kx; col++)
		{
			loadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
}

void SceneMgr::moveAll(BlockInfo* pBlockInfo, int nRows, int nCols, 
				int nTileRow, int nTileCol, xs::Point& ptNewTileOrigin, int dx, int dy)
{
	int row,col;

	setOriginTile(ptNewTileOrigin);
	
	for (col=0; col<nCols; col++)
	{
		int row_nCols = 0;
		for (row=0; row<nRows; row++,row_nCols+=nCols)
		{
			loadBlockInfo(pBlockInfo[row_nCols+col].ptLeftTop, 
				pBlockInfo[row_nCols+col].nIndex, nTileRow, nTileCol);
		}
	}
}