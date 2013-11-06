//==========================================================================
/**
* @file	  : ResourceNode.h
* @author : 
* created : 2008-1-18   13:50
* purpose : 
*/
//==========================================================================

#ifndef __RESOURCENODE_H__
#define __RESOURCENODE_H__

#include "IResourceManager.h"
#include "ResPool.h"

namespace xs {

    
	/** 资源实现类
	*/
	class ResourceNode : public IResourceNode, public PoolNode<LONGLONG>
	{
		//资源名称
		/*std::string    m_strName;*/
		ulong			m_uResID;

		LONGLONG           m_ulId; 		

		IResource*		m_pRes;
		ResourceState m_nState;
		int            m_nType;
		ulong          m_ulSize;
		IResourceLoader* m_pResourceLoader;
		int			   m_priorityValue;
	public:
		handle			m_Handle;

		Mutex			m_mutex;
		/** 构造函数
		*/
	public:
		/** 默认构造函数
		*/
		ResourceNode()
			:m_ulId(0),m_pRes(0),m_nState(stateUninit),
			m_nType(typeResourceModel),m_ulSize(0),m_pResourceLoader(0),m_priorityValue(0){}

		ResourceNode(/*const std::string& name*/const ulong uResID, LONGLONG id,
			IResource*pData,ResourceState nState,
			int nType,ulong ulSize,IResourceLoader *pResouceLoader)
		:m_uResID(uResID)/*m_strName(name)*/,m_ulId(id),m_pRes(pData),m_nState(nState),
		m_nType(nType),m_ulSize(ulSize),m_pResourceLoader(pResouceLoader),m_priorityValue(0){}

		ResourceNode(/*const std::string& name*/const ulong uResID, LONGLONG id)
			:m_uResID(uResID)/*m_strName(name)*/,m_ulId(id),m_pRes(0),m_nState(stateUninit),
			m_nType(typeResourceModel),m_ulSize(0),m_pResourceLoader(0),m_priorityValue(0){}

		virtual ulong			getSize()	const		{ return m_ulSize; }
		virtual LONGLONG		getKey()	const		{ return m_ulId; }
		virtual void			release();
		virtual ResourceState	getState()	const 
		{
			ResGuard<Mutex> lock((xs::Mutex&)m_mutex);
			return m_nState ;
		}

		/** 接口函数
		*/
	public:
		int		getType() const { return m_nType ;}
		LONGLONG		getId() const { return m_ulId ;}		
		IResource* getResource() const 
		{ 
			//ResGuard<Mutex> lock((xs::Mutex&)m_mutex);
			return m_pRes ;
		}

		int     getPriority() const { return m_priorityValue ;}

		//IResourceLoader* getResourceLoader() const { return m_pResourceLoader; }
		void    setResourceLoader(IResourceLoader* pResourceLoader){ m_pResourceLoader = pResourceLoader ;}

		void	setPriority(int priorityValue) { m_priorityValue = priorityValue ;}

		void	setType(int nType){ m_nType = nType ;}
		void	setId(LONGLONG id){ m_ulId = id ;}
		void	setResource(IResource* pData){ m_pRes = pData ;}
		void	setState(ResourceState state){ ResGuard<Mutex> lock((xs::Mutex&)m_mutex); m_nState = state ;}
		void    setSize(ulong ulSize){ m_ulSize = ulSize ;}

		//加载资源
		bool    load();


	};

} // namespace

#endif // __RESOURCENODE_H__