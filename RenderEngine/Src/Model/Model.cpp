//#include "pch.h"
#include "StdAfx.h"
#include "Model.h"

//namespace RE
namespace xs
{
#define SAFE_RELEASE_MODEL(x)	{if(x){x->release();x=0;}}

	Model::Model(const std::string& name,IRenderSystem* pRenderSystem) :
		m_strFilename(name),
		m_pRenderSystem(pRenderSystem),
		m_pVBBoundingVertices(0),
		m_pVBBoundingNormals(0),
		m_pIBBounding(0),
		m_pVBVertex(0),
		m_pVBNormal(0),
		m_pVBTexcoord(0),
		m_pIB(0),
		m_pSkeleton(0),
		m_pGlobalSequences(0),
		m_pVBVertexNormal(0),
		m_pOriginVertices(0),
		m_ui32VerticeNum(0),
		m_fLastCalcTime(0),
		m_bAnimatedGeometry(false),
		m_bAnimatedBones(false),
		m_bNeedPerInstanceAnimation(false),
		m_pBoundingVertices(0),
		m_pBoundingIndices(0)
	{
		for(Uint32 i = 0;i < 32;i++)
		{
			m_textures[i] = 0;
			m_useReplacableTextures[i] = false;
			m_replacableTextures[i] = 0;
			m_replacableTextureTypes[i] = -1;
		}
	}

	Model::~Model()
	{
		if(m_pBoundingVertices)
		{
			delete m_pBoundingVertices;
			m_pBoundingVertices = 0;
		}
		
		if(m_pBoundingIndices)
		{
			delete m_pBoundingIndices;
			m_pBoundingIndices = 0;
		}
		
		if(m_pOriginVertices)
		{
			delete m_pOriginVertices;
			m_pOriginVertices = 0;
		}
		if(m_pGlobalSequences)
		{
			delete[] m_pGlobalSequences;
			m_pGlobalSequences = 0;
		}
		STLDeleteSequence(m_vRibbonSystems);
		STLDeleteSequence(m_vParticleSystems);
		STLDeleteSequence(m_vAnimations);
		if(m_pSkeleton)
		{
			delete m_pSkeleton;
			m_pSkeleton = 0;
		}
		SAFE_RELEASE_MODEL(m_pVBVertexNormal);
		SAFE_RELEASE_MODEL(m_pVBBoundingVertices);
		SAFE_RELEASE_MODEL(m_pVBBoundingNormals);
		SAFE_RELEASE_MODEL(m_pIBBounding);
		SAFE_RELEASE_MODEL(m_pVBVertex);
		SAFE_RELEASE_MODEL(m_pVBTexcoord);
		SAFE_RELEASE_MODEL(m_pVBNormal);
		SAFE_RELEASE_MODEL(m_pIB);

		ITextureManager *pTextureManager = m_pRenderSystem->getTextureManager();
		for(Uint32 i = 0;i < 32;i++)
		{
			if(m_textures[i])
			{
				pTextureManager->releaseTexture(m_textures[i]);
				m_textures[i] = 0;
			}
			if(m_replacableTextures[i])
			{
				pTextureManager->releaseTexture(m_replacableTextures[i]);
				m_replacableTextures[i] = 0;
			}
		}


	}

	ITexture* Model::getTexture(Uint32 index)
	{
		Int32 type = m_replacableTextureTypes[index];
		if(type == -1)return m_textures[index];
		
		return m_replacableTextures[type];
	}

	Uint32 Model::getNumSubModels()
	{
		return m_vRenderPasses.size();
	}

	void	Model::setSubModelVisible(Uint32 index,Bool bVisible)
	{
		assert(index >= 0 && index < m_vRenderPasses.size());
		SubModel &p = m_vRenderPasses[index];
		p.bVisible = bVisible;
	}

	Uint32	Model::getSkinCount()
	{
		return m_vSkin.size();
	}

	Bool Model::setSkin(Uint32 index)
	{
		if(index < m_vSkin.size())
		{
			for(Uint32 j = 0;j < MAX_REPLACABLE_SKIN_COUNT;j++)
			{
				Uint32 id = m_vSkin[index].ui32Base + j;
				if(m_useReplacableTextures[m_vSkin[index].ui32Base + j])
				{
					ITextureManager *pTextureManager = m_pRenderSystem->getTextureManager();
					ITexture *pTexture = m_replacableTextures[id];
					if(pTexture)pTextureManager->releaseTexture(pTexture);
					m_replacableTextures[id] = pTextureManager->createTextureFromFile(m_vSkin[index].strFileName[j],FO_LINEAR,FO_LINEAR,FO_LINEAR);
				}
			}
			return true;
		}

		return false;
	}

