#include "stdafx.h"
#include "_ParticleSystemEmitterRealSphere.h"
#include "Bone.h"
#include "ModelNode.h"

namespace xs
{
	void ParticleSystemEmitterRealSphere::release()
	{
		delete this;
	}

	MmParticle ParticleSystemEmitterRealSphere::newParticle(int rangeIndex,AnimationTime *pTime)
	{
		MmParticle p;
		// TODO: maybe evaluate these outside the spawn function, since they will be common for a given frame?
		//对球形发射器，只考虑宽度，并且把宽度当成半径
		float w = m_pParticleSystem->m_pData->areal.getFrame(rangeIndex,pTime);
		//float l = m_pParticleSystem->m_pData->areaw.getFrame(rangeIndex,pTime) * 0.5f;
		float spd = m_pParticleSystem->m_pData->speed.getFrame(rangeIndex,pTime);
		float var = m_pParticleSystem->m_pData->variation.getFrame(rangeIndex,pTime);

		//球面发射器
		Matrix4 mtxY;
		mtxY.rotationX(Math::RangeRandom(0,180));
		Matrix4 mtxZ;
		mtxZ.rotationY(Math::RangeRandom(0,360));
		Vector3 bdir = mtxZ * mtxY * Vector3(0,1,0);
		bdir.normalize();
		bdir = w * bdir;


		Bone *pBone = m_pParticleSystem->m_pBone;
		//绑定到骨骼
		if( pBone && m_pParticleSystem->m_pData->attach2Bone != 0 )
		{
			//位置
			p.m_vPos = m_pParticleSystem->m_pData->pos + bdir;
			p.m_vPos = p.m_vPos + pBone->getPivot();
			//速度
			if( xs::Math::RealEqual(bdir.squaredLength(), 0) )
				p.m_vSpeed = Vector3::ZERO;
			else
			{					
				Vector3 dir = bdir;
				dir.normalize();
				p.m_vSpeed = dir * spd * (1.0f + Math::RangeRandom(-var,var));   // ?
			}
			//向下方向
			p.m_vDown = Vector3(0,-1,0);
		}
		//绝对坐标
		else
		{
			//位置
			p.m_vPos = m_pParticleSystem->m_pData->pos + bdir;
			if(pBone)
				p.m_vPos = pBone->getFullTransform()* ( pBone->getPivot() + p.m_vPos);
			if(m_pParticleSystem->m_pNode)
			{
				const Matrix4& mtx = m_pParticleSystem->m_pNode->getFullTransform();
				p.m_vPos = mtx * p.m_vPos;
			}

			//速度和向下方向
			if( xs::Math::RealEqual(bdir.squaredLength(), 0) )
			{
				p.m_vSpeed = Vector3::ZERO;
			}

			Vector3 dir = bdir;
			dir.normalize();
			if(pBone)
				dir = pBone->getFullRotation() * dir;
			if(m_pParticleSystem->m_pNode)
			{
				Matrix4 mtx = m_pParticleSystem->m_pNode->getFullTransform();
				Quaternion q = mtx.extractQuaternion();
				dir = q * dir;
			}
			dir.normalize();

			p.m_vSpeed = dir * spd * (1.0f + Math::RangeRandom(-var,var));   // ?
			p.m_vDown = Vector3(0,-1,0);
			p.m_vDown = dir * p.m_vDown;
		}

		//其它参数
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
	

	ParticleSystemEmitter* ParticleSystemEmitterFactoryRealSphere::newEmitter(MmParticleSystem *pParticleSystem)
	{
		return new ParticleSystemEmitterRealSphere(pParticleSystem);
	}
}
