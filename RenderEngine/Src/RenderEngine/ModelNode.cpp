#include "StdAfx.h"
#include "ModelNode.h"
#include "ModelInstance.h"
#include "ModelNodeCreater.h"
#include "Bone.h"

namespace xs
{

	ModelNode::ModelNode() : m_pParent(0),
		m_quatOrientation(Quaternion::IDENTITY),
		m_vPosition(Vector3::ZERO),
		m_vScale(Vector3::UNIT_SCALE),
		m_pModelInstance(0),
		m_surfaceDiffuse(ColorValue::White),
		m_pBone(0),
		m_stepLength(120),
		m_bOnlyInheritTranslation(false),
		m_bVisible(true)
	{
		m_boundingBox = Rect(-100,-100,100,100);
	}

	//-----------------------------------------------------------------------
	ModelNode::~ModelNode()
	{
		safeDelete(m_pModelInstance);
	}

	void ModelNode::release()
	{
		ChildNodeList::iterator it, itend;
		itend = m_vChildren.end();
		for (it = m_vChildren.begin(); it != itend; ++it)
		{
			ModelNode* child = (*it);
			child->release();
		}
		delete this;
	}

	void ModelNode::setVisible(bool bVisible)
	{
		m_bVisible = bVisible;
	}

	bool ModelNode::setModel(const char* modelName)
	{
		//当有子节点，那么无法重置模型
		if(numChildren() > 0)return false;

		ModelInstance *pModelInstance = new ModelInstance;
		if(pModelInstance)
		{
			if(!pModelInstance->setModel(modelName,this))
			{
				delete pModelInstance;
				pModelInstance = 0;
				return false;
			}
		}

		CPath path = modelName;
		std::string ext = path.getFileExt();
		if(StringHelper::casecmp(ext,"mdx") == 0)
			m_mt = MT_MDX;
		if(StringHelper::casecmp(ext,"mm") == 0)
			m_mt = MT_MM;
		if(StringHelper::casecmp(ext,"mz") == 0)
			m_mt = MT_MZ;
		if(StringHelper::casecmp(ext,"tx") == 0)//特效文件
			m_mt = MT_TX;
		if(StringHelper::casecmp(ext,"mx") == 0 )
			m_mt = MT_MX;
		safeDelete(m_pModelInstance);
		m_pModelInstance = pModelInstance;

		//设置包围盒
		m_AABB = m_pModelInstance->getAABB();
		_updateAABB();

		needUpdate();
		return true;
	}

	//-----------------------------------------------------------------------
	ModelNode* ModelNode::getParent(void ) const
	{
		return m_pParent;
	}
	//-----------------------------------------------------------------------
	void ModelNode::updateTransform()
	{
		makeTransform(m_vPosition,m_vScale,m_quatOrientation,mTransform);

		switch(m_mt)
		{
		case MT_MDX:
			{
				static Matrix4 precomputeMtx = 
					Matrix4(
					0, 1, 0, 0,
					0, 0, 1, 0,
					1, 0, 0, 0,
					0, 0, 0, 1);
				static Matrix4 precomputeMtx2 = 
					Matrix4(
					0.133333f, 0, 0, 0,
					0, 0, 0.133333f, 0,
					0, 0.133333f, 0, 0,
					0, 0, 0, 1);

				if(!m_pParent)
				{
					mTransform = mTransform * precomputeMtx;
				}
				else
				{
					mTransform = mTransform * precomputeMtx2;
				}
			}
			break;
		case MT_MZ:
			break;
		case MT_TX:
			break;
		case MT_MX:
			break;
		case MT_MM:
			{
				static Matrix4 precomputeMtx = 
					Matrix4(
					0, 0,-75, 0,
					0, 75, 0, 0,
					75, 0, 0, 0,
					0, 0, 0, 1);

				if(!m_pParent)
				{
					mTransform = mTransform * precomputeMtx;
				}
			}
			break;
		}

		if(m_pParent && m_pBone)
		{
			const Vector3& pivot = m_pBone->getTransformedPivot();
			if(m_bOnlyInheritTranslation)
			{
				Matrix4 mtxTranslation;
				mtxTranslation.makeTrans(pivot);
				
				//modified by xxh 20090928
				//mDerivedTransform = m_pParent->getFullTransform() * mtxTranslation * mTransform; //错误，未把modelnode的点的初始位置变换到父节点的空间中
				mDerivedTransform = m_pParent->getFullTransform() * mtxTranslation *  m_pBone->getInitMatrix() * mTransform;

			}
			else
			{
				Matrix4 m = m_pBone->getFullTransform();
				/////////////////////////////////////////////////
				//以前的计算方式
				//m[0][3] = pivot.x;
				//m[1][3] = pivot.y;
				//m[2][3] = pivot.z;
				//mDerivedTransform = m_pParent->getFullTransform() * m * mTransform;
				////////////////////////////////////////////////////////////

				///新的计算方式：把绑定的节点变换到被绑定的节点空间
				mDerivedTransform = m_pParent->getFullTransform() * m * m_pBone->getInitMatrix() * mTransform;

			}
		}
		else
		{
			mDerivedTransform = mTransform;
		}
	}
	//-----------------------------------------------------------------------
	const Matrix4& ModelNode::getFullTransform(void )
	{
		if(m_bNeedUpdate)
		{
			updateTransform();
			m_bNeedUpdate = false;
		}
		return mDerivedTransform;
	}

