#include "StdAfx.h"
#include "ModelCodecTx.h"
#include "RenderEngine/MzHeader.h"

namespace xs
{
	template<class T,class OT> void readKeyFrames(Stream *pStream,KeyFrames<T> *pKeyFrames,uint lastTime = 0)
	{
		uint numKeyFrames;
		pStream->read(&numKeyFrames,sizeof(numKeyFrames));
		for(uint i = 0;i < numKeyFrames;i++)
		{
			int time;
			OT data;
			pStream->read(&time,sizeof(time));
			pStream->read(&data,sizeof(data));
			KeyFrame<T> keyFrame;
			keyFrame.time = lastTime + time;
			keyFrame.v = data;
			keyFrame.in = data;
			keyFrame.out = data;
			pKeyFrames->addKeyFrame(keyFrame);
		}
	}
	template<class T,class OT> void writeKeyFrames(Stream * pDataStream,KeyFrames<T> *pKeyFrames,uint lastTime = 0)//谢学辉编写，慎用
	{
		uint numKeyFrames = pKeyFrames->numKeyFrames();
		pDataStream->write( &numKeyFrames, sizeof(numKeyFrames) );
		for( uint i = 0; i < numKeyFrames; i++)
		{
			KeyFrame<T> * kf;
			kf = pKeyFrames->getKeyFrame( i );
			int time =  kf->time - lastTime;
			pDataStream->write( &time, sizeof(time));
			OT data =(OT) kf->v;
			pDataStream->write( &data, sizeof(data) );
		}
	}

	template<class T>void readMmKeyFrames(Stream *pStream,KeyFrames<T>& keyFrames)//谢学辉编写，用于将Mm粒子系统的关键帧写到stream对象
	{
		MmAnimationBlock<T> mab;
		pStream->read(&mab.interpolationType,sizeof(mab.interpolationType));
		pStream->read(&mab.globalSequenceID,sizeof(mab.globalSequenceID));
		keyFrames.setInterpolationType(mab.interpolationType);
		keyFrames.setGlobalSequenceId(mab.globalSequenceID);
		uint numRanges,numTimes,numKeys;
		pStream->read(&numRanges,sizeof(numRanges));
		pStream->read(&numTimes,sizeof(numTimes));
		pStream->read(&numKeys,sizeof(numKeys));
		
		Assert(numTimes == numKeys);

		for(uint i = 0;i < numRanges;i++)
		{
			AnimRange ar;
			pStream->read(&ar,sizeof(ar));

			KeyFrameRange keyFrameRange;
			keyFrameRange.first = ar.first;
			keyFrameRange.second = ar.second;
			//keyFrames.addKeyFrameRange(keyFrameRange);
		}
		uint *pTime = new uint[numTimes];
		bool test = pStream->read(pTime,sizeof(uint) * numTimes);
		T *pKey = new T[numKeys];
		pStream->read(pKey,sizeof(T) * numKeys);
		for(uint i = 0;i < numTimes;i++)
		{
			KeyFrame<T> keyFrame(pTime[i],pKey[i]);
			keyFrames.addKeyFrame(keyFrame);
		}
		safeDeleteArray(pTime);
		safeDeleteArray(pKey);
	}

	template<class T> void writeMmKeyFrames(Stream *pDataStream,KeyFrames<T>& keyFrames)//谢学辉编写，用于将Mm粒子系统的关键帧写到stream对象
	{
		MmAnimationBlock<T> mab;
		mab.interpolationType = keyFrames.getInterpolationType();
		mab.globalSequenceID = keyFrames.getGlobalSequenceId();
		pDataStream->write( &mab.interpolationType, sizeof(mab.interpolationType));
		pDataStream->write( &mab.globalSequenceID, sizeof(mab.globalSequenceID));
		uint numRanges,numTimes,numKeys;
		numRanges = keyFrames.numKeyFrameRanges();
		pDataStream->write( &numRanges, sizeof(numRanges) );
		numTimes = numKeys = keyFrames.numKeyFrames();
		pDataStream->write( &numTimes, sizeof( numTimes) );
		pDataStream->write( &numKeys, sizeof( numKeys) );

		for( uint i = 0; i < numRanges; i++)
		{
			AnimRange ar;
			KeyFrameRange kfr = * keyFrames.getKeyFrameRange(i);
			ar.first = kfr.first;
			ar.second = kfr.second;
			pDataStream->write( &ar, sizeof(ar) );
		}

		for( uint i = 0 ; i < numTimes; i++)
		{
			uint time = keyFrames.getKeyFrame(i)->time;
			pDataStream->write(&time,sizeof(time));
		}

		for( uint i = 0; i < numTimes; i++)
		{
			T key = keyFrames.getKeyFrame(i)->v;
			pDataStream->write(&key, sizeof(key) );
		}
	}









	uint ModelCodecTx::getNumSubModels()
	{
		return m_vRenderPasses.size();
	}

	ISubModel*	ModelCodecTx::getSubModel(const char* name)
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

	ISubModel*	ModelCodecTx::getSubModel(int index)
	{
		if(index < 0 || index >= (int)m_vRenderPasses.size())return 0;

		return &m_vRenderPasses[index];
	}

	Animation* ModelCodecTx::getAnimation(uint index)
	{
		if(index >= m_vAnimations.size())return 0;
		return m_vAnimations[index];
	}

	uint	ModelCodecTx::getAnimationCount()
	{
		return m_vAnimations.size();
	}

	Animation* ModelCodecTx::hasAnimation(const std::string& animation)
	{
		HASH_MAP_NAMESPACE::hash_map<std::string,Animation*>::iterator it = m_AnimationMap.find(animation.c_str());
		if(it == m_AnimationMap.end())return 0;

		return it->second;
	}


	CoreSkeleton * ModelCodecTx::getCoreSkeletion()
	{
		return &m_skeleton;
	}

	IIndexBuffer*	ModelCodecTx::getIndexBuffer()
	{
		return m_pIB;
	}

	IVertexBuffer*	ModelCodecTx::getTexcoordBuffer()
	{
		return m_pVBTexcoord;
	}

	uint ModelCodecTx::getNumFaces()
	{
		return m_pIB ? m_pIB->getNumIndices() / 3 : 0;
	}

	uint ModelCodecTx::getNumVertices()
	{
		return m_ui32VerticeNum;
	}

	const std::string& ModelCodecTx::getFileName()
	{
		return m_strName;
	}

	uint ModelCodecTx::getVer()
	{
		return m_ver;
	}

	IVertexBuffer* ModelCodecTx::vertexBlend(Skeleton *pSkeleton,const AnimationTime *pTime)
	{
		if(!m_bAnimatedGeometry)return m_pVBPosNormal;
		if(!m_ui32VerticeNum || !pTime || !pSkeleton)return 0;

		//int index = pTime->current * 3 / 100;
		//int index = (pTime->current - pTime->start) * 3 / 100;//动画每秒30帧，最多持续20秒
		int index = (pTime->current * 3 /100) % TX_PRECOMPUTED_VBO_COUNT_MAX; //动画每秒30帧，最多持续34秒， 也就是总共有：30*34帧
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
			Bone* pBonesPointer[255];
			for( uint i=0; i< boneCount; ++i)
				pBonesPointer[i] = pSkeleton->getBone(i);

			Vector3 zero(0,0,0);
			for(uint i = 0;i < m_ui32VerticeNum;i++)
			{
				Vector3 v(0,0,0),n(0,0,0);
				_VertexData *ov = m_pVertexData + i;

				for(uint b = 0;b < 4;b++) 
				{
					if(ov->weights[b] > 0.0000001&&ov->bones[b]< boneCount ) //非常小的数
					{
						Vector3 tv,tn;
						Vector3 otv = Vector3(ov->pos[0],ov->pos[1],ov->pos[2]);
						Vector3 otn = Vector3(ov->normal[0],ov->normal[1],ov->normal[2]);
						Bone *pBone = pBonesPointer[ov->bones[b]];//pSkeleton->getBone(ov->bones[b]);
						if( pBone )
						{
							tv = pBone->getFullTransform() * otv;
							tn = pBone->getFullRotation() * otn;
							v += tv * ((float)ov->weights[b]);
							n += tn * ((float)ov->weights[b]);						
						}

						//Vector3 tv,tn;
						//Vector3 otv = Vector3(ov->pos[0],ov->pos[1],ov->pos[2]);
						//Vector3 otn = Vector3(ov->normal[0],ov->normal[1],ov->normal[2]);
						//Bone *pBone = pSkeleton->getBone(ov->bones[b]);
						//tv = pBone->getFullTransform() * otv;

						//
						//tn = pBone->getFullRotation() * otn;

						//v += tv * ((float)ov->weights[b]);
						//n += tn * ((float)ov->weights[b]);
					}
				}
				if( v == zero && n == zero ) 
				{
					v = ov->pos;
					n = ov->normal;
				}
				pSkinPos[i] = v;
				n.normalize();
				pSkinNormal[i] = n;
			}
			pVB->unlock();
		}

