//==========================================================================
/**
* @file	  : Resource.cpp
* @author : 
* created : 2008-1-18   13:50
*/
//==========================================================================

#include "stdafx.h"
#include "ResourceNode.h"
#include "ResourceManager.h"
extern xs::ResourceManager *gResourceManager;

namespace xs {

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