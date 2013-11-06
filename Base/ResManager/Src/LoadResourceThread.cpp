//==========================================================================
/**
* @file	  : LoadRecourceThread.h
* @author : 
* created : 2008-1-21  22:32
* purpose : 
*/
//==========================================================================

#include "stdafx.h"
#include "LoadResourceThread.h"
#include "ResourceNode.h"
#include <queue>
namespace xs
{
	
	void LoadTask::run()
	{

		IRenderSystem* pRenderSystem = m_pResourceThreadLoader->getRenderSystem();
		uint key = pRenderSystem->MTaddRenderTarget();
		m_semaphoreInit.Post();
		//任务循环处理
		while(true)
		{
			if(m_pResourceThreadLoader->isExit())
				break;
			
			ResourceNode* node = m_pResourceThreadLoader->getNode(m_bGroundPrivate);
			//没有加载的资源
			if(node == 0)
			{
				//没有资源加载
				m_semaphore.Wait(300);
			}
			else
			{
				m_stateTask = stateTaskBusy;
				//char str[256];
				//sprintf(str,"node->load;%p\n",node);
				//OutputDebugString(str);
				switch(node->getState())
				{
				case stateAdded:
					{
						node->load();
					}
					break;
				case stateRelease:
					{
						safeRelease(node);
					}
					break;
				default:
					break;
				}
				m_stateTask = stateTaskIdle;
			}
		}
		pRenderSystem->MTremoveRenderTarget(key);
		m_stateTask = stateTaskEnd;
	}
	void LoadTask::release()
	{
		//非正常状态下退出,即run函数没有结束循环的情况下,无法结束本函数.
		while(true)
		{
			if(m_stateTask == stateTaskEnd)
				break;
			m_semaphore.Wait(100);
		}
		delete this;
	}

/*
	bool mygreater::operator()(const ResourceNode*& _Left, const ResourceNode*& _Right) const
	{	
		return _Left->getPriority() > _Right->getPriority();
	}*/

	/**创建线程
	@param threads 线程数量
	@return 成功返回true,失败返回false
	@note  目前只支持一个线程,这个与opengl多线程支持技术有关,以后需要完善
	*/
	bool ResourceThreadLoader::create(int threads)
	{
		//清除线程池
		int i ;
		LoadTask* pLoadTask;
		close();
		m_exitFlag = false;
		m_threadMaxCount = threads ;//threads,force to 1 now 
		m_pRenderSystem = getRenderEngine()->getRenderSystem();
		m_masterRenderTarget = m_pRenderSystem->getCurrentRenderTarget() ;		
		m_pRenderSystem->setCurrentRenderTarget(0);

		for ( i = 0 ; i<m_threadMaxCount; i++)
		{
			bool bGroundPrivate = false;
			if (m_threadMaxCount > 1 && 0 == i)
			{
				// 如果有多个资源加载线程，则第一个创建的线程作为地表加载专用线程
				bGroundPrivate = true;
			}
			pLoadTask = new LoadTask(this, bGroundPrivate);
			m_taskList.push_back(pLoadTask);
			//m_threadPool.add(pLoadTask,THREAD_PRIORITY_LOWEST);
			//加高一点加载线程的优先级别 by yhc
			//crr 0210-07-09 设置亲缘性，只允许其运行在第一个cpu，这样多cpu时不会和主线程争时间片
			HANDLE hThread = m_threadPool.add(pLoadTask,Thread::Low);
			//SetThreadAffinityMask(hThread, 0x00000001);
			pLoadTask->wait();
		}
		m_pRenderSystem->setCurrentRenderTarget(m_masterRenderTarget);
		
		return true;
	}

	/** 提交加载线程资源节点信息
	@node 线程资源节点信息
	*/
	void ResourceThreadLoader::loadRes(ResourceNode*node)
	{
		if (NULL == node)
		{
			return;
		}
		int nTaskNum = m_taskList.size();
		if (nTaskNum > 1 && node->getType() == typeResourceGroundTile)
		{
			m_mutexQueueGround.Lock();
			m_queueNodesGround.push(node);
			m_mutexQueueGround.Unlock();
		}
		else
		{
			m_mutexQueue.Lock();
			m_queueNodes.push(node);
			m_mutexQueue.Unlock();
		}
		
		std::list<LoadTask*>::iterator it = m_taskList.begin();
		while(it != m_taskList.end())
		{
			if (nTaskNum > 1 && node->getType() == typeResourceGroundTile && !(*it)->IsGroundPrivate())
			{
				it++;
				continue;
			}

			if((*it)->post())
				break;
			it++;
		}
	}

	/** 为了复用代码，待删除的节点也由资源加载线程去释放
	@node 线程资源节点信息
	*/
	void ResourceThreadLoader::releaseRes(ResourceNode* node)
	{
		loadRes(node);
	}

	ResourceNode* ResourceThreadLoader::getNode(bool bGroundNode/* = false*/)
	{
		ResourceNode* pNode = 0;
		if (bGroundNode)
		{
			m_mutexQueueGround.Lock();
			if(!m_queueNodesGround.empty())
			{
				pNode = m_queueNodesGround.top();
				m_queueNodesGround.pop();
			}
			m_mutexQueueGround.Unlock();
		}
		else
		{
			m_mutexQueue.Lock();
			if(!m_queueNodes.empty())
			{
				pNode = m_queueNodes.top();
				m_queueNodes.pop();
			}
			m_mutexQueue.Unlock();
		}		
		return pNode;
	}
	
	/** 关闭线程
	*/
	void ResourceThreadLoader::close()
	{
		m_exitFlag = true ;		
		m_taskList.clear();
		m_threadPool.wait();
		m_threadPool.clear();
	}
}