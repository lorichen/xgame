#include "RenderPch.h"
#include "CommonIndexData.h"

namespace rkt
{

	static uint indexMapBuf(uint x,uint y)
	{
		return ((y + 1) / 2) * 9 + (y / 2) * 8 + x;
	}

	template<class V> void  stripify1(V *in,V *out)
	{
		for (int row = 0;row < 8;row++) 
		{
			V *thisrow = &in[indexMapBuf(0,row * 2)];
			V *nextrow = &in[indexMapBuf(0,(row + 1) * 2)];

			if(row > 0) *out++ = thisrow[0];
			for (int col = 0;col < 9;col++) 
			{
				*out++ = thisrow[col];
				*out++ = nextrow[col];
			}
			if (row < 7) *out++ = nextrow[8];
		}
	}
	template<class V> void  stripify2(V *in,V *out)
	{
		for(int row = 0;row < 8;row++) 
		{ 
			V *thisrow = &in[indexMapBuf(0,row * 2)];
			V *nextrow = &in[indexMapBuf(0,row * 2 + 1)];
			V *overrow = &in[indexMapBuf(0,(row + 1) * 2)];

			if (row > 0) *out++ = thisrow[0];// jump end
			for (int col = 0;col < 8;col++) 
			{
				*out++ = thisrow[col];
				*out++ = nextrow[col];
			}
			*out++ = thisrow[8];
			*out++ = overrow[8];
			*out++ = overrow[8];// jump start
			*out++ = thisrow[0];// jump end
			*out++ = thisrow[0];
			for (int col = 0;col < 8;col++) 
			{
				*out++ = overrow[col];
				*out++ = nextrow[col];
			}
			if (row<8) *out++ = overrow[8];
			if (row<7) *out++ = overrow[8];// jump start
		}
	}

	// only for outter 9 * 9,TriangleStrip
	//uint SceneCommonData::m_sIndexData[0].indexData.indexCount = 8 * 18 + 7 * 2;

	// outter 9 * 9 & inner 8 * 8,TriangleStrip
	//uint SceneCommonData::m_sIndexData[1].indexData.indexCount = 16 * 18 + 7 * 2 + 8 * 2;

	// only for outter 9 * 9,TriangleList
	//uint SceneCommonData::m_sIndexData[2].indexData.indexCount = 8 * 8 * 2 * 3;

	// outter 9 * 9 & inner 8 * 8,TriangleList
	//uint SceneCommonData::m_sIndexData[3].indexData.indexCount = 32 * 8 * 3;

/***
	SceneCommonData::SceneIndexData			SceneCommonData::m_sIndexData[6];

	IVertexBuffer*	SceneCommonData::m_spDetailMapTexcoordVB;
	IVertexBuffer*	SceneCommonData::m_spAlphaMapTexcoordVB;
	IVertexBuffer*	SceneCommonData::m_spAlphaMapTexcoordVBWow;

	IShader*			SceneCommonData::m_pFragmentShaders[4];
	IShader*			SceneCommonData::m_pVertexShader;
	IShaderProgram*		SceneCommonData::m_pShaderPrograms[4];
	IRenderSystem*		SceneCommonData::m_pRenderSystem = 0;

	ITexture*		SceneCommonData::m_pDefaultTexture = 0;****/

	/*
	Index					FPS			NumTriangles
	CommonIndexData1		374.625		2082
	CommonIndexData2		424.575		1718
	CommonIndexData3		324.378		4162
	CommonIndexData4		328.032		3382
	*/