		return pVB;
	}

	uint ModelCodecTx::numParticleSystem()
	{
		return m_vParticleEmitters.size();
	}

	ParticleEmitter2Data*	ModelCodecTx::getParticleSystemData(uint index)
	{
		if(index >= m_vParticleEmitters.size())return 0;
		return m_vParticleEmitters[index];
	}
	
	uint ModelCodecTx::numRibbonSystem()
	{
		return m_vRibbonEmitters.size();
	}

	RibbonEmitterData*	ModelCodecTx::getRibbonSystemData(uint index)
	{
		if(index >= m_vRibbonEmitters.size())return 0;
		return m_vRibbonEmitters[index];
	}

	uint ModelCodecTx::numMmParticleSystem()//谢学辉修改
	{
		//return 0;
		return m_vMmParticleSystemData.size();
	}

	MmParticleSystemData* ModelCodecTx::getMmParticleSystem(uint index)//谢学辉修改
	{
		//return 0;
		if( index >= m_vMmParticleSystemData.size() ) return 0;
		else return m_vMmParticleSystemData[index];
	}

	const AABB&	ModelCodecTx::getAABB()
	{
		if( !m_bCalculatedAABB)
		{
			if( m_pVertexData)  calcAABB(m_pVertexData,m_ui32VerticeNum,m_AABB,m_BoundingSphere);
			m_bCalculatedAABB =true; 
		}
		return m_AABB;
	}

	const Sphere& ModelCodecTx::getBoudingSphere()
	{
		if( !m_bCalculatedAABB)
		{
			if( m_pVertexData) calcAABB(m_pVertexData,m_ui32VerticeNum,m_AABB,m_BoundingSphere);
			m_bCalculatedAABB = true;
		}
		return m_BoundingSphere;
	}

	const char*	ModelCodecTx::getType() const
	{
		static std::string strType = "tx";
		return strType.c_str();
	}

	void ModelCodecTx::loadFeiBian(xs::Stream *pStream,const std::string& animationName)
	{
		uint lastTime = 0;
		Animation *pAnimation = 0;
		xs::DataChunk chunk;
		xs::DataChunk::stChunk *pChunk = chunk.beginChunk(pStream);
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
					uint ver;
					stream.read(&ver,sizeof(ver));
					if(ver != m_ver)return;
				}
				break;
			case 'MANM':
				{
					uint startTime,endTime;
					stream.read(&startTime,sizeof(startTime));
					stream.read(&endTime,sizeof(endTime));
					if(!m_vAnimations.empty())
					{
						Animation *pAnimation = m_vAnimations[m_vAnimations.size() - 1];
						lastTime = pAnimation->getTimeEnd() + 1;
					}
					pAnimation = new Animation(lastTime + startTime,lastTime + endTime,true,animationName.c_str());
					m_vAnimations.push_back(pAnimation);

					if(pAnimation)m_AnimationMap[animationName] = pAnimation;
				}
				break;
			case 'MBON':
				{
					if(!pAnimation)return;

					uint numBones;
					stream.read(&numBones,sizeof(numBones));
					for(uint i = 0;i < numBones;i++)
					{
						int id;
						stream.read(&id,sizeof(id));
						int parentId;
						stream.read(&parentId,sizeof(parentId));
						KeyFrames<Vector3> keyFramesTranslation;
						readKeyFrames<Vector3,Vector3>(&stream,&keyFramesTranslation,lastTime);
						KeyFrames<Quaternion> keyFramesRotation;
						readKeyFrames<Quaternion,Quaternion>(&stream,&keyFramesRotation,lastTime);
						KeyFrames<Vector3> keyFramesScale;
						readKeyFrames<Vector3,Vector3>(&stream,&keyFramesScale,lastTime);

						BoneData *pBone = m_vBones[i];
						if(!pBone)return;

						size_t size = keyFramesTranslation.numKeyFrames();
						for(size_t i = 0;i < size;i++)
						{
							KeyFrame<Vector3>& kf = *keyFramesTranslation.getKeyFrame(i);
							pBone->translation.addKeyFrame(kf);
						}
						size = keyFramesRotation.numKeyFrames();
						for(size_t i = 0;i < size;i++)
						{
							KeyFrame<Quaternion>& kf = *keyFramesRotation.getKeyFrame(i);
							pBone->rotation.addKeyFrame(kf);
						}
						size = keyFramesScale.numKeyFrames();
						for(size_t i = 0;i < size;i++)
						{
							KeyFrame<Vector3>& kf = *keyFramesScale.getKeyFrame(i);
							pBone->scale.addKeyFrame(kf);
						}
						bool b;
						stream.read(&b,sizeof(b));
						stream.read(&b,sizeof(b));
					}
				}
				break;
			}
			pChunk = chunk.nextChunk(pStream);
		}
	}

	//对硬件蒙皮的支持
	IVertexBuffer *	ModelCodecTx::getBlendWeightBuffer()
	{
		return m_pBlendWeightBuffer;
	}

	IVertexBuffer * ModelCodecTx::getBlendIndicesBuffer(uint subModelIndex)
	{
		if( subModelIndex >= m_vBlendIndices.size() ) return 0;
		else return m_vBlendIndices[subModelIndex];
	}

	IVertexBuffer * ModelCodecTx::getOriginVertexBuffer() 
	{
		return m_pVBPosNormal;
	}

	IShaderProgram * ModelCodecTx::getBlendShaderProgram()
	{
		return m_pBlendProgram;
	}

	//创建骨架
	void ModelCodecTx::createCoreSkeleton()
	{
		size_t boneCount = m_vBones.size();
		//需要保证不同的骨头的objectid不相同
		typedef std::map<int, CoreBone*> MapObjectId2CoreBone;
		MapObjectId2CoreBone mapFindParent;
		for( size_t i = 0; i < boneCount; ++i)
		{
			CoreBone * pCoreBone = m_skeleton.createCoreBone();
			pCoreBone->setName( m_vBones[i]->name);
			pCoreBone->setPivot( m_vBones[i]->pivotPoint);
			pCoreBone->setPivotRotation( m_vBones[i]->pivotRotation);
			xs::Matrix4 initMtx = xs::Matrix4::IDENTITY;
			initMtx.makeTrans( m_vBones[i]->initTrans);
			initMtx = initMtx * m_vBones[i]->initQuat;
			pCoreBone->setInitMatrix(initMtx);
			pCoreBone->setPrecomputeMatrix( m_vBones[i]->precomputeMtx);
			pCoreBone->setKeyFramesTranslation(m_vBones[i]->translation);
			pCoreBone->setKeyFramesRotation( m_vBones[i]->rotation);
			pCoreBone->setKeyFramesScale( m_vBones[i]->scale);
			pCoreBone->m_pMKTM = &m_vBones[i]->m_TMVector;
			pCoreBone->setCanBufferData(false);//不能缓存，特效都是用线性插值的，缓存则数据量很大，而且骨骼数少，计算量少
			mapFindParent[m_vBones[i]->objectId] = pCoreBone;//缓存Parent

			//初始化关键时间
			size_t transKeyFrameNum = m_vBones[i]->translation.numKeyFrames();
			size_t rotKeyFrameNum = m_vBones[i]->rotation.numKeyFrames();
			size_t scalKeyFrameNum = m_vBones[i]->scale.numKeyFrames();
			std::list<uint> transKeyTime;
			std::list<uint> rotKeyTime;
			std::list<uint> scalKeyTime;
			std::list<uint> aniStartKeyTime;

			for(size_t j = 0; j < transKeyFrameNum; ++j)
				transKeyTime.push_back( m_vBones[i]->translation.getKeyFrame(j)->time);
			for( size_t j = 0; j < rotKeyFrameNum; ++j)
				rotKeyTime.push_back( m_vBones[i]->rotation.getKeyFrame(j)->time);
			for( size_t j = 0; j <scalKeyFrameNum; ++j)
				scalKeyTime.push_back( m_vBones[i]->scale.getKeyFrame(j)->time);
			for( size_t j = 0; j <m_vAnimations.size(); ++j)
			{
				aniStartKeyTime.push_back( m_vAnimations[j]->getTimeStart() );
			}
			
			scalKeyTime.merge(aniStartKeyTime);
			scalKeyTime.unique();
			scalKeyTime.merge(rotKeyTime);
			scalKeyTime.unique();
			scalKeyTime.merge(transKeyTime);
			scalKeyTime.unique();
			
			for( std::list<uint>::iterator it = scalKeyTime.begin(); it != scalKeyTime.end(); ++it)
			{
				pCoreBone->m_keyTime.push_back( *it);
			}

			size_t totalKeyTimeNum = pCoreBone->m_keyTime.size();
			size_t aniNum = m_vAnimations.size();
			std::vector<uint> aniStartIndex(aniNum, 0);
			std::vector<uint> aniContainKeyTime(aniNum, 0);		
			size_t curKeyTimeIndex = 0;
			for(size_t j=0; j <aniNum; ++j)
			{
				aniStartIndex[j] = curKeyTimeIndex;
				uint aniTimeEnd = m_vAnimations[j]->getTimeEnd();
				for( size_t k = curKeyTimeIndex; k<totalKeyTimeNum; )
				{
					if( pCoreBone->m_keyTime[k] <= aniTimeEnd )
					{
						++aniContainKeyTime[j];
						++k;
					}
					else
					{
						curKeyTimeIndex = k;
						break;
					}			
				}		
			}
			for( size_t j =0; j <aniNum; ++j)
			{
				std::pair<uint, uint> aniPair;
				aniPair.first = aniStartIndex[j];
				aniPair.second = aniContainKeyTime[j];
				pCoreBone->m_keyTimeIndex[m_vAnimations[j]->getTimeStart()] = aniPair;
			}
		}

		//设定父骨骼
		for( size_t i =0; i < boneCount; ++i)
		{
			MapObjectId2CoreBone::iterator it = mapFindParent.find(m_vBones[i]->parentId);
			if( it != mapFindParent.end() )
			{
				m_skeleton.getCoreBone(i)->setParent( it->second);
			}
			//int parentId = m_vBones[i]->parentId;
			//if( -1 !=parentId )
			//{
			//	for( size_t j=0; j<boneCount; ++j)
			//	{
			//		if( m_vBones[j]->objectId == parentId )
			//		{
			//			m_skeleton.getCoreBone(i)->setParent( m_skeleton.getCoreBone(j));
			//			break;
			//		}
			//	}
			//}
		}	
	}

	void ModelCodecTx::initForGPUVertexBlend(MODELFACE* pFaceBuffer)
	{
		//检查
		if( 0 == m_pBlendProgram ) 
			return;

		if( !m_bLoadSkinInfoFromFile && 0 == pFaceBuffer)
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
			if( m_vBones.size()< VERTEX_BLEND_MATRICES_NUM )//不用拆分
			{
				uint passCount = m_vRenderPasses.size();
				uint bonesCount = m_vBones.size();
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
			else//需要拆分子mesh
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

	bool ModelCodecTx::decode(xs::Stream *pStream)
	{
		PP_BY_NAME( "ModelCodecTx::decode" );

		uint ver = 0;
		xs::DataChunk chunk;
		xs::DataChunk::stChunk *pChunk = chunk.beginChunk(pStream);

		//对于version13，有以下扩展
		/*
		1， MM粒子保存贴图遍历范围
		*/

		//对于version12，有以下扩展
		/*
		1， 普通粒子和MM粒子绑定到骨骼渲染时需要考虑骨骼初始矩阵
		*/
		//对于version11，有以下扩展
		/*
		1， 普通粒子和MM粒子可以绕Z轴旋转
		*/

		//对于version10，有以下扩展
		/*
		1， 可以设置材质属性
		*/

		//对于version 9, 有以下扩展
		/*
		1, 容许用户指定飘带方向
		*/

		//对于version 8, 有以下扩展
		/*
		1, 容许用户自定义渲染顺序
		2, 飘带系统增加了消失模式的选项
		*/

		//对于version 7, 有以下扩展
		/*
		1, 加入了粒子系统和飘带系统的是否绑定到骨骼的选项
		*/


		//对于version 6，有以下扩展
		/*
		1,加入了计算aabb
		2,加入了判断是否要骨骼插值
		3,加入了设置bouding rect
		*/


		//对于version5， 有以下扩展
		/*
		1, 粒子，飘带，增加了增强亮度的选项
		2，粒子飘带增加了同步放大的选项
		3，材质增加了增强亮度的选项，（材质同样增加了被遮挡在alpha混合后面的additive+ alpha选项)
		*/

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
					if(ver < 1)
					{
						return false;
					}
				}
				break;
			//这个扩展只有版本4以及4以上的才有
			//放大的倍数这个功能现在已经不用了。
			//case 'MEXT': //对特效文件的扩展，此扩展必须放在文件chunk的前面，否则就无效。扩展在将来可能会在增加啊
			//	{
			//		//定义扩展所占用的空间为64byte
			//		//前12字节为模型的初始放大倍数
			//		uchar ucValiByte;
			//		stream.read( &ucValiByte, sizeof(ucValiByte));
			//		if( ucValiByte == 12 )
			//		{
			//			stream.read( &m_fInitScaleX, sizeof(m_fInitScaleX));
			//			stream.read( &m_fInitScaleY, sizeof(m_fInitScaleY));
			//			stream.read( &m_fInitScaleZ, sizeof(m_fInitScaleZ));
			//			char * pcPad = new char[ 64 - sizeof(float)*3];
			//			stream.read(pcPad, 64- 3*sizeof(float) );
			//			delete [] pcPad;
			//		}
			//		else
			//		{
			//			char * pcPad = new char[64];
			//			stream.read(pcPad, 64);
			//			delete [] pcPad;
			//		}
			//	}
			//	break;
			case 'MVTX':
				{
					m_ui32VerticeNum = stream.getLength() / sizeof(_VertexData);
					m_pVertexData = new _VertexData[m_ui32VerticeNum];
					if( m_pVertexData == 0 ) return false;
					memcpy(m_pVertexData,pChunk->m_pData,pChunk->m_ui32DataSize);
					//这个注释掉，在需要的时候计算
					//calcAABB(m_pVertexData,m_ui32VerticeNum,m_AABB,m_BoundingSphere);

					m_pVBPosNormal = m_pRenderSystem->getBufferManager()->createVertexBuffer(12,2 * m_ui32VerticeNum, BU_STATIC_WRITE_ONLY /*BU_DYNAMIC_WRITE_ONLY_DISCARDABLE*/);
					if(!m_pVBPosNormal)return false;

					float* pBuffer = (float*)m_pVBPosNormal->lock(0,0, BL_NORMAL /*BL_DISCARD*/);
					if(pBuffer)
					{
						for(uint i = 0;i < m_ui32VerticeNum;i++)
						{
							*pBuffer++ = m_pVertexData[i].pos[0];
							*pBuffer++ = m_pVertexData[i].pos[1];
							*pBuffer++ = m_pVertexData[i].pos[2];
							int l = 0;
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
							int l= 0;
						}
						m_pVBTexcoord->unlock();
					}
				}
				break;
			case 'MURP':
				{
					int interType;
					stream.read(&interType, sizeof(interType));
					InterpolationType realInterType = static_cast<InterpolationType>(interType);
					int numuvkfs;
					stream.read(&numuvkfs, sizeof(numuvkfs));
					for( int i =0 ; i< numuvkfs; i++)
					{
						int time;
						stream.read(&time, sizeof(time));
						uint realTime = static_cast<uint>(time);
						int numuvs;
						stream.read(&numuvs, sizeof(numuvs));
						for( int j =0; j < numuvs; j++)
						{
							if( m_vUVKFs.size() == 0 )
							{
								m_vUVKFs.resize( numuvs, KeyFrames< Vector2 >() );
							}
							float x,y;
							stream.read(&x,sizeof(x));
							stream.read(&y,sizeof(y));
							Vector2 vc2(x,y);
							KeyFrame<Vector2> kfvc2(realTime, vc2);
							m_vUVKFs[j].addKeyFrame(kfvc2);
							m_vUVKFs[j].setInterpolationType(realInterType);
						}
					}
					if( m_vUVKFs.size() > 0 && m_vUVKFs[0].numKeyFrames() > 0  ) m_bAnimatedTexCoord = true;
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

					if(m_pBlendProgram && !m_bLoadSkinInfoFromFile)//蒙皮shader有效，且没有从文件读取蒙皮信息
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
							bool bSave = true;
							m_vShouldSavePasses.push_back(bSave);
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
					if( m_ver <= 9 )
					{
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

							//获取材质的alpha值
							uint uiInterType;
							stream.read(&uiInterType, sizeof(uiInterType));
							pMaterial->m_kfTrans.setInterpolationType( static_cast<InterpolationType>(uiInterType));
							uint numOpacKFs;
							stream.read(&numOpacKFs, sizeof(numOpacKFs));
							for( uint j=0; j < numOpacKFs; j++)
							{
								uint time;
								float v,in,out;
								stream.read(&time, sizeof(time) );
								stream.read(&v,sizeof(v));
								stream.read(&in, sizeof(in));
								stream.read(&out, sizeof(out));
								KeyFrame<float> kf(time, v, in, out);
								pMaterial->m_kfTrans.addKeyFrame( kf);
							}

							uint nTexture;
							stream.read(&nTexture,sizeof(nTexture));
							for(uint j = 0;j < nTexture;j++)
							{
								MaterialLayer layer;
								layer.m_bNoDepthSet = true;//如果没有设置是否写深度，则设置不写深度
								layer.m_bTwoSide = twoSide;
								layer.m_bUnshaded = lighting ?true:false;
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

								//读取uoffs
								uint uiUInterType;
								stream.read(&uiUInterType, sizeof(uiUInterType));
								layer.m_kfUOffs.setInterpolationType( static_cast<InterpolationType> (uiUInterType) );
								uint numUKFs;
								stream.read(&numUKFs, sizeof(numUKFs));
								for( uint k = 0; k< numUKFs; k++)
								{
									uint time;
									float v, in, out;
									stream.read(&time, sizeof(time));
									stream.read(&v, sizeof(v));
									stream.read(&in, sizeof(in));
									stream.read(&out, sizeof(out));
									KeyFrame<float> kf(time, v, in, out);
									layer.m_kfUOffs.addKeyFrame(kf);						
								}
								if( layer.m_kfUOffs.numKeyFrames() > 0 ) m_bAnimatedTexCoord  = true;

								//读取voffs
								uint uiVInterType;
								stream.read(&uiVInterType, sizeof(uiVInterType));
								layer.m_kfVOffs.setInterpolationType( static_cast<InterpolationType> (uiVInterType) );
								uint numVKFs;
								stream.read(&numVKFs, sizeof(numVKFs));
								for(uint k =0; k < numVKFs ; k++)
								{
									uint time;
									float v, in, out;
									stream.read(&time, sizeof(time));
									stream.read(&v, sizeof(v));
									stream.read(&in, sizeof(in));
									stream.read(&out, sizeof(out));
									KeyFrame<float> kf(time, v,in,out);
									layer.m_kfVOffs.addKeyFrame(kf);
								}
								if(layer.m_kfVOffs.numKeyFrames() > 0 )  m_bAnimatedTexCoord  = true;

								/////////////////////////////////
								//对材质的扩展，只有version 5 以上才有
								if( m_ver >= 5)
								{
									uint uiExtSize;
									stream.read(&uiExtSize, sizeof(uiExtSize));
									if( uiExtSize == sizeof(bool) )
									{
										//version 5 第一次扩展
										bool _bEnhanceValue;
										stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
										layer.m_bEnhanceValue = _bEnhanceValue;
									}

								}
								////////////////////////////////

								pMaterial->addLayer(layer);
							}

							m_vMaterials.push_back(mtl);
						}
					}
					else
					{
						for( uint i =0; i< numMaterials; ++i)
						{
							Material mtl;
							Material* pMaterial = &mtl;
							//材质名称
							uchar matNameLen;
							stream.read(&matNameLen,sizeof(matNameLen));
							char matName[128];
							stream.read(matName,matNameLen);
							matName[matNameLen] = 0;
							pMaterial->setName(matName);
							//材质alpha值
							uint uiInterType;
							stream.read(&uiInterType, sizeof(uiInterType));
							pMaterial->m_kfTrans.setInterpolationType( static_cast<InterpolationType>(uiInterType));
							uint numOpacKFs;
							stream.read(&numOpacKFs, sizeof(numOpacKFs));
							for( uint j=0; j < numOpacKFs; j++)
							{
								uint time;
								float v,in,out;
								stream.read(&time, sizeof(time) );
								stream.read(&v,sizeof(v));
								stream.read(&in, sizeof(in));
								stream.read(&out, sizeof(out));
								KeyFrame<float> kf(time, v, in, out);
								pMaterial->m_kfTrans.addKeyFrame( kf);
							}
							//材质子层
							uint nTexture;
							stream.read(&nTexture,sizeof(nTexture));
							for( uint j=0; j< nTexture; ++j)
							{
								MaterialLayer layer;
								//纹理名称
								uchar filenameLen;
								stream.read(&filenameLen,sizeof(filenameLen));
								char str[128];
								stream.read(str,filenameLen);
								str[filenameLen] = 0;
								CPath path (m_strName);
								CPath pathP ( path.getParentDir());
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
								//混合模式
								uchar bm;
								stream.read(&bm,sizeof(bm));
								layer.m_blendMode = (BlendMode)bm;
								//双面材质
								uchar twoSide;
								stream.read(&twoSide, sizeof(twoSide));
								layer.m_bTwoSide = twoSide?true:false;
								//散射光
								color diffuse;
								stream.read(&diffuse,sizeof(diffuse));
								layer.m_diffuse.a = diffuse.a;
								layer.m_diffuse.r = diffuse.r;
								layer.m_diffuse.g = diffuse.g;
								layer.m_diffuse.b = diffuse.b;
								//不使用光照
								uchar unshaded;
								stream.read(&unshaded, sizeof(unshaded));
								layer.m_bUnshaded = unshaded?true:false;
								//不使用雾化
								uchar unfogged;
								stream.read(&unfogged, sizeof(unfogged));
								layer.m_bUnfogged = unfogged?true:false;
								//球面环境映射
								uchar sphereEvnMap;
								stream.read(&sphereEvnMap, sizeof(sphereEvnMap));
								layer.m_SphereEnvironmentMap = sphereEvnMap?true:false;
								//不适用深度检测
								uchar nodepthtest;
								stream.read(&nodepthtest, sizeof(nodepthtest));
								layer.m_bNoDepthTest = nodepthtest?true:false;
								//不写深度
								uchar nodepthset;
								stream.read(&nodepthset, sizeof(nodepthset));
								layer.m_bNoDepthSet = nodepthset?true:false;
								//纹理U方向截取到边
								uchar clamps;
								stream.read(&clamps, sizeof(clamps));
								layer.m_bClampS = clamps?true:false;
								//纹理V方向截取到边
								uchar clampt;
								stream.read(&clampt, sizeof(clampt));
								layer.m_bClampT = clampt?true:false;
								//亮度增强（已经取消)
								uchar enhanceValue;
								stream.read(&enhanceValue,sizeof(enhanceValue));
								layer.m_bEnhanceValue = enhanceValue?true:false;
								//纹理动画
								//读取uoffset
								uint uiUInterType;
								stream.read(&uiUInterType, sizeof(uiUInterType));
								layer.m_kfUOffs.setInterpolationType( static_cast<InterpolationType> (uiUInterType) );
								uint numUKFs;
								stream.read(&numUKFs, sizeof(numUKFs));
								for( uint k = 0; k< numUKFs; k++)
								{
									uint time;
									float v, in, out;
									stream.read(&time, sizeof(time));
									stream.read(&v, sizeof(v));
									stream.read(&in, sizeof(in));
									stream.read(&out, sizeof(out));
									KeyFrame<float> kf(time, v, in, out);
									layer.m_kfUOffs.addKeyFrame(kf);						
								}
								if( layer.m_kfUOffs.numKeyFrames() > 0 ) m_bAnimatedTexCoord  = true;

								//读取voffset
								uint uiVInterType;
								stream.read(&uiVInterType, sizeof(uiVInterType));
								layer.m_kfVOffs.setInterpolationType( static_cast<InterpolationType> (uiVInterType) );
								uint numVKFs;
								stream.read(&numVKFs, sizeof(numVKFs));
								for(uint k =0; k < numVKFs ; k++)
								{
									uint time;
									float v, in, out;
									stream.read(&time, sizeof(time));
									stream.read(&v, sizeof(v));
									stream.read(&in, sizeof(in));
									stream.read(&out, sizeof(out));
									KeyFrame<float> kf(time, v,in,out);
									layer.m_kfVOffs.addKeyFrame(kf);
								}
								if(layer.m_kfVOffs.numKeyFrames() > 0 )  m_bAnimatedTexCoord  = true;
								
								//添加材质层
								pMaterial->addLayer(layer);

							}
							//添加材质
							m_vMaterials.push_back(mtl);
						}					
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
			case 'MANM':
				{
					uint32 nAnims; 
					stream.read(&nAnims,sizeof(nAnims));
					MODELANIMATION *pAnims = (MODELANIMATION *)(stream.getBuffer() + stream.getPosition());

					for(uint i = 0;i < nAnims;i++)
					{
						uchar AnimnameLen; 
						stream.read(&AnimnameLen,sizeof(AnimnameLen));
						char str[64];
						stream.read(str,AnimnameLen);
						str[AnimnameLen] = 0;

						uint startTime,endTime;
						stream.read(&startTime,sizeof(startTime));
						stream.read(&endTime,sizeof(endTime));

						Animation *pAnimation = new Animation(startTime,endTime,true,str);
						//modified by xxh ， 下面那样写不好，会使得m_vAnimations和m_AnimationMap不一致，改为这样。
						if( pAnimation)
						{
							m_vAnimations.push_back(pAnimation);
							if( pAnimation && m_AnimationMap.find( pAnimation->getName() ) == m_AnimationMap.end() )
								m_AnimationMap[pAnimation->getName() ] = pAnimation;
						}

						//m_vAnimations.push_back(pAnimation);
						//if(pAnimation)m_AnimationMap[str] = pAnimation;
					}
				}
				break;
			case 'MBON':
				{
					uint nBones;
					stream.read(&nBones,sizeof(nBones));

					for(uint i = 0;i < nBones;i++)
					{
						int id;
						stream.read(&id,sizeof(id));

						uchar JointnameLen; 
						stream.read(&JointnameLen,sizeof(JointnameLen));
						char str[81];
						stream.read(str,JointnameLen);
						str[JointnameLen] = 0;

						int parent;
						stream.read(&parent,sizeof(parent));
						Vector3 pivot;
						stream.read(&pivot,sizeof(pivot));

						BoneData *pData = new BoneData;
						//读取模型的初始变换
						float _t[3],_r[4],_s[3];
						stream.read(_t, sizeof(float)*3);
						stream.read(_r, sizeof(float)*4);
						stream.read(_s, sizeof(float)*3);
						pData->initTrans = Vector3(_t[0],_t[1],_t[2]);
						pData->initQuat = Quaternion(_r[0],_r[1],_r[2],_r[3]);
						pData->initScale = Vector3(_s[0],_s[1],_s[2]);					

						pData->billboarded = false;
						pData->billboardedLockX = false;
						pData->billboardedLockY =false;
						pData->billboardedLockZ = false;
						pData->cameraAnchored = false;
						pData->dontInheritRotation = false;
						pData->dontInheritScaling = false;
						pData->dontInheritTranslation = false;
						strcpy(pData->name,str);
						pData->objectId = id;
						pData->parentId = parent;
						pData->pivotPoint = pivot;

						pData->pivotRotation = false;

						//pData->translation.setInterpolationType(INTERPOLATION_NONE);
						//pData->rotation.setInterpolationType(INTERPOLATION_NONE);
						//pData->scale.setInterpolationType(INTERPOLATION_NONE);

						pData->translation.setInterpolationType(INTERPOLATION_LINEAR);
						pData->rotation.setInterpolationType(INTERPOLATION_LINEAR);
						pData->scale.setInterpolationType(INTERPOLATION_LINEAR);

						uint nKeyframes;
						stream.read(&nKeyframes,sizeof(nKeyframes));
						for(uint j = 0;j < nKeyframes;j++)
						{
							ModelKeyframeTranslation kf;
							stream.read(&kf,sizeof(kf));

							KeyFrame<Vector3> kfTranslation(kf.time,Vector3(kf.v[0],kf.v[1],kf.v[2]));

							pData->translation.addKeyFrame(kfTranslation);
						}
						stream.read(&nKeyframes,sizeof(nKeyframes));
						for(uint j = 0;j < nKeyframes;j++)
						{
							ModelKeyframeRotation kf;
							stream.read(&kf,sizeof(kf));

							KeyFrame<Quaternion> kfRotation(kf.time,Quaternion(kf.q[0],kf.q[1],kf.q[2],kf.q[3]));

							pData->rotation.addKeyFrame(kfRotation);
						}
						stream.read(&nKeyframes,sizeof(nKeyframes));
						for(uint j = 0;j < nKeyframes;j++)
						{
							ModelKeyframeScale kf;
							stream.read(&kf,sizeof(kf));
							Vector3 scal( kf.v[0],kf.v[1],kf.v[2] );

							KeyFrame<Vector3> kfScale(kf.time,scal);

							pData->scale.addKeyFrame(kfScale);
						}

						if( m_ver >= 6)//版本增加了是否插值功能
						{
							uchar ucExt6;
							stream.read(&ucExt6, sizeof(ucExt6));
							if( ucExt6 == sizeof(bool))//只增加的对插值的支持
							{
								stream.read(&m_bShouldBoneInterKeyFrame,sizeof(m_bShouldBoneInterKeyFrame));
								if( m_bShouldBoneInterKeyFrame)
								{
									pData->translation.setInterpolationType(INTERPOLATION_LINEAR);
									pData->rotation.setInterpolationType(INTERPOLATION_LINEAR);
									pData->scale.setInterpolationType(INTERPOLATION_LINEAR);
								}
								else
								{
									pData->translation.setInterpolationType(INTERPOLATION_NONE);
									pData->rotation.setInterpolationType(INTERPOLATION_NONE);
									pData->scale.setInterpolationType(INTERPOLATION_NONE);								
								}
							}
						}
						
						m_vBones.push_back(pData);
					}
				}
				break;

				case 'MBOX': //只有版本6才有，aabb加boudingbox
					{
						float vMin[3];
						float vMax[3];
						stream.read(vMin,sizeof(vMin));
						stream.read(vMax,sizeof(vMax));
						Vector3 v3Min(vMin);
						Vector3 v3Max(vMax);
						m_AABB.setExtents(v3Min,v3Max);
						m_BoundingSphere.setCenter(v3Min.midPoint(v3Max));
						m_BoundingSphere.setRadius((v3Max-v3Min).length()*0.5);
						m_bCalculatedAABB = true;
						float vBox[4];
						stream.read(vBox,sizeof(vBox));
						m_boundingBox.x = vBox[0];
						m_boundingBox.y = vBox[1];
						m_boundingBox.z = vBox[2];
						m_boundingBox.w = vBox[3];			
					}
					break;
				case 'MROR'://只有版本8以及8以上才有
					{
						m_bIsEnableCustomizeRenderOrder = true;
						m_pCustomizeRenderOrderQueue = new std::vector<int>[ECustomizeRenderOrderObject_Max];
						if( !m_pCustomizeRenderOrderQueue) 
						{
							m_bIsEnableCustomizeRenderOrder = false;
							break;
						}
						for( uint i = 0; i<ECustomizeRenderOrderObject_Max; ++i)
						{
							uint num;
							stream.read(&num, sizeof(num) );
							for( uint j =0; j < num; ++j)
							{
								int order;
								stream.read(&order, sizeof(order) );
								m_pCustomizeRenderOrderQueue[i].push_back(order);
							}
						}
					}
					break;

				//将带子系统与骨骼分开，参数基本上和绑定到袋子系统上的一致
				case 'MRIB':
					{
						uint numrs = 0;
						stream.read(&numrs, sizeof(numrs));
						for( uint i =0; i < numrs; i++)
						{
							RibbonEmitterData *pData = new RibbonEmitterData;
							int boneid;
							stream.read(&boneid,sizeof(boneid));
							bool visible;
							stream.read(&visible,sizeof(visible));
							float above;
							stream.read(&above,sizeof(above));
							float below;
							stream.read(&below,sizeof(below));
							short edgePerSecond;
							stream.read(&edgePerSecond,sizeof(edgePerSecond));
							float edgeLife;
							stream.read(&edgeLife,sizeof(edgeLife));
							float gravity;
							stream.read(&gravity,sizeof(gravity));
							short rows;
							stream.read(&rows,sizeof(rows));
							short cols;
							stream.read(&cols,sizeof(cols));
							short slot;
							stream.read(&slot,sizeof(slot));
							Color3 color;
							stream.read(&color,sizeof(color));
							float alpha;
							stream.read(&alpha,sizeof(alpha));
							short blendMode;
							stream.read(&blendMode,sizeof(blendMode));
							uchar textureFilenameLen;
							stream.read(&textureFilenameLen,sizeof(textureFilenameLen));
							char filename[256];
							stream.read(filename,textureFilenameLen);
							filename[textureFilenameLen] = 0;

							pData->visibility.setStaticData(visible);
							pData->heightAbove.setStaticData(above);
							pData->heightBelow.setStaticData(below);
							pData->emissionRate = edgePerSecond;
							pData->lifeSpan = edgeLife;
							pData->gravity = gravity;
							pData->rows = rows;
							pData->columns = cols;
							pData->textureSlot.setStaticData(slot);
							pData->color.setStaticData(color);
							pData->alpha.setStaticData(alpha);
							pData->filterMode = (BlendMode)blendMode;

							CPath path (m_strName);
							CPath pathP (path.getParentDir());
							if(!pathP.empty())
							{
								pathP.addTailSlash();
								pathP += filename;
							}
							pData->textureFilename = pathP.c_str();

							readKeyFrames<float,bool>(&stream,&pData->visibility);
							readKeyFrames<float,float>(&stream,&pData->heightAbove);
							readKeyFrames<float,float>(&stream,&pData->heightBelow);
							readKeyFrames<int,short>(&stream,&pData->textureSlot);
							readKeyFrames<Color3,Color3>(&stream,&pData->color);
							readKeyFrames<float,float>(&stream,&pData->alpha);

							pData->visibility.setInterpolationType(INTERPOLATION_NONE);
							pData->heightAbove.setInterpolationType(INTERPOLATION_LINEAR);
							pData->heightBelow.setInterpolationType(INTERPOLATION_LINEAR);
							pData->textureSlot.setInterpolationType(INTERPOLATION_NONE);
							pData->color.setInterpolationType(INTERPOLATION_LINEAR);
							pData->alpha.setInterpolationType(INTERPOLATION_LINEAR);
							
							ASSERT(boneid >= 0);
							pData->boneObjectId = boneid;

							//对普通粒子的扩展，只有version 5 以上才有
							if( m_ver >= 5)
							{
								uint uiExtSize;
								stream.read(&uiExtSize, sizeof(uiExtSize));
								if( uiExtSize == sizeof(bool) + sizeof(bool) + sizeof(float) *3  )
								{
									//version 5 第一次扩展
									bool _bEnhanceValue;
									stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
									pData->bEnhanceValue = _bEnhanceValue;
									bool _bEnableSynScale;
									stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
									pData->bEnableSynScale = _bEnableSynScale;
									Vector3 vInitSynScale;
									stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
									pData->vInitSynScale = vInitSynScale;	
								}
								else if( uiExtSize == sizeof(bool) + sizeof(bool) + sizeof(float) *3 + sizeof(char)*16)
								{
									//对version5的第二次扩展
									bool _bEnhanceValue;
									stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
									pData->bEnhanceValue = _bEnhanceValue;
									bool _bEnableSynScale;
									stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
									pData->bEnableSynScale = _bEnableSynScale;
									Vector3 vInitSynScale;
									stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
									pData->vInitSynScale = vInitSynScale;	

									char tmpName[16];
									stream.read(tmpName, sizeof(char)*16);
									memcpy(pData->name,tmpName,16);

								}
								else if( uiExtSize == sizeof(bool) + sizeof(bool) + sizeof(float) *3 + sizeof(char)*16 + sizeof(bool) )
								{
									//version 7 加入的绑定到骨骼
									bool _bEnhanceValue;
									stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
									pData->bEnhanceValue = _bEnhanceValue;
									bool _bEnableSynScale;
									stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
									pData->bEnableSynScale = _bEnableSynScale;
									Vector3 vInitSynScale;
									stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
									pData->vInitSynScale = vInitSynScale;	

									char tmpName[16];
									stream.read(tmpName, sizeof(char)*16);
									memcpy(pData->name,tmpName,16);
									bool _bAttach2Bone;
									stream.read(&_bAttach2Bone, sizeof(bool));
									pData->attach2Bone = _bAttach2Bone;							
								}
								else if( uiExtSize == sizeof(bool) + sizeof(bool) + sizeof(float) *3 + sizeof(char)*16 + sizeof(bool) + sizeof(int) )
								{
									//version 8 可以指定飘带的消失模式
									bool _bEnhanceValue;
									stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
									pData->bEnhanceValue = _bEnhanceValue;
									bool _bEnableSynScale;
									stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
									pData->bEnableSynScale = _bEnableSynScale;
									Vector3 vInitSynScale;
									stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
									pData->vInitSynScale = vInitSynScale;	

									char tmpName[16];
									stream.read(tmpName, sizeof(char)*16);
									memcpy(pData->name,tmpName,16);
									bool _bAttach2Bone;
									stream.read(&_bAttach2Bone, sizeof(bool));
									pData->attach2Bone = _bAttach2Bone;	
									int fadeoutmode;
									stream.read(&fadeoutmode,sizeof(int));
									pData->fadeoutmode = fadeoutmode;						
								}
								else if( uiExtSize == sizeof(bool) + sizeof(bool) + sizeof(float) *3 + sizeof(char)*16 + sizeof(bool) + sizeof(int)
									+ sizeof(bool) + sizeof(float)*3 )
								{
									//version 9 可以指定飘带的向上方向
									bool _bEnhanceValue;
									stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
									pData->bEnhanceValue = _bEnhanceValue;
									bool _bEnableSynScale;
									stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
									pData->bEnableSynScale = _bEnableSynScale;
									Vector3 vInitSynScale;
									stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
									pData->vInitSynScale = vInitSynScale;	

									char tmpName[16];
									stream.read(tmpName, sizeof(char)*16);
									memcpy(pData->name,tmpName,16);
									bool _bAttach2Bone;
									stream.read(&_bAttach2Bone, sizeof(bool));
									pData->attach2Bone = _bAttach2Bone;	
									int fadeoutmode;
									stream.read(&fadeoutmode,sizeof(int));
									pData->fadeoutmode = fadeoutmode;
									bool _bEnableAssignUp;
									stream.read(&_bEnableAssignUp, sizeof(bool));
									pData->enableAssignUpDir = _bEnableAssignUp;
									Vector3 vUp;
									stream.read(vUp.val, sizeof(vUp.x)*3);
									pData->vAssignedUpDir = vUp;	
								}
								else if( uiExtSize == sizeof(bool) + sizeof(bool) + sizeof(float) *3 + sizeof(char)*16 + sizeof(bool) + sizeof(int)
									+ sizeof(bool) + sizeof(float)*3 + sizeof(float) )
								{
									//version 10,增加飘带消失速度，运动速度成反比
									bool _bEnhanceValue;
									stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
									pData->bEnhanceValue = _bEnhanceValue;
									bool _bEnableSynScale;
									stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
									pData->bEnableSynScale = _bEnableSynScale;
									Vector3 vInitSynScale;
									stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
									pData->vInitSynScale = vInitSynScale;	

									char tmpName[16];
									stream.read(tmpName, sizeof(char)*16);
									memcpy(pData->name,tmpName,16);
									bool _bAttach2Bone;
									stream.read(&_bAttach2Bone, sizeof(bool));
									pData->attach2Bone = _bAttach2Bone;	
									int fadeoutmode;
									stream.read(&fadeoutmode,sizeof(int));
									pData->fadeoutmode = fadeoutmode;
									bool _bEnableAssignUp;
									stream.read(&_bEnableAssignUp, sizeof(bool));
									pData->enableAssignUpDir = _bEnableAssignUp;
									Vector3 vUp;
									stream.read(vUp.val, sizeof(vUp.x)*3);
									pData->vAssignedUpDir = vUp;
									float fadespeed;
									stream.read(&fadespeed, sizeof(fadespeed) );
									pData->fadespeed = fadespeed;
								}
								else
								{
								
								}
							}

							
							m_vRibbonEmitters.push_back(pData);
						}			
					}
					break;
				//将粒子系统与骨骼分开,参数基本上和绑定到骨骼上的粒子系统相同
				case 'MGPR': //普通粒子系统
				{
					uint gpsnum=0;
					stream.read(&gpsnum,sizeof(gpsnum));
					for( uint i=0; i<gpsnum; i++)
					{
						int boneid;
						stream.read(&boneid, sizeof(boneid));
						bool visible;
						stream.read(&visible,sizeof(visible));
						float speed;
						stream.read(&speed,sizeof(speed));
						float variation;
						stream.read(&variation,sizeof(variation));
						float coneAngle;
						stream.read(&coneAngle,sizeof(coneAngle));
						float gravity;
						stream.read(&gravity,sizeof(gravity));
						float life;
						stream.read(&life,sizeof(life));
						float emissionRate;
						stream.read(&emissionRate,sizeof(emissionRate));
						float width;
						stream.read(&width,sizeof(width));
						float length;
						stream.read(&length,sizeof(length));
						short blendMode;
						stream.read(&blendMode,sizeof(blendMode));
						short rows;
						stream.read(&rows,sizeof(rows));
						short cols;
						stream.read(&cols,sizeof(cols));
						float tailLength;
						stream.read(&tailLength,sizeof(tailLength));
						float timeMiddle;
						stream.read(&timeMiddle,sizeof(timeMiddle));
						Color3 colorStart;
						stream.read(&colorStart,sizeof(colorStart));
						Color3 colorMiddle;
						stream.read(&colorMiddle,sizeof(colorMiddle));
						Color3 colorEnd;
						stream.read(&colorEnd,sizeof(colorEnd));
						Vector3 alpha;
						stream.read(&alpha,sizeof(alpha));
						Vector3 scale;
						stream.read(&scale,sizeof(scale));
						typedef short short3[3];
						short3 headLifeSpan;
						stream.read(&headLifeSpan,sizeof(headLifeSpan));
						short3 headDecay;
						stream.read(&headDecay,sizeof(headDecay));
						short3 tailLifeSpan;
						stream.read(&tailLifeSpan,sizeof(tailLifeSpan));
						short3 tailDecay;
						stream.read(&tailDecay,sizeof(tailDecay));
						bool head,tail,unshaded,unfogged;
						stream.read(&head,sizeof(head));
						stream.read(&tail,sizeof(tail));
						stream.read(&unshaded,sizeof(unshaded));
						stream.read(&unfogged,sizeof(unfogged));
						uchar textureFilenameLen;
						stream.read(&textureFilenameLen,sizeof(textureFilenameLen));
						char filename[256];
						stream.read(filename,textureFilenameLen);
						filename[textureFilenameLen] = 0;

						ParticleEmitter2Data *pData = new ParticleEmitter2Data;
						if (m_ver < 12)
						{
							// 粒子绑定到骨骼时，需要将骨骼的初始矩阵一起运算，版本12以前的粒子系统均没有考虑，这是一个bug。
							// 为了避免以前制作好的特效重做，用这个变量标识是否需要考虑初始矩阵。[5/18/2011 zgz]
							pData->bNodeInitMatrix = false;
						}
						pData->filterMode = (BlendMode)blendMode;
						pData->speed.setStaticData(speed);
						pData->variation.setStaticData(variation);
						pData->latitude.setStaticData(coneAngle);
						pData->gravity.setStaticData(gravity);
						pData->visibility.setStaticData(visible);
						pData->emissionRate.setStaticData(emissionRate);
						pData->width.setStaticData(width);
						pData->length.setStaticData(length);

						CPath path (m_strName);
						CPath pathP (path.getParentDir());
						if(!pathP.empty())
						{
							pathP.addTailSlash();
							pathP += filename;
						}
						pData->textureFilename = pathP.c_str();

						pData->segmentColor1 = colorStart;
						pData->segmentColor2 = colorMiddle;
						pData->segmentColor3 = colorEnd;

						pData->alpha = alpha;
						pData->particleScaling = scale;
						pData->headLifeSpan = Vector3(headLifeSpan[0],headLifeSpan[1],headLifeSpan[2]);
						pData->headDecay = Vector3(headDecay[0],headDecay[1],headDecay[2]);
						pData->tailLifeSpan = Vector3(tailLifeSpan[0],tailLifeSpan[1],tailLifeSpan[2]);
						pData->tailDecay = Vector3(tailDecay[0],tailDecay[1],tailDecay[2]);

						pData->rows = rows;
						pData->columns = cols;

						pData->time = timeMiddle;
						pData->lifeSpan = life;
						pData->tailLength = tailLength;

						pData->head = head;
						pData->tail = tail;
						pData->unshaded = unshaded;
						pData->unfogged = unfogged;

						readKeyFrames<float,bool>(&stream,&pData->visibility);
						readKeyFrames<float,float>(&stream,&pData->speed);
						readKeyFrames<float,float>(&stream,&pData->variation);
						readKeyFrames<float,float>(&stream,&pData->latitude);
						readKeyFrames<float,float>(&stream,&pData->gravity);
						readKeyFrames<float,float>(&stream,&pData->emissionRate);
						readKeyFrames<float,float>(&stream,&pData->width);
						readKeyFrames<float,float>(&stream,&pData->length);

						pData->visibility.setInterpolationType(INTERPOLATION_NONE);
						pData->speed.setInterpolationType(INTERPOLATION_LINEAR);
						pData->variation.setInterpolationType(INTERPOLATION_LINEAR);
						pData->latitude.setInterpolationType(INTERPOLATION_LINEAR);
						pData->gravity.setInterpolationType(INTERPOLATION_LINEAR);
						pData->emissionRate.setInterpolationType(INTERPOLATION_LINEAR);
						pData->width.setInterpolationType(INTERPOLATION_LINEAR);
						pData->length.setInterpolationType(INTERPOLATION_LINEAR);
						ASSERT( boneid >= 0);
						pData->boneObjectId = boneid;

						//对普通粒子的扩展，只有version 5 以上才有
						if( m_ver >= 5)
						{
							uint uiExtSize;
							stream.read(&uiExtSize, sizeof(uiExtSize));
							if( uiExtSize == sizeof(bool) + sizeof(bool) + sizeof(float) *3  )
							{
								//version 5 第一次扩展
								bool _bEnhanceValue;
								stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
								pData->bEnhanceValue = _bEnhanceValue;
								bool _bEnableSynScale;
								stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
								pData->bEnableSynScale = _bEnableSynScale;
								Vector3 vInitSynScale;
								stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
								pData->vInitSynScale = vInitSynScale;	
							}
							else if( uiExtSize == sizeof(bool) + sizeof(bool) + sizeof(float) *3 + sizeof(char)*16)
							{
								//对version 5的第二次扩展
								bool _bEnhanceValue;
								stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
								pData->bEnhanceValue = _bEnhanceValue;
								bool _bEnableSynScale;
								stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
								pData->bEnableSynScale = _bEnableSynScale;
								Vector3 vInitSynScale;
								stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
								pData->vInitSynScale = vInitSynScale;

								char tmpName[16];
								stream.read(tmpName, sizeof(char)*16);
								memcpy(pData->name,tmpName,16);
							}
							else if ( uiExtSize == sizeof(bool) + sizeof(bool) + sizeof(float) *3 + sizeof(char)*16 + sizeof(bool) )
							{
								//对version 5的第三次扩展
								bool _bEnhanceValue;
								stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
								pData->bEnhanceValue = _bEnhanceValue;
								bool _bEnableSynScale;
								stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
								pData->bEnableSynScale = _bEnableSynScale;
								Vector3 vInitSynScale;
								stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
								pData->vInitSynScale = vInitSynScale;

								char tmpName[16];
								stream.read(tmpName, sizeof(char)*16);
								memcpy(pData->name,tmpName,16);
								stream.read(&pData->bEmmitTime2Space, sizeof(pData->bEmmitTime2Space));						
							}
							else if( uiExtSize == sizeof(bool) + sizeof(bool) + sizeof(float) *3 + sizeof(char)*16 + sizeof(bool)
								+ sizeof(int) + sizeof(float) )
							{
								//对version 5的第四次扩展
								bool _bEnhanceValue;
								stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
								pData->bEnhanceValue = _bEnhanceValue;
								bool _bEnableSynScale;
								stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
								pData->bEnableSynScale = _bEnableSynScale;
								Vector3 vInitSynScale;
								stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
								pData->vInitSynScale = vInitSynScale;

								char tmpName[16];
								stream.read(tmpName, sizeof(char)*16);
								memcpy(pData->name,tmpName,16);
								stream.read(&pData->bEmmitTime2Space, sizeof(pData->bEmmitTime2Space));		
								stream.read(&pData->iSpaceInterType, sizeof( pData->iSpaceInterType));
								stream.read(&pData->fSpaceUint, sizeof( pData->fSpaceUint));
							}
							else if ( uiExtSize == sizeof(bool) + sizeof(bool) + sizeof(float) *3 + sizeof(char)*16 + sizeof(bool)
								+ sizeof(int) + sizeof(float) + sizeof(bool)  )
							{
								//version 7 加入粒子绑定到骨骼
								bool _bEnhanceValue;
								stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
								pData->bEnhanceValue = _bEnhanceValue;
								bool _bEnableSynScale;
								stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
								pData->bEnableSynScale = _bEnableSynScale;
								Vector3 vInitSynScale;
								stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
								pData->vInitSynScale = vInitSynScale;

								char tmpName[16];
								stream.read(tmpName, sizeof(char)*16);
								memcpy(pData->name,tmpName,16);
								stream.read(&pData->bEmmitTime2Space, sizeof(pData->bEmmitTime2Space));		
								stream.read(&pData->iSpaceInterType, sizeof( pData->iSpaceInterType));
								stream.read(&pData->fSpaceUint, sizeof( pData->fSpaceUint));
								bool _bAttach2Bone;
								stream.read(&_bAttach2Bone, sizeof(bool));
								pData->attach2Bone = _bAttach2Bone;							
							}
							else if ( uiExtSize == sizeof(bool) + sizeof(bool) + sizeof(float) *3 + sizeof(char)*16 + sizeof(bool)
								+ sizeof(int) + sizeof(float) + sizeof(bool) + sizeof(int)*2 )
							{
								//added by zgz for version 8
								//支持粒子绕z轴旋转
								bool _bEnhanceValue;
								stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
								pData->bEnhanceValue = _bEnhanceValue;
								bool _bEnableSynScale;
								stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
								pData->bEnableSynScale = _bEnableSynScale;
								Vector3 vInitSynScale;
								stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
								pData->vInitSynScale = vInitSynScale;

								char tmpName[16];
								stream.read(tmpName, sizeof(char)*16);
								memcpy(pData->name,tmpName,16);
								stream.read(&pData->bEmmitTime2Space, sizeof(pData->bEmmitTime2Space));		
								stream.read(&pData->iSpaceInterType, sizeof( pData->iSpaceInterType));
								stream.read(&pData->fSpaceUint, sizeof( pData->fSpaceUint));
								bool _bAttach2Bone;
								stream.read(&_bAttach2Bone, sizeof(bool));
								pData->attach2Bone = _bAttach2Bone;				

								stream.read(&pData->nIntervalTime, sizeof(pData->nIntervalTime));
								stream.read(&pData->nAngle, sizeof(pData->nAngle));
							}
							else 
							{
							}
						}

						m_vParticleEmitters.push_back(pData);
					}
					
				}
				break;
				//谢学辉添加，解码MM粒子系统，添加开始
				case 'MPAR':
				{
					uint numMmParticleSystems;
					stream.read(&numMmParticleSystems,sizeof(numMmParticleSystems));
					for(uint i = 0;i < numMmParticleSystems;i++)
					{
						MmParticleSystemData * pdata = new MmParticleSystemData;
						if (m_ver < 12)
						{
							// 粒子绑定到骨骼时，需要将骨骼的初始矩阵一起运算，版本12以前的粒子系统均没有考虑，这是一个bug。
							// 为了避免以前制作好的特效重做，用这个变量标识是否需要考虑初始矩阵。[5/18/2011 zgz]
							pdata->bNodeInitMatrix = false;
						}

						float speed, variation, spread, lat, gravity, lifespan, rate, areal, areaw, grav2;
						stream.read(&speed, sizeof( speed ));
						pdata->speed.setStaticData(speed);
						readMmKeyFrames(&stream,(*pdata).speed);
						stream.read(&variation, sizeof(variation));
						pdata->variation.setStaticData(variation);
						readMmKeyFrames(&stream,(*pdata).variation);
						stream.read(&spread, sizeof(spread));
						pdata->spread.setStaticData(spread);
						readMmKeyFrames(&stream,(*pdata).spread);
						stream.read(&lat, sizeof(lat));
						pdata->lat.setStaticData(lat);
						readMmKeyFrames(&stream,(*pdata).lat);
						stream.read(&gravity, sizeof(gravity));
						pdata->gravity.setStaticData(gravity);
						readMmKeyFrames(&stream,(*pdata).gravity);
						stream.read(&lifespan, sizeof(lifespan));
						pdata->lifespan.setStaticData(lifespan);
						readMmKeyFrames(&stream,(*pdata).lifespan);
						stream.read(&rate, sizeof(rate));
						pdata->rate.setStaticData(rate);
						readMmKeyFrames(&stream,(*pdata).rate);
						stream.read(&areal, sizeof(areal));
						pdata->areal.setStaticData(areal);
						readMmKeyFrames(&stream,(*pdata).areal);
						stream.read(&areaw, sizeof(areaw));
						pdata->areaw.setStaticData(areaw);
						readMmKeyFrames(&stream,(*pdata).areaw);
						stream.read(&grav2, sizeof(grav2));
						pdata->grav2.setStaticData(grav2);
						readMmKeyFrames(&stream,(*pdata).grav2);
						stream.read(&(*pdata).p.mid,sizeof((*pdata).p.mid));
						uint colors[3];
						stream.read(&colors[0],sizeof(colors));
						(*pdata).p.colors[0].setAsARGB(colors[0]);
						(*pdata).p.colors[1].setAsARGB(colors[1]);
						(*pdata).p.colors[2].setAsARGB(colors[2]);
						stream.read(&(*pdata).p.sizes[0],sizeof((*pdata).p.sizes));
						stream.read(&(*pdata).p.d[0],sizeof((*pdata).p.d));
						stream.read(&(*pdata).p.unk[0],sizeof((*pdata).p.unk));
						stream.read(&(*pdata).p.scales[0],sizeof((*pdata).p.scales));
						stream.read(&(*pdata).p.slowdown,sizeof((*pdata).p.slowdown));
						stream.read(&(*pdata).p.rotation,sizeof((*pdata).p.rotation));
						stream.read(&(*pdata).p.f2[0],sizeof((*pdata).p.f2));
						stream.read(&(*pdata).pos,sizeof((*pdata).pos));
						uchar len;
						stream.read(&len,sizeof(len));
						char texture[256];
						stream.read(texture,len);
						texture[len] = 0;

						//
						char *fileName = strrchr(texture,'\\');
						xs::CPath fn;
                        fn = texture;
						if(fileName)
						{
							fileName++;
							fn = fileName;
						}
						CPath path ( m_strName );
						CPath pathP ( path.getParentDir() );
						if(!pathP.empty())
						{
							pathP.addTailSlash();
							pathP += fn;
						}
						else
						{
							pathP = fn;
						}
						(*pdata).texture = pathP;
						//
						stream.read( &pdata->blend, sizeof( pdata->blend));
						stream.read(&pdata->rows, sizeof(pdata->rows));
						stream.read(&pdata->cols, sizeof(pdata->cols));
						stream.read(&pdata->emitterType, sizeof(pdata->emitterType));
						stream.read(&pdata->type, sizeof(pdata->type));
						pdata->order = ( pdata->type > 0? -1:0);
						stream.read(&pdata->s2, sizeof(pdata->s2));
						stream.read(&pdata->bone, sizeof( pdata->bone));
						int flags;
						stream.read(&flags,sizeof(flags));
						(*pdata).billboardType = ((!(flags & 4096)) ? BILLBOARD_LOCK_NONE : BILLBOARD_NOT_USED);


						for(short j = 0;j < (*pdata).rows * (*pdata).cols;j++)
						{
							MmTexCoordSet tc;
							initTile(tc.tc,j,(*pdata).cols,(*pdata).rows,(*pdata).order);
							(*pdata).vTexcoords.push_back(tc);
						}

						//对MM粒子的扩展，只有version 5 以上才有
						if( m_ver >= 5)
						{
							uint uiExtSize;
							stream.read(&uiExtSize, sizeof(uiExtSize));
							if( uiExtSize == sizeof(bool) + sizeof(bool) + sizeof(float) *3  )
							{
								//version 5 第一次扩展
								bool _bEnhanceValue;
								stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
								pdata->bEnhanceValue = _bEnhanceValue;
								bool _bEnableSynScale;
								stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
								pdata->bEnableSynScale = _bEnableSynScale;
								Vector3 vInitSynScale;
								stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
								pdata->vInitSynScale = vInitSynScale;	
							}
							else if( uiExtSize == sizeof(bool) + sizeof(bool) +sizeof(float)*3 + sizeof(char)*16)
							{

								//version 5 第二次扩展
								bool _bEnhanceValue;
								stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
								pdata->bEnhanceValue = _bEnhanceValue;
								bool _bEnableSynScale;
								stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
								pdata->bEnableSynScale = _bEnableSynScale;
								Vector3 vInitSynScale;
								stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
								pdata->vInitSynScale = vInitSynScale;

								char tmpName[16];
								stream.read(tmpName, sizeof(char)*16);
								memcpy(pdata->name,tmpName,16);
							}
							else if( uiExtSize == sizeof(bool) + sizeof(bool) +sizeof(float)*3 + sizeof(char)*16 + sizeof(bool) )
							{
								//version 5 第三次扩展
								bool _bEnhanceValue;
								stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
								pdata->bEnhanceValue = _bEnhanceValue;
								bool _bEnableSynScale;
								stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
								pdata->bEnableSynScale = _bEnableSynScale;
								Vector3 vInitSynScale;
								stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
								pdata->vInitSynScale = vInitSynScale;

								char tmpName[16];
								stream.read(tmpName, sizeof(char)*16);
								memcpy(pdata->name,tmpName,16);
								stream.read(&pdata->bEmmitTime2Space, sizeof(pdata->bEmmitTime2Space));						
							}
							else if (uiExtSize == sizeof(bool) + sizeof(bool) +sizeof(float)*3 + sizeof(char)*16 + sizeof(bool) + 
								sizeof(int) + sizeof(float) ) 
							{
								//version 5 第四次扩展
								bool _bEnhanceValue;
								stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
								pdata->bEnhanceValue = _bEnhanceValue;
								bool _bEnableSynScale;
								stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
								pdata->bEnableSynScale = _bEnableSynScale;
								Vector3 vInitSynScale;
								stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
								pdata->vInitSynScale = vInitSynScale;

								char tmpName[16];
								stream.read(tmpName, sizeof(char)*16);
								memcpy(pdata->name,tmpName,16);
								stream.read(&pdata->bEmmitTime2Space, sizeof(pdata->bEmmitTime2Space));	
								stream.read(&pdata->iSpaceInterType, sizeof(pdata->iSpaceInterType));
								stream.read(&pdata->fSpaceUint, sizeof(pdata->fSpaceUint));							
							}
							else if ( uiExtSize == sizeof(bool) + sizeof(bool) +sizeof(float)*3 + sizeof(char)*16 + sizeof(bool) + 
								sizeof(int) + sizeof(float) + sizeof(short) )
							{
								//version 5 第四次扩展
								bool _bEnhanceValue;
								stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
								pdata->bEnhanceValue = _bEnhanceValue;
								bool _bEnableSynScale;
								stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
								pdata->bEnableSynScale = _bEnableSynScale;
								Vector3 vInitSynScale;
								stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
								pdata->vInitSynScale = vInitSynScale;

								char tmpName[16];
								stream.read(tmpName, sizeof(char)*16);
								memcpy(pdata->name,tmpName,16);
								stream.read(&pdata->bEmmitTime2Space, sizeof(pdata->bEmmitTime2Space));	
								stream.read(&pdata->iSpaceInterType, sizeof(pdata->iSpaceInterType));
								stream.read(&pdata->fSpaceUint, sizeof(pdata->fSpaceUint));	
								stream.read(&pdata->attach2Bone, sizeof( pdata->attach2Bone) );						
							}
							else if ( uiExtSize == sizeof(bool) + sizeof(bool) +sizeof(float)*3 + sizeof(char)*16 + sizeof(bool) + 
								sizeof(int) + sizeof(float) + sizeof(short) + sizeof(int)*2)
							{
								//added by zgz for version 8
								//支持粒子绕z轴旋转
								bool _bEnhanceValue;
								stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
								pdata->bEnhanceValue = _bEnhanceValue;
								bool _bEnableSynScale;
								stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
								pdata->bEnableSynScale = _bEnableSynScale;
								Vector3 vInitSynScale;
								stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
								pdata->vInitSynScale = vInitSynScale;

								char tmpName[16];
								stream.read(tmpName, sizeof(char)*16);
								memcpy(pdata->name,tmpName,16);
								stream.read(&pdata->bEmmitTime2Space, sizeof(pdata->bEmmitTime2Space));	
								stream.read(&pdata->iSpaceInterType, sizeof(pdata->iSpaceInterType));
								stream.read(&pdata->fSpaceUint, sizeof(pdata->fSpaceUint));	
								stream.read(&pdata->attach2Bone, sizeof( pdata->attach2Bone) );	
								
								stream.read(&pdata->nIntervalTime, sizeof( pdata->nIntervalTime) );	
								stream.read(&pdata->nAngle, sizeof( pdata->nAngle) );	
							}
							else if ( uiExtSize == sizeof(bool) + sizeof(bool) +sizeof(float)*3 + sizeof(char)*16 + sizeof(bool) + 
								sizeof(int) + sizeof(float) + sizeof(short) + sizeof(int)*2 + sizeof(int)*2)
							{
								//added by zgz for version 9
								//保存贴图遍历范围
								bool _bEnhanceValue;
								stream.read(&_bEnhanceValue, sizeof(_bEnhanceValue));
								pdata->bEnhanceValue = _bEnhanceValue;
								bool _bEnableSynScale;
								stream.read(&_bEnableSynScale, sizeof(_bEnableSynScale));
								pdata->bEnableSynScale = _bEnableSynScale;
								Vector3 vInitSynScale;
								stream.read(vInitSynScale.val, sizeof(vInitSynScale.x) *3 );
								pdata->vInitSynScale = vInitSynScale;

								char tmpName[16];
								stream.read(tmpName, sizeof(char)*16);
								memcpy(pdata->name,tmpName,16);
								stream.read(&pdata->bEmmitTime2Space, sizeof(pdata->bEmmitTime2Space));	
								stream.read(&pdata->iSpaceInterType, sizeof(pdata->iSpaceInterType));
								stream.read(&pdata->fSpaceUint, sizeof(pdata->fSpaceUint));	
								stream.read(&pdata->attach2Bone, sizeof( pdata->attach2Bone) );	

								stream.read(&pdata->nIntervalTime, sizeof( pdata->nIntervalTime) );	
								stream.read(&pdata->nAngle, sizeof( pdata->nAngle) );	

								stream.read(&pdata->iteratorArea, sizeof( pdata->iteratorArea) );	
							}
							else
							{
							
							}
						}

						m_vMmParticleSystemData.push_back(pdata);
					}
				
				}
				break;
				//添加结束
			}
			pChunk = chunk.nextChunk(pStream);
		}

		//读取非编
		//...

		//初始化硬件顶点的混合
		PP_BY_NAME_START("ModelCodecTx::decode::initForGPUVertexBlend");
		initForGPUVertexBlend(pFaceBuffer);
		PP_BY_NAME_STOP();
		safeDeleteArray( pFaceBuffer);

		PP_BY_NAME_START("ModelCodecTx::decode::createCoreSkeleton");
		createCoreSkeleton();
		PP_BY_NAME_STOP();

		m_memorySize += sizeof(*this);
		m_memorySize += m_skeleton.getMemeorySize();
		m_memorySize += m_vRenderPasses.size() * sizeof(SubModel);
		m_memorySize += m_vAnimations.size() * sizeof(Animation);
		m_memorySize += m_vParticleEmitters.size() * sizeof(ParticleEmitter2Data);
		m_memorySize += m_vRibbonEmitters.size() * sizeof(RibbonEmitterData);
		m_memorySize += m_vMmParticleSystemData.size() * sizeof(MmParticleSystemData);//谢学辉添加，处理MM粒子系统
		size_t size = m_vBones.size();
		for(size_t i = 0;i < size;i++)
		{
			m_memorySize += m_vBones[i]->getMemorySize();
		}
		size = m_vMaterials.size();
		for(size_t i = 0;i < size;i++)
		{
			m_memorySize += m_vMaterials[i].getMemorySize();
		}
		m_memorySize += m_ui32VerticeNum * sizeof(_VertexData);
		return true;
	}

	void ModelCodecTx::initTile(Vector2 *tc,int num,int cols,int rows,int order)//谢学辉添加
	{
		Vector2 otc[4];
		Vector2 a,b;
		int x = num % cols;
		int y = num / cols;
		a.x = x * (1.0f / cols);
		b.x = (x + 1) * (1.0f / cols);
		a.y = y * (1.0f / rows);
		b.y = (y + 1) * (1.0f / rows);

		otc[0] = a;
		otc[2] = b;
		otc[1].x = b.x;
		otc[1].y = a.y;
		otc[3].x = a.x;
		otc[3].y = b.y;

		for(int i = 0;i < 4;i++)
		{
			tc[(i + 4 - order) & 3] = otc[i];
		}
	}

	ModelCodecTx::~ModelCodecTx()
	{
		for(int i = 0;i < TX_PRECOMPUTED_VBO_COUNT_MAX /*8192*/;i++)
		{
			safeRelease(m_pPrecomputedVBs[i]);
		}
		STLDeleteSequence(m_vParticleEmitters);
		STLDeleteSequence(m_vMmParticleSystemData);
		STLDeleteSequence(m_vRibbonEmitters);
		STLDeleteSequence(m_vBones);
		STLDeleteSequence(m_vAnimations);
		safeRelease(m_pVBTexcoord);
		safeRelease(m_pVBPosNormal);
		safeRelease(m_pIB);
		safeRelease(m_pBlendWeightBuffer);
		safeRelease(m_pBlendProgram);
		STLReleaseSequence( m_vBlendIndices);
		safeDeleteArray(m_pVertexData);

		if( m_pCustomizeRenderOrderQueue) 
		{
			delete [] m_pCustomizeRenderOrderQueue;
			m_pCustomizeRenderOrderQueue = 0;
		}


#if defined(TX_TEX_VBO_STATIC_WRITE_ONLY)
		uint uiTexPreComp = TX_PRECOMPUTED_TEX_PASS_COUNT * TX_PRECOMPUTED_TEX_VBO_COUNT;
		for( uint i=0; i < uiTexPreComp; i++ )
		{
			safeRelease( m_pPrecomputedTexVBs[i]);
		}
#else
		std::map< int , IVertexBuffer *>::iterator it = m_mapPrecomputedTexBuffers.begin();
		for( ; it != m_mapPrecomputedTexBuffers.end(); it++)
		{
			safeRelease( ( (*it).second) ); 
		}
#endif
	}

	ModelCodecTx::ModelCodecTx(const std::string& name,IRenderSystem* pRenderSystem)
		: ManagedItem(name.c_str()),
		m_pRenderSystem(pRenderSystem),
		m_pVertexData(0),
		m_pVBTexcoord(0),
		m_pIB(0),
		m_ui32VerticeNum(0),
		m_bAnimatedGeometry(false),
		m_memorySize(0),
		m_pVBPosNormal(0),
		m_bAnimatedTexCoord(false),
		m_iMaxPassNum(0),
		m_bCalculatedAABB(false),
		m_bShouldBoneInterKeyFrame(true),
		m_boundingBox(-1.0f,-1.0f,1.0f,1.0f),
		m_bIsEnableCustomizeRenderOrder(false),
		m_pCustomizeRenderOrderQueue(0),
		m_pBlendWeightBuffer(0),
		m_pBlendProgram(0),
		m_bMergeSubModel(false),
		m_bLoadSkinInfoFromFile(false)
	{
		//memset(m_pPrecomputedVBs,0,8192 * sizeof(IVertexBuffer*));
		memset(m_pPrecomputedVBs,0,TX_PRECOMPUTED_VBO_COUNT_MAX * sizeof(IVertexBuffer*));

#if defined(TX_TEX_VBO_STATIC_WRITE_ONLY)
		memset(m_pPrecomputedTexVBs,0,TX_PRECOMPUTED_TEX_PASS_COUNT*TX_PRECOMPUTED_TEX_VBO_COUNT*sizeof(IVertexBuffer*));
		memset(m_pPrecomputedTexVBsPass,0,TX_PRECOMPUTED_TEX_PASS_COUNT*sizeof(IVertexBuffer *) );
#else
		m_mapPrecomputedTexBuffers.clear();
		m_uiPreAniTime = -1;
#endif
		m_vUVKFs.clear();
	}

	void  ModelCodecTx::calcAABB(_VertexData *pVtx,uint num,AABB& aabb,Sphere& sphere)
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

	size_t ModelCodecTx::getMomorySize()
	{
		return m_memorySize;
	}

	void ModelCodecTx::release()
	{
		delete this;
	}

	void * ModelCodecTx::getParticleSystemParam(unsigned int index, string paramname, string & type, int & count )
	 {
		//通用检测
		if( index >= m_vParticleEmitters.size() ) return 0;

		ParticleEmitter2Data & psdata = *m_vParticleEmitters[index];
		
		//标量参数
		if(paramname == "textureFilename") return getPSParamVector( &psdata.textureFilename, "string",type,1,count);
		if( paramname == "filterMode")
		{
			type = "short";
			count = 1;
			short * preturn = new short[1];
			*preturn = static_cast<short>(psdata.filterMode);
			return preturn;
		}
		if( paramname == "rows") return getPSParamVector(&psdata.rows, "int",type,1,count);
		if( paramname == "columns") return getPSParamVector(&psdata.columns, "int",type,1,count);
		if( paramname == "boneObjectId") return getPSParamVector(&psdata.boneObjectId, "int",type,1,count);
		if( paramname == "time") return getPSParamVector(&psdata.time, "float",type,1,count);
		if( paramname == "lifeSpan") return getPSParamVector(&psdata.lifeSpan, "float",type,1,count);
		if( paramname == "tailLength") return getPSParamVector(&psdata.tailLength, "float",type,1,count);

		//开关参数
		if( paramname == "head") return getPSParamVector(&psdata.head, "bool",type,1,count);
		if( paramname == "tail") return getPSParamVector(&psdata.tail, "bool",type,1,count);
		if( paramname == "unshaded") return getPSParamVector(&psdata.unshaded, "bool",type,1,count);
		if( paramname == "unfogged") return getPSParamVector(&psdata.unfogged, "bool",type,1,count);

		//帧参数
		if( paramname == "speed") return getPSKeyFramesParam(&psdata.speed, "float", type, count);
		if( paramname == "variation") return getPSKeyFramesParam(&psdata.variation, "float", type, count);
		if( paramname == "latitude") return getPSKeyFramesParam(&psdata.latitude, "float", type, count);
		if( paramname == "gravity") return getPSKeyFramesParam(&psdata.gravity, "float", type, count);
		if( paramname == "visibility") return getPSKeyFramesParam(&psdata.visibility, "float", type, count);
		if( paramname == "emissionRate") return getPSKeyFramesParam(&psdata.emissionRate, "float", type, count);
		if( paramname == "width") return getPSKeyFramesParam(&psdata.width, "float", type, count);
		if( paramname == "length") return getPSKeyFramesParam(&psdata.length, "float", type, count);

		//向量参数
		if( paramname == "segmentColor1") return getPSParamVector(psdata.segmentColor1.val, "float",type,3,count);
		if( paramname == "segmentColor2") return getPSParamVector(psdata.segmentColor2.val, "float",type,3,count);
		if( paramname == "segmentColor3") return getPSParamVector(psdata.segmentColor3.val, "float",type,3,count);
		if( paramname == "alpha") return getPSParamVector(psdata.alpha.val, "float",type,3,count);
		if( paramname == "particleScaling") return getPSParamVector(psdata.particleScaling.val, "float",type,3,count);
		if( paramname == "headLifeSpan") return getPSParamVector(psdata.headLifeSpan.val, "float",type,3,count);
		if( paramname == "headDecay") return getPSParamVector(psdata.headDecay.val, "float",type,3,count);
		if( paramname == "tailLifeSpan") return getPSParamVector(psdata.tailLifeSpan.val, "float",type,3,count);
		if( paramname == "tailDecay") return getPSParamVector(psdata.tailDecay.val, "float",type,3,count);

		//未使用参数
		if( paramname == "textureId") return getPSParamVector(&psdata.textureId, "int",type,1,count);
		if( paramname == "priorityPlane") return getPSParamVector(&psdata.priorityPlane, "int",type,1,count);
		if( paramname == "replaceableId") return getPSParamVector(&psdata.replaceableId, "int",type,1,count);
		if( paramname == "sortPrimitivesFarZ") return getPSParamVector(&psdata.sortPrimitivesFarZ, "bool",type,1,count);
		if( paramname == "lineEmitter") return getPSParamVector(&psdata.lineEmitter, "bool",type,1,count);
		if( paramname == "modelSpace") return getPSParamVector(&psdata.modelSpace, "bool",type,1,count);
		if( paramname == "alphaKey") return getPSParamVector(&psdata.alphaKey, "bool",type,1,count);
		if( paramname == "xyQuad") return getPSParamVector(&psdata.xyQuad, "bool",type,1,count);
		if( paramname == "squirt") return getPSParamVector(&psdata.squirt, "bool",type,1,count);

		//对版本5的扩展
		if( paramname == "enhancevalue") return getPSParamVector(&psdata.bEnhanceValue, "bool",type,1,count);
		if( paramname == "enablesynscale") return getPSParamVector(&psdata.bEnableSynScale, "bool",type,1,count);
		if( paramname == "initsynscale") return getPSParamVector(psdata.vInitSynScale.val, "float",type,3,count);

		if( paramname == "name" )
		{
			count = strlen(psdata.name)+1;
			type = "char";
			char * preturn = new char[count];
			memcpy(preturn, psdata.name,count);
			preturn[count-1] ='\0';
			return preturn;
		}

		if( paramname == "emmittime2space") return getPSParamVector(&psdata.bEmmitTime2Space,"bool",type,1,count);
		if( paramname == "spaceintertype" ) return getPSParamVector(&psdata.iSpaceInterType, "int",type,1,count);
		if( paramname == "spaceunit" ) return getPSParamVector(&psdata.fSpaceUint,"float",type,1,count);
		if( paramname == "attach2bone" ) return getPSParamVector(&psdata.attach2Bone,"bool",type,1,count);

		//对版本8的扩展
		if( paramname == "rotateZAxis")
		{
			int* preturn = new int[2];
			preturn[0] = psdata.nIntervalTime;
			preturn[1] = psdata.nAngle;
			type = "int";
			count = 2;
			return preturn;
		}

		//默认处理
		return 0;
	 }

	void * ModelCodecTx::getPSParamVector( void * pParam, string paramtype, string & type, int paramcount, int & count)
	{	
		if( paramcount <= 0 ) return 0;
		if( pParam == 0) return 0;

		type = paramtype;
		count = paramcount;

		if( paramtype == "bool")
		{
			bool * pbool = (bool *)pParam;
			bool * preturn = new bool[paramcount];
			for( int i = 0; i< paramcount; i++)
			{
				preturn[i] = pbool[i];
			}
			return preturn;
		}
		if( paramtype == "short")
		{
			short * pshort = (short *) pParam;
			short * preturn = new short[paramcount];
			for(int i =0; i < paramcount; i++)
			{
				preturn[i] = pshort[i];
			}
			return preturn;
		}
		if( paramtype == "int")
		{
			int * pint = (int *) pParam;
			int * preturn = new int[paramcount];
			for( int i = 0; i<paramcount; i++)
			{
				preturn[i] = pint[i];
			}
			return preturn;
		}
		if( paramtype == "float")
		{
			float * pfloat = (float *) pParam;
			float * preturn = new float[paramcount];
			for(int i =0; i< paramcount; i++)
			{
				preturn[i] = pfloat[i];
			}
			return preturn;
		}
		if( paramtype == "string")
		{
			string * pstring = (string *) pParam;
			string * preturn = new string[paramcount];
			for( int i = 0; i< paramcount; i++)
			{
				preturn[i] = pstring[i];
			}
			return preturn;
		}
		return 0;
	}

	void * ModelCodecTx::getPSKeyFramesParam(void * pkf, string kftype, string & type, int & count)
	{
		if( pkf == 0 ) return 0;

		if( kftype == "float")
		{
			KeyFrames<float> * pfloat = static_cast< KeyFrames<float> *> (pkf);
			int keyframesnum = pfloat->numKeyFrames();
			int keyframesrangenum = pfloat->numKeyFrameRanges();
			float * preturn = new float[ 1 + 1 + 1 + 1 +  4 *keyframesnum + 1 + 2*keyframesrangenum];

			preturn[0] = static_cast<float>(  pfloat->getInterpolationType() ) ;
			preturn[1] = static_cast<float>( pfloat->isEnableStaticData() ? 1:0);
			preturn[2] = static_cast<float>(pfloat->getStaticData() );
			preturn[3] = static_cast<float>(keyframesnum );
			for(int i = 0; i < keyframesnum; i ++)
			{
				preturn[4 + i*4 + 0] = static_cast<float>(pfloat->getKeyFrame(i)->time );
				preturn[4 + i*4 + 1] = static_cast<float>(pfloat->getKeyFrame(i)->v );
				preturn[4 + i*4 + 2] = static_cast<float>(pfloat->getKeyFrame(i)->in );
				preturn[4 + i*4 + 3] = static_cast<float>(pfloat->getKeyFrame(i)->out );
			}

			preturn[3 + keyframesnum*4 + 1] = static_cast<float>(keyframesrangenum);

			for(int i =0; i < keyframesrangenum; i++)
			{
				preturn[3+keyframesnum*4 + 1+ 1 +0] = static_cast<float>(pfloat->getKeyFrameRange(i)->first);
				preturn[3+keyframesnum*4 + 1+ 1 +1] = static_cast<float>(pfloat->getKeyFrameRange(i)->second);
			}

			type = kftype;
			count = 1 + 1 + 1 + 1 +  4 *keyframesnum + 1 + 2*keyframesrangenum;
			return preturn;
		}

		return 0;
	}

	bool ModelCodecTx::setParticleSystemParam(unsigned int index, string desname, void * psrc, int srcnum, string srctype)
	{
		//通用检测
		if( index >= m_vParticleEmitters.size() ) return false;
		if( psrc == 0) return false;
		if( srcnum <= 0 ) return false;

		ParticleEmitter2Data & psdata = *m_vParticleEmitters[index];

		//标量参数
		if(desname == "textureFilename") return setPSParamVector(psrc,srcnum,srctype,&psdata.textureFilename, 1,"string");
		//if(desname == "textureFilename") return false;
		if(desname == "filterMode")
		{
			if( srcnum != 1 || srctype != "short" ) return false;
			short * pshort = (short *) psrc;
			if( *pshort < 0 || *pshort > 5) return false;
			psdata.filterMode = static_cast<BlendMode>(pshort[0]);
			return true;
		}
		if(desname == "rows") return setPSParamVector(psrc,srcnum,srctype,&psdata.rows, 1,"int",true,1-0.0000001,1000000+0.0000001);
		if(desname == "columns") return setPSParamVector(psrc,srcnum,srctype,&psdata.columns, 1,"int",true,1-0.0000001,1000000+0.0000001);
		if(desname == "boneObjectId")
		{
			//现在不需要判断是否指定的骨骼已经绑定了粒子系统了
			if( srcnum != 1 || srctype != "int") return false;
			int curboneid = *((int*)psrc);
			if( curboneid < 0 || static_cast<uint>(curboneid) >= m_vBones.size() ) return false;
			psdata.boneObjectId = curboneid;
			return true;	
		}
		if(desname == "time") return setPSParamVector(psrc,srcnum,srctype,&psdata.time, 1,"float",true,0-0.0000001,1+0.0000001);
		if(desname == "lifeSpan") return setPSParamVector(psrc,srcnum,srctype,&psdata.lifeSpan, 1,"float",true,0,10000);
		if(desname == "tailLength") return setPSParamVector(psrc,srcnum,srctype,&psdata.tailLength, 1,"float");
		

		//开关参数
		if(desname == "head") return setPSParamVector(psrc,srcnum,srctype,&psdata.head, 1,"bool");
		if(desname == "tail") return setPSParamVector(psrc,srcnum,srctype,&psdata.tail, 1,"bool");
		if(desname == "unshaded") return setPSParamVector(psrc,srcnum,srctype,&psdata.unshaded, 1,"bool");
		if(desname == "unfogged") return setPSParamVector(psrc,srcnum,srctype,&psdata.unfogged, 1,"bool");


		//帧参数
		if( desname == "speed") return setPSKeyFramesParam( &psdata.speed,"float", static_cast<float *>(psrc), srcnum);
		if( desname == "variation") return setPSKeyFramesParam( &psdata.variation, "float", static_cast<float *>(psrc), srcnum);
		if( desname == "latitude") return setPSKeyFramesParam( &psdata.latitude, "float", static_cast<float *>(psrc), srcnum);
		if( desname == "gravity") return setPSKeyFramesParam( &psdata.gravity, "float", static_cast<float *>(psrc), srcnum);
		if( desname == "visibility") return setPSKeyFramesParam( &psdata.visibility, "float", static_cast<float *>(psrc), srcnum);
		if( desname == "emissionRate") return setPSKeyFramesParam( &psdata.emissionRate, "float", static_cast<float *>(psrc), srcnum);
		if( desname == "width") return setPSKeyFramesParam( &psdata.width, "float", static_cast<float *>(psrc), srcnum);
		if( desname == "length") return setPSKeyFramesParam( &psdata.length, "float", static_cast<float *>(psrc), srcnum);


		//向量参数
		if(desname == "segmentColor1") return setPSParamVector(psrc,srcnum,srctype,psdata.segmentColor1.val, 3,"float",true,0-0.0000001,1+0.0000001);
		if(desname == "segmentColor2") return setPSParamVector(psrc,srcnum,srctype,psdata.segmentColor2.val, 3,"float",true,0-0.0000001,1+0.0000001);
		if(desname == "segmentColor3") return setPSParamVector(psrc,srcnum,srctype,psdata.segmentColor3.val, 3,"float",true,0-0.0000001,1+0.0000001);
		if(desname == "alpha") return setPSParamVector(psrc,srcnum,srctype,psdata.alpha.val, 3,"float",true,0-0.0000001,1+0.0000001);
		if(desname == "particleScaling") return setPSParamVector(psrc,srcnum,srctype,psdata.particleScaling.val, 3,"float");
		if(desname == "headLifeSpan") return setPSParamVector(psrc,srcnum,srctype,psdata.headLifeSpan.val, 3,"float");
		if(desname == "headDecay") return setPSParamVector(psrc,srcnum,srctype,psdata.headDecay.val, 3,"float");
		if(desname == "tailLifeSpan") return setPSParamVector(psrc,srcnum,srctype,psdata.tailLifeSpan.val, 3,"float");
		if(desname == "tailDecay") return setPSParamVector(psrc,srcnum,srctype,psdata.tailDecay.val, 3,"float");

		//未使用参数
		if(desname == "textureId") return false;
		if(desname == "priorityPlane") return false;
		if(desname == "replaceableId") return false;
		if(desname == "sortPrimitivesFarZ") return false;
		if(desname == "lineEmitter") return false;
		if(desname == "modelSpace") return false;
		if(desname == "alphaKey") return false;
		if(desname == "xyQuad") return false;
		if(desname == "squirt") return false;

		//版本5的扩展
		if( desname =="enhancevalue" ) return setPSParamVector(psrc,srcnum,srctype,&psdata.bEnhanceValue, 1,"bool");
		if( desname == "enablesynscale") return  setPSParamVector(psrc,srcnum,srctype,&psdata.bEnableSynScale, 1,"bool");
		if( desname == "initsynscale" ) return setPSParamVector(psrc,srcnum,srctype,psdata.vInitSynScale.val, 3,"float");

		//版本5的扩展
		//普通粒子系统的名字
		if( desname == "name" )
		{
			if( srcnum >=16)
			{
				memcpy(psdata.name,(char *)psrc,15);
				psdata.name[15] = '\0';
			}
			else
			{
				memcpy(psdata.name,(char *)psrc,srcnum);
				memset(&psdata.name[srcnum],0,16-srcnum);
			}
			return true;
		}

		if( desname == "emmittime2space") return setPSParamVector(psrc, srcnum, srctype, &psdata.bEmmitTime2Space,1,"bool");
		if( desname == "spaceintertype") return setMMPSParamVector( psrc, srcnum, srctype, &psdata.iSpaceInterType,1,"int",true,1-0.0001,2+0.0001);
		if( desname == "spaceunit" ) return setMMPSParamVector( psrc, srcnum, srctype, &psdata.fSpaceUint,1,"float",true,0+0.00001, 10000000.0f);
		if( desname == "attach2bone" ) return setMMPSParamVector( psrc, srcnum, srctype, &psdata.attach2Bone,1,"bool");

		//版本8的扩展
		if( desname == "rotateZAxis" )
		{
			if( srcnum != 2 || srctype != "int" ) return false;
			int* pData = (int*)psrc;
			psdata.nIntervalTime = pData[0];
			psdata.nAngle = pData[1];
			return true;
		}
		//默认处理
		return false;
	}

	bool ModelCodecTx::setPSParamVector(void * psrc, int srcnum, string srctype, void* pdes, int desnum, string destype, bool enablerange, float min , float max  )
	{
		//通用检测
		if( psrc == 0 || pdes == 0 ) return false;
		if( srcnum <= 0 || desnum <= 0 ) return false;
		if( srcnum != desnum ) return false;
		if( srctype != destype ) return false;

		if( srctype == "string")
		{
			string * strsrc = (string *)psrc;
			string * strdes = (string *)pdes;
			for( int i = 0; i < srcnum; i++) strdes[i] = strsrc[i];
			return true;
		}

		if(srctype == "short")
		{
			short* ssrc = (short *) psrc;
			short* sdes = (short * )pdes;
			if ( enablerange == true )
			{
				for( int i = 0; i < srcnum; i ++ ) 
					if( (float)ssrc[i] < min || (float )ssrc[i] > max) return false;
			}
			for( int i=0;i < srcnum;i++) sdes[i] = ssrc[i];
			return true;
		}
		if(srctype == "bool")
		{
			bool* ssrc = (bool *) psrc;
			bool* sdes = (bool * )pdes;
			for( int i=0;i < srcnum;i++) sdes[i] = ssrc[i];
			return true;
		}

		if(srctype == "int")
		{
			int* isrc = (int *) psrc;
			int* ides = (int * )pdes;
			if( enablerange == true)
			{
				for( int i = 0; i < srcnum ; i++)
					if( (float)isrc[i] < min || (float)isrc[i] > max ) return false;
			}
			for( int i=0;i < srcnum;i++) ides[i] = isrc[i];
			return true;
		}

		if(srctype == "float")
		{
			float* fsrc = (float *) psrc;
			float* fdes = (float * )pdes;
			if( enablerange == true)
			{
				for(int i = 0; i< srcnum; i++)
					if( fsrc[i] < min || fsrc[i] > max ) return false;
			}
			for( int i=0;i < srcnum;i++) fdes[i] = fsrc[i];
			return true;
		}

		return false;
	}
	/*
	bool ModelCodecTx::setPSKeyFramesParam(KeyFrames<float> * pkf, float * pParamVal, int iParamNum, string szParamType)
	{
		//帧序列格式 interpolationType,enablestaticdata, StaticData, numoftimeframe, frame1,...,frameN, numofrangeframe, frame1,...,frameN
		//通用检测工作
		if( szParamType != "float") return false;
		if( pkf == 0 || pParamVal == 0) return false;
		if ( pParamVal[1] < 0.5 && iParamNum < 8 ) return false;

		int staticdatanum = 0;
		int staticdatapos = -1;
		int paramframenum = 0;
		int paramframepos = -1;
		int rangeframenum = 0;
		int rangeframepos = -1;

		if ( pParamVal[1] > 0.5 )	staticdatanum = 1;
		else	staticdatanum = 0;
		paramframenum = static_cast<int>( pParamVal[1 + 1 + staticdatanum + 1 - 1 ]);
		rangeframenum = static_cast<int>( pParamVal[ 1 + 1+ staticdatanum + 1 + paramframenum *4 + 1 -1]);

		if ( 1 + 1 + staticdatanum + 1 + paramframenum*4 + 1 + rangeframenum* 2 != iParamNum ) return false;

		//数据操作
		pkf->clearKeyFramesAndKeyFrameRanges();

		unsigned int uiItpType = static_cast<unsigned int>( pParamVal[0]);
		uiItpType = uiItpType % 4;
		pkf->setInterpolationType( static_cast<InterpolationType>( uiItpType ) );

		if( pParamVal[1] > 0.5 )
		{
			staticdatapos = 2;
			pkf->setStaticData( pParamVal[staticdatapos] );
		}
		else 
		{
			pkf->enableStaticData(false);
		}

		if( paramframenum > 0 )
		{
			paramframepos = 1 + 1 + staticdatanum + 1 - 1 + 1;
			for( int i = 0; i < paramframenum ; i ++)
			{
				int curpos = paramframepos + 4*i;
				KeyFrame<float> kf( (uint)(pParamVal[curpos + 0]), pParamVal[curpos+1], pParamVal[curpos+2], pParamVal[curpos+3] );
				pkf->addKeyFrame( kf);
			}
		}

		if( rangeframenum > 0)
		{
			rangeframepos = 1 + 1+ staticdatanum + 1 + paramframenum *4 + 1 -1 +1;
			for( int i = 0; i < rangeframenum; i++)
			{
				int curpos = rangeframepos + i*2;
				KeyFrameRange kfr( static_cast<int>(pParamVal[curpos+0]), static_cast<int>(pParamVal[curpos+1]) );
				pkf->addKeyFrameRange( kfr);
			}
		}
		return true;
	}
	*/
	bool ModelCodecTx::addParticleSystem()
	{	
		//现在不需要判断是有所有的骨骼都绑定到粒子系统了
		if( m_vBones.size() == 0 ) return false;
		ParticleEmitter2Data * ps = new ParticleEmitter2Data();
		ps->boneObjectId =  0; //默认绑定到0号骨骼

		//初始化一些数据
		ps->textureFilename = "data\\Creature\\Common\\Effect\\";
		ps->filterMode = static_cast<xs::BlendMode>(4);
		ps->lifeSpan =1;
		ps->head = true;
		ps->tail = true;
		ps->time = 0.5;
		KeyFrame<float> kfspeed(0,0);
		ps->speed.addKeyFrame(kfspeed);
		KeyFrame<float> kfvariation(0,0);
		ps->variation.addKeyFrame(kfvariation);
		KeyFrame<float> kflatitude(0,0);
		ps->latitude.addKeyFrame(kflatitude);
		KeyFrame<float> kfgravity(0,0);
		ps->gravity.addKeyFrame(kfgravity);
		KeyFrame<float> kfvisibility(0,1);
		ps->visibility.addKeyFrame( kfvisibility);
		KeyFrame<float> kfemissionrate(0,20);
		ps->emissionRate.addKeyFrame(kfemissionrate);
		KeyFrame<float> kfwidth(0,5);
		ps->width.addKeyFrame(kfwidth);
		KeyFrame<float> kflength(0,5);
		ps->length.addKeyFrame(kflength);

		m_vParticleEmitters.push_back(ps);

		//渲染顺序
		if(m_bIsEnableCustomizeRenderOrder && m_pCustomizeRenderOrderQueue )
		{	
			m_pCustomizeRenderOrderQueue[ECustomizeRenderOrderObject_ParticleSystem].push_back( getInitRenderOrder() );
		}
		return true;
	}
	bool ModelCodecTx::removeParticleSystem(unsigned int index)
	{
		if( index >= m_vParticleEmitters.size() )  return false;

		if( m_bIsEnableCustomizeRenderOrder && m_pCustomizeRenderOrderQueue  )
		{
			std::vector<ParticleEmitter2Data *>::iterator it = m_vParticleEmitters.begin();
			std::vector<int>::iterator itOrder = m_pCustomizeRenderOrderQueue[ECustomizeRenderOrderObject_ParticleSystem].begin();
			unsigned int count = 0;
			for( ; it != m_vParticleEmitters.end(); ++it, ++count,++itOrder)
			{
				if( count == index ) 
				{
					delete (*it);
					m_vParticleEmitters.erase(it);
					m_pCustomizeRenderOrderQueue[ECustomizeRenderOrderObject_ParticleSystem].erase(itOrder);
					return true;
				}
			}
			return false;
		
		}
		else
		{
			std::vector<ParticleEmitter2Data *>::iterator it = m_vParticleEmitters.begin();
			unsigned int count = 0;
			for( ; it != m_vParticleEmitters.end(); ++it, ++count)
			{
				if( count == index ) 
				{
					delete (*it);
					m_vParticleEmitters.erase(it);
					return true;
				}
			}
			return false;	
		}
	}

	bool ModelCodecTx::write(string filename,float fCopress)
	{

		Stream *pDataStream;
		DataChunk writeChunk;

		//最新版本是7，把所有旧的版本转换为版本4
		//写 'MVER'
		writeChunk.beginChunk('MVER', &pDataStream);
		//uint ver = m_ver;
		//uint ver = 5;
		//uint ver = 6;//当前版本号为，加入了计算aabb，加入了判断是否要骨骼插值,加入了bouding rect
		//uint ver = 7 ;//当前版本号为，加入了粒子是否绑定到骨骼
		//uint ver = 8;//当前版本号为8，容许用户自定义渲染顺序，同时飘带增加了消失模式
		//uint ver = 9;//当前版本号为9，容许用户自定飘带的向上方向
		//uint ver = 10;//当前版本号为10,把材质的参数都使用了
		//uint ver = 11;//当前版本号为11,普通粒子和MM粒子加入了旋转支持
		//uint ver = 12;//当前版本号为12, 普通粒子和MM粒子绑定到骨骼渲染时没有考虑骨骼初始矩阵的bug
		uint ver = 13;//当前版本号为13, MM粒子保存贴图遍历范围
		pDataStream->write(&ver,sizeof(ver));
		writeChunk.endChunk();

		////写'MEXT'
		//writeChunk.beginChunk('MEXT', &pDataStream);
		//uchar ucValidByte = 12;
		//pDataStream->write(&ucValidByte, sizeof(ucValidByte));
		//pDataStream->write(&m_fInitScaleX, sizeof(m_fInitScaleX));
		//pDataStream->write(&m_fInitScaleY, sizeof(m_fInitScaleY));
		//pDataStream->write(&m_fInitScaleZ, sizeof(m_fInitScaleZ));
		//char * pcPad = new char[ 64 - sizeof(float)*3];
		//memset(pcPad,0,64 - sizeof(float)*3);
		//pDataStream->write(pcPad, 64- 3*sizeof(float) );
		//delete [] pcPad;
		//writeChunk.endChunk();

		//写 'MVTX'
		if( m_pVertexData != 0 )
		{
			writeChunk.beginChunk('MVTX', &pDataStream);
			pDataStream->write(m_pVertexData, m_ui32VerticeNum * sizeof(_VertexData) );
			writeChunk.endChunk();
		}

		//写纹理动画
		if(m_vUVKFs.size() > 0 && m_vUVKFs[0].numKeyFrames() > 0 )
		{
			writeChunk.beginChunk('MURP', &pDataStream);
			int interType = static_cast<int>(m_vUVKFs[0].getInterpolationType());
			pDataStream->write(&interType, sizeof(interType) );
			int numuvkfs = m_vUVKFs[0].numKeyFrames();
			pDataStream->write(&numuvkfs, sizeof(numuvkfs));
			for( int i=0; i< numuvkfs; i++)
			{
				KeyFrame< Vector2 >  * puvkf = m_vUVKFs[0].getKeyFrame(i);
				int time = puvkf->time;
				pDataStream->write( &time, sizeof(time));
				int numuvs = m_vUVKFs.size();
				pDataStream->write(&numuvs, sizeof(numuvs));
				for( int j=0; j < numuvs; j++)
				{
					puvkf = m_vUVKFs[j].getKeyFrame(i);
					float x,y;
					x = puvkf->v.x;
					y = puvkf->v.y;
					pDataStream->write(&x, sizeof(x));
					pDataStream->write(&y, sizeof(y));
				}
			}
			writeChunk.endChunk();
		}

		//判断是否需要合并子mesh，这个功能与自定义渲染顺序冲突
		if( m_bMergeSubModel )
		{
			MODELFACE* pFace = 0;
			uint faceNum = m_pIB ? m_pIB->getNumIndices()/3 : 0;
			if( 0 != m_pIB && faceNum )
			{
				ushort * pBuffer = (ushort*)m_pIB->lock(0,0,BL_NORMAL);
				if( pBuffer != 0)
				{
					pFace = new MODELFACE[faceNum];
					for( uint i = 0; i < faceNum; i++)
					{
						pFace[i].index[0] = pBuffer[i*3 + 0];
						pFace[i].index[1] = pBuffer[i*3 + 1];
						pFace[i].index[2] = pBuffer[i*3 + 2];
					}
					m_pIB->unlock();
				}
			}

			if( pFace )
			{
				//按相同的材质分组
				typedef std::vector< std::vector<SubModel> > _RenderPassGroupContainer;
				_RenderPassGroupContainer renderPassGroup;
				uint numPass = m_vRenderPasses.size();	
				for( uint i =0; i<numPass; ++i)
				{
					bool found = false;
					for( uint j=0; j< renderPassGroup.size(); ++j )
					{
						if( (renderPassGroup[j])[0].m_matIndex == m_vRenderPasses[i].m_matIndex )
						{
							renderPassGroup[j].push_back(m_vRenderPasses[i]);
							found = true;
							break;
						}
					}

					if( !found )
					{
						std::vector<SubModel> tmpGroup;
						tmpGroup.push_back(m_vRenderPasses[i]);
						renderPassGroup.push_back(tmpGroup);
					}
				}

				//根据分组后的子mesh调整face数据
				MODELFACE* pFaceNew = new MODELFACE[faceNum];
				uint curFaceIndex = 0;
				for( uint i =0; i< renderPassGroup.size(); ++i)
				{
					for( uint j=0; j < renderPassGroup[i].size(); ++j)
					{
						for( uint k=0; k< renderPassGroup[i][j].indexCount; ++k)
						{
							uint oldfaceindex = ( renderPassGroup[i][j].indexStart + k) /3;
							uint oldfaceindexele = (renderPassGroup[i][j].indexStart + k)%3;
							uint newfaceindex = curFaceIndex / 3;
							uint newfaceindexele = curFaceIndex %3;
							pFaceNew[newfaceindex].index[newfaceindexele] = pFace[oldfaceindex].index[oldfaceindexele];
							++curFaceIndex;
						}
					}
				}

				//合并相同材质的子mesh
				std::vector<SubModel> newRenderPass;
				uint curIndexStart = 0;
				for( uint i =0; i< renderPassGroup.size(); ++i)
				{
					SubModel pass;
					pass.m_name = renderPassGroup[i][0].m_name;
					pass.m_bAnimated = false;
					pass.m_matIndex = renderPassGroup[i][0].m_matIndex;
					pass.vertexStart = renderPassGroup[i][0].vertexStart;
					pass.vertexEnd = renderPassGroup[i][0].vertexEnd;
					pass.indexStart = curIndexStart;
					pass.indexCount = 0;
					pass.setMaterial(renderPassGroup[i][0].getMaterial());
					for( uint j =0; j<renderPassGroup[i].size(); ++j)
					{
						if( renderPassGroup[i][j].m_bAnimated)
							pass.m_bAnimated = true;

						if( renderPassGroup[i][j].vertexStart < pass.vertexStart )
							pass.vertexStart = renderPassGroup[i][j].vertexStart;

						if( renderPassGroup[i][j].vertexEnd > pass.vertexEnd) 
							pass.vertexEnd = renderPassGroup[i][j].vertexEnd;

						pass.indexCount += renderPassGroup[i][j].indexCount;

						curIndexStart += renderPassGroup[i][j].indexCount;
					}

					newRenderPass.push_back(pass);
				}


				//写'MSUB'
				uint nNewSubmesh = newRenderPass.size();
				writeChunk.beginChunk('MSUB', &pDataStream);
				SUBMESH_V1_3 * pSubMesh = new SUBMESH_V1_3[nNewSubmesh];
				for( uint i=0; i < nNewSubmesh ; i++)
				{
					pSubMesh[i].istart = newRenderPass[i].indexStart;
					pSubMesh[i].icount = newRenderPass[i].indexCount;
					pSubMesh[i].vstart = newRenderPass[i].vertexStart;
					pSubMesh[i].vcount = newRenderPass[i].vertexEnd + 1 - pSubMesh[i].vstart;
					pSubMesh[i].matId = newRenderPass[i].m_matIndex;
					pSubMesh[i].animated = newRenderPass[i].m_bAnimated;
					uint namelen = newRenderPass[i].m_name.size();
					memcpy(pSubMesh[i].name, newRenderPass[i].m_name.c_str(), namelen);
					pSubMesh[i].name[namelen] = '\0';
				}
				pDataStream->write(pSubMesh, nNewSubmesh*(sizeof(SUBMESH_V1_3)) );
				delete [] pSubMesh;
				pSubMesh = 0;
				writeChunk.endChunk();



				//写'MFAC'
				writeChunk.beginChunk('MFAC', &pDataStream);
				pDataStream->write(pFaceNew, faceNum*sizeof(MODELFACE));
				writeChunk.endChunk();
				delete [] pFace;
				delete [] pFaceNew;
				pFace = 0;
				pFaceNew = 0;
			}

			//不使用自定义渲染顺序
			m_bIsEnableCustomizeRenderOrder = false;
		}
		else
		{
			//写'MSUB'
			uint nSubMesh = m_vRenderPasses.size();
			if( nSubMesh > 0)
			{
				bool bSaveTaggedSubMesh = false;
				for( uint i =0; i <m_vRenderPasses.size(); ++i )
				{
					if( !m_vShouldSavePasses[i] )
					{
						bSaveTaggedSubMesh = true;
						break;
					}
				}

				if( !bSaveTaggedSubMesh )
				{
					writeChunk.beginChunk('MSUB', &pDataStream);
					SUBMESH_V1_3 * pSubMesh = new SUBMESH_V1_3[nSubMesh];
					for( uint i=0; i < nSubMesh ; i++)
					{
						pSubMesh[i].istart = m_vRenderPasses[i].indexStart;
						pSubMesh[i].icount = m_vRenderPasses[i].indexCount;
						pSubMesh[i].vstart = m_vRenderPasses[i].vertexStart;
						pSubMesh[i].vcount = m_vRenderPasses[i].vertexEnd + 1 - pSubMesh[i].vstart;
						pSubMesh[i].matId = m_vRenderPasses[i].m_matIndex;
						pSubMesh[i].animated = m_vRenderPasses[i].m_bAnimated;
						uint namelen = m_vRenderPasses[i].m_name.size();
						memcpy(pSubMesh[i].name, m_vRenderPasses[i].m_name.c_str(), namelen);
						pSubMesh[i].name[namelen] = '\0';
					}
					pDataStream->write(pSubMesh, nSubMesh*(sizeof(SUBMESH_V1_3)) );
					delete [] pSubMesh;
					pSubMesh = 0;
					writeChunk.endChunk();

					//写'MSKN',只有在没有融合子mesh，且从文件读取蒙皮信息的情况下才会写蒙皮信息，否则写的蒙皮信息时错误的
					if( m_bLoadSkinInfoFromFile )
					{
						writeChunk.beginChunk('MSKN',&pDataStream);
						uint maxPallete = VERTEX_BLEND_MAPPINGS_NUM;
						pDataStream->write(&maxPallete, sizeof(maxPallete));
						uint maxValidMtx = VERTEX_BLEND_MATRICES_NUM;
						pDataStream->write(&maxValidMtx, sizeof(maxValidMtx));
						uint numMesh = m_vRenderPasses.size();
						pDataStream->write(&numMesh, sizeof(numMesh));
						for( uint i =0; i <numMesh; ++i)
						{
							pDataStream->write(&m_vRenderPasses[i].m_ValidIndices, sizeof(uchar));
							pDataStream->write(&m_vRenderPasses[i].m_BindedBonesIndices, sizeof(m_vRenderPasses[i].m_BindedBonesIndices));
							pDataStream->write(&m_vRenderPasses[i].m_BonesMappings, sizeof(m_vRenderPasses[i].m_BonesMappings));
						}
						writeChunk.endChunk();
					}

				}
				else
				{
					uint cntTaggedSubmesh = 0;
					for (uint i =0; i <m_vShouldSavePasses.size(); ++i)
					{
						if( m_vShouldSavePasses[i])
							++cntTaggedSubmesh;
					}

					if( cntTaggedSubmesh > 0 )
					{
						writeChunk.beginChunk('MSUB', &pDataStream);
						SUBMESH_V1_3 * pSubMesh = new SUBMESH_V1_3[cntTaggedSubmesh];
						uint j = 0;
						for( uint i=0; i < m_vRenderPasses.size() ; i++)
						{
							if( !m_vShouldSavePasses[i])
								continue;

							pSubMesh[j].istart = m_vRenderPasses[i].indexStart;
							pSubMesh[j].icount = m_vRenderPasses[i].indexCount;
							pSubMesh[j].vstart = m_vRenderPasses[i].vertexStart;
							pSubMesh[j].vcount = m_vRenderPasses[i].vertexEnd + 1 - pSubMesh[j].vstart;
							pSubMesh[j].matId = m_vRenderPasses[i].m_matIndex;
							pSubMesh[j].animated = m_vRenderPasses[i].m_bAnimated;
							uint namelen = m_vRenderPasses[i].m_name.size();
							memcpy(pSubMesh[j].name, m_vRenderPasses[i].m_name.c_str(), namelen);
							pSubMesh[j].name[namelen] = '\0';
							++j;
						}
						pDataStream->write(pSubMesh, cntTaggedSubmesh*(sizeof(SUBMESH_V1_3)) );
						delete [] pSubMesh;
						pSubMesh = 0;
						writeChunk.endChunk();

						//写'MSKN',只有在没有融合子mesh，且从文件读取蒙皮信息的情况下才会写蒙皮信息，否则写的蒙皮信息时错误的
						if( m_bLoadSkinInfoFromFile )
						{
							writeChunk.beginChunk('MSKN',&pDataStream);
							uint maxPallete = VERTEX_BLEND_MAPPINGS_NUM;
							pDataStream->write(&maxPallete, sizeof(maxPallete));
							uint maxValidMtx = VERTEX_BLEND_MATRICES_NUM;
							pDataStream->write(&maxValidMtx, sizeof(maxValidMtx));
							uint numMesh = cntTaggedSubmesh;
							pDataStream->write(&numMesh, sizeof(numMesh));
							for( uint i =0; i <m_vRenderPasses.size(); ++i)
							{
								if( !m_vShouldSavePasses[i])
									continue;

								pDataStream->write(&m_vRenderPasses[i].m_ValidIndices, sizeof(uchar));
								pDataStream->write(&m_vRenderPasses[i].m_BindedBonesIndices, sizeof(m_vRenderPasses[i].m_BindedBonesIndices));
								pDataStream->write(&m_vRenderPasses[i].m_BonesMappings, sizeof(m_vRenderPasses[i].m_BonesMappings));
							}
							writeChunk.endChunk();
						}
					}

					//不使用自定义渲染顺序
					m_bIsEnableCustomizeRenderOrder = false;

				}
			}

			//写'MFAC'
			if( m_pIB != 0 &&  m_pIB->getNumIndices() > 0)
			{
				uint nFace = m_pIB->getNumIndices() / 3;
				writeChunk.beginChunk('MFAC', &pDataStream);
				MODELFACE * pFace = new MODELFACE[nFace];
				ushort * pBuffer = (ushort*)m_pIB->lock(0,0,BL_NORMAL);
				if( pBuffer != 0)
				{
					for( uint i = 0; i < nFace; i++)
					{
						pFace[i].index[0] = pBuffer[i*3 + 0];
						pFace[i].index[1] = pBuffer[i*3 + 1];
						pFace[i].index[2] = pBuffer[i*3 + 2];
					}
					m_pIB->unlock();
					pDataStream->write(pFace,nFace * sizeof(MODELFACE));
				}
				delete[] pFace;
				pFace = 0;
				writeChunk.endChunk();
			}

		}

		//写材质
		uint numMaterials = m_vMaterials.size();
		if( numMaterials > 0)
		{
			writeChunk.beginChunk('MMTX', &pDataStream);
			pDataStream->write( &numMaterials, sizeof(numMaterials));
			for( uint i =0; i <numMaterials; ++i)
			{
				//写材质名称
				uchar matNameLen = m_vMaterials[i].getName().size();
				pDataStream->write(&matNameLen, sizeof(matNameLen));
				pDataStream->write( m_vMaterials[i].getName().c_str(), matNameLen);

				//写材质alpha值变化
				uint uiInterType;
				uiInterType = static_cast< uint>(m_vMaterials[i].m_kfTrans.getInterpolationType() );
				pDataStream->write( &uiInterType, sizeof(uiInterType));
				uint numOpacKFs = m_vMaterials[i].m_kfTrans.numKeyFrames();
				pDataStream->write( &numOpacKFs, sizeof(numOpacKFs) );
				for(uint ii =0; ii < numOpacKFs; ii++)
				{
					uint time;
					float v,in,out;
					KeyFrame<float> * pkf = m_vMaterials[i].m_kfTrans.getKeyFrame(ii);
					time = pkf->time;
					v = pkf->v;
					in = pkf->in;
					out = pkf->out;
					pDataStream->write(&time, sizeof(time));
					pDataStream->write(&v, sizeof(v));
					pDataStream->write(&in, sizeof(in));
					pDataStream->write(&out, sizeof(out));
					pkf = 0;
				}
				
				//写材质子层
				uint nTexture = m_vMaterials[i].numLayers();
				pDataStream->write( &nTexture, sizeof(nTexture) );
				for( uint ii = 0; ii < nTexture; ii ++)
				{
					//纹理名字
					CPath path (m_vMaterials[i].getLayer(ii)->m_szTexture);
					string filename = path.getFileName().c_str();
					uchar filenameLen = filename.size();
					pDataStream->write(&filenameLen,sizeof(filenameLen));
					pDataStream->write(filename.c_str(), filenameLen);
					//写混合模式
					uchar bm = static_cast<uchar>(m_vMaterials[i].getLayer(ii)->m_blendMode);
					pDataStream->write(&bm, sizeof(bm));
					//双面材质
					uchar twoside = m_vMaterials[i].getLayer(ii)->m_bTwoSide?1:0;
					pDataStream->write(&twoside, sizeof(twoside));
					//散射光
					const ColorValue & cvDiffuse = m_vMaterials[i].getLayer(ii)->m_diffuse;
					color	diffuse(cvDiffuse.a, cvDiffuse.r, cvDiffuse.g, cvDiffuse.b);
					pDataStream->write(&diffuse, sizeof(diffuse));
					//不使用光照
					uchar unshaded = m_vMaterials[i].getLayer(ii)->m_bUnshaded?1:0;
					pDataStream->write(&unshaded, sizeof(unshaded));
					//不使用雾化
					uchar unfogged = m_vMaterials[i].getLayer(ii)->m_bUnfogged?1:0;
					pDataStream->write(&unfogged, sizeof(unfogged));
					//球面环境映射
					uchar sphereEvnMap = m_vMaterials[i].getLayer(ii)->m_SphereEnvironmentMap?1:0;
					pDataStream->write(&sphereEvnMap, sizeof(sphereEvnMap));
					//不使用深度检测
					uchar nodepthtest = m_vMaterials[i].getLayer(ii)->m_bNoDepthTest?1:0;
					pDataStream->write(&nodepthtest, sizeof(nodepthtest));
					//不写深度
					uchar nodepthset = m_vMaterials[i].getLayer(ii)->m_bNoDepthSet?1:0;
					pDataStream->write(&nodepthset, sizeof(nodepthset));
					//纹理U方向截取到边
					uchar clamps = m_vMaterials[i].getLayer(ii)->m_bClampS?1:0;
					pDataStream->write(&clamps, sizeof(clamps));
					//纹理V方向截取到边
					uchar clampt = m_vMaterials[i].getLayer(ii)->m_bClampT?1:0;
					pDataStream->write(&clampt, sizeof(clampt));
					//亮度增强（已经取消)
					uchar enhanceValue = m_vMaterials[i].getLayer(ii)->m_bEnhanceValue?1:0;
					pDataStream->write(&enhanceValue, sizeof(enhanceValue));
					//纹理动画
					//写纹理的uvoffsets
					MaterialLayer * pml = m_vMaterials[i].getLayer(ii);
					//u offsets
					uint uiUInterType = static_cast<uint>( pml->m_kfUOffs.getInterpolationType() );
					pDataStream->write( &uiUInterType, sizeof(uiUInterType));
					uint numUKFs = pml->m_kfUOffs.numKeyFrames();
					pDataStream->write( &numUKFs, sizeof(numUKFs) );
					for( uint iii = 0; iii < numUKFs; iii++)
					{
						KeyFrame<float> * pkf = pml->m_kfUOffs.getKeyFrame(iii);
						uint time;
						float v,in,out;
						time = pkf->time;
						v = pkf->v;
						in = pkf->in;
						out = pkf->out;
						pDataStream->write( &time, sizeof(time));
						pDataStream->write( & v, sizeof(v));
						pDataStream->write( &in, sizeof(in));
						pDataStream->write(&out, sizeof(out));
					}
					//v offsets
					uint uiVInterType = static_cast<uint>(pml->m_kfVOffs.getInterpolationType());
					pDataStream->write( &uiVInterType, sizeof(uiVInterType));
					uint numVKFs = pml->m_kfVOffs.numKeyFrames();
					pDataStream->write( & numVKFs, sizeof(numVKFs) );
					for(uint iii = 0; iii < numVKFs; iii++)
					{
						KeyFrame<float> * pkf = pml->m_kfVOffs.getKeyFrame(iii);
						uint time;
						float v, in, out;
						time = pkf->time;
						v = pkf->v;
						in = pkf->in;
						out = pkf->out;
						pDataStream->write( &time, sizeof(time));
						pDataStream->write( & v, sizeof(v));
						pDataStream->write( &in, sizeof(in));
						pDataStream->write(&out, sizeof(out));
					}
				}
			}
			writeChunk.endChunk();
		}

		/** 版本10以前的写方法
		*/
		////写'MMTX'
		//uint numMaterials = m_vMaterials.size();
		//if( numMaterials > 0)
		//{
		//	writeChunk.beginChunk('MMTX', &pDataStream);
		//	pDataStream->write( &numMaterials, sizeof(numMaterials));
		//	for( uint i =0 ; i< numMaterials; i++)
		//	{
		//		uchar matNameLen = m_vMaterials[i].getName().size();
		//		pDataStream->write(&matNameLen, sizeof(matNameLen));
		//		pDataStream->write( m_vMaterials[i].getName().c_str(), matNameLen);
		//		bool lighting = m_vMaterials[i].getLayer(0)->m_bUnshaded ? true:false;
		//		pDataStream->write( &lighting, sizeof(lighting) );
		//		color ambient(0,0,0,0);
		//		pDataStream->write( &ambient, sizeof(ambient) );
		//		color diffuse(0,0,0,0);
		//		pDataStream->write( &diffuse, sizeof(diffuse) );
		//		color specular(0,0,0,0);
		//		pDataStream->write( &specular, sizeof(specular) );
		//		color emissive(0,0,0,0);
		//		pDataStream->write( &emissive, sizeof(emissive) );
		//		bool iTrans = false;
		//		pDataStream->write( &iTrans, sizeof(iTrans) );
		//		bool twoSide = false;	
		//		for( int ii = 0; ii < m_vMaterials[i].numLayers(); ii ++)
		//		{
		//			twoSide = m_vMaterials[i].getLayer(ii)->m_bTwoSide;
		//			break;
		//		}
		//		pDataStream->write( &twoSide, sizeof(twoSide) );

		//		//写材质的alpha值
		//		uint uiInterType;
		//		uiInterType = static_cast< uint>(m_vMaterials[i].m_kfTrans.getInterpolationType() );
		//		pDataStream->write( &uiInterType, sizeof(uiInterType));
		//		uint numOpacKFs = m_vMaterials[i].m_kfTrans.numKeyFrames();
		//		pDataStream->write( &numOpacKFs, sizeof(numOpacKFs) );
		//		for(uint ii =0; ii < numOpacKFs; ii++)
		//		{
		//			uint time;
		//			float v,in,out;
		//			KeyFrame<float> * pkf = m_vMaterials[i].m_kfTrans.getKeyFrame(ii);
		//			time = pkf->time;
		//			v = pkf->v;
		//			in = pkf->in;
		//			out = pkf->out;
		//			pDataStream->write(&time, sizeof(time));
		//			pDataStream->write(&v, sizeof(v));
		//			pDataStream->write(&in, sizeof(in));
		//			pDataStream->write(&out, sizeof(out));
		//			pkf = 0;
		//		}

		//		uint nTexture = m_vMaterials[i].numLayers();
		//		pDataStream->write( &nTexture, sizeof(nTexture) );
		//		for( uint ii = 0; ii < nTexture; ii ++)
		//		{
		//			CPath path = m_vMaterials[i].getLayer(ii)->m_szTexture;
		//			string filename = path.getFileName().c_str();
		//			uchar filenameLen = filename.size();
		//			pDataStream->write(&filenameLen,sizeof(filenameLen));
		//			pDataStream->write(filename.c_str(), filenameLen);
		//			uchar opType = static_cast<uchar>(static_cast<int>(m_vMaterials[i].getLayer(ii)->m_blendMode ));
		//			pDataStream->write(&opType, sizeof(opType));

		//			//写纹理的uvoffsets
		//			MaterialLayer * pml = m_vMaterials[i].getLayer(ii);
		//			//u offsets
		//			uint uiUInterType = static_cast<uint>( pml->m_kfUOffs.getInterpolationType() );
		//			pDataStream->write( &uiUInterType, sizeof(uiUInterType));
		//			uint numUKFs = pml->m_kfUOffs.numKeyFrames();
		//			pDataStream->write( &numUKFs, sizeof(numUKFs) );
		//			for( uint iii = 0; iii < numUKFs; iii++)
		//			{
		//				KeyFrame<float> * pkf = pml->m_kfUOffs.getKeyFrame(iii);
		//				uint time;
		//				float v,in,out;
		//				time = pkf->time;
		//				v = pkf->v;
		//				in = pkf->in;
		//				out = pkf->out;
		//				pDataStream->write( &time, sizeof(time));
		//				pDataStream->write( & v, sizeof(v));
		//				pDataStream->write( &in, sizeof(in));
		//				pDataStream->write(&out, sizeof(out));
		//			}

		//			//v offsets
		//			uint uiVInterType = static_cast<uint>(pml->m_kfVOffs.getInterpolationType());
		//			pDataStream->write( &uiVInterType, sizeof(uiVInterType));
		//			uint numVKFs = pml->m_kfVOffs.numKeyFrames();
		//			pDataStream->write( & numVKFs, sizeof(numVKFs) );
		//			for(uint iii = 0; iii < numVKFs; iii++)
		//			{
		//				KeyFrame<float> * pkf = pml->m_kfVOffs.getKeyFrame(iii);
		//				uint time;
		//				float v, in, out;
		//				time = pkf->time;
		//				v = pkf->v;
		//				in = pkf->in;
		//				out = pkf->out;
		//				pDataStream->write( &time, sizeof(time));
		//				pDataStream->write( & v, sizeof(v));
		//				pDataStream->write( &in, sizeof(in));
		//				pDataStream->write(&out, sizeof(out));
		//				
		//			}

		//			//写版本5的扩展
		//			uint _uiExtSize = sizeof(bool);
		//			pDataStream->write(&_uiExtSize, sizeof(_uiExtSize));
		//			pDataStream->write(&pml->m_bEnhanceValue, sizeof(pml->m_bEnhanceValue));
		//		}
		//	}
		//	writeChunk.endChunk();		
		//}

		//写 'MANM'
		uint32 nAnims = m_vAnimations.size();
		if( nAnims > 0)
		{
			writeChunk.beginChunk('MANM', &pDataStream);
			pDataStream->write( &nAnims, sizeof(nAnims) );
			for( uint i =0; i < nAnims; i++)
			{
				string animname = m_vAnimations[i]->getName();
				uchar animnameLen = animname.size();
				pDataStream->write(&animnameLen, sizeof( animnameLen) );
				pDataStream->write(animname.c_str(), animnameLen);
				uint startTime,endTime;
				startTime = m_vAnimations[i]->getTimeStart();
				endTime = m_vAnimations[i]->getTimeEnd();
				pDataStream->write(&startTime, sizeof(startTime));
				pDataStream->write(&endTime, sizeof(endTime));
			}
			writeChunk.endChunk();
		}

		//写 'MBON'
		uint nBones = m_vBones.size();
		if( nBones > 0)
		{
			writeChunk.beginChunk('MBON', &pDataStream);
			pDataStream->write( &nBones, sizeof(nBones) );
			for( uint i = 0; i < nBones; i++)
			{
				int id  = m_vBones[i]->objectId;
				pDataStream->write( &id, sizeof(id));
				string Jointname = m_vBones[i]->name;
				uchar JointnameLen = Jointname.size();
				pDataStream->write( &JointnameLen, sizeof(JointnameLen) );
				pDataStream->write( Jointname.c_str(), JointnameLen);
				int parent = m_vBones[i]->parentId;
				pDataStream->write( &parent, sizeof(parent) );
				Vector3 pivot = m_vBones[i]->pivotPoint;
				pDataStream->write( &pivot, sizeof(pivot) );

				//写骨骼的初始变换矩阵
				pDataStream->write(m_vBones[i]->initTrans.val,sizeof(float)*3);
				float _r[4];
				_r[0] = m_vBones[i]->initQuat.x;
				_r[1] = m_vBones[i]->initQuat.y;
				_r[2] = m_vBones[i]->initQuat.z;
				_r[3] = m_vBones[i]->initQuat.w;
				pDataStream->write(_r, sizeof(float)*4);
				pDataStream->write(m_vBones[i]->initScale.val, sizeof(float)*3);

				uint nKeyFrames = m_vBones[i]->translation.numKeyFrames();
				pDataStream->write( &nKeyFrames, sizeof(nKeyFrames) );
				for( uint ii = 0; ii < nKeyFrames; ii++)
				{
					ModelKeyframeTranslation mkft;
					mkft.time = m_vBones[i]->translation.getKeyFrame(ii)->time;
					mkft.v[0] = m_vBones[i]->translation.getKeyFrame(ii)->v[0];
					mkft.v[1] = m_vBones[i]->translation.getKeyFrame(ii)->v[1];
					mkft.v[2] = m_vBones[i]->translation.getKeyFrame(ii)->v[2];
					pDataStream->write( &mkft, sizeof(mkft) );
				} 

				nKeyFrames = m_vBones[i]->rotation.numKeyFrames();
				pDataStream->write( &nKeyFrames, sizeof(nKeyFrames) );
				for( uint ii = 0; ii < nKeyFrames; ii++)
				{
					ModelKeyframeRotation mkfr;
					mkfr.time = m_vBones[i]->rotation.getKeyFrame(ii)->time;
					mkfr.q[0] = m_vBones[i]->rotation.getKeyFrame(ii)->v.x;
					mkfr.q[1] = m_vBones[i]->rotation.getKeyFrame(ii)->v.y;
					mkfr.q[2] = m_vBones[i]->rotation.getKeyFrame(ii)->v.z;
					mkfr.q[3] = m_vBones[i]->rotation.getKeyFrame(ii)->v.w;
					pDataStream->write( &mkfr, sizeof(mkfr) );
				} 
				
				nKeyFrames = m_vBones[i]->scale.numKeyFrames();
				pDataStream->write( &nKeyFrames, sizeof( nKeyFrames) );
				for( uint ii = 0; ii < nKeyFrames; ii ++)
				{
					ModelKeyframeScale mkfs;
					mkfs.time = m_vBones[i]->scale.getKeyFrame(ii)->time;
					mkfs.v[0] = m_vBones[i]->scale.getKeyFrame(ii)->v[0];
					mkfs.v[1] = m_vBones[i]->scale.getKeyFrame(ii)->v[1];
					mkfs.v[2] = m_vBones[i]->scale.getKeyFrame(ii)->v[2];
					pDataStream->write( &mkfs, sizeof(mkfs) );
				}

				//版本6添加的参数：骨骼是否要插值
				//一般情况下使用插值，如果导出全部帧，则不插值
				uchar ucExt6 = sizeof(m_bShouldBoneInterKeyFrame);
				pDataStream->write(&ucExt6,sizeof(ucExt6));
				pDataStream->write(&m_bShouldBoneInterKeyFrame,sizeof(m_bShouldBoneInterKeyFrame));

			}
			writeChunk.endChunk();		
		}

		//只有版本6或者6以上才有
		//写aabb数据
		writeChunk.beginChunk('MBOX',&pDataStream);
		Vector3 _vMin,_vMax;
		_vMin = m_AABB.getMinimum();
		_vMax = m_AABB.getMaximum();
		pDataStream->write(_vMin.val,sizeof(float)*3);
		pDataStream->write(_vMax.val,sizeof(float)*3);
		//写bounding rect：二维的
		pDataStream->write(m_boundingBox.val,sizeof(m_boundingBox.val));
		writeChunk.endChunk();

		//只有版本8以及8以上才有
		//写自定义渲染顺序
		if( m_bIsEnableCustomizeRenderOrder )
		{
			writeChunk.beginChunk('MROR', &pDataStream);
			for( uint i = 0; i < ECustomizeRenderOrderObject_Max; ++i)
			{
				uint num = m_pCustomizeRenderOrderQueue[i].size();
				pDataStream->write(&num, sizeof(num) );
				for( uint j =0; j < num; ++j)
				{
					int order = m_pCustomizeRenderOrderQueue[i][j];
					pDataStream->write(&order, sizeof(order));
				}
			}
			writeChunk.endChunk();
		}

		//写RibbonSystem
		if(m_vRibbonEmitters.size() > 0)
		{
			writeChunk.beginChunk('MRIB',&pDataStream);
			uint numrs= m_vRibbonEmitters.size();
			pDataStream->write(&numrs, sizeof(numrs));
			for( uint i =0; i < numrs; i++)
			{
				int boneid = m_vRibbonEmitters[i]->boneObjectId;
				pDataStream->write(&boneid, sizeof(boneid));
				bool visible = !xs::Math::RealEqual(m_vRibbonEmitters[i]->visibility.getStaticData(), 0);
				pDataStream->write( &visible, sizeof(visible));
				float above = m_vRibbonEmitters[i]->heightAbove.getStaticData();
				pDataStream->write( &above, sizeof(above));
				float below = m_vRibbonEmitters[i]->heightBelow.getStaticData();
				pDataStream->write( &below, sizeof(below));
				short edgePerSecond = m_vRibbonEmitters[i]->emissionRate;
				pDataStream->write( &edgePerSecond, sizeof(edgePerSecond));
				float edgeLife = m_vRibbonEmitters[i]->lifeSpan;
				pDataStream->write( &edgeLife, sizeof(edgeLife));
				float gravity = m_vRibbonEmitters[i]->gravity;
				pDataStream->write( &gravity, sizeof(gravity));
				short rows = m_vRibbonEmitters[i]->rows;
				pDataStream->write( &rows, sizeof(rows));
				short cols = m_vRibbonEmitters[i]->columns;
				pDataStream->write( &cols, sizeof(cols));
				short slot = (short)m_vRibbonEmitters[i]->textureSlot.getStaticData();
				pDataStream->write( &slot, sizeof(slot));
				Color3 color = m_vRibbonEmitters[i]->color.getStaticData();
				pDataStream->write( &color, sizeof(color));
				float alpha = m_vRibbonEmitters[i]->alpha.getStaticData();
				pDataStream->write( &alpha, sizeof(alpha));
				short blendMode =(short) m_vRibbonEmitters[i]->filterMode;
				pDataStream->write( &blendMode, sizeof(blendMode));
				CPath path (m_vRibbonEmitters[i]->textureFilename );
				string filename = path.getFileName().c_str();	
				uchar textureFilenameLen = filename.size();
				pDataStream->write( &textureFilenameLen, sizeof(textureFilenameLen));
				pDataStream->write( filename.c_str(), textureFilenameLen);
				writeKeyFrames<float,bool>( pDataStream, &m_vRibbonEmitters[i]->visibility);
				writeKeyFrames<float,float>( pDataStream, &m_vRibbonEmitters[i]->heightAbove);
				writeKeyFrames<float,float>( pDataStream, &m_vRibbonEmitters[i]->heightBelow);
				writeKeyFrames<int,short>( pDataStream, &m_vRibbonEmitters[i]->textureSlot);
				writeKeyFrames<Color3,Color3>( pDataStream, &m_vRibbonEmitters[i]->color);
				writeKeyFrames<float,float>( pDataStream, &m_vRibbonEmitters[i]->alpha);

				//写扩展
				//uint _uiExt5Size = sizeof( bool ) + sizeof(bool) + sizeof(float)*3;
				//uint _uiExt5Size = sizeof( bool ) + sizeof(bool) + sizeof(float)*3 + sizeof(char)*16;
				//uint _uiExt5Size = sizeof( bool ) + sizeof(bool) + sizeof(float)*3 + sizeof(char)*16 + sizeof(bool) + sizeof(int);
				//uint _uiExt5Size = sizeof( bool ) + sizeof(bool) + sizeof(float)*3 + sizeof(char)*16 + sizeof(bool) + sizeof(int) + sizeof(bool) + sizeof(float)*3;
				uint _uiExt5Size = sizeof( bool ) + sizeof(bool) + sizeof(float)*3 + sizeof(char)*16 + sizeof(bool) + sizeof(int) + sizeof(bool) + sizeof(float)*3+sizeof(float);
				pDataStream->write(&_uiExt5Size, sizeof(_uiExt5Size));
				pDataStream->write(&m_vRibbonEmitters[i]->bEnhanceValue, sizeof( m_vRibbonEmitters[i]->bEnhanceValue));
				pDataStream->write(&m_vRibbonEmitters[i]->bEnableSynScale, sizeof(m_vRibbonEmitters[i]->bEnableSynScale));
				pDataStream->write(m_vRibbonEmitters[i]->vInitSynScale.val, sizeof(m_vRibbonEmitters[i]->vInitSynScale.x)*3);
				m_vRibbonEmitters[i]->name[15] = '\0';
				pDataStream->write(m_vRibbonEmitters[i]->name, sizeof(char) *16);
				pDataStream->write(&m_vRibbonEmitters[i]->attach2Bone, sizeof(bool));
				pDataStream->write(&m_vRibbonEmitters[i]->fadeoutmode, sizeof(int) );
				pDataStream->write(&m_vRibbonEmitters[i]->enableAssignUpDir, sizeof(bool));
				pDataStream->write(m_vRibbonEmitters[i]->vAssignedUpDir.val, sizeof(m_vRibbonEmitters[i]->vAssignedUpDir.x)*3);
				pDataStream->write(&m_vRibbonEmitters[i]->fadespeed,sizeof(m_vRibbonEmitters[i]->fadespeed));
			}
			writeChunk.endChunk();			
		}

		//写普通粒子系统
		if(m_vParticleEmitters.size() > 0)
		{
			writeChunk.beginChunk('MGPR', &pDataStream);
			uint numgps = m_vParticleEmitters.size();
			pDataStream->write(&numgps, sizeof(numgps));
			for( uint i=0; i < numgps; i++)
			{
				int boneid = m_vParticleEmitters[i]->boneObjectId;
				pDataStream->write(&boneid, sizeof(boneid));
				bool visible = !xs::Math::RealEqual(m_vParticleEmitters[i]->visibility.getStaticData(),0);
				pDataStream->write( &visible, sizeof(visible));
				float speed = m_vParticleEmitters[i]->speed.getStaticData();
				pDataStream->write( &speed, sizeof(speed));
				float variation = m_vParticleEmitters[i]->variation.getStaticData();
				pDataStream->write( &variation,sizeof(variation));
				float coneAngle = m_vParticleEmitters[i]->latitude.getStaticData();
				pDataStream->write( &coneAngle, sizeof(coneAngle));
				float gravity = m_vParticleEmitters[i]->gravity.getStaticData();
				pDataStream->write( &gravity, sizeof(gravity) );
				float life= m_vParticleEmitters[i]->lifeSpan;
				pDataStream->write( &life, sizeof(life));
				float emissionRate  = m_vParticleEmitters[i]->emissionRate.getStaticData();
				pDataStream->write( &emissionRate, sizeof(emissionRate));
				float width =  m_vParticleEmitters[i]->width.getStaticData();
				pDataStream->write( & width, sizeof(width));
				float length = m_vParticleEmitters[i]->length.getStaticData();
				pDataStream->write( &length, sizeof(length));
				short blendMode = (short)m_vParticleEmitters[i]->filterMode;
				pDataStream->write( &blendMode, sizeof(blendMode) );
				short rows =  m_vParticleEmitters[i]->rows;
				pDataStream->write(&rows, sizeof(rows));
				short cols= m_vParticleEmitters[i]->columns;
				pDataStream->write(&cols, sizeof(cols));
				float tailLength = m_vParticleEmitters[i]->tailLength;
				pDataStream->write(&tailLength,sizeof(tailLength));
				float timeMiddle = m_vParticleEmitters[i]->time;
				pDataStream->write(&timeMiddle, sizeof(timeMiddle));
				Color3 colorStart= m_vParticleEmitters[i]->segmentColor1;
				pDataStream->write( &colorStart, sizeof(colorStart));
				Color3 colorMiddle = m_vParticleEmitters[i]->segmentColor2;
				pDataStream->write( &colorMiddle, sizeof(colorMiddle) );
				Color3 colorEnd = m_vParticleEmitters[i]->segmentColor3;
				pDataStream->write( &colorEnd, sizeof(colorEnd));
				Vector3 alpha = m_vParticleEmitters[i]->alpha;
				pDataStream->write( &alpha,sizeof(alpha));
				Vector3 scale = m_vParticleEmitters[i]->particleScaling;
				pDataStream->write( &scale, sizeof(scale));
				typedef short short3[3];
				short3 headLifeSpan;
				headLifeSpan[0] =(short) m_vParticleEmitters[i]->headLifeSpan.val[0];
				headLifeSpan[1] =(short) m_vParticleEmitters[i]->headLifeSpan.val[1];
				headLifeSpan[2] =(short) m_vParticleEmitters[i]->headLifeSpan.val[2];
				pDataStream->write(headLifeSpan, sizeof(headLifeSpan));
				short3 headDecay;
				headDecay[0] =(float) m_vParticleEmitters[i]->headDecay.val[0];
				headDecay[1] =(float) m_vParticleEmitters[i]->headDecay.val[1];
				headDecay[2] =(float) m_vParticleEmitters[i]->headDecay.val[2];
				pDataStream->write(headDecay,sizeof(headDecay));
				short3 tailLifeSpan;
				tailLifeSpan[0] =(float) m_vParticleEmitters[i]->tailLifeSpan.val[0];
				tailLifeSpan[1] =(float) m_vParticleEmitters[i]->tailLifeSpan.val[1];
				tailLifeSpan[2] =(float) m_vParticleEmitters[i]->tailLifeSpan.val[2];
				pDataStream->write(tailLifeSpan,sizeof(tailLifeSpan));
				short3 tailDecay;
				tailDecay[0] =(float) m_vParticleEmitters[i]->tailDecay.val[0];
				tailDecay[1] =(float) m_vParticleEmitters[i]->tailDecay.val[1];
				tailDecay[2] =(float) m_vParticleEmitters[i]->tailDecay.val[2];
				pDataStream->write(tailDecay,sizeof(tailDecay));

				bool head,tail,unshaded,unfogged;
				head = m_vParticleEmitters[i]->head;
				pDataStream->write(&head, sizeof(head));
				tail = m_vParticleEmitters[i]->tail;
				pDataStream->write(&tail,sizeof(tail));
				unshaded = m_vParticleEmitters[i]->unshaded;
				pDataStream->write( &unshaded, sizeof(unshaded));
				unfogged = m_vParticleEmitters[i]->unfogged;
				pDataStream->write(&unfogged, sizeof(unfogged));				
				CPath path ( m_vParticleEmitters[i]->textureFilename );
				string filename = path.getFileName().c_str();
				uchar textureFilenameLen = filename.size();
				pDataStream->write( &textureFilenameLen, sizeof(textureFilenameLen));
				pDataStream->write( filename.c_str(), textureFilenameLen);
		
				writeKeyFrames<float,bool>(pDataStream,&m_vParticleEmitters[i]->visibility);
				writeKeyFrames<float,float>(pDataStream,&m_vParticleEmitters[i]->speed);
				writeKeyFrames<float,float>(pDataStream,&m_vParticleEmitters[i]->variation);
				writeKeyFrames<float,float>(pDataStream,&m_vParticleEmitters[i]->latitude);
				writeKeyFrames<float,float>(pDataStream,&m_vParticleEmitters[i]->gravity);
				writeKeyFrames<float,float>(pDataStream,&m_vParticleEmitters[i]->emissionRate);
				writeKeyFrames<float,float>(pDataStream,&m_vParticleEmitters[i]->width);
				writeKeyFrames<float,float>(pDataStream,&m_vParticleEmitters[i]->length);

				//写版本5的第四次扩展
				//uint _uiExt5Size = sizeof( bool ) + sizeof(bool) + sizeof(float)*3;
				//uint _uiExt5Size = sizeof( bool ) + sizeof(bool) + sizeof(float)*3 + sizeof(char) *16;
				//uint _uiExt5Size = sizeof( bool ) + sizeof(bool) + sizeof(float)*3 + sizeof(char) *16 + sizeof(bool);
				/*uint _uiExt5Size = sizeof( bool ) + sizeof(bool) + sizeof(float)*3 + sizeof(char) *16 + sizeof(bool) +
					sizeof(int) + sizeof(float);*/
				uint _uiExt5Size = sizeof( bool ) + sizeof(bool) + sizeof(float)*3 + sizeof(char) *16 + sizeof(bool) +
					sizeof(int) + sizeof(float) + sizeof(bool) + sizeof(int)*2;
				pDataStream->write(&_uiExt5Size, sizeof(_uiExt5Size));
				pDataStream->write(&m_vParticleEmitters[i]->bEnhanceValue, sizeof( m_vParticleEmitters[i]->bEnhanceValue));
				pDataStream->write(&m_vParticleEmitters[i]->bEnableSynScale, sizeof(m_vParticleEmitters[i]->bEnableSynScale));
				pDataStream->write(m_vParticleEmitters[i]->vInitSynScale.val, sizeof(m_vParticleEmitters[i]->vInitSynScale.x)*3);
				m_vParticleEmitters[i]->name[15]='\0';
				pDataStream->write(m_vParticleEmitters[i]->name,16);
				pDataStream->write(&m_vParticleEmitters[i]->bEmmitTime2Space,sizeof(m_vParticleEmitters[i]->bEmmitTime2Space));
				pDataStream->write(&m_vParticleEmitters[i]->iSpaceInterType, sizeof( m_vParticleEmitters[i]->iSpaceInterType));
				pDataStream->write(&m_vParticleEmitters[i]->fSpaceUint, sizeof(m_vParticleEmitters[i]->fSpaceUint));
				pDataStream->write(&m_vParticleEmitters[i]->attach2Bone, sizeof(m_vParticleEmitters[i]->attach2Bone));

				pDataStream->write(&m_vParticleEmitters[i]->nIntervalTime, sizeof(m_vParticleEmitters[i]->nIntervalTime));
				pDataStream->write(&m_vParticleEmitters[i]->nAngle, sizeof(m_vParticleEmitters[i]->nAngle));
				
			}
			writeChunk.endChunk();
		}
		//写'MPAR'
		if(m_vMmParticleSystemData.size() > 0)
		{
			writeChunk.beginChunk('MPAR',&pDataStream);
			uint numMmParticleSystems = m_vMmParticleSystemData.size();
			pDataStream->write( &numMmParticleSystems, sizeof(numMmParticleSystems) );
			for( uint i = 0; i < m_vMmParticleSystemData.size(); i++)
			{
				MmParticleSystemData * pdata = getMmParticleSystem(i);
				float speed = pdata->speed.getStaticData();
				float variation = pdata->variation.getStaticData();
				float spread = pdata->spread.getStaticData();
				float lat = pdata->lat.getStaticData();
				float gravity = pdata->gravity.getStaticData();
				float lifespan = pdata->lifespan.getStaticData();
				float rate = pdata->rate.getStaticData();
				float areal = pdata->areal.getStaticData();
				float areaw = pdata->areaw.getStaticData();
				float grav2 = pdata->grav2.getStaticData();
				pDataStream->write(&speed, sizeof(speed));
				writeMmKeyFrames(pDataStream, pdata->speed);
				pDataStream->write(&variation, sizeof(variation));
				writeMmKeyFrames(pDataStream, pdata->variation);
				pDataStream->write(&spread, sizeof(spread));
				writeMmKeyFrames(pDataStream, pdata->spread);
				pDataStream->write(&lat, sizeof(lat));
				writeMmKeyFrames(pDataStream, pdata->lat);
				pDataStream->write(&gravity, sizeof(gravity));
				writeMmKeyFrames(pDataStream, pdata->gravity);
				pDataStream->write(&lifespan, sizeof(lifespan));
				writeMmKeyFrames(pDataStream, pdata->lifespan);
				pDataStream->write(&rate, sizeof(rate));
				writeMmKeyFrames(pDataStream, pdata->rate);
				pDataStream->write(&areal, sizeof(areal));
				writeMmKeyFrames(pDataStream, pdata->areal);
				pDataStream->write(&areaw, sizeof(areaw));
				writeMmKeyFrames(pDataStream, pdata->areaw);
				pDataStream->write(&grav2, sizeof(grav2));
				writeMmKeyFrames(pDataStream, pdata->grav2);

				pDataStream->write( & pdata->p.mid, sizeof ( pdata->p.mid ) );
				uint argb =(uint) pdata->p.colors[0].getAsARGB();
				pDataStream->write( &argb, sizeof(argb) );
				argb = (uint) pdata->p.colors[1].getAsARGB();
				pDataStream->write( &argb, sizeof(argb));
				argb = (uint) pdata->p.colors[2].getAsARGB();
				pDataStream->write(&argb,sizeof(argb));
				pDataStream->write(&pdata->p.sizes[0], sizeof(pdata->p.sizes) );
				pDataStream->write(&pdata->p.d[0], sizeof(pdata->p.d) );
				pDataStream->write(&pdata->p.unk[0], sizeof(pdata->p.unk) );
				pDataStream->write(&pdata->p.scales[0], sizeof(pdata->p.scales) );
				pDataStream->write(&pdata->p.slowdown, sizeof(pdata->p.slowdown) );
				pDataStream->write(&pdata->p.rotation, sizeof(pdata->p.rotation) );
				pDataStream->write(&pdata->p.f2[0], sizeof(pdata->p.f2) );

				pDataStream->write(&pdata->pos, sizeof(pdata->pos) );
				CPath parpath ( pdata->texture );
				string parfilename = parpath.getFileName();
				uchar len = parfilename.size();
				pDataStream->write(&len, sizeof(len) );
				pDataStream->write(parfilename.c_str(),len);

				pDataStream->write(&pdata->blend, sizeof(pdata->blend));
				pDataStream->write(&pdata->rows, sizeof(pdata->rows));
				pDataStream->write(&pdata->cols, sizeof(pdata->cols));
				pDataStream->write(&pdata->emitterType, sizeof(pdata->emitterType));
				pDataStream->write(&pdata->type, sizeof(pdata->type));
				pDataStream->write(&pdata->s2, sizeof(pdata->s2));
				pDataStream->write(&pdata->bone, sizeof(pdata->bone));
				int flags;
				if( pdata->billboardType == BILLBOARD_LOCK_NONE)//只保存两种
				{
					flags = 0;
				}
				else 
				{
					flags = 4096;
				}
				pDataStream->write(&flags, sizeof(flags));

				//写版本5的第四次扩展
				//uint _uiExt5Size = sizeof( bool ) + sizeof(bool) + sizeof(float)*3;
				//uint _uiExt5Size = sizeof( bool ) + sizeof(bool) + sizeof(float)*3 + sizeof(char)*16;
				//uint _uiExt5Size = sizeof( bool ) + sizeof(bool) + sizeof(float)*3 + sizeof(char)*16 + sizeof(bool);
				/*uint _uiExt5Size = sizeof( bool ) + sizeof(bool) + sizeof(float)*3 + sizeof(char)*16 + sizeof(bool)
					+ sizeof(int) + sizeof(float);*/
				uint _uiExt5Size = sizeof( bool ) + sizeof(bool) + sizeof(float)*3 + sizeof(char)*16 + sizeof(bool)
					+ sizeof(int) + sizeof(float) + sizeof(short) + sizeof(int)*2 + sizeof(int)*2;
				pDataStream->write(&_uiExt5Size, sizeof(_uiExt5Size));
				pDataStream->write(&pdata->bEnhanceValue, sizeof(pdata->bEnhanceValue));
				pDataStream->write(&pdata->bEnableSynScale, sizeof(pdata->bEnableSynScale));
				pDataStream->write(pdata->vInitSynScale.val, sizeof(pdata->vInitSynScale.x)*3);
				pdata->name[15] = '\0';
				pDataStream->write(pdata->name, 16);
				pDataStream->write(&pdata->bEmmitTime2Space, sizeof( pdata->bEmmitTime2Space));
				pDataStream->write(&pdata->iSpaceInterType, sizeof( pdata->iSpaceInterType));
				pDataStream->write(&pdata->fSpaceUint, sizeof( pdata->fSpaceUint));
				pDataStream->write(&pdata->attach2Bone, sizeof(pdata->attach2Bone));

				pDataStream->write(&pdata->nIntervalTime, sizeof(pdata->nIntervalTime));
				pDataStream->write(&pdata->nAngle, sizeof(pdata->nAngle));

				pDataStream->write(&pdata->iteratorArea, sizeof(pdata->iteratorArea));
			}
			writeChunk.endChunk();
		}

		writeChunk.save(filename.c_str() ) ; 
		return true;
	}


	void * ModelCodecTx::getMMParticleSystemParam(unsigned int index,string paramname, string & type, int & count )
	{
		//通用检测
		if( index >= m_vMmParticleSystemData.size() ) return 0;

		MmParticleSystemData & mmpsdata = *m_vMmParticleSystemData[index];
		
		//基本参数
		if(paramname == "texture") return getMMPSParamVector( &mmpsdata.texture, "string",type,1,count);
		if( paramname == "bone") return getMMPSParamVector(&mmpsdata.bone, "short",type,1,count);
		if( paramname == "billboardType")
		{
			short * preturn = new short[1];
			*preturn = static_cast<short>(mmpsdata.billboardType);
			type = "short";
			count =1;
			return preturn;
		}
		if( paramname == "pos") return getMMPSParamVector(mmpsdata.pos.val, "float",type,3,count);
		if( paramname == "blend") return getMMPSParamVector(&mmpsdata.blend, "short",type,1,count);
		if( paramname == "emitterType") return getMMPSParamVector(&mmpsdata.emitterType, "short",type,1,count);
		if( paramname == "type") return getMMPSParamVector(&mmpsdata.type, "short",type,1,count);
		if( paramname == "cols") return getMMPSParamVector(&mmpsdata.cols, "short",type,1,count);
		if( paramname == "rows") return getMMPSParamVector(&mmpsdata.rows, "short",type,1,count);

		//附加参数
		if( paramname == "mid") return getMMPSParamVector(&mmpsdata.p.mid, "float",type,1,count);
		if( paramname == "colors")
		{
			float * preturn = new float[12];
			for( int i = 0; i < 3; i++)
			{
				for( int j = 0 ; j < 4; j++)
				{
					preturn[i*4+j] = mmpsdata.p.colors[i].val[j];
				}
			}
			type = "float";
			count = 12;
			return preturn;
		}
		if( paramname == "sizes") return getMMPSParamVector(mmpsdata.p.sizes, "float",type,3,count);
		if( paramname == "d") return getMMPSParamVector(mmpsdata.p.d, "short",type,10,count);
		if( paramname == "unk") return getMMPSParamVector(mmpsdata.p.unk, "float",type,3,count);
		if( paramname == "scales") return getMMPSParamVector(mmpsdata.p.scales, "float",type,3,count);
		if( paramname == "slowdown") return getMMPSParamVector(&mmpsdata.p.slowdown, "float",type,1,count);
		if( paramname == "rotation") return getMMPSParamVector(&mmpsdata.p.rotation, "float",type,1,count);
		if( paramname == "f2") return getMMPSParamVector(mmpsdata.p.f2, "float",type,16,count);

		//帧数据处理
		if( paramname == "speed") return getMMPSKeyFramesParam(&mmpsdata.speed, "float", type, count);
		if( paramname == "variation") return getMMPSKeyFramesParam(&mmpsdata.variation, "float", type, count);
		if( paramname == "spread") return getMMPSKeyFramesParam(&mmpsdata.spread, "float", type, count);
		if( paramname == "lat") return getMMPSKeyFramesParam(&mmpsdata.lat, "float", type, count);
		if( paramname == "gravity") return getMMPSKeyFramesParam(&mmpsdata.gravity, "float", type, count);
		if( paramname == "lifespan") return getMMPSKeyFramesParam(&mmpsdata.lifespan, "float", type, count);
		if( paramname == "rate") return getMMPSKeyFramesParam(&mmpsdata.rate, "float", type, count);
		if( paramname == "areal") return getMMPSKeyFramesParam(&mmpsdata.areal, "float", type, count);
		if( paramname == "areaw") return getMMPSKeyFramesParam(&mmpsdata.areaw, "float", type, count);
		if( paramname == "grav2") return getMMPSKeyFramesParam(&mmpsdata.grav2, "float", type, count);

		//对版本5的扩展
		if( paramname == "enhancevalue") return getMMPSParamVector(&mmpsdata.bEnhanceValue, "bool",type,1,count);
		if( paramname == "enablesynscale") return getMMPSParamVector(&mmpsdata.bEnableSynScale, "bool",type,1,count);
		if( paramname == "initsynscale") return getMMPSParamVector(mmpsdata.vInitSynScale.val, "float",type,3,count);

		if( paramname == "name" )
		{
			count = strlen(mmpsdata.name)+1;
			type = "char";
			char * preturn = new char[count];
			memcpy(preturn, mmpsdata.name,count);
			preturn[count-1] ='\0';
			return preturn;
		}

		if( paramname == "emmittime2space" ) return getMMPSParamVector(&mmpsdata.bEmmitTime2Space,"bool",type,1,count);
		if( paramname == "spaceintertype" ) return getMMPSParamVector(&mmpsdata.iSpaceInterType, "int",type,1,count);
		if( paramname == "spaceunit" ) return getMMPSParamVector(&mmpsdata.fSpaceUint,"float",type,1,count);
		if( paramname == "attach2bone" ) return  getMMPSParamVector(&mmpsdata.attach2Bone,"short",type,1,count);

		//对版本8的扩展
		if( paramname == "rotateZAxis")
		{
			int* preturn = new int[2];
			preturn[0] = mmpsdata.nIntervalTime;
			preturn[1] = mmpsdata.nAngle;
			type = "int";
			count = 2;
			return preturn;
		}

		//对版本9的扩展
		if( paramname == "iteratorarea") return  getMMPSParamVector(&mmpsdata.iteratorArea,"int",type,2,count);
		//默认处理
		return 0;
	}

	/*
	void * ModelCodecTx::aidGetMMPSParamVector( void * pParam, string paramtype, string & type, int paramcount ,int & count)
	{
		if( paramcount <= 0 ) return 0;
		if( pParam == 0) return 0;

		type = paramtype;
		count = paramcount;

		if( paramtype == "bool")
		{
			bool * pbool = (bool *)pParam;
			bool * preturn = new bool[paramcount];
			for( int i = 0; i< paramcount; i++)
			{
				preturn[i] = pbool[i];
			}
			return preturn;
		}
		if( paramtype == "short")
		{
			short * pshort = (short *) pParam;
			short * preturn = new short[paramcount];
			for(int i =0; i < paramcount; i++)
			{
				preturn[i] = pshort[i];
			}
			return preturn;
		}
		if( paramtype == "int")
		{
			int * pint = (int *) pParam;
			int * preturn = new int[paramcount];
			for( int i = 0; i<paramcount; i++)
			{
				preturn[i] = pint[i];
			}
			return preturn;
		}
		if( paramtype == "float")
		{
			float * pfloat = (float *) pParam;
			float * preturn = new float[paramcount];
			for(int i =0; i< paramcount; i++)
			{
				preturn[i] = pfloat[i];
			}
			return preturn;
		}
		if( paramtype == "string")
		{
			string * pstring = (string *) pParam;
			string * preturn = new string[paramcount];
			for( int i = 0; i< paramcount; i++)
			{
				preturn[i] = pstring[i];
			}
			return preturn;
		}
		return 0;
	}

	*/

	/*
	void * ModelCodecTx::aidGetMMPSParamsKeyFrames(MmParticleSystemData & mmpsdata, string paraname,string & type, int & count)
	{
		//type一定是"float"
		map<string, KeyFrames<float> * > parammap;
		parammap.insert(pair< string,KeyFrames<float> * >("speed", &mmpsdata.speed));
		parammap.insert(pair< string,KeyFrames<float> * >("variation", &mmpsdata.variation ));
		parammap.insert(pair< string,KeyFrames<float> * >("spread", &mmpsdata.spread));
		parammap.insert(pair< string,KeyFrames<float> * >("lat", &mmpsdata.lat ));
		parammap.insert(pair< string,KeyFrames<float> * >("gravity", &mmpsdata.gravity ));
		parammap.insert(pair< string,KeyFrames<float> * >("lifespan", &mmpsdata.lifespan ));
		parammap.insert(pair< string,KeyFrames<float> * >("rate", &mmpsdata.rate ));
		parammap.insert(pair< string,KeyFrames<float> * >("areal", &mmpsdata.areal ));
		parammap.insert(pair< string,KeyFrames<float> * >("areaw", &mmpsdata.areaw ));
		parammap.insert(pair< string,KeyFrames<float> * >("grav2", &mmpsdata.grav2 ));

		map<string, KeyFrames< float> * > ::iterator it = parammap.find(paraname);
		if ( it == parammap.end() ) return 0;

		int iCountKeyFrames = it->second->numKeyFrames();
		int iCountKeyFrameRanges = it->second->numKeyFrameRanges();
		int iCountStaticData = 0;
		( it->second->isEnableStaticData() ) ? (iCountStaticData = 1): (iCountStaticData = 0);
		float * preturn =  new float[ 1 + 1+ iCountStaticData + 1 +  4*iCountKeyFrames  + 1 + 4*iCountKeyFrameRanges ];

		int i = 0;
		preturn[ 0 ] = static_cast<float> (  it->second->getInterpolationType() );

		preturn[ 1 ] = static_cast < float> ( it->second->isEnableStaticData()?1:0);
		if( iCountStaticData > 0 ) preturn[1+1+1-1] = static_cast<float>(it->second->getStaticData() );

		preturn[ 1 + 1 + iCountStaticData + 1 -1 ] = static_cast<float>( iCountKeyFrames );
		for( i = 0; i < iCountKeyFrames; i++)
		{
			int curpos = 1 + 1 + iCountStaticData + 1 + 1 - 1 + i * 4;
			preturn[ curpos + 0] = static_cast<float>( it->second->getKeyFrame(i)->time);
			preturn[curpos+1] =static_cast<float>(it->second->getKeyFrame(i)->v);
			preturn[curpos + 2] = static_cast<float> (it->second->getKeyFrame(i)->in);
			preturn[curpos + 3 ] = static_cast<float>(it->second->getKeyFrame(i)->out );
		}

		preturn[ 1 + 1 + iCountStaticData + 1 + iCountKeyFrames * 4 + 1 -1] = static_cast<float>(iCountKeyFrameRanges);
		for( i = 0; i < iCountKeyFrameRanges; i ++)
		{
			int curpos = 1 + 1 + iCountStaticData + 1 + iCountKeyFrames * 4 + 1 + 1 -1 + 2 * i;
			preturn[curpos + 0] = static_cast<float>(it->second->getKeyFrameRange(i)->first );
			preturn[curpos + 1] = static_cast<float>(it->second->getKeyFrameRange(i)->second );
		}
		count = 1 + 1 + iCountStaticData + 1 + iCountKeyFrames * 4 + 1 + iCountKeyFrameRanges * 2;
		type ="float";
		 return preturn;
	}

	*/

	/*
	bool ModelCodecTx::aidSetMMPSParamVector(void * psrc, int srcnum, string srctype, void* pdes, int desnum, string destype, bool enablerange, float min, float max )
	{
		//通用检测
		if( psrc == 0 || pdes == 0 ) return false;
		if( srcnum <= 0 || desnum <= 0 ) return false;
		if( srcnum != desnum ) return false;
		if( srctype != destype ) return false;

		if( srctype == "string")
		{
			string * strsrc = (string *)psrc;
			string * strdes = (string *)pdes;
			for( int i = 0; i < srcnum; i++) strdes[i] = strsrc[i];
			return true;
		}

		if(srctype == "short")
		{
			short* ssrc = (short *) psrc;
			short* sdes = (short * )pdes;
			if ( enablerange == true )
			{
				for( int i = 0; i < srcnum; i ++ ) 
					if( (float )ssrc[i] < min|| (float )ssrc[i] > max+0.5 ) return false;
			}
			for( int i=0;i < srcnum;i++) sdes[i] = ssrc[i];
			return true;
		}
		if(srctype == "bool")
		{
			bool* ssrc = (bool *) psrc;
			bool* sdes = (bool * )pdes;
			for( int i=0;i < srcnum;i++) sdes[i] = ssrc[i];
			return true;
		}

		if(srctype == "int")
		{
			int* isrc = (int *) psrc;
			int* ides = (int * )pdes;
			if( enablerange == true)
			{
				for( int i = 0; i < srcnum ; i++)
					if( (float)isrc[i] < min || (float)isrc[i] > max ) return false;
			}
			for( int i=0;i < srcnum;i++) ides[i] = isrc[i];
			return true;
		}

		if(srctype == "float")
		{
			float* fsrc = (float *) psrc;
			float* fdes = (float * )pdes;
			if( enablerange == true)
			{
				for(int i = 0; i< srcnum; i++)
					if( fsrc[i] < min || fsrc[i] > max ) return false;
			}
			for( int i=0;i < srcnum;i++) fdes[i] = fsrc[i];
			return true;
		}

		return false;
	}

	*/
	
	/*
	bool ModelCodecTx::setMMPSKeyFramesParam(KeyFrames<float> * pkf, float * pParamVal, int iParamNum, string szParamType)
	{
		//帧序列格式 interpolationType,enablestaticdata, StaticData, numoftimeframe, frame1,...,frameN, numofrangeframe, frame1,...,frameN
		//通用检测工作
		if( szParamType != "float") return false;
		if( pkf == 0 || pParamVal == 0) return false;
		if ( pParamVal[1] < 0.5 && iParamNum < 8 ) return false;

		int staticdatanum = 0;
		int staticdatapos = -1;
		int paramframenum = 0;
		int paramframepos = -1;
		int rangeframenum = 0;
		int rangeframepos = -1;

		if ( pParamVal[1] > 0.5 )	staticdatanum = 1;
		else	staticdatanum = 0;
		paramframenum = static_cast<int>( pParamVal[1 + 1 + staticdatanum + 1 - 1 ]);
		rangeframenum = static_cast<int>( pParamVal[ 1 + 1+ staticdatanum + 1 + paramframenum *4 + 1 -1]);

		if ( 1 + 1 + staticdatanum + 1 + paramframenum*4 + 1 + rangeframenum* 2 != iParamNum ) return false;

		//数据操作
		pkf->clearKeyFramesAndKeyFrameRanges();

		unsigned int uiItpType = static_cast<unsigned int>( pParamVal[0]);
		uiItpType = uiItpType % 4;
		pkf->setInterpolationType( static_cast<InterpolationType>( uiItpType ) );

		if( pParamVal[1] > 0.5 )
		{
			staticdatapos = 2;
			pkf->setStaticData( pParamVal[staticdatapos] );
		}
		else 
		{
			pkf->enableStaticData(false);
		}

		if( paramframenum > 0 )
		{
			paramframepos = 1 + 1 + staticdatanum + 1 - 1 + 1;
			for( int i = 0; i < paramframenum ; i ++)
			{
				int curpos = paramframepos + 4*i;
				KeyFrame<float> kf( (uint)(pParamVal[curpos + 0]), pParamVal[curpos+1], pParamVal[curpos+2], pParamVal[curpos+3] );
				pkf->addKeyFrame( kf);
			}
		}

		if( rangeframenum > 0)
		{
			rangeframepos = 1 + 1+ staticdatanum + 1 + paramframenum *4 + 1 -1 +1;
			for( int i = 0; i < rangeframenum; i++)
			{
				int curpos = rangeframepos + i*2;
				KeyFrameRange kfr( static_cast<int>(pParamVal[curpos+0]), static_cast<int>(pParamVal[curpos+1]) );
				pkf->addKeyFrameRange( kfr);
			}
		}
		return true;	
	}

	*/

	bool ModelCodecTx::setMMParticleSystemParam(unsigned int index, string desname, void * psrc, int srcnum, string srctype)
	{
		//通用检测
		if( index >= m_vMmParticleSystemData.size() ) return false;
		if( psrc == 0) return false;
		if( srcnum <= 0 ) return false;



		MmParticleSystemData & mmpsdata = *m_vMmParticleSystemData[index];

		//基本参数
		if(desname == "texture") return setMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.texture, 1,"string");
		size_t bonenum = m_vBones.size();
		if(desname == "bone") return setMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.bone, 1,"short",true,-1.0000001,bonenum-1+0.0000001);
		if( desname =="billboardType")
		{
			if( srcnum != 1 || srctype != "short") return false;
			short * pshort = (short *)psrc;
			if( pshort[0] < 0 || pshort[0] > 4 ) return false;
			mmpsdata.billboardType = static_cast<BillboardType>(pshort[0]);
			return true;
		}
		if(desname == "pos") return setMMPSParamVector(psrc,srcnum,srctype,mmpsdata.pos.val, 3,"float");
		if(desname == "blend") return setMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.blend, 1,"short",true,0-0.0000001,5+0.0000001);
		if(desname == "emitterType") return setMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.emitterType, 1,"short",true,1-0.0000001,3+0.0000001);
		if(desname == "type") //修改order
		{
			bool result = setMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.type, 1,"short",true,0-0.0000001,2+0.0000001);
			if( result == true )
			{
				mmpsdata.order = (mmpsdata.type > 0 ? -1 : 0);
			}
			return result;
		}
		if(desname == "cols") 
		{
			bool result =  setMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.cols, 1,"short", true, 1-0.0000001,10000+0.0000001);
			if( result == true )//更新纹理坐标
			{
				ASSERT( mmpsdata.cols > 0 && mmpsdata.rows > 0);
				mmpsdata.vTexcoords.clear();
				for(int i= 0;i< mmpsdata.rows * mmpsdata.cols;i++)
				{
					MmTexCoordSet tc;
					initTile(tc.tc,i,mmpsdata.cols,mmpsdata.rows,mmpsdata.order);
					mmpsdata.vTexcoords.push_back(tc);
				}
			}
			return result;
		}

		if(desname == "rows") 
		{
			bool result = setMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.rows, 1,"short",true, 1-0.0000001,10000+0.0000001);
			if( result == true)//更新纹理坐标
			{
				ASSERT( mmpsdata.cols > 0 && mmpsdata.rows > 0);
				mmpsdata.vTexcoords.clear();
				for(int i= 0;i< mmpsdata.rows * mmpsdata.cols;i++)
				{
					MmTexCoordSet tc;
					initTile(tc.tc,i,mmpsdata.cols,mmpsdata.rows,mmpsdata.order);
					mmpsdata.vTexcoords.push_back(tc);
				}
			}
			return result;
		}

		//附加参数
		if( desname == "mid") return setMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.p.mid, 1,"float",true,0-0.0000001,1+0.0000001);
		if( desname == "colors")
		{
			if ( srcnum != 12) return false;
			if ( srctype != "float") return false;
			int i,j;
			float * pfloat = (float *)psrc;
			for( i=0;i<12;i++) 
			{
				if( pfloat[i] < -0.00001 || pfloat[i] > 1.00001) return false;
			}
			for(i = 0; i < 3; i++)
			{
				for( j = 0; j< 4; j++) mmpsdata.p.colors[i].val[j] = pfloat[ i*4+j];
			}
			return true;
		}
		if( desname == "sizes") return setMMPSParamVector(psrc,srcnum,srctype,mmpsdata.p.sizes, 3,"float");
		if( desname == "d") return setMMPSParamVector(psrc,srcnum,srctype,mmpsdata.p.d, 10,"short");
		if( desname == "unk") return setMMPSParamVector(psrc,srcnum,srctype,mmpsdata.p.unk, 3,"float");
		if( desname == "scales") return setMMPSParamVector(psrc,srcnum,srctype,mmpsdata.p.scales, 3,"float");
		if( desname == "slowdown") return setMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.p.slowdown, 1,"float");
		if( desname == "rotation") return setMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.p.rotation, 1,"float");
		if( desname == "f2") return setMMPSParamVector(psrc,srcnum,srctype,mmpsdata.p.f2, 16,"float");

		//处理帧数据
		if( desname == "speed") return setMMPSKeyFramesParam(&mmpsdata.speed, "float", static_cast<float *>(psrc), srcnum);
		if( desname == "variation") return setMMPSKeyFramesParam(&mmpsdata.variation, "float", static_cast<float *>(psrc), srcnum);
		if( desname == "spread") return setMMPSKeyFramesParam(&mmpsdata.spread, "float", static_cast<float *>(psrc), srcnum);
		if( desname == "lat") return setMMPSKeyFramesParam(&mmpsdata.lat, "float", static_cast<float *>(psrc), srcnum);
		if( desname == "gravity") return setMMPSKeyFramesParam(&mmpsdata.gravity, "float", static_cast<float *>(psrc), srcnum);
		if( desname == "lifespan") return setMMPSKeyFramesParam(&mmpsdata.lifespan, "float", static_cast<float *>(psrc), srcnum);
		if( desname == "rate") return setMMPSKeyFramesParam(&mmpsdata.rate, "float", static_cast<float *>(psrc), srcnum);
		if( desname == "areal") return setMMPSKeyFramesParam(&mmpsdata.areal, "float", static_cast<float *>(psrc), srcnum);
		if( desname == "areaw") return setMMPSKeyFramesParam(&mmpsdata.areaw, "float", static_cast<float *>(psrc), srcnum);
		if( desname == "grav2") return setMMPSKeyFramesParam(&mmpsdata.grav2, "float", static_cast<float *>(psrc), srcnum);	

		//版本5的扩展
		if( desname =="enhancevalue" ) return setMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.bEnhanceValue, 1,"bool");
		if( desname == "enablesynscale") return  setMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.bEnableSynScale, 1,"bool");
		if( desname == "initsynscale" ) return setMMPSParamVector(psrc,srcnum,srctype,mmpsdata.vInitSynScale.val, 3,"float");
		
		//mm粒子系统的名字
		if( desname == "name" )
		{
			if( srcnum >=16)
			{
				memcpy(mmpsdata.name,(char *)psrc,15);
				mmpsdata.name[15] = '\0';
			}
			else
			{
				memcpy(mmpsdata.name,(char *)psrc,srcnum);
				memset(&mmpsdata.name[srcnum],0,16-srcnum);
			}
			return true;
		}
		if( desname == "emmittime2space") return setMMPSParamVector( psrc, srcnum, srctype, &mmpsdata.bEmmitTime2Space,1,"bool");
		if( desname == "spaceintertype") return setMMPSParamVector( psrc, srcnum, srctype, &mmpsdata.iSpaceInterType,1,"int",true,1-0.00001,2+0.0001);
		if( desname == "spaceunit" ) return setMMPSParamVector( psrc, srcnum, srctype, &mmpsdata.fSpaceUint,1,"float",true,0+0.00001,100000000);
		if( desname == "attach2bone" ) return setMMPSParamVector( psrc, srcnum, srctype, &mmpsdata.attach2Bone,1,"short");

		//版本8的扩展
		if( desname == "rotateZAxis" )
		{
			if( srcnum != 2 || srctype != "int" ) return false;
			int* pData = (int*)psrc;
			mmpsdata.nIntervalTime = pData[0];
			mmpsdata.nAngle = pData[1];
			return true;
		}

		//版本9的扩展
		if( desname == "iteratorarea" )
		{
			if( srcnum != 2 || srctype != "int" ) return false;
			int* pData = (int*)psrc;
			if (pData[0] < 0 || pData[1] >= (mmpsdata.cols * mmpsdata.rows) )
			{
				return false;
			}
			mmpsdata.iteratorArea[0] = pData[0];
			mmpsdata.iteratorArea[1] = pData[1];
			return true;
		}
		//默认处理
		return false;
	}


	bool ModelCodecTx::addMMParticleSystem()
	{
		MmParticleSystemData * pmmpsdata = new MmParticleSystemData;
		MmParticleSystemData & mmpsdata = *pmmpsdata;

		mmpsdata.id = static_cast<int>( Math::UnitRandom() * INT_MAX );
		mmpsdata.billboardType = BILLBOARD_LOCK_NONE;
		mmpsdata.pos = Vector3(0.0, 0.0, 0.0);
		mmpsdata.bone = -1;
		mmpsdata.texture = "data\\Creature\\Common\\Effect\\";
		mmpsdata.blend = 4;
		mmpsdata.emitterType = 1;
		mmpsdata.type = 0;
		mmpsdata.order = 0;
		mmpsdata.s2 = 0 ;//暂未发现用处
		mmpsdata.cols = 1;
		mmpsdata.rows = 1;
		MmTexCoordSet tc;
		tc.tc[0] = Vector2(0,0);
		tc.tc[1] = Vector2(1,0);
		tc.tc[2] = Vector2(1,1);
		tc.tc[3] = Vector2(0,1);
		mmpsdata.vTexcoords.push_back(tc);
		mmpsdata.p.mid = 0.5;
		mmpsdata.p.colors[0] = Color4(1,1,1,1);
		mmpsdata.p.colors[1] = Color4(1,1,1,1);
		mmpsdata.p.colors[2] = Color4(1,1,1,1);
		int i = 0;
		for( i = 0; i< 3; i++)
			mmpsdata.p.sizes[i] = 1;
		for( i = 0; i < 10; i++)
			mmpsdata.p.d[i]= 0;
		for( i = 0; i < 3; i++ )
			mmpsdata.p.unk [i] = 0;
		for( i = 0; i < 3; i++)
			mmpsdata.p.scales[i] = 0 ;
		mmpsdata.p.slowdown = 0;
		mmpsdata.p.rotation = 1;
		for( i = 0; i < 16; i++)
			mmpsdata.p.f2[i] = 0;

		mmpsdata.speed.setInterpolationType( (InterpolationType) 1 );
		KeyFrame<float> kfspeed(0,0);
		mmpsdata.speed.addKeyFrame( kfspeed);

		mmpsdata.variation.setInterpolationType((InterpolationType) 1);
		KeyFrame<float> kfvariation(0,0);
		mmpsdata.variation.addKeyFrame(kfvariation);

		mmpsdata.spread.setInterpolationType((InterpolationType) 1);
		KeyFrame<float> kfspread(0,0);
		mmpsdata.spread.addKeyFrame(kfspread);

		mmpsdata.lat.setInterpolationType((InterpolationType) 1);
		KeyFrame<float> kflat(0,0);
		mmpsdata.lat.addKeyFrame(kflat);

		mmpsdata.gravity.setInterpolationType((InterpolationType) 1);
		KeyFrame<float> kfgravity(0,0);
		mmpsdata.gravity.addKeyFrame(kfgravity);

		mmpsdata.lifespan.setInterpolationType((InterpolationType) 1);
		KeyFrame<float> kflifespan(0,3);
		mmpsdata.lifespan.addKeyFrame(kflifespan);

		mmpsdata.rate.setInterpolationType((InterpolationType) 1);
		KeyFrame<float> kfrate(0,10);
		mmpsdata.rate.addKeyFrame(kfrate);

		mmpsdata.areal.setInterpolationType((InterpolationType) 1);
		KeyFrame<float> kfareal(0,10);
		mmpsdata.areal.addKeyFrame(kfareal);

		mmpsdata.areaw.setInterpolationType((InterpolationType) 1);
		KeyFrame<float> kfareaw(0,10);
		mmpsdata.areaw.addKeyFrame(kfareaw);

		mmpsdata.grav2.setInterpolationType((InterpolationType) 1);
		KeyFrame<float> kfgrav2(0,0);
		mmpsdata.grav2.addKeyFrame( kfgrav2);

		m_vMmParticleSystemData.push_back(pmmpsdata);

		//渲染顺序
		if(m_bIsEnableCustomizeRenderOrder && m_pCustomizeRenderOrderQueue )
		{	
			m_pCustomizeRenderOrderQueue[ECustomizeRenderOrderObject_MMParticleSystem].push_back( getInitRenderOrder() );
		}
		return true;
	}

	bool ModelCodecTx::removeMMParticleSystem(unsigned int index)
	{
		if( index >= m_vMmParticleSystemData.size() )  return false;

		if( m_bIsEnableCustomizeRenderOrder && m_pCustomizeRenderOrderQueue )
		{
			std::vector<MmParticleSystemData *>::iterator it = m_vMmParticleSystemData.begin();
			std::vector<int>::iterator itOrder = m_pCustomizeRenderOrderQueue[ECustomizeRenderOrderObject_MMParticleSystem].begin();
			unsigned int count = 0;
			for( ; it != m_vMmParticleSystemData.end(); ++it, ++count, ++itOrder)
			{
				if( count == index ) 
				{
					ASSERT(  *it != 0 );
					delete (*it);
					m_vMmParticleSystemData.erase(it);
					m_pCustomizeRenderOrderQueue[ECustomizeRenderOrderObject_MMParticleSystem].erase(itOrder);
					return true;
				}
			}
			return false;
		}
		else
		{
			std::vector<MmParticleSystemData *>::iterator it = m_vMmParticleSystemData.begin();
			unsigned int count = 0;
			for( ; it != m_vMmParticleSystemData.end(); ++it, ++count)
			{
				if( count == index ) 
				{
					ASSERT(  *it != 0 );
					delete (*it);
					m_vMmParticleSystemData.erase(it);
					return true;
				}
			}
			return false;	
		}		
	}




	//获取飘带系统参数
	void * ModelCodecTx::getRibbonSystemParam( unsigned int index, string paramname, string & type, int &count)
	{
		//通用检测
		if( index >= m_vRibbonEmitters.size() ) return 0;
		
		RibbonEmitterData * pre = m_vRibbonEmitters[index];

		if( paramname == "emissionRate" ) return getRSParamVector( &pre->emissionRate, "float", type, 1, count);
		if(paramname == "lifeSpan") return getRSParamVector( & pre->lifeSpan, "float", type, 1, count);
		if( paramname == "gravity" ) return getRSParamVector(&pre->gravity, "float", type, 1, count);

		if( paramname == "rows" ) return getRSParamVector(&pre->rows, "int", type, 1, count);
		if( paramname == "columns" ) return getRSParamVector(&pre->columns, "int", type, 1, count);
		if( paramname == "materialId" ) return getRSParamVector(&pre->materialId, "int", type, 1, count);
		if( paramname == "filterMode" ) return getRSParamVector(&pre->filterMode, "int", type, 1, count);
		if( paramname == "textureFilename" ) return getRSParamVector(&pre->textureFilename, "string", type, 1, count);
		if( paramname == "boneObjectId" ) return getRSParamVector(&pre->boneObjectId, "int", type, 1, count);

		if( paramname == "heightAbove") return getRSKeyFramesParam( &pre->heightAbove, "float", type, count);
		if( paramname == "heightBelow" ) return getRSKeyFramesParam( &pre->heightBelow, "float", type, count);
		if( paramname == "alpha") return  getRSKeyFramesParam( &pre->alpha, "float", type, count);
		if( paramname == "color") return getRSKeyFramesParam( &pre->color, "Color3", type, count);
		if( paramname == "textureSlot" ) return getRSKeyFramesParam( &pre->textureSlot, "int", type, count);
		if( paramname == "visibility") return  getRSKeyFramesParam( &pre->visibility, "float", type, count);

		//对版本5的扩展
		if( paramname == "enhancevalue") return getRSParamVector(&pre->bEnhanceValue, "bool",type,1,count);
		if( paramname == "enablesynscale") return getRSParamVector(&pre->bEnableSynScale, "bool",type,1,count);
		if( paramname == "initsynscale") return getRSParamVector(pre->vInitSynScale.val, "float",type,3,count);


		if( paramname == "name" )
		{
			count = strlen(pre->name)+1;
			type = "char";
			char * preturn = new char[count];
			memcpy(preturn, pre->name,count);
			preturn[count-1] ='\0';
			return preturn;
		}

		if( paramname == "attach2bone") return getRSParamVector(&pre->attach2Bone, "bool",type,1,count);
		if( paramname == "fadeoutmode" ) return getRSParamVector(&pre->fadeoutmode, "int", type,1,count);
		if( paramname == "enableassignupdir") return getRSParamVector(&pre->enableAssignUpDir, "bool",type,1,count);
		if( paramname == "assignedupdir")  return getRSParamVector(pre->vAssignedUpDir.val, "float",type,3,count);
		if( paramname == "fadespeed") return getRSParamVector(&pre->fadespeed, "float", type, 1, count);

		return 0;
	}
	//设置飘带系统参数	
	bool ModelCodecTx::setRibbonSystemParam( unsigned int index, string desname, void * psrc, int srcnum, string srctype)
	{
		//通用检测
		if( index >= m_vRibbonEmitters.size() ) return false;
		if( psrc == 0 ) return false;
		if( srcnum <=0 ) return false;

		RibbonEmitterData * pre = m_vRibbonEmitters[index];

		//基本参数
		if(desname == "emissionRate") return setRSParamVector(psrc, srcnum, srctype, &pre->emissionRate,1,"float");
		if(desname == "lifeSpan") return setRSParamVector(psrc, srcnum, srctype, &pre->lifeSpan,1,"float");
		if(desname == "gravity") return setRSParamVector(psrc, srcnum, srctype, &pre->gravity,1,"float");
		if(desname == "filterMode") return setRSParamVector(psrc, srcnum, srctype, &pre->filterMode,1,"int");
		if(desname == "textureFilename") return setRSParamVector(psrc, srcnum, srctype, &pre->textureFilename,1,"string");
		size_t bonenum = m_vBones.size();
		if(desname == "boneObjectId") return setRSParamVector(psrc, srcnum, srctype, &pre->boneObjectId,1,"int", true, -0.00001, (float)bonenum-1+0.00001);

		//帧参数

		if( desname == "heightAbove") return setRSKeyFramesParam(&pre->heightAbove,"float", static_cast<float*> (psrc), srcnum);
		if( desname == "heightBelow") return setRSKeyFramesParam(&pre->heightBelow,"float", static_cast<float*> (psrc), srcnum);
		if( desname == "alpha") return setRSKeyFramesParam(&pre->alpha,"float", static_cast<float*> (psrc), srcnum);
		if( desname == "visibility") return setRSKeyFramesParam(&pre->visibility,"float", static_cast<float*> (psrc), srcnum);
		if( desname == "color") return setRSKeyFramesParam(&pre->color,"Color3", static_cast<float*> (psrc), srcnum);

		//版本5的扩展
		if( desname =="enhancevalue" ) return setRSParamVector(psrc,srcnum,srctype,&pre->bEnhanceValue, 1,"bool");
		if( desname == "enablesynscale") return  setRSParamVector(psrc,srcnum,srctype,&pre->bEnableSynScale, 1,"bool");
		if( desname == "initsynscale" ) return setRSParamVector(psrc,srcnum,srctype,pre->vInitSynScale.val, 3,"float");

		//版本5的扩展
		//普通粒子系统的名字
		if( desname == "name" )
		{
			if( srcnum >=16)
			{
				memcpy(pre->name,(char *)psrc,15);
				pre->name[15] = '\0';
			}
			else
			{
				memcpy(pre->name,(char *)psrc,srcnum);
				memset(&pre->name[srcnum],0,16-srcnum);
			}
			return true;
		}

		if( desname =="attach2bone" ) return setRSParamVector(psrc,srcnum,srctype,&pre->attach2Bone, 1,"bool");
		if( desname == "fadeoutmode" ) return setRSParamVector(psrc,srcnum, srctype,&pre->fadeoutmode,1,"int",true,-0.00001, 1+0.0001);
		if( desname == "enableassignupdir") return setRSParamVector(psrc,srcnum, srctype,&pre->enableAssignUpDir,1,"bool");
		if( desname == "assignedupdir")  return setRSParamVector(psrc,srcnum,srctype,pre->vAssignedUpDir.val, 3,"float");
		if( desname == "fadespeed") return setRSParamVector(psrc, srcnum, srctype, &pre->fadespeed, 1, "float");
		return false;
	}

	//增加飘带系统
	bool ModelCodecTx::addRibbonSystem()
	{
		//现在不需要判定是否所有的骨骼都已经绑了飘带系统了
		if( m_vBones.size() ==  0) return false;
		RibbonEmitterData* pre = new RibbonEmitterData();
		pre->boneObjectId = 0;//默认绑定到0号骨骼

		//初始化一些数据。
		pre->filterMode = static_cast<xs::BlendMode>(4);
		pre->emissionRate = 100;
		pre->lifeSpan = 1;
		pre->gravity = 0;
		pre->textureFilename = "";

		pre->heightAbove.setStaticData(1);
		pre->heightBelow.setStaticData(1);
		pre->alpha.setStaticData(1);
		pre->visibility.setStaticData(1);
		pre->color.setStaticData( Color3(1.0f,1.0f,1.0f));
		m_vRibbonEmitters.push_back(pre);

		//渲染顺序
		if(m_bIsEnableCustomizeRenderOrder && m_pCustomizeRenderOrderQueue )
		{	
			m_pCustomizeRenderOrderQueue[ECustomizeRenderOrderObject_RibbonSystem].push_back( getInitRenderOrder() );
		}
		return true;
	}
	//删除飘带系统
	bool ModelCodecTx::removeRibbonSystem(unsigned int index)
	{
		if(index >= m_vRibbonEmitters.size() ) return false;
		
		if(  m_bIsEnableCustomizeRenderOrder && m_pCustomizeRenderOrderQueue )
		{
			std::vector<RibbonEmitterData*>::iterator it = m_vRibbonEmitters.begin();
			std::vector<int>::iterator itOrder = m_pCustomizeRenderOrderQueue[ECustomizeRenderOrderObject_RibbonSystem].begin();
			uint count =0;
			for( ; it != m_vRibbonEmitters.end(); ++it, ++count, ++itOrder)
			{
				if( count == index)
				{
					delete (*it);
					m_vRibbonEmitters.erase(it);
					m_pCustomizeRenderOrderQueue[ECustomizeRenderOrderObject_RibbonSystem].erase(itOrder);
					return true;
				}
			}
			return false;	
		}
		else
		{
			std::vector<RibbonEmitterData*>::iterator it = m_vRibbonEmitters.begin();
			uint count =0;
			for( ; it != m_vRibbonEmitters.end(); ++it, ++count)
			{
				if( count == index)
				{
					delete (*it);
					m_vRibbonEmitters.erase(it);
					return true;
				}
			}
			return false;
		}
	}

	void * ModelCodecTx::getRSParamVector( void * pParam, string paramtype, string & type, int paramcount, int & count)//谢学辉修改，慎用
	{
		//通用检测
		if( pParam == 0 ) return 0;
		if( paramcount <= 0) return 0;

		if( paramtype == "bool")
		{
			bool * pbool = (bool *)pParam;
			bool * preturn = new bool[paramcount];
			for( int i = 0; i< paramcount; i++)
			{
				preturn[i] = pbool[i];
			}
			type = paramtype;
			count  = paramcount;
			return preturn;
		}

		if ( paramtype == "int")
		{
			int * pint = static_cast<int *> (pParam);
			int * preturn = new  int[paramcount];
			for( int i = 0; i < paramcount; i++)
			{
				preturn[i] = pint[i];
			}
			type = paramtype;
			count = paramcount;
			return preturn;
		}

		if( paramtype == "float")
		{
			float * pfloat = static_cast<float *> (pParam);
			float * preturn = new float[paramcount];
			for( int i = 0; i < paramcount ; i++)
			{
				preturn[i] = pfloat[i];
			}
			type = paramtype;
			count = paramcount;
			return preturn;
		}

		if(paramtype == "string")
		{
			string * pstring = static_cast<string *>(pParam);
			string * preturn = new string[paramcount];
			for(int i =0; i < paramcount; i++)
				preturn[i] = pstring[i];
			type = paramtype;
			count = paramcount;
			return preturn;
		}
		return 0;
	}

	bool ModelCodecTx::setPSKeyFramesParam( void * pkf, string kftype, float * pParamVal, int iParamNum, bool enablerange, float min, float max)
	{
		if( pkf == 0 ) return 0;
		if( pParamVal == 0 ) return 0;

		if( kftype == "float")
		{
			//检查
			if(iParamNum < 5 ) return false;
			int kfnum = static_cast<int>( pParamVal[3]);
			if( ( 1 + 1+ 1+ 1 + 4 * kfnum + 1 ) > iParamNum ) return false;
			int kfrnum = static_cast<int> ( pParamVal[1 + 1+ 1+ 1 + 4 * kfnum ]);
			if( ( 1 + 1+ 1+ 1 + 4 * kfnum + 1 + 2*kfrnum ) != iParamNum ) return false;

			//设置
			KeyFrames<float> * pfloatkf = static_cast< KeyFrames<float> * > (pkf);
			pfloatkf->clearKeyFramesAndKeyFrameRanges();

			int iInterType = static_cast<int>( pParamVal[0] );
			if( iInterType < 0) iInterType = 0;
			if( iInterType > 3) iInterType = 3;
			pfloatkf->setInterpolationType( static_cast<InterpolationType>(iInterType) );

			bool enablestatic = ( pParamVal[1]> 0.5 ? true:false);
			pfloatkf->setStaticData( pParamVal[2] );
			pfloatkf->enableStaticData( enablestatic);

			for( int i = 0; i < kfnum; i ++ )
			{
				uint time = static_cast<uint>( pParamVal[4 + i*4 + 0]);
				float v = pParamVal[ 4+ i*4 + 1];
				float in = pParamVal[4 + i*4 +2];
				float out = pParamVal[4 + i*4 +3];
				KeyFrame<float> kf(time, v, in,out);
				pfloatkf->addKeyFrame( kf);
			}

			for( int i = 0; i < kfrnum; i ++)
			{
				KeyFrameTimeRange kfr;
				kfr.first = static_cast<uint>( pParamVal[1+1+1+1+ 4*kfnum + 1 + i *2 + 0]);
				kfr.second = static_cast<uint>( pParamVal[1+1+1+1+ 4*kfnum + 1 + i *2 + 0]);
				pfloatkf->addKeyFrameRange(kfr);
			}

			return true;
		}
		
		return false;

	}
	void * ModelCodecTx::getRSKeyFramesParam( void * pkf, string keyframetype,string & type, int & count)//谢学辉修改，慎用
	{
		if( pkf == 0 ) return 0;

		if( keyframetype == "int")
		{
			KeyFrames<int> * pintkf = static_cast< KeyFrames<int> * > (pkf);
			int iCountKeyFrames = pintkf->numKeyFrames();
			int iCountKeyFrameRanges = pintkf->numKeyFrameRanges();
			float * preturn  = new float[ 1 + 1 + 1 +1 + 4 * iCountKeyFrames + 1 + 2 * iCountKeyFrameRanges];
			preturn[0] = static_cast< float> ( pintkf->getInterpolationType() );
			preturn[1] = static_cast< float> ( pintkf->isEnableStaticData()? 1: 0 );
			preturn[2] = static_cast<float> ( pintkf->getStaticData() );
			preturn[3] = static_cast< float> ( iCountKeyFrames );
			for( int i = 0; i < iCountKeyFrames; i++)
			{
				preturn[4 + i*4 + 0] = static_cast<float>( pintkf->getKeyFrame(i)->time );
				preturn[4 + i*4 + 1] = static_cast<float>( pintkf->getKeyFrame(i)->v );
				preturn[4 + i*4 + 2] = static_cast<float>( pintkf->getKeyFrame(i)->in );
				preturn[4 + i*4 + 3] = static_cast<float>( pintkf->getKeyFrame(i)->out );
			}
			preturn[ 3 + iCountKeyFrames * 4 + 1] = static_cast<float> ( iCountKeyFrameRanges);
			for( int i = 0; i < iCountKeyFrameRanges; i++)
			{
				preturn[3 + iCountKeyFrameRanges*4 + 1 + 1 + i*2 + 0] = static_cast<float>(pintkf->getKeyFrameRange(i)->first );
				preturn[3 + iCountKeyFrameRanges*4 + 1 + 1 + i*2 + 1] = static_cast<float>(pintkf->getKeyFrameRange(i)->second);
			}
			type = "int";
			count = 1 + 1 + 1 +1 + 4 * iCountKeyFrames + 1 + 2 * iCountKeyFrameRanges;
			return preturn;
		}

		if( keyframetype =="float")
		{
			KeyFrames< float> * pfloatkf = static_cast<KeyFrames<float> * > (pkf);
			int iCountKeyFrames = pfloatkf->numKeyFrames();
			int iCountKeyFrameRanges = pfloatkf->numKeyFrameRanges();
			float * preturn  = new float[ 1 + 1 + 1 +1 + 4 * iCountKeyFrames + 1 + 2 * iCountKeyFrameRanges];
			preturn[0] = static_cast< float> ( pfloatkf->getInterpolationType() );
			preturn[1] = static_cast< float> ( pfloatkf->isEnableStaticData()? 1: 0 );
			preturn[2] = static_cast<float> ( pfloatkf->getStaticData() );
			preturn[3] = static_cast< float> ( iCountKeyFrames );
			for( int i = 0; i < iCountKeyFrames; i++)
			{
				preturn[4 + i*4 + 0] = static_cast<float>( pfloatkf->getKeyFrame(i)->time );
				preturn[4 + i*4 + 1] = static_cast<float>( pfloatkf->getKeyFrame(i)->v );
				preturn[4 + i*4 + 2] = static_cast<float>( pfloatkf->getKeyFrame(i)->in );
				preturn[4 + i*4 + 3] = static_cast<float>( pfloatkf->getKeyFrame(i)->out );
			}
			preturn[ 3 + iCountKeyFrames * 4 + 1] = static_cast<float> ( iCountKeyFrameRanges);
			for( int i = 0; i < iCountKeyFrameRanges; i++)
			{
				preturn[3 + iCountKeyFrameRanges*4 + 1 + 1 + i*2 + 0] = static_cast<float>(pfloatkf->getKeyFrameRange(i)->first );
				preturn[3 + iCountKeyFrameRanges*4 + 1 + 1 + i*2 + 1] = static_cast<float>(pfloatkf->getKeyFrameRange(i)->second);
			}
			type = "float";
			count = 1 + 1 + 1 +1 + 4 * iCountKeyFrames + 1 + 2 * iCountKeyFrameRanges;
			return preturn;		
		}

		if( keyframetype == "Color3")
		{
			KeyFrames< Color3> * pcolor3kf = static_cast<KeyFrames<Color3> * > (pkf);
			int iCountKeyFrames = pcolor3kf->numKeyFrames();
			int iCountKeyFrameRanges = pcolor3kf->numKeyFrameRanges();
			float * preturn  = new float[ 1 + 1 + 3 +1 + 10* iCountKeyFrames + 1 + 2 * iCountKeyFrameRanges];
			preturn[0] = static_cast< float> ( pcolor3kf->getInterpolationType() );
			preturn[1] = static_cast< float> ( pcolor3kf->isEnableStaticData()? 1: 0 );
			preturn[2] = static_cast<float>(pcolor3kf->getStaticData().val[0]);
			preturn[3] = static_cast<float>(pcolor3kf->getStaticData().val[1]);
			preturn[4] = static_cast<float>(pcolor3kf->getStaticData().val[2]);

			preturn[5] = static_cast< float> ( iCountKeyFrames );
			for( int i = 0; i < iCountKeyFrames; i++)
			{
				preturn[6 + i*10 + 0] = static_cast<float>( pcolor3kf->getKeyFrame(i)->time );
				preturn[6 + i*10 + 1] = static_cast<float>( pcolor3kf->getKeyFrame(i)->v.val[0]);
				preturn[6 + i*10+ 2] = static_cast<float>( pcolor3kf->getKeyFrame(i)->v.val[1]);
				preturn[6 + i*10+ 3] = static_cast<float>( pcolor3kf->getKeyFrame(i)->v.val[2]);

				preturn[6 + i*10+ 4] = static_cast<float>( pcolor3kf->getKeyFrame(i)->in.val[0]);
				preturn[6 + i*10+ 5] = static_cast<float>( pcolor3kf->getKeyFrame(i)->in.val[1]);
				preturn[6 + i*10+ 6] = static_cast<float>( pcolor3kf->getKeyFrame(i)->in.val[2]);

				preturn[6 + i*10+ 7] = static_cast<float>( pcolor3kf->getKeyFrame(i)->out.val[0]);
				preturn[6 + i*10+ 8] = static_cast<float>( pcolor3kf->getKeyFrame(i)->out.val[1]);
				preturn[6 + i*10+ 9] = static_cast<float>( pcolor3kf->getKeyFrame(i)->out.val[2]);		
			}
			preturn[ 5 + 10* iCountKeyFrames + 1] = static_cast<float> (iCountKeyFrameRanges);
			for( int i = 0; i < iCountKeyFrameRanges; i++)
			{
				preturn[ 5 + 10* iCountKeyFrames + 1 + 1 + i*2 + 0] = static_cast<float>(pcolor3kf->getKeyFrameRange(i)->first );
				preturn[ 5 + 10* iCountKeyFrames + 1 + 1 + i*2 + 1] = static_cast<float>(pcolor3kf->getKeyFrameRange(i)->second);
			}
			type = "Color3";
			count = 1 + 1 + 3 +1 + 10* iCountKeyFrames + 1 + 2 * iCountKeyFrameRanges;
			return preturn;	
		}

		return 0;
	}
	bool ModelCodecTx::setRSParamVector(void * psrc, int srcnum, string srctype, void* pdes, int desnum, string destype, bool enablerange , float min , float max  )//谢学辉修改，慎用
	{
		//通用检测
		if( psrc == 0 || pdes == 0 ) return false;
		if( srcnum <= 0 || desnum <= 0 ) return false;
		if( srcnum != desnum ) return false;
		if( srctype != destype ) return false;

		if( srctype == "string")
		{
			string * strsrc = (string *)psrc;
			string * strdes = (string *)pdes;
			for( int i = 0; i < srcnum; i++) strdes[i] = strsrc[i];
			return true;
		}

		if(srctype == "short")
		{
			short* ssrc = (short *) psrc;
			short* sdes = (short * )pdes;
			if ( enablerange == true )
			{
				for( int i = 0; i < srcnum; i ++ ) 
					if( (float )ssrc[i] < min|| (float )ssrc[i] > max+0.5 ) return false;
			}
			for( int i=0;i < srcnum;i++) sdes[i] = ssrc[i];
			return true;
		}
		if(srctype == "bool")
		{
			bool* ssrc = (bool *) psrc;
			bool* sdes = (bool * )pdes;
			for( int i=0;i < srcnum;i++) sdes[i] = ssrc[i];
			return true;
		}

		if(srctype == "int")
		{
			int* isrc = (int *) psrc;
			int* ides = (int * )pdes;
			if( enablerange == true)
			{
				for( int i = 0; i < srcnum ; i++)
					if( (float)isrc[i] < min || (float)isrc[i] > max ) return false;
			}
			for( int i=0;i < srcnum;i++) ides[i] = isrc[i];
			return true;
		}

		if(srctype == "float")
		{
			float* fsrc = (float *) psrc;
			float* fdes = (float * )pdes;
			if( enablerange == true)
			{
				for(int i = 0; i< srcnum; i++)
					if( fsrc[i] < min || fsrc[i] > max ) return false;
			}
			for( int i=0;i < srcnum;i++) fdes[i] = fsrc[i];
			return true;
		}

		return false;
	}
	bool ModelCodecTx::setRSKeyFramesParam( void * pkf, string keyframetype, float * pParamVal, int iParamNum )//谢学辉修改，设置帧参数
	{
		if( pkf == 0 ) return false;
		if( pParamVal == 0 ) return false;

		if( keyframetype == "int")
		{
			//检查
			if(iParamNum < 5 ) return false;
			int kfnum = static_cast<int>( pParamVal[3]);
			if( ( 1 + 1+ 1+ 1 + 4 * kfnum + 1 ) > iParamNum ) return false;
			int kfrnum = static_cast<int> ( pParamVal[1 + 1+ 1+ 1 + 4 * kfnum ]);
			if( ( 1 + 1+ 1+ 1 + 4 * kfnum + 1 + 2*kfrnum ) != iParamNum ) return false;

			//设置
			KeyFrames<int> * pint = static_cast< KeyFrames<int> * > (pkf);
			int iInterType = static_cast<int>( pParamVal[0] );
			if( iInterType < 0) iInterType = 0;
			if( iInterType > 3) iInterType = 3;
			pint->setInterpolationType( static_cast<InterpolationType>(iInterType) );

			bool enablestatic = ( pParamVal[1]> 0.5 ? true:false);
			pint->setStaticData( pParamVal[2] );
			pint->enableStaticData( enablestatic);

			pint->clearKeyFramesAndKeyFrameRanges();

			for( int i = 0; i < kfnum; i ++ )
			{
				uint time = static_cast<uint>( pParamVal[4 + i*4 + 0]);
				int v = (int)pParamVal[ 4+ i*4 + 1];
				int in = (int)pParamVal[4 + i*4 +2];
				int out = (int)pParamVal[4 + i*4 +3];
				KeyFrame<int> kf(time, v, in,out);
				pint->addKeyFrame( kf);
			}

			for( int i = 0; i < kfrnum; i ++)
			{
				KeyFrameTimeRange kfr;
				kfr.first = static_cast<uint>( pParamVal[1+1+1+1+ 4*kfnum + 1 + i *2 + 0]);
				kfr.second = static_cast<uint>( pParamVal[1+1+1+1+ 4*kfnum + 1 + i *2 + 0]);
				pint->addKeyFrameRange(kfr);
			}
			return true;
		}

		if( keyframetype == "float")
		{
			//检查
			if(iParamNum < 5 ) return false;
			int kfnum = static_cast<int>( pParamVal[3]);
			if( ( 1 + 1+ 1+ 1 + 4 * kfnum + 1 ) > iParamNum ) return false;
			int kfrnum = static_cast<int> ( pParamVal[1 + 1+ 1+ 1 + 4 * kfnum ]);
			if( ( 1 + 1+ 1+ 1 + 4 * kfnum + 1 + 2*kfrnum ) != iParamNum ) return false;

			//设置
			KeyFrames<float> * pfloatkf = static_cast< KeyFrames<float> * > (pkf);
			int iInterType = static_cast<int>( pParamVal[0] );
			if( iInterType < 0) iInterType = 0;
			if( iInterType > 3) iInterType = 3;
			pfloatkf->setInterpolationType( static_cast<InterpolationType>(iInterType) );

			bool enablestatic = ( pParamVal[1]> 0.5 ? true:false);
			pfloatkf->setStaticData( pParamVal[2] );
			pfloatkf->enableStaticData( enablestatic);

			pfloatkf->clearKeyFramesAndKeyFrameRanges();

			for( int i = 0; i < kfnum; i ++ )
			{
				uint time = static_cast<uint>( pParamVal[4 + i*4 + 0]);
				float v = pParamVal[ 4+ i*4 + 1];
				float in = pParamVal[4 + i*4 +2];
				float out = pParamVal[4 + i*4 +3];
				KeyFrame<float> kf(time, v, in,out);
				pfloatkf->addKeyFrame( kf);
			}

			for( int i = 0; i < kfrnum; i ++)
			{
				KeyFrameTimeRange kfr;
				kfr.first = static_cast<uint>( pParamVal[1+1+1+1+ 4*kfnum + 1 + i *2 + 0]);
				kfr.second = static_cast<uint>( pParamVal[1+1+1+1+ 4*kfnum + 1 + i *2 + 1]);
				pfloatkf->addKeyFrameRange(kfr);
			}

			return true;		
		}

		if( keyframetype == "Color3")
		{
			//检查
			if(iParamNum < 7 ) return false;
			int kfnum = static_cast<int>( pParamVal[5]);
			if( ( 1 + 1+ 3 + 1 + 10 * kfnum + 1 ) > iParamNum ) return false;
			int kfrnum = static_cast<int> ( pParamVal[1 + 1+ 3 + 1 + 10 * kfnum ]);
			if( ( 1 + 1+ 3+ 1 + 10 * kfnum + 1 + 2*kfrnum ) != iParamNum ) return false;

			//设置
			KeyFrames<Color3> * pcolor3 = static_cast< KeyFrames<Color3> * > (pkf);
			pcolor3->clearKeyFramesAndKeyFrameRanges();

			int iInterType = static_cast<int>( pParamVal[0] );
			if( iInterType < 0) iInterType = 0;
			if( iInterType > 3) iInterType = 3;
			pcolor3->setInterpolationType( static_cast<InterpolationType>(iInterType) );

			bool enablestatic = ( pParamVal[1]> 0.5 ? true:false);
			Color3 staticcolor( Vector3( pParamVal[2], pParamVal[3], pParamVal[4]) );
			pcolor3->setStaticData( staticcolor );
			pcolor3->enableStaticData( enablestatic);

			for(int i = 0; i < kfnum; i++ )
			{
				uint time = static_cast<uint>( pParamVal[6 + i*10 + 0]);
				Color3 v( Vector3( pParamVal[6+i*10+ 1], pParamVal[6 + i*10 +2], pParamVal[ 6+i*10 +3]));
				Color3 in( Vector3(pParamVal[ 6 + i*10 +4], pParamVal[ 6 + i*10 +5] ,pParamVal[ 6 + i*10 +6]) );
				Color3 out( Vector3( pParamVal[ 6 + i*10 +7],pParamVal[ 6 + i*10 +8],pParamVal[ 6 + i*10 +9] ));
				KeyFrame<Color3> kf(time, v, in, out );
				pcolor3->addKeyFrame(kf);
				
			}

			for( int i =0; i < kfrnum; i++)
			{
				KeyFrameTimeRange kfr;
				kfr.first = static_cast<uint>( pParamVal[1 + 1 + 3 + 1 + 10*kfnum + 1 +  2*i + 0]);
				kfr.second = static_cast<uint>( pParamVal[1 + 1 + 3 + 1 + 10*kfnum + 1 +  2*i + 1]);
				pcolor3->addKeyFrameRange(kfr);
			}
			
			return true;
			
		}

		return false;

	}


	void * ModelCodecTx::getMMPSParamVector( void * pParam, string paramtype, string & type, int paramcount, int & count)
	{
		if( paramcount <= 0 ) return 0;
		if( pParam == 0) return 0;

		type = paramtype;
		count = paramcount;

		if( paramtype == "bool")
		{
			bool * pbool = (bool *)pParam;
			bool * preturn = new bool[paramcount];
			for( int i = 0; i< paramcount; i++)
			{
				preturn[i] = pbool[i];
			}
			return preturn;
		}
		if( paramtype == "short")
		{
			short * pshort = (short *) pParam;
			short * preturn = new short[paramcount];
			for(int i =0; i < paramcount; i++)
			{
				preturn[i] = pshort[i];
			}
			return preturn;
		}
		if( paramtype == "int")
		{
			int * pint = (int *) pParam;
			int * preturn = new int[paramcount];
			for( int i = 0; i<paramcount; i++)
			{
				preturn[i] = pint[i];
			}
			return preturn;
		}
		if( paramtype == "float")
		{
			float * pfloat = (float *) pParam;
			float * preturn = new float[paramcount];
			for(int i =0; i< paramcount; i++)
			{
				preturn[i] = pfloat[i];
			}
			return preturn;
		}
		if( paramtype == "string")
		{
			string * pstring = (string *) pParam;
			string * preturn = new string[paramcount];
			for( int i = 0; i< paramcount; i++)
			{
				preturn[i] = pstring[i];
			}
			return preturn;
		}
		return 0;
	}

	void * ModelCodecTx::getMMPSKeyFramesParam(void * pkf, string kftype, string & type, int & count)
	{
		if( pkf == 0 ) return 0;

		if( kftype == "float")
		{
			KeyFrames<float> * pfloat = static_cast< KeyFrames<float> *> (pkf);
			int keyframesnum = pfloat->numKeyFrames();
			int keyframesrangenum = pfloat->numKeyFrameRanges();
			float * preturn = new float[ 1 + 1 + 1 + 1 +  4 *keyframesnum + 1 + 2*keyframesrangenum];

			preturn[0] = static_cast<float>(  pfloat->getInterpolationType() ) ;
			preturn[1] = static_cast<float>( pfloat->isEnableStaticData() ? 1:0);
			preturn[2] = static_cast<float>(pfloat->getStaticData() );
			preturn[3] = static_cast<float>(keyframesnum );
			for(int i = 0; i < keyframesnum; i ++)
			{
				preturn[4 + i*4 + 0] = static_cast<float>(pfloat->getKeyFrame(i)->time );
				preturn[4 + i*4 + 1] = static_cast<float>(pfloat->getKeyFrame(i)->v );
				preturn[4 + i*4 + 2] = static_cast<float>(pfloat->getKeyFrame(i)->in );
				preturn[4 + i*4 + 3] = static_cast<float>(pfloat->getKeyFrame(i)->out );
			}

			preturn[3 + keyframesnum*4 + 1] = static_cast<float>(keyframesrangenum);

			for(int i =0; i < keyframesrangenum; i++)
			{
				preturn[3+keyframesnum*4 + 1+ 1 +0] = static_cast<float>(pfloat->getKeyFrameRange(i)->first);
				preturn[3+keyframesnum*4 + 1+ 1 +1] = static_cast<float>(pfloat->getKeyFrameRange(i)->second);
			}

			type = kftype;
			count = 1 + 1 + 1 + 1 +  4 *keyframesnum + 1 + 2*keyframesrangenum;
			return preturn;
		}

		return 0;
	}

	bool ModelCodecTx::setMMPSParamVector(void * psrc, int srcnum, string srctype, void* pdes, int desnum, string destype, bool enablerange , float min , float max  )
	{
		//通用检测
		if( psrc == 0 || pdes == 0 ) return false;
		if( srcnum <= 0 || desnum <= 0 ) return false;
		if( srcnum != desnum ) return false;
		if( srctype != destype ) return false;

		if( srctype == "string")
		{
			string * strsrc = (string *)psrc;
			string * strdes = (string *)pdes;
			for( int i = 0; i < srcnum; i++) strdes[i] = strsrc[i];
			return true;
		}

		if(srctype == "short")
		{
			short* ssrc = (short *) psrc;
			short* sdes = (short * )pdes;
			if ( enablerange == true )
			{
				for( int i = 0; i < srcnum; i ++ ) 
					if( (float)ssrc[i] < min || (float )ssrc[i] > max) return false;
			}
			for( int i=0;i < srcnum;i++) sdes[i] = ssrc[i];
			return true;
		}
		if(srctype == "bool")
		{
			bool* ssrc = (bool *) psrc;
			bool* sdes = (bool * )pdes;
			for( int i=0;i < srcnum;i++) sdes[i] = ssrc[i];
			return true;
		}

		if(srctype == "int")
		{
			int* isrc = (int *) psrc;
			int* ides = (int * )pdes;
			if( enablerange == true)
			{
				for( int i = 0; i < srcnum ; i++)
					if( (float)isrc[i] < min || (float)isrc[i] > max ) return false;
			}
			for( int i=0;i < srcnum;i++) ides[i] = isrc[i];
			return true;
		}

		if(srctype == "float")
		{
			float* fsrc = (float *) psrc;
			float* fdes = (float * )pdes;
			if( enablerange == true)
			{
				for(int i = 0; i< srcnum; i++)
					if( fsrc[i] < min || fsrc[i] > max ) return false;
			}
			for( int i=0;i < srcnum;i++) fdes[i] = fsrc[i];
			return true;
		}

		return false;
	
	}

	bool ModelCodecTx::setMMPSKeyFramesParam( void * pkf, string kftype, float * pParamVal, int iParamNum, bool enablerange , float min , float max )
	{
		if( pkf == 0 ) return 0;
		if( pParamVal == 0 ) return 0;

		if( kftype == "float")
		{
			//检查
			if(iParamNum < 5 ) return false;
			int kfnum = static_cast<int>( pParamVal[3]);
			if( ( 1 + 1+ 1+ 1 + 4 * kfnum + 1 ) > iParamNum ) return false;
			int kfrnum = static_cast<int> ( pParamVal[1 + 1+ 1+ 1 + 4 * kfnum ]);
			if( ( 1 + 1+ 1+ 1 + 4 * kfnum + 1 + 2*kfrnum ) != iParamNum ) return false;

			//设置
			KeyFrames<float> * pfloatkf = static_cast< KeyFrames<float> * > (pkf);
			pfloatkf->clearKeyFramesAndKeyFrameRanges();

			int iInterType = static_cast<int>( pParamVal[0] );
			if( iInterType < 0) iInterType = 0;
			if( iInterType > 3) iInterType = 3;
			pfloatkf->setInterpolationType( static_cast<InterpolationType>(iInterType) );

			bool enablestatic = ( pParamVal[1]> 0.5 ? true:false);
			pfloatkf->setStaticData( pParamVal[2] );
			pfloatkf->enableStaticData( enablestatic);

			for( int i = 0; i < kfnum; i ++ )
			{
				uint time = static_cast<uint>( pParamVal[4 + i*4 + 0]);
				float v = pParamVal[ 4+ i*4 + 1];
				float in = pParamVal[4 + i*4 +2];
				float out = pParamVal[4 + i*4 +3];
				KeyFrame<float> kf(time, v, in,out);
				pfloatkf->addKeyFrame( kf);
			}

			for( int i = 0; i < kfrnum; i ++)
			{
				KeyFrameTimeRange kfr;
				kfr.first = static_cast<uint>( pParamVal[1+1+1+1+ 4*kfnum + 1 + i *2 + 0]);
				kfr.second = static_cast<uint>( pParamVal[1+1+1+1+ 4*kfnum + 1 + i *2 + 0]);
				pfloatkf->addKeyFrameRange(kfr);
			}

			return true;
		}
		
		return false;	
	}

	int ModelCodecTx::texBlend(const AnimationTime *pTime)
	{

#if defined(TX_TEX_VBO_STATIC_WRITE_ONLY)

		if( 0==pTime) return 0;
		//获取最大材质层数
		if(m_iMaxPassNum <= 0 )
		{
			int passnum = 0;
			for( uint i=0; i < m_vMaterials.size(); i++)
			{
				if( m_vMaterials[i].numLayers() > passnum ) passnum = m_vMaterials[i].numLayers();
			}
			m_iMaxPassNum = passnum;
		}
		if(0 == m_iMaxPassNum) return 0;//没有材质，不需要更新
		Assert( m_iMaxPassNum <=  TX_PRECOMPUTED_TEX_PASS_COUNT );

		if( !m_bAnimatedTexCoord ) return 0;
		
		uint uiVBOIndex = ( pTime->current * TX_PRECOMPUTED_TEX_FPS / 1000 ) % TX_PRECOMPUTED_TEX_VBO_COUNT;

		for( int i =0; i < m_iMaxPassNum; i++)
		{
			uint uiRealVBOIndex = i*TX_PRECOMPUTED_TEX_VBO_COUNT+uiVBOIndex;
			if( m_pPrecomputedTexVBs[uiRealVBOIndex] ) 
			{
				m_pPrecomputedTexVBsPass[i] = m_pPrecomputedTexVBs[uiRealVBOIndex];
				continue;
			}
			else
			{
				IVertexBuffer * ptexbuf = m_pRenderSystem->getBufferManager()->createVertexBuffer(8, m_ui32VerticeNum, BU_STATIC_WRITE_ONLY, CBF_IMMEDIATE);
				if( 0 == ptexbuf)
				{
					//什么也不做，保持上次的vbo对象，这样至少可以看到图像
					continue;
				}
				float * pBuffer = (float *) ptexbuf->lock(0,0,BL_DISCARD);
				if( 0 == pBuffer ) continue;
				
				for( uint j = 0; j< m_vRenderPasses.size(); j++)
				{
					Material * pmtl = 0;
					for( uint k = 0; k < m_vMaterials.size(); k++)
					{
						if ( m_vRenderPasses[j].m_matIndex == k )
						{
							pmtl = &m_vMaterials[k];
							break;
						}
					}
					if( pmtl == 0  || pmtl->numLayers()-1 < i) continue;
					MaterialLayer * pml = pmtl->getLayer(i);

					//获取uv offsets
					float uoff, voff;
					if( pml->m_kfUOffs.numKeyFrames() == 0 ) uoff =0;
					else uoff = pml->m_kfUOffs.getFrame(pTime);
					if( pml->m_kfVOffs.numKeyFrames() == 0 ) voff =0;
					else voff = pml->m_kfVOffs.getFrame(pTime);

					int start = m_vRenderPasses[j].getVertexStart();
					int end = m_vRenderPasses[j].getVertexEnd();

					if(m_vUVKFs.size() >0 && m_vUVKFs[0].numKeyFrames() > 0 )//使用了unwrap uvw修改器，mesh的纹理随时间变化
					{
						Vector2 tex;
						for( int k= start; k <= end; k++)//注意边界条件
						{
							tex = m_vUVKFs[k].getFrame(pTime);
							pBuffer[2*k+0] = tex.x + uoff;
							pBuffer[2*k+1] = tex.y + voff;
						}
					}
					else //mesh的纹理没有变化
					{
						for( int k = start; k <= end; k++)//注意边界条件
						{
							pBuffer[2*k + 0] = m_pVertexData[k].texcoords[0] + uoff;
							pBuffer[2*k + 1] = m_pVertexData[k].texcoords[1] + voff;
						}
					}	
				}
				ptexbuf->unlock();
				m_pPrecomputedTexVBs[uiRealVBOIndex]= ptexbuf;
				m_pPrecomputedTexVBsPass[i] = m_pPrecomputedTexVBs[uiRealVBOIndex];
				continue;
			}
		}
		return m_iMaxPassNum;

#else
		//判断时间间隔是否大于1000/30 tick
		if( pTime->current - m_uiPreAniTime < 33 ) 
			return m_iMaxPassNum;
		else m_uiPreAniTime = pTime->current;


		//获取材质的最大层数
		if( m_iMaxPassNum <= 0)
		{
			int passnum = 0;
			for( uint i=0; i < m_vMaterials.size(); i++)
			{
				if( m_vMaterials[i].numLayers() > passnum ) passnum = m_vMaterials[i].numLayers();
			}
			m_iMaxPassNum = passnum;
		}
		
		if( m_iMaxPassNum == 0) return 0;//此时所有的材质不存在纹理
		
		//如果不是纹理动画，返回。
		if( !m_bAnimatedTexCoord ) return m_iMaxPassNum;
		
		//纹理缓存对象不存在，重新构造纹理缓存对象
		if( m_mapPrecomputedTexBuffers.size() <= 0  ) 
		{
			for( int i = 0; i < m_iMaxPassNum; i++)
			{
				IVertexBuffer * ptexbuf = m_pRenderSystem->getBufferManager()->createVertexBuffer(8, m_ui32VerticeNum, BU_DYNAMIC);
				if( ptexbuf == 0 ) 
				{
					//清除纹理动画
					m_bAnimatedTexCoord = false;
					for( int j = 0; j < i ; j++)
						delete m_mapPrecomputedTexBuffers[j];
					m_mapPrecomputedTexBuffers.clear();
					return m_iMaxPassNum;	
				}
				m_mapPrecomputedTexBuffers.insert( pair<int,  IVertexBuffer *>(i, ptexbuf) );
			}
		}

		//更新缓存对象
		for( int i =0; i < m_iMaxPassNum; i++ )
		{
			float * pBuffer = (float *) m_mapPrecomputedTexBuffers[i]->lock(0,0,BL_NORMAL);
			if( pBuffer == 0) continue;

			for( uint j =0; j < m_vRenderPasses.size() ; j++)
			{
				Material * pmtl = 0;
				for( uint k = 0; k < m_vMaterials.size(); k++)
				{
					if ( m_vRenderPasses[j].m_matIndex == k )
					{
						pmtl = &m_vMaterials[k];
						break;
					}
				}
				if( pmtl == 0 ) continue;
				if(pmtl->numLayers() -1 < i ) continue;

				MaterialLayer * pml = pmtl->getLayer(i);

				//获取uv offsets
				//if( pml->m_kfUOffs.numKeyFrames() == 0 && pml->m_kfVOffs.numKeyFrames() == 0) continue;
				float uoff, voff;
				if( pml->m_kfUOffs.numKeyFrames() == 0 ) uoff =0;
				else uoff = pml->m_kfUOffs.getFrame(pTime);
				if( pml->m_kfVOffs.numKeyFrames() == 0 ) voff =0;
				else voff = pml->m_kfVOffs.getFrame(pTime);

				int start = m_vRenderPasses[j].getVertexStart();
				int end = m_vRenderPasses[j].getVertexEnd();

				if(m_vUVKFs.size() >0 && m_vUVKFs[0].numKeyFrames() > 0 )//使用了unwrap uvw修改器，mesh的纹理随时间变化
				{
					Vector2 tex;
					for( int k= start; k <= end; k++)//注意边界条件
					{
						tex = m_vUVKFs[k].getFrame(pTime);
						pBuffer[2*k+0] = tex.x + uoff;
						pBuffer[2*k+1] = tex.y + voff;
					}
				}
				else //mesh的纹理没有变化
				{
					for( int k = start; k <= end; k++)//注意边界条件
					{
						pBuffer[2*k + 0] = m_pVertexData[k].texcoords[0] + uoff;
						pBuffer[2*k + 1] = m_pVertexData[k].texcoords[1] + voff;
					}
				}
			}
			m_mapPrecomputedTexBuffers[i]->unlock();
		}

		return m_iMaxPassNum;	
#endif

	}

	IVertexBuffer* ModelCodecTx::getTexCoordBuffer(int pass)
	{
		if( ! m_bAnimatedTexCoord  || ( 0 == m_iMaxPassNum ) ) return m_pVBTexcoord;

#if defined(TX_TEX_VBO_STATIC_WRITE_ONLY)
		return m_pPrecomputedTexVBsPass[pass];
#else
		std::map<int ,  IVertexBuffer * >::iterator it = m_mapPrecomputedTexBuffers.find(pass);
		if( it != m_mapPrecomputedTexBuffers.end() )
			return it->second;
		else
			return m_pVBTexcoord;
#endif

	}
	bool ModelCodecTx::isAnimatedTexCoord()
	{
		return m_bAnimatedTexCoord;
	}

	void * ModelCodecTx::getExtParam(std::string strParamName, std::string & strType, int & count)
	{
		//default;
		strType = "";
		count = 0;
		return 0;
	}
	bool ModelCodecTx::setExtParam( std::string strParamName, void * psrc, int srcnum, string srctype)
	{
		if( psrc == 0) return false;

		//设置包围盒
		if( strParamName == std::string("boundingbox") && srcnum == 4 && srctype == std::string("float") )
		{
			float * pRect = static_cast<float*>(psrc);
			m_boundingBox.x = pRect[0];
			m_boundingBox.y = pRect[1];
			m_boundingBox.z = pRect[2];
			m_boundingBox.w = pRect[3];
			return true;
		}

		//设置子模型的次序
		if( strParamName == std::string("submodelindex") && srcnum == 2 && srctype == std::string("short") )
		{
			short srcIndex = (static_cast<short *>(psrc))[0];
			short desIndex = (static_cast<short*>(psrc))[1];
			if( srcIndex < 0 || static_cast<uint>(srcIndex) >= m_vRenderPasses.size() ) return false;
			if( desIndex < 0 || static_cast<uint>(desIndex) > m_vRenderPasses.size() ) return false;//可以等于renderpass的大小
			if( srcIndex == desIndex ) return true;
			short delta = 0;
			if( srcIndex > desIndex ) delta = 1;
			std::vector<SubModel>::iterator it = m_vRenderPasses.begin();
			for( short i= 0; i <desIndex; ++i)
				++it;
			m_vRenderPasses.insert(it, m_vRenderPasses[srcIndex]);
			std::vector<bool>::iterator its = m_vShouldSavePasses.begin();
			for( short i =0; i <desIndex; ++i )
				++its;
			m_vShouldSavePasses.insert(its, m_vShouldSavePasses[srcIndex]);
			if( m_pBlendProgram )
			{
				std::vector<xs::IVertexBuffer*>::iterator iti = m_vBlendIndices.begin();
				for( short i =0; i < desIndex; ++i)
					++iti;
				m_vBlendIndices.insert(iti,m_vBlendIndices[srcIndex]);
			}
			it = m_vRenderPasses.begin();
			for( short i=0; i < srcIndex+delta; ++i)
				++it;
			m_vRenderPasses.erase(it);
			its = m_vShouldSavePasses.begin();
			for( short i =0; i <srcIndex+delta; ++i )
				++its;
			m_vShouldSavePasses.erase(its);
			if( m_pBlendProgram)
			{
				std::vector<xs::IVertexBuffer*>::iterator iti = m_vBlendIndices.begin();
				for( short i=0; i <srcIndex+delta; ++i)
					++iti;
				m_vBlendIndices.erase(iti);
			}		
			return true;				
		}

		//设置材质
		if( strParamName.find("material") != std::string::npos )
		{
			std::string::size_type sep1 = strParamName.find_first_of(':');
			std::string::size_type sep2 = strParamName.find_last_of(':');
			if( sep1 == std::string::npos 
				|| sep2 == std::string::npos
				|| sep2 == sep1 )
				return false;

			std::string mtlName = strParamName.substr( sep1 +1, sep2-sep1 -1);
			std::string attribName = strParamName.substr( sep2 +1, std::string::npos);
			
			std::vector<Material>::iterator it = m_vMaterials.begin();
			for( ; it != m_vMaterials.end(); ++it)
			{
				if( it->getName() == mtlName )
					break;
			}
			if( it == m_vMaterials.end() )
				return false;

			xs::MaterialLayer* pLayer = it->getLayer(0);
			if( attribName == std::string("diffuse"))
			{
				if( srcnum != 4 || srctype != std::string("float") )
					return false;

				float* pColor = (float*)psrc;
				pLayer->m_diffuse.a = pColor[0];
				pLayer->m_diffuse.r = pColor[1];
				pLayer->m_diffuse.g = pColor[2];
				pLayer->m_diffuse.b = pColor[3];
				return true;
			}

			else if( attribName == std::string("blendmode"))
			{
				if( srcnum != 1 || srctype != std::string("short"))
					return false;

				short* sbm = (short*)psrc;
				pLayer->m_blendMode = static_cast<xs::BlendMode>(*sbm);
				return true;
			}

			else if(attribName == std::string("twoside"))
			{
				if( srcnum != 1 || srctype != std::string("short"))
					return false;

				short* sts = (short*)psrc;
				pLayer->m_bTwoSide = *sts ? true:false;
				return true;
			}

			else if (attribName == std::string("nodepthset"))
			{
				if( srcnum != 1 || srctype != std::string("short"))
					return false;

				short* sndt = (short*)psrc;
				pLayer->m_bNoDepthSet = *sndt ? true:false;
				return true;
			}

			else if( attribName == std::string("nodepthtest") )
			{
				if( srcnum != 1 || srctype != std::string("short"))
					return false;

				short* sndt = (short*)psrc;
				pLayer->m_bNoDepthTest = *sndt?true:false;
				return true;
			}

			else if( attribName == std::string("unshaded"))
			{
				if( srcnum !=1 || srctype != std::string("short"))
					return false;

				short* sus = (short*)psrc;
				pLayer->m_bUnshaded = *sus ? true:false;
				return true;
			}

			else if( attribName == std::string("unfogged"))
			{
				if( srcnum != 1 || srctype != std::string("short"))
					return false;

				short* suf = (short*)psrc;
				pLayer->m_bUnfogged = *suf?true:false;
				return true;
			}

			else if(attribName == std::string("sphereenvironmentmap"))
			{
				if( srcnum != 1 || srctype != std::string("short"))
					return false;

				short* ssem = (short*)psrc;
				pLayer->m_SphereEnvironmentMap = *ssem? true:false;
				return true;
			}

			else if( attribName == std::string("clamps"))
			{
				if( srcnum != 1 || srctype != std::string("short"))
					return false;

				short* scs = (short*)psrc;
				pLayer->m_bClampS = *scs ? true : false;
				return true;
			}

			else if(attribName == std::string("clampt"))
			{
				if( srcnum != 1 || srctype != std::string("short") )
					return false;

				short* sct = (short*)psrc;
				pLayer->m_bClampT = *sct? true:false;
				return true;
			}

			else
			{
				return  false;
			}

		}

		//合并子mesh
		if( strParamName == std::string("mergesubmodel") 
			&& srcnum == 1 
			&& srctype == std::string("short")
			&& 0 != psrc )
		{
			short * pMerge = (short*)psrc;
			m_bMergeSubModel = *pMerge?true:false;
			return true;
		}

		//设置是否保存子模型
		if( strParamName == std::string("savesubmodel")
			&& srcnum == 2
			&& srctype == std::string("short")
			&& 0 != psrc )
		{
			short submodelIndex = *((short*)(psrc));
			bool bSave = ((short*)(psrc))[1]?true:false;
			if( submodelIndex <0 || submodelIndex >= m_vShouldSavePasses.size() )
				return false;
			else
			{
				m_vShouldSavePasses[submodelIndex] = bSave;
				return true;
			}
		}

		//default
		return false;
	}

	void ModelCodecTx::getBoundingBox( Vector4 &rect)
	{
		rect = m_boundingBox;
		return;
	}

	bool ModelCodecTx::enableCustomizeRenderOrder(bool enable)
	{
		if( enable)
		{
			if( !m_pCustomizeRenderOrderQueue )
			{
				m_pCustomizeRenderOrderQueue = new std::vector<int>[ECustomizeRenderOrderObject_Max];
				if( !m_pCustomizeRenderOrderQueue) return false;
				//submodel
				for( uint i=0; i < m_vRenderPasses.size(); ++i)
					m_pCustomizeRenderOrderQueue[ECustomizeRenderOrderObject_SubModel].push_back(0);
				for( uint i=0; i < m_vParticleEmitters.size(); ++i)
					m_pCustomizeRenderOrderQueue[ECustomizeRenderOrderObject_ParticleSystem].push_back(0);
				for( uint i=0; i < m_vRibbonEmitters.size(); ++i)
					m_pCustomizeRenderOrderQueue[ECustomizeRenderOrderObject_RibbonSystem].push_back(0);
				for( uint i =0; i < m_vMmParticleSystemData.size(); ++i)
					m_pCustomizeRenderOrderQueue[ECustomizeRenderOrderObject_MMParticleSystem].push_back(0);
			}
		}
		else
		{
			if( m_pCustomizeRenderOrderQueue) 
			{
				delete [] m_pCustomizeRenderOrderQueue;
				m_pCustomizeRenderOrderQueue = 0;
			}
		}
		m_bIsEnableCustomizeRenderOrder = enable;
		return true;
	}
	bool ModelCodecTx::isEnabledCustomizeRenderOrder()
	{
		return m_bIsEnableCustomizeRenderOrder;
	}
	bool ModelCodecTx::setCustomizeRenderOrder( uint objectType, uint index, int order)
	{
		if( !m_pCustomizeRenderOrderQueue) return false;
		if( objectType >= ECustomizeRenderOrderObject_Max) return false;
		if( m_pCustomizeRenderOrderQueue[objectType].size() <= index ) return false;
		m_pCustomizeRenderOrderQueue[objectType][index] =order;
		return true;
	}
	int	ModelCodecTx::getCustomizeRenderOrder(uint objectType, uint index)
	{
		if( !m_pCustomizeRenderOrderQueue ) return 0;
		if( objectType >= ECustomizeRenderOrderObject_Max ) return 0;
		if( m_pCustomizeRenderOrderQueue[objectType].size() <= index )  return 0;
		return m_pCustomizeRenderOrderQueue[objectType][index];
	}

	int ModelCodecTx::getInitRenderOrder()
	{
		int initRenderOrder = -1;
		if(m_bIsEnableCustomizeRenderOrder && m_pCustomizeRenderOrderQueue )
		{		
			for( uint i = 0; i < ECustomizeRenderOrderObject_Max; ++i)
			{
				for( uint j =0; j < m_pCustomizeRenderOrderQueue[i].size(); ++j)
				{
					if( m_pCustomizeRenderOrderQueue[i][j] > initRenderOrder ) initRenderOrder = m_pCustomizeRenderOrderQueue[i][j];
				}
			}	
		}	
		return initRenderOrder + 1;
	}

}