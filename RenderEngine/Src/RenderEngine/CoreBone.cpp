#include "StdAfx.h"
#include "CoreBone.h"
#include "Animation.h"
#include "Camera.h"
#include "HelperFuncs.h"

namespace xs
{
	void	CoreBone::setName(const char *name)
	{
		m_name = name;
	}

	const char*	 CoreBone::getName()
	{
		return m_name.c_str();
	}
	
	void	CoreBone::setPivotRotation(bool pivotRotation)
	{
		m_bPivotRotation = pivotRotation;
	}
	bool   CoreBone::getPivotRotation()
	{
		return m_bPivotRotation;
	}

	void 	CoreBone::setPivot(const Vector3& vPivot)
	{
		m_vPivot = vPivot;
	}

	const	Vector3&	CoreBone::getPivot()
	{
		return m_vPivot;
	}

	void	CoreBone::setPrecomputeMatrix(const Matrix4& mtx)
	{
		m_precomputeMtx = mtx;
	}
	const xs::Matrix4 & CoreBone::getPrecomputeMatrix()
	{
		return m_precomputeMtx;
	}

	void 	CoreBone::setParent(CoreBone* pCoreBone)
	{
		m_pParent = pCoreBone;
	}

	CoreBone*	CoreBone::getParent()
	{
		return m_pParent;
	}

	void CoreBone::setCanBufferData(bool canBuffer)
	{
		m_bCanBufferData = canBuffer;
	}


	size_t CoreBone::getMemeorySize()
	{
		size_t memorySize = sizeof(this);
		//其它的大小不可预估，暂时忽略
		return memorySize;
	}

	CoreBone::~CoreBone()
	{
		if( !m_vNoPivotData.empty())
		{
			KeyFrameDataContainerIterator it = m_vNoPivotData.begin();
			KeyFrameDataContainerIterator end = m_vNoPivotData.end();
			for( ; it != end; ++it)
			{
				delete  it->second;
			}
		}
		m_vNoPivotData.clear();

		if( !m_vPivotData.empty())
		{
			KeyFrameDataContainerIterator it = m_vPivotData.begin();
			KeyFrameDataContainerIterator end = m_vPivotData.end();
			for( ; it != end; ++it)
			{
				delete it->second;
			}
		}
		m_vPivotData.clear();
	}


	//void Bone::_calcMatrix_NoPivotRotation(IRenderSystem *pRenderSystem,AnimationTime* pTime,int rangeIndex)
	//{
	//	if(m_bCalced)return;

	//	Matrix4 m;
	//	Quaternion q;

	//	m = Matrix4::IDENTITY;

	//	//translation
	//	Vector3 translation = m_pTranslation->getFrame(rangeIndex,pTime);
	//	m[0][3] += translation.x;
	//	m[1][3] += translation.y;
	//	m[2][3] += translation.z;

	//	//rotation
	//	q = m_pRotation->getFrame(rangeIndex,pTime);
	//	m = m * q;

	//	//scale
	//	Vector3 scale = m_pScale->getFrame(rangeIndex,pTime);
	//	m = m * Matrix4::getScale(scale);

	//	if(m_billboardType != BILLBOARD_NOT_USED)
	//	{
	//		Matrix4 mbb = calcBillboardMatrix(pRenderSystem->getModelViewMatrix(),m_vPivot,m_billboardType);
	//		m = m * mbb;
	//	}
	//	
	//	q = m.extractQuaternion();
	//	if(m_pParent)
	//	{
	//		m_pParent->calcMatrix(pRenderSystem,pTime,rangeIndex);
	//		m_mtxTransform = m_pParent->m_mtxTransform * m;
	//	}
	//	else
	//	{
	//		m_mtxTransform = m;
	//	}

	//	if(m_pParent)
	//	{
	//		m_quatRotation = m_pParent->m_quatRotation * q;
	//	}
	//	else
	//	{
	//		m_quatRotation = q;
	//	}
	//
	//	m_vTransformedPivot = m_mtxTransform * m_vPivot;

	//	
	//	m_bCalced = true;
	//}

	//void Bone::_calcMatrix_PivotRotation(IRenderSystem *pRenderSystem,AnimationTime* pTime,int rangeIndex)
	//{
	//	if(m_bCalced)return;

