#include "stdafx.h"
#include "ResourceGT.h"
#include "GroundTile.h"
#include "IGlobalClient.h"
#include "IEntityClient.h"
#include "SceneManager.h"

extern RunType g_runtype;
size_t ResourceGT::size() const
{
	return m_size;
}

void ResourceGT::release()
{
	//return;
	/*char str[256];
	sprintf(str,"ResourceNode:release:%p\n",this);
	OutputDebugString(str);*/

	releaseResource();
	delete this;
}

void ResourceGT::releaseResource()
{
	safeRelease(m_res.m_pTextures[0]);
	safeRelease(m_res.m_pTextures[1]);
	safeRelease(m_res.m_pTextures[2]);
	safeRelease(m_res.m_pTextures[3]);

	safeRelease(m_res.m_pAlphaMaps[0]);
	safeRelease(m_res.m_pAlphaMaps[1]);
	safeRelease(m_res.m_pAlphaMaps[2]);

	safeRelease(m_res.m_pShaderMap);
}

void ResourceGT::loadTextures(GroundTile *pTile)
{
	uint layerNum = pTile->getTextureLayerNum();
	for(uint i = 0;i < layerNum;i++)
	{
		if(!m_res.m_pTextures[i])
		{
			//应该是TAM_CLAMP_TO_EDGE模式
			m_res.m_pTextures[i] = getRenderEngine()->getRenderSystem()->getTextureManager()->createTextureFromFile(pTile->getTextureFileName(i),FO_POINT,FO_POINT,FO_NONE,TAM_CLAMP_TO_EDGE/*TAM_WRAP*/,TAM_CLAMP_TO_EDGE/*TAM_WRAP*/);
		}
	}
}

void ResourceGT::loadAllTextures(GroundTile *pTile)
{
	//d3d9不支持大头的PF_R8G8B8，需要转换格式，但是却浪费了效率.
	//这里将shaderMap的格式转变成PF_B8G8R8，这样D3D9一定支持，OGL看实现是否支持，
	//将来在底层实现格式的转换。
	loadTextures(pTile);
	uint layerNum = pTile->getTextureLayerNum();
	if(layerNum > 1)
	{
		uchar shaderMap[64 * 64 * 3];
		for(uint i = 1;i < layerNum;i++)
		{
			uchar *pAlphaMap = pTile->getAlphaMap(i - 1);

			//只有不支持shader的时候采用多重纹理混合
			if( !gGlobalClient->getRenderSystem()->getCapabilities()->hasCapability(RSC_HIGHLEVEL_SHADER)
				&& !gGlobalClient->getRenderSystem()->getCapabilities()->hasCapability(RSC_LOWLEVEL_SHADER) )
			{
				m_res.m_pAlphaMaps[i - 1] = getRenderEngine()->getRenderSystem()->getTextureManager()->createTextureFromRawData(pAlphaMap,64,64,PF_A8,FO_POINT,FO_POINT,FO_NONE,TAM_CLAMP_TO_EDGE,TAM_CLAMP_TO_EDGE);
				//这个m_pAlphaMaps在渲染的时候并没有用到,占了一些无用的内存
			}


			//将格式由PF_R8G8B8转换成PF_B8G8R8.所以将数据反转过来填充
			//for(int p = 0;p < 64 * 64;p++)
			//{
			//	shaderMap[p * 3 + i - 1] = pAlphaMap[p];
			//}
			for( int p = 0; p < 64*64; ++p)
			{
				//shaderMap[p * 3 + 2 -( i - 1) ] = pAlphaMap[p];//等价于下面
				shaderMap[p * 3 + 3 - i ] = pAlphaMap[p];
			}
		}
		if(layerNum > 1)
		{
			//将格式由PF_R8G8B8转换成PF_B8G8R8
			m_res.m_pShaderMap = getRenderEngine()->getRenderSystem()->getTextureManager()->
				createTextureFromRawData(shaderMap,64, 64,PF_B8G8R8 /*PF_R8G8B8*/,FO_POINT,FO_POINT,FO_NONE,TAM_CLAMP_TO_EDGE,TAM_CLAMP_TO_EDGE);
		}
	}
}

bool ResourceGT::load(const ulong uResID)
{	
	CSceneLock sceneLock;	
	int nGridY = uResID & 0x7FF;
	int nGridX = (uResID >> 11) & 0x7FF;
	int nMapID = (uResID >> 22);

	if (g_runtype == RUN_TYPE_GAME)
	{
		IEntityClient* pEntityClient = gGlobalClient->getEntityClient();
		if (NULL != pEntityClient)
		{
			int nCurMapID = pEntityClient->GetMapID();
			if (nMapID != nCurMapID)
			{
				return false;
			}
		}		
	}
	
	ISceneManager2* pSceneManger = gGlobalClient->getSceneManager();
	if (NULL == pSceneManger)
	{
		return false;
	}
	IGround* pGround = pSceneManger->getGround();
	if (NULL == pGround)
	{
		return false;
	}

	GroundTile *pTile = static_cast<GroundTile*>(pGround->getGroundTile(nGridX,nGridY));
	if(NULL == pTile)
	{
		return false;
	}
	loadAllTextures(pTile);

	//计算占用内存大小
	m_size = 0;
	for(int i = 0;i < 4;i++)
	{
		if(m_res.m_pTextures[i])
		{
			m_size += m_res.m_pTextures[i]->getMemorySize();
		}
	}
	for(int i = 0;i < 3;i++)
	{
		if(m_res.m_pAlphaMaps[i])
		{
			m_size += m_res.m_pAlphaMaps[i]->getMemorySize();
		}
	}
	if(m_res.m_pShaderMap)
	{
		m_size += m_res.m_pShaderMap->getMemorySize();
	}

	return true;
}

void* ResourceGT::getInnerData()
{
	return (void*)&m_res;
}