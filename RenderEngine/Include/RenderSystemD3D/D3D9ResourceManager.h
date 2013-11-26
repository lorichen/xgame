#ifndef __D3D9ResourceManager_H__
#define __D3D9ResourceManager_H__

#include <set>

namespace xs
{
	//D3D9资源管理接口
	class D3D9Resource
	{
	public:
		// 设备丢失，在这里释放Default Pool管理的资源
		virtual void notifyOnDeviceLost() {}

		//设备恢复, 在这里重建Default Pool 管理的资源
		virtual void notifyOnDeviceReset() {}
	};


	
	class D3D9ResourceManager;
	//D3D9资源管理器创建工厂
	class D3D9ResourceManagerCreater
	{
	public:
		static D3D9ResourceManager * create();
	};

	//D3D9 资源管理器
	class  D3D9ResourceManager
	{
	public://这些函数已经同步好了，不需要调用锁定函数

		//通知设备丢失
		void notifyOnDeviceLost	();

		//通知设备重置
		void notifyOnDeviceReset();

		// 资源创建时的回调函数
		void notifyResourceCreated		(D3D9Resource* pResource);

		// 资源销毁时的回调函数
		void notifyResourceRelease		(D3D9Resource* pResource);


	public: //留给用户自己锁定的。这些函数的调用必须成对出现

		// 考虑到多线程问题,用于锁定对设备的访问
		inline void lockDeviceAccess() { m_DeviceMutex.Lock(); };

		// 考虑到多线程问题,用于解锁对设备的任何访问
		inline void unlockDeviceAccess() { m_DeviceMutex.Unlock(); };

		// 考虑到多线程问题, 用于锁定对资源管理器的访问
		inline void lockResourceMgrAccess() { m_ResourceMgrMutex.Lock(); };

		// 考虑到多线程问题, 用于解锁对资源管理器的访问
		inline void unlockResourceMgrAccess() {m_ResourceMgrMutex.Unlock(); };

		//考虑到多线程问题,用于锁定资源的访问
		inline void lockResourceAccess() { m_ResourceLocker.Wait() ; /*m_ResourceMutex.Lock();*/ };
		
		//考虑到多线程的问题，用于解锁对资源的访问
		inline void unlockResourceAccess()  { m_ResourceLocker.Post();  /*m_ResourceMutex.Unlock();*/ };

		//锁定所有线程对资源的访问
		void lockAllResourceAccess();

		//解锁，与lockAllResourceAccess对应
		void unlockAllResourceAccess();

		// 通知开始进行渲染了，所有的资源创建暂停
		void beginFrame();

		// 通知渲染完毕，暂停的资源创建可以继续了
		void endFrame();

		//等待绘制命令结束，在结束后可以创建资源
		void waitForEndFrame() { ::WaitForSingleObject(m_hRenderEvent, INFINITE); };
		

	public:
		//释放资源管理器，调用时必须保证辅助线程已经结束了。
		void release();		

	private:
		bool create();
		D3D9ResourceManager();
		~D3D9ResourceManager();
		//友元
	protected:
		friend class D3D9Resource;	
		friend class D3D9ResourceManagerCreater;

	protected:
		typedef std::set<D3D9Resource*>			ResourceContainer;
		typedef ResourceContainer::iterator		ResourceContainerIterator;	
		ResourceContainer			m_Resources;//资源容器

		xs::Mutex					m_DeviceMutex;		//设备访问互斥锁
		//xs::Mutex					m_ResourceMutex;	//资源互斥锁
		xs::Semaphore				m_ResourceLocker;	//资源互斥锁
		const static uint			cs_uiResSynAccessCount;//资源并发访问的数量
		xs::Mutex					m_ResourceMgrMutex;	//	资源管理器访问互斥锁

		HANDLE						m_hRenderEvent;
		LONG						m_lRenderLockCount;	
	};
}

#endif