	//	if(m_pParent)
	//	{
	//		m_pParent->calcMatrix(pRenderSystem,pTime,rangeIndex);
	//	}

	//	Matrix4 m;
	//	Quaternion q;

	//	m.makeTrans(m_vPivot);

	//	//translation
	//	if(m_pTranslation->getInterpolationType() != INTERPOLATION_NONE)
	//	{
	//		Vector3 translation = m_pTranslation->getFrame(rangeIndex,pTime);
	//		m[0][3] += translation.x;
	//		m[1][3] += translation.y;
	//		m[2][3] += translation.z;
	//	}

	//	//rotation
	//	if(m_pRotation->getInterpolationType() != INTERPOLATION_NONE)
	//	{
	//		q = m_pRotation->getFrame(rangeIndex,pTime);
	//		m = m * q;
	//	}

	//	//scale
	//	if(m_pScale->getInterpolationType() != INTERPOLATION_NONE)
	//	{
	//		Vector3 scale = m_pScale->getFrame(rangeIndex,pTime);
	//		m = m * Matrix4::getScale(scale);
	//	}

	//	if(m_billboardType != BILLBOARD_NOT_USED)
	//	{
	//		Matrix4 mtx = pRenderSystem->getModelViewMatrix();
	//		if(m_pNode)
	//		{
	//			mtx = mtx * m_pNode->getFullTransform();
	//		}
	//		Matrix4 mbb = calcBillboardMatrix(mtx,m_vPivot,m_billboardType);
	//		m = m * mbb;
	//	}

	//	Matrix4 m4;
	//	m4.makeTrans(-m_vPivot);

	//	m = m * m4;

	//	if(m_pParent)
	//	{
	//		m_mtxTransform = m_pParent->m_mtxTransform * m;
	//	}
	//	else
	//	{
	//		m_mtxTransform = m;
	//	}

	//	m_quatRotation = m_mtxTransform.extractQuaternion();

	//	m_vTransformedPivot = m_mtxTransform * m_vPivot;

	//	m_bCalced = true;
	//}

	uint CoreBone::getClosetKeyFrameTime(AnimationTime * pTime,int rangeIndex)
	{
		PP_BY_NAME("CoreBone::getClosetKeyFrameTime::getClosetKeyFrameTime");
		if( 0 == pTime) 
			return 0;

		//顺序查找
		if( pTime->current <= pTime->start)
			return pTime->start;
		std::pair<uint, uint> pair; 
		pair = m_keyTimeIndex[pTime->start];
		uint start = pair.first;
		uint end = start + pair.second;
		for( uint i=start; i < end; ++i)
		{
			if( m_keyTime[i] > pTime->current )
				return m_keyTime[i-1];
		}
		return m_keyTime[end -1];

		////二分查找
		//std::pair<uint, uint> pair;
		//pair = m_keyTimeIndex[pTime->start];
		//uint low = pair.first;
		//uint high = low + pair.second -1;
		//if( pTime->current <= m_keyTime[low] ) 
		//	return m_keyTime[low];
		//if( pTime->current >= m_keyTime[high] )
		//	return m_keyTime[high];
		//while( low <= high)
		//{
		//	uint middle = low + (high - low)/2;
		//	if( m_keyTime[middle] > pTime->current )
		//		high = middle -1;
		//	else if( m_keyTime[middle] < pTime->current)
		//		low = middle + 1;
		//	else
		//		return m_keyTime[middle];
		//}
		//return m_keyTime[high];
	}

