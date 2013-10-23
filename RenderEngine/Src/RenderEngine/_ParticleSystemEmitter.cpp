#include "stdafx.h"
#include "_ParticleSystemEmitter.h"
#include "_ParticleSystem.h"

namespace xs
{
	ParticleSystemEmitterFactory::ParticleSystemEmitterFactoryList	ParticleSystemEmitterFactory::m_Factories;

	void ParticleSystemEmitterFactory::registerEmitterFactory(uint type,ParticleSystemEmitterFactory *pFactory)
	{
		m_Factories[type] = pFactory;
	}

	void ParticleSystemEmitterFactory::unregisterEmitterFactory(uint type)
	{
		m_Factories.erase(type);
	}

	ParticleSystemEmitter *ParticleSystemEmitterFactory::newEmitter(uint type,MmParticleSystem *pParticleSystem)
	{
		ParticleSystemEmitterFactoryList::iterator it = m_Factories.find(type);
		if(it != m_Factories.end())
		{
			return (*it).second->newEmitter(pParticleSystem);
		}

		return 0;
	}
}