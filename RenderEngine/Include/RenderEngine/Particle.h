#ifndef __Particle_H__
#define __Particle_H__

namespace xs
{
	class ParticleSystem;

	class _RenderEngineExport Particle
	{
	public:
		Vector3		m_originPos;
		Vector3		m_p;
		Vector3		m_v;
		float		m_life;

		int m_currentFrame;
		int m_numHeadFrames;
		int m_numHeadDecayFrames;
		int m_numTailFrames;
		int m_numTailDecayFrames;

		float		m_scale;//缩放
		Vector2		m_t[4];
		Color4		m_color;
		Color4		m_fullColor;

		float		m_gravity;

		ParticleEmitter2Data*	m_pData;
		ParticleSystem*			m_pParent;

		float  m_ftickDelta;
		float  m_fSelfRotAngle; //自身绕Z轴旋转角度 zgz
	public:
		Particle() : m_ftickDelta(0.0f), m_fSelfRotAngle(0.0f)
		{

		}
		bool update(float tickDelta,IRenderSystem *pRenderSystem);
	};
}

#endif