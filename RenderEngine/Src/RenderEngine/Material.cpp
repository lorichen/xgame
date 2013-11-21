#include "StdAfx.h"
#include "Material.h"

namespace xs
{
	size_t Material::getMemorySize()
	{
		return sizeof(*this) + m_vLayers.size() * sizeof(MaterialLayer);
	}

	bool MaterialLayer::beginPass(IRenderSystem *pRenderSystem)
	{
		PP_BY_NAME("MaterialLayer::beginPass");
		if(!m_pTexture)return false;

		PP_BY_NAME_START("MaterialLayer::beginPass::saveContext");
		saveContext(pRenderSystem);
		PP_BY_NAME_STOP();

		pRenderSystem->setTexture(0,m_pTexture);

		if(m_bClampS)
		{
			pRenderSystem->setTextureWrapS(0,TAM_CLAMP_TO_EDGE);
		}
		else
		{
			pRenderSystem->setTextureWrapS(0,TAM_WRAP);
		}
		if(m_bClampT)
		{
			pRenderSystem->setTextureWrapT(0,TAM_CLAMP_TO_EDGE);
		}
		else
		{
			pRenderSystem->setTextureWrapT(0,TAM_WRAP);
		}
		if(m_bTwoSide)
		{
			pRenderSystem->setCullingMode(CULL_NONE);
		}
		
		pRenderSystem->setSurfaceDiffuse(m_fullDiffuse);

		if(m_bUnshaded)
		{
			pRenderSystem->setLightingEnabled(false);
		}
		else 
		{
			pRenderSystem->setLightingEnabled(true);
		}

		if(m_bUnfogged)
		{
			pRenderSystem->enableFog(false);
		}

		if(m_SphereEnvironmentMap)
		{
			pRenderSystem->setTextureCoordCalculation(0,TEXCALC_ENVIRONMENT_MAP);
		}

		BlendMode bm = m_blendMode;
		if((bm == BM_OPAQUE || bm == BM_TRANSPARENT || bm == BM_MODULATE2X || bm == BM_TRANSPARENT) && m_fullDiffuse.a < 1.0f)
			bm = BM_ALPHA_BLEND;

		switch(bm)
		{
		case BM_OPAQUE:
			{
				pRenderSystem->setSceneBlending(SBF_ONE,SBF_ZERO);
				pRenderSystem->setAlphaCheckEnabled(false);
				pRenderSystem->setDepthBufferCheckEnabled(true);
				pRenderSystem->setDepthBufferWriteEnabled(true);

				break;
			}

		case BM_TRANSPARENT:
			{
				pRenderSystem->setSceneBlending(SBF_ONE,SBF_ZERO);
				pRenderSystem->setAlphaCheckEnabled(true);
				pRenderSystem->setAlphaFunction(CMPF_GREATER_EQUAL,0.7f);
				pRenderSystem->setDepthBufferCheckEnabled(true);
				pRenderSystem->setDepthBufferWriteEnabled(true);

				break;
			}

		case BM_ALPHA_BLEND:
			{
				pRenderSystem->setDepthBufferFunction(CMPF_LESS_EQUAL);
				pRenderSystem->setAlphaCheckEnabled(false);
				pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);
				pRenderSystem->setDepthBufferCheckEnabled(true);
				if(m_fullDiffuse.a < 1.0f)
					pRenderSystem->setDepthBufferWriteEnabled(true);
				else
					pRenderSystem->setDepthBufferWriteEnabled(false);

				break;
			}

		case BM_ADDITIVE:
			{
				pRenderSystem->setSceneBlending(SBF_SOURCE_COLOR,SBF_ONE);
				pRenderSystem->setAlphaCheckEnabled(false);
				pRenderSystem->setDepthBufferCheckEnabled(true);
				pRenderSystem->setDepthBufferWriteEnabled(false);

				break;
			}

		case BM_ADDITIVE_ALPHA:
			{
				pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE);
				pRenderSystem->setAlphaCheckEnabled(false);
				pRenderSystem->setDepthBufferCheckEnabled(true);
				pRenderSystem->setDepthBufferWriteEnabled(false);

				break;
			}

