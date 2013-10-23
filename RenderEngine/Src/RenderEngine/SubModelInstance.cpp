#include "StdAfx.h"
#include "SubModelInstance.h"
#include "ModelInstance.h"
#include "ModelNode.h"

namespace xs
{
	SubModelInstance::~SubModelInstance()
	{
		m_mtl.finalize();
	}

	SubModelInstance::SubModelInstance(ModelInstance *pMI,ISubModel *pSubModel,uint subModelIndex) : m_pMI(pMI),
		m_pSubModel(pSubModel),
		m_bVisible(true),
		m_uiSubModelIndex(subModelIndex)
	{
		m_name = pSubModel->getName();
		indexStart = pSubModel->getIndexStart();
		indexCount = pSubModel->getIndexCount();
		vertexStart = pSubModel->getVertexStart();
		vertexEnd = pSubModel->getVertexEnd();
		m_mtlOrigin = pSubModel->getMaterial();

		m_pGeosetAnimation = pSubModel->getGeosetAnimation();
		m_GeosetAlpha = 1.0f;

		if( pMI->getBlendShaderProgram() ) //如果能够硬件骨骼混合
		{
			//for( uint i = 0; i < VERTEX_BLEND_MATRICES_NUM; ++i) 
			//	m_BindedBonesMatrices[i] = xs::Matrix4::IDENTITY;//初始化很重要
			for( uint i = 0; i <VERTEX_BLEND_MATRICES_NUM; ++i)
			{
				for( uint j = 0; j<3; ++j)
				{
					m_BindedBonesMatricesF[i][j][0] = xs::Matrix4::IDENTITY[j][0];
					m_BindedBonesMatricesF[i][j][1] = xs::Matrix4::IDENTITY[j][1];
					m_BindedBonesMatricesF[i][j][2] = xs::Matrix4::IDENTITY[j][2];
					m_BindedBonesMatricesF[i][j][3] = xs::Matrix4::IDENTITY[j][3];
				}
			}
		}

		m_mtl = m_mtlOrigin;
		m_mtl.initialize();
	}

	void SubModelInstance::updateBonesMatrix(xs::Skeleton * pSkeleton)
	{
		Assert(pSkeleton);
		if( 0 == m_pMI->getBlendShaderProgram() )return;
		if( 0 == pSkeleton) return;

		uint validIndicesNum = 0;
		uchar * pBindedBonesIndices = m_pSubModel->getBindedBonesIndices(validIndicesNum);
		for( uint i=0; i<validIndicesNum; ++i)
		{
			xs::Bone * pBone = pSkeleton->getBone(pBindedBonesIndices[i]);
			if( pBone) 
			{
				const xs::Matrix4 & fulltrans = pBone->getFullTransform();
				//m_BindedBonesMatrices[i] = fulltrans;
				for( uint j =0; j <3; ++j)
				{
					m_BindedBonesMatricesF[i][j][0] = fulltrans[j][0];
					m_BindedBonesMatricesF[i][j][1] = fulltrans[j][1];
					m_BindedBonesMatricesF[i][j][2] = fulltrans[j][2];
					m_BindedBonesMatricesF[i][j][3] = fulltrans[j][3];
				}
			}
		}
	}

	//void SubModelInstance::beforeRender(IRenderSystem *pRenderSystem,ModelNode *pNode)
	//{
	//	pRenderSystem->getSceneBlending(m_sbfSrc,m_sbfDst);
	//	const BlendMode& bm = m_mtl.getBlendMode();
	//	switch (bm)
	//	{
	//	case BM_OPAQUE:
	//		pRenderSystem->setSceneBlending(SBF_ONE,SBF_ZERO);
	//		pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);
	//		m_alphaCheck = pRenderSystem->alphaCheckEnabled();
	//		pRenderSystem->getAlphaFunction(m_alphaFunc,m_alphaCheckRef);
	//		pRenderSystem->setAlphaCheckEnabled(true);
	//		pRenderSystem->setAlphaFunction(CMPF_GREATER,0.01f);
	//		break;
	//	case BM_TRANSPARENT:
	//		{
	//			m_alphaCheck = pRenderSystem->alphaCheckEnabled();
	//			pRenderSystem->getAlphaFunction(m_alphaFunc,m_alphaCheckRef);
	//			pRenderSystem->setAlphaCheckEnabled(true);
	//			pRenderSystem->setAlphaFunction(CMPF_GREATER,0.01f);
	//		}
	//		break;
	//	case BM_ALPHA_BLEND:
	//		pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);
	//		m_alphaCheck = pRenderSystem->alphaCheckEnabled();
	//		pRenderSystem->getAlphaFunction(m_alphaFunc,m_alphaCheckRef);
	//		pRenderSystem->setAlphaCheckEnabled(true);
	//		pRenderSystem->setAlphaFunction(CMPF_GREATER,0.01f);
	//		break;
	//	case BM_ADDITIVE:
	//		pRenderSystem->setSceneBlending(SBF_SOURCE_COLOR,SBF_ONE);
	//		break;
	//	case BM_ADDITIVE_ALPHA:
	//		pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE);
	//		break;
	//	default:
	//		pRenderSystem->setSceneBlending(SBF_DEST_COLOR,SBF_SOURCE_COLOR);
	//		break;
	//	}