	Uint32	Model::getAnimationCount()
	{
		return m_vAnimations.size();
	}

	Animation* Model::hasAnimation(const std::string& animation)
	{
		int i = m_AnimationMap.size();
		std::hash_map<std::string,Animation*>::iterator ii = m_AnimationMap.begin();
		std::string s = (*ii).first.c_str();
		std::hash_map<std::string,Animation*>::iterator it = m_AnimationMap.find(animation.c_str());
		if(it == m_AnimationMap.end())return 0;

		return it->second;
	}

	void	Model::updateGeometry(Uint32 frame,IRenderSystem *pRenderSystem,IVertexBuffer *pVBVertexBlend)
	{
		Animation *pCurrentAnimation = hasAnimation(animation);

		if(pCurrentAnimation && ((m_bAnimatedBones && m_pSkeleton) || !m_vParticleSystems.empty() || !m_vRibbonSystems.empty()))
		{
			Uint32 timeStart = pCurrentAnimation->getTimeStart();
			Uint32 timeStride = pCurrentAnimation->getTimeEnd() - timeStart;
			if(timeStride == 0)timeStride = 1;
			Uint32 globalTime = tick;
			Uint32 offsetTime = globalTime % timeStride;
			Uint32 time = offsetTime + timeStart;
			Uint32 rangeIndex = pCurrentAnimation->getRangeIndex();

			if(m_bAnimatedBones && m_pSkeleton && pCurrentAnimation)
			{
				m_pSkeleton->calcMatrix(pRenderSystem,tick,time,rangeIndex,m_pGlobalSequences);
			}
			if(pCurrentAnimation && !m_vParticleSystems.empty())
			{
				std::vector<ParticleSystem*>::iterator begin = m_vParticleSystems.begin();
				std::vector<ParticleSystem*>::iterator end = m_vParticleSystems.end();
				std::vector<ParticleSystem*>::iterator it;
				for(it = begin;it != end;++it)
				{
					ParticleSystem *pParticleSystem = (*it);
					Uint32 t = timeStart + (time + (Int32)(timeStride * pParticleSystem->getRandom())) % timeStride;
					pParticleSystem->update(tick,tickDelta,t,rangeIndex,m_pGlobalSequences);
				}
			}
			if(pCurrentAnimation && !m_vRibbonSystems.empty())
			{
				std::vector<RibbonSystem*>::iterator begin = m_vRibbonSystems.begin();
				std::vector<RibbonSystem*>::iterator end = m_vRibbonSystems.end();
				std::vector<RibbonSystem*>::iterator it;
				for(it = begin;it != end;++it)
				{
					RibbonSystem *pRibbonSystem = (*it);
					pRibbonSystem->update(tick,tickDelta,time,rangeIndex,m_pGlobalSequences);
				}
			}
		}

		if(m_bAnimatedGeometry)
		{
			Real inv255 = 0.0039215689f;//1 / 255.0f;
			Vector3 *pVertices = (Vector3*)m_pVBVertexNormal->lock(0,0,BL_DISCARD);
			if(pVertices)
			{
				size_t size = m_vRenderPasses.size();
				for(size_t i = 0;i < size;i++) 
				{
					SubModel &p = m_vRenderPasses[i];
					if(p.bAnimatedGeometry && p.bVisible)
					{
						for(Uint16 j = p.vertexStart;j < p.vertexEnd;j++)
						{
							Vector3 v(0,0,0),n(0,0,0);
							AnimatedVertex *ov = m_pOriginVertices + j;

							for(Uint16 b = 0;b < 4;b++) 
							{
								if(ov->weights[b] > 0) 
								{
									Bone *pBone = m_pSkeleton->getBone(ov->bones[b]);
									Vector3 tv = pBone->getFullTransform() * ov->pos;
									Vector3 tn = pBone->getFullRotation() * ov->normal;
									Real weightInv255 = ov->weights[b] * inv255;
									v += tv * weightInv255;
									n += tn * weightInv255;
								}
							}

							pVertices[j] = v;
							pVertices[m_ui32VerticeNum + j] = n;
						}
					}
				}
				m_pVBVertexNormal->unlock();
			}
		}
	}

