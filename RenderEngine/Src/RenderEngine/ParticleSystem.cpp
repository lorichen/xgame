#include "StdAfx.h"
#include "ParticleSystem.h"
#include "Bone.h"
#include "ModelNode.h"
#include "_ParticleSystem.h"
/*
2011/2/23 忽略了亮度增强和同步放大参数
*/

namespace xs
{
	//粒子系统部分运算可以放到GPU进行，以后再优化
	bool ParticleSystem::m_b25d = true;//使用2.5d模式
	bool ParticleSystem::s_bPause = false;
	const uint ParticleSystem::s_MaxVertices(512);//最大支持512个顶点，最大顶点数必须是4的倍数，应为绘制的是矩形
	const bool ParticleSystem::s_UseVBO(true);//使用vbo对象

	void ParticleSystem::set25dMode(bool b25d)
	{
		m_b25d = b25d;
		MmParticleSystem::set25d(b25d);
	}

	void ParticleSystem::setPause(bool bPause)
	{
		s_bPause = bPause;
		MmParticleSystem::setPause(bPause);
	}


	ParticleSystem::ParticleSystem()
	{
		m_pData = 0;
		m_pBone = 0;

		m_currentEmission = 0.0f;
		m_pTexture = 0;

		m_pNode = 0;
		m_color = ColorValue::White;

		//vertex buffer
		m_pVB = 0;
		m_pTexVB = 0;
		m_pDiffuseVB = 0;
		m_pIB = 0;
		m_uiPreviousQuads= 0;

		m_bInitializeTexture = false;
		m_bGenerate = true;
	}

	ParticleSystem::~ParticleSystem()
	{
		finalizeResource();
	}

	void ParticleSystem::Clear()
	{
		safeRelease(m_pTexture);
		m_pTexture = 0;
		m_vParticles.clear();
		m_currentEmission = 0.0f;
		m_color = ColorValue::White;

		m_uiPreviousQuads= 0;
		m_bInitializeTexture = false;
	}

	int ParticleSystem::__getCurParticleNum()
	{
		return m_vParticles.size();
	}
	
	void ParticleSystem::SetGenerateParticle(bool bGenerate)
	{
		m_bGenerate = bGenerate;
	}

	void ParticleSystem::generateParticleByTime(const AnimationTime * pTime, float tickDelta,IRenderSystem* pRenderSystem )
	{
		if(pTime == 0 || pRenderSystem == 0 ) return;

		float timeFactor = tickDelta / 1000.0f;
		m_currentEmission += m_pData->emissionRate.getFrame(pTime) * timeFactor;
		uint num = Math::Floor( m_currentEmission );
		m_currentEmission = m_currentEmission - static_cast<float>(num);

		Particle p;
		float		speed, width, length, latitude, variation;
		p.m_pData = m_pData;
		p.m_pParent = this;
		const Matrix4& mtxBone = m_pBone->getFullTransform();

		for( uint i =0; i < num; ++i)
		{
			speed = m_pData->speed.getFrame(pTime);	
			width = m_pData->width.getFrame(pTime);
			length = m_pData->length.getFrame(pTime);
			latitude = m_pData->latitude.getFrame(pTime);
			variation = m_pData->variation.getFrame(pTime);
			speed *= (1.0f + Math::RangeRandom(0.0f, variation));

			if( m_pData->attach2Bone)//绑定到骨骼
			{
				//位置
				Vector3 pivotPoint = m_pBone->getPivot();
				pivotPoint.x += Math::RangeRandom(-width,width);
				pivotPoint.z += Math::RangeRandom(-length,length);
				p.m_p = pivotPoint;
				p.m_originPos = p.m_p;

				//速度
				Matrix4 mtxY;
				mtxY.rotationX(Math::RangeRandom(0,latitude));
				Matrix4 mtxZ;
				mtxZ.rotationY(Math::RangeRandom(0,360));
				p.m_v = mtxZ * mtxY* Vector3(0.0f,1.0f,0.0f);
				p.m_v.normalize();
				p.m_v *= speed;	

				//重力
				p.m_gravity = m_pData->gravity.getFrame(pTime);
			}
			else//不绑定到骨骼
			{
				//位置
				Matrix4 mtx;
				if(m_pNode) 
					mtx = m_pNode->getFullTransform() * mtxBone;
				else 
					mtx = mtxBone;
				Vector3 pivotPoint = m_pBone->getPivot();
				pivotPoint.x += Math::RangeRandom(-width,width);
				pivotPoint.z += Math::RangeRandom(-length,length);
				p.m_p = mtx * pivotPoint;
				p.m_originPos = p.m_p;
				//速度
				Matrix4 mtxY;
				mtxY.rotationX(Math::RangeRandom(0,latitude));
				Matrix4 mtxZ;
				mtxZ.rotationY(Math::RangeRandom(0,360));
				p.m_v = mtxZ * mtxY* Vector3(0.0f,1.0f,0.0f);
				Quaternion q1 = mtxBone.extractQuaternion();
				p.m_v = q1 * p.m_v;
				if( m_pNode)
				{
					Quaternion q2 = m_pNode->getFullTransform().extractQuaternion();
					p.m_v = q2 * p.m_v;
				}
				p.m_v.normalize();
				p.m_v *= speed;		
				//重力
				p.m_gravity = m_pData->gravity.getFrame(pTime);
			}
			
			p.m_life = m_pData->lifeSpan;
			p.m_currentFrame = 0;
			if(m_pData->head)
			{
				p.m_numHeadFrames = static_cast<int>((m_pData->headLifeSpan.y - m_pData->headLifeSpan.x + 1.0f) * m_pData->headLifeSpan.z);
				p.m_numHeadDecayFrames = static_cast<int>((m_pData->headDecay.y - m_pData->headDecay.x + 1.0f) * m_pData->headDecay.z);
			}
			else
			{
				p.m_numHeadFrames = 0;
				p.m_numHeadDecayFrames = 0;
			}

			if(m_pData->tail)
			{
				p.m_numTailFrames = static_cast<int>((m_pData->tailLifeSpan.y - m_pData->tailLifeSpan.x + 1.0f) * m_pData->tailLifeSpan.z);
				p.m_numTailDecayFrames = static_cast<int>((m_pData->tailDecay.y - m_pData->tailDecay.x + 1.0f) * m_pData->tailDecay.z);
			}
			else
			{
				p.m_numTailFrames = 0;
				p.m_numTailDecayFrames = 0;
			}

			if (0 == m_pData->nIntervalTime)
			{
				p.m_fSelfRotAngle = m_pData->nAngle;
			}
			p.update(tickDelta,pRenderSystem);
			m_vParticles.push_back(p);
		}
	}