	//	m_cullingMode = pRenderSystem->getCullingMode();
	//	if(m_mtl.isTwoSide())
	//	{
	//		pRenderSystem->setCullingMode(CULL_NONE);
	//	}
	//	else
	//	{
	//		pRenderSystem->setCullingMode(CULL_CLOCKWISE);
	//	}

	//	pRenderSystem->setTexture(0,m_pTexture);

	//	m_surfaceDiffuse = pRenderSystem->getSurfaceDiffuse();
	//	if(pNode && pNode->isGlobalDiffuse())
	//	{
	//		pRenderSystem->setSurfaceDiffuse(pNode->getSurfaceDiffuse());
	//	}
	//	else
	//	{
	//		pRenderSystem->setSurfaceDiffuse(m_mtl.getSurfaceDiffuse());
	//	}
	//}

	//void SubModelInstance::afterRender(IRenderSystem *pRenderSystem)
	//{
	//	const BlendMode& bm = m_mtl.getBlendMode();
	//	switch (bm)
	//	{
	//	case BM_TRANSPARENT:
	//		{
	//			pRenderSystem->setAlphaCheckEnabled(m_alphaCheck);
	//			pRenderSystem->setAlphaFunction(m_alphaFunc,m_alphaCheckRef);
	//		}
	//		break;
	//	case BM_ALPHA_BLEND:
	//	case BM_OPAQUE:
	//		pRenderSystem->setAlphaCheckEnabled(m_alphaCheck);
	//		pRenderSystem->setAlphaFunction(m_alphaFunc,m_alphaCheckRef);
	//	case BM_ADDITIVE:
	//	case BM_ADDITIVE_ALPHA:
	//	default:
	//		pRenderSystem->setSceneBlending(m_sbfSrc,m_sbfDst);
	//		break;
	//	}

	//	pRenderSystem->setCullingMode(m_cullingMode);
	//	pRenderSystem->setTexture(0,0);
	//	pRenderSystem->setSurfaceDiffuse(m_surfaceDiffuse);
	//}

	int SubModelInstance::getRenderOrder()
	{
		return m_mtl.getRenderOrder();
	}

	void SubModelInstance::updateColor(const ColorValue& diffuse)
	{
		int numLayers = m_mtl.numLayers();
		for(int i = 0;i < numLayers;i++)
		{
			MaterialLayer *pLayer = m_mtl.getLayer(i);
			pLayer->m_fullDiffuse = diffuse * pLayer->m_diffuse;
		}
	}

	void SubModelInstance::update(const AnimationTime *pAnimationTime,const ColorValue& diffuse)
	{
		if(m_pGeosetAnimation)
		{
			m_GeosetAlpha = m_pGeosetAnimation->alpha.getFrame(pAnimationTime);
		}
		int numLayers = m_mtl.numLayers();
		for(int i = 0;i < numLayers;i++)
		{
			MaterialLayer *pLayer = m_mtl.getLayer(i);
			pLayer->m_fullDiffuse = diffuse * pLayer->m_diffuse;
			if( m_mtl.m_kfTrans.numKeyFrames() > 0) 
			{
				pLayer->m_fullDiffuse.a = diffuse.a * m_mtl.m_kfTrans.getFrame(pAnimationTime);
			}
		}
	}

