#include "stdafx.h"
#include "GroundEyeshot.h"
#include "lzss.h"

GroundTile::GroundTile() : m_textureLayerNum(0),m_bLoaded(false)
{
	m_pRenderSystem = getRenderEngine()->getRenderSystem();

	m_alphaMap[0] = m_alphaMap[1] = m_alphaMap[2] = 0;
	m_shadowMap = 0;
	m_handle = 0;
	m_bAlphaMapLoaded = false;
	m_handle = 0;
	m_basicInfoLoad = false;
}

GroundTile::~GroundTile()
{
	safeDeleteArray(m_alphaMap[0]);
	safeDeleteArray(m_alphaMap[1]);
	safeDeleteArray(m_alphaMap[2])

	safeDeleteArray(m_shadowMap);
}

void GroundTile::save(xs::Stream *pDataStream,HASH_MAP_NAMESPACE::hash_map<std::string,int>& mTextures)
{
	uint layerNum = getTextureLayerNum();
	pDataStream->write(&layerNum,sizeof(layerNum));
	for(uint k = 0;k < layerNum;k++)
	{

		std::string textureFileName = getTextureFileName(k);
		HASH_MAP_NAMESPACE::hash_map<std::string,int>::iterator it = mTextures.find(textureFileName);
		//
		int index = (*it).second;
		pDataStream->write(&index,sizeof(index));
	}
	for(uint k = 1;k < layerNum;k++)
	{
#if 0
		uchar *pAlphaMap = getAlphaMap(k - 1);
		uchar alphaMap[64 * 32],alphaMapCompressed[64 * 32];
		for(uint l = 0;l < 64 * 32;l++)
		{
			uchar a1 = pAlphaMap[l * 2 + 0];
			uchar a2 = pAlphaMap[l * 2 + 1];
			uchar a = (a1 >> 4) | (a2 & 0xF0);
			alphaMap[l] = a;
		}
		int size;
		Encode(alphaMap,64 * 32,alphaMapCompressed,&size);
#else
		uchar *pAlphaMap = getAlphaMap(k - 1);
		uchar alphaMapCompressed[128 * 128];
		int size;
		Encode(pAlphaMap,64 * 64,alphaMapCompressed,&size);
#endif
		pDataStream->write(&size,sizeof(size));
		pDataStream->write(alphaMapCompressed,size);

		/*char str[256];
		sprintf(str,"O=%d,F=%d\n",64 * 32,size);
		OutputDebugString(str);*/
	}
}

void GroundTile::loadBasicInfo(xs::Stream *pMapStream,std::vector<std::string>& vTextures)
{
	if(m_basicInfoLoad)return;

	uint layerNum;
	pMapStream->read(&layerNum,sizeof(layerNum));
	setTextureLayerNum(layerNum);
	for(uint i = 0;i < layerNum;i++)
	{
		int index;
		pMapStream->read(&index,sizeof(index));
		setTextureFileName(i,vTextures[index].c_str(),false);
	}

	m_basicInfoLoad = true;
}

void GroundTile::load(xs::Stream *pMapStream)
{
	if(m_bLoaded)return;

	uint layerNum;
	pMapStream->read(&layerNum,sizeof(layerNum));
	for(uint i = 0;i < layerNum;i++)
	{
		int index;
		pMapStream->read(&index,sizeof(index));
	}

	for(uint i = 1;i < layerNum;i++)
	{
#if 0
		uchar alphaMap[64 * 32],alphaMapCompressed[64 * 32];
		int size;
		pMapStream->read(&size,sizeof(size));
		pMapStream->read(alphaMapCompressed,size);
		int tsize;
		Decode(alphaMapCompressed,size,alphaMap,&tsize);
		uchar *pAlphaMap = getAlphaMap(i - 1);
		for(uint j = 0;j < 64 * 32;j++)
		{
			uchar a = alphaMap[j];
			pAlphaMap[2 * j + 0] = (a & 0x0f) << 4;
			pAlphaMap[2 * j + 1] = (a & 0xf0);
		}
		if(bLoad2GPU)updateAlphaMap(i - 1);
#else
		uchar alphaMapCompressed[128 * 128];
		int size;
		pMapStream->read(&size,sizeof(size));
		pMapStream->read(alphaMapCompressed,size);
		int tsize;
		uchar *pAlphaMap = getAlphaMap(i - 1);
		Decode(alphaMapCompressed,size,pAlphaMap,&tsize);
#endif
	}

	m_bLoaded = true;
}

