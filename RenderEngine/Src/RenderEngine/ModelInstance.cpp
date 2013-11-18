#include "StdAfx.h"
#include "ModelInstance.h"
#include "Camera.h"
#include "RenderQueue.h"
#include "Animation.h"
#include "SubModelInstance.h"
#include "Skeleton.h"
#include "ParticleSystem.h"
#include "RibbonSystem.h"
#include "_ParticleSystem.h"

namespace xs
{
	ModelInstance::ModelInstance() : m_pModel(0),	
		m_pCurrentAnimation(0),
		m_speed(1),
		m_pNode(0),
		m_pVBVertexBlend(0),
		m_pIB(0),
		m_pVBTexcoord(0),
		m_pVBBlendWeight(0),
		m_pBlendShaderProgram(0),
		m_needDrawShadow(true)
	{
		m_loopTimes = -1;
		m_pCallback = 0;
		m_lastUpdateTime = 0;
	}

	ModelInstance::~ModelInstance()
	{
		STLDeleteSequence(m_vRibbonSystem);
		STLDeleteSequence(m_vParticleSystem);
		STLDeleteSequence(m_vMmParticleSystem);
		STLDeleteSequence(m_subModels);
		IModelManager *pModelManager = getRenderEngine()->getModelManager();
		pModelManager->releaseModel(m_pModel);
	}

	//void ModelInstance::loadBone(Bone *pBone,BoneData *pData)
	//{
	//	if(pData->billboarded)
	//		pBone->setBillboard(BILLBOARD_LOCK_NONE);
	//	else if(pData->billboardedLockX)
	//		pBone->setBillboard(BILLBOARD_LOCK_X);
	//	else if(pData->billboardedLockY)
	//		pBone->setBillboard(BILLBOARD_LOCK_Y);
	//	else if(pData->billboardedLockZ)
	//		pBone->setBillboard(BILLBOARD_LOCK_Z);
	//	else 
	//		pBone->setBillboard(BILLBOARD_NOT_USED);
	//	pBone->setName(pData->name);

	//	pBone->setPivot(pData->pivotPoint);
	//	pBone->setPivotRotation(pData->pivotRotation);

	//	//设置骨骼的初始变换矩阵，用于绑定其它节点，目前只设定位移和旋转变换
	//	Matrix4 mtx = Matrix4::IDENTITY;
	//	mtx.makeTrans(pData->initTrans);
	//	mtx = mtx * pData->initQuat;
	//	//Matrix4 _mtx_tmp = Matrix4::IDENTITY;
	//	//_mtx_tmp.setScale(pData->initScale);
	//	//mtx = mtx * _mtx_tmp;
	//	pBone->setInitMatrix(mtx);

	//	//pBone->setInterpolationTypeTranslation(pData->translation.getInterpolationType());
	//	//pBone->setInterpolationTypeRotation(pData->rotation.getInterpolationType());
	//	//pBone->setInterpolationTypeScale(pData->scale.getInterpolationType());

	//	pBone->setKeyFramesTranslation(pData->translation);
	//	pBone->setKeyFramesRotation(pData->rotation);
	//	pBone->setKeyFramesScale(pData->scale);
	//	pBone->m_pMKTM = &pData->m_TMVector;
	//}

	bool ModelInstance::setModel(const char* modelName,ModelNode *pNode)
	{
		IModelManager *pModelManager = getRenderEngine()->getModelManager();
		IModel *pModel = pModelManager->loadModel(modelName);
		if(!pModel)return false;

		m_pModel = pModel;
		m_pNode = pNode;

		//设置buffer		
		m_pIB = m_pModel->getIndexBuffer();
		m_pVBTexcoord = m_pModel->getTexcoordBuffer();
		m_pVBBlendWeight = m_pModel->getBlendWeightBuffer();
		m_pBlendShaderProgram = m_pModel->getBlendShaderProgram();//需要再构造SubModelInstance前设置
		m_needDrawShadow = m_pModel->needDrawShadow();

		//创建子mesh
		uint numSubModels = m_pModel->getNumSubModels();
		for(uint i = 0;i < numSubModels;i++)
		{
			ISubModel *pSubModel = m_pModel->getSubModel(i);
			SubModelInstance *pSubModelInstance = new SubModelInstance(this,pSubModel, i);
			m_subModels.push_back(pSubModelInstance);
			m_vBlendIndices.push_back( m_pModel->getBlendIndicesBuffer(i));//保存骨骼混合的索引buffer
		}

		//创建骨骼树
		if( !m_skeleton.create(m_pModel->getCoreSkeletion() ) )
		{
			return false;
		}
		//uint numBones = m_pModel->numBones();
		//for(uint i = 0;i < numBones;i++)
		//{
		//	BoneData *pData = m_pModel->getBone(i);
		//	Bone *pBone = m_skeleton.createBone(m_pNode);
		//	loadBone(pBone,pData);
		//}
		////骨骼树形结构构建
		//for(size_t i = 0;i < numBones;i++)
		//{
		//	BoneData *pBone = m_pModel->getBone(i);
		//	if(pBone->parentId != -1)
		//	{
		//		for(size_t j = 0;j < numBones;j++)
		//		{
		//			BoneData *pParent = m_pModel->getBone(j);
		//			if(pBone->parentId == pParent->objectId)
		//			{
		//				m_skeleton.getBone(i)->setParent(m_skeleton.getBone(j));
		//				break;
		//			}
		//		}
		//	}
		//	m_skeleton.getBone(i)->setPrecomputeMatrix(pBone->precomputeMtx);
		//}

		//ParticleSystem
		size_t size = m_pModel->numParticleSystem();
		for(size_t j = 0;j < size;j++)
		{
			ParticleSystem *pParticleSystem = new ParticleSystem;
			pParticleSystem->m_pData = m_pModel->getParticleSystemData(j);
			pParticleSystem->m_pBone = m_skeleton.getBone(pParticleSystem->m_pData->boneObjectId);
			pParticleSystem->m_pNode = m_pNode;
			m_vParticleSystem.push_back(pParticleSystem);
		}

		//RibbonSystem
		size = m_pModel->numRibbonSystem();
		for(size_t j = 0;j < size;j++)
		{
			RibbonSystem *pRibbonSystem = new RibbonSystem;
			pRibbonSystem->m_pData = m_pModel->getRibbonSystemData(j);
			pRibbonSystem->m_pBone = m_skeleton.getBone(pRibbonSystem->m_pData->boneObjectId);
			pRibbonSystem->m_pNode = m_pNode;
			m_vRibbonSystem.push_back(pRibbonSystem);
		}

		//MmParticleSystem
		size = m_pModel->numMmParticleSystem();
		for(size_t j = 0;j < size;j++)
		{
			MmParticleSystem *pMmPs = new MmParticleSystem;
			pMmPs->m_pData = m_pModel->getMmParticleSystem(j);
			pMmPs->m_pNode = m_pNode;
			pMmPs->m_pBone = m_skeleton.getBone(pMmPs->m_pData->bone);
			m_vMmParticleSystem.push_back(pMmPs);
		}

		if(m_pModel->getAnimationCount())
			setCurrentAnimation(pModel->getAnimation(0)->getName());

		return true;
	}