	Bone*	ModelNode::getBoneAttachedTo()
	{
		return m_pBone;
	}

	void ModelNode::updateColor(IRenderSystem* pRenderSystem,const ColorValue& diffuse)
	{
		ColorValue color = diffuse * m_surfaceDiffuse;
		m_pModelInstance->updateColor(pRenderSystem,color);

		ChildNodeList::iterator it, itend;
		itend = m_vChildren.end();
		for (it = m_vChildren.begin(); it != itend; ++it)
		{
			ModelNode* child = (*it);
			child->updateColor(pRenderSystem,color);
		}
	}
	//-----------------------------------------------------------------------
	void  ModelNode::update(float tick,float tickDelta,IRenderSystem* pRenderSystem,const ColorValue& diffuse)
	{
		PP_BY_NAME("ModelNode::update");

		ColorValue color = diffuse * m_surfaceDiffuse;
		PP_BY_NAME_START("ModelNode::update::m_pModelInstance->update");
		m_pModelInstance->update(tick,tickDelta,pRenderSystem,color);
		PP_BY_NAME_STOP();

		PP_BY_NAME_START("ModelNode::update::updateTransform");
		updateTransform();
		PP_BY_NAME_STOP();

		PP_BY_NAME_START("ModelNode::update::child->update");
		ChildNodeList::iterator it, itend;
		itend = m_vChildren.end();
		for (it = m_vChildren.begin(); it != itend; ++it)
		{
			ModelNode* child = (*it);
			child->update(tick, tickDelta,pRenderSystem,color);
		}
		PP_BY_NAME_STOP();

		//_updateAABB();
	}

	void ModelNode::render(IRenderSystem* pRenderSystem,bool useMtl)
	{
		PP_BY_NAME("ModelNode::render");
		m_renderQueue.clear();
		updateRenderQueue(&m_renderQueue);
		m_renderQueue.render(pRenderSystem,useMtl);
	}

	//
	void ModelNode::renderByTime(IRenderSystem* pRenderSystem,bool useMtl,float tickCurrent)
	{
		if(m_pModelInstance)
			m_pModelInstance->SetVBOByTime(tickCurrent);
		render(pRenderSystem,useMtl);
	}

	//-----------------------------------------------------------------------
	ModelNode* ModelNode::createChild(const char* boneName,const char*modelName,const Vector3& translate, const Quaternion& rotate)
	{
		ModelNode *newNode = ModelNodeCreater::create(modelName);
		if(newNode)
		{
			if(!addChild(newNode,boneName))
			{
				newNode->release();
				newNode = 0;
			}
			else
			{
				newNode->translate(translate);
				newNode->rotate(rotate);
			}
		}

		return newNode;
	}
	//-----------------------------------------------------------------------
	bool  ModelNode::addChild(ModelNode* child,const char* boneName)
	{
		if( 0 == child )
			return false;

		Bone *pBone = m_pModelInstance->getBone(boneName);
		if(!pBone)return false;

		m_vChildren.push_back(child);
		child->m_pParent = this;
		child->m_pBone = pBone;
		child->needUpdate();

		return true;
	}
	//-----------------------------------------------------------------------
	ushort ModelNode::numChildren(void ) const
	{
		return static_cast<ushort>(m_vChildren.size());
	}
	//-----------------------------------------------------------------------
	ModelNode* ModelNode::getChild(ushort index) const
	{
		return m_vChildren[index];
	}
	//-----------------------------------------------------------------------
	ModelNode* ModelNode::removeChild(ModelNode* child)
	{
		ChildNodeList::iterator i, iend;
		iend = m_vChildren.end();
		for (i = m_vChildren.begin(); i != iend; ++i)
		{
			if (*i == child)
			{
				m_vChildren.erase(i);
				child->m_pParent = 0;
				child->m_pBone = 0;
				child->needUpdate();
				break;
			}
		}
		return child;
	}
	//-----------------------------------------------------------------------
	void ModelNode::destroyChild(ModelNode* child)
	{
		ModelNode *pNode = removeChild(child);
		if(pNode)
		{
			pNode->release();
			pNode = 0;
		}
	}
	//-----------------------------------------------------------------------
	void ModelNode::destroyAllChildren(void)
	{
		while(numChildren())
		{
			destroyChild(getChild(0));
		}
	}
	//-----------------------------------------------------------------------
	const Quaternion& ModelNode::getOrientation() const
	{
		return m_quatOrientation;
	}

