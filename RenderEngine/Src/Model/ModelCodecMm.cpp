#include "StdAfx.h"
#include "ModelCodecMm.h"

namespace xs
{
	template<class T>void readKeyFrames(Stream *pStream,KeyFrames<T>& keyFrames)
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

	template<class T> void writeKeyFrames(Stream *pDataStream,KeyFrames<T>& keyFrames)//谢学辉编写，用于将关键帧写到stream对象
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

	template<class T>  void writeMMSequence(Stream *pDataStream,T &t)//谢学辉编写，用于将容器写到stream对象
	{
		T::iterator i = t.begin();
		for(; i != t.end(); ++i)
		{
			pDataStream->write(&(*i),sizeof(*i));
		}
	}

	uint ModelCodecMm::getNumSubModels()
	{
		return m_vRenderPasses.size();
	}

	ISubModel*	ModelCodecMm::getSubModel(const char* name)
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

	ISubModel*	ModelCodecMm::getSubModel(int index)
	{
		if(index < 0 || index >= (int)m_vRenderPasses.size())return 0;

		return &m_vRenderPasses[index];
	}

	Animation* ModelCodecMm::getAnimation(uint index)
	{
		if(index >= m_vAnimations.size())return 0;
		return m_vAnimations[index];
	}

	uint	ModelCodecMm::getAnimationCount()
	{
		return m_vAnimations.size();
	}

	Animation* ModelCodecMm::hasAnimation(const std::string& animation)
	{
		stdext::hash_map<std::string,Animation*>::iterator it = m_AniamtionMap.find(animation.c_str());
		if(it == m_AniamtionMap.end())return 0;

		return it->second;
	}

	//获取模型的骨架
	CoreSkeleton * ModelCodecMm::getCoreSkeletion()
	{
		return &m_skeleton;
	}


	IIndexBuffer*	ModelCodecMm::getIndexBuffer()
	{
		return m_pIB;
	}

	IVertexBuffer*	ModelCodecMm::getTexcoordBuffer()
	{
		return m_pVBTexcoord;
	}

	uint ModelCodecMm::getNumFaces()
	{
		return m_numFaces;
	}

	uint ModelCodecMm::getNumVertices()
	{
		return m_numVertices;
	}

	const std::string& ModelCodecMm::getFileName()
	{
		return m_strName;
	}

	uint ModelCodecMm::getVer()
	{
		return m_ver;
	}

