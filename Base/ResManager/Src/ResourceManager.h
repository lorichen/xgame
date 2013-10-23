//==========================================================================
/**
* @file	  : ResourceManager.h
* @author : 
* created : 2008-1-18   13:59
* purpose : 
*/
//==========================================================================

#ifndef __RESOURCEMANAGER_H__
#define __RESOURCEMANAGER_H__

#include "IResourceManager.h"
#include "ResourceNode.h"
#include "LoadResourceThread.h"

#include <hash_map>

namespace xs
{
	class ResourceThreadLoader ;

	/** 资源管理类
	*/
	class ResourceManager : public IResourceManager
	{
		IResourceLoader*				m_ResourceLoaders[ResourceTypeMaxCount]; /// 加载器列表
		bool							m_bImmediately;		/// 是否立即加载资源（不使用后台线程加载）
		ResPool<ResourceNode*, LONGLONG>	m_LoadedResPool;	/// 资源的缓冲池

		ResourceThreadLoader            m_threadLoader ;

		ResLoadModel					m_resLoadModel;     //资源加载模式


	public:
		ResourceManager();
		virtual ~ResourceManager();

		void grow(ulong size);
		
		void releaseResourceNode(ResourceNode* node);

		/** 创建线程序
		@param threads 创建的线程序数量
		@note  因为Opengl多线程支持方面有些技术还没有弄清楚,创建2线程序之后不正常\n
		目前只能够创建一个线程,以后考虑实现
		*/
		bool     create(int threads) ;

		// IResourceManager
	public:
		virtual void registerResourceLoader(ResourceType type, IResourceLoader* resLoader);
		virtual void unregisterResourceLoader(ResourceType type);
		virtual handle requestResource(const ulong uResID/*const std::string& resName*/, ResourceType type, bool bImmediately = false, int priority = 0);
		virtual bool releaseResource(handle resHandle, bool bDelayRecycle = false);
		/** 设置资源加载模式,
		@param mode modeSynch 同步 modeAsynch 异步
		*/
		virtual void setLoadMode(ResLoadModel mode); 
		virtual const ResLoadModel  getLoadMode() const; 
		/** 停止加载线程
		*/
		virtual void stop();

		virtual void release();
	};

} // namespace

extern xs::ResourceManager *gResourceManager;

#endif // __RESOURCEMANAGER_H__
