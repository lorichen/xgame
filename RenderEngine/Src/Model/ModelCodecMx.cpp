#include "StdAfx.h"
#include "ModelCodecMx.h"
#include "RenderEngine/MzHeader.h"
#include "BoneAnimationSetManager.h"



namespace xs
{

	ModelCodecMx::ModelCodecMx(const std::string& name,IRenderSystem* pRenderSystem)
		: ManagedItem(name.c_str()),
		m_pRenderSystem(pRenderSystem),
		m_pVertexData(0),
		m_pVBTexcoord(0),
		m_pIB(0),
		m_ui32VerticeNum(0),
		m_bAnimatedGeometry(false),
		m_memorySize(0),
		m_pVBPosNormal(0),
		m_pBoneAnimation(0),
		m_bCalculatedAABB(false),
		m_pBlendWeightBuffer(0),
		m_pBlendProgram(0),
		m_bLoadSkinInfoFromFile(false)
	{
		memset(m_pPrecomputedVBs,0,/*8192*/ MX_PRECOMPUTED_VBO_COUNT_MAX * sizeof(IVertexBuffer*));
	}

	void ModelCodecMx::release()
	{
		delete this;
	}

	ModelCodecMx::~ModelCodecMx()
	{
		for(int i = 0;i <  MX_PRECOMPUTED_VBO_COUNT_MAX /*8192*/;i++)
		{
			safeRelease(m_pPrecomputedVBs[i]);
		}
		STLDeleteSequence(m_vParticleEmitters);
		STLDeleteSequence(m_vRibbonEmitters);
		//下面两个已经用骨骼动画代替了
		//STLDeleteSequence(m_vBones);
		//STLDeleteSequence(m_vAnimations);
		BoneAnimationSetManager::getInstance().releaseBoneAnimationSet(m_pBoneAnimation);
		m_pBoneAnimation = 0;
		safeRelease(m_pVBTexcoord);
		safeRelease(m_pVBPosNormal);
		safeRelease(m_pIB);
		safeRelease(m_pBlendWeightBuffer);
		safeRelease(m_pBlendProgram);
		STLReleaseSequence(m_vBlendIndices);
		safeDeleteArray(m_pVertexData);	
	}

	Animation* ModelCodecMx::hasAnimation(const std::string& animation)
	{
		if( m_pBoneAnimation )
			return m_pBoneAnimation->getAnimationByName(animation);
		return 0;
	}

	const std::string& ModelCodecMx::getFileName()
	{
		return m_strName;
	}

	const AABB&	ModelCodecMx::getAABB()
	{
		if( !m_bCalculatedAABB)
		{
			if( m_pVertexData) calcAABB(m_pVertexData,m_ui32VerticeNum,m_AABB,m_BoundingSphere);
			m_bCalculatedAABB = true;
		}
		return m_AABB;
	}

	const Sphere& ModelCodecMx::getBoudingSphere()
	{
		if( !m_bCalculatedAABB)
		{
			if( m_pVertexData) calcAABB(m_pVertexData,m_ui32VerticeNum,m_AABB,m_BoundingSphere);
			m_bCalculatedAABB = true;
		}
		return m_BoundingSphere;
	}

	uint ModelCodecMx::getAnimationCount()
	{
		if( m_pBoneAnimation)
			return m_pBoneAnimation->getAnimationCount();
		return 0;
	}

	Animation* ModelCodecMx::getAnimation(uint index)
	{
		if( m_pBoneAnimation)
			return m_pBoneAnimation->getAnimationByIndex(index);
		return 0;
	}

	CoreSkeleton * ModelCodecMx::getCoreSkeletion()
	{
		return m_pBoneAnimation->getCoreSkeletion();
	}

	uint ModelCodecMx::getNumSubModels()
	{
		return m_vRenderPasses.size();
	}

	ISubModel*	ModelCodecMx::getSubModel(const char* name)
	{
		size_t size = m_vRenderPasses.size();
		for(uint i = 0;i < size;i++)
		{
			if(StringHelper::casecmp(m_vRenderPasses[i].m_name,name) == 0)
			{
				return &m_vRenderPasses[i];
			}
		}
		return 0;
	}

