/*
描述：骨骼动画集，用于保存骨骼动作。
*/
#ifndef __BONEANIMATIONSET_H__
#define __BONEANIMATIONSET_H__

namespace xs
{
	class BoneAnimationSet:public ManagedItem
	{
	public:
		BoneAnimationSet(std::string strFileName);
		~BoneAnimationSet();
	public:
		/**请不要使用release来释放模型，应该使用BoneAnimatinSetManager::releaseModel，因为模型是引用计数对象
		*/
		void release();

		/*获取所占用的内存的大小
		*/
		size_t getMemorySize();

		/*获取骨骼动画文件名
		*/
		const std::string& getFileName();

		/*解码动作文件
		*/
		bool decode(xs::Stream * data);

		/*按名称获取动画
		*/
		Animation * getAnimationByName(const std::string& animation);

		/*获取动画数量
		*/
		uint getAnimationCount();

		/*按序号获取动画
		*/
		Animation * getAnimationByIndex(uint index);

		///*获取骨骼数
		//*/
		//uint getBoneNum();

		///*获取骨骼数据
		//*/
		//BoneData * getBoneByIndex(uint index);

		/*获取骨架
		*/
		CoreSkeleton*	getCoreSkeletion();

		bool write(string filename,float fCopress);

	private:
		//创建骨架
		void createCoreSkeleton();

	private:
		std::vector<BoneData*>	m_vBones;//骨骼数据
		CoreSkeleton			m_skeleton;//骨架
		std::vector<Animation*>		m_vAnimations;//动画数值
		HASH_MAP_NAMESPACE::hash_map<std::string,Animation*>	m_AnimationMap;//动画映射表
		uint m_version;//版本号
		size_t m_memorySize;//所占用的内存数
	};
}

#endif