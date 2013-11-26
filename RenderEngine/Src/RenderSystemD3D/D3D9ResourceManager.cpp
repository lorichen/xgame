#include "StdAfx.h"
#include "D3D9ResourceManager.h"

namespace xs
{


	D3D9ResourceManager * D3D9ResourceManagerCreater::create()
	{
		D3D9ResourceManager * pMgr = new D3D9ResourceManager();

		//初始化
		if( !pMgr->create() )
		{
			pMgr->release();
			return 0;
		}

		return pMgr;
	}


	const  uint D3D9ResourceManager::cs_uiResSynAccessCount = 8;//资源并发访问的数量

	bool D3D9ResourceManager::create()
	{
		m_lRenderLockCount = 0;
		m_hRenderEvent  = ::CreateEvent(NULL, TRUE, TRUE, NULL);
		if( NULL == m_hRenderEvent) return false;
		else return true;	
	}


	D3D9ResourceManager::D3D9ResourceManager():
	m_ResourceLocker(cs_uiResSynAccessCount)//支持8个线程同时访问资源
	{

	}

	D3D9ResourceManager::~D3D9ResourceManager()
	{
		
	}

	void D3D9ResourceManager::release()
	{
		if( m_hRenderEvent) CloseHandle( m_hRenderEvent);
		delete this;
	}

	void D3D9ResourceManager::notifyOnDeviceLost()
	{
		ResourceContainerIterator it = m_Resources.begin();
		for( ; it != m_Resources.end(); ++it)
		{
			(*it)->notifyOnDeviceLost();
		}
	}

	void D3D9ResourceManager::notifyOnDeviceReset()
	{
		ResourceContainerIterator it = m_Resources.begin();
		for( ; it != m_Resources.end(); ++it)
		{
			(*it)->notifyOnDeviceReset();
		}
	}

	void D3D9ResourceManager::notifyResourceCreated(D3D9Resource* pResource)
	{	
		Assert( pResource != 0 );
		xs::ResGuard<xs::Mutex> Lock( m_ResourceMgrMutex );	
		m_Resources.insert( pResource);
	}


	void D3D9ResourceManager::notifyResourceRelease(D3D9Resource* pResource)
	{		
		xs::ResGuard<xs::Mutex> Lock( m_ResourceMgrMutex );
		m_Resources.erase(pResource);
	}

	//锁定所有线程对资源的访问
	void D3D9ResourceManager::lockAllResourceAccess()
	{
		for( uint i = 0; i< cs_uiResSynAccessCount; ++i)
		{
			m_ResourceLocker.Wait();
		}
	}

	//解锁，与lockAllResourceAccess对应
	void D3D9ResourceManager::unlockAllResourceAccess()
	{
		for( uint i = 0; i <  cs_uiResSynAccessCount; ++i)
		{
			m_ResourceLocker.Post();
		}
	}

	//
	void D3D9ResourceManager::beginFrame()
	{
		++m_lRenderLockCount;
		if( m_lRenderLockCount == 1) ::ResetEvent(m_hRenderEvent);
	}

	// 
	void D3D9ResourceManager::endFrame()
	{
		--m_lRenderLockCount;
		if( m_lRenderLockCount ==0) ::SetEvent( m_hRenderEvent);
		if( m_lRenderLockCount < 0 )
		{
			Info( "beginFrame or endFrame calls occure illegal!")
		}
		
	}

}