#include "stdafx.h"
#include "_ParticleSystem.h"
#include "_ParticleSystemEmitter.h"
#include "ModelNode.h"
#include "HelperFuncs.h"
#include "Bone.h"
#include <map>
#include <string>
using namespace std;

/*
2011/2/23 忽略了亮度增强和同步放大参数
*/


namespace xs
{
	bool MmParticleSystem::m_b25d = true;
	bool MmParticleSystem::s_bPause = false;
	const uint MmParticleSystem::s_MaxVertices(512);//最大顶点数,必须是4的倍数，因为绘制的是四边形
	const bool MmParticleSystem::s_UseVBO(true);//是否使用vbo对象

	template<class T>T lifeRamp(float life,float mid,const T &a,const T &b,const T &c)
	{
		if(life <= mid)
			return interpolate<T>(life / mid,a,b);
		else 
			return interpolate<T>((life - mid) / (1.0f - mid),b,c);
	}

	MmParticleSystem::MmParticleSystem()
	{
		m_pEmitter = 0;
		m_rem = 0;	
		m_random = Math::UnitRandom();


		m_pTexture = 0;
		m_pVB= 0;
		m_pTexVB = 0;
		m_pDiffuseVB  = 0;
		m_pIB = 0;
		m_uiPreviousQuads= 0;

		m_bInitializeTexture = false;
		m_bGenerate = true;
	}

	MmParticleSystem::~MmParticleSystem()
	{
		if(m_pEmitter)
		{
			m_pEmitter->release();
			m_pEmitter = 0;
		}

		finalizeResource();
	}



	void MmParticleSystem::set25d(bool b25d)
	{
		m_b25d = b25d;
	}

	void MmParticleSystem::setPause(bool bPause)
	{
		s_bPause = bPause;
	}

	int MmParticleSystem::__getCurParticleNum()
	{
		return m_particleList.size();
	}

	void MmParticleSystem::SetGenerateParticle(bool bGenerate)
	{
		m_bGenerate = bGenerate;
	}

	void MmParticleSystem::update(IRenderSystem * pRenderSystem, AnimationTime *pTime,float tickDelta,int rangeIndex)
	{
		//初始化资源
		if( pRenderSystem) initializeResource(pRenderSystem);

		if (s_bPause)
		{
			return;
		}


		//计算参数
		float timeFactor = tickDelta / 1000.0f;
		float grav = m_pData->gravity.getFrame(rangeIndex,pTime);		

		//构造粒子发生器
		if(!m_pEmitter)
		{
			m_pEmitter = ParticleSystemEmitterFactory::newEmitter(m_pData->emitterType,this);
		}

		//产生新的粒子
		if( m_pData->bEmmitTime2Space == false && m_pEmitter && m_bGenerate) //时间上均匀
		{	
			float rate = m_pData->rate.getFrame(rangeIndex,pTime);
			float spawnNum = (timeFactor * rate) + m_rem;
			if (spawnNum < 1.0f)
			{
				m_rem = spawnNum;
				if(m_rem < 0)
					m_rem = 0;
			}
			else
			{
				int toSpawn = xs::Math::Floor(spawnNum);
				m_rem = spawnNum - toSpawn;
				if( toSpawn < 1) 
					toSpawn = 1;

				for(int i = 0;i < toSpawn; i++)
				{
					MmParticle p = m_pEmitter->newParticle(rangeIndex,pTime);
					if (0 == m_pData->nIntervalTime)
					{
						p.m_fSelfRotAngle = m_pData->nAngle;
					}
					m_particleList.push_back(p);
				}
			}
		}
		else if( m_pEmitter && m_pData->bEmmitTime2Space && m_bGenerate) // 空间上均匀
		{
			//此时的rate用于判断是否产生粒子，而不用于判断产生的粒子数，产生的粒子数由
			//距离决定
			if( m_pData->rate.getFrame(rangeIndex,pTime) > 0.5f )
			{
				MmParticle p = m_pEmitter->newParticle(rangeIndex, pTime);
				if (0 == m_pData->nIntervalTime)
				{
					p.m_fSelfRotAngle = m_pData->nAngle;
				}

				if( m_particleList.empty() )
					m_particleList.push_back(p);
				else
				{
					MmParticle & p0 = m_particleList.back();
					float fDist = ( p.m_vPos - p0.m_vPos).length();
					if( m_pNode )
						fDist =fDist /  m_pNode->getScale().x;
					//暂时不考虑动画的结束，认为动画一致是循环的
					//m_rem这个参数没有用，因为是根据上一个粒子的位置来计算产生的粒子的。
					float spawnNum = fDist * m_pData->fSpaceUint;
					if( spawnNum > 1.0f ) 
					{
						int toSpawn = xs::Math::Floor(spawnNum);
						if( toSpawn < 1 )
							toSpawn = 1;
						Vector3 vPosDelta = p.m_vPos - p0.m_vPos;
						Vector3 vOriPosDelta = p.m_vOrigin - p0.m_vOrigin;
						Vector3 vDownDelta = p.m_vDown - p0.m_vDown;
						Vector3 vSpeedDelta = p.m_vSpeed - p0.m_vSpeed;
						float fMaxLife = p.m_fMaxLife - p0.m_fMaxLife;
						for( int i = 0; i< toSpawn; ++i)
						{
							MmParticle p1 = m_pEmitter->newParticle(rangeIndex, pTime);
							if (0 == m_pData->nIntervalTime)
							{
								p1.m_fSelfRotAngle = m_pData->nAngle;
							}

							float ratio = static_cast<float>(i+1)/static_cast<float>(toSpawn);
							p1.m_vPos = p0.m_vPos + vPosDelta* ratio;
							p1.m_vOrigin = p0.m_vOrigin + vOriPosDelta*ratio;
							p1.m_vDown = p0.m_vDown + vDownDelta * ratio;
							p1.m_vSpeed = p0.m_vSpeed + vSpeedDelta * ratio;
							p1.m_fMaxLife = p0.m_fMaxLife + fMaxLife * ratio;


							m_particleList.push_back(p1);
						}
					}
				}	
			}
		}	

		//删除超出范围的粒子
		if( m_particleList.size() > s_MaxVertices/4)
		{
			uint deletecount = m_particleList.size() - s_MaxVertices/4;
			MmParticleListIterator it = m_particleList.begin();
			for( uint i=0; i < deletecount; ++i)
			{
				it = m_particleList.erase(it);
			}
		}

		//更新粒子
		float speed = 1.0f;
		MmParticleList::iterator begin = m_particleList.begin();
		MmParticleList::iterator end = m_particleList.end();
		xs::Vector3 v3NodeScale = xs::Vector3::UNIT_SCALE;
		if(m_pNode)
		{
			Vector3 t;
			Quaternion q;
			xs::Matrix4 mtx = m_pNode->getFullTransform();
			mtx.resolve(t,q,v3NodeScale);
		}
		for(MmParticleList::iterator it = begin; it != end;)
		{
			MmParticle &p = *it;
			//更新速度
			p.m_vSpeed += p.m_vDown * grav * timeFactor;
			if(m_pData->p.slowdown > 0)
				speed = expf(-1.0f * m_pData->p.slowdown * p.m_fLife);

			//更新位置
			if( m_pBone && m_pData->attach2Bone != 0 )//绑定到骨骼
			{
				p.m_vPos += p.m_vSpeed * speed * timeFactor;
			}
			else//不绑定到骨骼
			{
				Vector3 s = v3NodeScale;
				p.m_vPos += (p.m_vSpeed * speed * timeFactor) * s;
			}	

			//更新其它参数
			p.m_fLife += timeFactor;
			float life = p.m_fLife / p.m_fMaxLife;
			//根据粒子的生命计算大小和颜色
			p.m_fSize = lifeRamp<float>(life,m_pData->p.mid,m_pData->p.sizes[0],m_pData->p.sizes[1],m_pData->p.sizes[2]);
			p.m_color = lifeRamp<ColorValue>(life,m_pData->p.mid,m_pData->p.colors[0],m_pData->p.colors[1],m_pData->p.colors[2]);
			
			// 计算粒子旋转四元数 zgz
			p.m_ftickDelta += tickDelta;	
			if ( 0 != m_pData->nIntervalTime && 0 != m_pData->nAngle)
			{
				p.m_fSelfRotAngle = (p.m_ftickDelta / m_pData->nIntervalTime) * m_pData->nAngle;
			}			
			
			// 选择分割的纹理 [5/18/2011 zgz]
			if (0 != m_pData->iteratorArea[0] || 0 != m_pData->iteratorArea[1])
			{
				float r = life >= 1.0f ? 1.0f : life;
				p.m_i32Tile = xs::interpolate(r, m_pData->iteratorArea[0], m_pData->iteratorArea[1]);
			}

			//删除死亡的粒子
			if(life >= 1.0f)
				m_particleList.erase(it++);
			else
				++it;
		}		
	}

