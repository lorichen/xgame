#include "StdAfx.h"
#include "CoreSkeleton.h"
#include "Skeleton.h"

namespace xs
{
	Skeleton::Skeleton():m_pCoreSkeleton(0)
	{
	}

	Skeleton::~Skeleton()
	{
		STLDeleteSequence(m_vBones);
	}

	bool Skeleton::create(CoreSkeleton* pCoreSkeleton)
	{
		if( 0 == pCoreSkeleton ) 
			return false;

		m_pCoreSkeleton = pCoreSkeleton;

		typedef std::map<CoreBone*, Bone*> MapCoreBone2Bone;
		MapCoreBone2Bone mapFindParent;

		uint count = pCoreSkeleton->getCoreBoneCount();
		for( uint i =0; i <count; ++i)
		{
			CoreBone * pCoreBone = pCoreSkeleton->getCoreBone(i);
			Bone* pBone = new Bone(pCoreBone);
			pBone->setName( pCoreBone->getName() );
			pBone->setPivot( pCoreBone->getPivot() );
			pBone->setPivotRotation( pCoreBone->getPivotRotation() );
			pBone->setInitMatrix( pCoreBone->getInitMatrix() );
			pBone->setPrecomputeMatrix( pCoreBone->getPrecomputeMatrix() );
			m_vBones.push_back( pBone);
			mapFindParent[pCoreBone] = pBone;
		}

		for( uint i =0; i<count; ++i)
		{
			MapCoreBone2Bone::iterator it =mapFindParent.find(pCoreSkeleton->getCoreBone(i)->getParent() );
			if( it != mapFindParent.end() )
			{
				m_vBones[i]->setParent( it->second );
			}
			//CoreBone * pCoreBoneParent = pCoreSkeleton->getCoreBone(i)->getParent();
			//if( 0 != pCoreBoneParent )
			//{
			//	for( uint j=0; j<count; ++j)
			//	{
			//		CoreBone* pParent = pCoreSkeleton->getCoreBone(j);
			//		if(  pCoreBoneParent == pParent )
			//		{
			//			m_vBones[i]->setParent( m_vBones[j]);
			//			break;
			//		}
			//	}
			//}
		}
		
		return true;
	}

	uint	Skeleton::getBoneCount() const
	{
		return m_vBones.size();
	}


	Bone*	Skeleton::getBone(ushort index) const
	{
		if( index < m_vBones.size() ) return m_vBones[index];
		return 0;
	}

	Bone*	Skeleton::getBone(const char *name) const
	{
		size_t size = m_vBones.size();
		for( uint i=0; i<m_vBones.size(); ++i)
		{
			if(StringHelper::casecmp(m_vBones[i]->getName(),name) == 0)return m_vBones[i];
		}
		return 0;
	}

	void 	Skeleton::calcMatrix(AnimationTime* pTime,int rangeIndex)
	{
		size_t size = m_vBones.size();

		for( size_t i =0; i<size; ++i)
		{
			m_vBones[i]->setNeedUpdate(true);
		}

		for(size_t i = 0;i < size; ++i)
		{
			m_vBones[i]->calcMatrix(pTime,rangeIndex);
		}
	}
}