ITexture*	GroundTile::getShaderMapTexture()
{
	GroundTileResource *pResource = getResource();
	if(!pResource)return 0;

	return pResource->m_pShaderMap;
}

GroundTileResource *GroundTile::getResource()
{
	if(!m_handle || !isValidHandle(m_handle))return 0;

	IResourceNode *pNode = (IResourceNode*)getHandleData(m_handle);
	if(pNode)
	{
		IResource *pResource = pNode->getResource();
		if(pResource)
		{
			return (GroundTileResource*)pResource->getInnerData();
		}
	}

	return 0;
}

ITexture*	GroundTile::getAlphaMapTexture(uint index)
{
	GroundTileResource *pResource = getResource();
	if(!pResource)return 0;

	return pResource->m_pAlphaMaps[index];
}

ITexture*	GroundTile::getTexture(uint index)
{
	GroundTileResource *pResource = getResource();
	if(!pResource)return 0;

	return pResource->m_pTextures[index];
}

const char*		GroundTile::getTextureFileName(uint index)
{
	return m_textureLayerFileName[index].c_str();
}

void		GroundTile::setTextureFileName(uint index,const char* szFileName,bool bLoad2GPU)
{
	GroundTileResource *pResource = getResource();

	if(m_textureLayerFileName[index] != szFileName)
	{
		if(!m_textureLayerFileName[index].empty())
		{
			if(pResource)safeRelease(pResource->m_pTextures[index]);
		}

		m_textureLayerFileName[index] = szFileName;
		if(bLoad2GPU)
		{
			if(pResource)pResource->m_pTextures[index] = m_pRenderSystem->getTextureManager()->createTextureFromFile(szFileName,FO_POINT,FO_POINT,FO_NONE,TAM_WRAP,TAM_WRAP);
		}
	}
}

uchar*		GroundTile::getAlphaMap(uint index)
{
	if(!m_alphaMap[index])
	{
		m_alphaMap[index] = new uchar[64 * 64];
		memset(m_alphaMap[index],0,64 * 64);
	}

	return m_alphaMap[index];
}

void GroundTile::_updateAlphaMap(uint index,bool updateShaderMap)
{
	//d3d9不支持大头的PF_R8G8B8，需要转换格式，但是却浪费了效率.
	//这里将shaderMap的格式转变成PF_B8G8R8，这样D3D9一定支持，OGL看实现是否支持，
	//将来在底层实现格式的转换。
	GroundTileResource *pResource = getResource();
	if(!pResource)return;

	if(!pResource->m_pAlphaMaps[index])
	{
		pResource->m_pAlphaMaps[index] = m_pRenderSystem->getTextureManager()->createTextureFromRawData(m_alphaMap[index],64,64,PF_A8,FO_POINT,FO_POINT,FO_NONE,TAM_CLAMP_TO_EDGE,TAM_CLAMP_TO_EDGE);
	}
	else
	{
		pResource->m_pAlphaMaps[index]->setSubData(0,0,0,64,64,m_alphaMap[index]);
	}
	if(updateShaderMap)
	{
		uchar shaderMap[64 * 64 * 3];
		for(uint i = 1;i < m_textureLayerNum;i++)
		{
			//将格式由PF_R8G8B8转换成PF_B8G8R8.所以将数据反转过来填充
			for(int p = 0;p < 64 * 64;p++)
			{
				shaderMap[p * 3 + i - 1] = m_alphaMap[i - 1][p];
			}
			
			/*
			for( int p = 0; p < 64*64; ++p)
			{
				shaderMap[p * 3 + 2 -( i - 1) ] = m_alphaMap[i-1][p];//等价于下面
				//shaderMap[p * 3 + 3 - i ] = m_alphaMap[i-1][p];
			}
			*/
			
		}
		if(m_textureLayerNum > 1)
		{
			if(!pResource->m_pShaderMap)
			{
				//将格式由PF_R8G8B8转换成PF_B8G8R8
				pResource->m_pShaderMap = m_pRenderSystem->getTextureManager()->createTextureFromRawData(
					shaderMap,64,64,PF_R8G8B8/*PF_B8G8R8*/,FO_POINT,FO_POINT,FO_NONE,TAM_CLAMP_TO_EDGE,TAM_CLAMP_TO_EDGE);
			}
			else
				pResource->m_pShaderMap->setSubData(0,0,0,64,64,shaderMap);
		}
	}
}

