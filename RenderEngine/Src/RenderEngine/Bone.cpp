#include "StdAfx.h"
#include "CoreBone.h"
#include "Bone.h"

namespace xs
{

	void Bone::setName(const char *name)
	{
		m_name = name;
	}

	void Bone::setPivotRotation(bool pivotRotation)
	{
		m_bPivotRotation = pivotRotation;
	}

	void Bone::setPivot(const Vector3& vPivot)
	{
		m_vPivot = vPivot;
	}

	void Bone::setParent(Bone * pBone)
	{
		m_pParent = pBone;
	}

	void Bone::setPrecomputeMatrix(const Matrix4& mtx)
	{
		m_precomputeMtx = mtx;
	}

	const char* Bone::getName()
	{
		return m_name.c_str();
	}

	Bone*	Bone::getParent()
	{
		return m_pParent;
	}

	const Vector3& Bone::getPivot()
	{
		return m_vPivot;
	}

	bool Bone::getNeedUpdate()
	{
		return m_bNeedUpdate;
	}

	void Bone::setNeedUpdate(bool needUpdate)
	{
		m_bNeedUpdate = needUpdate;
	}

	void Bone::calcMatrix(AnimationTime * pTime, int rangeIndex)
	{
		if( !m_bNeedUpdate) return; //不需要更新了

		if( m_pParent )
		{
			m_pParent->calcMatrix(pTime, rangeIndex);
			m_pCoreBone->getBoneData(pTime, rangeIndex,m_mtxTransform,m_quatRotation);
			m_mtxTransform =m_pParent->m_mtxTransform /*m_pParent->getFullTransform()*/ * m_mtxTransform;
			m_quatRotation =m_pParent->m_quatRotation /*m_pParent->getFullRotation()*/ * m_quatRotation;
			m_vTransformedPivot = m_mtxTransform * m_vPivot;
		}
		else
		{
			m_pCoreBone->getBoneData(pTime, rangeIndex,m_mtxTransform,m_quatRotation);
			m_vTransformedPivot = m_mtxTransform * m_vPivot;		
		}

		m_bNeedUpdate = false;  //已经更新了
	}

}