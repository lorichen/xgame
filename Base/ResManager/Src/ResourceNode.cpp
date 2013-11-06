//==========================================================================
/**
* @file	  : Resource.cpp
* @author : 
* created : 2008-1-18   13:50
*/
//==========================================================================

#include "stdafx.h"
#include "ResourceManager.h"
#include "ResourceNode.h"


namespace xs {

    template <class PoolNodeType, class KeyType>
    void ResPool<PoolNodeType,KeyType>::checkSize()
    {
        // 如果溢出，则释放可回收列表头部的节点，直到缓冲池不溢出
        if (isOverflow() && mRecycledNodeList.size() > 0)
        {
            //modified by xxh 20091104， 对处于状态stateError的东西，以前的策略是让它放在那里，以避免再次加载
            //这样有一定好处，但是有两点不足：1，没加载成功的东西，永远不能再次加载了，2，会使得mRecycledNodeList维护的
            //节点数量随游戏时间而剧增。所以作这样的修改：对于已经在mRecycledNodeList里面的处于stateError状态的节点，
            //在处理回收站时，把它清掉，这样就会有再次加载的机会，同时也不会频繁加载，回收站维护的stateError状态的节点不会
            //因为stateError而剧增。
            /*
             NodeList::iterator rlist_it = mRecycledNodeList.begin();
             while (rlist_it != mRecycledNodeList.end())
             {
             //Info("removeNode: delete object, id="<<(*rlist_it)->getKey()<<endl);
             if ((*rlist_it)->getState() != stateLoaded)
             {
             rlist_it++;
             continue;
             }
             mMemSize -= (*rlist_it)->getSize();
             LONGLONG key = (*rlist_it)->getKey();
             (*rlist_it)->release();
             mNodeMap.erase(key);
             mRecycledNodeList.erase(rlist_it);
             rlist_it = mRecycledNodeList.begin();
             if (!isOverflow())
             break;
             }
             */
            typename NodeList::iterator rlist_it = mRecycledNodeList.begin();
            ResourceState _ResState;
            while( rlist_it != mRecycledNodeList.end() )
            {
                _ResState = (*rlist_it)->getState();
                if( _ResState == stateLoaded || _ResState == stateError )
                {
                    mMemSize -= (*rlist_it)->getSize();
                    LONGLONG key = (*rlist_it)->getKey();
                    
                    //不能在主线程中直接删除, 由资源加载线程删除
                    //主线程只需做很少的线程同步，耗费时间的操作在资源加载线程间做同步
                    //(*rlist_it)->release();
                    (*rlist_it)->setState(stateRelease);
                    if(gResourceManager)
                        gResourceManager->releaseResourceNode(*rlist_it);
                    
                    mNodeMap.erase(key);
                    mRecycledNodeList.erase(rlist_it);
                    rlist_it = mRecycledNodeList.begin();
                    if (!isOverflow())
                        break;
                }
                else//此时可能在loading
                {
                    ++rlist_it;
                    continue;
                }
            }
        }
        
        
        DWORD dwCurTick = getTickCount();
        if (isOverflow() && mDelayRecycledNodeList.size() > 0)
        {
            // 延迟回收的节点 TODO
            typename NodeList::iterator rlist_it = mDelayRecycledNodeList.begin();
            ResourceState _ResState;
            while( rlist_it != mDelayRecycledNodeList.end() )
            {
                _ResState = (*rlist_it)->getState();
                if( _ResState == stateLoaded || _ResState == stateError )
                {
                    mMemSize -= (*rlist_it)->getSize();
                    LONGLONG key = (*rlist_it)->getKey();
                    
                    DWORD dwRemoveTick = mDelyNodeTick[key];
                    if (dwCurTick - dwRemoveTick < 120000)
                    {
                        // 延迟回收的节点保存至少2分钟
                        ++rlist_it;
                        continue;
                    }
                    
                    //不能在主线程中直接删除, 由资源加载线程删除
                    //主线程只需做很少的线程同步，耗费时间的操作在资源加载线程间做同步
                    //(*rlist_it)->release();
                    (*rlist_it)->setState(stateRelease);
                    if(gResourceManager)
                        gResourceManager->releaseResourceNode(*rlist_it);
                    
                    mNodeMap.erase(key);
                    mDelayRecycledNodeList.erase(rlist_it);
                    rlist_it = mDelayRecycledNodeList.begin();
                    mDelyNodeTick.erase(key);
                    if (!isOverflow())
                        break;
                }
                else//此时可能在loading
                {
                    ++rlist_it;
                    continue;
                }
            }
        }
    }
    
    //-------------------------------
	bool ResourceNode::load()
	{
		//ResGuard<Mutex> lock(m_mutex);

		if(m_nState != stateAdded)
			return true;
		if(m_pResourceLoader == 0)
			return false;
		m_nState = stateLoading;

		// 不能直接赋值，不然其他线程就可以立即获取这个指针进行操作，可能造成m_pRes为野指针
		//m_pRes = m_pResourceLoader->loadResource(m_uResID);	
		IResource* pRes = m_pResourceLoader->loadResource(m_uResID);	
		if(pRes)
		{
			//Info("资源加载成功\n");
			m_ulSize = pRes->size();
			if(gResourceManager)
				gResourceManager->grow(m_ulSize);
//#ifdef  RKT_TEST
//			Info(
//				"addNode: key="<<getKey()
//				<<",ref="<<getRefCount()
//				<<",size="<<_ff("%.02f", ((float)getSize()/1024))
//				<<endl);
//#endif
			//Info("ResourceNode::load ok!"<<(ulong)m_uResID<<endl);

		}
		else
		{
			//Info("ResourceNode::load error!"<<(ulong)m_uResID<<endl);
			//Error("Load resource failed, type: "<<m_nType<<", name: "<<m_strName<<endl);
		}
		
		ResGuard<Mutex> lock(m_mutex);
		m_nState = ( pRes ? stateLoaded : stateError);		
		m_pRes = pRes;
		return (m_pRes ? true : false);		
	}
	void ResourceNode::release()
	{
		{
		ResGuard<Mutex> lock(m_mutex);
		safeRelease(m_pRes);
		rktCloseHandle(m_Handle);
		//char str[256];
		//sprintf(str,"ResourceNode::release:%p\n",this);
		//OutputDebugString(str);
		}
		delete this ;
	}

} // namespace