void		GroundTile::updateAlphaMap(uint index)
{
	_updateAlphaMap(index,true);
}

uchar*		GroundTile::getShadowMap()
{
	if(!m_shadowMap)
	{
		m_shadowMap = new uchar[64 * 64];
		memset(m_shadowMap,0,64 * 64);
	}

	return m_shadowMap;
}

void		GroundTile::updateShadowMap()
{
}

uint		GroundTile::getTextureLayerNum()
{
	return m_textureLayerNum;
}

const Matrix4& GroundTile::getWorldMatrix()
{
	xs::Point pt;
	xs::Point ptViewLt = m_pGround->getViewLeftTop();
	pt.x = m_ptCoord.x * 256;
	pt.y = m_ptCoord.y * 256;

	m_mtxWorld.makeTrans(pt.x - ptViewLt.x,pt.y - ptViewLt.y,0);

	return m_mtxWorld;
}

xs::Rect GroundTile::getRect()
{
	xs::Rect rectTile;
	xs::Point ptViewLt = m_pGround->getViewLeftTop();
	int nGroundTileWidth = m_pGround->getGroundTileWidth();
	int nGroundTileHeight = m_pGround->getGroundTileHeight();
	rectTile.left = m_ptCoord.x * nGroundTileWidth;
	rectTile.top = m_ptCoord.y * nGroundTileHeight;
	rectTile.right = rectTile.left + nGroundTileWidth;
	rectTile.bottom = rectTile.top + nGroundTileHeight;
	//OffsetRect(&rectTile,ptViewLt.x,ptViewLt.y);

	return rectTile;
}

int GroundTile::getX()
{
	return m_ptCoord.x;
}

int GroundTile::getY()
{
	return m_ptCoord.y;
}

void GroundTile::setTextureLayerNum(uint num)
{
	GroundTileResource *pResource = getResource();

	if(m_textureLayerNum == num)return;
	else if(m_textureLayerNum < num)m_textureLayerNum = num;
	else
	{
		for(uint i = num;i < m_textureLayerNum;i++)
		{
			if(pResource)safeRelease(pResource->m_pTextures[i]);
			m_textureLayerFileName[i] = "";
			if(num > 0)
			{
				if(pResource)safeRelease(pResource->m_pAlphaMaps[i - 1]);
				safeDeleteArray(m_alphaMap[i - 1]);
			}
		}
		if(pResource)safeRelease(pResource->m_pShaderMap);
		m_textureLayerNum = num;
	}
}

void GroundTile::loadAlphaMaps(xs::Stream* pMapStream)
{
	if(m_bAlphaMapLoaded)return;

	uint layerNum;
	pMapStream->read(&layerNum,sizeof(layerNum));
	for(uint i = 0;i < layerNum;i++)
	{
		int index;
		pMapStream->read(&index,sizeof(index));
	}

	for(uint i = 1;i < layerNum;i++)
	{
		uchar alphaMapCompressed[128 * 128];
		int size;
		pMapStream->read(&size,sizeof(size));
		pMapStream->read(alphaMapCompressed,size);
		int tsize;
		uchar *pAlphaMap = getAlphaMap(i - 1);
		Decode(alphaMapCompressed,size,pAlphaMap,&tsize);
	}

	m_bAlphaMapLoaded = true;
}

bool GroundTile::isLoaded()
{
	return m_handle && isValidHandle(m_handle) && getHandleData(m_handle) != 0;
}
