#include "StdAfx.h"
#include "ModelCodecWarcraft3.h"

namespace xs
{
	void getReplaceableTexture(int id,char* filename)
	{

		switch(id)
		{
		case 1:
			{
				strcpy(filename,"ReplaceableTextures\\TeamColor\\TeamColor00.blp");
			}
			break;
		case 2:
			{
				strcpy(filename,"ReplaceableTextures\\TeamGlow\\TeamGlow00.blp");
			}
			break;
		case 11:
			{
				strcpy(filename,"ReplaceableTextures\\Cliff\\Cliff0.blp");
			}
			break;
		case 31:
			{
				strcpy(filename,"ReplaceableTextures\\LordaeronTree\\LordaeronSummerTree.blp");
			}
			break;
		case 32:
			{
				strcpy(filename,"ReplaceableTextures\\AshenvaleTree\\AshenTree.blp");
			}
			break;
		case 33:
			{
				strcpy(filename,"ReplaceableTextures\\BarrensTree\\BarrensTree.blp");
			}
			break;
		case 34:
			{
				strcpy(filename,"ReplaceableTextures\\NorthrendTree\\NorthTree.blp");
			}
			break;
		case 35:
			{
				strcpy(filename,"ReplaceableTextures\\Mushroom\\MushroomTree.blp");
			}
			break;
		default:
			strcpy(filename,"ReplaceableTextures\\TeamColor\\TeamColor00.blp");
			break;
		}
	}

	template<class T>
	T W2M(T v)
	{
		return v;
	}
	template<class T>
	T W2M_s(T v)
	{
		return v;
	}

	template<>
	Vector3 W2M<Vector3>(Vector3 v)
	{
		return v;
		static Quaternion q = Quaternion(-0.5f,-0.5f,-0.5f,0.5f);
		return q * v;
	}

	template<>
	Vector3 W2M_s<Vector3>(Vector3 v)
	{
		return v;
		static Quaternion q = Quaternion(-0.5f,-0.5f,-0.5f,0.5f);
		return q * v;
	}

	template<>
	Quaternion W2M<Quaternion>(Quaternion v)
	{
		return v;
		Vector3 x1,x2,x3;
		v.ToAxes(x1,x2,x3);
		Vector3 y1,y2,y3;
		y1 = x3;
		y2 = x1;
		y3 = x2;
		Quaternion p;
		p.FromAxes(y1,y2,y3);
		return p;
	}

	template<class T> void	ModelCodecWarcraft3::readKeyFrames(Stream *pStream,KeyFrames<T>& keyFrames,bool bScale)
	{
		int numKeyFrames;
		pStream->read(&numKeyFrames,sizeof(numKeyFrames));

		InterpolationType interpolationType;
		pStream->read(&interpolationType,sizeof(interpolationType));
		keyFrames.setInterpolationType(interpolationType);

		int globalSequenceId;
		pStream->read(&globalSequenceId,sizeof(globalSequenceId));
		keyFrames.setGlobalSequenceId(globalSequenceId);

		for(int i = 0;i < numKeyFrames;i++)
		{
			uint time;
			pStream->read(&time,sizeof(time));
			T v;
			pStream->read(&v,sizeof(v));
			T in,out;

			switch(interpolationType)
			{
			case INTERPOLATION_HERMITE:
			case INTERPOLATION_BEZIER:
				{
					pStream->read(&in,sizeof(in));
					pStream->read(&out,sizeof(out));
					break;
				}
			}

			if(bScale)
			{
				v = W2M_s(v);
				in = W2M_s(in);
				out = W2M_s(out);
			}
			else
			{
				v = W2M(v);
				in = W2M(in);
				out = W2M(out);
			}
			KeyFrame<T> keyFrame(time,v,in,out);
			keyFrames.addKeyFrame(keyFrame);
		}
	}

	const std::string& ModelCodecWarcraft3::getFileName()
	{
		return m_strName;
	}

	const AABB&	ModelCodecWarcraft3::getAABB()
	{
		return m_aabb;
	}

	const Sphere& ModelCodecWarcraft3::getBoudingSphere()
	{
		return m_sphere;
	}

	uint	ModelCodecWarcraft3::getAnimationCount()
	{
		return m_vAnimations.size();
	}

	Animation* ModelCodecWarcraft3::getAnimation(uint index)
	{
		if(index >= m_vAnimations.size())return 0;
		return m_vAnimations[index];
	}

	Animation* ModelCodecWarcraft3::hasAnimation(const std::string& animation)
	{
		AnimationMap::iterator it = m_AnimationMap.find(animation);
		if(it == m_AnimationMap.end())return 0;

		return it->second;
	}


	CoreSkeleton * ModelCodecWarcraft3::getCoreSkeletion()
	{
		return &m_skeleton;
	}

	uint ModelCodecWarcraft3::getNumSubModels()
	{
		return m_vSubModels.size();
	}

	ISubModel*	ModelCodecWarcraft3::getSubModel(const char* name)
	{
		size_t size = m_vSubModels.size();
		for(uint i = 0;i < size;i++)
		{
			if(StringHelper::casecmp(m_vSubModels[i].m_name,name) == 0)
			{
				return &m_vSubModels[i];
			}
		}

		return 0;
	}

	ISubModel*	ModelCodecWarcraft3::getSubModel(int index)
	{
		if(index < 0 || index >= (int)m_vSubModels.size())return 0;

		return &m_vSubModels[index];
	}

	IIndexBuffer*	ModelCodecWarcraft3::getIndexBuffer()
	{
		return m_pIB;
	}

	IVertexBuffer*	ModelCodecWarcraft3::getTexcoordBuffer()
	{
		return m_pVBTexcoord;
	}

	uint ModelCodecWarcraft3::getNumFaces()
	{
		return m_numFaces;
	}

	uint ModelCodecWarcraft3::getNumVertices()
	{
		return m_numVertices;
	}

	uint ModelCodecWarcraft3::getVer()
	{
		return model.ver;
	}

	IVertexBuffer* ModelCodecWarcraft3::vertexBlend(Skeleton *pSkeleton,const AnimationTime *pTime)
	{
		if(!m_numVertices || !pTime || !pSkeleton)return 0;

		int index = (pTime->current * 3 / 100)% W3_PRECOMPUTED_VBO_COUNT_MAX;
		IVertexBuffer *pVB = m_pPrecomputedVBs[index];
		if(pVB)return pVB;
		m_pPrecomputedVBs[index] = m_pRenderSystem->getBufferManager()->createVertexBuffer(12,m_numVertices << 1,BU_STATIC_WRITE_ONLY, CBF_IMMEDIATE);
		pVB = m_pPrecomputedVBs[index];

		if(!pVB)return 0;

		IVertexBuffer *pVertexBlend = pVB;
		Vector3* pVertex = (Vector3*)pVertexBlend->lock(0,0,BL_DISCARD);
		if(!pVertex)return 0;

		//Skinning
		size_t pos = 0;
		size_t size = m_vGeosets.size();
		for(size_t i = 0;i < size;i++)
		{
			GeosetData* pData = m_vGeosets[i];
			size_t numVertices = pData->vertexContainer.size();
			for(size_t j = 0;j < numVertices;j++)
			{
				GeosetVertexData& vertex = pData->vertexContainer[j];
				GeosetGroupData& groupData = pData->groupContainer[vertex.vertexGroup];
				size_t matrixListSize = groupData.matrixList.size();
				pVertex[pos + j] = Vector3(0,0,0);
				pVertex[m_numVertices + pos + j] = Vector3(0,0,0);

				Vector3 TempVector;
				Vector3 TempVector2;
				Vector3 TempNormalTarget;
				Vector3 TempNormalTarget2;
				Vector3 NormalTarget;
				Vector3 TransformedNormalTarget;

				for(size_t k = 0;k < matrixListSize;k++)
				{
					GeosetGroupNodeData& node = groupData.matrixList[k];
					Bone *pBone = pSkeleton->getBone(node.nodeId);

					pVertex[pos + j] += pBone->getFullTransform() * vertex.position;
					pVertex[m_numVertices + pos + j] += pBone->getFullRotation() * vertex.normal;
				}
				float scale = (matrixListSize == 0) ? 0.0f : (1.0f / matrixListSize);
				pVertex[pos + j] = pVertex[pos + j] * scale;
				pVertex[m_numVertices + pos + j] = pVertex[m_numVertices + pos + j] * scale;
				pVertex[m_numVertices + pos + j].normalize();
			}

			pos += numVertices;
		}

		pVertexBlend->unlock();

		return pVertexBlend;
	}

	uint ModelCodecWarcraft3::numParticleSystem()
	{
		return m_vParticleEmitter2s.size();
	}

	ParticleEmitter2Data*	ModelCodecWarcraft3::getParticleSystemData(uint index)
	{
		if(index >= m_vParticleEmitter2s.size())return 0;

		return (ParticleEmitter2Data*)((uchar*)m_vParticleEmitter2s[index] + sizeof(BoneData));
	}

	uint ModelCodecWarcraft3::numRibbonSystem()
	{
		return m_vRibbonEmitters.size();
	}

	RibbonEmitterData*	ModelCodecWarcraft3::getRibbonSystemData(uint index)
	{
		if(index >= m_vRibbonEmitters.size())return 0;

		return (RibbonEmitterData*)((uchar*)m_vRibbonEmitters[index] + sizeof(BoneData));
	}

