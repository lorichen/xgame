// Win32Dll.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "LoadResourceThread.h"
#include "ResourceManager.h"
#include "Thread.h"

xs::ResourceManager *gResourceManager = 0;

namespace xs
{

	ResourceManager::ResourceManager()
	{
		memset(&m_ResourceLoaders, 0, sizeof(m_ResourceLoaders));
		m_resLoadModel = modeAsynch;
	}
	ResourceManager::~ResourceManager()
	{

	}

	void ResourceManager::grow(ulong size)
	{
		m_LoadedResPool.grow(size);
	}

	void ResourceManager::releaseResourceNode(ResourceNode* node)
	{
		m_threadLoader.releaseRes(node);
	}

	bool  ResourceManager::create(int threads) 
	{
		m_bImmediately = threads == 0?true : false ;
		if(m_bImmediately)
			return true ;
		return m_threadLoader.create(threads);
	}
	//保存接口指针
	void ResourceManager::registerResourceLoader(ResourceType type, IResourceLoader* resLoader)
	{
		Assert(type >= 0 && type < ResourceTypeMaxCount);
		if (type >= 0 && type < ResourceTypeMaxCount)
		{
			if (!m_ResourceLoaders[type])
				m_ResourceLoaders[type] = resLoader ;
			else
				Error("ResourceLoader(type="<<type<<") had registed"<<endl);
		}
		else
		{
			Error("Invalid resource type("<<type<<")"<<endl);
		}
	}


	void ResourceManager::unregisterResourceLoader(ResourceType type)
	{
		Assert(type >= 0 && type < ResourceTypeMaxCount);
		if (type >= 0 && type < ResourceTypeMaxCount)
			safeRelease(m_ResourceLoaders[type]);
	}

	handle ResourceManager::requestResource(const ulong uResID/*const std::string& resName*/, ResourceType type, bool bImmediately/* = false*/, int priority/* = 0*/)
	{
		Assert(type >= 0 && type < ResourceTypeMaxCount);
		if (type < 0 || type >= ResourceTypeMaxCount)
		{
			ErrorOnce("ResourceType("<<type<<") is Error!"<<endl);
			return INVALID_HANDLE;
		}
		if (!m_ResourceLoaders[type]) // 对应类型加载器未注册
		{
			ErrorOnce("ResourceType("<<type<<") not registed"<<endl);
			return INVALID_HANDLE;
		}

		//看是否已经有了资源
		/*ulong uKey = STR2ID(resName.c_str());*/
		LONGLONG uid = 0;
		uid |= uResID;
		uid <<= 32;//移动32位
		uid |= type;

		ResourceNode* node = m_LoadedResPool.find(uid);
		if (node)
		{
			//Info("m_LoadedResPool Found!"<<(ulong)uResID<<endl);
			return node->m_Handle;
		}

		node = new ResourceNode(uResID, uid);
		//char str[256];
		//sprintf(str,"new ResourceNode:%p\n",node);
		//OutputDebugString(str);
		node->m_Handle = rktCreateHandle(static_cast<IResourceNode*>(node));

		node->setResourceLoader(m_ResourceLoaders[type]);
		node->setState(stateAdded);
		node->setPriority(priority);
		node->setType(type);
		 
		if (!m_bImmediately && !bImmediately && m_resLoadModel == modeAsynch)
		{	
			m_threadLoader.loadRes(node);
		}
		else // 立即加载
		{
			//Info("node->load!"<<(ulong)uResID<<endl);
			if (!node->load())
			{
				//加载资源出错了,但是节点资源依然保留,这样就不会重复加载错误的资源
				//Error("Load resource failed, type: "<<type<<", name: "<<resName<<endl);	
				//Info("node->load! error"<<(ulong)uResID<<endl);
			}
		}


		/*if (type != typeResourceModel)*/
		//delete node;
		//return 0;
		m_LoadedResPool.addNode(node) ;
		//node->release();
		//delete node;

		return node->m_Handle;
	}

	/** 卸载指定的资源,等待列表中或者正在加载的资源不卸载
	*/
	bool ResourceManager::releaseResource(handle resHandle, bool bDelayRecycle/* = false*/)
	{
		if (!isValidHandle(resHandle)) // ignore!!
			return false;

		IResourceNode* resNode = (IResourceNode*)getHandleData(resHandle);
		Assert(resNode != NULL);

		ResourceNode* node = static_cast<ResourceNode*>(resNode);
		if(node)
			return m_LoadedResPool.removeNode(node->getId(), bDelayRecycle);
		return false;
	}

	void ResourceManager::setLoadMode(ResLoadModel mode)
	{
		m_resLoadModel = mode;
	}

	const ResLoadModel  ResourceManager::getLoadMode() const
	{
		return m_resLoadModel;
	}

	void ResourceManager::stop()
	{
		//首先关闭线程		
		m_threadLoader.close();
	}

	void ResourceManager::release()
	{
		//首先关闭线程		
		m_threadLoader.close();

		m_LoadedResPool.close() ;

		for (int i=0; i<ResourceTypeMaxCount; i++)
			safeRelease(m_ResourceLoaders[i]);

		delete this;
	}


	API_EXPORT IResourceManager* LoadRMM(int threads)
	{

		ResourceManager* resourceManager = new ResourceManager();
		Assert(resourceManager != NULL);
		if (!resourceManager)
			return NULL;
		if (!resourceManager->create(threads))
		{
			Error("Create Resource Manager failed, threads:"<<threads<<endl);
			return NULL;
		}
		gResourceManager = resourceManager;

		return static_cast<IResourceManager*>(resourceManager);
	}

}