	void	Model::render_Instancing(IRenderSystem* pRenderSystem)
	{
		size_t size = m_vRenderPasses.size();
		for(size_t i = 0;i < size;i++)
		{
			SubModel &p = m_vRenderPasses[i];
			if(p.bVisible && p.initialize(this,pRenderSystem))
			{
				pRenderSystem->drawRangeIndexedPrimitive(
					PT_TRIANGLES,
					m_pIB->getType(),
					p.indexStart,
					p.indexCount,
					p.vertexStart,
					p.vertexEnd);
				p.finalize(pRenderSystem);
			}
		}
	}

	void	Model::setGeometry_Instancing(IRenderSystem* pRenderSystem,IVertexBuffer *pVBVertexBlend)
	{
		pRenderSystem->setIndexBuffer(m_pIB);
		if(m_bAnimatedGeometry)
		{
			pRenderSystem->setVertexVertexBuffer(m_pVBVertexNormal);
			pRenderSystem->setNormalVertexBuffer(m_pVBVertexNormal,m_ui32VerticeNum * 12);
		}
		else
		{
			pRenderSystem->setVertexVertexBuffer(m_pVBVertex);
			pRenderSystem->setNormalVertexBuffer(m_pVBNormal);
		}
		pRenderSystem->setTexcoordVertexBuffer(0,m_pVBTexcoord);
	}

	void	Model::resetGeometry_Instancing(IRenderSystem* pRenderSystem)
	{
		pRenderSystem->setIndexBuffer(0);
		pRenderSystem->setVertexVertexBuffer(0);
		pRenderSystem->setNormalVertexBuffer(0);
		pRenderSystem->setTexcoordVertexBuffer(0,0);
	}

	void	Model::setGeometrySubModel_Instancing(Uint32 index,IRenderSystem* pRenderSystem)
	{
		SubModel &p = m_vRenderPasses[index];
		if(p.bVisible)p.initialize(this,pRenderSystem);
	}

	void	Model::resetGeometrySubModel_Instancing(Uint32 index,IRenderSystem* pRenderSystem)
	{
		SubModel &p = m_vRenderPasses[index];
		if(p.bVisible)p.finalize(pRenderSystem);
	}

	void	Model::renderSubModel_Instancing(Uint32 index,IRenderSystem* pRenderSystem)
	{
		SubModel &p = m_vRenderPasses[index];
		pRenderSystem->drawRangeIndexedPrimitive(
			PT_TRIANGLES,
			m_pIB->getType(),
			p.indexStart,
			p.indexCount,
			p.vertexStart,
			p.vertexEnd);
	}

	void	Model::renderFX_Instancing(Real tick,Real tickDelta,IRenderSystem* pRenderSystem,Camera* pCamera)
	{
		if(!m_vParticleSystems.empty())
		{
			std::vector<ParticleSystem*>::iterator begin = m_vParticleSystems.begin();
			std::vector<ParticleSystem*>::iterator end = m_vParticleSystems.end();
			std::vector<ParticleSystem*>::iterator it;
			for(it = begin;it != end;++it)
			{
				ParticleSystem *pParticleSystem = (*it);
				pParticleSystem->render(pRenderSystem);
			}
		}
		if(!m_vRibbonSystems.empty())
		{
			std::vector<RibbonSystem*>::iterator begin = m_vRibbonSystems.begin();
			std::vector<RibbonSystem*>::iterator end = m_vRibbonSystems.end();
			std::vector<RibbonSystem*>::iterator it;
			for(it = begin;it != end;++it)
			{
				RibbonSystem *pRibbonSystem = (*it);
				pRibbonSystem->render(pRenderSystem,pCamera);
			}
		}
	}

	const std::string& Model::getFileName()
	{
		return m_strFilename;
	}