	uint ModelCodecWarcraft3::numMmParticleSystem()
	{
		return 0;
	}

	MmParticleSystemData* ModelCodecWarcraft3::getMmParticleSystem(uint index)
	{
		return 0;
	}

	const char*	ModelCodecWarcraft3::getType() const
	{
		static std::string strType = "mdx";
		return strType.c_str();
	}

	uint reverseUint(uint tag)
	{
		uint NewDWord;
		char* Source;
		char* Target;

		Source = reinterpret_cast<char*>(&tag);
		Target = reinterpret_cast<char*>(&NewDWord);

		Target[0] = Source[3];
		Target[1] = Source[2];
		Target[2] = Source[1];
		Target[3] = Source[0];

		return NewDWord;
	}

	void ModelCodecWarcraft3::createCoreSkeleton()
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
			pCoreBone->setCanBufferData(true);//mdx不会出现在游戏里面，这样设置方便查看游戏是否用了mdx
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

	bool ModelCodecWarcraft3::decode(xs::Stream *pStream)
	{
		PP_BY_NAME("ModelCodecWarcraft3::decode");

		uint identityTag;
		pStream->read(&identityTag,sizeof(identityTag));
		identityTag = reverseUint(identityTag);
		if(identityTag != 'MDLX')return false;

		DataChunk chunk;
		xs::DataChunk::stChunk *pChunk = chunk.beginChunk(pStream);
		while(pChunk)
		{
			if(pChunk->m_ui32DataSize == 0)
			{
				pChunk = chunk.nextChunk(pStream);
				continue;
			}
			xs::MemoryStream stream((uchar*)pChunk->m_pData,pChunk->m_ui32DataSize);
			uint type = pChunk->m_ui32Type;
			type = reverseUint(type);
			switch(type)
			{
			case 'VERS':
				{
					stream.read(&model.ver,sizeof(model.ver));
				}
				break;
			case 'MODL':
				{
					model.name[MODEL_NAME_SIZE] = 0;
					stream.read(model.name,MODEL_NAME_SIZE);
					uint unknown;
					stream.read(&unknown,sizeof(unknown));
					float radius;
					stream.read(&radius,sizeof(radius));
					Vector3 vmin;
					stream.read(&vmin.x,sizeof(vmin.x));
					stream.read(&vmin.y,sizeof(vmin.y));
					stream.read(&vmin.z,sizeof(vmin.z));
					Vector3 vmax;
					stream.read(&vmax.x,sizeof(vmax.x));
					stream.read(&vmax.y,sizeof(vmax.y));
					stream.read(&vmax.z,sizeof(vmax.z));
					model.extent.radius = radius;
					model.extent.min = W2M(vmin);
					model.extent.max = W2M(vmax);
					stream.read(&model.blendTime,sizeof(model.blendTime));

					m_aabb.setExtents(vmin,vmax);
					m_sphere.setCenter(vmax.midPoint(vmin));
					m_sphere.setRadius(radius);
				}
				break;
			case 'SEQS':
				{
					uint currentSize;

					currentSize = 0;
					while(currentSize < pChunk->m_ui32DataSize)
					{
						char name[MODEL_NAME_SEQUENCE_SIZE + 1];
						name[MODEL_NAME_SEQUENCE_SIZE] = 0;
						stream.read(name,MODEL_NAME_SEQUENCE_SIZE);
						uint timeStart,timeEnd;
						stream.read(&timeStart,sizeof(timeStart));
						stream.read(&timeEnd,sizeof(timeEnd));
						float moveSpeed;
						stream.read(&moveSpeed,sizeof(moveSpeed));
						uint nonLooping;
						stream.read(&nonLooping,sizeof(nonLooping));
						float rarity;
						stream.read(&rarity,sizeof(rarity));

						uint unknown;
						stream.read(&unknown,sizeof(unknown));
						float radius;
						stream.read(&radius,sizeof(radius));
						Vector3 vmin,vmax;
						stream.read(&vmin.x,sizeof(vmin.x));
						stream.read(&vmin.y,sizeof(vmin.y));
						stream.read(&vmin.z,sizeof(vmin.z));
						stream.read(&vmax.x,sizeof(vmax.x));
						stream.read(&vmax.y,sizeof(vmax.y));
						stream.read(&vmax.z,sizeof(vmax.z));
						vmin = W2M(vmin);
						vmax = W2M(vmax);

						Animation *pAnimation = new Animation(timeStart,timeEnd,nonLooping == false,name);
						m_vAnimations.push_back(pAnimation);
						m_AnimationMap[name] = pAnimation;

						currentSize += 132;
					}
				}
				break;
			case 'GLBS':
				{
					uint currentSize;

					currentSize = 0;
					while(currentSize < pChunk->m_ui32DataSize)
					{
						int globalSequence;
						currentSize += 4;
						stream.read(&globalSequence,sizeof(globalSequence));
						m_vGlobalSequences.push_back(globalSequence);
					}

				}
				break;
			case 'TEXS':
				{
					uint currentSize;

					currentSize = 0;
					while(currentSize < pChunk->m_ui32DataSize)
					{
						currentSize += 268;

						TextureData data;
						data.fileName[MODEL_NAME_TEXTURE_SIZE] = 0;
						stream.read(&data.replaceableId,sizeof(data.replaceableId));
						stream.read(data.fileName,MODEL_NAME_TEXTURE_SIZE);

						if(StringHelper::casecmp(data.fileName,"") == 0)
						{
							getReplaceableTexture(data.replaceableId,data.fileName);
						}

						uint unknown;
						stream.read(&unknown,sizeof(unknown));

						uint flags;
						stream.read(&flags,sizeof(flags));
						data.wrapWidth = (0 != (flags & 1));
						data.wrapHeight = (0 != (flags & 2));

						m_vTextures.push_back(data);
					}
				}
				break;
			case 'MTLS':
				{
					uint currentSize;

					currentSize = 0;
					while(currentSize < pChunk->m_ui32DataSize)
					{
						uint mtlSize;
						stream.read(&mtlSize,sizeof(mtlSize));
						currentSize += mtlSize;

						MaterialData data;

						stream.read(&data.priorityPlane,sizeof(data.priorityPlane));
						uint flags;
						stream.read(&flags,sizeof(flags));
						data.constantColor = (0 != (flags & 1));
						data.sortPrimitivesFarZ = (0 != (flags & 16));
						data.fullResolution = (0 != (flags & 32));

						uint tag;
						stream.read(&tag,sizeof(tag));
						tag = reverseUint(tag);
						if(tag != 'LAYS')break;

						uint numLayers;
						stream.read(&numLayers,sizeof(numLayers));
						for(uint i = 0;i < numLayers;i++)
						{
							int layerSize;
							stream.read(&layerSize,sizeof(layerSize));

							int endPos = stream.getPosition() + layerSize - sizeof(layerSize);

							MaterialLayerData *pLayer = new MaterialLayerData;

							stream.read(&pLayer->blendMode,sizeof(pLayer->blendMode));

							uint flags;
							stream.read(&flags,sizeof(flags));

							pLayer->unshaded = (0 != (flags & 1));
							pLayer->sphereEnvironmentMap = (0 != (flags & 2));
							pLayer->twoSided = (0 != (flags & 16));
							pLayer->unfogged = (0 != (flags & 32));
							pLayer->noDepthTest = (0 != (flags & 64));
							pLayer->noDepthSet = (0 != (flags & 128));

							stream.read(&pLayer->textureId,sizeof(pLayer->textureId));
							pLayer->animatedTextureId.setStaticData(pLayer->textureId);
							stream.read(&pLayer->textureAnimationId,sizeof(pLayer->textureAnimationId));

							uint unknown;
							stream.read(&unknown,sizeof(unknown));

							float alpha;
							stream.read(&alpha,sizeof(alpha));
							pLayer->alpha.setStaticData(alpha);

							while(stream.getPosition() < endPos)
							{
								uint tag;
								stream.read(&tag,sizeof(tag));
								tag = reverseUint(tag);
								switch(tag)
								{
								case 'KMTA':
									{
										readKeyFrames(&stream,pLayer->alpha,false);
									}
									break;
								case 'KMTF':
									{
										readKeyFrames(&stream,pLayer->animatedTextureId,false);
									}
									break;
								}
							}

							data.layerContainer.push_back(pLayer);
						}
						m_vMaterials.push_back(data);
					}
				}
				break;
			case 'TXAN':
				{
					uint currentSize;
					uint textureAnimationSize;

					currentSize = 0;
					while(currentSize < pChunk->m_ui32DataSize)
					{
						textureAnimationSize;
						stream.read(&textureAnimationSize,sizeof(textureAnimationSize));
						currentSize += textureAnimationSize;

						TextureAnimationData *pData = new TextureAnimationData;

						int endPos = stream.getPosition() + textureAnimationSize - sizeof(textureAnimationSize); 

						while(stream.getPosition() < endPos)
						{
							uint tag;
							stream.read(&tag,sizeof(tag));
							tag = reverseUint(tag);
							switch(tag)
							{
							case 'KTAT':
								readKeyFrames(&stream,pData->translation,false);
								break;
							case 'KTAR':
								readKeyFrames(&stream,pData->rotation,false);
								break;
							case 'KTAS':
								readKeyFrames(&stream,pData->scale,true);
								break;
							}
						}

						m_vTextureAnimations.push_back(pData);
					}
				}
				break;
			case 'GEOS':
				{
					uint currentSize;
					uint geosetSize;

					currentSize = 0;
					while(currentSize < pChunk->m_ui32DataSize)
					{
						stream.read(&geosetSize,sizeof(geosetSize));
						currentSize += geosetSize;

						GeosetData *pData = new GeosetData;
						readGeosetData(&stream,pData);
						m_vGeosets.push_back(pData);
					}

					size_t size = m_vGeosets.size();

					m_numVertices = 0;
					m_numFaces = 0;
					
					for(size_t i = 0;i < size;i++)
					{
						GeosetData *pData = m_vGeosets[i];
						m_numFaces += pData->faceContainer.size();
						m_numVertices += pData->vertexContainer.size();
					}

					/*m_pVBPosNormal = m_pRenderSystem->getBufferManager()->createVertexBuffer(12,2 * m_numVertices,BU_DYNAMIC_WRITE_ONLY);
					if(!m_pVBPosNormal)return false;*/
					m_pVBTexcoord = m_pRenderSystem->getBufferManager()->createVertexBuffer(8,m_numVertices,BU_STATIC_WRITE_ONLY);
					if(!m_pVBTexcoord)return false;
					m_pIB = m_pRenderSystem->getBufferManager()->createIndexBuffer(IT_16BIT,m_numFaces * 3,BU_STATIC_WRITE_ONLY);
					if(!m_pIB)return false;

					/*float* p1 = (float*)m_pVBPosNormal->lock(0,0,BL_DISCARD);*/
					float* p2 = (float*)m_pVBTexcoord->lock(0,0,BL_NORMAL);
					short *p3 = (short *)m_pIB->lock(0,0,BL_NORMAL);

					int vertexStart = 0;
					int indexStart = 0;
					for(size_t i = 0;i < size;i++)
					{
						GeosetData *pData = m_vGeosets[i];
						SubModel subModel;

						char name[256];
						sprintf(name,"part%d",i);
						subModel.m_name = name;

						subModel.vertexStart = vertexStart;
						subModel.vertexEnd = vertexStart + pData->vertexContainer.size() - 1;
						subModel.indexStart = indexStart;
						subModel.indexCount = pData->faceContainer.size() * 3;

						m_vSubModels.push_back(subModel);

						size_t s1 = pData->vertexContainer.size();
						for(size_t j = 0;j < s1;j++)
						{
							GeosetVertexData &data = pData->vertexContainer[j];
							/**p1++ = data.position.x;
							*p1++ = data.position.y;
							*p1++ = data.position.z;*/

							*p2++ = data.texturePosition.x;
							*p2++ = data.texturePosition.y;
						}

						size_t s3 = pData->faceContainer.size();
						for(size_t j = 0;j < s3;j++)
						{
							GeosetFaceData &data = pData->faceContainer[j];
							data.index1 += vertexStart;
							data.index2 += vertexStart;
							data.index3 += vertexStart;

							*p3++ = data.index1;
							*p3++ = data.index2;
							*p3++ = data.index3;
						}

						vertexStart = subModel.vertexEnd + 1;
						indexStart += subModel.indexCount;
					}

					/*for(size_t i = 0;i < size;i++)
					{
						GeosetData *pData = m_vGeosets[i];
						size_t s1 = pData->vertexContainer.size();
						for(size_t j = 0;j < s1;j++)
						{
							GeosetVertexData &data = pData->vertexContainer[j];
							*p1++ = data.normal.x;
							*p1++ = data.normal.y;
							*p1++ = data.normal.z;
						}
					}*/

					//m_pVBPosNormal->unlock();
					m_pVBTexcoord->unlock();
					m_pIB->unlock();
				}
				break;
			case 'GEOA':
				{
					uint currentSize;
					uint geosetAnimationSize;

					currentSize = 0;
					while(currentSize < pChunk->m_ui32DataSize)
					{
						stream.read(&geosetAnimationSize,sizeof(geosetAnimationSize));
						currentSize += geosetAnimationSize;

						int endPos = stream.getPosition() + geosetAnimationSize - sizeof(geosetAnimationSize);

						GeosetAnimationData *pData = new GeosetAnimationData;

						float alpha;
						stream.read(&alpha,sizeof(alpha));
						pData->alpha.setStaticData(alpha);

						uint flags;
						stream.read(&flags,sizeof(flags));

						pData->dropShadow = (0 != (flags & 1));
						pData->useColor = (0 != (flags & 2));

						Color3 color;
						stream.read(&color,sizeof(color));
						pData->color.setStaticData(color);

						uint id;
						stream.read(&id,sizeof(id));
						pData->geosetId = (id == 0xFFFFFFFF) ? -1 : id;

						while(stream.getPosition() < endPos)
						{
							uint tag;
							stream.read(&tag,sizeof(tag));
							tag = reverseUint(tag);
							switch(tag)
							{
							case 'KGAO':
								readKeyFrames(&stream,pData->alpha,false);
								break;
							case 'KGAC':
								readKeyFrames(&stream,pData->color,false);
								break;
							default:
								Assert(0);
								break;
							}
						}

						m_vGeosetAnimations.push_back(pData);
					}
				}
				break;
			case 'BONE':
				{
					int endPos = stream.getPosition() + pChunk->m_ui32DataSize;
					while(stream.getPosition() < endPos)
					{
						BoneExtraData *pBone = new BoneExtraData;
						m_vBoneExtras.push_back(pBone);
						m_vBones.push_back(pBone);

						readBaseData(&stream,pBone);

						uint id;
						stream.read(&id,sizeof(id));
						pBone->geosetId = (id == 0xFFFFFFFF) ? -1 : id;

						stream.read(&id,sizeof(id));
						pBone->geosetAnimationId = (id == 0xFFFFFFFF) ? -1 : id;
					}
				}
				break;
			case 'LITE':
				{
					uint currentSize;
					int lightSize;

					currentSize = 0;
					while(currentSize < pChunk->m_ui32DataSize)
					{
						stream.read(&lightSize,sizeof(lightSize));
						currentSize += lightSize;

						int endPos = stream.getPosition() + lightSize - sizeof(lightSize);

						LightData *pLight = new LightData;
						readBaseData(&stream,pLight);
						m_vBones.push_back(pLight);
						LightData *pData = pLight;

						stream.read(&pData->type,sizeof(pData->type));
						float tmp;
						stream.read(&tmp,sizeof(tmp));
						pData->attenuationStart.setStaticData(tmp);
						stream.read(&tmp,sizeof(tmp));
						pData->attenuationEnd.setStaticData(tmp);

						Color3 v;
						stream.read(&v,sizeof(v));
						pData->color.setStaticData(v);

						stream.read(&tmp,sizeof(tmp));
						pData->intensity.setStaticData(tmp);

						stream.read(&v,sizeof(v));
						pData->ambientColor.setStaticData(v);

						stream.read(&tmp,sizeof(tmp));
						pData->ambientIntensity.setStaticData(tmp);

						while(stream.getPosition() < endPos)
						{
							uint tag;
							stream.read(&tag,sizeof(tag));
							tag = reverseUint(tag);
							switch(tag)
							{
							case 'KLAV':
								readKeyFrames(&stream,pData->visibility,false);
								break;
							case 'KLAC':
								readKeyFrames(&stream,pData->color,false);
								break;
							case 'KLAI':
								readKeyFrames(&stream,pData->intensity,false);
								break;
							case 'KLBC':
								readKeyFrames(&stream,pData->ambientColor,false);
								break;
							case 'KLBI':
								readKeyFrames(&stream,pData->ambientIntensity,false);
								break;
							default:
								Assert(0);
								break;
							}
						}
						m_vLights.push_back(pLight);
					}
				}
				break;
			case 'HELP':
				{
					int endPos = stream.getPosition() + pChunk->m_ui32DataSize;
					while(stream.getPosition() < endPos)
					{
						HelperData *pHelper = new HelperData;
						readBaseData(&stream,pHelper);
						m_vHelpers.push_back(pHelper);
						m_vBones.push_back(pHelper);
					}
				}
				break;
			case 'ATCH':
				{
					uint currentSize;
					uint attachmentSize;

					currentSize = 0;
					while(currentSize < pChunk->m_ui32DataSize)
					{
						stream.read(&attachmentSize,sizeof(attachmentSize));
						currentSize += attachmentSize;

						int endPos = stream.getPosition() + attachmentSize - sizeof(attachmentSize);

						AttachmentData *pAttachment = new AttachmentData;

						readBaseData(&stream,pAttachment);
						m_vBones.push_back(pAttachment);
						AttachmentData *pData = pAttachment;

						pData->path[MODEL_NAME_ATTACHMENT_SIZE] = 0;
						stream.read(pData->path,MODEL_NAME_ATTACHMENT_SIZE);

						uint unknown;
						stream.read(&unknown,sizeof(unknown));

						stream.read(&pData->attachmentId,sizeof(pData->attachmentId));

						while(stream.getPosition() < endPos)
						{
							uint tag;
							stream.read(&tag,sizeof(tag));
							tag = reverseUint(tag);
							switch(tag)
							{
							case 'KATV':
								readKeyFrames(&stream,pData->visibility,false);
								break;
							default:
								Assert(0);
								break;
							}
						}
						m_vAttachments.push_back(pAttachment);
					}
				}
				break;
			case 'PIVT':
				{
					uint numPivotPoints = pChunk->m_ui32DataSize / 12;

					m_vPivotPoints.clear();
					m_vPivotPoints.reserve(numPivotPoints);
					for(uint i = 0;i < numPivotPoints;i++)
					{
						Vector3 v;
						stream.read(&v,sizeof(v));
						v = W2M(v);
						m_vPivotPoints.push_back(v);
					}
				}
				break;
			case 'PREM':
				{
					uint currentSize;
					uint particleEmitterSize;
					currentSize = 0;
					while(currentSize < pChunk->m_ui32DataSize)
					{
						stream.read(&particleEmitterSize,sizeof(particleEmitterSize));
						currentSize += particleEmitterSize;
						int endPos = stream.getPosition() + particleEmitterSize - sizeof(particleEmitterSize);

						ParticleEmitterData *pParticleEmitter = new ParticleEmitterData;

						uint nodeType;
						readBaseData(&stream,pParticleEmitter,&nodeType);
						m_vBones.push_back(pParticleEmitter);
						ParticleEmitterData *pData = pParticleEmitter;

						pData->emitterUsesMdl = (0 != (nodeType & NODE_FLAG_EMITTER_USES_MDL));
						pData->emitterUsesTga = (0 != (nodeType & NODE_FLAG_EMITTER_USES_TGA));

						float tmp;
						stream.read(&tmp,sizeof(tmp));pData->emissionRate.setStaticData(tmp);
						stream.read(&tmp,sizeof(tmp));pData->gravity.setStaticData(tmp);
						stream.read(&tmp,sizeof(tmp));pData->longitude.setStaticData(tmp);
						stream.read(&tmp,sizeof(tmp));pData->latitude.setStaticData(tmp);

						pData->particleFileName[MODEL_NAME_PARTICLE_EMITTER_SIZE] = 0;
						stream.read(pData->particleFileName,MODEL_NAME_PARTICLE_EMITTER_SIZE);

						uint unknown;
						stream.read(&unknown,sizeof(unknown));

						stream.read(&tmp,sizeof(tmp));pData->particleLifeSpan.setStaticData(tmp);
						stream.read(&tmp,sizeof(tmp));pData->particleInitialVelocity.setStaticData(tmp);

						while(stream.getPosition() < endPos)
						{
							uint tag;
							stream.read(&tag,sizeof(tag));
							tag = reverseUint(tag);
							switch(tag)
							{
							case 'KPEV':
								readKeyFrames(&stream,pData->visibility,false);
								break;
							default:
								Assert(0);
								break;
							}
						}

						m_vParticleEmitters.push_back(pParticleEmitter);
					}
				}
				break;
			case 'PRE2':
				{
					uint currentSize;
					uint particleEmitter2Size;
					currentSize = 0;
					while(currentSize < pChunk->m_ui32DataSize)
					{
						stream.read(&particleEmitter2Size,sizeof(particleEmitter2Size));
						currentSize += particleEmitter2Size;

						int endPos = stream.getPosition() + particleEmitter2Size -sizeof(particleEmitter2Size);

						Warcraft3ParticleEmitter2 *pParticleEmitter2 = new Warcraft3ParticleEmitter2;

						uint nodeType;
						readBaseData(&stream,pParticleEmitter2,&nodeType);
						m_vBones.push_back(pParticleEmitter2);
						Warcraft3ParticleEmitter2 *pData = pParticleEmitter2;

						pData->xyQuad = (0 != (nodeType & NODE_FLAG_XY_QUAD));
						pData->unshaded = (0 != (nodeType & NODE_FLAG_UNSHADED));
						pData->unfogged = (0 != (nodeType & NODE_FLAG_UNFOGGED));
						pData->modelSpace = (0 != (nodeType & NODE_FLAG_MODEL_SPACE));
						pData->lineEmitter = (0 != (nodeType & NODE_FLAG_LINE_EMITTER));
						pData->sortPrimitivesFarZ = (0 != (nodeType & NODE_FLAG_SORT_PRIMITIVES_FAR_Z));

						float tmp;
						stream.read(&tmp,sizeof(tmp));pData->speed.setStaticData(tmp);
						stream.read(&tmp,sizeof(tmp));pData->variation.setStaticData(tmp);
						stream.read(&tmp,sizeof(tmp));pData->latitude.setStaticData(tmp);
						stream.read(&tmp,sizeof(tmp));pData->gravity.setStaticData(tmp);
						stream.read(&tmp,sizeof(tmp));pData->lifeSpan = tmp;
						stream.read(&tmp,sizeof(tmp));pData->emissionRate.setStaticData(tmp);
						stream.read(&tmp,sizeof(tmp));pData->length.setStaticData(tmp);
						stream.read(&tmp,sizeof(tmp));pData->width.setStaticData(tmp);

						uint filterMode;
						stream.read(&filterMode,sizeof(filterMode));
						switch(filterMode)
						{
						case 0:
							{
								pData->filterMode = BM_ALPHA_BLEND;
								break;
							}

						case 1:
							{
								pData->filterMode = BM_ADDITIVE;
								break;
							}

						case 2:
							{
								pData->filterMode = BM_MODULATE;
								break;
							}

						case 4:
							{
								pData->filterMode = BM_ADDITIVE_ALPHA;
								break;
							}

						default:
							{
								Assert(0);
								break;
							}
						}

						stream.read(&pData->rows,sizeof(pData->rows));
						stream.read(&pData->columns,sizeof(pData->columns));

						uint headTail;
						stream.read(&headTail,sizeof(headTail));
						switch(headTail)
						{
						case 0:
							{
								pData->head = true;
								break;
							}

						case 1:
							{
								pData->tail = true;
								break;
							}

						case 2:
							{
								pData->head = true;
								pData->tail = true;
								break;
							}
						}

						stream.read(&pData->tailLength,sizeof(pData->tailLength));
						stream.read(&pData->time,sizeof(pData->time));

						Color3 c;
						stream.read(&c,sizeof(c));pData->segmentColor1 = c;
						stream.read(&c,sizeof(c));pData->segmentColor2 = c;
						stream.read(&c,sizeof(c));pData->segmentColor3 = c;

						uchar a;
						Vector3 alpha;
						stream.read(&a,sizeof(uchar));a /= 255.0f;alpha.x = a;
						stream.read(&a,sizeof(uchar));a /= 255.0f;alpha.y = a;
						stream.read(&a,sizeof(uchar));a /= 255.0f;alpha.z = a;

						Vector3 v;
						stream.read(&v,sizeof(v));pData->particleScaling = v;

						uint u;
						stream.read(&u,sizeof(u));v.x = u;
						stream.read(&u,sizeof(u));v.y = u;
						stream.read(&u,sizeof(u));v.z = u;
						pData->headLifeSpan = v;

						stream.read(&u,sizeof(u));v.x = u;
						stream.read(&u,sizeof(u));v.y = u;
						stream.read(&u,sizeof(u));v.z = u;
						pData->headDecay = v;

						stream.read(&u,sizeof(u));v.x = u;
						stream.read(&u,sizeof(u));v.y = u;
						stream.read(&u,sizeof(u));v.z = u;
						pData->tailLifeSpan = v;

						stream.read(&u,sizeof(u));v.x = u;
						stream.read(&u,sizeof(u));v.y = u;
						stream.read(&u,sizeof(u));v.z = u;
						pData->tailDecay = v;

						stream.read(&u,sizeof(u));
						pData->textureId = (u == 0xFFFFFFFF) ? -1 : u;

						stream.read(&u,sizeof(u));
						pData->squirt = (u != 0);

						stream.read(&pData->priorityPlane,sizeof(pData->priorityPlane));
						stream.read(&pData->replaceableId,sizeof(pData->replaceableId));

						while(stream.getPosition() < endPos)
						{
							uint tag;
							stream.read(&tag,sizeof(tag));
							tag = reverseUint(tag);
							switch(tag)
							{
							case 'KP2V':
								readKeyFrames(&stream,pData->visibility,false);
								break;
							case 'KP2E':
								readKeyFrames(&stream,pData->emissionRate,false);
								break;
							case 'KP2W':
								readKeyFrames(&stream,pData->width,false);
								break;
							case 'KP2N':
								readKeyFrames(&stream,pData->length,false);
								break;
							case 'KP2S':
								readKeyFrames(&stream,pData->speed,false);
								break;
							case 'KP2L':
								readKeyFrames(&stream,pData->latitude,false);
								break;
							default:
								Assert(0);
								break;
							}
						}

						m_vParticleEmitter2s.push_back(pParticleEmitter2);
					}
				}
				break;
			case 'RIBB':
				{
					uint currentSize;
					uint ribbonEmitterSize;

					Stream *pStream = &stream;

					currentSize = 0;
					while(currentSize < pChunk->m_ui32DataSize)
					{
						stream.read(&ribbonEmitterSize,sizeof(ribbonEmitterSize));
						currentSize += ribbonEmitterSize;

						int endPos = stream.getPosition() + ribbonEmitterSize - sizeof(ribbonEmitterSize);

						Warcraft3RibbonEmitter *pRibbonEmitter = new Warcraft3RibbonEmitter;

						readBaseData(pStream,pRibbonEmitter);
						m_vBones.push_back(pRibbonEmitter);
						Warcraft3RibbonEmitter *pData = pRibbonEmitter;

						float tmp;
						stream.read(&tmp,sizeof(tmp));pData->heightAbove.setStaticData(tmp);
						stream.read(&tmp,sizeof(tmp));pData->heightBelow.setStaticData(tmp);
						stream.read(&tmp,sizeof(tmp));pData->alpha.setStaticData(tmp);
						Color3 v;
						stream.read(&v,sizeof(v));
						pData->color.setStaticData(v);

						stream.read(&pData->lifeSpan,sizeof(pData->lifeSpan));

						uint unknown;
						stream.read(&unknown,sizeof(unknown));
						uint u;
						stream.read(&u,sizeof(u));
						pData->emissionRate = u;
						stream.read(&pData->rows,sizeof(pData->rows));
						stream.read(&pData->columns,sizeof(pData->columns));

						stream.read(&u,sizeof(u));
						pData->materialId = (u == 0xFFFFFFFF) ? -1 : u;
						stream.read(&pData->gravity,sizeof(pData->gravity));

						while(stream.getPosition() < endPos)
						{
							uint tag;
							stream.read(&tag,sizeof(tag));
							tag = reverseUint(tag);
							switch(tag)
							{
							case 'KRVS':
								readKeyFrames(pStream,pData->visibility,false);
								break;
							case 'KRHA':
								readKeyFrames(pStream,pData->heightAbove,false);
								break;
							case 'KRHB':
								readKeyFrames(pStream,pData->heightBelow,false);
								break;
							case 'KRAL':
								readKeyFrames(pStream,pData->alpha,false);
								break;
							default:
								Assert(0);
								break;
							}
						}
						m_vRibbonEmitters.push_back(pRibbonEmitter);
					}

				}
				break;
			case 'EVTS':
				{
					int endPos = stream.getPosition() + pChunk->m_ui32DataSize;
					while(stream.getPosition() < endPos)
					{
						EventObjectData *pEventObject = new EventObjectData;

						readBaseData(&stream,pEventObject);
						m_vBones.push_back(pEventObject);
						EventObjectData *pData = pEventObject;

						uint tag;
						stream.read(&tag,sizeof(tag));
						tag = reverseUint(tag);

						Assert(tag == 'KEVT');

						uint numEventTracks;
						stream.read(&numEventTracks,sizeof(numEventTracks));
						uint u;
						stream.read(&u,sizeof(u));
						pData->globalSequenceId = (u == 0xFFFFFFFF) ? -1 : u;

						for(uint i = 0;i < numEventTracks;i++)
						{
							stream.read(&u,sizeof(u));
							pData->eventTrack.push_back(u);
						}

						m_vEventObjects.push_back(pEventObject);
					}
				}
				break;
			case 'CAMS':
				{
					uint currentSize;
					uint cameraSize;

					currentSize = 0;
					while(currentSize < pChunk->m_ui32DataSize)
					{
						stream.read(&cameraSize,sizeof(cameraSize));
						currentSize += cameraSize;

						int endPos = stream.getPosition() + cameraSize - sizeof(cameraSize);

						CameraData *pData = new CameraData;

						pData->name[MODEL_NAME_CAMERA_SIZE] = 0;
						stream.read(pData->name,MODEL_NAME_CAMERA_SIZE);

						stream.read(&pData->source,sizeof(pData->source));
						stream.read(&pData->fieldOfView,sizeof(pData->fieldOfView));
						stream.read(&pData->farDistance,sizeof(pData->farDistance));
						stream.read(&pData->nearDistance,sizeof(pData->nearDistance));

						stream.read(&pData->target,sizeof(pData->target));

						while(stream.getPosition() < endPos)
						{
							uint tag;
							stream.read(&tag,sizeof(tag));
							tag = reverseUint(tag);
							switch(tag)
							{
							case 'KCTR':
								readKeyFrames(&stream,pData->sourceTranslation,false);
								break;
							case 'KTTR':
								readKeyFrames(&stream,pData->targetTranslation,false);
								break;
							case 'KCRL':
								readKeyFrames(&stream,pData->rotation,false);
								break;
							default:
								Assert(0);
								break;
							}
						}

						m_vCameras.push_back(pData);
					}
				}
				break;
			case 'CLID':
				{
					int endPos = stream.getPosition() + pChunk->m_ui32DataSize;
					while(stream.getPosition() < endPos)
					{
						CollisionShapeData *pCollisionShape = new CollisionShapeData;

						readBaseData(&stream,pCollisionShape);
						m_vBones.push_back(pCollisionShape);
						CollisionShapeData *pData = pCollisionShape;

						uint u;
						stream.read(&u,sizeof(u));

						uint numVertices = 0;
						switch(u)
						{
						case 0:
							numVertices = 2;
							pData->type = COLLISION_SHAPE_TYPE_BOX;
							break;
						case 2:
							numVertices = 1;
							pData->type = COLLISION_SHAPE_TYPE_SPHERE;
							break;
						default:
							Assert(0);
							break;
						}

						pData->vertexList.clear();
						for(uint i = 0; i < numVertices; i++)
						{
							Vector3 v;
							stream.read(&v,sizeof(v));
							v = W2M(v);
							pData->vertexList.push_back(v);
						}

						if(pData->type == COLLISION_SHAPE_TYPE_SPHERE)
						{
							stream.read(&pData->boundsRadius,sizeof(pData->boundsRadius));
						}

						m_vCollisionShapes.push_back(pCollisionShape);
					}
				}
				break;
			}
			pChunk = chunk.nextChunk(pStream);
		}

		//骨骼pivot
		size_t size = m_vPivotPoints.size();
		for(size_t i = 0; i < size; i++)
		{
			BoneData *pBone = m_vBones[i];
			pBone->pivotPoint = m_vPivotPoints[i];
		}

		//Materials
		std::vector<Material> vMtls;

		size = m_vMaterials.size();
		for(size_t i = 0;i < size;i++)
		{
			MaterialData& data = m_vMaterials[i];
			Material mtl;
			char str[256];
			sprintf(str,"mtl%d",i);
			mtl.setName(str);
			MaterialLayer layer;
			size_t layerNum = data.layerContainer.size();

			for(size_t j = 0;j < layerNum;j++)
			{
				MaterialLayerData& layerData = *data.layerContainer[j];
				layer.m_blendMode = layerData.blendMode;
				layer.m_bUnfogged = layerData.unfogged;
				layer.m_bUnshaded = layerData.unshaded;
				layer.m_bTwoSide = layerData.twoSided;
				layer.m_SphereEnvironmentMap = layerData.sphereEnvironmentMap;
				layer.m_bNoDepthSet = layerData.noDepthSet;
				layer.m_bNoDepthTest = layerData.noDepthTest;
				TextureData& texture = m_vTextures[layerData.textureId];

				layer.m_szTexture = texture.fileName;
				if(!getFileSystem()->isExist(texture.fileName))
				{
					int iPos = m_strName.rfind('\\');
					if(iPos != std::string::npos)
					{
						std::string strTargetDir(m_strName.c_str(),0,iPos);
						CPath path = texture.fileName;
						std::string fn = path.getFileName();
						layer.m_szTexture = strTargetDir + "\\" + fn;
					}
				}
				layer.m_bClampS = !texture.wrapWidth;
				layer.m_bClampT = !texture.wrapHeight;
				mtl.addLayer(layer);
			}
			vMtls.push_back(mtl);
		}

		//Geoset Mtl
		size = m_vGeosets.size();
		for(size_t i = 0;i < size;i++)
		{
			GeosetData& data = *m_vGeosets[i];
			m_vSubModels[i].setMaterial(vMtls[data.materialId]);
		}

		//Geoset Animation
		size = m_vGeosetAnimations.size();
		for(size_t i = 0;i < size;i++)
		{
			GeosetAnimationData *pData = m_vGeosetAnimations[i];
			m_vSubModels[pData->geosetId].m_GeosetAnimation.alpha = pData->alpha;
		}

		//ParticleSystem
		size = m_vParticleEmitter2s.size();
		for(size_t i = 0;i < size;i++)
		{
			Warcraft3ParticleEmitter2 *pData = m_vParticleEmitter2s[i];
			if(pData->textureId != -1)
			{
				pData->textureFilename = m_vTextures[pData->textureId].fileName;
			}
			else if(pData->replaceableId != -1)
			{
				char tn[MODEL_NAME_TEXTURE_SIZE + 1];
				getReplaceableTexture(pData->replaceableId,tn);
				pData->textureFilename = tn;
			}
			if(!getFileSystem()->isExist(pData->textureFilename.c_str()))
			{
				int iPos = m_strName.rfind('\\');
				if(iPos != std::string::npos)
				{
					std::string strTargetDir(m_strName.c_str(),0,iPos);
					CPath path = pData->textureFilename;
					std::string fn = path.getFileName();
					pData->textureFilename = strTargetDir + "\\" + fn;
				}
			}
			pData->boneObjectId = pData->objectId;
		}

		//RibbonSystem
		size = m_vRibbonEmitters.size();
		for(size_t i = 0;i < size;i++)
		{
			Warcraft3RibbonEmitter *pData = m_vRibbonEmitters[i];
			if(pData->materialId != -1)
			{
				pData->filterMode = vMtls[pData->materialId].getLayer(0)->m_blendMode;
				pData->textureFilename = vMtls[pData->materialId].getLayer(0)->m_szTexture;
			}
			if(!getFileSystem()->isExist(pData->textureFilename.c_str()))
			{
				int iPos = m_strName.rfind('\\');
				if(iPos != std::string::npos)
				{
					std::string strTargetDir(m_strName.c_str(),0,iPos);
					CPath path = pData->textureFilename;
					std::string fn = path.getFileName();
					pData->textureFilename = strTargetDir + "\\" + fn;
				}
			}
			pData->boneObjectId = pData->objectId;
		}
		////Vertex Data
		//m_pVertexData = new _VertexData[m_numVertices];
		//memset(m_pVertexData,0,m_numVertices * sizeof(_VertexData));
		//size = m_vGeosets.size();
		//size_t pos = 0;
		//for(size_t i = 0;i < size;i++)
		//{
		//	GeosetData* pData = m_vGeosets[i];
		//	size_t numVertices = pData->vertexContainer.size();
		//	for(size_t j = 0;j < numVertices;j++)
		//	{
		//		GeosetVertexData& vertex = pData->vertexContainer[j];

		//		_VertexData *pVertex = &m_pVertexData[pos + j];

		//		pVertex->pos[0] = vertex.position.x;
		//		pVertex->pos[1] = vertex.position.y;
		//		pVertex->pos[2] = vertex.position.z;
		//		pVertex->normal[0] = vertex.normal.x;
		//		pVertex->normal[1] = vertex.normal.y;
		//		pVertex->normal[2] = vertex.normal.z;
		//		pVertex->color[0] = 1.0f;
		//		pVertex->color[1] = 1.0f;
		//		pVertex->color[2] = 1.0f;
		//		pVertex->color[3] = 1.0f;
		//		pVertex->texcoords[0] = vertex.texturePosition.x;
		//		pVertex->texcoords[1] = vertex.texturePosition.y;

		//		GeosetGroupData& groupData = pData->groupContainer[vertex.vertexGroup];
		//		size_t matrixListSize = groupData.matrixList.size();

		//		int index = 0;
		//		float weight = 1.0f / matrixListSize;
		//		if(matrixListSize > 4)
		//		{
		//			index = index;
		//		}
		//		for(size_t k = 0;k < matrixListSize;k++)
		//		{
		//			GeosetGroupNodeData& node = groupData.matrixList[k];
		//			pVertex->bones[index] = node.nodeId;
		//			pVertex->weights[index] = weight;

		//			index++;
		//		}
		//	}

		//	pos += numVertices;
		//}

		//创建骨架
		PP_BY_NAME_START("ModelCodecWarcraft3::decode::createCoreSkeleton");
		createCoreSkeleton();
		PP_BY_NAME_STOP();

		m_memorySize += sizeof(*this);
		m_memorySize += m_skeleton.getMemeorySize();
		m_memorySize += m_vAnimations.size() * sizeof(Animation);
		m_memorySize += m_vGlobalSequences.size() * sizeof(uint);
		m_memorySize += m_vTextures.size() * sizeof(TextureData);
		size = m_vBones.size();
		for(size_t i = 0;i < size;i++)
		{
			m_memorySize += m_vBones[i]->getMemorySize();
		}
		m_memorySize += m_vMaterials.size() * sizeof(MaterialData);
		m_memorySize += m_vPivotPoints.size() * sizeof(Vector3);
		m_memorySize += m_vParticleEmitters.size() * sizeof(ParticleEmitter2Data);
		m_memorySize += m_vRibbonEmitters.size() * sizeof(RibbonEmitterData);

		//m_vGeosets没计算
		m_memorySize += m_vGeosets.size() * sizeof( GeosetData);
		
		return true;
	}

