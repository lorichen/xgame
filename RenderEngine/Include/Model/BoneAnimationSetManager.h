#ifndef __BONEANIMATIONSETMANAGER_H__
#define __BONEANIMATIONSETMANAGER_H__

#include "RenderEngine/Manager.h"
#include "BoneAnimationSet.h"
namespace xs
{
	class BoneAnimationSetManager: public Manager< BoneAnimationSet * >, public Singleton<BoneAnimationSetManager>
	{
	public:
		BoneAnimationSetManager();
		virtual ~BoneAnimationSetManager();

	public:
		/** 加载骨骼动画
		@param fileName 骨骼动画文件名
		@return 骨骼动画指针
		*/
		BoneAnimationSet * loadBoneAnimationSet( const char * fileName);

		/** 从骨骼动画文件名称获取骨骼动画
		@param fileName 骨骼动画文件名
		@return 骨骼动画指针
		*/
		BoneAnimationSet * getByName(const char* fileName);

		/** 根据名字销毁骨骼动画
		@param fileName 骨骼动画文件名
		*/
		void releaseBoneAnimationSet( const char * fileName);

		/** 根据指针销毁骨骼动画
		@param fileName 骨骼动画指针
		*/
		void releaseBoneAnimationSet(  BoneAnimationSet * pAni);


	public:
		/** 实际的删除函数
		@param id 要删除的对象
		@remarks 需要实现此函数来做真正的删除
		*/
		virtual void  doDelete(BoneAnimationSet* id);

	private:
		struct BananaSkin
		{
			BoneAnimationSet * pAni;
			ManagedItem * pManagedItem;
		};
		typedef std::list<BananaSkin> RecycleBin;
		RecycleBin m_recycleBin;
		size_t	m_recycleBinSize;
	};
}
#endif