	ISubModel*	ModelCodecMx::getSubModel(int index)
	{
		if(index < 0 || index >= (int)m_vRenderPasses.size())return 0;
		return &m_vRenderPasses[index];
	}

	IIndexBuffer* ModelCodecMx::getIndexBuffer()
	{
		return m_pIB;
	}

	IVertexBuffer* ModelCodecMx::getTexcoordBuffer()
	{
		return m_pVBTexcoord;
	}

	uint ModelCodecMx::getNumFaces()
	{
		return m_pIB ? m_pIB->getNumIndices() / 3 : 0;
	}

	uint ModelCodecMx::getNumVertices()
	{
		return m_ui32VerticeNum;
	}

	uint ModelCodecMx::getVer()
	{
		return m_ver;
	}

	IVertexBuffer* ModelCodecMx::vertexBlend(Skeleton *pSkeleton,const AnimationTime *pTime)
	{
		if(!m_bAnimatedGeometry)return m_pVBPosNormal;
		if(!m_ui32VerticeNum || !pTime || !pSkeleton)return 0;

		int index = (pTime->current * 3 / 100) % MX_PRECOMPUTED_VBO_COUNT_MAX ;
		IVertexBuffer *pVB = m_pPrecomputedVBs[index];
		if(pVB)return pVB;
		m_pPrecomputedVBs[index] = m_pRenderSystem->getBufferManager()->createVertexBuffer(12,m_ui32VerticeNum << 1,BU_STATIC_WRITE_ONLY, CBF_IMMEDIATE);
		pVB = m_pPrecomputedVBs[index];

		if(!pVB)return 0;

		Vector3* pData = (Vector3*)pVB->lock(0,0,BL_DISCARD);
		if(pData)
		{
			Vector3 *pSkinPos = &pData[0];
			Vector3 *pSkinNormal = &pData[m_ui32VerticeNum];
			uint boneCount = pSkeleton->getBoneCount();
			Bone* pBonesPonter[255];
			for( uint i=0; i<boneCount; ++i)
			{
				pBonesPonter[i] = pSkeleton->getBone(i);
			}

			for(uint i = 0;i < m_ui32VerticeNum;i++)
			{
				Vector3 v(0,0,0),n(0,0,0);
				_VertexData *ov = m_pVertexData + i;

				for(uint b = 0;b < 4;b++) 
				{
					if(ov->weights[b] > 0&& ov->bones[b]< boneCount ) 
					{
						Vector3 tv,tn;
						Vector3 otv = Vector3(ov->pos[0],ov->pos[1],ov->pos[2]);

						Bone *pBone = pBonesPonter[ov->bones[b]]; // pSkeleton->getBone(ov->bones[b]);
						tv = pBone->getFullTransform() * otv;

						Vector3 otn = Vector3(ov->normal[0],ov->normal[1],ov->normal[2]);
						tn = pBone->getFullRotation() * otn;

						v += tv * ((float)ov->weights[b]);
						n += tn * ((float)ov->weights[b]);
					}
				}
				pSkinPos[i] = v;
				n.normalize();
				pSkinNormal[i] = n;
			}
			pVB->unlock();
		}

		return pVB;
	}

	uint ModelCodecMx::numParticleSystem()
	{
		return m_vParticleEmitters.size();
	}

	ParticleEmitter2Data* ModelCodecMx::getParticleSystemData(uint index)
	{
		if(index >= m_vParticleEmitters.size())return 0;
		return m_vParticleEmitters[index];
	}

	uint ModelCodecMx::numRibbonSystem()
	{
		return m_vRibbonEmitters.size();
	}

	RibbonEmitterData*	ModelCodecMx::getRibbonSystemData(uint index)
	{
		if(index >= m_vRibbonEmitters.size())return 0;
		return m_vRibbonEmitters[index];
	}

	uint ModelCodecMx::numMmParticleSystem()
	{
		return 0;
	}

	MmParticleSystemData* ModelCodecMx::getMmParticleSystem(uint index)
	{
		return 0;
	}

	size_t ModelCodecMx::getMomorySize()
	{
		return m_memorySize;
	}

	const char*	ModelCodecMx::getType() const
	{
		static std::string strType = "mx";
		return strType.c_str();
	}