	void ModelCodecWarcraft3::readGeosetData(Stream* pStream,GeosetData *pData)
	{
		uint tag;
		pStream->read(&tag,sizeof(tag));
		tag = reverseUint(tag);

		Assert(tag == 'VRTX');

		uint numVertices;
		pStream->read(&numVertices,sizeof(numVertices));
		std::vector<Vector3> positionList;
		std::vector<Vector3> normalList;
		std::vector<Vector2> texturePositionList;
		std::vector<char> groupList;
		std::vector<uint> groupIndexList;

		positionList.reserve(numVertices);
		normalList.reserve(numVertices);
		groupList.reserve(numVertices);
		groupIndexList.reserve(numVertices);
		texturePositionList.reserve(numVertices);

		for(uint i = 0; i < numVertices; i++)
		{
			Vector3 v;
			pStream->read(&v,sizeof(v));
			v = W2M(v);
			positionList.push_back(v);
		}

		pStream->read(&tag,sizeof(tag));
		tag = reverseUint(tag);

		Assert(tag == 'NRMS');

		uint numNormals;
		pStream->read(&numNormals,sizeof(numNormals));
		Assert(numNormals == numVertices);

		for(uint i = 0; i < numNormals; i++)
		{
			Vector3 v;
			pStream->read(&v,sizeof(v));
			v = W2M(v);
			normalList.push_back(v);
		}

		pStream->read(&tag,sizeof(tag));
		tag = reverseUint(tag);

		Assert(tag == 'PTYP');

		uint numFaceTypeGroups;
		pStream->read(&numFaceTypeGroups,sizeof(numFaceTypeGroups));

		for(uint i = 0; i < numFaceTypeGroups; i++)
		{
			int faceType;
			pStream->read(&faceType,sizeof(faceType));
			Assert(faceType == 4 && "Can only load TriangleLists.");
		}

		pStream->read(&tag,sizeof(tag));
		tag = reverseUint(tag);
		
		Assert(tag == 'PCNT');

		uint numFaceGroups;
		pStream->read(&numFaceGroups,sizeof(numFaceGroups));

		for(uint i = 0; i < numFaceGroups; i++)
		{
			uint numIndices;
			pStream->read(&numIndices,sizeof(numIndices));
		}

		pStream->read(&tag,sizeof(tag));
		tag = reverseUint(tag);

		Assert(tag == 'PVTX');

		uint numIndices;
		pStream->read(&numIndices,sizeof(numIndices));
		Assert(numIndices % 3 == 0);
		uint numFaces = numIndices / 3;
		pData->faceContainer.reserve(numFaces);

		for(uint i = 0;i < numFaces;i++)
		{
			GeosetFaceData face;
			pStream->read(&face,sizeof(face));
			pData->faceContainer.push_back(face);
		}

		pStream->read(&tag,sizeof(tag));
		tag = reverseUint(tag);

		Assert(tag == 'GNDX');

		uint numVertexGroups;
		pStream->read(&numVertexGroups,sizeof(numVertexGroups));
		Assert(numVertices == numVertexGroups);

		for(uint i = 0; i < numVertexGroups; i++)
		{
			char group;
			pStream->read(&group,sizeof(group));
			groupList.push_back(group);
		}

		pStream->read(&tag,sizeof(tag));
		tag = reverseUint(tag);

		Assert(tag == 'MTGC');

		uint numGroups;
		pStream->read(&numGroups,sizeof(numGroups));
		for(uint i = 0; i < numGroups; i++)
		{
			GeosetGroupData group;
			int numMatrices;
			pStream->read(&numMatrices,sizeof(numMatrices));
			group.matrixListSize = numMatrices;
			pData->groupContainer.push_back(group);
		}

		pStream->read(&tag,sizeof(tag));
		tag = reverseUint(tag);

		Assert(tag == 'MATS');

		int j = -1;
		int indexSize = 0;
		uint numMatrices;
		pStream->read(&numMatrices,sizeof(numMatrices));
		for(uint i = 0;i < numMatrices;i++)
		{
			if(indexSize <= 0)
			{
				j++;
				indexSize = pData->groupContainer[j].matrixListSize;
			}

			GeosetGroupNodeData node;
			pStream->read(&node.nodeId,sizeof(node.nodeId));
			pData->groupContainer[j].matrixList.push_back(node);

			indexSize--;
		}

		pStream->read(&pData->materialId,sizeof(pData->materialId));
		pStream->read(&pData->selectionGroup,sizeof(pData->selectionGroup));

		uint unselectable;
		pStream->read(&unselectable,sizeof(unselectable));
		pData->unselectable = (unselectable == 4);

		pStream->read(&pData->extent.radius,sizeof(float));
		pStream->read(&pData->extent.min.x,sizeof(float));
		pStream->read(&pData->extent.min.y,sizeof(float));
		pStream->read(&pData->extent.min.z,sizeof(float));
		pStream->read(&pData->extent.max.x,sizeof(float));
		pStream->read(&pData->extent.max.y,sizeof(float));
		pStream->read(&pData->extent.max.z,sizeof(float));

		pData->extent.min = W2M(pData->extent.min);
		pData->extent.max = W2M(pData->extent.max);

		uint numAnimations;
		pStream->read(&numAnimations,sizeof(numAnimations));
		for(uint i = 0;i < numAnimations;i++)
		{
			Extent extent;
			pStream->read(&extent.radius,sizeof(float));
			pStream->read(&extent.min.x,sizeof(float));
			pStream->read(&extent.min.y,sizeof(float));
			pStream->read(&extent.min.z,sizeof(float));
			pStream->read(&extent.max.x,sizeof(float));
			pStream->read(&extent.max.y,sizeof(float));
			pStream->read(&extent.max.z,sizeof(float));
			extent.min = W2M(extent.min);
			extent.max = W2M(extent.max);

			pData->extentContainer.push_back(extent);
		}

		pStream->read(&tag,sizeof(tag));
		tag = reverseUint(tag);
		Assert(tag == 'UVAS');

		uint numTextureVertexGroups;
		pStream->read(&numTextureVertexGroups,sizeof(numTextureVertexGroups));

		pStream->read(&tag,sizeof(tag));
		tag = reverseUint(tag);
		Assert(tag == 'UVBS');

		uint numTexturePositions;
		pStream->read(&numTexturePositions,sizeof(numTexturePositions));
		Assert(numTexturePositions == numVertices);

		for(uint i = 0; i < numTexturePositions; i++)
		{
			Vector2 v;
			pStream->read(&v,sizeof(v));

			texturePositionList.push_back(v);
		}

		for(uint i = 0; i < numVertices; i++)
		{
			GeosetVertexData vertex;
			vertex.position = positionList[i];
			if(normalList.size() > i)
				vertex.normal = normalList[i];
			else
				vertex.normal = Vector3(0,0,1);
			vertex.texturePosition = texturePositionList[i];
			vertex.vertexGroup = groupList[i];

			pData->vertexContainer.push_back(vertex);
		}
	}