	void	Model::render(IRenderSystem* pRenderSystem,IModelCallback *pCallback)
	{
		size_t size = m_vRenderPasses.size();
		if(size > 0)
		{
			pRenderSystem->setIndexBuffer(m_pIB);
			if(m_bAnimatedGeometry)
			{
				pRenderSystem->setVertexVertexBuffer(m_pVBVertexNormal);
				pRenderSystem->setNormalVertexBuffer(m_pVBVertexNormal,m_ui32VerticeNum * 12);
			}
			else
			{
				pRenderSystem->setVertexVertexBuffer(m_pVBVertex);
				pRenderSystem->setNormalVertexBuffer(m_pVBNormal);
			}
			pRenderSystem->setTexcoordVertexBuffer(0,m_pVBTexcoord);
			for(size_t i = 0;i < size;i++)
			{
				SubModel &p = m_vRenderPasses[i];
				if(p.bVisible && p.initialize(this,pRenderSystem))
				{
					pRenderSystem->drawRangeIndexedPrimitive(
						PT_TRIANGLES,
						m_pIB->getType(),
						p.indexStart,
						p.indexCount,
						p.vertexStart,
						p.vertexEnd);
					p.finalize(pRenderSystem);
				}
			}
			pRenderSystem->setIndexBuffer(0);
			pRenderSystem->setVertexVertexBuffer(0);
			pRenderSystem->setNormalVertexBuffer(0);
			pRenderSystem->setTexcoordVertexBuffer(0,0);
		}
		/*
		Vector3 *pVertex = (Vector3 *)m_pVBVertex->lock(0,0,BL_NORMAL);
		Vector3 *pNormal = (Vector3 *)m_pVBNormal->lock(0,0,BL_NORMAL);
		for(Uint32 i = 0;i < m_pVBVertex->getNumVertices();i++)
		{
			pRenderSystem->beginPrimitive(PT_LINES);
			pRenderSystem->sendVertex(pVertex[i]);
			pRenderSystem->sendVertex(pVertex[i] + 15 * pNormal[i]);
			pRenderSystem->endPrimitive();
		}
		m_pVBNormal->unlock();
		m_pVBVertex->unlock();*/
		/*
		if(m_pSkeleton)
		{
			Bool bLightingEnabled = m_pRenderSystem->isLightingEnabled();
			if(bLightingEnabled)m_pRenderSystem->setLightingEnabled(false);
			Uint32 size = m_pSkeleton->getBoneCount();
			pRenderSystem->beginPrimitive(PT_LINES);
				for(Uint32 i = 0;i < size;i++)
				{
					Bone *pBone = m_pSkeleton->getBone(i);
					Bone *pParent = pBone->getParent();
					if(pBone && pParent)
					{
						pRenderSystem->sendVertex(pBone->getTransformedPivot());
						pRenderSystem->sendVertex(pParent->getTransformedPivot());
					}
				}
			pRenderSystem->endPrimitive();
			if(bLightingEnabled)m_pRenderSystem->setLightingEnabled(true);
		}
		*/
/*
		if(m_pVBBoundingVertices && m_pVBBoundingNormals && m_pIBBounding)
		{
			pRenderSystem->setCullingMode(CULL_NONE);
			pRenderSystem->setFillMode(FM_WIREFRAME);
			pRenderSystem->setColor(ColorValue(1,0,0,1));
			pRenderSystem->setIndexBuffer(m_pIBBounding);
			pRenderSystem->setVertexVertexBuffer(m_pVBBoundingVertices);
			pRenderSystem->setNormalVertexBuffer(m_pVBBoundingNormals);
			pRenderSystem->drawIndexedPrimitive(PT_TRIANGLES,m_pIBBounding->getNumIndices(),m_pIBBounding->getType());
			pRenderSystem->setIndexBuffer(0);
			pRenderSystem->setNormalVertexBuffer(0);
			pRenderSystem->setVertexVertexBuffer(0);
			pRenderSystem->setColor(ColorValue::White);
			pRenderSystem->setCullingMode(CULL_CLOCKWISE);
			pRenderSystem->setFillMode(pCamera->getFillMode());
		}
*/
		if(!m_vParticleSystems.empty())
		{
			std::vector<ParticleSystem*>::iterator begin = m_vParticleSystems.begin();
			std::vector<ParticleSystem*>::iterator end = m_vParticleSystems.end();
			std::vector<ParticleSystem*>::iterator it;
			for(it = begin;it != end;++it)
			{
				ParticleSystem *pParticleSystem = (*it);
				pParticleSystem->render(pRenderSystem,pCamera);
			}
		}
		if(!m_vRibbonSystems.empty())
		{
			std::vector<RibbonSystem*>::iterator begin = m_vRibbonSystems.begin();
			std::vector<RibbonSystem*>::iterator end = m_vRibbonSystems.end();
			std::vector<RibbonSystem*>::iterator it;
			for(it = begin;it != end;++it)
			{
				RibbonSystem *pRibbonSystem = (*it);
				pRibbonSystem->render(pRenderSystem,pCamera);
			}
		}

	}