	//-----------------------------------------------------------------------
	void  ModelNode::setOrientation( const Quaternion & q )
	{
		m_quatOrientation = q;
		needUpdate();
	}
	//-----------------------------------------------------------------------
	void  ModelNode::setOrientation( float w, float x, float y, float z)
	{
		m_quatOrientation.w = w;
		m_quatOrientation.x = x;
		m_quatOrientation.y = y;
		m_quatOrientation.z = z;
		needUpdate();
	}
	//-----------------------------------------------------------------------
	void  ModelNode::resetOrientation(void )
	{
		m_quatOrientation = Quaternion::IDENTITY;
		needUpdate();
	}

	//-----------------------------------------------------------------------
	void  ModelNode::setPosition(const Vector3& pos)
	{
		m_vPosition = pos;
		needUpdate();
	}


	//-----------------------------------------------------------------------
	void  ModelNode::setPosition(float x, float y, float z)
	{
		Vector3 v(x,y,z);
		setPosition(v);
	}

	//-----------------------------------------------------------------------
	const Vector3 & ModelNode::getPosition(void ) const
	{
		return m_vPosition;
	}
	//-----------------------------------------------------------------------
	Matrix3 ModelNode::getLocalAxes(void ) const
	{
		Vector3 axisX = Vector3::UNIT_X;
		Vector3 axisY = Vector3::UNIT_Y;
		Vector3 axisZ = Vector3::UNIT_Z;

		axisX = m_quatOrientation * axisX;
		axisY = m_quatOrientation * axisY;
		axisZ = m_quatOrientation * axisZ;

		return Matrix3(axisX.x, axisY.x, axisZ.x,
			axisX.y, axisY.y, axisZ.y,
			axisX.z, axisY.z, axisZ.z);
	}

	//-----------------------------------------------------------------------
	void  ModelNode::translate(const Vector3& d)
	{
		m_vPosition += m_quatOrientation * d;
		needUpdate();

	}
	//-----------------------------------------------------------------------
	void  ModelNode::translate(float x, float y, float z)
	{
		Vector3 v(x,y,z);
		translate(v);
	}
	//-----------------------------------------------------------------------
	void  ModelNode::translate(const Matrix3& axes, const Vector3& move)
	{
		Vector3 derived = axes * move;
		translate(derived);
	}
	//-----------------------------------------------------------------------
	void  ModelNode::translate(const Matrix3& axes, float x, float y, float z)
	{
		Vector3 d(x,y,z);
		translate(axes,d);
	}
	//-----------------------------------------------------------------------
	void  ModelNode::roll(const Radian& angle)
	{
		rotate(Vector3::UNIT_Z, angle);
	}
	//-----------------------------------------------------------------------
	void  ModelNode::roll(float degrees)
	{
		roll ( Angle(degrees) );
	}
	//-----------------------------------------------------------------------
	void  ModelNode::pitch(const Radian& angle)
	{
		rotate(Vector3::UNIT_X, angle);
	}
	//-----------------------------------------------------------------------
	void  ModelNode::pitch(float degrees)
	{
		pitch ( Angle(degrees) );
	}
	//-----------------------------------------------------------------------
	void  ModelNode::yaw(const Radian& angle)
	{
		rotate(Vector3::UNIT_Y, angle);

	}
	//-----------------------------------------------------------------------
	void  ModelNode::yaw(float degrees)
	{
		yaw ( Angle(degrees) );
	}
	//-----------------------------------------------------------------------
	void  ModelNode::rotate(const Vector3& axis, const Radian& angle)
	{
		Quaternion q;
		q.FromAngleAxis(angle,axis);
		rotate(q);
	}
	//-----------------------------------------------------------------------
	void  ModelNode::rotate(const Vector3& axis, float degrees)
	{
		rotate ( axis, Angle(degrees) );
	}