	ModelNode* ModelInstance::getModelNode()
	{
		return m_pNode;
	}

    const AABB& ModelInstance::getAABB()
	{
		return m_pModel->getAABB();
	}

	const Sphere& ModelInstance::getBoundingSphere()
	{
		return m_pModel->getBoudingSphere();
	}

	void  ModelInstance::updateRenderQueue(RenderQueue *pRenderQueue)
	{
		if( !pRenderQueue) return;

		if( m_pModel && m_pModel->isEnabledCustomizeRenderOrder() )
		{
			std::map<int, IRenderable *> mapRenderOrder;

			for( uint i =0; i < m_subModels.size(); ++i)
			{
				if( m_subModels[i]->isVisible() )
					mapRenderOrder[m_pModel->getCustomizeRenderOrder(IModel::ECustomizeRenderOrderObject_SubModel,i)] = m_subModels[i];
			}

			for( uint i = 0; i < m_vParticleSystem.size(); ++i)
			{
				mapRenderOrder[m_pModel->getCustomizeRenderOrder(IModel::ECustomizeRenderOrderObject_ParticleSystem,i)] = m_vParticleSystem[i];
			}

			for( uint i =0; i < m_vRibbonSystem.size(); ++i)
			{
				mapRenderOrder[m_pModel->getCustomizeRenderOrder(IModel::ECustomizeRenderOrderObject_RibbonSystem,i)] = m_vRibbonSystem[i];
			}
			
			for( uint i = 0; i < m_vMmParticleSystem.size(); ++i)
			{
				mapRenderOrder[m_pModel->getCustomizeRenderOrder(IModel::ECustomizeRenderOrderObject_MMParticleSystem,i)] = m_vMmParticleSystem[i];
			}

			std::map<int, IRenderable *>::iterator it = mapRenderOrder.begin();
			for( ; it != mapRenderOrder.end(); ++it) pRenderQueue->add( (*it).second,RENDER_QUEUE_OPAQUE);
		}
		else
		{
			SubModelList::iterator begin = m_subModels.begin();
			SubModelList::iterator end = m_subModels.end();
			while(begin != end)
			{
				SubModelInstance* pSubModel = (*begin);
				if(pSubModel->isVisible())
				{
					pRenderQueue->add(pSubModel,pSubModel->getRenderOrder());
				}
				++begin;
			}
			ParticleSystemList::iterator b = m_vParticleSystem.begin();
			ParticleSystemList::iterator e = m_vParticleSystem.end();
			while(b != e)
			{
				ParticleSystem* pParticleSystem = (*b);
				pRenderQueue->add(pParticleSystem,pParticleSystem->getRenderOrder());
				++b;
			}
			RibbonSystemList::iterator be = m_vRibbonSystem.begin();
			RibbonSystemList::iterator ee = m_vRibbonSystem.end();
			while(be != ee)
			{
				RibbonSystem* pRibbonSystem = (*be);
				pRenderQueue->add(pRibbonSystem,pRibbonSystem->getRenderOrder());
				++be;
			}
			MmParticleSystemList::iterator mb = m_vMmParticleSystem.begin();
			MmParticleSystemList::iterator me = m_vMmParticleSystem.end();
			while(mb != me)
			{
				MmParticleSystem* pParticleSystem = (*mb);
				pRenderQueue->add(pParticleSystem,pParticleSystem->getRenderOrder());
				++mb;
			}
		}
	}
	
	void ModelInstance::updateColor(IRenderSystem* pRenderSystem,const ColorValue& diffuse)
	{
		SubModelList::iterator begin = m_subModels.begin();
		SubModelList::iterator end = m_subModels.end();
		while(begin != end)
		{
			SubModelInstance* pSubModel = (*begin);
			pSubModel->updateColor(diffuse);
			++begin;
		}
		ParticleSystemList::iterator b = m_vParticleSystem.begin();
		ParticleSystemList::iterator e = m_vParticleSystem.end();
		while(b != e)
		{
			ParticleSystem* pParticleSystem = (*b);
			pParticleSystem->setColor(diffuse);
			++b;
		}
		RibbonSystemList::iterator be = m_vRibbonSystem.begin();
		RibbonSystemList::iterator ee = m_vRibbonSystem.end();
		while(be != ee)
		{
			RibbonSystem* pRibbonSystem = (*be);
			pRibbonSystem->setColor(diffuse);
			++be;
		}
		MmParticleSystemList::iterator mb = m_vMmParticleSystem.begin();
		MmParticleSystemList::iterator me = m_vMmParticleSystem.end();
		while(mb != me)
		{
			MmParticleSystem* pParticleSystem = (*mb);
			pParticleSystem->setColor(diffuse);
			++mb;
		}
	}

