#include "StdAfx.h"
#include "CoreSkeleton.h"
#include "Animation.h"

namespace xs
{
	CoreSkeleton::~CoreSkeleton()
	{
		STLDeleteSequence(m_vCoreBones);
	}

	CoreSkeleton::CoreSkeleton()
	{
	}

	uint	CoreSkeleton::getCoreBoneCount() const
	{
		return m_vCoreBones.size();
	}

	CoreBone*	CoreSkeleton::getCoreBone(ushort index) const
	{
		if(index < m_vCoreBones.size())return m_vCoreBones[index];

		return 0;
	}

	size_t		CoreSkeleton::getMemeorySize()
	{
		size_t memorySize = sizeof(this);
		size_t count = m_vCoreBones.size();
		for( size_t i =0; i<count; ++i)
			memorySize += m_vCoreBones[i]->getMemeorySize();

		return memorySize;
	}

	CoreBone*	CoreSkeleton::getCoreBone(const char *name) const
	{
		size_t size = m_vCoreBones.size();
		for(size_t i = 0;i < size;i++)
		{
			if(StringHelper::casecmp(m_vCoreBones[i]->getName(),name) == 0)return m_vCoreBones[i];
		}

		return 0;
	}

	CoreBone*	CoreSkeleton::createCoreBone()
	{
		CoreBone *pCoreBone = new CoreBone();
		m_vCoreBones.push_back(pCoreBone);
		return pCoreBone;
	}
}