	void ModelCodecWarcraft3::readBaseData(Stream* pStream,BoneData* pData,uint *pNodeType)
	{
		int size;
		pStream->read(&size,sizeof(size));

		pData->name[MODEL_NAME_NODE_SIZE] = 0;
		pStream->read(pData->name,MODEL_NAME_NODE_SIZE);

		uint id;
		pStream->read(&id,sizeof(id));
		pData->objectId = (id == 0xFFFFFFFF) ? -1 : id;
		pStream->read(&id,sizeof(id));
		pData->parentId = (id == 0xFFFFFFFF) ? -1 : id;

		static Matrix4 precomputeMtx = 
			Matrix4(
			0, 1, 0, 0,
			0, 0, 1, 0,
			1, 0, 0, 0,
			0, 0, 0, 1);

		if(pData->parentId == -1)
		{
			pData->precomputeMtx = precomputeMtx;
		}

		uint flags;
		pStream->read(&flags,sizeof(flags));
		if(pNodeType){*pNodeType = flags;}

		pData->dontInheritTranslation = (0 != (flags & NODE_FLAG_DONT_INHERIT_TRANSLATION));

		pData->dontInheritTranslation = (0 != (flags & NODE_FLAG_DONT_INHERIT_TRANSLATION));
		pData->dontInheritRotation = (0 != (flags & NODE_FLAG_DONT_INHERIT_ROTATION));
		pData->dontInheritScaling = (0 != (flags & NODE_FLAG_DONT_INHERIT_SCALING));
		pData->billboarded = (0 != (flags & NODE_FLAG_BILLBOARDED));
		pData->billboardedLockX = (0 != (flags & NODE_FLAG_BILLBOARDED_LOCK_X));
		pData->billboardedLockY = (0 != (flags & NODE_FLAG_BILLBOARDED_LOCK_Y));
		pData->billboardedLockZ = (0 != (flags & NODE_FLAG_BILLBOARDED_LOCK_Z));
		pData->cameraAnchored = (0 != (flags & NODE_FLAG_CAMERA_ANCHORED));

		pData->pivotRotation = true;

		size -= 96;
		int endPos = pStream->getPosition() + size;

		while(pStream->getPosition() < endPos)
		{
			uint tag;
			pStream->read(&tag,sizeof(tag));
			tag = reverseUint(tag);

			switch(tag)
			{
			case 'KGTR':
				readKeyFrames(pStream,pData->translation,false);
				break;
			case 'KGRT':
				readKeyFrames(pStream,pData->rotation,false);
				break;
			case 'KGSC':
				readKeyFrames(pStream,pData->scale,true);
				break;
			default:
				Assert(0);
				break;
			}
		}
	}