	void ModelInstance::advanceTime(float tickDelta)
	{
		if(!m_pCurrentAnimation)return;

		uint timeStart = m_pCurrentAnimation->getTimeStart();
		uint timeEnd = m_pCurrentAnimation->getTimeEnd();
		uint timeLen = timeEnd - timeStart;

		if(m_loopTimes == 0)
		{
			m_currentTime.current = timeEnd;
			return;
		}

		m_currentTime.current += m_speed * tickDelta;
		while(m_currentTime.current >= timeEnd)
		{
			if(m_loopTimes == -1)
			{
				if(m_pCallback)
				{
					m_pCallback->onOneCycle(m_currentAnimation,m_loopTimes);
					if(timeStart != m_currentTime.start)
					{
						return;
					}
				}
				m_currentTime.current -= (timeEnd - timeStart);
			}
			else if(m_loopTimes > 0)
			{
				m_loopTimes--;
				if(m_pCallback)
				{
					m_pCallback->onOneCycle(m_currentAnimation,m_loopTimes);
					if(timeStart != m_currentTime.start)
					{
						return;
					}
				}
				//如果循环次数为0，则要设置成动作结束，否则模型会出现一帧到开始的画面
				if( m_loopTimes == 0 ) m_currentTime.current =timeEnd;
				else m_currentTime.current -= (timeEnd - timeStart);
			}
			else
			{
				m_currentTime.current = timeEnd;
				return;
			}
		}
	}

	void  ModelInstance::update(float tick,float tickDelta,IRenderSystem* pRenderSystem,const ColorValue& diffuse)
	{
		PP_BY_NAME("ModelInstance::update");

		advanceTime(tickDelta);

		//uint numBones = m_skeleton.getBoneCount();
		//if(!numBones)return;

		ulong now = xs::getTickCount();	
		// 这里33ms的检查会导致跳帧，先屏蔽掉，如果对游戏效率有影响再打开这个限制 [4/27/2011 zgz]
		//if(m_lastUpdateTime == 0 || now - m_lastUpdateTime >= 33)
		{
			float lastUpdateTime = m_lastUpdateTime;
			m_lastUpdateTime = now;
			

			PP_BY_NAME_START("ModelInstance::update::m_skeleton.calcMatrix");
			//if(m_pCurrentAnimation)m_skeleton.calcMatrix(pRenderSystem,&m_currentTime,m_pCurrentAnimation->getRangeIndex());
			if(m_pCurrentAnimation)m_skeleton.calcMatrix(&m_currentTime,m_pCurrentAnimation->getRangeIndex());
			PP_BY_NAME_STOP();
			float tickDelta2 = now - lastUpdateTime;

			PP_BY_NAME_START("ModelInstance::update::pRibbonSystem->update");
			RibbonSystemList::iterator be = m_vRibbonSystem.begin();
			RibbonSystemList::iterator ee = m_vRibbonSystem.end();
			while(be != ee)
			{
				RibbonSystem* pRibbonSystem = (*be);
				pRibbonSystem->setColor(diffuse);
				pRibbonSystem->update(&m_currentTime,/*tickDelta*/tickDelta2,pRenderSystem);
				++be;
			}
			PP_BY_NAME_STOP();

			
			if( m_pBlendShaderProgram )//如果能够硬件混合
			{
				PP_BY_NAME_START("ModelInstance::update::pSubModel->updateBonesMatrix");
				SubModelList::iterator begin = m_subModels.begin();
				SubModelList::iterator end = m_subModels.end();
				while(begin != end)
				{
					SubModelInstance* pSubModel = (*begin);
					pSubModel->updateBonesMatrix(&m_skeleton);
					++begin;
				}
				m_pVBVertexBlend = m_pModel->getOriginVertexBuffer();
				PP_BY_NAME_STOP();
			}
			else
			{
				PP_BY_NAME_START("ModelInstance::update::m_pModel->vertexBlend");
				m_pVBVertexBlend = m_pModel->vertexBlend(&m_skeleton,&m_currentTime);
				PP_BY_NAME_STOP();
			}

			PP_BY_NAME_START("ModelInstance::update::m_pModel->texBlend");
			m_pModel->texBlend(&m_currentTime);//added for uv animation 2009/07/24
			PP_BY_NAME_STOP();
			
			PP_BY_NAME_START("ModelInstance::update::pSubModel->update");
			SubModelList::iterator begin = m_subModels.begin();
			SubModelList::iterator end = m_subModels.end();
			while(begin != end)
			{
				SubModelInstance* pSubModel = (*begin);
				pSubModel->update(&m_currentTime,diffuse);
				++begin;
			}
			PP_BY_NAME_STOP();				
		}		

		PP_BY_NAME_START("ModelInstance::update::pParticleSystem->update");
		ParticleSystemList::iterator b = m_vParticleSystem.begin();
		ParticleSystemList::iterator e = m_vParticleSystem.end();
		while(b != e)
		{
			ParticleSystem* pParticleSystem = (*b);
			pParticleSystem->setColor(diffuse);
			pParticleSystem->update(&m_currentTime,tickDelta,pRenderSystem);
			++b;
		}
		PP_BY_NAME_STOP();

		PP_BY_NAME_START("ModelInstance::update::pMmParticleSystem->update");
		MmParticleSystemList::iterator mb = m_vMmParticleSystem.begin();
		MmParticleSystemList::iterator me = m_vMmParticleSystem.end();
		while(mb != me)
		{
			MmParticleSystem* pParticleSystem = (*mb);
			pParticleSystem->setColor(diffuse);
			pParticleSystem->update(pRenderSystem, &m_currentTime,tickDelta,-1);
			++mb;
		}
		PP_BY_NAME_STOP();
	}

	//void ModelInstance::render(IRenderSystem* pRenderSystem,ModelNode *pNode)
	//{
	//	pRenderSystem->setIndexBuffer(m_pIB);
	//	pRenderSystem->setVertexVertexBuffer(m_pVBVertexBlend);
	//	pRenderSystem->setNormalVertexBuffer(m_pVBVertexBlend,getNumVertices() * 12);
	//	pRenderSystem->setTexcoordVertexBuffer(0,m_pVBTexcoord);

	//	SubModelList::iterator begin = m_subModels.begin();
	//	SubModelList::iterator end = m_subModels.end();
	//	while(begin != end)
	//	{
	//		SubModelInstance *pSubModel = (*begin);
	//		if(pSubModel->isVisible())
	//		{
	//			pSubModel->beforeRender(pRenderSystem,pNode);

