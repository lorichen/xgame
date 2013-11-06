#ifndef __GroundTile_H__
#define __GroundTile_H__

#include "ISceneManager2.h"

class GroundEyeshot;

struct GroundTileResource
{
	GroundTileResource()
	{
		m_pTextures[0] = 0;
		m_pTextures[1] = 0;
		m_pTextures[2] = 0;
		m_pTextures[3] = 0;
		m_pAlphaMaps[0] = 0;
		m_pAlphaMaps[1] = 0;
		m_pAlphaMaps[2] = 0;
		m_pShaderMap = 0;
	}

	ITexture*	m_pTextures[4];
	ITexture*	m_pAlphaMaps[3];
	ITexture*	m_pShaderMap;
};

class GroundTile : public IGroundTile
{
	friend class GroundRenderQueue;
public:
	/**获得第index层贴图
	@param index 层(0-3)
	@return 贴图指针
	*/
	virtual ITexture*	getTexture(uint index);

	/**获得第index层贴图的文件名
	@param index 层
	@return 贴图文件名
	*/
	virtual const char*		getTextureFileName(uint index);

	/**设置第index层的贴图文件名
	@param index 层
	@param szFileName 贴图文件名
	@param bLoad2GPU 是否立即加载贴图
	*/
	virtual void		setTextureFileName(uint index,const char* szFileName,bool bLoad2GPU);

	/**获得第index层Alpha贴图数据
	@param index 层(0-2)
	@return 贴图数据
	*/
	virtual	uchar*		getAlphaMap(uint index);

	/**更新Alpha贴图
	@param index 层
	*/
	virtual void		updateAlphaMap(uint index);

	/**获得阴影贴图的数据
	@return 贴图数据
	*/
	virtual uchar*		getShadowMap();

	/**更新阴影贴图数据
	*/
	virtual void		updateShadowMap();

	/**获得贴图的层数
	@return 贴图层数
	*/
	virtual uint		getTextureLayerNum();

	/**设置贴图的层数
	@param num 层数(0-4)
	*/
	virtual void		setTextureLayerNum(uint num);

	/**取得包围框
	@return 包围框
	*/
	virtual RECT		getRect(); // 获取地表块相对地图的世界坐标

	/**取得地表的X下标
	@param x X下标
	*/
	virtual int			getX(); // 获取地表块对应的宫格的列值

	/**取得地表的Y下标
	@param y Y下标
	*/
	virtual int			getY();	// 获取地表块对应的宫格的行值

public:
	GroundTile();
	~GroundTile();

	const Matrix4& getWorldMatrix();
	void loadAlphaMaps(xs::Stream* pStream);
	void setResHandle(handle h)
	{
		m_handle = h;
		if(isValidHandle(h))
		{
			//Info("ooo handle = "<<(ulong)m_handle<<endl);
		}
		else
		{
			//Info("xxx handle = "<<(ulong)m_handle<<endl);
		}
	}
	handle getResHandle(){return m_handle;}
	GroundTileResource *getResource();
	ITexture*	getShaderMapTexture();
	ITexture*	getAlphaMapTexture(uint index);

private:
	uint		m_textureLayerNum;
	std::string	m_textureLayerFileName[4];
	uchar*		m_alphaMap[3];
	uchar*		m_shadowMap;
	bool		m_bLoaded;
	Matrix4		m_mtxWorld;
	handle		m_handle;
	bool		m_bAlphaMapLoaded;
	bool		m_basicInfoLoad;

private:
	IRenderSystem*	m_pRenderSystem;

public:
	GroundEyeshot*	m_pGround;
	POINT		m_ptCoord;	//地表块坐标，每个块是512*512

private:
	void _draw(const RECT& rect);
	void _updateAlphaMap(uint index,bool updateShaderMap = true);

public:
	bool isLoaded();
	void setLoaded(bool bLoaded){m_bLoaded = bLoaded;}
	void loadBasicInfo(xs::Stream *pMapStream,std::vector<std::string>& vTextures);
	void load(xs::Stream *pMapStream);
	void save(xs::Stream *pDataStream,stdext::hash_map<std::string,int>& mTextures);
};

#endif