	void  SubModelInstance::renderCPUVertexBlend(IRenderSystem* pRenderSystem,bool useMtl)
	{
		PP_BY_NAME("SubModelInstance::renderCPUVertexBlend");

		IVertexBuffer *pVB = m_pMI->getVertexBuffer();
		if( pVB == 0) 
			return;

		IIndexBuffer *pIndexBuffer = m_pMI->getIndexBuffer();
		if( pIndexBuffer ==0 ) 
			return;

		pRenderSystem->setIndexBuffer(pIndexBuffer);
		pRenderSystem->setNormalVertexBuffer(pVB,m_pMI->getNumVertices() * 12);
		//pRenderSystem->setTexcoordVertexBuffer(0,m_pMI->getVertexBufferTexcoord());
		pRenderSystem->setVertexVertexBuffer(pVB);

		Matrix4 mtx = pRenderSystem->getWorldMatrix();
		pRenderSystem->setWorldMatrix(mtx * getWorldTransform());

		if( useMtl)
		{
			if(m_pMI->isAnimatedTexCoord() )
			{
				int numLayer = m_mtl.numLayers();
				for(int i = 0;i < numLayer;i++)
				{
					MaterialLayer *pLayer = m_mtl.getLayer(i);
					if(pLayer->beginPass(pRenderSystem))
					{
						PP_BY_NAME_START("SubModelInstance::renderCPUVertexBlend::drawRangeIndexedPrimitive(useMtl=true,isAnimatedTexCoord=true)");

						pRenderSystem->setTexcoordVertexBuffer(0, m_pMI->getTexCoordBuffer( i ) );
						pRenderSystem->drawRangeIndexedPrimitive(PT_TRIANGLES,m_pMI->getIndexBuffer()->getType(),indexStart,indexCount,vertexStart,vertexEnd);
						
						PP_BY_NAME_STOP();

						pLayer->endPass(pRenderSystem);
					}
				}

			}
			else
			{
				pRenderSystem->setTexcoordVertexBuffer(0,m_pMI->getVertexBufferTexcoord());
				int numLayer = m_mtl.numLayers();
				for(int i = 0;i < numLayer;i++)
				{
					MaterialLayer *pLayer = m_mtl.getLayer(i);
					if(pLayer->beginPass(pRenderSystem))
					{
						PP_BY_NAME_START("SubModelInstance::renderCPUVertexBlend::drawRangeIndexedPrimitive(useMtl=true,isAnimatedTexCoord=false)");

						pRenderSystem->drawRangeIndexedPrimitive(PT_TRIANGLES,m_pMI->getIndexBuffer()->getType(),indexStart,indexCount,vertexStart,vertexEnd);
						
						PP_BY_NAME_STOP();

						pLayer->endPass(pRenderSystem);
					}
				}
			}

		}
		else
		{
			PP_BY_NAME_START("SubModelInstance::renderCPUVertexBlend::drawRangeIndexedPrimitive(useMtl=false)");
			IShaderProgram*		pShaderProgram = pRenderSystem->getShaderProgramManager()->getCurrentShaderProgam();
			if( 0 != pShaderProgram )
			{
				//绑定变换矩阵
				if( pShaderProgram->getProgramType() == SPT_HIGHLEVEL)
					static_cast<IHighLevelShaderProgram*>(pShaderProgram)->bindTransformMatrix(TMT_WORLD_VIEW_PROJECTION);
				else
					static_cast<ILowLevelShaderProgram*>(pShaderProgram)->bindTransformMatrix(0,TMT_WORLD_VIEW_PROJECTION);
			}
			pRenderSystem->setTexcoordVertexBuffer(0,m_pMI->getVertexBufferTexcoord());
			pRenderSystem->drawRangeIndexedPrimitive(PT_TRIANGLES,m_pMI->getIndexBuffer()->getType(),indexStart,indexCount,vertexStart,vertexEnd);
			PP_BY_NAME_STOP();
		}

		pRenderSystem->setWorldMatrix(mtx);

		pRenderSystem->setIndexBuffer(0);
		pRenderSystem->setVertexVertexBuffer(0);
		pRenderSystem->setNormalVertexBuffer(0);
		pRenderSystem->setTexcoordVertexBuffer(0,0);
	}

