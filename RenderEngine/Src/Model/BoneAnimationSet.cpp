#include "StdAfx.h"
#include "BoneAnimationSet.h"
#include "MzHeader.h"

namespace xs
{
	void BoneAnimationSet::release()
	{
		delete this;
	}

	BoneAnimationSet::~BoneAnimationSet()
	{
		STLDeleteSequence(m_vBones);
		STLDeleteSequence(m_vAnimations);
		m_AnimationMap.clear();
	}

	size_t BoneAnimationSet::getMemorySize()
	{
		return m_memorySize;
	}

	const std::string& BoneAnimationSet::getFileName()
	{
		return m_strName;
	}

	BoneAnimationSet::BoneAnimationSet(std::string strFileName)
		:ManagedItem(strFileName.c_str()),
		m_version(0)
	{

	}

	bool BoneAnimationSet::decode(xs::Stream * data)
	{
		if( !data) return false;

		xs::DataChunk chunk;
		xs::DataChunk::stChunk * pChunk = chunk.beginChunk(data);

		while( pChunk)
		{
			if( pChunk->m_ui32DataSize == 0 )
			{
				pChunk = chunk.nextChunk(data);
				continue;
			}
			xs::MemoryStream stream( (uchar*)pChunk->m_pData, pChunk->m_ui32ChunkSize);
			switch( pChunk->m_ui32Type)
			{
			case 'MVER':
				{
					stream.read(&m_version, sizeof(m_version));
					if( m_version <= 0 ) return false;
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
						if( pAnimation)
						{
							m_vAnimations.push_back(pAnimation);
							if( pAnimation && m_AnimationMap.find(pAnimation->getName() ) == m_AnimationMap.end() )
								m_AnimationMap[pAnimation->getName() ] = pAnimation;
						}
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
						//pData->initQuat = Quaternion(_r[0],-_r[2],_r[1],_r[3]);//按3dmax9的方式，这种方式不适合本引擎
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

						pData->translation.setInterpolationType(INTERPOLATION_NONE);
						pData->rotation.setInterpolationType(INTERPOLATION_NONE);
						pData->scale.setInterpolationType(INTERPOLATION_NONE);
						//pData->translation.setInterpolationType(INTERPOLATION_LINEAR);
						//pData->rotation.setInterpolationType(INTERPOLATION_LINEAR);
						//pData->scale.setInterpolationType(INTERPOLATION_LINEAR);

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

							KeyFrame<Vector3> kfScale(kf.time,Vector3(kf.v[0],kf.v[1],kf.v[2]));

							pData->scale.addKeyFrame(kfScale);
						}
						m_vBones.push_back(pData);	
					}
				}
				break;
			}
			pChunk = chunk.nextChunk(data);
		}

		//创建骨架
		PP_BY_NAME_START("BoneAnimationSet::decode::createCoreSkeleton");
		createCoreSkeleton();
		PP_BY_NAME_STOP();

		m_memorySize += sizeof(*this);
		m_memorySize += m_skeleton.getMemeorySize();
		m_memorySize += m_vAnimations.size() * sizeof(Animation);
		size_t size = m_vBones.size();
		for( size_t i =0; i < size; ++i)
			m_memorySize += m_vBones[i]->getMemorySize();

		return true;
	}

	bool BoneAnimationSet::write(string filename,float fCopress)
	{
		//filename = m_strName+"1";
		Stream *pDataStream;
		DataChunk writeChunk;

		//最新版本是7，把所有旧的版本转换为版本4
		//写 'MVER'
		writeChunk.beginChunk('MVER', &pDataStream);
		uint ver = 4 ;//在导出插件里面计算aabb
		pDataStream->write(&ver,sizeof(ver));
		writeChunk.endChunk();

		//写 'MANM'
		uint32 nAnims = m_vAnimations.size();
		//add by yhc 2010.5.10
		//修正前一个动作的结束时间和后一个动作的开始时间一样的饿问题
		bool bFixWrongTime = false;
		if( nAnims > 0)
		{
			if( nAnims>1 && m_vAnimations[0]->getTimeEnd()==m_vAnimations[1]->getTimeStart() )
				bFixWrongTime = true;

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
				if(bFixWrongTime)
					startTime += i*33;

				endTime = m_vAnimations[i]->getTimeEnd();
				if(bFixWrongTime)
					endTime += i*33;
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

				KeyFrames<Vector3> translation;
				KeyFrames<Quaternion> rotation;
				KeyFrames<Vector3> scale;
				
				//去掉重复和多余的keyframe 
				CompressPos(m_vBones[i]->translation,translation,fCopress);
				uint nKeyFrames = translation.numKeyFrames();
				//修正时间
				if(bFixWrongTime)
				{
					int nCurAniIndex=0;
					for( uint ii = 0; ii < nKeyFrames; ii++)
					{
						KeyFrame<Vector3> * kf =  translation.getKeyFrame(ii);
						if( kf->time>m_vAnimations[nCurAniIndex]->getTimeEnd() )
							nCurAniIndex++;
						kf->time += nCurAniIndex*33;
					}

				}
				pDataStream->write( &nKeyFrames, sizeof(nKeyFrames) );
				for( uint ii = 0; ii < nKeyFrames; ii++)
				{
					ModelKeyframeTranslation mkft;
					mkft.time = translation.getKeyFrame(ii)->time;
					mkft.v[0] = translation.getKeyFrame(ii)->v[0];
					mkft.v[1] = translation.getKeyFrame(ii)->v[1];
					mkft.v[2] = translation.getKeyFrame(ii)->v[2];
					pDataStream->write( &mkft, sizeof(mkft) );
				} 
				
				CompressQua(m_vBones[i]->rotation,rotation,fCopress);
				nKeyFrames = rotation.numKeyFrames();
				//修正时间
				if(bFixWrongTime)
				{
					int nCurAniIndex=0;
					for( uint ii = 0; ii < nKeyFrames; ii++)
					{
						KeyFrame<Quaternion> * kf =  rotation.getKeyFrame(ii);
						if( kf->time>m_vAnimations[nCurAniIndex]->getTimeEnd() )
							nCurAniIndex++;
						kf->time += nCurAniIndex*33;
					}
				}

				pDataStream->write( &nKeyFrames, sizeof(nKeyFrames) );
				for( uint ii = 0; ii < nKeyFrames; ii++)
				{
					ModelKeyframeRotation mkfr;
					mkfr.time = rotation.getKeyFrame(ii)->time;
					mkfr.q[0] = rotation.getKeyFrame(ii)->v.x;
					mkfr.q[1] = rotation.getKeyFrame(ii)->v.y;
					mkfr.q[2] = rotation.getKeyFrame(ii)->v.z;
					mkfr.q[3] = rotation.getKeyFrame(ii)->v.w;
					pDataStream->write( &mkfr, sizeof(mkfr) );
				} 
				
				CompressPos(m_vBones[i]->scale,scale,fCopress);
				nKeyFrames = scale.numKeyFrames();
				//修正时间
				if(bFixWrongTime)
				{
					int nCurAniIndex=0;
					for( uint ii = 0; ii < nKeyFrames; ii++)
					{
						KeyFrame<Vector3> * kf =  scale.getKeyFrame(ii);
						if( kf->time>m_vAnimations[nCurAniIndex]->getTimeEnd() )
							nCurAniIndex++;
						kf->time += nCurAniIndex*33;
					}

				}
				pDataStream->write( &nKeyFrames, sizeof( nKeyFrames) );
				for( uint ii = 0; ii < nKeyFrames; ii ++)
				{
					ModelKeyframeScale mkfs;
					mkfs.time = scale.getKeyFrame(ii)->time;
					mkfs.v[0] = scale.getKeyFrame(ii)->v[0];
					mkfs.v[1] = scale.getKeyFrame(ii)->v[1];
					mkfs.v[2] = scale.getKeyFrame(ii)->v[2];
					pDataStream->write( &mkfs, sizeof(mkfs) );
				}
			
			}
			writeChunk.endChunk();		
		}

		writeChunk.save(filename.c_str());
		return true;
	}
	Animation * BoneAnimationSet::getAnimationByName(const std::string& animation)
	{
		HASH_MAP_NAMESPACE::hash_map<std::string,Animation*>::iterator it = m_AnimationMap.find(animation);
		if(it == m_AnimationMap.end())return 0;

		return it->second;
	}

	uint BoneAnimationSet::getAnimationCount()
	{
		return m_vAnimations.size();
	}

	Animation * BoneAnimationSet::getAnimationByIndex(uint index)
	{
		if(index >= m_vAnimations.size())return 0;
		return m_vAnimations[index];
	}

	//uint BoneAnimationSet::getBoneNum()
	//{
	//	return m_vBones.size();
	//}

	//BoneData * BoneAnimationSet::getBoneByIndex(uint index)
	//{
	//	if(index >= m_vBones.size())return 0;
	//	return m_vBones[index];
	//}

	CoreSkeleton* BoneAnimationSet::getCoreSkeletion()
	{
		return &m_skeleton;
	}

	void BoneAnimationSet::createCoreSkeleton()
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
			pCoreBone->setCanBufferData(true);//设置能够缓存数据，因为是不插值的
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
}