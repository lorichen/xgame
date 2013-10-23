#ifndef __CoreSkeleton_H__
#define __CoreSkeleton_H__

#include "CoreBone.h"

namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	//2010年12月8日：骨骼不支持billboard，不支持插值，但会缓存计算好的骨骼矩阵。

	/** 骨架，管理骨骼以及骨骼的层次关系
	*/
	class _RenderEngineExport CoreSkeleton
	{
	public:
		///
		CoreSkeleton();

		///
		~CoreSkeleton();

		/// 创建一个骨骼
		CoreBone*	createCoreBone();

		/// 获得骨骼的数量
		uint	getCoreBoneCount() const;

		/// 获得第index个骨骼
		CoreBone*	getCoreBone(ushort index) const;

		/// 根据名称获得骨骼
		CoreBone*	getCoreBone(const char *name) const;

		/// 获取骨架的大小,这个函数不要经常调用
		size_t		getMemeorySize();
		
	private:
		std::vector<CoreBone*>		m_vCoreBones;	
	};
	/** @} */
}

#endif