	const AABB&	Model::getAABB()
	{
		return m_AABB;
	}
	
	const Sphere& Model::getBoudingSphere()
	{
		return m_BoundingSphere;
	}

	RibbonSystem*	Model::onGetRibbonSystem()
	{
		RibbonSystem *pRibbonSystem = new RibbonSystem;
		if(pRibbonSystem)m_vRibbonSystems.push_back(pRibbonSystem);

		return pRibbonSystem;
	}

	ParticleSystem*	Model::onGetParticleSystem()
	{
		ParticleSystem *p = new ParticleSystem;
		if(p)m_vParticleSystems.push_back(p);

		return p;
	}

	void	Model::onGetGlobalSequences(Int32 *pGlobalSequences,Uint32 ui32GlobalSequences)
	{
		if(ui32GlobalSequences)
		{
			m_pGlobalSequences = new Int32[ui32GlobalSequences];
			memcpy(m_pGlobalSequences,pGlobalSequences,ui32GlobalSequences * sizeof(Int32));
		}
	}

	void	Model::onGetRenderPasses(const std::vector<SubModel>& vRenderPasses)
	{
		size_t size = vRenderPasses.size();
		for(size_t i = 0;i < size;i++)
		{
			m_vRenderPasses.push_back(vRenderPasses[i]);
		}
	}

	void	Model::onGetIndices(Uint16 *pIndices,Uint32 ui32Indices)
	{
		if(!ui32Indices)return;
		m_pIB = m_pRenderSystem->getBufferManager()->createIndexBuffer(IT_16BIT,ui32Indices,BU_STATIC_WRITE_ONLY);
		if(m_pIB)
		{
			Uint16 *pIndicesBuffer = (Uint16*)m_pIB->lock(0,0,BL_DISCARD);
			if(pIndicesBuffer)
			{
				memcpy(pIndicesBuffer,pIndices,ui32Indices * sizeof(Uint16));
				m_pIB->unlock();
			}
		}
	}

	Bone*		Model::getBone(Int16 bone)
	{
		if(m_pSkeleton)
			return m_pSkeleton->getBone(bone);
		return 0;
	}
	void	Model::onGetIsAnimated(Bool bAnimatedBones,Bool bAnimatedGeometry,Bool bNeedPerInstanceAnimation)
	{
		m_bAnimatedGeometry = bAnimatedGeometry;
		m_bAnimatedBones = bAnimatedBones;
		m_bNeedPerInstanceAnimation = bNeedPerInstanceAnimation;
	}

	void	Model::onGetSkin(const std::vector<CreatureSkin>& vSkin)
	{
		m_vSkin = vSkin;
		if(!m_vSkin.empty())
		{
			setSkin(0);
		}
	}

	void	Model::onGetTexture(Uint32 index,Bool bReplacableTexture,Int32 type,CPChar fileName)
	{
		if(!bReplacableTexture)
		{
			ITextureManager *pTextureManager = m_pRenderSystem->getTextureManager();
			m_textures[index] = pTextureManager->createTextureFromFile(fileName,FO_LINEAR,FO_LINEAR,FO_LINEAR);
		}
		else
		{
			m_textures[index] = 0;
			m_replacableTextureTypes[index] = type;
			m_useReplacableTextures[type] = true;
		}
	}

	void	Model::calcAABB(Vector3 *pVertices,Uint32 ui32Vertices,Uint32 ui32Stride,AABB& aabb,Sphere& sphere)
	{
		if(!ui32Vertices)return;

		Vector3 vMin = pVertices[0];
		Vector3 vMax = pVertices[0];

		Vector3 *p = pVertices;
		for(Uint32 i = 1;i < ui32Vertices;i++)
		{
			p = (Vector3*)((Uint8*)p + ui32Stride);

			if(p->x > vMax.x)vMax.x = p->x;
			if(p->y > vMax.y)vMax.y = p->y;
			if(p->z > vMax.z)vMax.z = p->z;
			if(p->x < vMin.x)vMin.x = p->x;
			if(p->y < vMin.y)vMin.y = p->y;
			if(p->z < vMin.z)vMin.z = p->z;
		}
		aabb.setExtents(vMin,vMax);
		sphere.setCenter(vMin.midPoint(vMax));
		sphere.setRadius((vMax - vMin).length() * 0.5f);
	}