	void  SceneCommonData::initialize(IRenderSystem *pRenderSystem)
	{
		m_pRenderSystem = pRenderSystem;
		IBufferManager *pBufferManager = pRenderSystem->getBufferManager();
		/**
			TriangleStrip Simple
		*/
		uint indexCount = 8 * 18 + 7 * 2;
		m_sIndexData[1].indexBuffer = 
			pBufferManager->createIndexBuffer(IT_16BIT,indexCount,BU_STATIC_WRITE_ONLY);
		short* pIndices1 = static_cast<short*>(m_sIndexData[1].indexBuffer->lock(0,0,BL_DISCARD));
		short *defstrip1 = new short[indexCount];
		for (uint i = 0;i < indexCount;i++) defstrip1[i] = i;
		stripify1<short>(defstrip1,pIndices1);
		delete[] defstrip1;
		m_sIndexData[1].indexBuffer->unlock();
		m_sIndexData[1].primitiveType = PT_TRIANGLE_STRIP;

		/**
			TriangleStrip Complex
			*/
		indexCount = 16 * 18 + 7 * 2 + 8 * 2;
		m_sIndexData[0].indexBuffer = pBufferManager->createIndexBuffer(IT_16BIT,indexCount,BU_STATIC_WRITE_ONLY);
		short* pIndices2 = static_cast<short*>(
			m_sIndexData[0].indexBuffer->lock(0,0,BL_DISCARD));
		short *defstrip2 = new short[indexCount];
		for (uint i = 0;i < indexCount;i++) defstrip2[i] = i;
		stripify2<short>(defstrip2,pIndices2);
		delete[] defstrip2;
		m_sIndexData[0].indexBuffer->unlock();
		m_sIndexData[0].primitiveType = PT_TRIANGLE_STRIP;

		/*
		 *	TriangleList Simple
		 */
		indexCount = 8 * 8 * 2 * 3;
		m_sIndexData[5].indexBuffer = pBufferManager->createIndexBuffer(IT_16BIT,indexCount,BU_STATIC_WRITE_ONLY);
		short* pIndices3 = static_cast<short*>(
			m_sIndexData[5].indexBuffer->lock(0,0,BL_DISCARD));
		for(int j = 0;j < 8;j++)
		for(int i = 0;i < 8;i++)
		{
			short base = j * 17 + i;
			*pIndices3++ = base + 18;
			*pIndices3++ = base;
			*pIndices3++ = base + 17;
			*pIndices3++ = base + 1;
			*pIndices3++ = base;
			*pIndices3++ = base + 18;
		}
		m_sIndexData[5].indexBuffer->unlock();
		m_sIndexData[5].primitiveType = PT_TRIANGLES;

		//TriangleStrip Complex°æ±¾µÄTriangleList°æ±¾
		indexCount = 32 * 8 * 3;
		m_sIndexData[4].indexBuffer = pBufferManager->createIndexBuffer(IT_16BIT,indexCount,BU_STATIC_WRITE_ONLY);
		short* pIndices4 = static_cast<short*>(
			m_sIndexData[4].indexBuffer->lock(0,0,BL_DISCARD));
		for(int i = 0;i < 8;i++)
		{
			for(int j = 0;j < 8;j++)
			{
				short base = i * 17 + j;
				*pIndices4++ = base + 9;
				*pIndices4++ = base + 1;
				*pIndices4++ = base;
				*pIndices4++ = base + 17;
				*pIndices4++ = base + 18;
				*pIndices4++ = base + 9;
			}
			for(int j = 0;j < 7;j++)
			{
				short base = i * 17 + j + 9;
				*pIndices4++ = base;
				*pIndices4++ = base + 1;
				*pIndices4++ = base - 8;
				*pIndices4++ = base + 9;
				*pIndices4++ = base + 1;
				*pIndices4++ = base;
			}
			*pIndices4++ = i * 17 + 17;
			*pIndices4++ = i * 17 + 9;
			*pIndices4++ = i * 17;
			*pIndices4++ = i * 17 + 8 + 8;
			*pIndices4++ = i * 17 + 8 + 17;
			*pIndices4++ = i * 17 + 8;
		}
		m_sIndexData[4].indexBuffer->unlock();
		m_sIndexData[4].primitiveType = PT_TRIANGLES;

		indexCount = 8 * 4 * 3;
		m_sIndexData[2].indexBuffer = pBufferManager->createIndexBuffer(IT_16BIT,indexCount,BU_STATIC_WRITE_ONLY);
		short* pIndices5 = static_cast<short*>(
			m_sIndexData[2].indexBuffer->lock(0,0,BL_DISCARD));
		for(int j = 0;j < 4;j++)
		for(int i = 0;i < 4;i++)
		{
			short base = j * 34 + i * 2;
			*pIndices5++ = base + 36;
			*pIndices5++ = base;
			*pIndices5++ = base + 34;
			*pIndices5++ = base + 2;
			*pIndices5++ = base;
			*pIndices5++ = base + 36;
		}
		m_sIndexData[2].indexBuffer->unlock();
		m_sIndexData[2].primitiveType = PT_TRIANGLES;

		indexCount = 4 * 2 * 3;
		m_sIndexData[3].indexBuffer = pBufferManager->createIndexBuffer(IT_16BIT,indexCount,BU_STATIC_WRITE_ONLY);
		short* pIndices6 = static_cast<short*>(
			m_sIndexData[3].indexBuffer->lock(0,0,BL_DISCARD));
		for(int j = 0;j < 2;j++)
		for(int i = 0;i < 2;i++)
		{
			short base = j * 68 + i * 4;
			*pIndices6++ = base + 72;
			*pIndices6++ = base;
			*pIndices6++ = base + 68;
			*pIndices6++ = base + 4;
			*pIndices6++ = base;
			*pIndices6++ = base + 72;
		}
		m_sIndexData[3].indexBuffer->unlock();
		m_sIndexData[3].primitiveType = PT_TRIANGLES;

		indexCount = 8 * 8 * 4 * 3;
		m_sIndexData[6].indexBuffer = pBufferManager->createIndexBuffer(IT_16BIT,indexCount,BU_STATIC_WRITE_ONLY);
		short* pIndices7 = static_cast<short*>(
			m_sIndexData[6].indexBuffer->lock(0,0,BL_DISCARD));
		for(int j = 0;j < 8;j++)
		for(int i = 0;i < 8;i++)
		{
			short base = j * 17 + i;
			*pIndices7++ = base + 0;
			*pIndices7++ = base + 9;
			*pIndices7++ = base + 1;
			*pIndices7++ = base + 1;
			*pIndices7++ = base + 9;
			*pIndices7++ = base + 18;
			*pIndices7++ = base + 18;
			*pIndices7++ = base + 9;
			*pIndices7++ = base + 17;
			*pIndices7++ = base + 17;
			*pIndices7++ = base + 9;
			*pIndices7++ = base + 0;
		}
		m_sIndexData[6].indexBuffer->unlock();
		m_sIndexData[6].primitiveType = PT_TRIANGLES;

		indexCount = 8 * 8 * 2 * 3;
		m_sIndexData[7].indexBuffer = pBufferManager->createIndexBuffer(IT_16BIT,indexCount,BU_STATIC_WRITE_ONLY);
		short* pIndices8 = static_cast<short*>(
			m_sIndexData[7].indexBuffer->lock(0,0,BL_DISCARD));
		for(int j = 0;j < 8;j++)
		for(int i = 0;i < 8;i++)
		{
			short base = j * 17 + i;
			*pIndices8++ = base + 18;
			*pIndices8++ = base;
			*pIndices8++ = base + 17;
			*pIndices8++ = base + 1;
			*pIndices8++ = base;
			*pIndices8++ = base + 18;
		}
		m_sIndexData[7].indexBuffer->unlock();
		m_sIndexData[7].primitiveType = PT_TRIANGLES;

		//DetailMap Texcoord binds
		m_spDetailMapTexcoordVB = pBufferManager->createVertexBuffer(2 * sizeof(float),145,BU_STATIC_WRITE_ONLY);
		float* pTexC = static_cast<float*>(m_spDetailMapTexcoordVB->lock(0,0,BL_DISCARD));
		//fill the VB
#define DETAIL_TILE_NUM		8
		const float fDetailMapHalfSize = 0.5f * DETAIL_TILE_NUM / 8.0f;
		float tx,ty;
		for(int j = 0;j < 17;j++) 
		for(int i = 0;i < ((j % 2) ? 8 : 9);i++)
		{
			tx = DETAIL_TILE_NUM / 8.0f * i;
			ty = DETAIL_TILE_NUM / 8.0f * j * 0.5f;
			if(j % 2) 
			{
				// offset by half
				tx += fDetailMapHalfSize;
			}
			*pTexC++ = tx;
			*pTexC++ = ty;
		}
		m_spDetailMapTexcoordVB->unlock();

		//AlphaMap Texcoord binds
		m_spAlphaMapTexcoordVB = pBufferManager->createVertexBuffer(2 * sizeof(float),145,BU_STATIC_WRITE_ONLY);
		pTexC = static_cast<float*>(m_spAlphaMapTexcoordVB->lock(0,0,BL_DISCARD));

		//fill the VB
		const float fAlphaMapHalfSize = 0.5f * 1.0f / 8.0f;
		for(int j = 0;j < 17;j++) 
		for(int i = 0;i < ((j % 2) ? 8 : 9);i++)
		{
			tx = 1.0f / 8.0f * i;
			ty = 1.0f / 8.0f * j * 0.5f;
			if(j % 2) 
			{
				// offset by half
				tx += fAlphaMapHalfSize;
			}
			*pTexC++ = tx;
			*pTexC++ = ty;
		}
		m_spAlphaMapTexcoordVB->unlock();

		//WowMap AlphaMap Texcoord binds
		m_spAlphaMapTexcoordVBWow = pBufferManager->createVertexBuffer(2 * sizeof(float),145,BU_STATIC_WRITE_ONLY);
		pTexC = static_cast<float*>(m_spAlphaMapTexcoordVBWow->lock(0,0,BL_DISCARD));

		//fill the VB
		const float fAlphaMapHalfSizeWow = 0.5f * 1.0f / 8.0f;
		for(int j = 0;j < 17;j++) 
		for(int i = 0;i < ((j % 2) ? 8 : 9);i++)
		{
			tx = 1.0f / 8.0f * i;
			ty = 1.0f / 8.0f * j * 0.5f;
			if(j % 2) 
			{
				// offset by half
				tx += fAlphaMapHalfSizeWow;
			}
			//*pTexC++ = tx * 0.95f;
			//*pTexC++ = ty * 0.95f;
			const int divs = 32;
			const float inv = 1.0f / divs;
			const float mul = (divs - 1.0f);
			*pTexC++ = tx * (mul * inv);
			*pTexC++ = ty * (mul * inv);
		}
		m_spAlphaMapTexcoordVBWow->unlock();

		IShaderManager *pShaderManager = pRenderSystem->getShaderManager();

		if(m_pRenderSystem->getCapabilities()->hasCapability(RSC_HIGHLEVEL_SHADER))
		{
			m_pVertexShader = pShaderManager->createShaderFromFile(ST_VERTEX_PROGRAM,"terrain.vert");
			for(uint i = 0;i < 4;i++)
			{
				char szFrag[256];
				sprintf_s(szFrag,"terrain%d.frag",i + 1);
				m_pFragmentShaders[i] = pShaderManager->createShaderFromFile(ST_FRAGMENT_PROGRAM,szFrag);
				m_pShaderPrograms[i] = pShaderManager->createShaderProgram();
				if(m_pFragmentShaders[i])m_pShaderPrograms[i]->addShader(m_pFragmentShaders[i]);
				if(m_pVertexShader)m_pShaderPrograms[i]->addShader(m_pVertexShader);
				m_pShaderPrograms[i]->link();
			}
		}
		else if(m_pRenderSystem->getCapabilities()->hasCapability(RSC_LOWLEVEL_SHADER))
		{
			for(uint i = 0;i < 4;i++)
			{
				char szFrag[256];
				sprintf_s(szFrag,"terrain%d.fx",i + 1);
				m_pFragmentShaders[i] = pShaderManager->createShaderFromFile(ST_FRAGMENT_PROGRAM,szFrag);
				m_pShaderPrograms[i] = pShaderManager->createShaderProgram();
				if(m_pFragmentShaders[i])m_pShaderPrograms[i]->addShader(m_pFragmentShaders[i]);
				m_pShaderPrograms[i]->link();
			}
		}

		m_pDefaultTexture = m_pRenderSystem->getTextureManager()->createTextureFromFile("Checkers.Blp",FO_LINEAR,FO_LINEAR,FO_LINEAR,TAM_WRAP,TAM_WRAP);
	}

	void  SceneCommonData::finalize()
	{
		m_pRenderSystem->getTextureManager()->releaseTexture(m_pDefaultTexture);
		m_spDetailMapTexcoordVB->release();
		m_spAlphaMapTexcoordVB->release();
		m_spAlphaMapTexcoordVBWow->release();
		for(uint i = 0;i < sizeof(m_sIndexData) / sizeof(m_sIndexData[0]);i++)
		{
			m_sIndexData[i].indexBuffer->release();
		}
		IShaderManager *pShaderManager = m_pRenderSystem->getShaderManager();
		if(m_pVertexShader)pShaderManager->releaseShader(m_pVertexShader);
		for(uint i = 0;i < 4;i++)
		{
			if(m_pFragmentShaders[i])pShaderManager->releaseShader(m_pFragmentShaders[i]);
			if(m_pShaderPrograms[i])m_pShaderPrograms[i]->release();
		}
	}
	
	_RenderEngineExport SceneCommonData& getSceneCommonData()
	{
		return SceneCommonData::getInstance();
	}
}