	bool MmParticleSystem::initializeResource(IRenderSystem * pRenderSystem)
	{
		//初始化纹理
		if( 0 == m_pTexture )
		{
			if( !m_bInitializeTexture )
			{
				m_pTexture = pRenderSystem->getTextureManager()->createTextureFromFile(m_pData->texture);
				m_bInitializeTexture = true;
			}
			if( 0 == m_pTexture)
				return false;
		}
		
		//初始化位置vbo
		if( NULL == m_pVB )
		{
			m_pVB = pRenderSystem->getBufferManager()->createVertexBuffer(12, s_MaxVertices, BU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
			if( NULL == m_pVB ) return false;
		}

		//初始化texcoord vbo
		if( NULL == m_pTexVB)
		{
			m_pTexVB = pRenderSystem->getBufferManager()->createVertexBuffer(8, s_MaxVertices, BU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
			if( NULL == m_pTexVB) return false;
		}

		//初始化diffuse color vbo
		if( NULL == m_pDiffuseVB)
		{
			m_pDiffuseVB = pRenderSystem->getBufferManager()->createVertexBuffer(4, s_MaxVertices, BU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
			if( NULL == m_pDiffuseVB) return false;
		}

		//初始化index buffer
		if( NULL == m_pIB)
		{
			m_pIB = pRenderSystem->getBufferManager()->createIndexBuffer(IT_16BIT, (s_MaxVertices/4)*6, BU_STATIC_WRITE_ONLY);
			if( NULL == m_pIB) return false;
			ushort pIndices[(s_MaxVertices/4)*6];
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
			m_pIB->writeData(0, sizeof(pIndices), pIndices, false);
		}

		return true;
	}

	void MmParticleSystem::finalizeResource()
	{
		safeRelease( m_pTexture);
		safeRelease(m_pVB);
		safeRelease(m_pTexVB);
		safeRelease(m_pDiffuseVB);
		safeRelease(m_pIB);
	}

	void MmParticleSystem::renderVBO(IRenderSystem * pRenderSystem)
	{
		if( NULL == pRenderSystem )
			return;

		//更新VBO
		m_uiPreviousQuads = m_particleList.size()< s_MaxVertices/4? m_particleList.size():s_MaxVertices/4;
		if( m_uiPreviousQuads <= 0 ) return;//不需要更新vbo
		MmParticleList::reverse_iterator rit = m_particleList.rbegin();

		//写位置VBO
		Vector3 bv0,bv1,bv2,bv3;//计算 bv0,bv1,bv2,bv3
		Vector3 _right, _up, _look(0, 0, 1);

		Matrix4 mbb = Matrix4::IDENTITY;//bill board 矩阵
		if(m_pData->billboardType != BILLBOARD_NOT_USED)
		{
			// 这里计算的billboard有bug，当pos与camera重合时mbb为0, [5/6/2011 zgz]
			//mbb = calcBillboardMatrix(pRenderSystem->getModelViewMatrix(),m_pData->pos,m_pData->billboardType);			
			xs::Matrix4 mtxInverseModelView = pRenderSystem->getModelViewMatrix().inverse();//用于计算billboard

			Vector3 _camera = mtxInverseModelView * Vector3(0,0,0);
			_up = (mtxInverseModelView*Vector3(0,1,0)) - _camera;
			_up.normalize();
			_right = (mtxInverseModelView*Vector3(1,0,0))- _camera;
			_right.normalize();
			_look = _right.crossProduct(_up);
			_look.normalize();
		}
		
		if(m_b25d)
		{
			if(m_pData->type == 0 || m_pData->type == 2)
			{
				float f = 0.707106781f;
				Vector3 s = xs::Vector3::UNIT_SCALE;
				if(m_pNode)
				{
					Vector3 t;
					Quaternion q;
					Vector3 scale;
					xs::Matrix4 mtx = m_pNode->getFullTransform();
					mtx.resolve(t,q,scale);
					s = s * scale;
				}
				if(m_pData->billboardType == BILLBOARD_NOT_USED)
				{
					bv0 = Vector3(-f,0,+f) * s;
					bv1 = Vector3(+f,0,+f) * s;
					bv2 = Vector3(+f,0,-f) * s;
					bv3 = Vector3(-f,0,-f) * s;
				}
				else
				{
					// modelview矩阵是会变化的，不能硬编码 [5/6/2011 zgz]
// 					Vector3 look;
// 					Vector3 _up;
// 					look = Vector3(-1.224745f,1,1.224745f);
// 					_up = Vector3(0,1,0);
// 					Vector3 _right = _up.crossProduct(look);
// 					_right.normalize();
// 					_up = look.crossProduct(_right);
// 					_up.normalize();

					bv0 = Vector3(-(_right + _up) * f * s);
					bv1 = Vector3( (_right - _up) * f * s);
					bv2 = Vector3( (_right + _up) * f * s);
					bv3 = Vector3(-(_right - _up) * f * s);
				}
			}
			else if( m_pData->type == 1)
			{
				if(m_pData->billboardType != BILLBOARD_NOT_USED)
				{
					//以前的方式看不到
					//bv0 = mbb * Vector3(0,-1.0f,0);
					//bv1 = mbb * Vector3(0,+1.0f,0);
					
// 					bv0 = mbb * Vector3(1.0f,0.0f,1.0f);
// 					bv1 = mbb * Vector3(-1.0f,0.0f,-1.0f);
					bv0 = Vector3((_right + _look));
					bv1 = Vector3( -(_right + _look));
				}

				else
				{
					//以前的方式看不到
					//bv0 = Vector3(0,-1.0f,0);
					//bv1 = Vector3(0,+1.0f,0);
					bv0 = Vector3(1.0f,0.0f,1.0f);
					bv1 = Vector3(-1.0f,0.0f,-1.0f);
				}	
			}
		}
		else
		{
			if(m_pData->type == 0 || m_pData->type == 2)
			{
				float f = 0.707106781f; // sqrt(2)/2
				
				if(m_pData->billboardType != BILLBOARD_NOT_USED)
				{
// 					bv0 = mbb * Vector3(0,-f,+f);
// 					bv1 = mbb * Vector3(0,+f,+f);
// 					bv2 = mbb * Vector3(0,+f,-f);
// 					bv3 = mbb * Vector3(0,-f,-f);

					bv0 = Vector3(-(_right + _up) * f);
					bv1 = Vector3( (_right - _up) * f);
					bv2 = Vector3( (_right + _up) * f);
					bv3 = Vector3(-(_right - _up) * f);
				}
				else
				{
					bv0 = Vector3(-f,0,+f);
					bv1 = Vector3(+f,0,+f);
					bv2 = Vector3(+f,0,-f);
					bv3 = Vector3(-f,0,-f);
				}
			}
			else if( m_pData->type == 1)
			{
				if(m_pData->billboardType != BILLBOARD_NOT_USED)
				{
					//以前的方式看不到
					//bv0 = mbb * Vector3(0,-1.0f,0);
					//bv1 = mbb * Vector3(0,+1.0f,0);

// 					bv0 = mbb * Vector3(1.0f,0.0f,1.0f);
// 					bv1 = mbb * Vector3(-1.0f,0.0f,-1.0f);
					bv0 = Vector3((_right + _look));
					bv1 = Vector3( -(_right + _look));
				}
				else
				{
					//以前的方式看不到
					//bv0 = Vector3(0,-1.0f,0);
					//bv1 = Vector3(0,+1.0f,0);
					bv0 = Vector3(1.0f,0.0f,1.0f);
					bv1 = Vector3(-1.0f,0.0f,-1.0f);
				}
			}
		}		

		float posBuffer[s_MaxVertices * 3];
		float * pPosBuffer = &posBuffer[0];
		rit = m_particleList.rbegin();
		if(m_pData->type == 0 || m_pData->type == 2)
		{
			xs::Vector3 * vertexarray[] = { &bv0, &bv1,&bv2,&bv3};
			if( m_pBone && m_pData->attach2Bone )
			{
				Matrix4 mtxFull = Matrix4::IDENTITY;
				if (m_pData->bNodeInitMatrix)
				{
					mtxFull = m_pBone->getFullTransform() *  m_pBone->getInitMatrix();
				}
				else
				{
					mtxFull = m_pBone->getFullTransform();
				}
				 
				if( m_pNode )
					mtxFull = m_pNode->getFullTransform() * mtxFull;				
				
				for( uint i = 0; i < m_uiPreviousQuads; ++i)
				{
					Quaternion q;
					q.FromAngleAxis(rit->m_fSelfRotAngle, _look);

					Vector3 vTemp1 = mtxFull *(rit->m_vPos);
					for( uint j = 0; j < 4; ++j)
					{
						Vector3 v = vTemp1 +  q * (*vertexarray[j])*rit->m_fSize ;
						*pPosBuffer++ = v.x;
						*pPosBuffer++ = v.y;
						*pPosBuffer++ = v.z;
					}
					++rit;
				}
			}
			else
			{
				for( uint i = 0; i < m_uiPreviousQuads; ++i)
				{
					Quaternion q;
					q.FromAngleAxis(rit->m_fSelfRotAngle, _look);

					for( uint j = 0; j < 4; ++j)
					{
						Vector3 v = rit->m_vPos + q * (*vertexarray[j])*rit->m_fSize ;
						*pPosBuffer++ = v.x;
						*pPosBuffer++ = v.y;
						*pPosBuffer++ = v.z;
					}
					++rit;
				}
			}
		}
		else if( m_pData->type == 1)
		{
			if( m_pBone && m_pData->attach2Bone )
			{
				Matrix4 mtxFull = Matrix4::IDENTITY;
				if (m_pData->bNodeInitMatrix)
				{
					mtxFull = m_pBone->getFullTransform() *  m_pBone->getInitMatrix();
				}
				else
				{
					mtxFull = m_pBone->getFullTransform();
				}

				if(m_pNode)
					mtxFull = m_pNode->getFullTransform() * mtxFull;
				for( uint i = 0; i <m_uiPreviousQuads; ++i)
				{
					Quaternion q;
					q.FromAngleAxis(rit->m_fSelfRotAngle, _look);

					Vector3 vTemp1 = mtxFull *(rit->m_vPos);
					Vector3 vTemp2 = mtxFull *(rit->m_vOrigin);
					Vector3 vPos[4];
					vPos[0] = vTemp1 + q * bv0 * rit->m_fSize;
					vPos[1] = vTemp1 + q * bv1 * rit->m_fSize;
					vPos[2] = vTemp2 + q * bv0 * rit->m_fSize;
					vPos[3] = vTemp2 + q * bv1 * rit->m_fSize;
					for( uint j = 0; j < 4; ++j)
					{
						*pPosBuffer++ = vPos[j].x;
						*pPosBuffer++ = vPos[j].y;
						*pPosBuffer++ = vPos[j].z;
					}
					++rit;
				}
			}
			else
			{
				for( uint i = 0; i < m_uiPreviousQuads; ++i)
				{
					Quaternion q;
					q.FromAngleAxis(rit->m_fSelfRotAngle, _look);

					Vector3 vPos[4];
					vPos[0] = rit->m_vPos + q * bv0 * rit->m_fSize;
					vPos[1] = rit->m_vPos + q * bv1 * rit->m_fSize;
					vPos[2] = rit->m_vOrigin + q * bv0 * rit->m_fSize;
					vPos[3] = rit->m_vOrigin + q * bv1 * rit->m_fSize;
					for( uint j = 0; j < 4; ++j)
					{
						*pPosBuffer++ = vPos[j].x;
						*pPosBuffer++ = vPos[j].y;
						*pPosBuffer++ = vPos[j].z;	
					}
					++rit;
				}
			}
		}
		m_pVB->writeData(0, m_uiPreviousQuads * 4 *3 * 4, posBuffer, true);

		//写纹理坐标
		float texBuffer[s_MaxVertices * 2];
		float * pTexBuffer = &texBuffer[0];
		rit = m_particleList.rbegin();
		for( uint i=0; i <m_uiPreviousQuads; ++i)
		{
			for( uint j = 0; j < 4; ++j)
			{
				*pTexBuffer++ = m_pData->vTexcoords[rit->m_i32Tile].tc[j].x;
				*pTexBuffer++ = m_pData->vTexcoords[rit->m_i32Tile].tc[j].y;
			}
			++rit;
		}
		m_pTexVB->writeData(0, m_uiPreviousQuads * 4 * 2 * 4, texBuffer, true);

		//写颜色数据
		DWORD diffuseBuffer[s_MaxVertices];
		uchar * pDiffuseBuffer = (uchar*)(&diffuseBuffer[0]);
		rit = m_particleList.rbegin();
		if( pRenderSystem->getRenderSystemType() == RS_D3D9 ) //d3d9 render system
		{
			for( uint i = 0; i < m_uiPreviousQuads; ++i)
			{
				uchar r=static_cast<uint>( rit->m_color.r * 255)  &0x00ff;
				uchar g=static_cast<uint>( rit->m_color.g * 255)  &0x00ff;
				uchar b=static_cast<uint>( rit->m_color.b * 255)  &0x00ff;
				uchar a=static_cast<uint>( rit->m_color.a * 255)  &0x00ff;
				for( uint j = 0; j < 4; ++j)
				{
					*pDiffuseBuffer++ = b;
					*pDiffuseBuffer++ = g;
					*pDiffuseBuffer++ = r;
					*pDiffuseBuffer++ = a;
				}
				++rit;
			}
		} 
		else // ogl render system
		{
			for( uint i = 0; i < m_uiPreviousQuads; ++i)
			{
				uchar r=static_cast<uint>( rit->m_color.r * 255)  &0x00ff;
				uchar g=static_cast<uint>( rit->m_color.g * 255)  &0x00ff;
				uchar b=static_cast<uint>( rit->m_color.b * 255)  &0x00ff;
				uchar a=static_cast<uint>( rit->m_color.a * 255)  &0x00ff;
				for( uint j=0; j < 4; ++j)
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

		//开始绘制
		if( m_uiPreviousQuads > 0 )
		{
			IHighLevelShaderProgram* pShader = pRenderSystem->getShaderProgram(ESP_V3_UV_C);
			//pRenderSystem->bindCurrentShaderProgram(pShader,true);
			if(pShader)
			{
				pShader->bind();
				pShader->bindTransformMatrix(TMT_WORLD_VIEW_PROJECTION);
			}

			pRenderSystem->setNormalVertexBuffer(0);
			pRenderSystem->setVertexVertexBuffer(m_pVB,0);
			pRenderSystem->setTexcoordVertexBuffer(0,m_pTexVB,0);
			pRenderSystem->setDiffuseVertexBuffer(m_pDiffuseVB,0);
			pRenderSystem->setIndexBuffer(m_pIB);
			pRenderSystem->drawIndexedPrimitive( PT_TRIANGLES, m_uiPreviousQuads * 6, IT_16BIT);
			pRenderSystem->setVertexVertexBuffer(0,0);
			pRenderSystem->setTexcoordVertexBuffer(0,0,0);
			pRenderSystem->setDiffuseVertexBuffer(0,0);
			pRenderSystem->setIndexBuffer(0);
		}
	}

	void MmParticleSystem::renderManual(IRenderSystem * pRenderSystem)
	{
#if 0 //暂时不用这段代码，如果要用参考renderVBO来修改
		if( NULL == pRenderSystem)
			return;

		Matrix4 mbb;
		Vector3 bv0,bv1,bv2,bv3;
		if(m_pData->billboardType != BILLBOARD_NOT_USED)
		{
			mbb = calcBillboardMatrix(pRenderSystem->getModelViewMatrix(),m_pData->pos,m_pData->billboardType);
		}

		if(m_b25d)
		{
			float f = 0.707106781f;
			Vector3 s = Vector3(1,1,1);
			if(m_pNode)
			{
				Matrix4 mtx = m_pNode->getFullTransform();
				Vector3 t;
				Quaternion q;
				mtx.resolve(t,q,s);
			}
			if(m_pData->type == 0 || m_pData->type == 2)
			{
				if(m_pData->billboardType == BILLBOARD_NOT_USED)
				{
					bv0 = Vector3(-f,0,+f) * s;
					bv1 = Vector3(+f,0,+f) * s;
					bv2 = Vector3(+f,0,-f) * s;
					bv3 = Vector3(-f,0,-f) * s;
				}
				else
				{
					Vector3 look;
					Vector3 _up;
					look = Vector3(-1.224745f,1,1.224745f);
					_up = Vector3(0,1,0);
					Vector3 _right = _up.crossProduct(look);
					_right.normalize();
					_up = look.crossProduct(_right);
					_up.normalize();

					bv0 = Vector3(-(_right + _up) * f * s);
					bv1 = Vector3( (_right - _up) * f * s);
					bv2 = Vector3( (_right + _up) * f * s);
					bv3 = Vector3(-(_right - _up) * f * s);
				}

				pRenderSystem->beginPrimitive(PT_QUADS);
				MmParticleList::iterator begin = m_particleList.begin();
				MmParticleList::iterator end = m_particleList.end();
				size_t size = m_particleList.size();
				for(MmParticleList::iterator it = begin; it != end;++it)
				{
					MmParticle& p = (*it);
					pRenderSystem->setColor(p.m_color);
					pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[0]);
					pRenderSystem->sendVertex(p.m_vPos + bv0 * (p.m_fSize));
					pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[1]);
					pRenderSystem->sendVertex(p.m_vPos + bv1 * (p.m_fSize));
					pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[2]);
					pRenderSystem->sendVertex(p.m_vPos + bv2 * (p.m_fSize));
					pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[3]);
					pRenderSystem->sendVertex(p.m_vPos + bv3 * (p.m_fSize));
				}
				pRenderSystem->endPrimitive();
			}
			else if(m_pData->type == 1)
			{
				if(m_pData->billboardType != BILLBOARD_NOT_USED)
				{
					//以前的方式看不到
					//bv0 = mbb * Vector3(0,-1.0f,0);
					//bv1 = mbb * Vector3(0,+1.0f,0);
					bv0 = mbb * Vector3(1.0f,0.0f,1.0f);
					bv1 = mbb * Vector3(-1.0f,0.0f,-1.0f);
				}
				else
				{
					//以前的方式看不到
					//bv0 = Vector3(0,-1.0f,0);
					//bv1 = Vector3(0,+1.0f,0);
					bv0 = Vector3(1.0f,0.0f,1.0f);
					bv1 = Vector3(-1.0f,0.0f,-1.0f);
				}
				pRenderSystem->beginPrimitive(PT_QUADS);
				MmParticleList::iterator begin = m_particleList.begin();
				MmParticleList::iterator end = m_particleList.end();
				size_t size = m_particleList.size();
				for(MmParticleList::iterator it = begin; it != end;++it)
				{
					MmParticle& p = (*it);
					pRenderSystem->setColor(p.m_color);
					pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[0]);
					pRenderSystem->sendVertex(p.m_vPos + bv0 * p.m_fSize * s);
					pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[1]);
					pRenderSystem->sendVertex(p.m_vPos + bv1 * p.m_fSize * s);
					pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[2]);
					pRenderSystem->sendVertex(p.m_vOrigin + bv1 * p.m_fSize * s);
					pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[3]);
					pRenderSystem->sendVertex(p.m_vOrigin + bv0 * p.m_fSize * s);
				}
				pRenderSystem->endPrimitive();
			}
		}
		else
		{
			/*if(m_pData->billboardType != BILLBOARD_NOT_USED)
			{
				mbb = calcBillboardMatrix(pRenderSystem->getModelViewMatrix(),m_pData->pos,m_pData->billboardType);
			}*/

			if(m_pData->type == 0 || m_pData->type == 2)
			{
				float f = 0.707106781f; // sqrt(2)/2

				if(m_pData->billboardType != BILLBOARD_NOT_USED)
				{
					bv0 = mbb * Vector3(0,-f,+f);
					bv1 = mbb * Vector3(0,+f,+f);
					bv2 = mbb * Vector3(0,+f,-f);
					bv3 = mbb * Vector3(0,-f,-f);
				}
				else
				{
					bv0 = Vector3(-f,0,+f);
					bv1 = Vector3(+f,0,+f);
					bv2 = Vector3(+f,0,-f);
					bv3 = Vector3(-f,0,-f);
				}

				Vector3 s = Vector3(1,1,1);
				if(m_pNode)
				{
					Matrix4 mtx = m_pNode->getFullTransform();
					Vector3 t;
					Quaternion q;
					mtx.resolve(t,q,s);
				}
				pRenderSystem->beginPrimitive(PT_QUADS);
				MmParticleList::iterator begin = m_particleList.begin();
				MmParticleList::iterator end = m_particleList.end();
				size_t size = m_particleList.size();
				for(MmParticleList::iterator it = begin; it != end;++it)
				{
					MmParticle& p = (*it);
					pRenderSystem->setColor(p.m_color);
					pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[0]);
					pRenderSystem->sendVertex(p.m_vPos + bv0 * (p.m_fSize * s));
					pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[1]);
					pRenderSystem->sendVertex(p.m_vPos + bv1 * (p.m_fSize * s));
					pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[2]);
					pRenderSystem->sendVertex(p.m_vPos + bv2 * (p.m_fSize * s));
					pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[3]);
					pRenderSystem->sendVertex(p.m_vPos + bv3 * (p.m_fSize * s));
				}
				pRenderSystem->endPrimitive();
			}
			else if(m_pData->type == 1)
			{
				// particles from origin to position

				if(m_pData->billboardType != BILLBOARD_NOT_USED)
				{
					//以前的方式看不到
					//bv0 = mbb * Vector3(0,-1.0f,0);
					//bv1 = mbb * Vector3(0,+1.0f,0);
					bv0 = mbb * Vector3(1.0f,0.0f,1.0f);
					bv1 = mbb * Vector3(-1.0f,0.0f,-1.0f);
				}
				else
				{
					//以前的方式看不到
					//bv0 = Vector3(0,-1.0f,0);
					//bv1 = Vector3(0,+1.0f,0);
					bv0 = Vector3(1.0f,0.0f,1.0f);
					bv1 = Vector3(-1.0f,0.0f,-1.0f);
				}

				Vector3 s = Vector3(1,1,1);
				if(m_pNode)
				{
					Matrix4 mtx = m_pNode->getFullTransform();
					Vector3 t;
					Quaternion q;
					mtx.resolve(t,q,s);
				}

				pRenderSystem->beginPrimitive(PT_QUADS);
				MmParticleList::iterator begin = m_particleList.begin();
				MmParticleList::iterator end = m_particleList.end();
				size_t size = m_particleList.size();
				for(MmParticleList::iterator it = begin; it != end;++it)
				{
					MmParticle& p = (*it);
					pRenderSystem->setColor(p.m_color);
					pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[0]);
					pRenderSystem->sendVertex(p.m_vPos + bv0 * p.m_fSize * s);
					pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[1]);
					pRenderSystem->sendVertex(p.m_vPos + bv1 * p.m_fSize * s);
					pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[2]);
					pRenderSystem->sendVertex(p.m_vOrigin + bv1 * p.m_fSize * s);
					pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[3]);
					pRenderSystem->sendVertex(p.m_vOrigin + bv0 * p.m_fSize * s);
				}
				pRenderSystem->endPrimitive();
			}
		}
