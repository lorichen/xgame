#ifndef __ParticleSystemEmitterSphere_H__
#define __ParticleSystemEmitterSphere_H__

#include "_Particle.h"
#include "_ParticleSystem.h"
#include "_ParticleSystemEmitter.h"

namespace xs
{
	/** Sphere粒子系统发射器
	*/
	class ParticleSystemEmitterSphere : public ParticleSystemEmitter
	{
	public:
		ParticleSystemEmitterSphere(MmParticleSystem *pParticleSystem) : ParticleSystemEmitter(pParticleSystem){}
	public:
		virtual void	release();
		virtual MmParticle newParticle(int rangeIndex,AnimationTime *pTime);
	};

	/** Sphere粒子系统发射器工厂
	*/
	class ParticleSystemEmitterFactorySphere : public ParticleSystemEmitterFactory
	{
	protected:
		ParticleSystemEmitterFactorySphere(){}
	public:
		static ParticleSystemEmitterFactory* Instance()
		{
			static ParticleSystemEmitterFactorySphere e;
			return &e;
		}
	public:
		virtual ParticleSystemEmitter*	newEmitter(MmParticleSystem *pParticleSystem);
	};
}

#endif