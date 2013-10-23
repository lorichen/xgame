#ifndef __Material_H__
#define __Material_H__

#include <string>
#include"KeyFrames.h"

namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	/*
	2009/07/24	xiexuehui	added key frames to materiallayer of uv offsets to class MaterialLayer to realise uv animation
	2009/07/27 xiexuehui added key frames to material for alpha animation
	*/
	class _RenderEngineExport MaterialLayer
	{
	public:
		//名字
		std::string		m_szTexture;
		//混合模式
		BlendMode		m_blendMode;
		//两面
		bool			m_bTwoSide;
		//漫反射
		ColorValue		m_diffuse;

		bool			m_bUnshaded;
		bool			m_bUnfogged;
		bool			m_SphereEnvironmentMap;
		bool			m_bNoDepthTest;
		bool			m_bNoDepthSet;

		bool			m_bClampS;
		bool			m_bClampT;
		KeyFrames<float> m_kfUOffs;// u offset
		KeyFrames<float> m_kfVOffs;// v offset
		bool			m_bEnhanceValue;//增强亮度,这个现在被取消了


public:
		ITexture*		m_pTexture;

	public:
		MaterialLayer()
		{
			m_blendMode = BM_OPAQUE;
			
			m_bTwoSide = true;

			m_diffuse = ColorValue(1.0f,1.0f,1.0f,1.0f);
			m_fullDiffuse = m_diffuse;
			m_bUnshaded = false;
			m_bUnfogged = false;
			m_SphereEnvironmentMap = false;
			m_bNoDepthTest = false;
			m_bNoDepthSet = false;

			m_bClampS = false;
			m_bClampT = false;

			m_pTexture = 0;
			m_bEnhanceValue = false;
		}

	public:
		int getRenderOrder();

		bool beginPass(IRenderSystem *pRenderSystem);
		void endPass(IRenderSystem *pRenderSystem);

	private:
		//渲染状态备份/恢复信息
		SceneBlendFactor	m_sbfSrc,m_sbfDst;
		bool m_alphaCheck;
		float m_alphaCheckRef;
		CullingMode	m_cullingMode;
		CompareFunction	m_alphaFunc;
		ColorValue m_surfaceDiffuse;
		bool		m_bDepthCheck;
		bool		m_bDepthWrite;
		bool		m_bLightingEnabled;
		bool		m_bFogEnabled;
		CompareFunction	m_depthFunction;

	public:
		ColorValue m_fullDiffuse;

	private:
		void saveContext(IRenderSystem *pRenderSystem);
		void loadContext(IRenderSystem *pRenderSystem);
	};

	class _RenderEngineExport Material
	{
	public:
		size_t getMemorySize();
		int	getRenderOrder();
		int numLayers();
		MaterialLayer *getLayer(int layer);
		void addLayer(const MaterialLayer& layer);

		void setName(const std::string& name){m_name = name;}
		const std::string& getName(){return m_name;}

		void initialize();
		void finalize();

	private:
		std::vector<MaterialLayer>	m_vLayers;
		std::string		m_name;
	public:
		KeyFrames<float> m_kfTrans;

	};

	/** @} */
}

#endif