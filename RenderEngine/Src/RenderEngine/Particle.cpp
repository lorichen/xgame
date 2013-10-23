#include "StdAfx.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "ModelNode.h"
#include "Bone.h"


//如果粒子绑定到骨骼，则重力，速度等都是相对于粒子的父节点来说的，而不是相对于根节点来说的

namespace xs
{
	bool Particle::update(float tickDelta,IRenderSystem *pRenderSystem)
	{
		float timeFactor;

		timeFactor = static_cast<float>(tickDelta) / 1000.0f;

		m_life -= timeFactor;
		if(m_life <= 0.0f) return false;
		
		//更新位置
		m_v.y += (m_gravity * timeFactor);
		m_p += (m_v * timeFactor);

		float lifeFactor = Math::RealEqual(m_pData->lifeSpan,0.0f) ? 0.0f : (1.0f - (m_life / m_pData->lifeSpan));
		float cellWidth = (m_pData->columns == 0) ? 1.0f : (1.0f / m_pData->columns);
		float cellHeight = (m_pData->rows == 0) ? 1.0f : (1.0f / m_pData->rows);

		Color4 color1,color2,color3;
		color1.r = m_pData->segmentColor1.r;
		color1.g = m_pData->segmentColor1.g;
		color1.b = m_pData->segmentColor1.b;
		color1.a = m_pData->alpha.x;

		color2.r = m_pData->segmentColor2.r;
		color2.g = m_pData->segmentColor2.g;
		color2.b = m_pData->segmentColor2.b;
		color2.a = m_pData->alpha.y;

		color3.r = m_pData->segmentColor3.r;
		color3.g = m_pData->segmentColor3.g;
		color3.b = m_pData->segmentColor3.b;
		color3.a = m_pData->alpha.z;

		float scale;
		Color4 color;
		if(lifeFactor < m_pData->time)
		{
			float tempInterval = m_pData->time;
			float tempFactor = Math::RealEqual(tempInterval,0.0f) ? 0.0f : (lifeFactor / tempInterval);

			scale = m_pData->particleScaling.x + tempFactor * (m_pData->particleScaling.y - m_pData->particleScaling.x);
			color = interpolate(tempFactor,color1,color2);
		}
		else
		{
			float tempInterval = 1.0f - m_pData->time;
			float tempFactor = (tempInterval == 0.0f) ? 0.0f : ((lifeFactor - m_pData->time) / tempInterval);

			scale = m_pData->particleScaling.y + tempFactor * (m_pData->particleScaling.z - m_pData->particleScaling.y);
			color = interpolate(tempFactor,color2,color3);
		}

		if( m_pParent->m_pData->attach2Bone)
		{
			//绑定到骨骼，不需要父节点的放大率
		}
		else
		{
			if(m_pParent->m_pNode)
			{
				Matrix4 mtx = m_pParent->m_pNode->getFullTransform();
				Vector3 t,s;
				Quaternion q;
				mtx.resolve(t,q,s);
				scale *= s.x;
			}		
		}
		m_scale = scale;
		

		int totalFrames = m_numHeadFrames + m_numHeadDecayFrames + m_numTailFrames + m_numTailDecayFrames;
		m_currentFrame = static_cast<int>(lifeFactor * totalFrames);

		int index;
		int frameStart,frameEnd,frameInterval,frameIntervalStart;
		if(m_currentFrame < m_numHeadFrames)
		{
			frameStart = static_cast<int>(m_pData->headLifeSpan.x);
			frameEnd = static_cast<int>(m_pData->headLifeSpan.y);
			frameInterval = frameEnd - frameStart + 1;
			frameIntervalStart = 0;

			index = frameStart + ((m_currentFrame - frameIntervalStart) % frameInterval);
		}
		else if(m_currentFrame < (m_numHeadFrames + m_numHeadDecayFrames))
		{
			frameStart = static_cast<int>(m_pData->headDecay.x);
			frameEnd = static_cast<int>(m_pData->headDecay.y);
			frameInterval = frameEnd - frameStart + 1;
			frameIntervalStart = m_numHeadFrames;

			index = frameStart + ((m_currentFrame - frameIntervalStart) % frameInterval);
		}
		else if(m_currentFrame < (m_numHeadFrames + m_numHeadDecayFrames + m_numTailFrames))
		{
			frameStart = static_cast<int>(m_pData->tailLifeSpan.x);
			frameEnd = static_cast<int>(m_pData->tailLifeSpan.y);
			frameInterval = frameEnd - frameStart + 1;
			frameIntervalStart = m_numHeadFrames + m_numHeadDecayFrames;

			index = frameStart + ((m_currentFrame - frameIntervalStart) % frameInterval);
		}
		else if(m_currentFrame < (m_numHeadFrames + m_numHeadDecayFrames + m_numTailFrames + m_numTailDecayFrames))
		{
			frameStart = static_cast<int>(m_pData->tailDecay.x);
			frameEnd = static_cast<int>(m_pData->tailDecay.y);
			frameInterval = frameEnd - frameStart + 1;
			frameIntervalStart = m_numHeadFrames + m_numHeadDecayFrames + m_numTailFrames;

			index = frameStart + ((m_currentFrame - frameIntervalStart) % frameInterval);
		}
		else
		{
			index = 0;
		}

		int row = (m_pData->columns == 0) ? 0 : (index / m_pData->columns);
		int column = (m_pData->columns == 0) ? 0 : (index % m_pData->columns);

		float left = cellWidth * column;
		float top = cellHeight * row;
		float right = cellWidth * (column + 1);
		float bottom = cellHeight * (row + 1);

		m_t[0] = Vector2(left,top);
		m_t[1] = Vector2(left,bottom);
		m_t[2] = Vector2(right,bottom);
		m_t[3] = Vector2(right,top);

		m_fullColor = m_pParent->m_color * color;

		m_ftickDelta += tickDelta;	
		if (0 != m_pData->nIntervalTime && 0 != m_pData->nAngle)
		{
			m_fSelfRotAngle = (m_ftickDelta / m_pData->nIntervalTime) * m_pData->nAngle;
		}
		return TRUE;
	}
}