	ModelCodecWarcraft3::ModelCodecWarcraft3(const std::string& name,IRenderSystem* pRenderSystem) : ManagedItem(name),m_pRenderSystem(pRenderSystem)
	{
		m_numFaces = 0;
		m_numVertices = 0;
		//m_pVBPosNormal = 0;
		m_pVBTexcoord = 0;
		m_pIB = 0;
		memset(m_pPrecomputedVBs,0, W3_PRECOMPUTED_VBO_COUNT_MAX /*8192*/ * sizeof(IVertexBuffer*));
		m_memorySize = 0;
	}

	ModelCodecWarcraft3::~ModelCodecWarcraft3()
	{
		for(int i = 0;i < W3_PRECOMPUTED_VBO_COUNT_MAX /*8192*/;i++)
		{
			safeRelease(m_pPrecomputedVBs[i]);
		}
		size_t size = m_vMaterials.size();
		for(size_t i = 0;i < size;i++)
		{
			STLDeleteSequence(m_vMaterials[i].layerContainer);
		}
		m_vMaterials.clear();

		safeRelease(m_pVBTexcoord);
		//safeRelease(m_pVBPosNormal);
		safeRelease(m_pIB);

		STLDeleteSequence(m_vTextureAnimations);
		STLDeleteSequence(m_vGeosets);
		STLDeleteSequence(m_vGeosetAnimations);
		STLDeleteSequence(m_vBoneExtras);
		STLDeleteSequence(m_vLights);
		STLDeleteSequence(m_vHelpers);
		STLDeleteSequence(m_vAttachments);
		STLDeleteSequence(m_vParticleEmitters);
		STLDeleteSequence(m_vParticleEmitter2s);
		STLDeleteSequence(m_vRibbonEmitters);
		STLDeleteSequence(m_vEventObjects);
		STLDeleteSequence(m_vCameras);
		STLDeleteSequence(m_vCollisionShapes);
	}