	IVertexBuffer *	ModelCodecMx::getBlendWeightBuffer()
	{
		return m_pBlendWeightBuffer;
	}

	IVertexBuffer * ModelCodecMx::getBlendIndicesBuffer(uint subModelIndex)
	{
		if( subModelIndex >= m_vBlendIndices.size() ) return 0;
		else return m_vBlendIndices[subModelIndex];
	}

	IVertexBuffer * ModelCodecMx::getOriginVertexBuffer()
	{
		return m_pVBPosNormal;
	}

	IShaderProgram * ModelCodecMx::getBlendShaderProgram()
	{
		return m_pBlendProgram;
	}

	void ModelCodecMx::initForGPUVertexBlend(MODELFACE* pFaceBuffer)
	{
		//检测
		if( 0 == m_pBlendProgram )
			return;

		if( 0 ==m_pBoneAnimation ||
			m_pBoneAnimation->getCoreSkeletion()->getCoreBoneCount() == 0)
		{
			safeRelease(m_pBlendProgram);
			return;//没有骨骼
		}

		if(!m_bLoadSkinInfoFromFile && 0 == pFaceBuffer)
		{
			safeRelease(m_pBlendProgram);
			return;
		}

		//生成顶点混合的权重buffer
		m_pBlendWeightBuffer = m_pRenderSystem->getBufferManager()->createVertexBuffer(12,m_ui32VerticeNum,BU_STATIC_WRITE_ONLY);
		if( 0 == m_pBlendWeightBuffer)
		{
			safeRelease(m_pBlendProgram);
			return;
		}
		float * pWeight = (float*)m_pBlendWeightBuffer->lock(0,0,BL_NORMAL);
		if( 0 == pWeight)
		{
			safeRelease(m_pBlendProgram);
			safeRelease(m_pBlendWeightBuffer);
			return;
		}
		for( uint i =0; i<m_ui32VerticeNum; ++i)
		{
			*pWeight++ = m_pVertexData[i].weights[0];
			*pWeight++ = m_pVertexData[i].weights[1];
			*pWeight++ = m_pVertexData[i].weights[2];
		}
		m_pBlendWeightBuffer->unlock();


		if( !m_bLoadSkinInfoFromFile )//没有从文件读取蒙皮信息，需要拆分子Mesh
		{
			if( m_pBoneAnimation->getCoreSkeletion()->getCoreBoneCount()< VERTEX_BLEND_MATRICES_NUM )//不用拆分
			{
				uint passCount = m_vRenderPasses.size();
				uint bonesCount = m_pBoneAnimation->getCoreSkeletion()->getCoreBoneCount();
				for(uint i=0; i< passCount; ++i)
				{
					m_vRenderPasses[i].m_ValidIndices = bonesCount+1;

					for( uint j = 0; j<bonesCount; ++j)
						m_vRenderPasses[i].m_BindedBonesIndices[j] = j;
					m_vRenderPasses[i].m_BindedBonesIndices[bonesCount] = VERTEX_BLEND_MAPPINGS_NUM -1;

					for(uint j =0; j<bonesCount; ++j)
						m_vRenderPasses[i].m_BonesMappings[j] = j;
					for( uint j=bonesCount; j<VERTEX_BLEND_MAPPINGS_NUM; ++j)
						m_vRenderPasses[i].m_BonesMappings[j] = bonesCount;
				}
			}
			else
			{
				//拆分为子mesh
				std::vector<SubModel> tmpRenderPasses;
				uint nSubMesh = m_vRenderPasses.size();
				for( uint mesh=0; mesh < nSubMesh; mesh++)
				{
					//初始化
					uchar bonesMappings[VERTEX_BLEND_MAPPINGS_NUM];
					for( uint i =0; i < VERTEX_BLEND_MAPPINGS_NUM; ++i)
						bonesMappings[i] = VERTEX_BLEND_MATRICES_NUM;
					uchar reverseBonesMappings[VERTEX_BLEND_MATRICES_NUM-1];
					for( uint i =0; i< VERTEX_BLEND_MATRICES_NUM - 1; ++i)//第VERTEX_BLEND_MATRICES_NUM个矩阵时哑元
						reverseBonesMappings[i] = 255;
					uint stacknum = 0;
					uint submeshfaceindexstart = m_vRenderPasses[mesh].indexStart / 3;

					uint facestart = m_vRenderPasses[mesh].indexStart / 3;
					uint facenum = m_vRenderPasses[mesh].indexCount / 3;
					for( uint i = 0; i < facenum; ++i)
					{
						uchar newbones[12];
						uchar newbonescount = 0;
						uint faceindex = facestart + i;
						for( uint j =0; j < 3; ++j)
						{
							uint vertexindex = pFaceBuffer[faceindex].index[j];
							for( uint k =0; k<4; ++k)
							{
								uint bonesindex = m_pVertexData[vertexindex].bones[k];
								if( bonesMappings[bonesindex] == VERTEX_BLEND_MATRICES_NUM )
								{
									newbones[newbonescount] = bonesindex;
									bonesMappings[bonesindex] = stacknum + newbonescount;
									++newbonescount;	
								}	
							}
						}
						if( stacknum + newbonescount > VERTEX_BLEND_MATRICES_NUM -1 )//此时需要生成一个子mesh,第VERTEX_BLEND_MATRICES_NUM个矩阵时哑元
						{
							//调整bonesMappings
							for( uint j=0; j <newbonescount; ++j)
								bonesMappings[ newbones[j] ] = VERTEX_BLEND_MATRICES_NUM;

							//生成或者拆分子mesh
							SubModel pass;
							pass.indexStart = submeshfaceindexstart* 3;
							pass.indexCount = (faceindex -1  - submeshfaceindexstart + 1) *3;//不需要包含当前的面
							pass.vertexStart = m_vRenderPasses[mesh].vertexStart;
							pass.vertexEnd = m_vRenderPasses[mesh].vertexEnd;
							pass.m_matIndex = m_vRenderPasses[mesh].m_matIndex;
							pass.m_bAnimated = m_vRenderPasses[mesh].m_bAnimated;
							pass.m_name = m_vRenderPasses[mesh].m_name;
							char namepostfix[64];
							itoa(pass.indexStart, namepostfix, 10);
							pass.m_name = pass.m_name +"_"+ namepostfix +"_";
							itoa(pass.indexCount, namepostfix, 10);
							pass.m_name += namepostfix;
							//拷贝硬件顶点混合数据
							for( uint j =0; j< VERTEX_BLEND_MATRICES_NUM-1; ++j)
								pass.m_BindedBonesIndices[j] = reverseBonesMappings[j];
							pass.m_BindedBonesIndices[VERTEX_BLEND_MATRICES_NUM-1] = VERTEX_BLEND_MAPPINGS_NUM - 1;
							for( uint j = 0; j <VERTEX_BLEND_MAPPINGS_NUM; ++j)
							{
								if( bonesMappings[j] == VERTEX_BLEND_MATRICES_NUM)
									pass.m_BonesMappings[j] = VERTEX_BLEND_MATRICES_NUM - 1;
								else
									pass.m_BonesMappings[j] =bonesMappings[j]; 
							}
							pass.m_ValidIndices = VERTEX_BLEND_MATRICES_NUM;

							tmpRenderPasses.push_back(pass);	

							//重新初始化
							for( uint j =0; j <VERTEX_BLEND_MAPPINGS_NUM; ++j)
								bonesMappings[j] = VERTEX_BLEND_MATRICES_NUM;
							for( uint j = 0; j <VERTEX_BLEND_MATRICES_NUM-1; ++j)
								reverseBonesMappings[j]  = 255;
							stacknum = 0;
							submeshfaceindexstart = faceindex;
							--i;//这里很重要
						}	
						else if( i == facenum - 1)//此时到达末尾，也需要生成一个子mesh
						{
							//查看最后一个面是否有新的骨骼加入
							for( uint j=0; j <newbonescount; ++j)
							{
								reverseBonesMappings[stacknum] = newbones[j];
								++stacknum;	
							}

							//生成或者拆分子mesh
							SubModel pass;
							pass.indexStart = submeshfaceindexstart* 3;
							pass.indexCount = (faceindex   - submeshfaceindexstart + 1) *3;//需要包含当前的面
							pass.vertexStart = m_vRenderPasses[mesh].vertexStart;
							pass.vertexEnd = m_vRenderPasses[mesh].vertexEnd;
							pass.m_matIndex = m_vRenderPasses[mesh].m_matIndex;
							pass.m_bAnimated = m_vRenderPasses[mesh].m_bAnimated;
							pass.m_name = m_vRenderPasses[mesh].m_name;//最后一个子mesh保留原来的名字，这样，如果不用拆分，子mesh还是原来的名字
							//拷贝硬件顶点混合数据
							for( uint j =0; j< VERTEX_BLEND_MATRICES_NUM-1; ++j)
								pass.m_BindedBonesIndices[j] = reverseBonesMappings[j];
							pass.m_BindedBonesIndices[VERTEX_BLEND_MATRICES_NUM-1] = VERTEX_BLEND_MAPPINGS_NUM -1;
							for( uint j = 0; j <VERTEX_BLEND_MAPPINGS_NUM; ++j)
							{
								if( bonesMappings[j] == VERTEX_BLEND_MATRICES_NUM)
									pass.m_BonesMappings[j] = VERTEX_BLEND_MATRICES_NUM - 1;
								else
									pass.m_BonesMappings[j] =bonesMappings[j]; 
							}

							pass.m_ValidIndices = stacknum + newbonescount + 1;

							tmpRenderPasses.push_back(pass);

							//不需要重新初始化
						}
						else if( newbonescount > 0) //此时添加新骨骼
						{
							for( uint j=0; j <newbonescount; ++j)
							{
								reverseBonesMappings[stacknum] = newbones[j];
								++stacknum;	
							}						
						}
					}
				}

				//将子mesh复制到m_vRenderPasses
				m_vRenderPasses.clear();
				for( uint i=0; i<tmpRenderPasses.size(); ++i)
				{
					m_vRenderPasses.push_back( tmpRenderPasses[i]);
				}
				tmpRenderPasses.clear();

				//重新给子Mesh设置纹理
				size_t size = m_vRenderPasses.size();
				for(size_t j = 0;j < size;j++)
				{
					SubModel& subModel = m_vRenderPasses[j];
					size_t mtlSize = m_vMaterials.size();
					for(size_t k = 0;k < mtlSize;k++)
					{
						if(subModel.m_matIndex == k)
						{
							subModel.setMaterial(m_vMaterials[k]);
							break;
						}
					}
				}
			}
		}


		//生成每个子mesh对应的骨骼索引buffer
		for( uint i =0; i< m_vRenderPasses.size(); ++i)
		{
			xs::SubModel & sub = m_vRenderPasses[i];
			IVertexBuffer* pBlendIndices = m_pRenderSystem->getBufferManager()->createVertexBuffer(4,m_ui32VerticeNum,BU_STATIC_WRITE_ONLY);
			if( 0 == pBlendIndices)
			{
				safeRelease(m_pBlendProgram);
				safeRelease(m_pBlendWeightBuffer);
				STLReleaseSequence(m_vBlendIndices);
				return;	
			}
			uint * pIndices = (uint*)pBlendIndices->lock(0,0,BL_NORMAL);
			if( 0 == pIndices)
			{
				safeRelease(m_pBlendProgram);
				safeRelease(m_pBlendWeightBuffer);
				STLReleaseSequence(m_vBlendIndices);
				return;
			}
			for( uint i=0; i <m_ui32VerticeNum; ++i)
			{
				*pIndices = 0;
				*pIndices |= sub.m_BonesMappings[ m_pVertexData[i].bones[0] ];
				*pIndices |= sub.m_BonesMappings[ m_pVertexData[i].bones[1] ] << 8;
				*pIndices |= sub.m_BonesMappings[ m_pVertexData[i].bones[2] ] << 16;
				*pIndices |= sub.m_BonesMappings[ m_pVertexData[i].bones[3] ] << 24;
				++pIndices;
			}
			pBlendIndices->unlock();
			m_vBlendIndices.push_back( pBlendIndices);
		}

	}