	void CoreBone::getBoneData(
		AnimationTime* pTime, 
		int rangeIndex, 
		xs::Matrix4 & mtxTransform,
		xs::Quaternion & quatRotation
		)
	{
		//PP_BY_NAME("CoreBone::getBoneData");
		if( !m_bCanBufferData ) //不能缓存计算结果，此时直接计算
		{
			if( m_bPivotRotation )
			{
				mtxTransform.makeTrans(m_vPivot);

				//translation
				if(m_pTranslation->getInterpolationType() != INTERPOLATION_NONE)
				{
					Vector3 translation = m_pTranslation->getFrame(rangeIndex,pTime);
					mtxTransform[0][3] += translation.x;
					mtxTransform[1][3] += translation.y;
					mtxTransform[2][3] += translation.z;
				}

				//rotation
				if(m_pRotation->getInterpolationType() != INTERPOLATION_NONE)
				{
					xs::Quaternion q =  m_pRotation->getFrame(rangeIndex,pTime);
					mtxTransform = mtxTransform * q;
				}

				//scale
				if(m_pScale->getInterpolationType() != INTERPOLATION_NONE)
				{
					Vector3 scale = m_pScale->getFrame(rangeIndex,pTime);
					mtxTransform = mtxTransform * Matrix4::getScale(scale);
				}

				//no billboard calculation

				Matrix4 m4;
				m4.makeTrans(-m_vPivot);
				mtxTransform = mtxTransform * m4;

				quatRotation = mtxTransform.extractQuaternion();

				return;
			}
			else
			{
				mtxTransform = Matrix4::IDENTITY;

				//translation
				Vector3 translation = m_pTranslation->getFrame(rangeIndex, pTime);
				mtxTransform[0][3] += translation.x;
				mtxTransform[1][3] += translation.y;
				mtxTransform[2][3] += translation.z;

				//rotation
				xs::Quaternion q = m_pRotation->getFrame(rangeIndex,pTime);
				mtxTransform = mtxTransform * q;

				//scale
				Vector3 scale = m_pScale->getFrame(rangeIndex,pTime);
				mtxTransform = mtxTransform * Matrix4::getScale(scale);

				//no billboard calculation

				quatRotation =mtxTransform.extractQuaternion();

				return;		
			}

		}
		else//能够缓存数据
		{
			//PP_BY_NAME("CoreBone::getBoneData::BufferData");

			///////////////////////////////////////////////////////////////////
			//uint keyFrameTime = getClosetKeyFrameTime(pTime, rangeIndex);
			//改成内联函数

			//PP_BY_NAME_START("CoreBone::getClosetKeyFrameTime::getClosetKeyFrameTime");

			uint keyFrameTime = 0xffffffff;

			////顺序查找
			////不可能
			////if( pTime->current <= pTime->start)
			////	return pTime->start;
			//std::pair<uint, uint> pair; 
			//pair = m_keyTimeIndex[pTime->start];
			//uint start = pair.first;
			//uint end = start + pair.second;
			//for( uint i=start; i < end; ++i)
			//{
			//	if( m_keyTime[i] > pTime->current )
			//	{
			//		keyFrameTime = m_keyTime[i-1];
			//		break;
			//	}
			//}
			//if( 0xffffffff == keyFrameTime)
			//	keyFrameTime = m_keyTime[end -1];

			//二分查找
			std::pair<uint, uint> pair;
			pair = m_keyTimeIndex[pTime->start];
			if( 1 == pair.second )//很多骨骼只有1个关键时间
			{
				keyFrameTime = m_keyTime[pair.first];
			}
			else
			{
				uint low = pair.first;
				uint high = low + pair.second -1;
				//if( pTime->current <= m_keyTime[low] ) 
				//	return m_keyTime[low];
				//if( pTime->current >= m_keyTime[high] )
				//	return m_keyTime[high];
				while( low <= high)
				{
					//uint middle = low + (high - low)/2;
					uint middle = (low + high) >> 1;
					if( m_keyTime[middle] > pTime->current )
						high = middle -1;
					else if( m_keyTime[middle] < pTime->current)
						low = middle + 1;
					else
					{
						keyFrameTime =  m_keyTime[middle];
						break;
					}
				}
				if( 0xffffffff ==  keyFrameTime )
					keyFrameTime = m_keyTime[high];
			}
			

			//PP_BY_NAME_STOP();

			////////////////////////////////////////////////////////////////////

			// m_vPivotData[keyFrameTime] 中将记录pTime->current插值计算的结果，可能造成一种情况： [4/29/2011 zgz]
			// 比如一帧的时间为 0~33ms, 第一次进行插值时pTime->current = 1ms;
			// 那么意味着0~33ms之间的任何时刻都将使用 1ms 时刻的插值结果。这样可能会造成两桢之间偏差过大，不够平滑；
			// 因此这里用keyFrameTime，避免出现波动。
			
			AnimationTime curInterpolationTime;
			curInterpolationTime.start = pTime->start;
			curInterpolationTime.end = pTime->end;
			curInterpolationTime.current = keyFrameTime;

			if( m_bPivotRotation)
			{
				//PP_BY_NAME("CoreBone::getBoneData::BufferData::PivotRotation");
				//从缓存取
				KeyFrameData * pData = m_vPivotData[keyFrameTime];
				if( pData)
				{
					mtxTransform = pData->m_mtxTransform;
					quatRotation = pData->m_quatRotation;
					return;
				}

				//如果缓存没有，计算获取
				pData = new KeyFrameData();

				pData->m_mtxTransform.makeTrans(m_vPivot);

				//translation
				if(m_pTranslation->getInterpolationType() != INTERPOLATION_NONE)
				{
					Vector3 translation = m_pTranslation->getFrame(rangeIndex, &curInterpolationTime);
					pData->m_mtxTransform[0][3] += translation.x;
					pData->m_mtxTransform[1][3] += translation.y;
					pData->m_mtxTransform[2][3] += translation.z;
				}

				//rotation
				if(m_pRotation->getInterpolationType() != INTERPOLATION_NONE)
				{
					xs::Quaternion q =  m_pRotation->getFrame(rangeIndex, &curInterpolationTime);
					pData->m_mtxTransform = pData->m_mtxTransform * q;
				}

				//scale
				if(m_pScale->getInterpolationType() != INTERPOLATION_NONE)
				{
					Vector3 scale = m_pScale->getFrame(rangeIndex, &curInterpolationTime);
					pData->m_mtxTransform = pData->m_mtxTransform * Matrix4::getScale(scale);
				}

				//no billboard calculation

				Matrix4 m4;
				m4.makeTrans(-m_vPivot);

				pData->m_mtxTransform = pData->m_mtxTransform * m4;

				pData->m_quatRotation = pData->m_mtxTransform.extractQuaternion();

				m_vPivotData[keyFrameTime] = pData;
				mtxTransform = pData->m_mtxTransform;
				quatRotation = pData->m_quatRotation;

				return;
			}
			else
			{
				//PP_BY_NAME("CoreBone::getBoneData::BufferData::NoPivotRotation");

				//从缓存获取
				KeyFrameData * pData = m_vNoPivotData[keyFrameTime];
				if( pData)
				{
					mtxTransform = pData->m_mtxTransform;
					quatRotation = pData->m_quatRotation;
					return;
				}	

				//计算获取
				pData = new KeyFrameData();

				pData->m_mtxTransform = Matrix4::IDENTITY;

				//translation
				Vector3 translation = m_pTranslation->getFrame(rangeIndex, &curInterpolationTime);
				pData->m_mtxTransform[0][3] += translation.x;
				pData->m_mtxTransform[1][3] += translation.y;
				pData->m_mtxTransform[2][3] += translation.z;

				//rotation
				xs::Quaternion q = m_pRotation->getFrame(rangeIndex, &curInterpolationTime);
				pData->m_mtxTransform = pData->m_mtxTransform * q;

				//scale
				Vector3 scale = m_pScale->getFrame(rangeIndex, &curInterpolationTime);
				pData->m_mtxTransform = pData->m_mtxTransform * Matrix4::getScale(scale);

				//no billboard calculation

				pData->m_quatRotation = pData->m_mtxTransform.extractQuaternion();

				m_vNoPivotData[keyFrameTime] = pData;
				mtxTransform = pData->m_mtxTransform;
				quatRotation = pData->m_quatRotation;
				return;
			}
		}
	}

	//void  Bone::calcMatrix(IRenderSystem *pRenderSystem,AnimationTime* pTime,int rangeIndex)
	//{
	//	m_bPivotRotation ? _calcMatrix_PivotRotation(pRenderSystem,pTime,rangeIndex) : _calcMatrix_NoPivotRotation(pRenderSystem,pTime,rangeIndex);
	//}

	void CoreBone::setKeyFramesTranslation(KeyFrames<Vector3>& keyFrames)
	{
		m_pTranslation = &keyFrames;
	}

	void CoreBone::setKeyFramesRotation(KeyFrames<Quaternion>& keyFrames)
	{
		m_pRotation = &keyFrames;
	}

	void CoreBone::setKeyFramesScale( KeyFrames<Vector3>& keyFrames)
	{
		m_pScale = &keyFrames;
	}
}