	size_t ModelCodecWarcraft3::getMomorySize()
	{
		return m_memorySize;
	}
	
	void ModelCodecWarcraft3::release()
	{
		delete this;
	}


	/*
	void * ModelCodecWarcraft3::getParticleSystemParam(unsigned int index, string paramname, string & type, int & count )
	 {
		//通用检测
		if( index >= m_vParticleEmitter2s.size() ) return 0;

		ParticleEmitter2Data & psdata = *getParticleSystemData(index);
		
		//标量参数
		if(paramname == "textureFilename") return aidGetPSParamVector( &psdata.textureFilename, "string",type,1,count);
		if( paramname == "filterMode")
		{
			type = "short";
			count = 1;
			short * preturn = new short[1];
			*preturn = static_cast<short>(psdata.filterMode);
			return preturn;
		}
		if( paramname == "rows") return aidGetPSParamVector(&psdata.rows, "int",type,1,count);
		if( paramname == "columns") return aidGetPSParamVector(&psdata.columns, "int",type,1,count);
		if( paramname == "boneObjectId") return aidGetPSParamVector(&psdata.boneObjectId, "int",type,1,count);
		if( paramname == "time") return aidGetPSParamVector(&psdata.time, "float",type,1,count);
		if( paramname == "lifeSpan") return aidGetPSParamVector(&psdata.lifeSpan, "float",type,1,count);
		if( paramname == "tailLength") return aidGetPSParamVector(&psdata.tailLength, "float",type,1,count);

		//开关参数
		if( paramname == "head") return aidGetPSParamVector(&psdata.head, "bool",type,1,count);
		if( paramname == "tail") return aidGetPSParamVector(&psdata.tail, "bool",type,1,count);
		if( paramname == "unshaded") return aidGetPSParamVector(&psdata.unshaded, "bool",type,1,count);
		if( paramname == "unfogged") return aidGetPSParamVector(&psdata.unfogged, "bool",type,1,count);

		//帧参数
		if( paramname == "speed") return aidGetPSParamsKeyFrames(psdata, paramname, type, count);
		if( paramname == "variation") return aidGetPSParamsKeyFrames(psdata, paramname, type, count);
		if( paramname == "latitude") return aidGetPSParamsKeyFrames(psdata, paramname, type, count);
		if( paramname == "gravity") return aidGetPSParamsKeyFrames(psdata, paramname, type, count);
		if( paramname == "visibility") return aidGetPSParamsKeyFrames(psdata, paramname, type, count);
		if( paramname == "emissionRate") return aidGetPSParamsKeyFrames(psdata, paramname, type, count);
		if( paramname == "width") return aidGetPSParamsKeyFrames(psdata, paramname, type, count);
		if( paramname == "length") return aidGetPSParamsKeyFrames(psdata, paramname, type, count);

		//向量参数
		if( paramname == "segmentColor1") return aidGetPSParamVector(psdata.segmentColor1.val, "float",type,3,count);
		if( paramname == "segmentColor2") return aidGetPSParamVector(psdata.segmentColor2.val, "float",type,3,count);
		if( paramname == "segmentColor3") return aidGetPSParamVector(psdata.segmentColor3.val, "float",type,3,count);
		if( paramname == "alpha") return aidGetPSParamVector(psdata.alpha.val, "float",type,3,count);
		if( paramname == "particleScaling") return aidGetPSParamVector(psdata.particleScaling.val, "float",type,3,count);
		if( paramname == "headLifeSpan") return aidGetPSParamVector(psdata.headLifeSpan.val, "float",type,3,count);
		if( paramname == "headDecay") return aidGetPSParamVector(psdata.headDecay.val, "float",type,3,count);
		if( paramname == "tailLifeSpan") return aidGetPSParamVector(psdata.tailLifeSpan.val, "float",type,3,count);
		if( paramname == "tailDecay") return aidGetPSParamVector(psdata.tailDecay.val, "float",type,3,count);

		//未使用参数
		if( paramname == "textureId") return aidGetPSParamVector(&psdata.textureId, "int",type,1,count);
		if( paramname == "priorityPlane") return aidGetPSParamVector(&psdata.priorityPlane, "int",type,1,count);
		if( paramname == "replaceableId") return aidGetPSParamVector(&psdata.replaceableId, "int",type,1,count);
		if( paramname == "sortPrimitivesFarZ") return aidGetPSParamVector(&psdata.sortPrimitivesFarZ, "bool",type,1,count);
		if( paramname == "lineEmitter") return aidGetPSParamVector(&psdata.lineEmitter, "bool",type,1,count);
		if( paramname == "modelSpace") return aidGetPSParamVector(&psdata.modelSpace, "bool",type,1,count);
		if( paramname == "alphaKey") return aidGetPSParamVector(&psdata.alphaKey, "bool",type,1,count);
		if( paramname == "xyQuad") return aidGetPSParamVector(&psdata.xyQuad, "bool",type,1,count);
		if( paramname == "squirt") return aidGetPSParamVector(&psdata.squirt, "bool",type,1,count);

		//默认处理
		return 0;
	 }
	 */

