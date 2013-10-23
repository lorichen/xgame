#ifndef __Skeleton_H__
#define __Skeleton_H__
#include "Bone.h"

namespace xs
{
	class CoreSkeleton;
	class Bone;

	class _RenderEngineExport Skeleton
	{
	public:
		///构造函数
		Skeleton();

		///析构函数
		~Skeleton();

		///创建函数
		bool	create(CoreSkeleton* pCoreSkeleton);

		/// 获得骨骼的数量
		uint	getBoneCount() const;

		/// 获得第index个骨骼
		Bone*	getBone(ushort index) const;

		/// 根据名称获得骨骼
		Bone*	getBone(const char *name) const;

		/// 计算传递矩阵
		void 	calcMatrix(AnimationTime* pTime,int rangeIndex = -1);

	private:
		CoreSkeleton*		m_pCoreSkeleton;//关联的真正的骨架
		std::vector<Bone*>	m_vBones;
	};
}
#endif