	void  SubModelInstance::renderGPUVertexBlend(IRenderSystem* pRenderSystem,bool useMtl)
	{
		PP_BY_NAME("SubModelInstance::renderGPUVertexBlend");

		IVertexBuffer *pVB = m_pMI->getVertexBuffer();
		if( pVB == 0) 
			return;

		IIndexBuffer *pIndexBuffer = m_pMI->getIndexBuffer();
		if( pIndexBuffer ==0 ) 
			return;

		IVertexBuffer * pBlendWeightBuffer = m_pMI->getBlendWeightBuffer();
		if( 0 == pBlendWeightBuffer)
			return;

		IVertexBuffer * pBlendIndicesBuffer = m_pMI->getBlendIndicesBuffer( m_uiSubModelIndex);
		if( 0 == pBlendIndicesBuffer)
			return;

		IShaderProgram * pProgram = m_pMI->getBlendShaderProgram();
		if( 0 == pProgram)  return;
		if( SPT_LOWLEVEL != pProgram->getProgramType() ) return; //用Lowlevel的shader program
		//if( SPT_HIGHLEVEL != pProgram->getProgramType() ) return; //用high level 的shader program
		
		pRenderSystem->setIndexBuffer(pIndexBuffer);
		pRenderSystem->setNormalVertexBuffer(pVB,m_pMI->getNumVertices() * 12);
		//pRenderSystem->setTexcoordVertexBuffer(0,m_pMI->getVertexBufferTexcoord());
		pRenderSystem->setVertexVertexBuffer(pVB);
		pRenderSystem->setBlendWeightVertexBuffer( pBlendWeightBuffer, 0);
		pRenderSystem->setBlendIndicesVertexBuffer( pBlendIndicesBuffer,0);

		Matrix4 mtx = pRenderSystem->getWorldMatrix();
		pRenderSystem->setWorldMatrix(mtx * getWorldTransform());

		pProgram->bind();
		//IHighLevelShaderProgram * pHighLevel = static_cast<IHighLevelShaderProgram*>(pProgram);
		//pHighLevel->bindTransformMatrix(TMT_WORLD_VIEW_PROJECTION);
		//pHighLevel->setUniformMatrixArray("g_blend_matrices_palette", VERTEX_BLEND_MATRICES_NUM, m_BindedBonesMatrices );
		ILowLevelShaderProgram * pLowLevel = static_cast<ILowLevelShaderProgram*>(pProgram);
		pLowLevel->setUniformFloatVector4(0,&m_BindedBonesMatricesF[0][0][0],VERTEX_BLEND_MATRICES_NUM * 3);
		pLowLevel->bindTransformMatrix(249,TMT_WORLD_VIEW_PROJECTION);

		if( useMtl)
		{
			if(m_pMI->isAnimatedTexCoord() )
			{
				int numLayer = m_mtl.numLayers();
				for(int i = 0;i < numLayer;i++)
				{
					MaterialLayer *pLayer = m_mtl.getLayer(i);
					if(pLayer->beginPass(pRenderSystem))
					{
						PP_BY_NAME_START("SubModelInstance::renderGPUVertexBlend::drawRangeIndexedPrimitive(useMtl=true, isAnimatedTexCoord=true)");

						//设置颜色变量
						xs::Vector4 fulldiffuse(pLayer->m_fullDiffuse.r, pLayer->m_fullDiffuse.g, pLayer->m_fullDiffuse.b, pLayer->m_fullDiffuse.a);
						//pHighLevel->setUniformVector4("g_material_ambient", xs::Vector4(m_fullDiffuse.r, m_fullDiffuse.g, m_fullDiffuse.b, m_fullDiffuse.a));
						//pHighLevel->setUniformVector4("g_material_diffuse", xs::Vector4(m_fullDiffuse.r, m_fullDiffuse.g, m_fullDiffuse.b, m_fullDiffuse.a));
						pLowLevel->setUniformFloatVector4(253, fulldiffuse.val,1);
						pRenderSystem->setTexcoordVertexBuffer(0, m_pMI->getTexCoordBuffer( i ) );
						pRenderSystem->drawRangeIndexedPrimitive(PT_TRIANGLES,m_pMI->getIndexBuffer()->getType(),indexStart,indexCount,vertexStart,vertexEnd);

						PP_BY_NAME_STOP();

						pLayer->endPass(pRenderSystem);
						
					}
				}

			}
			else
			{
				pRenderSystem->setTexcoordVertexBuffer(0,m_pMI->getVertexBufferTexcoord());
				int numLayer = m_mtl.numLayers();
				for(int i = 0;i < numLayer;i++)
				{
					MaterialLayer *pLayer = m_mtl.getLayer(i);
					if(pLayer->beginPass(pRenderSystem))
					{
						PP_BY_NAME_START("SubModelInstance::renderGPUVertexBlend::drawRangeIndexedPrimitive(useMtl=true,isAnimatedTexCoord=fale)");

						//设置颜色变量
						xs::Vector4 fulldiffuse(pLayer->m_fullDiffuse.r, pLayer->m_fullDiffuse.g, pLayer->m_fullDiffuse.b, pLayer->m_fullDiffuse.a);
						//pHighLevel->setUniformVector4("g_material_ambient", fulldiffuse);
						//pHighLevel->setUniformVector4("g_material_diffuse", fulldiffuse);
						pLowLevel->setUniformFloatVector4(253, fulldiffuse.val,1);					
						pRenderSystem->drawRangeIndexedPrimitive(PT_TRIANGLES,m_pMI->getIndexBuffer()->getType(),indexStart,indexCount,vertexStart,vertexEnd);

						PP_BY_NAME_STOP();

						pLayer->endPass(pRenderSystem);
					}
				}
			}

		}
		else
		{
			PP_BY_NAME_START("SubModelInstance::renderGPUVertexBlend::drawRangeIndexedPrimitive(useMtl=false)");

			//设置颜色变量
			xs::ColorValue color = pRenderSystem->getColor();
			//pHighLevel->setUniformVector4("g_material_ambient", xs::Vector4(color.r, color.g, color.b, color.a));
			//pHighLevel->setUniformVector4("g_material_diffuse", xs::Vector4(color.r, color.g, color.b, color.a));
			pLowLevel->setUniformFloatVector4(253, color.val,1);
			pRenderSystem->setTexcoordVertexBuffer(0,m_pMI->getVertexBufferTexcoord());
			pRenderSystem->drawRangeIndexedPrimitive(PT_TRIANGLES,m_pMI->getIndexBuffer()->getType(),indexStart,indexCount,vertexStart,vertexEnd);

			PP_BY_NAME_STOP();
		}

		pProgram->unbind();

		pRenderSystem->setWorldMatrix(mtx);

		pRenderSystem->setBlendIndicesVertexBuffer(0);
		pRenderSystem->setBlendWeightVertexBuffer(0);
		pRenderSystem->setIndexBuffer(0);
		pRenderSystem->setVertexVertexBuffer(0);
		pRenderSystem->setNormalVertexBuffer(0);
		pRenderSystem->setTexcoordVertexBuffer(0,0);
	}

