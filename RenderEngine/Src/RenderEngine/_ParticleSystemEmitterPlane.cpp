#include "stdafx.h"
#include "_ParticleSystemEmitterPlane.h"
#include "Bone.h"
#include "ModelNode.h"

namespace xs
{
	void	ParticleSystemEmitterPlane::release()
	{
		delete this;
	}

	MmParticle ParticleSystemEmitterPlane::newParticle(int rangeIndex,AnimationTime *pTime)
	{
		MmParticle p;
		// TODO: maybe evaluate these outside the spawn function, since they will be common for a given frame?
		float w = m_pParticleSystem->m_pData->areal.getFrame(rangeIndex,pTime) * 0.5f;
		float l = m_pParticleSystem->m_pData->areaw.getFrame(rangeIndex,pTime) * 0.5f;
		float spd = m_pParticleSystem->m_pData->speed.getFrame(rangeIndex,pTime);
		float var = m_pParticleSystem->m_pData->variation.getFrame(rangeIndex,pTime);

		Bone *pBone = m_pParticleSystem->m_pBone;
		//绑定到骨骼
		if( pBone && m_pParticleSystem->m_pData->attach2Bone != 0 )
		{
			//位置
			p.m_vPos = m_pParticleSystem->m_pData->pos + Vector3(Math::RangeRandom(-l,l),0,Math::RangeRandom(-w,w));
			p.m_vPos = p.m_vPos + pBone->getPivot();
			//向下方向
			p.m_vDown = Vector3(0,-1,0);
			//速度
			p.m_vSpeed = xs::Vector3::UNIT_Y * spd * (1.0f + Math::RangeRandom(-var,var));
		}
		else//世界绝对位置
		{
			//位置
			p.m_vPos = m_pParticleSystem->m_pData->pos + Vector3(Math::RangeRandom(-l,l),0,Math::RangeRandom(-w,w));
			if(pBone)
				p.m_vPos = pBone->getFullTransform() *( pBone->getPivot() + p.m_vPos);
			if(m_pParticleSystem->m_pNode)
			{
				const Matrix4& mtx = m_pParticleSystem->m_pNode->getFullTransform();
				p.m_vPos = mtx * p.m_vPos;
			}

			//向上方向
			Vector3 dir = Vector3(0,1,0);
			if(pBone)
			{
				dir = pBone->getFullRotation() * dir;
			}
			if(m_pParticleSystem->m_pNode)
			{
				Matrix4 mtx = m_pParticleSystem->m_pNode->getFullTransform();
				Quaternion q = mtx.extractQuaternion();
				dir = q * dir;
			}
			dir.normalize();
			//向下方向
			p.m_vDown = -1 * dir;
			p.m_vSpeed = dir * spd * (1.0f + Math::RangeRandom(-var,var));
		}

		p.m_fLife = 0;
		p.m_fMaxLife = m_pParticleSystem->m_pData->lifespan.getFrame(rangeIndex,pTime);
		p.m_vOrigin = p.m_vPos;
		// 随机选择分割的纹理 [5/18/2011 zgz]
		if (0 == m_pParticleSystem->m_pData->iteratorArea[0] && 0 == m_pParticleSystem->m_pData->iteratorArea[1])
		{
			p.m_i32Tile = Math::RangeRandom(0,m_pParticleSystem->m_pData->rows * m_pParticleSystem->m_pData->cols - 1);
		}
		

		return p;
	}

	ParticleSystemEmitter*	ParticleSystemEmitterFactoryPlane::newEmitter(MmParticleSystem *pParticleSystem)
	{
		return new ParticleSystemEmitterPlane(pParticleSystem);
	}
}