#endif
	}

	void MmParticleSystem::setContext(IRenderSystem * pRenderSystem)
	{
		pRenderSystem->setLightingEnabled(false);
		pRenderSystem->setCullingMode(CULL_NONE);
		pRenderSystem->setTexture(0,m_pTexture);

		switch(m_pData->blend)
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

		//考虑到颜色增强
		if( m_pData->bEnhanceValue)
		{
			//暂时不考虑	
		}
	}

	//如果没有使用vbo对象，暂时还没写绑定到骨骼的代码
	void MmParticleSystem::render(IRenderSystem* pRenderSystem,bool useMtl)
	{
		PP_BY_NAME( "MmParticleSystem::render" );

		//检测
		if(!useMtl)return;
		if(m_particleList.empty())return;

		//检测资源
		if( 0 == pRenderSystem
			|| 0 == m_pTexture
			|| 0 == m_pVB
			|| 0 == m_pTexVB
			|| 0 == m_pDiffuseVB
			|| 0 == m_pIB) return;
		
		//保存渲染状态
		saveContext(pRenderSystem);

		//设置渲染状态
		setContext(pRenderSystem);

#if 1
		if( s_UseVBO )
			renderVBO(pRenderSystem);
		else
			renderManual(pRenderSystem);
#else  //做要用来作测试。将m_pVB的顶点大小设为24，这边便可以用了

		if( s_UseVBO)
		{
			//开始绘制
			if( m_uiPreviousQuads > 0 )
			{		
				pRenderSystem->setVertexVertexBuffer(m_pVB,0);
				pRenderSystem->setTexcoordVertexBuffer(0,m_pVB,12);
				pRenderSystem->setDiffuseVertexBuffer(m_pVB,20);
				pRenderSystem->drawPrimitive(PT_QUADS,0,m_uiPreviousQuads*4);
				pRenderSystem->setVertexVertexBuffer(0,0);
				pRenderSystem->setTexcoordVertexBuffer(0,0,0);
				pRenderSystem->setDiffuseVertexBuffer(0,0);
			}
		}

		Matrix4 mbb;
		Vector3 bv0,bv1,bv2,bv3;
		if(m_pData->billboardType != BILLBOARD_NOT_USED)
		{
			mbb = calcBillboardMatrix(pRenderSystem->getModelViewMatrix(),m_pData->pos,m_pData->billboardType);
		}

		if(m_b25d)
		{
			float f = 0.707106781f;
			Vector3 s = Vector3(1,1,1);
			if(m_pNode)
			{
				Matrix4 mtx = m_pNode->getFullTransform();
				Vector3 t;
				Quaternion q;
				mtx.resolve(t,q,s);
			}
			if(m_pData->type == 0 || m_pData->type == 2)
			{
				if(m_pData->billboardType == BILLBOARD_NOT_USED)
				{
					bv0 = Vector3(-f,0,+f) * s;
					bv1 = Vector3(+f,0,+f) * s;
					bv2 = Vector3(+f,0,-f) * s;
					bv3 = Vector3(-f,0,-f) * s;
				}
				else
				{
					Vector3 look;
					Vector3 _up;
					look = Vector3(-1.224745f,1,1.224745f);
					_up = Vector3(0,1,0);
					Vector3 _right = _up.crossProduct(look);
					_right.normalize();
					_up = look.crossProduct(_right);
					_up.normalize();

					bv0 = Vector3(-(_right + _up) * f * s);
					bv1 = Vector3( (_right - _up) * f * s);
					bv2 = Vector3( (_right + _up) * f * s);
					bv3 = Vector3(-(_right - _up) * f * s);
				}

				if( s_UseVBO )
				{
					//更新vbo
					m_uiPreviousQuads = m_particleList.size()< s_MaxVertices/4? m_particleList.size():s_MaxVertices/4;
					uint _i = 0;
					MmParticleList::reverse_iterator rit = m_particleList.rbegin();

					float * _pVB = (float*)m_pVB->lock(0,0,BL_DISCARD);
					if( !_pVB )
					{
						if( _pVB) m_pVB->unlock();
						loadContext(pRenderSystem);
						return;
					}

					//用于粒子附着在骨骼上的顶点的位置的计算
					Matrix4 mtxFull(Matrix4::IDENTITY);
					if( m_pNode) mtxFull = m_pNode->getFullTransform();
					uchar _r,_g,_b,_a;
					xs::Vector3 * vertexarray[] = { &bv0, &bv1,&bv2,&bv3};

					if( m_pBone && m_pData->attach2Bone )
					{
						for(_i = 0; _i < m_uiPreviousQuads; ++_i)
						{
							_r=static_cast<uint>( rit->m_color.r * 255)  &0x00ff;
							_g=static_cast<uint>( rit->m_color.g * 255)  &0x00ff;
							_b=static_cast<uint>( rit->m_color.b * 255)  &0x00ff;
							_a=static_cast<uint>( rit->m_color.a * 255)  &0x00ff;
							Vector3 vTemp1 = mtxFull *(rit->m_vPos);
							for( uint _j = 0; _j <4; ++_j)
							{
								//position
								Vector3 _v = vTemp1 + (*vertexarray[_j])*rit->m_fSize ;
								*_pVB++ = _v.x;
								*_pVB++ = _v.y;
								*_pVB++ = _v.z;
								//texture coord
								*_pVB++ = m_pData->vTexcoords[rit->m_i32Tile].tc[_j].x;
								*_pVB++ = m_pData->vTexcoords[rit->m_i32Tile].tc[_j].y;
								//color coord
								uchar * _pCB = (uchar *)(_pVB);
								*_pCB++ = _r;
								*_pCB++ = _g;
								*_pCB++ = _b;
								*_pCB++ = _a;
								_pVB++;//increament _pVB;						
							}	
							++rit;
						}	
						m_pVB->unlock();
					}
					else
					{
						for(_i = 0; _i < m_uiPreviousQuads; ++_i)
						{
							_r=static_cast<uint>( rit->m_color.r * 255)  &0x00ff;
							_g=static_cast<uint>( rit->m_color.g * 255)  &0x00ff;
							_b=static_cast<uint>( rit->m_color.b * 255)  &0x00ff;
							_a=static_cast<uint>( rit->m_color.a * 255)  &0x00ff;
							for( uint _j = 0; _j <4; ++_j)
							{
								//position
								Vector3 _v = rit->m_vPos + (*vertexarray[_j])*rit->m_fSize ;
								*_pVB++ = _v.x;
								*_pVB++ = _v.y;
								*_pVB++ = _v.z;
								//texture coord
								*_pVB++ = m_pData->vTexcoords[rit->m_i32Tile].tc[_j].x;
								*_pVB++ = m_pData->vTexcoords[rit->m_i32Tile].tc[_j].y;
								//color coord
								uchar * _pCB = (uchar *)(_pVB);
								*_pCB++ = _r;
								*_pCB++ = _g;
								*_pCB++ = _b;
								*_pCB++ = _a;
								_pVB++;//increament _pVB;						
							}	
							++rit;
						}	
						m_pVB->unlock();		
					}
				}
				else
				{
					pRenderSystem->beginPrimitive(PT_QUADS);
					MmParticleList::iterator begin = m_particleList.begin();
					MmParticleList::iterator end = m_particleList.end();
					size_t size = m_particleList.size();
					for(MmParticleList::iterator it = begin; it != end;++it)
					{
						MmParticle& p = (*it);
						pRenderSystem->setColor(p.m_color);
						pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[0]);
						pRenderSystem->sendVertex(p.m_vPos + bv0 * (p.m_fSize));
						pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[1]);
						pRenderSystem->sendVertex(p.m_vPos + bv1 * (p.m_fSize));
						pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[2]);
						pRenderSystem->sendVertex(p.m_vPos + bv2 * (p.m_fSize));
						pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[3]);
						pRenderSystem->sendVertex(p.m_vPos + bv3 * (p.m_fSize));
					}
					pRenderSystem->endPrimitive();
				}
			}
			else if(m_pData->type == 1)
			{
				if(m_pData->billboardType != BILLBOARD_NOT_USED)
				{
					//以前的方式看不到
					//bv0 = mbb * Vector3(0,-1.0f,0);
					//bv1 = mbb * Vector3(0,+1.0f,0);
					bv0 = mbb * Vector3(1.0f,0.0f,1.0f);
					bv1 = mbb * Vector3(-1.0f,0.0f,-1.0f);
				}
				else
				{
					//以前的方式看不到
					//bv0 = Vector3(0,-1.0f,0);
					//bv1 = Vector3(0,+1.0f,0);
					bv0 = Vector3(1.0f,0.0f,1.0f);
					bv1 = Vector3(-1.0f,0.0f,-1.0f);
				}
				if(s_UseVBO)
				{
					//更新vbo
					m_uiPreviousQuads = m_particleList.size()< s_MaxVertices/4? m_particleList.size():s_MaxVertices/4;
					uint _i = 0;
					MmParticleList::reverse_iterator rit = m_particleList.rbegin();

					float * _pVB = (float*)m_pVB->lock(0,0,BL_DISCARD);
					if( !_pVB)
					{
						if( _pVB) m_pVB->unlock();
						loadContext(pRenderSystem);
						return;
					}

					//用于粒子附着在骨骼上的顶点的位置的计算
					Matrix4 mtxFull(Matrix4::IDENTITY);
					if( m_pNode) mtxFull = m_pNode->getFullTransform();

					uchar _r,_g,_b,_a;

					if( m_pBone && m_pData->attach2Bone)
					{
						for( _i=0; _i <m_uiPreviousQuads;++_i)
						{
							Vector3 vTemp1 = mtxFull *(rit->m_vPos);
							Vector3 vTemp2 = mtxFull *(rit->m_vOrigin);
							Vector3 vPos[4];
							vPos[0] = vTemp1 + bv0 * rit->m_fSize;
							vPos[1] = vTemp1 + bv1 * rit->m_fSize;
							vPos[2] = vTemp2 + bv0 * rit->m_fSize;
							vPos[3] = vTemp2 + bv1 * rit->m_fSize;
							_r=static_cast<uint>( rit->m_color.r * 255)  &0x00ff;
							_g=static_cast<uint>( rit->m_color.g * 255)  &0x00ff;
							_b=static_cast<uint>( rit->m_color.b * 255)  &0x00ff;
							_a=static_cast<uint>( rit->m_color.a * 255)  &0x00ff;
							for( uint _j =0; _j <4; ++_j)
							{
								//position
								*_pVB++ = vPos[_j].x;
								*_pVB++ = vPos[_j].y;
								*_pVB++ = vPos[_j].z;
								//texture coord
								*_pVB++ = m_pData->vTexcoords[rit->m_i32Tile].tc[_j].x;
								*_pVB++ = m_pData->vTexcoords[rit->m_i32Tile].tc[_j].y;
								//color 
								uchar * _pCB = (uchar*)(_pVB);
								*_pCB++ = _r;
								*_pCB++ = _g;
								*_pCB++ = _b;
								*_pCB++ = _a;
								_pVB++;//increament _pVB;							
							}
							++rit;
						}
						m_pVB->unlock();
					}
					else
					{
						for( _i=0; _i <m_uiPreviousQuads;++_i)
						{
							Vector3 vPos[4];
							vPos[0] = rit->m_vPos + bv0 * rit->m_fSize;
							vPos[1] = rit->m_vPos + bv1 * rit->m_fSize;
							vPos[2] = rit->m_vOrigin + bv0 * rit->m_fSize;
							vPos[3] = rit->m_vOrigin + bv1 * rit->m_fSize;
							_r=static_cast<uint>( rit->m_color.r * 255)  &0x00ff;
							_g=static_cast<uint>( rit->m_color.g * 255)  &0x00ff;
							_b=static_cast<uint>( rit->m_color.b * 255)  &0x00ff;
							_a=static_cast<uint>( rit->m_color.a * 255)  &0x00ff;
							for( uint _j =0; _j <4; ++_j)
							{
								//position
								*_pVB++ = vPos[_j].x;
								*_pVB++ = vPos[_j].y;
								*_pVB++ = vPos[_j].z;
								//texture coord
								*_pVB++ = m_pData->vTexcoords[rit->m_i32Tile].tc[_j].x;
								*_pVB++ = m_pData->vTexcoords[rit->m_i32Tile].tc[_j].y;
								//color 
								uchar * _pCB = (uchar*)(_pVB);
								*_pCB++ = _r;
								*_pCB++ = _g;
								*_pCB++ = _b;
								*_pCB++ = _a;
								_pVB++;//increament _pVB;							
							}
							++rit;
						}
						m_pVB->unlock();					
					}
				}
				else
				{
					pRenderSystem->beginPrimitive(PT_QUADS);
					MmParticleList::iterator begin = m_particleList.begin();
					MmParticleList::iterator end = m_particleList.end();
					size_t size = m_particleList.size();
					for(MmParticleList::iterator it = begin; it != end;++it)
					{
						MmParticle& p = (*it);
						pRenderSystem->setColor(p.m_color);
						pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[0]);
						pRenderSystem->sendVertex(p.m_vPos + bv0 * p.m_fSize * s);
						pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[1]);
						pRenderSystem->sendVertex(p.m_vPos + bv1 * p.m_fSize * s);
						pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[2]);
						pRenderSystem->sendVertex(p.m_vOrigin + bv1 * p.m_fSize * s);
						pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[3]);
						pRenderSystem->sendVertex(p.m_vOrigin + bv0 * p.m_fSize * s);
					}
					pRenderSystem->endPrimitive();
				}
			}
		}
		else
		{
			/*if(m_pData->billboardType != BILLBOARD_NOT_USED)
			{
				mbb = calcBillboardMatrix(pRenderSystem->getModelViewMatrix(),m_pData->pos,m_pData->billboardType);
			}*/

			if(m_pData->type == 0 || m_pData->type == 2)
			{
				float f = 0.707106781f; // sqrt(2)/2
				
				if(m_pData->billboardType != BILLBOARD_NOT_USED)
				{
					bv0 = mbb * Vector3(0,-f,+f);
					bv1 = mbb * Vector3(0,+f,+f);
					bv2 = mbb * Vector3(0,+f,-f);
					bv3 = mbb * Vector3(0,-f,-f);
				}
				else
				{
					bv0 = Vector3(-f,0,+f);
					bv1 = Vector3(+f,0,+f);
					bv2 = Vector3(+f,0,-f);
					bv3 = Vector3(-f,0,-f);
				}

				Vector3 s = Vector3(1,1,1);
				if(m_pNode)
				{
					Matrix4 mtx = m_pNode->getFullTransform();
					Vector3 t;
					Quaternion q;
					mtx.resolve(t,q,s);
				}

				if( s_UseVBO)
				{
					//更新vbo
					m_uiPreviousQuads = m_particleList.size()< s_MaxVertices/4? m_particleList.size():s_MaxVertices/4;
					uint _i = 0;
					MmParticleList::reverse_iterator rit = m_particleList.rbegin();

					float * _pVB = (float*)m_pVB->lock(0,0,BL_DISCARD);
					if( !_pVB)
					{
						if( _pVB) m_pVB->unlock();
						loadContext(pRenderSystem);
						return;
					}

					//用于粒子附着在骨骼上的顶点的位置的计算
					Matrix4 mtxFull(Matrix4::IDENTITY);
					if( m_pNode) mtxFull = m_pNode->getFullTransform();
					uchar _r,_g,_b,_a;
					xs::Vector3 * vertexarray[] = { &bv0, &bv1,&bv2,&bv3};

					if( m_pBone && m_pData->attach2Bone )
					{
						for(_i = 0; _i < m_uiPreviousQuads; ++_i)
						{
							_r=static_cast<uint>( rit->m_color.r * 255)  &0x00ff;
							_g=static_cast<uint>( rit->m_color.g * 255)  &0x00ff;
							_b=static_cast<uint>( rit->m_color.b * 255)  &0x00ff;
							_a=static_cast<uint>( rit->m_color.a * 255)  &0x00ff;
							Vector3 vTemp1 = mtxFull *(rit->m_vPos);
							for( uint _j = 0; _j <4; ++_j)
							{
								//position
								Vector3 _v = vTemp1 + (*vertexarray[_j])*rit->m_fSize ;
								*_pVB++ = _v.x;
								*_pVB++ = _v.y;
								*_pVB++ = _v.z;
								//texture coord
								*_pVB++ = m_pData->vTexcoords[rit->m_i32Tile].tc[_j].x;
								*_pVB++ = m_pData->vTexcoords[rit->m_i32Tile].tc[_j].y;
								//color coord
								uchar * _pCB = (uchar *)(_pVB);
								*_pCB++ = _r;
								*_pCB++ = _g;
								*_pCB++ = _b;
								*_pCB++ = _a;
								_pVB++;//increament _pVB;						
							}	
							++rit;
						}	
						m_pVB->unlock();
					}
					else
					{
						for(_i = 0; _i < m_uiPreviousQuads; ++_i)
						{
							_r=static_cast<uint>( rit->m_color.r * 255)  &0x00ff;
							_g=static_cast<uint>( rit->m_color.g * 255)  &0x00ff;
							_b=static_cast<uint>( rit->m_color.b * 255)  &0x00ff;
							_a=static_cast<uint>( rit->m_color.a * 255)  &0x00ff;
							for( uint _j = 0; _j <4; ++_j)
							{
								//position
								Vector3 _v = rit->m_vPos + (*vertexarray[_j])*rit->m_fSize ;
								*_pVB++ = _v.x;
								*_pVB++ = _v.y;
								*_pVB++ = _v.z;
								//texture coord
								*_pVB++ = m_pData->vTexcoords[rit->m_i32Tile].tc[_j].x;
								*_pVB++ = m_pData->vTexcoords[rit->m_i32Tile].tc[_j].y;
								//color coord
								uchar * _pCB = (uchar *)(_pVB);
								*_pCB++ = _r;
								*_pCB++ = _g;
								*_pCB++ = _b;
								*_pCB++ = _a;
								_pVB++;//increament _pVB;						
							}	
							++rit;
						}	
						m_pVB->unlock();		
					}
				}
				else
				{
					pRenderSystem->beginPrimitive(PT_QUADS);
					MmParticleList::iterator begin = m_particleList.begin();
					MmParticleList::iterator end = m_particleList.end();
					size_t size = m_particleList.size();
					for(MmParticleList::iterator it = begin; it != end;++it)
					{
						MmParticle& p = (*it);
						pRenderSystem->setColor(p.m_color);
						pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[0]);
						pRenderSystem->sendVertex(p.m_vPos + bv0 * (p.m_fSize * s));
						pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[1]);
						pRenderSystem->sendVertex(p.m_vPos + bv1 * (p.m_fSize * s));
						pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[2]);
						pRenderSystem->sendVertex(p.m_vPos + bv2 * (p.m_fSize * s));
						pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[3]);
						pRenderSystem->sendVertex(p.m_vPos + bv3 * (p.m_fSize * s));
					}
					pRenderSystem->endPrimitive();
				}
			}
			else if(m_pData->type == 1)
			{
				// particles from origin to position

				if(m_pData->billboardType != BILLBOARD_NOT_USED)
				{
					//以前的方式看不到
					//bv0 = mbb * Vector3(0,-1.0f,0);
					//bv1 = mbb * Vector3(0,+1.0f,0);
					bv0 = mbb * Vector3(1.0f,0.0f,1.0f);
					bv1 = mbb * Vector3(-1.0f,0.0f,-1.0f);
				}
				else
				{
					//以前的方式看不到
					//bv0 = Vector3(0,-1.0f,0);
					//bv1 = Vector3(0,+1.0f,0);
					bv0 = Vector3(1.0f,0.0f,1.0f);
					bv1 = Vector3(-1.0f,0.0f,-1.0f);
				}

				Vector3 s = Vector3(1,1,1);
				if(m_pNode)
				{
					Matrix4 mtx = m_pNode->getFullTransform();
					Vector3 t;
					Quaternion q;
					mtx.resolve(t,q,s);
				}

				if( s_UseVBO)
				{
					//更新vbo
					m_uiPreviousQuads = m_particleList.size()< s_MaxVertices/4? m_particleList.size():s_MaxVertices/4;
					uint _i = 0;
					MmParticleList::reverse_iterator rit = m_particleList.rbegin();

					float * _pVB = (float*)m_pVB->lock(0,0,BL_DISCARD);
					if( !_pVB)
					{
						if( _pVB) m_pVB->unlock();
						loadContext(pRenderSystem);
						return;
					}

					//用于粒子附着在骨骼上的顶点的位置的计算
					Matrix4 mtxFull(Matrix4::IDENTITY);
					if( m_pNode) mtxFull = m_pNode->getFullTransform();
					uchar _r,_g,_b,_a;

					if( m_pBone && m_pData->attach2Bone)
					{
						for( _i=0; _i <m_uiPreviousQuads;++_i)
						{
							Vector3 vTemp1 = mtxFull *(rit->m_vPos);
							Vector3 vTemp2 = mtxFull *(rit->m_vOrigin);
							Vector3 vPos[4];
							vPos[0] = vTemp1 + bv0 * rit->m_fSize;
							vPos[1] = vTemp1 + bv1 * rit->m_fSize;
							vPos[2] = vTemp2 + bv0 * rit->m_fSize;
							vPos[3] = vTemp2 + bv1 * rit->m_fSize;
							_r=static_cast<uint>( rit->m_color.r * 255)  &0x00ff;
							_g=static_cast<uint>( rit->m_color.g * 255)  &0x00ff;
							_b=static_cast<uint>( rit->m_color.b * 255)  &0x00ff;
							_a=static_cast<uint>( rit->m_color.a * 255)  &0x00ff;
							for( uint _j =0; _j <4; ++_j)
							{
								//position
								*_pVB++ = vPos[_j].x;
								*_pVB++ = vPos[_j].y;
								*_pVB++ = vPos[_j].z;
								//texture coord
								*_pVB++ = m_pData->vTexcoords[rit->m_i32Tile].tc[_j].x;
								*_pVB++ = m_pData->vTexcoords[rit->m_i32Tile].tc[_j].y;
								//color 
								uchar * _pCB = (uchar*)(_pVB);
								*_pCB++ = _r;
								*_pCB++ = _g;
								*_pCB++ = _b;
								*_pCB++ = _a;
								_pVB++;//increament _pVB;							
							}
							++rit;
						}
						m_pVB->unlock();
					}
					else
					{
						for( _i=0; _i <m_uiPreviousQuads;++_i)
						{
							Vector3 vPos[4];
							vPos[0] = rit->m_vPos + bv0 * rit->m_fSize;
							vPos[1] = rit->m_vPos + bv1 * rit->m_fSize;
							vPos[2] = rit->m_vOrigin + bv0 * rit->m_fSize;
							vPos[3] = rit->m_vOrigin + bv1 * rit->m_fSize;
							_r=static_cast<uint>( rit->m_color.r * 255)  &0x00ff;
							_g=static_cast<uint>( rit->m_color.g * 255)  &0x00ff;
							_b=static_cast<uint>( rit->m_color.b * 255)  &0x00ff;
							_a=static_cast<uint>( rit->m_color.a * 255)  &0x00ff;
							for( uint _j =0; _j <4; ++_j)
							{
								//position
								*_pVB++ = vPos[_j].x;
								*_pVB++ = vPos[_j].y;
								*_pVB++ = vPos[_j].z;
								//texture coord
								*_pVB++ = m_pData->vTexcoords[rit->m_i32Tile].tc[_j].x;
								*_pVB++ = m_pData->vTexcoords[rit->m_i32Tile].tc[_j].y;
								//color 
								uchar * _pCB = (uchar*)(_pVB);
								*_pCB++ = _r;
								*_pCB++ = _g;
								*_pCB++ = _b;
								*_pCB++ = _a;
								_pVB++;//increament _pVB;							
							}
							++rit;
						}
						m_pVB->unlock();					
					}
				}
				else
				{

					pRenderSystem->beginPrimitive(PT_QUADS);
					MmParticleList::iterator begin = m_particleList.begin();
					MmParticleList::iterator end = m_particleList.end();
					size_t size = m_particleList.size();
					for(MmParticleList::iterator it = begin; it != end;++it)
					{
						MmParticle& p = (*it);
						pRenderSystem->setColor(p.m_color);
						pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[0]);
						pRenderSystem->sendVertex(p.m_vPos + bv0 * p.m_fSize * s);
						pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[1]);
						pRenderSystem->sendVertex(p.m_vPos + bv1 * p.m_fSize * s);
						pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[2]);
						pRenderSystem->sendVertex(p.m_vOrigin + bv1 * p.m_fSize * s);
						pRenderSystem->setTexcoord(m_pData->vTexcoords[p.m_i32Tile].tc[3]);
						pRenderSystem->sendVertex(p.m_vOrigin + bv0 * p.m_fSize * s);
					}
					pRenderSystem->endPrimitive();
				}
			}
		}