	IVertexBuffer* ModelCodecMm::vertexBlend(Skeleton *pSkeleton,const AnimationTime *pTime)
	{
		if(!m_numVertices || !pTime || !pSkeleton)return 0;
		if(pSkeleton->getBoneCount() == 0 )
			return 0;

		int index = ( pTime->current * 3 / 100 ) % MM_PRECOMPUTED_VBO_COUNT_MAX;
		IVertexBuffer *pVB = m_pPrecomputedVBs[index];
		if(pVB)return pVB;
		m_pPrecomputedVBs[index] = m_pRenderSystem->getBufferManager()->createVertexBuffer(12,m_numVertices << 1,BU_STATIC_WRITE_ONLY, CBF_IMMEDIATE);
		pVB = m_pPrecomputedVBs[index];

		if(!pVB)return 0;

		Vector3* pData = (Vector3*)pVB->lock(0,0,BL_DISCARD);
		if(pData)
		{
			Vector3 *pSkinPos = &pData[0];
			Vector3 *pSkinNormal = &pData[m_numVertices];

			Bone* pBonesPointer[255];
			for(uint i=0; i<255; ++i)
			{
				pBonesPointer[i] = pSkeleton->getBone(i);
			}

			for(uint i = 0;i < m_numVertices;i++)
			{
				Vector3 v(0,0,0),n(0,0,0);
				AnimatedVertex *ov = m_pOriginVertices + i;

				for(uint b = 0;b < 4;b++) 
				{
					if(ov->weights[b] > 0) 
					{
						float ratio = ov->weights[b] / 255.0f;
						Vector3 tv,tn;
						Vector3 otv = ov->pos;

						Bone *pBone = pBonesPointer[ov->bones[b]];//pSkeleton->getBone(ov->bones[b]);
						tv = pBone->getFullTransform() * otv;

						Vector3 otn = ov->normal;
						tn = pBone->getFullRotation() * otn;

						v += tv * ratio;
						n += tn * ratio;
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

	uint ModelCodecMm::numParticleSystem()
	{
		return 0;
	}

	ParticleEmitter2Data*	ModelCodecMm::getParticleSystemData(uint index)
	{
		return 0;
	}

	uint ModelCodecMm::numRibbonSystem()
	{
		return 0;
	}

	RibbonEmitterData*	ModelCodecMm::getRibbonSystemData(uint index)
	{
		return 0;
	}

	uint ModelCodecMm::numMmParticleSystem()
	{
		return m_vMmParticleSystems.size();
	}

	MmParticleSystemData* ModelCodecMm::getMmParticleSystem(uint index)
	{
		if(index >= m_vMmParticleSystems.size())return 0;

		return &m_vMmParticleSystems[index];
	}

	const AABB&	ModelCodecMm::getAABB()
	{
		return m_aabb;
	}

	const Sphere& ModelCodecMm::getBoudingSphere()
	{
		return m_sphere;
	}

	const char*	ModelCodecMm::getType() const
	{
		static std::string strType = "mm";
		return strType.c_str();
	}

	void initTile(Vector2 *tc,int num,int cols,int rows,int order)
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

	/*
	bool ModelCodecMm::write(string filename)
	{
	Stream *pDataStream;
	DataChunk writeChunk;

	//写 'MVER'
	writeChunk.beginChunk('MVER', &pDataStream);
	pDataStream->write(&m_ver,sizeof(m_ver));
	writeChunk.endChunk();

	//写 'MGSQ'
	writeChunk.beginChunk('MGSQ', &pDataStream);
	writeMMSequence(pDataStream, m_vGlobalSequences );
	writeChunk.endChunk();

	//写 'MANI'
	writeChunk.beginChunk('MANI', &pDataStream);
	pDataStream->write(&m_bAnimatedGeometry,sizeof(m_bAnimatedGeometry));
	pDataStream->write(&m_bAnimatedBones,sizeof(m_bAnimatedBones));
	pDataStream->write(&m_bNeedPerInstanceAnimation,sizeof(m_bNeedPerInstanceAnimation));
	writeChunk.endChunk();

	//写 'MBVH'
	//此参数没用，不用写入

	//写 'MMTX'
	uint numTextures = m_vTextures.size();
	if( numTextures >  0 )
	{
	writeChunk.beginChunk('MMTX',&pDataStream);	
	pDataStream->write( &numTextures, sizeof(numTextures) );
	for( uint i = 0; i < numTextures; i++)
	{
	pDataStream->write( & m_vTextures[i].replacable, sizeof( m_vTextures[i].replacable));
	pDataStream->write( & m_vTextures[i].swrap, sizeof( m_vTextures[i].swrap));
	pDataStream->write( & m_vTextures[i].twrap, sizeof( m_vTextures[i].twrap));	
	CPath path = m_vTextures[i].filename;
	string texturefilename = path.getFileName();
	uchar filenameLen =(uchar)texturefilename.size();
	pDataStream->write(&filenameLen, sizeof(filenameLen));
	pDataStream->write(texturefilename.c_str(), filenameLen);
	}
	writeChunk.endChunk();
	}


	//写 'MMTL'
	uint numMaterials = m_vMaterials.size();
	if( numMaterials > 0)
	{
	writeChunk.beginChunk('MMTL', &pDataStream);
	for( uint i = 0; i < numMaterials; i++)
	{
	MmMaterial mm;
	mm.blendMode = m_vMaterials[i].getLayer(0)->m_blendMode;
	mm.twoSide = m_vMaterials[i].getLayer(0)->m_bTwoSide;
	mm.envmap = m_vMaterials[i].getLayer(0)->m_SphereEnvironmentMap;
	mm.unlit = m_vMaterials[i].getLayer(0)->m_bUnshaded;
	mm.zwrite = !m_vMaterials[i].getLayer(0)->m_bNoDepthSet;
	mm.textureIndex = m_vRecordMaterialstextureIndex[i];
	pDataStream->write(&mm, sizeof(mm) );
	}
	writeChunk.endChunk();
	}

	//写 'MVTX'
	if( m_numVertices > 0 )
	{
	writeChunk.beginChunk('MVTX', &pDataStream);
	for( uint i = 0 ; i < m_numVertices; i++ )
	{
	MmVertex mmv;
	AnimatedVertex * pav = dynamic_cast<AnimatedVertex *>(&mmv);
	*pav = m_pOriginVertices[i];
	Vector2 * pvector2 = (Vector2 *) m_pVBTexcoord->lock(0,0,BL_NORMAL);
	mmv.texcoords = *pvector2;
	m_pVBTexcoord->unlock();
	pDataStream->write( &mmv, sizeof(mmv));
	}
	writeChunk.endChunk();
	}

	//写 'MFAC'
	if( m_numFaces > 0 )
	{			
	ushort * pIndicesBuffer = (ushort*)m_pIB->lock(0,0,BL_NORMAL);		
	if ( pIndicesBuffer )
	{
	writeChunk.beginChunk('MFAC', &pDataStream);
	pDataStream->write( pIndicesBuffer, m_numFaces * sizeof(MmFace) );
	writeChunk.endChunk();
	}		
	m_pIB->unlock();
	}

	//写 'MSUB'
	if( m_vRenderPasses.size() > 0)
	{
	writeChunk.beginChunk('MSUB',&pDataStream);
	for( uint i = 0; i < m_vRenderPasses.size(); i++)
	{
	MmSub ms;
	ms.istart = m_vRenderPasses[i].indexStart;
	ms.icount = m_vRenderPasses[i].indexCount;
	ms.vstart = m_vRenderPasses[i].vertexStart;
	ms.vcount = m_vRenderPasses[i].vertexEnd - m_vRenderPasses[i].vertexStart;
	ms.materialIndex = m_vRecordMaterialIndex[i];
	pDataStream->write( &ms, sizeof(ms));
	}
	writeChunk.endChunk();
	}

	//写 'MBON'
	if( m_vBones.size() > 0)
	{
	writeChunk.beginChunk('MBON', &pDataStream);
	uint numbones = m_vBones.size();
	pDataStream->write(&numbones, sizeof(numbones) );
	for( uint i = 0; i < m_vBones.size(); i++)
	{
	MmBone mb;
	mb.parent = m_vBones[i]->parentId;
	mb.billboard = m_vBones[i]->billboarded;
	mb.pivot = m_vBones[i]->pivotPoint;

	pDataStream->write(&mb.parent, sizeof( mb.parent));
	pDataStream->write(&mb.billboard, sizeof( mb.billboard));
	pDataStream->write(&mb.pivot, sizeof( mb.pivot));
	writeKeyFrames(pDataStream,m_vBones[i]->translation);
	writeKeyFrames(pDataStream,m_vBones[i]->rotation);
	writeKeyFrames(pDataStream,m_vBones[i]->scale);
	}
	writeChunk.endChunk();
	}

	//写 'MANM'
	if( m_vAnimations.size() > 0)
	{
	writeChunk.beginChunk('MANM', &pDataStream);
	for( uint i = 0 ;i < m_vAnimations.size(); i++)
	{
	MmAnimation ma;
	ma.timeStart = m_vAnimations[i]->getTimeStart();
	ma.timeEnd = m_vAnimations[i]->getTimeEnd();
	ma.loop = m_vAnimations[i]->isLoop();
	string name = m_vAnimations[i]->getName();
	for( uint ii = 0; ii < name.size(); ii++)
	{
	ma.name[ii] = name.c_str()[ii];
	}
	ma.name[name.size()] = '\0';
	pDataStream->write(&ma, sizeof(ma) );
	}
	writeChunk.endChunk();
	}

	//写 'MPAR'
	if(m_vMmParticleSystems.size() > 0)
	{
	writeChunk.beginChunk('MPAR',&pDataStream);
	uint numMmParticleSystems = m_vMmParticleSystems.size();
	pDataStream->write( &numMmParticleSystems, sizeof(numMmParticleSystems) );
	for( uint i = 0; i < m_vMmParticleSystems.size(); i++)
	{
	MmParticleSystemData * pdata = getMmParticleSystem(i);
	writeKeyFrames(pDataStream, pdata->speed);
	writeKeyFrames(pDataStream, pdata->variation);
	writeKeyFrames(pDataStream, pdata->spread);
	writeKeyFrames(pDataStream, pdata->lat);
	writeKeyFrames(pDataStream, pdata->gravity);
	writeKeyFrames(pDataStream, pdata->lifespan);
	writeKeyFrames(pDataStream, pdata->rate);
	writeKeyFrames(pDataStream, pdata->areal);
	writeKeyFrames(pDataStream, pdata->areaw);
	writeKeyFrames(pDataStream, pdata->grav2);

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
	CPath parpath= pdata->texture;
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
	}
	writeChunk.endChunk();
	}

	writeChunk.save(filename.c_str() ) ; 
	return true;

	}

	*/



	//对硬件蒙皮的支持
	IVertexBuffer *	ModelCodecMm::getBlendWeightBuffer()
	{
		return m_pBlendWeightBuffer;
	}

	IVertexBuffer * ModelCodecMm::getBlendIndicesBuffer(uint subModelIndex)
	{
		if( subModelIndex >= m_vBlendIndices.size() ) return 0;
		else return m_vBlendIndices[subModelIndex];
	}

	IVertexBuffer * ModelCodecMm::getOriginVertexBuffer() 
	{
		return m_pVBPosNormal;
	}

	IShaderProgram * ModelCodecMm::getBlendShaderProgram()
	{
		return m_pBlendProgram;
	}

	void ModelCodecMm::createCoreSkeleton()
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
			pCoreBone->setCanBufferData(true);//mm不会出现在游戏里面，这样设置方便查看游戏是否用了mdx
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

	void ModelCodecMm::initForGPUVertexBlend(MmFace* pFaceBuffer)
	{
		if( 0 == m_pBlendProgram ) return;
		if( 0 == pFaceBuffer)
		{
			safeRelease(m_pBlendProgram);
			return;
		}

		//生成默认的位置法向量buffer
		m_pVBPosNormal = m_pRenderSystem->getBufferManager()->createVertexBuffer(12,2 * m_numVertices, BU_STATIC_WRITE_ONLY);
		if( 0 == m_pVBPosNormal)
		{
			safeRelease(m_pBlendProgram);
			return;
		}
		Vector3 *pVertex = (Vector3 *)m_pVBPosNormal->lock(0,0,BL_NORMAL);
		if( 0 == pVertex)
		{
			safeRelease(m_pVBPosNormal);
			safeRelease(m_pBlendProgram);
			return;
		}
		for( uint i =0; i< m_numVertices; ++i)
		{
			pVertex[i] = m_pOriginVertices[i].pos;
		}
		for( uint i= 0; i< m_numVertices; ++i)
		{
			pVertex[m_numVertices + i] = m_pOriginVertices[i].normal;
		}
		m_pVBPosNormal->unlock();	

		//生成顶点混合的权重buffer
		m_pBlendWeightBuffer = m_pRenderSystem->getBufferManager()->createVertexBuffer(12,m_numVertices,BU_STATIC_WRITE_ONLY);
		if( 0 == m_pBlendWeightBuffer)
		{
			safeRelease(m_pBlendProgram);
			safeRelease(m_pVBPosNormal);
			return;
		}
		float * pWeight = (float*)m_pBlendWeightBuffer->lock(0,0,BL_NORMAL);
		if( 0 == pWeight)
		{
			safeRelease(m_pBlendProgram);
			safeRelease(m_pVBPosNormal);
			safeRelease(m_pBlendWeightBuffer);
			return;
		}
		for( uint i =0; i<m_numVertices; ++i)
		{
			*pWeight++ = m_pOriginVertices[i].weights[0]/static_cast<float>(255);
			*pWeight++ = m_pOriginVertices[i].weights[1]/static_cast<float>(255);
			*pWeight++ = m_pOriginVertices[i].weights[2]/static_cast<float>(255);
		}
		m_pBlendWeightBuffer->unlock();


		if( m_vBones.size() < VERTEX_BLEND_MATRICES_NUM )//不用拆分
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
							uint bonesindex = m_pOriginVertices[vertexindex].bones[k];
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
			for(size_t i = 0;i < size;i++)
			{
				SubModel& subModel = m_vRenderPasses[i];
				if(-1 != subModel.m_matIndex)
					subModel.setMaterial(m_vMaterials[subModel.m_matIndex]);
			}

		}


		//生成每个子mesh对应的骨骼索引buffer
		for( uint i =0; i< m_vRenderPasses.size(); ++i)
		{
			xs::SubModel & sub = m_vRenderPasses[i];
			IVertexBuffer* pBlendIndices = m_pRenderSystem->getBufferManager()->createVertexBuffer(4,m_numVertices,BU_STATIC_WRITE_ONLY);
			if( 0 == pBlendIndices)
			{
				safeRelease(m_pBlendProgram);
				safeRelease(m_pVBPosNormal);
				safeRelease(m_pBlendWeightBuffer);
				STLReleaseSequence(m_vBlendIndices);
				return;	
			}
			uint * pIndices = (uint*)pBlendIndices->lock(0,0,BL_NORMAL);
			if( 0 == pIndices)
			{
				safeRelease(m_pBlendProgram);
				safeRelease(m_pVBPosNormal);
				safeRelease(m_pBlendWeightBuffer);
				STLReleaseSequence(m_vBlendIndices);
				return;
			}
			for( uint i=0; i <m_numVertices; ++i)
			{
				*pIndices = 0;
				*pIndices |= sub.m_BonesMappings[ m_pOriginVertices[i].bones[0] ];
				*pIndices |= sub.m_BonesMappings[ m_pOriginVertices[i].bones[1] ] << 8;
				*pIndices |= sub.m_BonesMappings[ m_pOriginVertices[i].bones[2] ] << 16;
				*pIndices |= sub.m_BonesMappings[ m_pOriginVertices[i].bones[3] ] << 24;
				++pIndices;
			}
			pBlendIndices->unlock();
			m_vBlendIndices.push_back( pBlendIndices);
		}
	}

	bool ModelCodecMm::decode(xs::Stream *pStream)
	{	
		PP_BY_NAME("ModelCodecMm::decode");

		DataChunk chunk;
		xs::DataChunk::stChunk *pChunk = chunk.beginChunk(pStream);

		//构建着色器程序
		MmFace* pFaceBuffer = 0;
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
					stream.read(&m_ver,sizeof(m_ver));
				}
				break;
			case 'MGSQ':
				{
					uint numGlobalSequences = stream.getLength() / sizeof(int);
					for(uint i = 0;i < numGlobalSequences;i++)
					{
						uint data;
						stream.read(&data,sizeof(data));
						m_vGlobalSequences.push_back(data);
					}
				}
				break;
			case 'MANI':
				{
					stream.read(&m_bAnimatedGeometry,sizeof(m_bAnimatedGeometry));
					stream.read(&m_bAnimatedBones,sizeof(m_bAnimatedBones));
					stream.read(&m_bNeedPerInstanceAnimation,sizeof(m_bNeedPerInstanceAnimation));
				}
				break;
			case 'MBVH':
				{
					uint numVertices;
					uint numNormals;
					uint numIndices;
					stream.read(&numVertices,sizeof(numVertices));
					stream.read(&numNormals,sizeof(numNormals));
					stream.read(&numIndices,sizeof(numIndices));
				}
				break;
			case 'MMTX':
				{
					uint numTextures;
					stream.read(&numTextures,sizeof(numTextures));
					for(uint i = 0;i < numTextures;i++)
					{
						MmTexture mt;
						stream.read(&mt.replacable,sizeof(mt.replacable));
						stream.read(&mt.swrap,sizeof(mt.swrap));
						stream.read(&mt.twrap,sizeof(mt.twrap));
						uchar filenameLen;
						stream.read(&filenameLen,sizeof(filenameLen));
						char str[256];
						stream.read(str,filenameLen);
						str[filenameLen] = 0;

						char *fileName = strrchr(str,'\\');
						xs::CPath fn = str;
						if(fileName)
						{
							fileName++;
							fn = fileName;
						}
						CPath path = m_strName;
						CPath pathP = path.getParentDir();
						if(!pathP.empty())
						{
							pathP.addTailSlash();
							pathP += fn;
						}
						else
						{
							pathP = fn;
						}
						mt.filename = pathP;

						m_vTextures.push_back(mt);
					}
				}
				break;
			case 'MMTL':
				{
					uint numMaterials = stream.getLength() / sizeof(MmMaterial);
					for(uint i = 0;i < numMaterials;i++)
					{
						MmMaterial mm;
						stream.read(&mm,sizeof(mm));

						Material mtl;
						MaterialLayer layer;

						m_vRecordMaterialstextureIndex.push_back(mm.textureIndex);//用来记录textureIndex

						if(mm.textureIndex != -1)
						{
							layer.m_szTexture = m_vTextures[mm.textureIndex].filename;
							layer.m_bClampS = (m_vTextures[mm.textureIndex].swrap == TAM_CLAMP_TO_EDGE);
							layer.m_bClampT = (m_vTextures[mm.textureIndex].twrap == TAM_CLAMP_TO_EDGE);
						}
						layer.m_blendMode = mm.blendMode;
						layer.m_bTwoSide = mm.twoSide;
						layer.m_SphereEnvironmentMap = mm.envmap;
						layer.m_bUnshaded = mm.unlit;
						layer.m_bNoDepthSet = !mm.zwrite;
						mtl.addLayer(layer);
						m_vMaterials.push_back(mtl);
					}
				}
				break;
			case 'MVTX':
				{
					m_numVertices = stream.getLength() / sizeof(MmVertex);
					MmVertex *pmv = (MmVertex *)stream.getBuffer();

					m_pOriginVertices = new AnimatedVertex[m_numVertices];
					for(uint i = 0;i < m_numVertices;i++)
					{
						memcpy(&m_pOriginVertices[i],&pmv[i],sizeof(AnimatedVertex));
					}

					calcAABB(&pmv->pos,sizeof(MmVertex),m_numVertices);

					IBufferManager *pBufferManager = m_pRenderSystem->getBufferManager();

					/*
					//m_pVBPosNormal = pBufferManager->createVertexBuffer(12,2 * m_numVertices, BU_STATIC_WRITE_ONLY);
					//if(m_pVBPosNormal)
					//{
					//	Vector3 *pVertex = (Vector3 *)m_pVBPosNormal->lock(0,0,BL_NORMAL);
					//	if(pVertex)
					//	{
					//		Vector3 *p = &pmv->pos;
					//		for(uint i = 0;i < m_numVertices;i++)
					//		{
					//			pVertex[i] = *p;
					//			p = (Vector3*)((uchar*)p + sizeof(MmVertex));
					//		}
					//		p = &pmv->normal;
					//		for(uint i = 0;i < m_numVertices;i++)
					//		{
					//			pVertex[m_numVertices + i] = *p;
					//			p = (Vector3*)((uchar*)p + sizeof(MmVertex));
					//		}
					//		m_pVBPosNormal->unlock();
					//	}
					//}
					*/
					

					m_pVBTexcoord = pBufferManager->createVertexBuffer(8,m_numVertices,BU_STATIC_WRITE_ONLY);
					if(m_pVBTexcoord)
					{
						Vector2 *pTexcoord = (Vector2 *)m_pVBTexcoord->lock(0,0,BL_NORMAL);
						if(pTexcoord)
						{
							Vector2 *p = &pmv->texcoords;
							for(uint i = 0;i < m_numVertices;i++)
							{
								pTexcoord[i] = *p;
								p = (Vector2*)((uchar*)p + sizeof(MmVertex));
							}
							m_pVBTexcoord->unlock();
						}
					}
				}
				break;
			case 'MFAC':
				{
					m_numFaces = stream.getLength() / sizeof(MmFace);
					MmFace *pmf = (MmFace*)stream.getBuffer();

					if(m_numFaces)
					{
						uint numIndices = 3 * m_numFaces;
						m_pIB = m_pRenderSystem->getBufferManager()->createIndexBuffer(IT_16BIT,numIndices,BU_STATIC_WRITE_ONLY);
						if(m_pIB)
						{
							ushort *pIndicesBuffer = (ushort*)m_pIB->lock(0,0,BL_NORMAL);
							if(pIndicesBuffer)
							{
								memcpy(pIndicesBuffer,&pmf->index[0],m_numFaces * sizeof(MmFace));
								m_pIB->unlock();
							}
						}

						if(m_pBlendProgram )
						{
							pFaceBuffer = new MmFace[m_numFaces];
							for( uint i=0; i < m_numFaces; ++i)
								pFaceBuffer[i] = pmf[i];
						}
					}
				}
				break;
			case 'MSUB':
				{
					uint numSubs = stream.getLength() / sizeof(MmSub);
					for(uint i = 0;i < numSubs;i++)
					{
						MmSub ms;
						stream.read(&ms,sizeof(ms));
						SubModel pass;
						
						m_vRecordMaterialIndex.push_back(ms.materialIndex);//记录materialIndex
						if(ms.materialIndex != -1)
						{
							pass.m_matIndex = ms.materialIndex;
							pass.setMaterial(m_vMaterials[ms.materialIndex]);
						}
						else
						{
							pass.m_matIndex = -1;
						}

						pass.indexStart = ms.istart;
						pass.indexCount = ms.icount;
						pass.vertexStart = ms.vstart;
						pass.vertexEnd = pass.vertexStart + ms.vcount;

						char str[256];
						sprintf(str,"mtl%d",i);
						pass.m_name = str;

						m_vRenderPasses.push_back(pass);
					}
				}
				break;
			case 'MBON':
				{
					uint numBones;
					stream.read(&numBones,sizeof(numBones));
					for(uint i = 0;i < numBones;i++)
					{
						BoneData *pData = new BoneData;
						m_vBones.push_back(pData);

						MmBone mb;
						stream.read(&mb.parent,sizeof(mb.parent));
						stream.read(&mb.billboard,sizeof(mb.billboard));
						stream.read(&mb.pivot,sizeof(mb.pivot));

						pData->objectId = i;
						pData->parentId = mb.parent;
						pData->billboarded = mb.billboard;
						pData->pivotPoint = mb.pivot;
						pData->pivotRotation = true;

						readKeyFrames(&stream,pData->translation);
						readKeyFrames(&stream,pData->rotation);
						readKeyFrames(&stream,pData->scale);

						sprintf(pData->name,"bone%d",i);

						static Matrix4 precomputeMtx = 
							Matrix4(
							0, 0,-75, 0,
							0, 75, 0, 0,
							75, 0, 0, 0,
							0, 0, 0, 1);

						if(mb.parent == -1)
						{
							pData->precomputeMtx = precomputeMtx;
						}
					}
				}
				break;
			case 'MANM':
				{
					uint numAnimations = stream.getLength() / sizeof(MmAnimation);
					for(uint i = 0;i < numAnimations;i++)
					{
						MmAnimation ma;
						stream.read(&ma,sizeof(ma));
						Animation* pAnimation = new Animation(ma.timeStart,ma.timeEnd,ma.loop,ma.name);
						//modified by xxh 20091011， 下面那样写不好，会使得m_vAnimations和m_AnimationMap不一致，改为这样。
						if( pAnimation )
						{
							m_vAnimations.push_back( pAnimation);
							if( pAnimation && m_AniamtionMap.find( pAnimation->getName() ) == m_AniamtionMap.end() )
								m_AniamtionMap[pAnimation->getName()] = pAnimation;
						}
						//m_vAnimations.push_back(pAnimation);
						//if(m_AniamtionMap.find(ma.name) == m_AniamtionMap.end())
						//	m_AniamtionMap[ma.name] = pAnimation;
					}
				}
				break;
			case 'MPAR':
				{
					uint numMmParticleSystems;
					stream.read(&numMmParticleSystems,sizeof(numMmParticleSystems));
					for(uint i = 0;i < numMmParticleSystems;i++)
					{
						MmParticleSystemData data;
						readKeyFrames(&stream,data.speed);
						readKeyFrames(&stream,data.variation);
						readKeyFrames(&stream,data.spread);
						readKeyFrames(&stream,data.lat);
						readKeyFrames(&stream,data.gravity);
						readKeyFrames(&stream,data.lifespan);
						readKeyFrames(&stream,data.rate);
						readKeyFrames(&stream,data.areal);
						readKeyFrames(&stream,data.areaw);
						readKeyFrames(&stream,data.grav2);
						stream.read(&data.p.mid,sizeof(data.p.mid));
						uint colors[3];
						stream.read(&colors[0],sizeof(colors));
						data.p.colors[0].setAsARGB(colors[0]);
						data.p.colors[1].setAsARGB(colors[1]);
						data.p.colors[2].setAsARGB(colors[2]);
						stream.read(&data.p.sizes[0],sizeof(data.p.sizes));
						stream.read(&data.p.d[0],sizeof(data.p.d));
						stream.read(&data.p.unk[0],sizeof(data.p.unk));
						stream.read(&data.p.scales[0],sizeof(data.p.scales));
						stream.read(&data.p.slowdown,sizeof(data.p.slowdown));
						stream.read(&data.p.rotation,sizeof(data.p.rotation));
						stream.read(&data.p.f2[0],sizeof(data.p.f2));
						stream.read(&data.pos,sizeof(data.pos));
						uchar len;
						stream.read(&len,sizeof(len));
						char texture[256];
						stream.read(texture,len);
						texture[len] = 0;

						//
						char *fileName = strrchr(texture,'\\');
						xs::CPath fn = texture;
						if(fileName)
						{
							fileName++;
							fn = fileName;
						}
						CPath path = m_strName;
						CPath pathP = path.getParentDir();
						if(!pathP.empty())
						{
							pathP.addTailSlash();
							pathP += fn;
						}
						else
						{
							pathP = fn;
						}
						data.texture = pathP;
						//

#define READ(a) stream.read(&data.a,sizeof(data.a));
						READ(blend);
						READ(rows);
						READ(cols);
						READ(emitterType);
						READ(type);
						data.order = (data.type > 0 ? -1 : 0);
						READ(s2);
						READ(bone);
						int flags;
						stream.read(&flags,sizeof(flags));
						data.billboardType = ((!(flags & 4096)) ? BILLBOARD_LOCK_NONE : BILLBOARD_NOT_USED);


						for(short j = 0;j < data.rows * data.cols;j++)
						{
							MmTexCoordSet tc;
							initTile(tc.tc,j,data.cols,data.rows,data.order);
							data.vTexcoords.push_back(tc);
						}
						m_vMmParticleSystems.push_back(data);
					}
				}
				break;
			}
			pChunk = chunk.nextChunk(pStream);
		}

		//初始化硬件顶点的混合
		PP_BY_NAME_START("ModelCodecMm::decode::initForGPUVertexBlend");
		initForGPUVertexBlend(pFaceBuffer);
		PP_BY_NAME_STOP();
		safeDeleteArray( pFaceBuffer);

		//创建骨架
		PP_BY_NAME_START("");
		createCoreSkeleton();
		PP_BY_NAME_STOP();

		m_memorySize += sizeof(*this);
		m_memorySize += m_skeleton.getMemeorySize();
		m_memorySize += m_vAnimations.size() * sizeof(Animation);
		m_memorySize += m_vGlobalSequences.size() * sizeof(uint);
		m_memorySize += m_vTextures.size() * sizeof(MmTexture);
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

		//忘记计算原始顶点数组的大小了
		m_memorySize += m_numVertices * sizeof( AnimatedVertex);

		return true;
	}


	ModelCodecMm::~ModelCodecMm()
	{
		for(int i = 0;i < MM_PRECOMPUTED_VBO_COUNT_MAX /*30720*/;i++)
		{
			safeRelease(m_pPrecomputedVBs[i]);
		}
		if((ulong)m_pOriginVertices == 0xbaadf00d)
		{
			m_pOriginVertices = m_pOriginVertices;
		}
		safeDelete(m_pOriginVertices);
		STLDeleteSequence(m_vBones);
		STLDeleteSequence(m_vAnimations);
		safeRelease(m_pVBPosNormal);
		safeRelease(m_pBlendProgram);
		safeRelease(m_pBlendWeightBuffer);
		STLReleaseSequence( m_vBlendIndices);
	}

	ModelCodecMm::ModelCodecMm(const std::string& name,IRenderSystem* pRenderSystem)
		: ManagedItem(name.c_str()),
		m_pRenderSystem(pRenderSystem),
		m_pVBTexcoord(0),
		m_pIB(0),
		m_bAnimatedGeometry(0),
		m_bAnimatedBones(0),m_bNeedPerInstanceAnimation(0),
		m_numVertices(0),
		m_numFaces(0),
		m_memorySize(0),
		m_pOriginVertices(0),
		m_pVBPosNormal(0),
		m_pBlendWeightBuffer(0),
		m_pBlendProgram(0)
	{
		memset(m_pPrecomputedVBs,0,/*30720*/ MM_PRECOMPUTED_VBO_COUNT_MAX * sizeof(IVertexBuffer*));
	}

	void	ModelCodecMm::calcAABB(Vector3* pVertices,uint ui32Stride,uint numVertices)
	{
		if(!m_numVertices)return;

		Vector3 vMin = pVertices[0];
		Vector3 vMax = pVertices[0];

		Vector3 *p = pVertices;
		for(uint i = 1;i < numVertices;i++)
		{
			p = (Vector3*)((uchar*)p + ui32Stride);

			if(p->x > vMax.x)vMax.x = p->x;
			if(p->y > vMax.y)vMax.y = p->y;
			if(p->z > vMax.z)vMax.z = p->z;
			if(p->x < vMin.x)vMin.x = p->x;
			if(p->y < vMin.y)vMin.y = p->y;
			if(p->z < vMin.z)vMin.z = p->z;
		}
		m_aabb.setExtents(vMin,vMax);
		m_sphere.setCenter(vMin.midPoint(vMax));
		m_sphere.setRadius((vMax - vMin).length() * 0.5f);
	}

	size_t ModelCodecMm::getMomorySize()
	{
		return m_memorySize;
	}

	void ModelCodecMm::release()
	{
		delete this;
	}

	/*

	void * ModelCodecMm::aidGetMMPSParamVector( void * pParam, string paramtype, string & type, int paramcount ,int & count)
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
	void * ModelCodecMm::aidGetMMPSParamsKeyFrames(MmParticleSystemData & mmpsdata, string paraname,string & type, int & count)
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

	void * ModelCodecMm::getMMParticleSystemParam(unsigned int index,string paramname, string & type, int & count )
	{
		//通用检测
		if( index >= m_vMmParticleSystems.size() ) return 0;

		MmParticleSystemData & mmpsdata = m_vMmParticleSystems.at(index);
		
		//基本参数
		if(paramname == "texture") return aidGetMMPSParamVector( &mmpsdata.texture, "string",type,1,count);
		if( paramname == "bone") return aidGetMMPSParamVector(&mmpsdata.bone, "short",type,1,count);
		if( paramname == "billboardType")
		{
			short * preturn = new short[1];
			*preturn = static_cast<short>(mmpsdata.billboardType);
			type = "short";
			count =1;
			return preturn;
		}
		if( paramname == "pos") return aidGetMMPSParamVector(mmpsdata.pos.val, "float",type,3,count);
		if( paramname == "blend") return aidGetMMPSParamVector(&mmpsdata.blend, "short",type,1,count);
		if( paramname == "emitterType") return aidGetMMPSParamVector(&mmpsdata.emitterType, "short",type,1,count);
		if( paramname == "type") return aidGetMMPSParamVector(&mmpsdata.type, "short",type,1,count);
		if( paramname == "cols") return aidGetMMPSParamVector(&mmpsdata.cols, "short",type,1,count);
		if( paramname == "rows") return aidGetMMPSParamVector(&mmpsdata.rows, "short",type,1,count);

		//附加参数
		if( paramname == "mid") return aidGetMMPSParamVector(&mmpsdata.p.mid, "float",type,1,count);
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
		if( paramname == "sizes") return aidGetMMPSParamVector(mmpsdata.p.sizes, "float",type,3,count);
		if( paramname == "d") return aidGetMMPSParamVector(mmpsdata.p.d, "short",type,10,count);
		if( paramname == "unk") return aidGetMMPSParamVector(mmpsdata.p.unk, "float",type,3,count);
		if( paramname == "scales") return aidGetMMPSParamVector(mmpsdata.p.scales, "float",type,3,count);
		if( paramname == "slowdown") return aidGetMMPSParamVector(&mmpsdata.p.slowdown, "float",type,1,count);
		if( paramname == "rotation") return aidGetMMPSParamVector(&mmpsdata.p.rotation, "float",type,1,count);
		if( paramname == "f2") return aidGetMMPSParamVector(mmpsdata.p.f2, "float",type,16,count);

		//帧数据处理
		if( paramname == "speed") return aidGetMMPSParamsKeyFrames(mmpsdata, paramname, type, count);
		if( paramname == "variation") return aidGetMMPSParamsKeyFrames(mmpsdata, paramname, type, count);
		if( paramname == "spread") return aidGetMMPSParamsKeyFrames(mmpsdata, paramname, type, count);
		if( paramname == "lat") return aidGetMMPSParamsKeyFrames(mmpsdata, paramname, type, count);
		if( paramname == "gravity") return aidGetMMPSParamsKeyFrames(mmpsdata, paramname, type, count);
		if( paramname == "lifespan") return aidGetMMPSParamsKeyFrames(mmpsdata, paramname, type, count);
		if( paramname == "rate") return aidGetMMPSParamsKeyFrames(mmpsdata, paramname, type, count);
		if( paramname == "areal") return aidGetMMPSParamsKeyFrames(mmpsdata, paramname, type, count);
		if( paramname == "areaw") return aidGetMMPSParamsKeyFrames(mmpsdata, paramname, type, count);
		if( paramname == "grav2") return aidGetMMPSParamsKeyFrames(mmpsdata, paramname, type, count);

		//默认处理
		return 0;
	}

*/

/*
	bool ModelCodecMm::aidSetMMPSParamVector(void * psrc, int srcnum, string srctype, void* pdes, int desnum, string destype, bool enablerange, float min, float max )
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
	bool ModelCodecMm::setMMPSKeyFramesParam(KeyFrames<float> * pkf, float * pParamVal, int iParamNum, string szParamType)
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

/*

	bool ModelCodecMm::setMMParticleSystemParam(unsigned int index, string desname, void * psrc, int srcnum, string srctype)
	{
		//通用检测
		if( index >= m_vMmParticleSystems.size() ) return false;
		if( psrc == 0) return false;
		if( srcnum <= 0 ) return false;

		MmParticleSystemData & mmpsdata = m_vMmParticleSystems.at(index);

		//基本参数
		if(desname == "texture") return aidSetMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.texture, 1,"string");
		size_t bonenum = m_vBones.size();
		if(desname == "bone") return aidSetMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.bone, 1,"short",true,-1.0000001,bonenum-1+0.0000001);
		if( desname =="billboardType")
		{
			if( srcnum != 1 || srctype != "short") return false;
			short * pshort = (short *)psrc;
			if( pshort[0] < 0 || pshort[0] > 4 ) return false;
			mmpsdata.billboardType = static_cast<BillboardType>(pshort[0]);
			return true;
		}
		if(desname == "pos") return aidSetMMPSParamVector(psrc,srcnum,srctype,mmpsdata.pos.val, 3,"float");
		if(desname == "blend") return aidSetMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.blend, 1,"short",true,0-0.0000001,5+0.0000001);
		if(desname == "emitterType") return aidSetMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.emitterType, 1,"short",true,1-0.0000001,2+0.0000001);
		if(desname == "type") //修改order
		{
			bool result = aidSetMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.type, 1,"short",true,0-0.0000001,2+0.0000001);
			if( result == true )
			{
				mmpsdata.order = (mmpsdata.type > 0 ? -1 : 0);
			}
			return result;
		}
		if(desname == "cols") 
		{
			bool result =  aidSetMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.cols, 1,"short", true, 1-0.0000001,10000+0.0000001);
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
			bool result = aidSetMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.rows, 1,"short",true, 1-0.0000001,10000+0.0000001);
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
		if( desname == "mid") return aidSetMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.p.mid, 1,"float",true,0-0.0000001,1+0.0000001);
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
		if( desname == "sizes") return aidSetMMPSParamVector(psrc,srcnum,srctype,mmpsdata.p.sizes, 3,"float");
		if( desname == "d") return aidSetMMPSParamVector(psrc,srcnum,srctype,mmpsdata.p.d, 10,"short");
		if( desname == "unk") return aidSetMMPSParamVector(psrc,srcnum,srctype,mmpsdata.p.unk, 3,"float");
		if( desname == "scales") return aidSetMMPSParamVector(psrc,srcnum,srctype,mmpsdata.p.scales, 3,"float");
		if( desname == "slowdown") return aidSetMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.p.slowdown, 1,"float");
		if( desname == "rotation") return aidSetMMPSParamVector(psrc,srcnum,srctype,&mmpsdata.p.rotation, 1,"float");
		if( desname == "f2") return aidSetMMPSParamVector(psrc,srcnum,srctype,mmpsdata.p.f2, 16,"float");

		//处理帧数据
		map< string , KeyFrames<float> * >  mapKF;
		mapKF.insert(pair<string, KeyFrames<float> *> ( string("speed"), & mmpsdata.speed) );
		mapKF.insert(pair<string, KeyFrames<float> *> ( string("variation"), & mmpsdata.variation) );
		mapKF.insert(pair<string, KeyFrames<float> *> ( string("spread"), & mmpsdata.spread) );
		mapKF.insert(pair<string, KeyFrames<float> *> ( string("lat"), & mmpsdata.lat) );
		mapKF.insert(pair<string, KeyFrames<float> *> ( string("gravity"), & mmpsdata.gravity) );
		mapKF.insert(pair<string, KeyFrames<float> *> ( string("lifespan"), & mmpsdata.lifespan) );
		mapKF.insert(pair<string, KeyFrames<float> *> ( string("rate"), & mmpsdata.rate) );
		mapKF.insert(pair<string, KeyFrames<float> *> ( string("areal"), & mmpsdata.areal) );
		mapKF.insert(pair<string, KeyFrames<float> *> ( string("areaw"), & mmpsdata.areaw) );
		mapKF.insert(pair<string, KeyFrames<float> *> ( string("grav2"), & mmpsdata.grav2) );
		map<string, KeyFrames<float> * >::iterator itKF = mapKF.find(desname);
		if (  itKF != mapKF.end() )
		{
			return setMMPSKeyFramesParam( itKF->second, static_cast<float*>(psrc), srcnum,srctype);
		}	

		//默认处理
		return false;
	}

	*/

	/*
	bool ModelCodecMm::addMMParticleSystem()
	{
		MmParticleSystemData mmpsdata;

		mmpsdata.id = static_cast<int>( Math::UnitRandom() * INT_MAX );
		mmpsdata.billboardType = BILLBOARD_LOCK_NONE;
		mmpsdata.pos = Vector3(0.0, 0.0, 0.0);
		mmpsdata.bone = -1;
		mmpsdata.texture = "";
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
		mmpsdata.p.colors[0] = Color4(1,0,0,0);
		mmpsdata.p.colors[1] = Color4(0,1,0,0.5);
		mmpsdata.p.colors[2] = Color4(0,0,1,0);
		int i = 0;
		for( i = 0; i< 3; i++)
			mmpsdata.p.sizes[i] =  i * 0.5;
		for( i = 0; i < 10; i++)
			mmpsdata.p.d[i]= 0;
		for( i = 0; i < 3; i++ )
			mmpsdata.p.unk [i] = 0;
		for( i = 0; i < 3; i++)
			mmpsdata.p.scales[i] = 0 ;
		mmpsdata.p.slowdown = 1;
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

		m_vMmParticleSystems.push_back(mmpsdata);
		return true;
	}

	*/
	/*
	bool ModelCodecMm::removeMMParticleSystem(unsigned int index)
	{
		if( index >= m_vMmParticleSystems.size() )  return false;

		std::vector<MmParticleSystemData>::iterator it = m_vMmParticleSystems.begin();
		unsigned int count = 0;
		for( ; it != m_vMmParticleSystems.end(); ++it, ++count)
		{
			if( count == index ) 
			{
				m_vMmParticleSystems.erase(it);
				return true;
			}
		}
		return false;
	}
	*/
}