	//			pSubModel->render(pRenderSystem);

	//			pSubModel->afterRender(pRenderSystem);
	//		}
	//		++begin;
	//	}

	//	pRenderSystem->setIndexBuffer(0);
	//	pRenderSystem->setVertexVertexBuffer(0);
	//	pRenderSystem->setNormalVertexBuffer(0);
	//	pRenderSystem->setTexcoordVertexBuffer(0,0);
	//}

	void	ModelInstance::setAnimationSpeed(float speed)
	{
		m_speed = speed;
	}

	bool ModelInstance::setCurrentAnimation(const std::string& animation,int loopTimes)
	{
		Assert(loopTimes != 0);

		m_loopTimes = loopTimes;
		if(animation.empty())return false;
		Animation *pAnimation = m_pModel->hasAnimation(animation);
		if(!pAnimation)return false;

		if(m_currentAnimation != animation || loopTimes>0)
		{
			m_currentAnimation = animation;
			m_pCurrentAnimation = pAnimation;

			m_currentTime.start = pAnimation->getTimeStart();
			m_currentTime.end = pAnimation->getTimeEnd();
			m_currentTime.current = m_currentTime.start;
		}

		return true;
	}

	// add by zjp；获取当前的动作的ticks
	int				ModelInstance::getAnimationTicks(const std::string& animation)
	{
		if(animation.empty())return false;
		Animation *pAnimation = m_pModel->hasAnimation(animation);
		if(!pAnimation)
			return 0;
		int nTicks = pAnimation->getTimeEnd()-pAnimation->getTimeStart();
		return nTicks;
	}

	void			ModelInstance::setAnimationTime(uint time)
	{
		m_currentTime.current = time;
		if(m_currentTime.current < m_currentTime.start)m_currentTime.current = m_currentTime.start;
		if(m_currentTime.current > m_currentTime.end)m_currentTime.current = m_currentTime.end;
	}
	
	AnimationTime*			ModelInstance::getAnimationTime()
	{
		return &m_currentTime;
	}

	const std::string& ModelInstance::getCurrentAnimation()
	{
		return m_currentAnimation;
	}

	Animation* ModelInstance::_getCurrentAnimation()
	{
		return m_pCurrentAnimation;
	}

	Bone*	ModelInstance::getBone(const char*boneName)
	{
		return m_skeleton.getBone(boneName);
	}

	uint ModelInstance::getAnimationCount()
	{
		return m_pModel->getAnimationCount();
	}

	Animation* ModelInstance::getAnimation(uint index)
	{
		return m_pModel->getAnimation(index);
	}

	uint ModelInstance::getBoneCount()
	{
		return m_skeleton.getBoneCount();
	}

	Bone* ModelInstance::getBone(int index)
	{
		return m_skeleton.getBone(index);
	}

	const std::string& ModelInstance::getFileName()
	{
		return m_pModel->getFileName();
	}

	uint ModelInstance::getNumFaces()
	{
		return m_pModel->getNumFaces();
	}

	uint ModelInstance::getNumVertices()
	{
		return m_pModel->getNumVertices();
	}

	uint ModelInstance::getNumSubModels()
	{
		return m_pModel->getNumSubModels();
	}

	SubModelInstance*	ModelInstance::getSubModel(const char* subModelName)
	{
		SubModelList::iterator begin = m_subModels.begin();
		SubModelList::iterator end = m_subModels.end();
		while(begin != end)
		{
			SubModelInstance *pSubModelInstance = (*begin);
			if(StringHelper::casecmp(pSubModelInstance->getName(),subModelName) == 0)
			{
				return pSubModelInstance;
			}
			++begin;
		}

		return 0;
	}

	SubModelInstance*	ModelInstance::getFirstSubModel()
	{
		m_iteratorSubModel = m_subModels.begin();
		return getNextSubModel();
	}

	SubModelInstance*	ModelInstance::getNextSubModel()
	{
		if(m_iteratorSubModel != m_subModels.end())
		{
			return (*m_iteratorSubModel++);
		}

		return 0;
	}

	uint ModelInstance::getVer()
	{
		return m_pModel->getVer();
	}

	void ModelInstance::showAllSubModels(bool show)
	{
		SubModelList::iterator begin = m_subModels.begin();
		SubModelList::iterator end = m_subModels.end();
		while(begin != end)
		{
			(*begin)->setVisible(show);
			++begin;
		}
	}

	IIndexBuffer*	ModelInstance::getIndexBuffer()
	{
		return m_pIB;
	}

	IVertexBuffer*	ModelInstance::getVertexBuffer()
	{
		return m_pVBVertexBlend;
	}

	IVertexBuffer*	ModelInstance::getVertexBufferTexcoord()
	{
		return m_pVBTexcoord;
	}

	IVertexBuffer*	ModelInstance::getBlendWeightBuffer()
	{
		return m_pVBBlendWeight;
	}

	IVertexBuffer*  ModelInstance::getBlendIndicesBuffer(uint subModelIndex)
	{
		if( subModelIndex >= m_vBlendIndices.size()) return 0;
		else return m_vBlendIndices[subModelIndex];
	}

	IShaderProgram*	ModelInstance::getBlendShaderProgram()
	{
		return m_pBlendShaderProgram;
	}

	bool ModelInstance::needDrawShadow()
	{
		return m_needDrawShadow;
	}

	void ModelInstance::resetFX()
	{
		ParticleSystemList::iterator b = m_vParticleSystem.begin();
		ParticleSystemList::iterator e = m_vParticleSystem.end();
		while(b != e)
		{
			ParticleSystem* pParticleSystem = (*b);
			pParticleSystem->reset();
			++b;
		}
		RibbonSystemList::iterator be = m_vRibbonSystem.begin();
		RibbonSystemList::iterator ee = m_vRibbonSystem.end();
		while(be != ee)
		{
			RibbonSystem* pRibbonSystem = (*be);
			pRibbonSystem->reset();
			++be;
		}
		MmParticleSystemList::iterator mb = m_vMmParticleSystem.begin();
		MmParticleSystemList::iterator me = m_vMmParticleSystem.end();
		while(mb != me)
		{
			MmParticleSystem* pParticleSystem = (*mb);
			pParticleSystem->reset();
			++mb;
		}
	}