		case BM_MODULATE:
			{
				pRenderSystem->setSceneBlending(SBF_DEST_COLOR,SBF_SOURCE_COLOR);
				pRenderSystem->setAlphaCheckEnabled(false);
				pRenderSystem->setDepthBufferCheckEnabled(true);
				pRenderSystem->setDepthBufferWriteEnabled(false);

				break;
			}
		case BM_MODULATE2X:
			{
				pRenderSystem->setSceneBlending(SBF_DEST_COLOR,SBF_SOURCE_COLOR);
				pRenderSystem->setAlphaCheckEnabled(false);
				pRenderSystem->setDepthBufferCheckEnabled(true);
				pRenderSystem->setDepthBufferWriteEnabled(false);

				break;
			}
		case BM_OPAQUE_NOWRITEDEPTH:
			{
				pRenderSystem->setSceneBlending(SBF_ONE,SBF_ZERO);
				pRenderSystem->setAlphaCheckEnabled(false);
				pRenderSystem->setDepthBufferCheckEnabled(true);
				pRenderSystem->setDepthBufferWriteEnabled(false);

				break;	
			}
		}

		if(m_bNoDepthTest)
		{
			pRenderSystem->setDepthBufferCheckEnabled(false);
		}

		if(m_bNoDepthSet)
		{
			pRenderSystem->setDepthBufferWriteEnabled(false);
		}

		return true;
	}

	void MaterialLayer::endPass(IRenderSystem *pRenderSystem)
	{
		PP_BY_NAME("MaterialLayer::endPass");
		PP_BY_NAME_START("MaterialLayer::endPass::loadContext");
		loadContext(pRenderSystem);
		PP_BY_NAME_STOP();
	}

	void MaterialLayer::saveContext(IRenderSystem *pRenderSystem)
	{
		PP_BY_NAME("MaterialLayer::saveContext");
		pRenderSystem->getSceneBlending(m_sbfSrc,m_sbfDst);
		m_alphaCheck = pRenderSystem->alphaCheckEnabled();
		pRenderSystem->getAlphaFunction(m_alphaFunc,m_alphaCheckRef);
		m_cullingMode = pRenderSystem->getCullingMode();
		m_surfaceDiffuse = pRenderSystem->getSurfaceDiffuse();
		m_bDepthCheck = pRenderSystem->isDepthBufferCheckEnabled();
		m_bDepthWrite = pRenderSystem->isDepthBufferWriteEnabled();
		m_bLightingEnabled = pRenderSystem->isLightingEnabled();
		m_bFogEnabled = pRenderSystem->isFogEnabled();
		m_depthFunction = pRenderSystem->getDepthBufferFunction();
	}

	void MaterialLayer::loadContext(IRenderSystem *pRenderSystem)
	{
		PP_BY_NAME("MaterialLayer::loadContext");
		pRenderSystem->setTextureWrapS(0,TAM_WRAP);
		pRenderSystem->setTextureWrapT(0,TAM_WRAP);

		pRenderSystem->setTexture(0,0);
		pRenderSystem->setTextureCoordCalculation(0,TEXCALC_NONE);
		pRenderSystem->setSceneBlending(m_sbfSrc,m_sbfDst);
		pRenderSystem->setAlphaCheckEnabled(m_alphaCheck);
		pRenderSystem->setAlphaFunction(m_alphaFunc,m_alphaCheckRef);
		pRenderSystem->setCullingMode(m_cullingMode);
		pRenderSystem->setSurfaceDiffuse(m_surfaceDiffuse);
		pRenderSystem->setDepthBufferCheckEnabled(m_bDepthCheck);
		pRenderSystem->setDepthBufferWriteEnabled(m_bDepthWrite);
		pRenderSystem->setLightingEnabled(m_bLightingEnabled);
		pRenderSystem->enableFog(m_bFogEnabled);
		pRenderSystem->setDepthBufferFunction(m_depthFunction);
	}

	int MaterialLayer::getRenderOrder()
	{
		switch(m_blendMode)
		{
		case BM_OPAQUE:
			{
				if(m_fullDiffuse.a < 1.0f)
					return RENDER_QUEUE_BLEND;
				return RENDER_QUEUE_OPAQUE;
			}

		case BM_TRANSPARENT:
			{
				return RENDER_QUEUE_TRANSPARENT;
			}

		case BM_ALPHA_BLEND:
			{
				return RENDER_QUEUE_BLEND;
			}

		case BM_ADDITIVE:
		case BM_ADDITIVE_ALPHA:
		case BM_MODULATE:
			{
				return RENDER_QUEUE_FX;
			}
		case BM_MODULATE2X:
			{
				return RENDER_QUEUE_TRANSPARENT;
			}
		}

		return RENDER_QUEUE_OPAQUE;
	}

	int	Material::getRenderOrder()
	{
		int currentRenderOrder = 0;

		size_t size = m_vLayers.size();
		for(size_t i = 0; i < size; i++)
		{
			MaterialLayer *pLayer = &m_vLayers[i];
			int renderOrder = pLayer->getRenderOrder();
			if(renderOrder > currentRenderOrder)
				currentRenderOrder = renderOrder;
		}

		return currentRenderOrder;
	}

	int Material::numLayers()
	{
		return m_vLayers.size();
	}

	MaterialLayer* Material::getLayer(int layer)
	{
		if(layer < 0 || layer >= (int)m_vLayers.size())return 0;

		return &m_vLayers[layer];
	}

	void Material::addLayer(const MaterialLayer& layer)
	{
		m_vLayers.push_back(layer);
	}

	void Material::initialize()
	{
		ITextureManager *pTextureManager = getRenderEngine()->getRenderSystem()->getTextureManager();

		size_t size = m_vLayers.size();
		for(size_t i = 0;i < size;i++)
		{
			MaterialLayer& layer = m_vLayers[i];
			if(!layer.m_szTexture.empty())
			{
				layer.m_pTexture = pTextureManager->createTextureFromFile(layer.m_szTexture,FO_LINEAR,FO_LINEAR,FO_LINEAR);
			}
		}
	}

	void Material::finalize()
	{
		size_t size = m_vLayers.size();
		for(size_t i = 0;i < size;i++)
		{
			MaterialLayer& layer = m_vLayers[i];
			safeRelease(layer.m_pTexture);
		}
	}
}