	bool ModelCodecMx::decode(xs::Stream *pStream)
	{
		PP_BY_NAME("ModelCodecMx::decode");

		//当前版本是3，版本3支持导出计算aabb
		uint ver = 0;
		xs::DataChunk chunk;
		xs::DataChunk::stChunk *pChunk = chunk.beginChunk(pStream);

		//构建着色器程序
		MODELFACE* pFaceBuffer = 0;
		if( m_pRenderSystem->getRenderSystemType() == RS_D3D9
			&& m_pRenderSystem->getCapabilities()->hasCapability(RSC_HIGHLEVEL_SHADER) )
		{
			m_pBlendProgram = m_pRenderSystem->getShaderProgramManager()->createShaderProgram(/*SPT_HIGHLEVEL*/ SPT_LOWLEVEL);
			if( m_pBlendProgram )
			{
				bool ret = m_pBlendProgram->addShaderFromFile(ST_VERTEX_PROGRAM,/*"data\\Shader\\D3D9\\SkinMesh.hlsl"*/ "data\\Shader\\D3D9\\SkinMesh_NoLightingCalc.asm");
				if(!ret)
				{
					safeRelease(m_pBlendProgram);
				}
				else
				{
					m_pBlendProgram->link();
				}
			}		
		}
		else
		{
			//OGL 待定
		}

		while(pChunk)
		{
			if(pChunk->m_ui32DataSize == 0)
			{
				pChunk = chunk.nextChunk(pStream);
				continue;
			}
			xs::MemoryStream stream((uchar*)pChunk->m_pData,pChunk->m_ui32DataSize);
			switch(pChunk->m_ui32Type)
			{
				case 'MVER':
				{
					stream.read(&ver,sizeof(ver));
					m_ver = ver;
					if(ver < 1) return false;
				}
				break;

				case 'MBOX':
					{
						Vector3 vMin,vMax;
						stream.read(vMin.val, sizeof(vMin.val));
						stream.read(vMax.val, sizeof(vMax.val));
						m_AABB.setMinimum(vMin);
						m_AABB.setMaximum(vMax);
						m_BoundingSphere.setCenter( vMin.midPoint(vMax) );
						m_BoundingSphere.setRadius( (vMax-vMin).length() * 0.5);
						m_bCalculatedAABB = true;
					}
					break;

				case 'MVTX':
				{
					m_ui32VerticeNum = stream.getLength() / sizeof(_VertexData);
					m_pVertexData = new _VertexData[m_ui32VerticeNum];
					memcpy(m_pVertexData,pChunk->m_pData,pChunk->m_ui32DataSize);
					calcAABB(m_pVertexData,m_ui32VerticeNum,m_AABB,m_BoundingSphere);

					m_pVBPosNormal = m_pRenderSystem->getBufferManager()->createVertexBuffer(12,2 * m_ui32VerticeNum,BU_STATIC_WRITE_ONLY);
					if(!m_pVBPosNormal)return false;

					float* pBuffer = (float*)m_pVBPosNormal->lock(0,0,/*BL_DISCARD*/BL_NORMAL);
					if(pBuffer)
					{
						for(uint i = 0;i < m_ui32VerticeNum;i++)
						{
							*pBuffer++ = m_pVertexData[i].pos[0];
							*pBuffer++ = m_pVertexData[i].pos[1];
							*pBuffer++ = m_pVertexData[i].pos[2];
						}
						for(uint i = 0;i < m_ui32VerticeNum;i++)
						{
							*pBuffer++ = m_pVertexData[i].normal[0];
							*pBuffer++ = m_pVertexData[i].normal[1];
							*pBuffer++ = m_pVertexData[i].normal[2];
						}
						m_pVBPosNormal->unlock();
					}

					m_pVBTexcoord = m_pRenderSystem->getBufferManager()->createVertexBuffer(8,m_ui32VerticeNum,BU_STATIC_WRITE_ONLY);
					if(!m_pVBTexcoord)return false;

					pBuffer = (float*)m_pVBTexcoord->lock(0,0,BL_NORMAL);
					if(pBuffer)
					{
						for(uint i = 0;i < m_ui32VerticeNum;i++)
						{
							*pBuffer++ = m_pVertexData[i].texcoords[0];
							*pBuffer++ = m_pVertexData[i].texcoords[1];
						}
						m_pVBTexcoord->unlock();
					}
				}
				break;

				case 'MFAC':
				{
					uint nFace = stream.getLength() / sizeof(MODELFACE);
					MODELFACE* pFace = (MODELFACE *)stream.getBuffer();

					m_pIB = m_pRenderSystem->getBufferManager()->createIndexBuffer(IT_16BIT,nFace * 3,BU_STATIC_WRITE_ONLY);
					if(!m_pIB)return false;

					ushort *pBuffer = (ushort *)m_pIB->lock(0,0,BL_NORMAL);
					if(pBuffer)
					{
						for(uint i = 0;i < nFace;i++)
						{
							pBuffer[i * 3 + 0] = pFace[i].index[0];
							pBuffer[i * 3 + 1] = pFace[i].index[1];
							pBuffer[i * 3 + 2] = pFace[i].index[2];
						}
						m_pIB->unlock();
					}

					if(m_pBlendProgram && !m_bLoadSkinInfoFromFile )//蒙皮shader有效，且没有从文件读取蒙皮信息
					{
						pFaceBuffer = new MODELFACE[nFace];
						for( uint i=0; i < nFace; ++i)
							pFaceBuffer[i] = pFace[i];
					}
				}
				break;

				case 'MSUB':
				{
					uint nSubMesh = stream.getLength() / sizeof(SUBMESH_V1_3);
					if(nSubMesh)
					{
						SUBMESH_V1_3 *pSubMesh = (SUBMESH_V1_3 *)stream.getBuffer();
						for(uint i = 0;i < nSubMesh;i++)
						{
							SubModel pass;
							pass.indexStart = pSubMesh[i].istart;
							pass.indexCount = pSubMesh[i].icount;
							pass.vertexStart = pSubMesh[i].vstart;
							pass.vertexEnd = pSubMesh[i].vstart + pSubMesh[i].vcount - 1;
							pass.m_matIndex = pSubMesh[i].matId;
							pass.m_bAnimated = pSubMesh[i].animated;
							pass.m_name = pSubMesh[i].name;

							if(pass.m_bAnimated)
								m_bAnimatedGeometry = true; 

							m_vRenderPasses.push_back(pass);
						}
					}
				}
				break;

				case 'MSKN'://蒙皮信息
					{
						uint palleteNum;
						stream.read( &palleteNum, sizeof(palleteNum));
						if( palleteNum != VERTEX_BLEND_MAPPINGS_NUM)//不是256
							break;

						uint validMtxNum;
						stream.read(&validMtxNum, sizeof(validMtxNum));
						if( validMtxNum > VERTEX_BLEND_MATRICES_NUM) //超出支持的骨骼数
							break;

						uint meshNum;
						stream.read(&meshNum, sizeof(meshNum));
						if( meshNum != m_vRenderPasses.size() )//m_vRenderPasses没有被读取
							break;

						for( uint i=0; i<meshNum; ++i)
						{
							stream.read( &m_vRenderPasses[i].m_ValidIndices, sizeof(uchar));
							stream.read(&m_vRenderPasses[i].m_BindedBonesIndices, validMtxNum );
							stream.read(&m_vRenderPasses[i].m_BonesMappings,VERTEX_BLEND_MAPPINGS_NUM);
						}

						m_bLoadSkinInfoFromFile = true;
					}
					break;

				case 'MMTX':
				{
					uint numMaterials;
					stream.read(&numMaterials,sizeof(numMaterials));
					for(uint i = 0;i < numMaterials;i++)
					{
						Material mtl;
						Material *pMaterial = &mtl;
						Assert(pMaterial != 0);

						uchar matNameLen;
						stream.read(&matNameLen,sizeof(matNameLen));
						char matName[128];
						stream.read(matName,matNameLen);
						matName[matNameLen] = 0;
						pMaterial->setName(matName);

						bool lighting;
						stream.read(&lighting,sizeof(lighting));

						color ambient;
						stream.read(&ambient,sizeof(ambient));

						color diffuse;
						stream.read(&diffuse,sizeof(diffuse));

						color specular;
						stream.read(&specular,sizeof(specular));

						color emissive;
						stream.read(&emissive,sizeof(emissive));

						bool iTrans;
						stream.read(&iTrans,sizeof(iTrans));

						bool twoSide;
						stream.read(&twoSide,sizeof(twoSide));

						uint nTexture;
						stream.read(&nTexture,sizeof(nTexture));
						for(uint j = 0;j < nTexture;j++)
						{
							MaterialLayer layer;

							layer.m_bTwoSide = twoSide;

							uchar filenameLen;
							stream.read(&filenameLen,sizeof(filenameLen));
							char str[128];
							stream.read(str,filenameLen);
							str[filenameLen] = 0;

							CPath path (m_strName);
							CPath pathP (path.getParentDir());
							if(!pathP.empty())
							{
								pathP.addTailSlash();
								pathP += str;
							}
							else
							{
								pathP = str;
							}
							layer.m_szTexture = pathP.c_str();

							uchar opType;
							stream.read(&opType,sizeof(opType));
							int bm = opType;

							layer.m_blendMode = (BlendMode)bm;
							pMaterial->addLayer(layer);
						}
						m_vMaterials.push_back(mtl);
					}

					size_t size = m_vRenderPasses.size();
					for(size_t j = 0;j < size;j++)
					{
						SubModel& subModel = m_vRenderPasses[j];
						size_t mtlSize = m_vMaterials.size();
						for(size_t k = 0;k < mtlSize;k++)
						{
							if(subModel.m_matIndex == k)
							{
								subModel.setMaterial(m_vMaterials[k]);
								break;
							}
						}
					}
				}
				break;

				case 'MANI':
				{
					uchar filenameLen;
					stream.read(&filenameLen, sizeof(filenameLen));
					char str[128];
					stream.read(str, filenameLen);
					str[filenameLen] = 0;

					CPath path1,path2;
					path1 = str;
					path1 = path1.getFileName();
					path2 = m_strName;
					path2 = path2.getParentDir();
					if( !path2.empty() )
					{
						path2.addTailSlash();
						path2 += path1;
					}
					else path2 = path1;
					m_pBoneAnimation = BoneAnimationSetManager::getInstance().loadBoneAnimationSet(path2.c_str());
					if( m_pBoneAnimation == 0 )//加载动作失败
						return false;					
				}
				break;		
			}
			pChunk = chunk.nextChunk(pStream);
		}

		//读取非编
		//...

		//初始化硬件顶点的混合
		PP_BY_NAME_START("ModelCodecMx::decode::initForGPUVertexBlend");
		initForGPUVertexBlend(pFaceBuffer);
		PP_BY_NAME_STOP();
		safeDeleteArray( pFaceBuffer);

		//设置内存大小
		m_memorySize += sizeof(*this);
		m_memorySize += m_vRenderPasses.size() * sizeof(SubModel);
		m_memorySize += m_vParticleEmitters.size() * sizeof(ParticleEmitter2Data);
		m_memorySize += m_vRibbonEmitters.size() * sizeof(RibbonEmitterData);
		size_t size = m_vMaterials.size();
		for(size_t i = 0;i < size;i++)
		{
			m_memorySize += m_vMaterials[i].getMemorySize();
		}
		m_memorySize += m_ui32VerticeNum * sizeof(_VertexData);

		if( !m_pBoneAnimation ) return false;

		return true;
	}
	//用于将模型写到文件
	bool ModelCodecMx::write(string filename,float fCopress)
	{
		return m_pBoneAnimation->write(filename,fCopress);
	}
	void ModelCodecMx::loadFeiBian(xs::Stream *pStream,const std::string& animationName)
	{
		//this function will not be realized
		return;
	}

	void  ModelCodecMx::calcAABB(_VertexData *pVtx,uint num,AABB& aabb,Sphere& sphere)
	{
		if(!num)return;

		Vector3 vMin = Vector3(pVtx[0].pos[0],pVtx[0].pos[1],pVtx[0].pos[2]);
		Vector3 vMax = vMin;

		for(uint i = 1;i < num;i++)
		{
			Vector3 v = Vector3(pVtx[i].pos[0],pVtx[i].pos[1],pVtx[i].pos[2]);
			Vector3	*p = &v;

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

}