	void ModelInstance::showRibbonSystem(bool bShow)
	{
		RibbonSystemList::iterator be = m_vRibbonSystem.begin();
		RibbonSystemList::iterator ee = m_vRibbonSystem.end();
		while(be != ee)
		{
			RibbonSystem* pRibbonSystem = (*be);
			pRibbonSystem->show(bShow);
			++be;
		}
	}

	size_t ModelInstance::getMemorySize()
	{
		//由于模型管理器已经管理了模型的内存，故资源管理器不需要再管理了，而且以前的管理方式也是错误的。没有考虑到共享
		size_t size =0; 
		size += sizeof(*this);
		size += m_subModels.size() * sizeof( SubModelInstance);
		size += m_vParticleSystem.size() * sizeof(ParticleSystem);
		size += m_vRibbonSystem.size() * sizeof(RibbonSystem);
		size += m_vMmParticleSystem.size() * sizeof(MmParticleSystem);
		return size;
	}

	int ModelInstance::GetParticleSystemCount()
	{
		return m_vParticleSystem.size();
	}
	int ModelInstance::getRibbonSystemCount()
	{
		return m_vRibbonSystem.size();
	}

	int ModelInstance::GetMMParticleSystemCount()
	{
		return m_vMmParticleSystem.size();
	}

	void * ModelInstance::getMMParticleSystemParam(unsigned int index, string paramname, string & type, int & count )
	{
		if(m_pModel == 0 ) return 0;
		else 
		{
			if( paramname == "animationtime" )
			{
				float * panitime  = new float[2];
				panitime[0] = (float)m_currentTime.start;
				panitime[1] = (float)m_currentTime.end;
				type = "float";
				count = 2;
				return panitime;
			}
			else if( paramname == "particlenum")
			{
				int * pint = new int[1];
				if( index >= m_vMmParticleSystem.size() ) pint[0] =0;
				else pint[0] = m_vMmParticleSystem[index]->__getCurParticleNum();
				type = "int";
				count = 1;
				return pint;
			}
			else
			{
				return m_pModel->getMMParticleSystemParam(index,paramname,type,count);
			}
		}
	}


	bool ModelInstance::setAllPSCurAnimationTime( uint aniStart, uint aniEnd)
	{
		if( aniStart < 0 || aniEnd <0 || aniStart > aniEnd ) return false;
		int curAniIndex = -1;
		for(uint i = 0 ; i <  m_pModel->getAnimationCount() ; i++)
		{
			if(  ModelInstance::m_pCurrentAnimation == m_pModel->getAnimation(i) ) 
			{
				curAniIndex = i;
				break;
			}
		}
		if( curAniIndex  < 0 ) return false;
		
		if( curAniIndex > 0 && aniStart < m_pModel->getAnimation( curAniIndex-1)->getTimeEnd() ) return false;
		
		m_pModel->getAnimation( curAniIndex)->setTimeStart( aniStart);
		m_pModel->getAnimation(curAniIndex)->setTimeEnd(aniEnd);

		uint formerend = m_currentTime.end;

		uint curInterval =  (uint)( (float)(m_currentTime.current-m_currentTime.start)*( (float)( aniEnd-aniStart)/ (float)(m_currentTime.end - m_currentTime.start )) );
		m_currentTime.start = aniStart;
		m_currentTime.current = aniStart + curInterval;
		m_currentTime.end = aniEnd;

		for( uint i = curAniIndex + 1; i < m_pModel->getAnimationCount(); i++)
		{
			uint outinterval = m_pModel->getAnimation(i)->getTimeStart() - formerend;
			uint ininterval = m_pModel->getAnimation(i)->getTimeEnd() - m_pModel->getAnimation(i)->getTimeStart();
			m_pModel->getAnimation(i)->setTimeStart( outinterval + m_pModel->getAnimation(i-1)->getTimeEnd() );
			formerend = m_pModel->getAnimation(i)->getTimeEnd();
			m_pModel->getAnimation(i)->setTimeEnd( m_pModel->getAnimation(i)->getTimeStart() + ininterval );
		}
		return true;	
	}