	void ParticleSystem::generateParticleBySpace(const AnimationTime * pTime, float tickDelta,IRenderSystem* pRenderSystem)
	{
		if( pTime == 0 || pRenderSystem == 0 ) return;

		// 按照美术要求，这里的emissionRate和mm粒子系统中rate含义一致，用于判断是否产生粒子，而不用于判断产生的粒子数，产生的粒子数由
		//距离决定 [5/3/2011 zgz]
		if (m_pData->emissionRate.getFrame(pTime) <= 0.5f)
		{
			return;
		}

		Particle p;
		float		speed, width, length, latitude, variation;
		p.m_pData = m_pData;
		p.m_pParent = this;
		const Matrix4& mtxBone = m_pBone->getFullTransform();

		speed = m_pData->speed.getFrame(pTime);	
		width = m_pData->width.getFrame(pTime);
		length = m_pData->length.getFrame(pTime);
		latitude = m_pData->latitude.getFrame(pTime);
		variation = m_pData->variation.getFrame(pTime);
		speed *= (1.0f + Math::RangeRandom(0.0f, variation));

		if( m_pData->attach2Bone )//绑定到骨骼
		{
			//位置
			Vector3 pivotPoint = m_pBone->getPivot();
			pivotPoint.x += Math::RangeRandom(-width,width);
			pivotPoint.z += Math::RangeRandom(-length,length);
			p.m_p = pivotPoint;
			p.m_originPos = p.m_p;
			//速度
			Matrix4 mtxY;
			mtxY.rotationX(Math::RangeRandom(0,latitude));
			Matrix4 mtxZ;
			mtxZ.rotationY(Math::RangeRandom(0,360));
			p.m_v = mtxZ * mtxY* Vector3(0.0f,1.0f,0.0f);
			p.m_v.normalize();
			p.m_v *= speed;	
			//重力
			p.m_gravity = m_pData->gravity.getFrame(pTime);
		}
		else//不绑定到骨骼
		{
			//位置
			Matrix4 mtx;
			if(m_pNode)
				mtx = m_pNode->getFullTransform() * mtxBone;
			else 
				mtx = mtxBone;
			Vector3 pivotPoint = m_pBone->getPivot();
			pivotPoint.x += Math::RangeRandom(-width,width);
			pivotPoint.z += Math::RangeRandom(-length,length);
			p.m_p = mtx * pivotPoint;
			p.m_originPos = p.m_p;
			//速度
			Matrix4 mtxY;
			mtxY.rotationX(Math::RangeRandom(0,latitude));
			Matrix4 mtxZ;
			mtxZ.rotationY(Math::RangeRandom(0,360));
			p.m_v = mtxZ * mtxY* Vector3(0.0f,1.0f,0.0f);
			Quaternion q1 = mtxBone.extractQuaternion();
			p.m_v = q1 * p.m_v;
			if( m_pNode)
			{
				Quaternion q2 = m_pNode->getFullTransform().extractQuaternion();
				p.m_v = q2 * p.m_v;
			}
			p.m_v.normalize();
			p.m_v *= speed;		
			//重力
			p.m_gravity = m_pData->gravity.getFrame(pTime);
		}

		p.m_life = m_pData->lifeSpan;
		p.m_currentFrame = 0;
		if(m_pData->head)
		{
			p.m_numHeadFrames = static_cast<int>((m_pData->headLifeSpan.y - m_pData->headLifeSpan.x + 1.0f) * m_pData->headLifeSpan.z);
			p.m_numHeadDecayFrames = static_cast<int>((m_pData->headDecay.y - m_pData->headDecay.x + 1.0f) * m_pData->headDecay.z);
		}
		else
		{
			p.m_numHeadFrames = 0;
			p.m_numHeadDecayFrames = 0;
		}

		if(m_pData->tail)
		{
			p.m_numTailFrames = static_cast<int>((m_pData->tailLifeSpan.y - m_pData->tailLifeSpan.x + 1.0f) * m_pData->tailLifeSpan.z);
			p.m_numTailDecayFrames = static_cast<int>((m_pData->tailDecay.y - m_pData->tailDecay.x + 1.0f) * m_pData->tailDecay.z);
		}
		else
		{
			p.m_numTailFrames = 0;
			p.m_numTailDecayFrames = 0;
		}
		if (0 == m_pData->nIntervalTime)
		{
			p.m_fSelfRotAngle = m_pData->nAngle;
		}
		p.update(tickDelta,pRenderSystem);


		//真正产生粒子
		if( m_vParticles.empty() )
		{
			m_vParticles.push_back(p);
		}
		else
		{
			Particle & p0 = m_vParticles.back();
			float fDist = ( p.m_p - p0.m_p).length();
			if( m_pNode ) 
				fDist =fDist /  m_pNode->getScale().x;
			float spawnNum = fDist * m_pData->fSpaceUint;
			if( spawnNum > 1.0f)
			{
				int toSpawn = xs::Math::Floor(spawnNum);
				float ratio = 1.0f / static_cast<float>(toSpawn);
				Vector3 vPosDelta = p.m_p - p0.m_p;
				Vector3 vOriPosDelta = p.m_originPos - p0.m_originPos;
				Vector3 vSpeedDelta = p.m_v - p0.m_v;
				float fLifeDelta = p.m_life - p0.m_life;
				float fgravityDelta = p.m_gravity - p0.m_gravity;
				float fscale = p.m_scale - p0.m_scale;
				for( int i =0; i< toSpawn; ++i)
				{
					float r = ratio * static_cast<float>(i+1);
					p.m_p = p0.m_p + vPosDelta * r;
					p.m_originPos = p0.m_originPos + vOriPosDelta * r;
					p.m_v  = p0.m_v + vSpeedDelta * r;
					p.m_life = p0.m_life + fLifeDelta *r;
					p.m_gravity = p0.m_gravity + fgravityDelta *r;
					p.m_scale = p0.m_scale + fscale*r;
					m_vParticles.push_back(p);
				}
			}
		}
	}