	//-----------------------------------------------------------------------
	void  ModelNode::rotate(const Quaternion& q)
	{
		m_quatOrientation = q * m_quatOrientation;
		needUpdate();
	}
	//-----------------------------------------------------------------------
	void  ModelNode::setScale(const Vector3& scale)
	{
		m_vScale = scale;
		needUpdate();
	}
	//-----------------------------------------------------------------------
	void  ModelNode::setScale(float x, float y, float z)
	{
		m_vScale.x = x;
		m_vScale.y = y;
		m_vScale.z = z;
		needUpdate();
	}
	//-----------------------------------------------------------------------
	const Vector3 & ModelNode::getScale(void ) const
	{
		return m_vScale;
	}
	//-----------------------------------------------------------------------
	void  ModelNode::scale(const Vector3& scale)
	{
		m_vScale = m_vScale * scale;
		needUpdate();

	}
	//-----------------------------------------------------------------------
	void  ModelNode::scale(float x, float y, float z)
	{
		m_vScale.x *= x;
		m_vScale.y *= y;
		m_vScale.z *= z;
		needUpdate();

	}
	//-----------------------------------------------------------------------
	void  ModelNode::makeTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation, 
		Matrix4& destMatrix) const
	{
		destMatrix = Matrix4::IDENTITY;
		// Ordering:
		//    1. Scale
		//    2. Rotate
		//    3. Translate

		// Parent scaling is already applied to derived position
		// Own scale is applied before rotation
		Matrix3 rot3x3, scale3x3;
		orientation.ToRotationMatrix(rot3x3);
		scale3x3 = Matrix3::ZERO;
		scale3x3[0][0] = scale.x;
		scale3x3[1][1] = scale.y;
		scale3x3[2][2] = scale.z;

		destMatrix = rot3x3 * scale3x3;
		destMatrix.setTrans(position);
	}

	const AABB&		ModelNode::getWorldAABB(void ) const
	{
		return m_AABB;
	}

	const Sphere& ModelNode::getWorldSphere(void ) const
	{
		//zgz 这里的sphere不会实时更新，并不能代表模型的真实位置
		return m_sphere;
	}

	void  ModelNode::_updateAABB(void )
	{
		// Reset bounds first
		m_AABB.setNull();

		// Update bounds from own attached objects
		m_AABB.merge(m_pModelInstance->getAABB());

		// Merge with children
		ChildNodeList::iterator child,childEnd = m_vChildren.end();
		for (child = m_vChildren.begin();child != childEnd;++child)
		{
			ModelNode* sceneChild = static_cast<ModelNode*>(*child);
			sceneChild->_updateAABB();
			m_AABB.merge(sceneChild->m_AABB);
		}

		const Vector3& vMin = m_AABB.getMinimum();
		const Vector3& vMax = m_AABB.getMaximum();
		m_sphere.setCenter(vMin.midPoint(vMax));
		m_sphere.setRadius((vMax - vMin).length() * 0.5f);
	}

	void  ModelNode::updateRenderQueue(RenderQueue *pQueue)
	{
		PP_BY_NAME("ModelNode::updateRenderQueue");
		if( !m_bVisible ) return; //如果模型为不可见，则不更新渲染队列
		m_pModelInstance->updateRenderQueue(pQueue);

		ushort childNum = numChildren();
		for(ushort i = 0;i < childNum;i++)
		{
			ModelNode *pChild = getChild(i);
			if(pChild)
			{
				pChild->updateRenderQueue(pQueue);
			}
		}
	}

	void ModelNode::needUpdate()
	{
		m_bNeedUpdate = true;
	}

	ModelInstance*	ModelNode::getModelInstance()
	{
		return m_pModelInstance;
	}

	/*void ModelNode::setColor(const ColorValue& color,bool recursive)
	{
		m_color = color;
		if(m_pModelInstance)m_pModelInstance->setColor(color);

		if(recursive)
		{
			ChildNodeList::iterator it, itend;
			itend = m_vChildren.end();
			for (it = m_vChildren.begin(); it != itend; ++it)
			{
				ModelNode* child = (*it);
				child->setColor(color,recursive);
			}
		}
	}

	void ModelNode::setTransparency(float transparency,bool recursive)
	{
		m_transparency = transparency;
		if(m_pModelInstance)m_pModelInstance->setTransparency(transparency);

		if(recursive)
		{
			ChildNodeList::iterator it, itend;
			itend = m_vChildren.end();
			for (it = m_vChildren.begin(); it != itend; ++it)
			{
				ModelNode* child = (*it);
				child->setTransparency(transparency);
			}
		}
	}*/

	void ModelNode::setGlobalDiffuse(const ColorValue& color)
	{
		m_surfaceDiffuse = color;
	}

	const ColorValue& ModelNode::getSurfaceDiffuse()
	{
		return m_surfaceDiffuse;
	}

	size_t ModelNode::getMemorySize()
	{
		//modified by xxh 20091103
		//由于模型管理器已经管理了模型的内存，故资源管理器不需要再管理了，而且以前的管理方式也是错误的。没有考虑到共享
		size_t size = 0; 
		/*
		size += sizeof(m_pParent);	
		size += sizeof( m_quatOrientation);
		size += sizeof(m_vPosition);
		size += sizeof(m_vScale);
		size += sizeof(mTransform);
		size += sizeof(mDerivedTransform);
		size += sizeof(m_itBone);
		size += sizeof(m_stepLength);
		size += sizeof(m_renderQueue);
		size += sizeof( m_bOnlyInheritTranslation);
		size += sizeof( m_boundingBox);
		size += sizeof(m_mt);
		size += sizeof(m_AABB);
		size += sizeof(m_sphere);
		size += sizeof(m_pModelInstance);
		size += sizeof(m_pBone);
		size += sizeof(m_bNeedUpdate);
		size += sizeof(m_surfaceDiffuse);
		
		size += 466;// 上面的参数占内存466字节

		size += m_vChildren.capacity() * sizeof( ModelNode * );
		size += m_boneNameIt.capacity() * sizeof(char);

		size += m_pModelInstance->getMemorySize();
		*/
		size += sizeof(*this);
		size += m_pModelInstance->getMemorySize();
		ChildNodeList::iterator it, itend;
		itend = m_vChildren.end();
		for (it = m_vChildren.begin(); it != itend; ++it)
		{
			ModelNode* child = (*it);
			size += child->getMemorySize();
		}

		return size;
		

		/*

		size_t size = m_pModelInstance->getMemorySize();

		ChildNodeList::iterator it, itend;
		itend = m_vChildren.end();
		for (it = m_vChildren.begin(); it != itend; ++it)
		{
			ModelNode* child = (*it);
			size += child->getMemorySize();
		}

		return size;
		*/
	}

	ModelNode* ModelNode::getFirstChildNodeByBone(const char* boneName)
	{
		if(!boneName)
			return 0;

		m_itBone = m_vChildren.begin();

		if(m_itBone == m_vChildren.end())
			return 0;

		m_boneNameIt = boneName;
		return getNextChildNodeByBone();
	}

	ModelNode* ModelNode::getNextChildNodeByBone()
	{
		ChildNodeList::iterator end = m_vChildren.end();
		while(m_itBone != end)
		{
			ModelNode *pNode = (*m_itBone++);
			Bone *pBone = pNode->getBoneAttachedTo();
			if(pBone && StringHelper::casecmp(pBone->getName(),m_boneNameIt) == 0)
			{
				return pNode;
			}
		}

		return 0;
	}

	const std::string& ModelNode::getName() const
	{
		return m_strName;
	}

	void ModelNode::setName(const std::string& name)
	{
		m_strName = name;
	}

	void ModelNode::setStepLength(int length)
	{
		m_stepLength = length;
	}

	int ModelNode::getStepLength()
	{
		return m_stepLength;
	}

	void ModelNode::setBoundingBox(const Rect& rect)
	{
		m_boundingBox = rect;
	}

	const Rect& ModelNode::getBoundingBox()
	{
		return m_boundingBox;
	}

	void ModelNode::SetGenerateParticle(bool bGenerate)
	{
		m_pModelInstance->SetGenerateParticle(bGenerate);

		ChildNodeList::iterator it, itend;
		itend = m_vChildren.end();
		for (it = m_vChildren.begin(); it != itend; ++it)
		{
			ModelNode* child = (*it);
			child->SetGenerateParticle(bGenerate);
		}
	}

	int  ModelNode::GetParticleNum()
	{
		int nParticleNum = 0;

		nParticleNum += m_pModelInstance->GetAllParticleNum();

		ChildNodeList::iterator it, itend;
		itend = m_vChildren.end();
		for (it = m_vChildren.begin(); it != itend; ++it)
		{
			ModelNode* child = (*it);
			nParticleNum += child->GetParticleNum();
		}

		return nParticleNum;
	}
}