	bool ModelInstance::setMMParticleSystemParam(unsigned int index, string desname, void * psrc, int srcnum, string srctype)
	{
		if( m_pModel == 0 ) return false;
		else
		{
			//设置动画时间
			if( desname == "animationtime" )
			{
				//检测
				if( psrc == 0 || srcnum != 2 || srctype != "float" ) return false;
				uint aniStart = static_cast< uint >(( ( float*)psrc)[0]);
				uint aniEnd = static_cast< uint >(( ( float*)psrc)[1]);
				return setAllPSCurAnimationTime(aniStart, aniEnd);
			} 
			else if( m_pModel->setMMParticleSystemParam(index,desname,psrc,srcnum, srctype ) == true) //其它设置
			{
				//过滤
				if( desname == "bone")
				{
					m_vMmParticleSystem[index]->m_pBone = m_skeleton.getBone(m_vMmParticleSystem[index]->m_pData->bone);
				}
				//通用操作
				m_vMmParticleSystem[index]->Clear();
				return true;
			}
			else 
			{
				return false;
			}
		}
	}
	bool ModelInstance::addMMParticleSystem()
	{
		if(m_pModel == 0 || m_pNode == 0) return false;
		else
		{
			if( m_pModel->addMMParticleSystem() == true )
			{
				//重新初始化，防止vector位移
				MmParticleSystemList::iterator it = m_vMmParticleSystem.begin();
				for( ; it != m_vMmParticleSystem.end(); ++it)
				{
					delete (*it);
				}
				m_vMmParticleSystem.clear();
				

				uint size = m_pModel->numMmParticleSystem();
				for( uint count = 0; count < size; count++)
				{
					MmParticleSystem * pmmps = new MmParticleSystem();
					pmmps->m_pData = m_pModel->getMmParticleSystem(count);
					pmmps->m_pNode = m_pNode;
					if(pmmps->m_pData->bone >= 0 ) pmmps->m_pBone = m_skeleton.getBone(pmmps->m_pData->bone);
					else pmmps->m_pBone = 0;
					pmmps->Clear();
					m_vMmParticleSystem.push_back(pmmps);
				}
				return true;
			}
			else 
			{
				return false;
			}	
		}
	}
	bool ModelInstance::removeMMParticleSystem(unsigned int index)
	{
		if( m_pModel == 0 ) return false;
		else
		{
			if( m_pModel->removeMMParticleSystem(index) == true )
			{
				//重新加载
				MmParticleSystemList::iterator it = m_vMmParticleSystem.begin();
				for( ; it != m_vMmParticleSystem.end(); ++it )
				{
					delete (*it);
				}
				m_vMmParticleSystem.clear();

				uint size = m_pModel->numMmParticleSystem();
				for( uint count = 0; count < size; count++)
				{
					MmParticleSystem * pmmps = new MmParticleSystem();
					pmmps->m_pData = m_pModel->getMmParticleSystem(count);
					pmmps->m_pNode = m_pNode;
					if(pmmps->m_pData->bone >= 0 ) pmmps->m_pBone = m_skeleton.getBone(pmmps->m_pData->bone);
					else pmmps->m_pBone = 0;
					pmmps->Clear();
					m_vMmParticleSystem.push_back(pmmps);
				}
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	void * ModelInstance::getParticleSystemParam(unsigned int index, string paramname, string & type, int & count )
	{
		if(m_pModel == 0 ) return 0;
		else
		{
			if( paramname == "animationtime")
			{
				float * panitime  = new float[2];
				panitime[0] = (float)m_currentTime.start;
				panitime[1] = (float)m_currentTime.end;
				type = "float";
				count = 2;
				return panitime;
			}
			if( paramname == "particlenum" )
			{
				int * pint = new int[1];
				if( index >= m_vParticleSystem.size() ) pint[0] = 0;
				else pint[0] = m_vParticleSystem[index]->__getCurParticleNum();
				type = "int";
				count = 1;
				return pint;
			}
			else
			{
				return m_pModel->getParticleSystemParam(index,paramname,type,count);
			}
		}
	}

	void * ModelInstance::getRibbonSystemParam( unsigned int index, string paramname, string & type, int &count)
	{
		if(m_pModel == 0 ) return 0;
		else
		{
			if( paramname == "animationtime")
			{
				float * panitime  = new float[2];
				panitime[0] = (float)m_currentTime.start;
				panitime[1] = (float)m_currentTime.end;
				type = "float";
				count = 2;
				return panitime;
			}
			else
			{
				return m_pModel->getRibbonSystemParam(index,paramname,type,count);
			}
		}
	}

	bool ModelInstance::setParticleSystemParam(unsigned int index, string desname, void * psrc, int srcnum, string srctype)
	{
		if( m_pModel == 0 ) return false;
		else
		{
			//设置动画时间
			if( desname == "animationtime" )
			{
				//检测
				if( psrc == 0 || srcnum != 2 || srctype != "float" ) return false;
				uint aniStart = static_cast< uint >(( ( float*)psrc)[0]);
				uint aniEnd = static_cast< uint >(( ( float*)psrc)[1]);
				return setAllPSCurAnimationTime(aniStart, aniEnd);
			} 
			else if( m_pModel->setParticleSystemParam(index,desname,psrc,srcnum, srctype ) == true)
			{
				//过滤
				if( desname == "boneObjectId")
				{
					m_vParticleSystem[index]->m_pBone = m_skeleton.getBone(m_vParticleSystem[index]->m_pData->boneObjectId);
				}
				//通用操作
				m_vParticleSystem[index]->Clear();
				return true;
			}
			else 
			{
				return false;
			}
		}
	}
	bool ModelInstance::setRibbonSystemParam( unsigned int index, string desname, void * psrc, int srcnum, string srctype)
	{
		if( m_pModel == 0 ) return false;
		else
		{
			//设置动画时间
			if( desname == "animationtime" )
			{
				//检测
				if( psrc == 0 || srcnum != 2 || srctype != "float" ) return false;
				uint aniStart = static_cast< uint >(( ( float*)psrc)[0]);
				uint aniEnd = static_cast< uint >(( ( float*)psrc)[1]);
				return setAllPSCurAnimationTime(aniStart, aniEnd);
			} 
			else if( m_pModel->setRibbonSystemParam(index,desname,psrc,srcnum, srctype ) == true)
			{
				//过滤
				if( desname == "boneObjectId")
				{
					m_vRibbonSystem[index]->m_pBone = m_skeleton.getBone(m_vRibbonSystem[index]->m_pData->boneObjectId);
				}
				//通用操作
				m_vRibbonSystem[index]->Clear();
				return true;
			}
			else 
			{
				return false;
			}
		}
		
	}
	bool ModelInstance::addParticleSystem()
	{
		if(m_pModel == 0 || m_pNode == 0) return false;
		else
		{
			if( m_pModel->addParticleSystem() == true )
			{
				//重新初始化，防止vector位移
				ParticleSystemList::iterator it = m_vParticleSystem.begin();
				for( ; it != m_vParticleSystem.end(); ++it)
				{
					delete (*it);
				}
				m_vParticleSystem.clear();
				

				uint size = m_pModel->numParticleSystem();
				for( uint count = 0; count < size; count++)
				{
					ParticleSystem * pps = new ParticleSystem();
					pps->m_pData = m_pModel->getParticleSystemData(count);
					pps->m_pNode = m_pNode;
					if(pps->m_pData->boneObjectId >= 0 ) pps->m_pBone = m_skeleton.getBone(pps->m_pData->boneObjectId);
					else pps->m_pBone = 0;
					pps->Clear();
					m_vParticleSystem.push_back(pps);
				}
				return true;
			}
			else 
			{
				return false;
			}	
		}
	}

	bool ModelInstance::addRibbonSystem()
	{
		if(m_pModel == 0 || m_pNode == 0) return false;
		else
		{
			if( m_pModel->addRibbonSystem() == true )
			{
				//重新初始化，防止vector位移
				RibbonSystemList::iterator it = m_vRibbonSystem.begin();
				for( ; it != m_vRibbonSystem.end(); ++it)
				{
					delete (*it);
				}
				m_vRibbonSystem.clear();
				

				uint size = m_pModel->numRibbonSystem();
				for( uint count = 0; count < size; count++)
				{
					RibbonSystem * prs = new RibbonSystem();
					prs->m_pData = m_pModel->getRibbonSystemData(count);
					prs->m_pNode = m_pNode;
					if( prs->m_pData->boneObjectId >= 0) prs->m_pBone = m_skeleton.getBone( prs->m_pData->boneObjectId);
					else prs->m_pBone =0;
					prs->Clear();
					m_vRibbonSystem.push_back(prs);
				}
				return true;
			}
			else 
			{
				return false;
			}		
		}	
	}

	bool ModelInstance::removeParticleSystem(unsigned int index)
	{
		if( m_pModel == 0 ) return false;
		else
		{
			if( m_pModel->removeParticleSystem(index) == true )
			{
				//重新加载
				ParticleSystemList::iterator it = m_vParticleSystem.begin();
				for( ; it != m_vParticleSystem.end(); ++it )
				{
					delete (*it);
				}
				m_vParticleSystem.clear();

				uint size = m_pModel->numParticleSystem();
				for( uint count = 0; count < size; count++)
				{
					ParticleSystem * pps = new ParticleSystem();
					pps->m_pData = m_pModel->getParticleSystemData(count);
					pps->m_pNode = m_pNode;
					if(pps->m_pData->boneObjectId >= 0 ) pps->m_pBone = m_skeleton.getBone(pps->m_pData->boneObjectId);
					else pps->m_pBone = 0;
					pps->Clear();
					m_vParticleSystem.push_back(pps);
				}
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	bool ModelInstance::removeRibbonSystem(unsigned int index)
	{
		if( m_pModel == 0 ) return false;
		else
		{
			if( m_pModel->removeRibbonSystem(index) == true )
			{
				//重新加载
				RibbonSystemList::iterator it = m_vRibbonSystem.begin();
				for( ; it != m_vRibbonSystem.end(); ++it )
				{
					delete (*it);
				}
				m_vRibbonSystem.clear();

				uint size = m_pModel->numRibbonSystem();
				for( uint count = 0; count < size; count++)
				{
					RibbonSystem * prs = new RibbonSystem();
					prs->m_pData = m_pModel->getRibbonSystemData(count);
					prs->m_pNode = m_pNode;
					if( prs->m_pData->boneObjectId >=0 ) prs->m_pBone = m_skeleton.getBone(prs->m_pData->boneObjectId);
					else prs->m_pBone =0;
					prs->Clear();
					m_vRibbonSystem.push_back(prs);
				}
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	bool ModelInstance::writeModel(string filename,float fCopress)
	{
		if( m_pModel == 0) return false;
		else return m_pModel->write(filename,fCopress);
	}

	bool ModelInstance::isAnimatedTexCoord()
	{
		return m_pModel->isAnimatedTexCoord();
	}
	IVertexBuffer* ModelInstance::getTexCoordBuffer(int pass)
	{
		return m_pModel->getTexCoordBuffer(pass);
	}

	void ModelInstance::SetVBOByTime(float tickCurrent)
	{
		AnimationTime temptime;
		temptime.start = m_pCurrentAnimation->getTimeStart();
		temptime.end = m_pCurrentAnimation->getTimeEnd();
		temptime.current = tickCurrent;
		//uint timeLen = temptime.end - temptime.start;

	
		//temptime.current += m_speed * tickDelta;
		/*while(temptime.current >= temptime.end)
		{
			temptime.current -= timeLen;
		}*/
		
		m_pVBVertexBlend = m_pModel->vertexBlend(&m_skeleton,&temptime);
	}

	bool ModelInstance::setExtParam( std::string strParamName, void * psrc, int srcnum, string srctype)
	{	
		if( psrc == 0) return false;

		//设置包围盒
		if( strParamName == std::string("boundingbox") ) 
			return m_pModel->setExtParam(strParamName,psrc,srcnum, srctype);

		//设置子模型顺序
		if( strParamName == std::string("submodelindex") )
		{
			bool ret = m_pModel->setExtParam(strParamName,psrc, srcnum, srctype);
			if( !ret) 
				return false;
			else
			{
				m_subModels.clear();
				m_vBlendIndices.clear();
				uint numSubModels = m_pModel->getNumSubModels();
				for(uint i = 0;i < numSubModels;i++)
				{
					ISubModel *pSubModel = m_pModel->getSubModel(i);
					SubModelInstance *pSubModelInstance = new SubModelInstance(this,pSubModel, i);
					m_subModels.push_back(pSubModelInstance);
					m_vBlendIndices.push_back( m_pModel->getBlendIndicesBuffer(i));
				}
				return true;
			}
		}

		//默认返回模型设置的结果
		return m_pModel->setExtParam(strParamName, psrc, srcnum, srctype);
	}

	bool ModelInstance::isEnableCustomizeRenderOrder()
	{
		if( !m_pModel) return false;
		return m_pModel->isEnabledCustomizeRenderOrder();
	}

	bool ModelInstance::enableCustomizeRenderOrder(bool enable)
	{
		if( !m_pModel) return false;
		bool initialized = true;
		if(  !m_pModel->isEnabledCustomizeRenderOrder() ) initialized = false;
		if( !m_pModel->enableCustomizeRenderOrder(enable) ) return false;

		if( enable && !initialized )
		{
			//初始化
			std::vector<int> vIndex[RENDER_QUEUE_COUNT];
			std::vector<int> vObjectType[RENDER_QUEUE_COUNT];

			SubModelList::iterator begin = m_subModels.begin();
			SubModelList::iterator end = m_subModels.end();
			int countSub = 0;
			while(begin != end)
			{
				SubModelInstance* pSubModel = (*begin);
				int order = pSubModel->getRenderOrder();
				vIndex[order].push_back(countSub);
				vObjectType[order].push_back(IModel::ECustomizeRenderOrderObject_SubModel);
				++begin;
				++countSub;
			}

			ParticleSystemList::iterator b = m_vParticleSystem.begin();
			ParticleSystemList::iterator e = m_vParticleSystem.end();
			int countPar = 0;
			while(b != e)
			{
				ParticleSystem* pParticleSystem = (*b);
				int order = pParticleSystem->getRenderOrder();
				vIndex[order].push_back(countPar);
				vObjectType[order].push_back(IModel::ECustomizeRenderOrderObject_ParticleSystem);
				++b;
				++countPar;
			}

			RibbonSystemList::iterator be = m_vRibbonSystem.begin();
			RibbonSystemList::iterator ee = m_vRibbonSystem.end();
			int countRib = 0;
			while(be != ee)
			{
				RibbonSystem* pRibbonSystem = (*be);
				int order = pRibbonSystem->getRenderOrder();
				vIndex[order].push_back(countRib);
				vObjectType[order].push_back(IModel::ECustomizeRenderOrderObject_RibbonSystem);
				++be;
				++countRib;
			}

			MmParticleSystemList::iterator mb = m_vMmParticleSystem.begin();
			MmParticleSystemList::iterator me = m_vMmParticleSystem.end();
			int countMMP = 0;
			while(mb != me)
			{
				MmParticleSystem* pParticleSystem = (*mb);
				int order = pParticleSystem->getRenderOrder();
				vIndex[order].push_back(countMMP);
				vObjectType[order].push_back(IModel::ECustomizeRenderOrderObject_MMParticleSystem);
				++mb;
				++countMMP;
			}

			//设置
			int orderbase = 0;
			for( uint i=0; i <RENDER_QUEUE_COUNT; ++i)
			{
				for( uint j=0; j <vIndex[i].size(); ++j)
				{
					int order = orderbase + j;
					m_pModel->setCustomizeRenderOrder(vObjectType[i][j],vIndex[i][j], order);
				}
				orderbase += vIndex[i].size();
			}		
		}
		return true;
	}


	bool ModelInstance::setCustomizeRenderOrder( uint objectType, uint index, int order)
	{
		if( !m_pModel) return false;
		if( !m_pModel->isEnabledCustomizeRenderOrder() ) return false;
		if(!m_pModel->setCustomizeRenderOrder(objectType, index, order) ) return false;
		else return true;
	}

	void ModelInstance::getCustomizeRenderOrder(std::vector<int> & listRenderOrder, std::vector<int> & listObjectType, std::vector<int> & listIndex, std::vector<std::string> & listname)
	{
		if( !m_pModel) return;
		if( !m_pModel->isEnabledCustomizeRenderOrder() ) return;
		uint subnum = m_pModel->getNumSubModels();
		for( uint i =0; i < subnum; ++i)
		{
			int order = m_pModel->getCustomizeRenderOrder(IModel::ECustomizeRenderOrderObject_SubModel, i);
			listRenderOrder.push_back(order);
			listObjectType.push_back(IModel::ECustomizeRenderOrderObject_SubModel);
			listIndex.push_back(i);
			listname.push_back( m_pModel->getSubModel(i)->getName() );
		}

		uint parnum = m_pModel->numParticleSystem();
		for( uint i =0; i < parnum; ++i)
		{
			int order = m_pModel->getCustomizeRenderOrder(IModel::ECustomizeRenderOrderObject_ParticleSystem,i);
			listRenderOrder.push_back(order);
			listObjectType.push_back(IModel::ECustomizeRenderOrderObject_ParticleSystem);
			listIndex.push_back(i);
			listname.push_back(std::string(""));
		}

		uint ribnum = m_pModel->numRibbonSystem();
		for( uint i=0; i< ribnum; ++i)
		{
			int order = m_pModel->getCustomizeRenderOrder(IModel::ECustomizeRenderOrderObject_RibbonSystem,i);
			listRenderOrder.push_back(order);
			listObjectType.push_back(IModel::ECustomizeRenderOrderObject_RibbonSystem);
			listIndex.push_back(i);	
			listname.push_back(std::string(""));
		}

		uint mmpnum = m_pModel->numMmParticleSystem();
		for( uint i=0; i<mmpnum; ++i)
		{
			int order = m_pModel->getCustomizeRenderOrder(IModel::ECustomizeRenderOrderObject_MMParticleSystem,i);
			listRenderOrder.push_back(order);
			listObjectType.push_back(IModel::ECustomizeRenderOrderObject_MMParticleSystem);
			listIndex.push_back(i);		
			listname.push_back(std::string(""));
		}
	}

	void ModelInstance::SetGenerateParticle(bool bGenerate)
	{
		ParticleSystemList::iterator b = m_vParticleSystem.begin();
		ParticleSystemList::iterator e = m_vParticleSystem.end();
		while(b != e)
		{
			ParticleSystem* pParticleSystem = (*b);
			pParticleSystem->SetGenerateParticle(bGenerate);
			++b;
		}

		MmParticleSystemList::iterator mb = m_vMmParticleSystem.begin();
		MmParticleSystemList::iterator me = m_vMmParticleSystem.end();
		while(mb != me)
		{
			MmParticleSystem* pParticleSystem = (*mb);
			pParticleSystem->SetGenerateParticle(bGenerate);
			mb++;
		}
	}

	int  ModelInstance::GetAllParticleNum()
	{
		int nAllParticleNum = 0;

		ParticleSystemList::iterator b = m_vParticleSystem.begin();
		ParticleSystemList::iterator e = m_vParticleSystem.end();
		while(b != e)
		{
			ParticleSystem* pParticleSystem = (*b);
			nAllParticleNum += pParticleSystem->__getCurParticleNum();
			++b;
		}

		MmParticleSystemList::iterator mb = m_vMmParticleSystem.begin();
		MmParticleSystemList::iterator me = m_vMmParticleSystem.end();
		while(mb != me)
		{
			MmParticleSystem* pParticleSystem = (*mb);
			nAllParticleNum += pParticleSystem->__getCurParticleNum();
			mb++;
		}

		return nAllParticleNum;
	}
}