	void ParticleSystem::update(const AnimationTime *pTime,float tickDelta,IRenderSystem* pRenderSystem)
	{
		//更新资源
		initializeResource(pRenderSystem);

		if (s_bPause)
		{
			return;
		}

		//检验
		if(!m_pBone)return;		

		//更新粒子
		std::list<Particle>::iterator begin = m_vParticles.begin();
		std::list<Particle>::iterator end = m_vParticles.end();
		while(begin != end)
		{
			Particle& p = (*begin);
			if(p.update(tickDelta,pRenderSystem)) ++begin;
			else m_vParticles.erase(begin++);
		}

		//产生粒子
		const Matrix4& mtxBone = m_pBone->getFullTransform();
		if(m_pData->visibility.getFrame(pTime) > 0.0f && m_bGenerate)
		{
			if( !m_pData->bEmmitTime2Space ) //时间上均匀
				generateParticleByTime( pTime, tickDelta, pRenderSystem);
			else  //在空间上均匀
				generateParticleBySpace(pTime, tickDelta, pRenderSystem);			
		}

		//将多余的粒子删除
		if( m_vParticles.size() > s_MaxVertices / 4 )
		{
			uint deletecount = m_vParticles.size() - s_MaxVertices/4;
			ParticleContainerIterator it = m_vParticles.begin();
			for( uint i=0; i< deletecount; ++i)
			{
				it = m_vParticles.erase(it);
			}
		}
		return;
	}

	void ParticleSystem::saveContext(IRenderSystem *pRenderSystem)
	{
		m_lightingEnabled = pRenderSystem->isLightingEnabled();
		pRenderSystem->getSceneBlending(m_sbfSrc,m_sbfDst);
		m_alphaCheck = pRenderSystem->alphaCheckEnabled();
		pRenderSystem->getAlphaFunction(m_alphaFunc,m_alphaCheckRef);
		m_cullingMode = pRenderSystem->getCullingMode();
		m_bDepthCheck = pRenderSystem->isDepthBufferCheckEnabled();
		m_bFogEnabled = pRenderSystem->isFogEnabled();
		m_bDepthWrite = pRenderSystem->isDepthBufferWriteEnabled();
	}

	void ParticleSystem::loadContext(IRenderSystem *pRenderSystem)
	{
		pRenderSystem->setLightingEnabled(m_lightingEnabled);
		pRenderSystem->setTexture(0,0);
		pRenderSystem->setSceneBlending(m_sbfSrc,m_sbfDst);
		pRenderSystem->setAlphaCheckEnabled(m_alphaCheck);
		pRenderSystem->setAlphaFunction(m_alphaFunc,m_alphaCheckRef);
		pRenderSystem->setCullingMode(m_cullingMode);
		pRenderSystem->setDepthBufferCheckEnabled(m_bDepthCheck);
		pRenderSystem->enableFog(m_bFogEnabled);
		pRenderSystem->setDepthBufferWriteEnabled(m_bDepthWrite);
	}