	/*
	void * ModelCodecWarcraft3::aidGetPSParamVector( void * pParam, string paramtype, string & type, int paramcount, int & count)
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
	void * ModelCodecWarcraft3::aidGetPSParamsKeyFrames( ParticleEmitter2Data & psdata, string paraname,string & type, int & count)
	{

		//type一定是"float"
		map<string, KeyFrames<float> * > parammap;
		parammap.insert(pair< string,KeyFrames<float> * >("speed", &psdata.speed));
		parammap.insert(pair< string,KeyFrames<float> * >("variation", &psdata.variation ));
		parammap.insert(pair< string,KeyFrames<float> * >("latitude", &psdata.latitude));
		parammap.insert(pair< string,KeyFrames<float> * >("gravity", &psdata.gravity ));
		parammap.insert(pair< string,KeyFrames<float> * >("visibility", &psdata.visibility ));
		parammap.insert(pair< string,KeyFrames<float> * >("emissionRate", &psdata.emissionRate ));
		parammap.insert(pair< string,KeyFrames<float> * >("width", &psdata.width ));
		parammap.insert(pair< string,KeyFrames<float> * >("length", &psdata.length ));

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
	bool ModelCodecWarcraft3::setParticleSystemParam(unsigned int index, string desname, void * psrc, int srcnum, string srctype)
	{
		//通用检测
		if( index >= m_vParticleEmitter2s.size() ) return false;
		if( psrc == 0) return false;
		if( srcnum <= 0 ) return false;

		ParticleEmitter2Data & psdata = *getParticleSystemData(index);

		//标量参数
		//if(desname == "textureFilename") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.textureFilename, 1,"string");
		if(desname == "textureFilename") return false;
		if(desname == "filterMode")
		{
			if( srcnum != 1 || srctype != "short" ) return false;
			short * pshort = (short *) psrc;
			if( *pshort < 0 || *pshort > 5) return false;
			psdata.filterMode = static_cast<BlendMode>(pshort[0]);
			return true;
		}
		if(desname == "rows") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.rows, 1,"int");
		if(desname == "columns") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.columns, 1,"int");
		if(desname == "boneObjectId")
		{
			return false;
		}
		if(desname == "time") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.time, 1,"float");
		if(desname == "lifeSpan") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.lifeSpan, 1,"float");
		if(desname == "tailLength") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.tailLength, 1,"float");
		

		//开关参数
		if(desname == "head") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.head, 1,"bool");
		if(desname == "tail") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.tail, 1,"bool");
		if(desname == "unshaded") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.unshaded, 1,"bool");
		if(desname == "unfogged") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.unfogged, 1,"bool");


		//帧参数
		map< string , KeyFrames<float> * >  mapKF;
		mapKF.insert(pair<string, KeyFrames<float> *> ( string("speed"), & psdata.speed) );
		mapKF.insert(pair<string, KeyFrames<float> *> ( string("variation"), & psdata.variation) );
		mapKF.insert(pair<string, KeyFrames<float> *> ( string("latitude"), & psdata.latitude) );
		mapKF.insert(pair<string, KeyFrames<float> *> ( string("gravity"), & psdata.gravity) );
		mapKF.insert(pair<string, KeyFrames<float> *> ( string("visibility"), & psdata.visibility) );
		mapKF.insert(pair<string, KeyFrames<float> *> ( string("emissionRate"), & psdata.emissionRate) );
		mapKF.insert(pair<string, KeyFrames<float> *> ( string("width"), & psdata.width) );
		mapKF.insert(pair<string, KeyFrames<float> *> ( string("length"), & psdata.length) );
		map<string, KeyFrames<float> * >::iterator itKF = mapKF.find(desname);
		if (  itKF != mapKF.end() )
		{
			return setPSKeyFramesParam( itKF->second, static_cast<float*>(psrc), srcnum,srctype);
		}

		//向量参数
		if(desname == "segmentColor1") return aidSetPSParamVector(psrc,srcnum,srctype,psdata.segmentColor1.val, 3,"float",true,0,1);
		if(desname == "segmentColor2") return aidSetPSParamVector(psrc,srcnum,srctype,psdata.segmentColor2.val, 3,"float",true,0,1);
		if(desname == "segmentColor3") return aidSetPSParamVector(psrc,srcnum,srctype,psdata.segmentColor3.val, 3,"float",true,0,1);
		if(desname == "alpha") return aidSetPSParamVector(psrc,srcnum,srctype,psdata.alpha.val, 3,"float");
		if(desname == "particleScaling") return aidSetPSParamVector(psrc,srcnum,srctype,psdata.particleScaling.val, 3,"float");
		if(desname == "headLifeSpan") return aidSetPSParamVector(psrc,srcnum,srctype,psdata.headLifeSpan.val, 3,"float");
		if(desname == "headDecay") return aidSetPSParamVector(psrc,srcnum,srctype,psdata.headDecay.val, 3,"float");
		if(desname == "tailLifeSpan") return aidSetPSParamVector(psrc,srcnum,srctype,psdata.tailLifeSpan.val, 3,"float");
		if(desname == "tailDecay") return aidSetPSParamVector(psrc,srcnum,srctype,psdata.tailDecay.val, 3,"float");


		//if(desname == "textureId") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.textureId, 1,"int");
		//if(desname == "priorityPlane") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.priorityPlane, 1,"int");
		//if(desname == "replaceableId") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.replaceableId, 1,"int");
		//if(desname == "sortPrimitivesFarZ") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.sortPrimitivesFarZ, 1,"bool");
		//if(desname == "lineEmitter") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.lineEmitter, 1,"bool");
		//if(desname == "modelSpace") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.modelSpace, 1,"bool");
		//if(desname == "alphaKey") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.alphaKey, 1,"bool");
		//if(desname == "xyQuad") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.xyQuad, 1,"bool");
		//if(desname == "squirt") return aidSetPSParamVector(psrc,srcnum,srctype,&psdata.squirt, 1,"bool");


		if(desname == "textureId") return false;
		if(desname == "priorityPlane") return false;
		if(desname == "replaceableId") return false;
		if(desname == "sortPrimitivesFarZ") return false;
		if(desname == "lineEmitter") return false;
		if(desname == "modelSpace") return false;
		if(desname == "alphaKey") return false;
		if(desname == "xyQuad") return false;
		if(desname == "squirt") return false;

		//默认处理
		return false;
	}
	*/

	/*
	bool ModelCodecWarcraft3::aidSetPSParamVector(void * psrc, int srcnum, string srctype, void* pdes, int desnum, string destype, bool enablerange, float min , float max  )
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
					if( ssrc[i] < (short)( min + 0.5) || ssrc[i] > (short)(max+0.5) ) return false;
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
					if( isrc[i] < (int)( min+0.5) || isrc[i] > (int)(max+0.5) ) return false;
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
	bool ModelCodecWarcraft3::setPSKeyFramesParam(KeyFrames<float> * pkf, float * pParamVal, int iParamNum, string szParamType)
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
	bool ModelCodecWarcraft3::addParticleSystem()
	{	
		return false;	
	}
	*/

	/*
	bool ModelCodecWarcraft3::removeParticleSystem(unsigned int index)
	{
		return false;
	}
	*/

	/*
	bool ModelCodecWarcraft3::write(string filename)
	{
		return false;
	}
	*/
}