	void  SubModelInstance::render(IRenderSystem* pRenderSystem,bool useMtl)
	{
		if(m_GeosetAlpha < 0.001f)
			return;

		if(pRenderSystem->getRenderMode() == RENDER_MODE_SELECTION )
			return;

		if( !useMtl && !m_pMI->needDrawShadow() ) //不需要绘制阴影
			return; 

		if( m_pMI->getBlendShaderProgram() )
			renderGPUVertexBlend(pRenderSystem, useMtl);
		else
			renderCPUVertexBlend(pRenderSystem, useMtl);
	}

	const Matrix4& SubModelInstance::getWorldTransform() const
	{
		ModelNode *pNode = m_pMI->getModelNode();
		if(pNode)
		{
			return pNode->getFullTransform();
		}
		else
		{
			static Matrix4 m = Matrix4::IDENTITY;
			return m;
		}
	}

	Material*	SubModelInstance::getMaterial()
	{
		return &m_mtl;
	}

	Material* SubModelInstance::getOriginalMaterial()
	{
		return &m_mtlOrigin;
	}

	const char* SubModelInstance::getName()
	{
		return m_name.c_str();
	}

	bool SubModelInstance::isVisible()
	{
		return m_bVisible;
	}

	void  SubModelInstance::setVisible(bool bVisible)
	{
		m_bVisible = bVisible;
	}

	void SubModelInstance::setTexture(int layer,const char* szName)
	{
		Assert(layer >= 0 && layer < m_mtlOrigin.numLayers());
		MaterialLayer *pLayer = m_mtl.getLayer(layer);

		const std::string& textureName = pLayer->m_szTexture;
		if(StringHelper::casecmp(textureName,szName) == 0)return;

		safeRelease(pLayer->m_pTexture);
		pLayer->m_szTexture = "";

		if(!szName)return;

		IRenderEngine *pRenderEngine = getRenderEngine();
		IRenderSystem *pRenderSystem = pRenderEngine->getRenderSystem();
		ITextureManager *pTextureManager = pRenderSystem->getTextureManager();
		pLayer->m_pTexture = pTextureManager->createTextureFromFile(szName,FO_LINEAR,FO_LINEAR,FO_LINEAR);
		if(pLayer->m_pTexture)
		{
			pLayer->m_szTexture = szName;
		}
	}

	ITexture* SubModelInstance::getTexture(int layer)
	{
		Assert(layer >= 0 && layer < m_mtlOrigin.numLayers());

		return m_mtl.getLayer(layer)->m_pTexture;
	}

	ushort SubModelInstance::numIndices()
	{
		return indexCount;
	}
}