	void ParticleSystem::setContext(IRenderSystem * pRenderSystem)
	{
		if( NULL == pRenderSystem)
			return;

		if(m_pData->unshaded)
			pRenderSystem->setLightingEnabled(false);
		//由于fog没什么影响且消耗性能，故禁止
		//if(m_pData->unfogged)
		//	pRenderSystem->enableFog(false);
		pRenderSystem->enableFog(false);

		pRenderSystem->setCullingMode(CULL_NONE);
		pRenderSystem->setTexture(0,m_pTexture);

		switch(m_pData->filterMode)
		{
		case BM_OPAQUE:
			pRenderSystem->setSceneBlending(SBF_ONE,SBF_ZERO);
			pRenderSystem->setAlphaCheckEnabled(false);
			break;
		case BM_TRANSPARENT:
			pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);
			pRenderSystem->setAlphaCheckEnabled(true);
			pRenderSystem->setAlphaFunction(CMPF_GREATER,(uchar)0xBE);
			break;
		case BM_ALPHA_BLEND:
			pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);
			pRenderSystem->setAlphaCheckEnabled(false);
			break;
		case BM_ADDITIVE:
			pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE);
			pRenderSystem->setAlphaCheckEnabled(false);
			break;
		case BM_ADDITIVE_ALPHA:
			pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE);
			pRenderSystem->setAlphaCheckEnabled(false);
			break;
		case BM_MODULATE:
			pRenderSystem->setSceneBlending(SBF_ZERO,SBF_SOURCE_COLOR);
			pRenderSystem->setAlphaCheckEnabled(false);
			break;
		}

		pRenderSystem->setDepthBufferCheckEnabled(true);
		pRenderSystem->setDepthBufferWriteEnabled(false);

		//增强亮度
		if( m_pData->unshaded  && m_pData->bEnhanceValue )
		{
		}	
	}

	void ParticleSystem::finalizeResource()
	{
		safeRelease(m_pTexture);//释放纹理
		safeRelease(m_pVB);//释放VBO
		safeRelease(m_pTexVB);//纹理VBO
		safeRelease(m_pDiffuseVB);//diffuse颜色VBO
		safeRelease(m_pIB);	//释放Index Buffer
	}

	bool ParticleSystem::initializeResource(IRenderSystem * pRenderSystem)
	{
		if( NULL == pRenderSystem )
			return false;

		//创建纹理
		if( NULL == m_pTexture)
		{
			if( !m_bInitializeTexture)
			{
				m_pTexture = pRenderSystem->getTextureManager()->createTextureFromFile(m_pData->textureFilename);
				m_bInitializeTexture = true;
			}
			if( NULL == m_pTexture) 
				return false;
		}		

		//创建位置 VBO
		if( NULL == m_pVB)
		{
			m_pVB = pRenderSystem->getBufferManager()->createVertexBuffer(12, s_MaxVertices, BU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
			if( NULL == m_pVB)  return false;
		}

		//创建纹理 VBO
		if( NULL == m_pTexVB)
		{
			m_pTexVB = pRenderSystem->getBufferManager()->createVertexBuffer(8, s_MaxVertices, BU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
			if( NULL == m_pTexVB)  return false;
		}

		//创建diffuse 颜色 VBO
		if( NULL == m_pDiffuseVB)
		{
			m_pDiffuseVB = pRenderSystem->getBufferManager()->createVertexBuffer(4, s_MaxVertices, BU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
			if( NULL == m_pDiffuseVB)  return false;
		}

		//创建IBO
		if( NULL == m_pIB)
		{
			m_pIB = pRenderSystem->getBufferManager()->createIndexBuffer(IT_16BIT, (s_MaxVertices/4) * 6, BU_STATIC_WRITE_ONLY);
			if( NULL == m_pIB) return false;
			//预先将数据写入index buffer
			ushort pIndices[(s_MaxVertices/4) * 6];
			uint maxQuadsCount = s_MaxVertices/4;
			for( uint i = 0; i < maxQuadsCount; ++i)
			{
				uint startpos = i * 6;
				uint startindex = i * 4;
				//第一个三角形
				pIndices[startpos+ 0] = startindex + 0;
				pIndices[startpos+ 1] = startindex + 1;
				pIndices[startpos+ 2] = startindex + 2;
				//第二个三角形
				pIndices[startpos+ 3] = startindex + 0;
				pIndices[startpos+ 4] = startindex + 2;
				pIndices[startpos+ 5] = startindex + 3;
			}
			m_pIB->writeData(0, sizeof( pIndices), pIndices, false);
		}
		
		return true;
	}

	void ParticleSystem::renderVBO(IRenderSystem * pRenderSystem)
	{
		if( NULL == pRenderSystem )
			return;

		//更新VBO
		m_uiPreviousQuads = m_vParticles.size()< s_MaxVertices/4 ? m_vParticles.size():s_MaxVertices/4;
		if( m_uiPreviousQuads <= 0 ) return;//不需要更新vbo
		std::list<Particle>::reverse_iterator rit;


		//写位置
		xs::Matrix4 mtxInverseModelView = pRenderSystem->getModelViewMatrix().inverse();//用于计算billboard
		Vector3 _camera = mtxInverseModelView * Vector3(0,0,0);
		Vector3 _up = (mtxInverseModelView*Vector3(0,1,0)) - _camera;
		_up.normalize();
		Vector3 _right =(mtxInverseModelView*Vector3(1,0,0))- _camera;
		_right.normalize();
		Vector3 _look = _right.crossProduct(_up);
		_look.normalize();

		if( m_pData->attach2Bone)
		{	
			xs::Matrix4 mtxNodeFullTrans = xs::Matrix4::IDENTITY;			
			if( m_pBone )
			{
				if (m_pData->bNodeInitMatrix)
				{
					mtxNodeFullTrans = m_pBone->getFullTransform() * m_pBone->getInitMatrix(); 
				}
				else
				{
					mtxNodeFullTrans = m_pBone->getFullTransform();
				}
			}
				
			if( m_pNode )
				 mtxNodeFullTrans = m_pNode->getFullTransform()*mtxNodeFullTrans;

			if( m_pData->head )
			{
				float posBuffer[s_MaxVertices*3];
				float * pPosBuffer = &posBuffer[0];
				rit = m_vParticles.rbegin();

				for( uint i=0; i < m_uiPreviousQuads; ++i)
				{
					Quaternion q;
					q.FromAngleAxis(rit->m_fSelfRotAngle, _look);

					xs::Vector3 center = mtxNodeFullTrans * rit->m_p;
					Vector3 pos[4];
					pos[0] = Vector3(center + q * (_right + _up) * -rit->m_scale);
					pos[1] = Vector3(center + q * (_right - _up) * rit->m_scale);
					pos[2] = Vector3(center + q * (_right + _up) * rit->m_scale);
					pos[3] = Vector3(center + q * (_right - _up) * -rit->m_scale);

					for( uint j = 0; j < 4; ++j)
					{
						*pPosBuffer++ = pos[j].x;
						*pPosBuffer++ = pos[j].y;
						*pPosBuffer++ = pos[j].z;
					}
					++rit;
				}
				m_pVB->writeData(0,m_uiPreviousQuads * 4 * 3 *4, posBuffer, true);
			}
			else
			{
				float posBuffer[s_MaxVertices*3];
				float * pPosBuffer = &posBuffer[0];
				rit = m_vParticles.rbegin();

				for( uint i=0; i < m_uiPreviousQuads; ++i)
				{
					Quaternion q;
					q.FromAngleAxis(rit->m_fSelfRotAngle, _look);

					xs::Vector3 oricenter = mtxNodeFullTrans * rit->m_originPos;
					xs::Vector3 curcenter = mtxNodeFullTrans * rit->m_p;
					Vector3 pos[4];
					pos[0] = Vector3(oricenter + q * (_right + _up) * -rit->m_scale);
					pos[1] = Vector3(oricenter + q * (_right - _up) * rit->m_scale);
					pos[2] = Vector3(curcenter + q * (_right + _up) * rit->m_scale);
					pos[3] = Vector3(curcenter + q * (_right - _up) * -rit->m_scale);

					for( uint j = 0; j < 4; ++j)
					{
						*pPosBuffer++ = pos[j].x;
						*pPosBuffer++ = pos[j].y;
						*pPosBuffer++ = pos[j].z;
					}

					++rit;
				}
				m_pVB->writeData(0,m_uiPreviousQuads * 4 * 3 *4, posBuffer, true);

			}
		}
		else
		{
			if( m_pData->head )
			{
				float posBuffer[s_MaxVertices*3];
				float * pPosBuffer = &posBuffer[0];
				std::list<Particle>::reverse_iterator rit = m_vParticles.rbegin();
				for( uint i=0; i < m_uiPreviousQuads; ++i)
				{
					Quaternion q;
					q.FromAngleAxis(rit->m_fSelfRotAngle, _look);

					Vector3 pos[4];
					pos[0] = Vector3(rit->m_p + q * (_right + _up) * -rit->m_scale);
					pos[1] = Vector3(rit->m_p + q * (_right - _up) * rit->m_scale);
					pos[2] = Vector3(rit->m_p + q * (_right + _up) * rit->m_scale);
					pos[3] = Vector3(rit->m_p + q * (_right - _up) * -rit->m_scale);

					for( uint j = 0; j < 4; ++j)
					{
						*pPosBuffer++ = pos[j].x;
						*pPosBuffer++ = pos[j].y;
						*pPosBuffer++ = pos[j].z;
					}
					++rit;
				}
				m_pVB->writeData(0,m_uiPreviousQuads * 4 * 3 * 4, posBuffer, true);	
			}
			else
			{
				float posBuffer[s_MaxVertices*3];
				float * pPosBuffer = &posBuffer[0];
				std::list<Particle>::reverse_iterator rit = m_vParticles.rbegin();
				for( uint i=0; i < m_uiPreviousQuads; ++i)
				{
					Quaternion q;
					q.FromAngleAxis(rit->m_fSelfRotAngle, _look);

					Vector3 pos[4];
					pos[0] = Vector3(rit->m_originPos + q * (_right + _up) * -rit->m_scale);
					pos[1] = Vector3(rit->m_originPos + q * (_right - _up) * rit->m_scale);
					pos[2] = Vector3(rit->m_p + q * (_right + _up) * rit->m_scale);
					pos[3] = Vector3(rit->m_p + q * (_right - _up) * -rit->m_scale);

					for( uint j = 0; j < 4; ++j)
					{
						*pPosBuffer++ = pos[j].x;
						*pPosBuffer++ = pos[j].y;
						*pPosBuffer++ = pos[j].z;
					}
					++rit;
				}
				m_pVB->writeData(0,m_uiPreviousQuads * 4 * 3 * 4, posBuffer, true);
			}	
		}

		//写纹理
		float texBuffer[s_MaxVertices*2];
		float * pTexBuffer = &texBuffer[0];
		rit = m_vParticles.rbegin();
		for( uint i=0; i< m_uiPreviousQuads; ++i)
		{
			for( uint j = 0; j < 4; ++j)
			{
				*pTexBuffer++ = rit->m_t[j].x;
				*pTexBuffer++ = rit->m_t[j].y;
			}
			++rit;
		}
		m_pTexVB->writeData(0, m_uiPreviousQuads * 4 * 2 * 4, texBuffer, true);

		//写颜色
		uint diffuseBuffer[s_MaxVertices];
		uchar * pDiffuseBuffer = (uchar*) (&diffuseBuffer[0]);
		rit = m_vParticles.rbegin();
		if( pRenderSystem->getRenderSystemType() == RS_D3D9 )//d3d9 rendersystem
		{
			for( uint i=0; i < m_uiPreviousQuads; ++i)
			{
				uchar r=static_cast<uint>( rit->m_fullColor.r * 255)  &0x00ff;
				uchar g=static_cast<uint>( rit->m_fullColor.g * 255)  &0x00ff;
				uchar b=static_cast<uint>( rit->m_fullColor.b * 255)  &0x00ff;
				uchar a=static_cast<uint>( rit->m_fullColor.a * 255)  &0x00ff;
				for( uint j = 0; j < 4; ++j)
				{
					//这里需要验证颜色的顺序
					*pDiffuseBuffer++ = b;
					*pDiffuseBuffer++ = g;
					*pDiffuseBuffer++ = r;
					*pDiffuseBuffer++ = a;
				}
				++rit;
			}	
		}
		else//ogl render system
		{
			for( uint i=0; i < m_uiPreviousQuads; ++i)
			{
				uchar r=static_cast<uint>( rit->m_fullColor.r * 255)  &0x00ff;
				uchar g=static_cast<uint>( rit->m_fullColor.g * 255)  &0x00ff;
				uchar b=static_cast<uint>( rit->m_fullColor.b * 255)  &0x00ff;
				uchar a=static_cast<uint>( rit->m_fullColor.a * 255)  &0x00ff;
				for( uint j = 0; j < 4; ++j)
				{
					*pDiffuseBuffer++ = r;
					*pDiffuseBuffer++ = g;
					*pDiffuseBuffer++ = b;
					*pDiffuseBuffer++ = a;
				}
				++rit;
			}			
		}
		m_pDiffuseVB->writeData(0, m_uiPreviousQuads * 4 * 1 * 4, diffuseBuffer, true);


		//绘制
		if( m_uiPreviousQuads > 0 )
		{
			IShaderProgram* pShader = pRenderSystem->getShaderProgram(ESP_V3_UV_C);
			pRenderSystem->bindCurrentShaderProgram(pShader,true);

			pRenderSystem->setNormalVertexBuffer(0);
			pRenderSystem->setVertexVertexBuffer(m_pVB,0);
			pRenderSystem->setTexcoordVertexBuffer(0,m_pTexVB, 0);
			pRenderSystem->setDiffuseVertexBuffer(m_pDiffuseVB, 0);
			pRenderSystem->setIndexBuffer( m_pIB);
			pRenderSystem->drawIndexedPrimitive(PT_TRIANGLES, m_uiPreviousQuads * 6, IT_16BIT);
			pRenderSystem->setVertexVertexBuffer(0);
			pRenderSystem->setTexcoordVertexBuffer(0,0);
			pRenderSystem->setDiffuseVertexBuffer(0);
		}
	}

	void ParticleSystem::renderManual(IRenderSystem * pRenderSystem)
	{
		if( NULL == pRenderSystem )
			return;

		Color4 c = pRenderSystem->getColor();
		if(  m_pData->attach2Bone )
		{
			xs::Matrix4 mtxNodeFullTrans = xs::Matrix4::IDENTITY;
			if( m_pNode )
				mtxNodeFullTrans = m_pNode->getFullTransform();

			xs::Matrix4 mtxInverseModelView = xs::Matrix4::IDENTITY;
			xs::Matrix4 mtxCurModelView = pRenderSystem->getModelViewMatrix();
			mtxCurModelView = mtxCurModelView * mtxNodeFullTrans;
			mtxInverseModelView = mtxCurModelView.inverse();

			if( m_pData->head )
			{
				std::list<Particle>::iterator begin = m_vParticles.begin();
				std::list<Particle>::iterator end = m_vParticles.end();

				while(begin != end)
				{
					Particle& p = (*begin);

					xs::Vector3 center = mtxNodeFullTrans * p.m_p;
					Vector3 look;
					Vector3 _up;
					Vector3 camera = mtxInverseModelView * Vector3(0,0,0);
					look = (camera - center);
					look.normalize();
					_up = (mtxInverseModelView * Vector3(0,1,0)) - camera;
					_up.normalize();
					Vector3 _right = _up.crossProduct(look);
					_right.normalize();
					_up = look.crossProduct(_right);
					_up.normalize();

					Vector3 pos[4];
					pos[0] = Vector3(center + (_right + _up) * -p.m_scale);
					pos[1] = Vector3(center + (_right - _up) * p.m_scale);
					pos[2] = Vector3(center + (_right + _up) * p.m_scale);
					pos[3] = Vector3(center + (_right - _up) * -p.m_scale);


					pRenderSystem->setColor(p.m_fullColor);
					pRenderSystem->beginPrimitive(PT_QUADS);

					pRenderSystem->setTexcoord(p.m_t[0]);
					pRenderSystem->sendVertex( pos[0]);
					pRenderSystem->setTexcoord(p.m_t[1]);
					pRenderSystem->sendVertex( pos[1] );
					pRenderSystem->setTexcoord(p.m_t[2]);
					pRenderSystem->sendVertex( pos[2] );
					pRenderSystem->setTexcoord(p.m_t[3]);
					pRenderSystem->sendVertex( pos[3] );

					pRenderSystem->endPrimitive();
					++begin;
				}		
			}
			else
			{
				std::list<Particle>::iterator begin = m_vParticles.begin();
				std::list<Particle>::iterator end = m_vParticles.end();

				while(begin != end)
				{
					Particle& p = (*begin);

					xs::Vector3 oricenter = mtxNodeFullTrans * p.m_originPos;
					xs::Vector3 curcenter = mtxNodeFullTrans * p.m_p;
					Vector3 look;
					Vector3 _up;
					Vector3 camera = mtxInverseModelView * Vector3(0,0,0);
					look = (camera - curcenter);
					look.normalize();
					_up = (mtxInverseModelView * Vector3(0,1,0)) - camera;
					_up.normalize();
					Vector3 _right = _up.crossProduct(look);
					_right.normalize();
					_up = look.crossProduct(_right);
					_up.normalize();

					Vector3 pos[4];
					pos[0] = Vector3(oricenter + (_right + _up) * -p.m_scale);
					pos[1] = Vector3(oricenter + (_right - _up) * p.m_scale);
					pos[2] = Vector3(curcenter + (_right + _up) * p.m_scale);
					pos[3] = Vector3(curcenter + (_right - _up) * -p.m_scale);

					pRenderSystem->setColor(p.m_fullColor);
					pRenderSystem->beginPrimitive(PT_QUADS);

					pRenderSystem->setTexcoord(p.m_t[0]);
					pRenderSystem->sendVertex(pos[0]);
					pRenderSystem->setTexcoord(p.m_t[1]);
					pRenderSystem->sendVertex(pos[1]);
					pRenderSystem->setTexcoord(p.m_t[2]);
					pRenderSystem->sendVertex(pos[2]);
					pRenderSystem->setTexcoord(p.m_t[3]);
					pRenderSystem->sendVertex(pos[3]);

					pRenderSystem->endPrimitive();

					++begin;
				}
			}
		}
		else
		{
			xs::Matrix4 mtxInverseModelView = xs::Matrix4::IDENTITY;
			xs::Matrix4 mtxCurModelView = pRenderSystem->getModelViewMatrix();
			mtxInverseModelView = mtxCurModelView.inverse();

			if( m_pData->head)
			{
				std::list<Particle>::iterator begin = m_vParticles.begin();
				std::list<Particle>::iterator end = m_vParticles.end();
				while(begin != end)
				{
					Particle& p = (*begin);

					Vector3 look;
					Vector3 _up;
					if(ParticleSystem::m_b25d)
					{
						look = Vector3(-1.224745f,1,1.224745f);
						_up = Vector3(0,1,0);
					}
					else
					{
						Vector3 camera = mtxInverseModelView * Vector3(0,0,0);
						look = (camera - p.m_p);
						look.normalize();
						_up = (mtxInverseModelView * Vector3(0,1,0)) - camera;
						_up.normalize();
					}
					Vector3 _right = _up.crossProduct(look);
					_right.normalize();
					_up = look.crossProduct(_right);
					_up.normalize();

					Vector3 pos[4];
					pos[0] = Vector3(p.m_p + (_right + _up) * -p.m_scale);
					pos[1] = Vector3(p.m_p + (_right - _up) * p.m_scale);
					pos[2] = Vector3(p.m_p + (_right + _up) * p.m_scale);
					pos[3] = Vector3(p.m_p + (_right - _up) * -p.m_scale);

					pRenderSystem->setColor(p.m_fullColor);
					pRenderSystem->beginPrimitive(PT_QUADS);
					pRenderSystem->setTexcoord(p.m_t[0]);
					pRenderSystem->sendVertex(pos[0]);
					pRenderSystem->setTexcoord(p.m_t[1]);
					pRenderSystem->sendVertex(pos[1]);
					pRenderSystem->setTexcoord(p.m_t[2]);
					pRenderSystem->sendVertex(pos[2]);
					pRenderSystem->setTexcoord(p.m_t[3]);
					pRenderSystem->sendVertex(pos[3]);
					pRenderSystem->endPrimitive();
					++begin;
				}
			}
			else
			{
				std::list<Particle>::iterator begin = m_vParticles.begin();
				std::list<Particle>::iterator end = m_vParticles.end();
				while(begin != end)
				{
					Particle& p = (*begin);

					Vector3 look;
					Vector3 _up;
					if(ParticleSystem::m_b25d)
					{
						look = Vector3(-1.224745f,1,1.224745f);
						_up = Vector3(0,1,0);
					}
					else
					{
						Vector3 camera = mtxInverseModelView * Vector3(0,0,0);
						look = (camera - p.m_p);
						look.normalize();
						_up = (mtxInverseModelView * Vector3(0,1,0)) - camera;
						_up.normalize();
					}
					Vector3 _right = _up.crossProduct(look);
					_right.normalize();
					_up = look.crossProduct(_right);
					_up.normalize();

					Vector3 pos[4];
					pos[0] = Vector3(p.m_originPos + (_right + _up) * -p.m_scale);
					pos[1] = Vector3(p.m_originPos + (_right - _up) * p.m_scale);
					pos[2] = Vector3(p.m_p + (_right + _up) * p.m_scale);
					pos[3] = Vector3(p.m_p + (_right - _up) * -p.m_scale);


					pRenderSystem->setColor(p.m_fullColor);
					pRenderSystem->beginPrimitive(PT_QUADS);
					pRenderSystem->setTexcoord(p.m_t[0]);
					pRenderSystem->sendVertex(pos[0]);
					pRenderSystem->setTexcoord(p.m_t[1]);
					pRenderSystem->sendVertex(pos[1]);
					pRenderSystem->setTexcoord(p.m_t[2]);
					pRenderSystem->sendVertex(pos[2]);
					pRenderSystem->setTexcoord(p.m_t[3]);
					pRenderSystem->sendVertex(pos[3]);
					pRenderSystem->endPrimitive();

					++begin;	
				}
			}
		}
		pRenderSystem->setColor(c);
	}
	

	//渲染的时候都使用vbo对象
	void ParticleSystem::render(IRenderSystem *pRenderSystem,bool useMtl)
	{
		PP_BY_NAME( "ParticleSystem::render" );

		if(!useMtl)return;
		if(m_vParticles.empty())
			return;

		///检查资源
		if( 0 == pRenderSystem
			|| 0 == m_pTexture
			|| 0 == m_pVB
			|| 0 == m_pTexVB
			|| 0 == m_pDiffuseVB
			|| 0 == m_pIB )
			return;

		//初始化资源
		if(!initializeResource(pRenderSystem) )
			return;

		//保存渲染状态
		saveContext(pRenderSystem);

		//设置渲染状态
		setContext(pRenderSystem);
	
		//绘制
#if 1
		if(s_UseVBO)
			renderVBO(pRenderSystem);
		else
			renderManual(pRenderSystem);
#else   //做要用来作测试。将m_pVB的顶点大小设为24，这边便可以用了

		//进行绘制
		if( s_UseVBO)
		{
			//开始绘制
			if( m_uiPreviousQuads > 0 )
			{

				pRenderSystem->setVertexVertexBuffer(m_pVB,0);
				pRenderSystem->setTexcoordVertexBuffer(0,m_pVB,12);
				pRenderSystem->setDiffuseVertexBuffer(m_pVB,20);
				pRenderSystem->setIndexBuffer( m_pIB);
				pRenderSystem->drawIndexedPrimitive(PT_TRIANGLES, m_uiPreviousQuads * 6, IT_16BIT);
				//pRenderSystem->drawPrimitive(PT_QUADS,0,m_uiPreviousQuads*4);//d3d9没有QUADS基本图元，改为TRIANGLES
				pRenderSystem->setVertexVertexBuffer(0);
				pRenderSystem->setTexcoordVertexBuffer(0,0);
				pRenderSystem->setDiffuseVertexBuffer(0);
			}
		}

		//更新vbo
		if(m_pData->head)
		{
			Color4 c = pRenderSystem->getColor();
			if( s_UseVBO)
			{
				//更新vbo
				m_uiPreviousQuads = m_vParticles.size()< s_MaxVertices/4? m_vParticles.size():s_MaxVertices/4;
				uint _i = 0;
				std::list<Particle>::reverse_iterator  rit = m_vParticles.rbegin();

				float * _pVB = (float*)m_pVB->lock(0,0,BL_DISCARD);
				if( !_pVB)
				{
					m_pVB->unlock();
					loadContext(pRenderSystem);
					return;
				}

				//用于粒子附着在骨骼上的顶点的位置的计算
				Matrix4 mtxFull(Matrix4::IDENTITY);
				Vector3 v0,v1,v2,v3;
				if( m_pNode) mtxFull = m_pNode->getFullTransform();
				Vector3 _v;

				Vector3 _vPos;
				uchar _r,_g,_b,_a;

				if( m_pData->attach2Bone)
				{
					for( _i=0; _i<m_uiPreviousQuads; ++_i)
					{
						_r=static_cast<uint>( rit->m_fullColor.r * 255)  &0x00ff;
						_g=static_cast<uint>( rit->m_fullColor.g * 255)  &0x00ff;
						_b=static_cast<uint>( rit->m_fullColor.b * 255)  &0x00ff;
						_a=static_cast<uint>( rit->m_fullColor.a * 255)  &0x00ff;
						for( uint _j =0; _j<4;++_j)
						{
							//position
							_v = mtxFull * rit->m_r[_j];
							*_pVB++ = _v.x; 
							*_pVB++ = _v.y;
							*_pVB++ = _v.z;
							//texture coord
							*_pVB++ = rit->m_t[_j].x;
							*_pVB++ = rit->m_t[_j].y;
							//color
							uchar * _pCB = (uchar*)(_pVB);
							*_pCB++ = _r;
							*_pCB++ = _g;
							*_pCB++ = _b;
							*_pCB++ = _a;						
							_pVB++;//increament _pVB						
						}
						++rit;
					}
					m_pVB->unlock();
				}
				else
				{
					for( _i=0; _i<m_uiPreviousQuads; ++_i)
					{
						_r=static_cast<uint>( rit->m_fullColor.r * 255)  &0x00ff;
						_g=static_cast<uint>( rit->m_fullColor.g * 255)  &0x00ff;
						_b=static_cast<uint>( rit->m_fullColor.b * 255)  &0x00ff;
						_a=static_cast<uint>( rit->m_fullColor.a * 255)  &0x00ff;
						for( uint _j =0; _j<4;++_j)
						{
							//position
							*_pVB++ = rit->m_r[_j].x; 
							*_pVB++ = rit->m_r[_j].y;
							*_pVB++ = rit->m_r[_j].z;
							//texture coord
							*_pVB++ = rit->m_t[_j].x;
							*_pVB++ = rit->m_t[_j].y;
							//color
							uchar * _pCB = (uchar*)(_pVB);
							*_pCB++ = _r;
							*_pCB++ = _g;
							*_pCB++ = _b;
							*_pCB++ = _a;						
							_pVB++;//increament _pVB						
						}
						++rit;
					}
					m_pVB->unlock();
				}	
			}
			else
			{
				uint _uit1 = xs::getTickCount();
				std::list<Particle>::iterator begin = m_vParticles.begin();
				std::list<Particle>::iterator end = m_vParticles.end();
				while(begin != end)
				{
					Particle& p = (*begin);
					pRenderSystem->setColor(p.m_fullColor);
					pRenderSystem->beginPrimitive(PT_QUADS);
					pRenderSystem->setTexcoord(p.m_t[0]);
					pRenderSystem->sendVertex(p.m_r[0]);
					pRenderSystem->setTexcoord(p.m_t[1]);
					pRenderSystem->sendVertex(p.m_r[1]);
					pRenderSystem->setTexcoord(p.m_t[2]);
					pRenderSystem->sendVertex(p.m_r[2]);
					pRenderSystem->setTexcoord(p.m_t[3]);
					pRenderSystem->sendVertex(p.m_r[3]);
					pRenderSystem->endPrimitive();

					++begin;
				}
				uint _uit2 = xs::getTickCount();
				uint _delta = _uit2 - _uit1;		
			}
			pRenderSystem->setColor(c);
		}
		else
		{
			Color4 c = pRenderSystem->getColor();
			if( s_UseVBO)
			{
				//更新vbo
				m_uiPreviousQuads = m_vParticles.size()< s_MaxVertices/4? m_vParticles.size():s_MaxVertices/4;
				uint _i = 0;
				std::list<Particle>::reverse_iterator  rit = m_vParticles.rbegin();

				float * _pVB = (float*)m_pVB->lock(0,0,BL_DISCARD);
				if( !_pVB )
				{
					if( _pVB ) m_pVB->unlock();
					loadContext(pRenderSystem);
					return;
				}

				//用于粒子附着在骨骼上的顶点的位置的计算
				Matrix4 mtxFull(Matrix4::IDENTITY);
				Vector3 v0,v1,v2,v3;
				Vector3 _v;
				if( m_pNode) mtxFull = m_pNode->getFullTransform();

				Vector3 _vPos;
				uchar _r,_g,_b,_a;

				if( m_pData->attach2Bone)
				{
					for( _i = 0; _i < m_uiPreviousQuads; ++_i)
					{
						_r=static_cast<uint>( rit->m_fullColor.r * 255)  &0x00ff;
						_g=static_cast<uint>( rit->m_fullColor.g * 255)  &0x00ff;
						_b=static_cast<uint>( rit->m_fullColor.b * 255)  &0x00ff;
						_a=static_cast<uint>( rit->m_fullColor.a * 255)  &0x00ff;
						for( uint _j=0; _j<4;++_j)
						{
							//position
							_v = mtxFull * rit->m_rt[_j];
							*_pVB++ = _v.x; 
							*_pVB++ = _v.y;
							*_pVB++ = _v.z;
							//texture coord
							*_pVB++ = rit->m_t[_j].x;
							*_pVB++ = rit->m_t[_j].y;
							//color
							uchar * _pCB = (uchar*)(_pVB);
							*_pCB++ = _r;
							*_pCB++ = _g;
							*_pCB++ = _b;
							*_pCB++ = _a;						
							_pVB++;//increament _pVB		
						}	
						++rit;
					}
					m_pVB->unlock();
				}
				else
				{
					for( _i = 0; _i < m_uiPreviousQuads; ++_i)
					{
						_r=static_cast<uint>( rit->m_fullColor.r * 255)  &0x00ff;
						_g=static_cast<uint>( rit->m_fullColor.g * 255)  &0x00ff;
						_b=static_cast<uint>( rit->m_fullColor.b * 255)  &0x00ff;
						_a=static_cast<uint>( rit->m_fullColor.a * 255)  &0x00ff;
						for( uint _j=0; _j<4;++_j)
						{
							//position
							*_pVB++ = rit->m_rt[_j].x; 
							*_pVB++ = rit->m_rt[_j].y;
							*_pVB++ = rit->m_rt[_j].z;
							//texture coord
							*_pVB++ = rit->m_t[_j].x;
							*_pVB++ = rit->m_t[_j].y;
							//color
							uchar * _pCB = (uchar*)(_pVB);
							*_pCB++ = _r;
							*_pCB++ = _g;
							*_pCB++ = _b;
							*_pCB++ = _a;						
							_pVB++;//increament _pVB		
						}	
						++rit;
					}
					m_pVB->unlock();
				}				
			}
			else
			{
				std::list<Particle>::iterator begin = m_vParticles.begin();
				std::list<Particle>::iterator end = m_vParticles.end();
				while(begin != end)
				{
					Particle& p = (*begin);
					pRenderSystem->setColor(p.m_fullColor);
					pRenderSystem->beginPrimitive(PT_QUADS);
					pRenderSystem->setTexcoord(p.m_t[0]);
					pRenderSystem->sendVertex(p.m_rt[0]);
					pRenderSystem->setTexcoord(p.m_t[1]);
					pRenderSystem->sendVertex(p.m_rt[1]);
					pRenderSystem->setTexcoord(p.m_t[2]);
					pRenderSystem->sendVertex(p.m_rt[2]);
					pRenderSystem->setTexcoord(p.m_t[3]);
					pRenderSystem->sendVertex(p.m_rt[3]);
					pRenderSystem->endPrimitive();

					++begin;
				}
			}
			pRenderSystem->setColor(c);
		}
#endif
		//恢复渲染状态
		loadContext(pRenderSystem);

	}

	int ParticleSystem::getRenderOrder()
	{
		switch(m_pData->filterMode)
		{
		case BM_OPAQUE:
			{
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
		}

		return RENDER_QUEUE_OPAQUE;
	}

	void ParticleSystem::reset()
	{
		m_vParticles.clear();
		m_uiPreviousQuads= 0;
	}

	void ParticleSystem::setColor(const ColorValue& color)
	{
		m_color = color;
	}

}