#ifndef __GroundRenderQueue_H__
#define __GroundRenderQueue_H__

class GroundTile;

//处理地表的渲染
//


class GroundRenderQueue
{
public:
	GroundRenderQueue()
	{
		for(int i = 0;i < 4;i++)
		{
			m_pShaderPrograms[i] = 0;
		}

		m_pVB = 0;
		m_pVBTexcoord = 0;
		m_pFont = 0;
		m_pDistantVB = 0;
		m_pDistantTexVB = 0;
		m_pTileLineVB = 0;
		m_pTileLineColorVB1 = 0;
		m_pTileLineColorVB2 = 0;

		m_vTiles.reserve(256);
	}

public:
	void reset();
	void addTile(GroundTile*	pTile);
	void render();
    bool RenderDistance(int nMapWidth, int nMapHeight, int nViewWidth, int nViewHeight, int nViewTopX, int nViewTopY,
        ITexture* pDistanceTexture);
    void renderTileLine(int nViewTopX, int nViewTopY);
	void initialize();
	void finalize();

private:
	IShaderProgram*		m_pShaderPrograms[4];
	
	IVertexBuffer*		m_pVB;
	IVertexBuffer*		m_pVBTexcoord;
	IVertexBuffer*		m_pDistantVB;//远景贴图用
	IVertexBuffer*		m_pDistantTexVB;//远景贴图用
	IVertexBuffer*		m_pTileLineVB;//绘制Tile边框 
	IVertexBuffer*		m_pTileLineColorVB1;//绘制Tile边框
	IVertexBuffer*		m_pTileLineColorVB2;//绘制Tile边框
	xs::IFont*			m_pFont;

	typedef std::vector<GroundTile*> TileList;
	TileList	m_vTiles;
};

#endif