	void	Model::onGetAnimatedVertice(AnimatedVertex *pVertices,Uint32 ui32Vertices,Uint32 ui32Stride)
	{
		if(!pVertices || !ui32Vertices)return;

		m_ui32VerticeNum = ui32Vertices;

		calcAABB(&pVertices->pos,ui32Vertices,ui32Stride,m_AABB,m_BoundingSphere);

		m_pOriginVertices = new AnimatedVertex[ui32Vertices];
		for(Uint32 i = 0;i < ui32Vertices;i++)
		{
			memcpy(m_pOriginVertices + i,(Uint8*)pVertices + i * ui32Stride,sizeof(AnimatedVertex));
		}

		IBufferManager *pBufferManager = m_pRenderSystem->getBufferManager();

		m_pVBVertexNormal = pBufferManager->createVertexBuffer(12,2 * ui32Vertices,BU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
		if(m_pVBVertexNormal)
		{
			Vector3 *pVertex = (Vector3 *)m_pVBVertexNormal->lock(0,0,BL_DISCARD);
			if(pVertex)
			{
				Vector3 *p = &pVertices->pos;
				for(Uint32 i = 0;i < ui32Vertices;i++)
				{
					pVertex[i] = *p;
					p = (Vector3*)((Uint8*)p + ui32Stride);
				}
				p = &pVertices->normal;
				for(Uint32 i = 0;i < ui32Vertices;i++)
				{
					pVertex[ui32Vertices + i] = *p;
					p = (Vector3*)((Uint8*)p + ui32Stride);
				}
				m_pVBVertexNormal->unlock();
			}
		}
	}

	void	Model::onGetVerticePositions(Vector3 *pVertices,Uint32 ui32Vertices,Uint32 ui32Stride)
	{
		if(!pVertices || !ui32Vertices)return;

		m_ui32VerticeNum = ui32Vertices;

		calcAABB(pVertices,ui32Vertices,ui32Stride,m_AABB,m_BoundingSphere);

		IBufferManager *pBufferManager = m_pRenderSystem->getBufferManager();
		m_pVBVertex = pBufferManager->createVertexBuffer(12,ui32Vertices,BU_DYNAMIC);
		if(m_pVBVertex)
		{
			Vector3 *pVertex = (Vector3 *)m_pVBVertex->lock(0,0,BL_DISCARD);
			if(pVertex)
			{
				Vector3 *p = pVertices;
				for(Uint32 i = 0;i < ui32Vertices;i++)
				{
					pVertex[i] = *p;
					p = (Vector3*)((Uint8*)p + ui32Stride);
				}
				m_pVBVertex->unlock();
			}
		}
	}

	void	Model::onGetVerticeNormals(Vector3 *pVertices,Uint32 ui32Vertices,Uint32 ui32Stride)
	{
		if(!pVertices)return;
		IBufferManager *pBufferManager = m_pRenderSystem->getBufferManager();

		m_pVBNormal = pBufferManager->createVertexBuffer(12,ui32Vertices,BU_DYNAMIC);
		if(m_pVBNormal)
		{
			Vector3 *pNormal = (Vector3 *)m_pVBNormal->lock(0,0,BL_DISCARD);
			if(pNormal)
			{
				Vector3 *p = pVertices;
				for(Uint32 i = 0;i < ui32Vertices;i++)
				{
					pNormal[i] = *p;
					p = (Vector3*)((Uint8*)p + ui32Stride);
				}
				m_pVBNormal->unlock();
			}
		}
	}

	void	Model::onGetVerticeTexcoords(Vector2 *pVertices,Uint32 ui32Vertices,Uint32 ui32Stride)
	{
		if(!pVertices)return;
		IBufferManager *pBufferManager = m_pRenderSystem->getBufferManager();

		m_pVBTexcoord = pBufferManager->createVertexBuffer(8,ui32Vertices,BU_STATIC_WRITE_ONLY);
		if(m_pVBTexcoord)
		{
			Vector2 *pTexcoord = (Vector2 *)m_pVBTexcoord->lock(0,0,BL_DISCARD);
			if(pTexcoord)
			{
				Vector2 *p = pVertices;
				for(Uint32 i = 0;i < ui32Vertices;i++)
				{
					pTexcoord[i] = *p;
					p = (Vector2*)((Uint8*)p + ui32Stride);
				}
				m_pVBTexcoord->unlock();
			}
		}
	}

	void	Model::onGetName(Char *szName)
	{
		m_strModelName = szName;
	}

	Skeleton*	Model::onGetSkeleton()
	{
		m_pSkeleton = new Skeleton;

		return m_pSkeleton;
	}

	Animation*	Model::onGetAnimation(Uint32 start,Uint32 end,Bool bLoop,Int16 id,Uint32 rangeIndex)
	{
		Char strAnimation[256];
		sprintf_s(strAnimation,"%d",id);

		if(m_AnimationMap.find(strAnimation) != m_AnimationMap.end())
		{
			TRACE0_ERROR("Same Animations in the Model.");
			return 0;
		}
		Animation *pAnimation = new Animation(start,end,bLoop,strAnimation,rangeIndex);
		m_vAnimations.push_back(pAnimation);

		if(pAnimation)m_AnimationMap[strAnimation] = pAnimation;

		int i = m_AnimationMap.size();

		std::hash_map<std::string,Animation*>::iterator ii = m_AnimationMap.begin();
		std::string s = (*ii).first;

		return pAnimation;
	}

	void Model::onGetBoundingVolumeData(Vector3 *pVertices,Uint32 ui32Vertices,Vector3 *pNormals,Uint32 ui32Normals,Uint16 *pIndices,Uint32 ui32Indices)
	{
		IBufferManager *pBufferManager = m_pRenderSystem->getBufferManager();

		//VertexBuffer
		m_pVBBoundingVertices = pBufferManager->createVertexBuffer(12,ui32Vertices,BU_STATIC_WRITE_ONLY);
		if(m_pVBBoundingVertices)
		{
			Vector3 *pVerticesBuffer = (Vector3*)m_pVBBoundingVertices->lock(0,0,BL_DISCARD);
			if(pVerticesBuffer)
			{
				memcpy(pVerticesBuffer,pVertices,ui32Vertices * sizeof(Vector3));
				m_pVBBoundingVertices->unlock();
			}
		}

		//NormalBuffer
		m_pVBBoundingNormals = pBufferManager->createVertexBuffer(12,ui32Normals,BU_STATIC_WRITE_ONLY);
		if(m_pVBBoundingNormals)
		{
			Vector3 *pNormalsBuffer = (Vector3*)m_pVBBoundingNormals->lock(0,0,BL_DISCARD);
			if(pNormalsBuffer)
			{
				memcpy(pNormalsBuffer,pNormals,ui32Normals * sizeof(Vector3));
				m_pVBBoundingNormals->unlock();
			}
		}

		//IndexBuffer
		m_pIBBounding = pBufferManager->createIndexBuffer(IT_16BIT,ui32Indices,BU_STATIC_WRITE_ONLY);
		if(m_pIBBounding)
		{
			Uint16 *pIndicesBuffer = (Uint16*)m_pIBBounding->lock(0,0,BL_DISCARD);
			if(pIndicesBuffer)
			{
				memcpy(pIndicesBuffer,pIndices,ui32Indices * sizeof(Uint16));
				m_pIBBounding->unlock();
			}
		}
		m_pBoundingVertices = new Vector3[ui32Vertices];
		memcpy(m_pBoundingVertices,pVertices,ui32Vertices * sizeof(Vector3));

		calcAABB(m_pBoundingVertices,ui32Vertices,sizeof(Vector3),m_CollisionAABB,m_CollisionSphere);

		m_pBoundingIndices = new Uint32[ui32Indices];
		//16BIT --> 32BIT
		for(Uint32 i = 0;i < ui32Indices;i++)
			m_pBoundingIndices[i] = pIndices[i];
#ifdef SUPPORT_COLLISION_DETECTION
		m_cd.initialize(m_pBoundingVertices,ui32Vertices,m_pBoundingIndices,ui32Indices);
#endif
	}
	
	void*	Model::getCollisionModel()
	{
#ifdef SUPPORT_COLLISION_DETECTION
		return (void*)&m_cd.m_model;
#else
		return 0;
#endif
	}

	const AABB& Model::getCollisionAABB()
	{
		return m_CollisionAABB;
	}

}