#endif

		//恢复渲染状态
		loadContext(pRenderSystem);
	}

	int MmParticleSystem::getRenderOrder()
	{
		switch(m_pData->blend)
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

	void MmParticleSystem::setColor(const ColorValue& color)
	{
		m_color = color;
	}

	void MmParticleSystem::reset()
	{
		m_particleList.clear();
		m_uiPreviousQuads= 0;
	}

	void MmParticleSystem::saveContext(IRenderSystem *pRenderSystem)
	{
		m_lightingEnabled = pRenderSystem->isLightingEnabled();
		pRenderSystem->getSceneBlending(m_sbfSrc,m_sbfDst);
		m_alphaCheck = pRenderSystem->alphaCheckEnabled();
		pRenderSystem->getAlphaFunction(m_alphaFunc,m_alphaCheckRef);
		m_cullingMode = pRenderSystem->getCullingMode();
		m_bDepthCheck = pRenderSystem->isDepthBufferCheckEnabled();
		m_bDepthWrite = pRenderSystem->isDepthBufferWriteEnabled();
		m_saveColor = pRenderSystem->getColor();
	}

	void MmParticleSystem::loadContext(IRenderSystem *pRenderSystem)
	{
		pRenderSystem->setLightingEnabled(m_lightingEnabled);
		pRenderSystem->setTexture(0,0);
		pRenderSystem->setSceneBlending(m_sbfSrc,m_sbfDst);
		pRenderSystem->setAlphaCheckEnabled(m_alphaCheck);
		pRenderSystem->setAlphaFunction(m_alphaFunc,m_alphaCheckRef);
		pRenderSystem->setCullingMode(m_cullingMode);
		pRenderSystem->setDepthBufferCheckEnabled(m_bDepthCheck);
		pRenderSystem->setDepthBufferWriteEnabled(m_bDepthWrite);
		pRenderSystem->setColor(m_saveColor);
	}

	void MmParticleSystem::Clear()
	{
		m_particleList.clear();
		if( m_pEmitter ) 
		{
			m_pEmitter->release();
			m_pEmitter = 0;
		}

		if( m_pTexture )
		{
			safeRelease(m_pTexture);
			m_pTexture = 0;
		}
		m_rem = 0;
		m_random = Math::UnitRandom();
		m_b25d = true;
		m_uiPreviousQuads= 0;;
		m_bInitializeTexture = false;
	}
}