#include "StdAfx.h"
#include "RideManager.h"
#include "VisualComponentMZ.h"
#include "MWDLoader.h"



RideManager::RideManager():m_pOwner(0),
m_bIsRide(false),
m_ulResId(0),
m_pRideNode(0),
m_ulNewResId(0),
m_handle(INVALID_HANDLE),
m_bLoaded(false),
m_bCanRequestRes(false)
{

}

void RideManager::create(VisualComponentMZ* pOwner, IRideCallBack* pCallBack)
{
	if( 0 == pOwner)
		throw std::string("RideManager::create:pOwner=0");
	if( 0 == pCallBack)
		throw std::string("RideManager::create:pCallBack=0");
	m_pRideCallBack = pCallBack;
	m_pOwner = pOwner;
}

RideManager::~RideManager()
{
	releaseRes();
}

void RideManager::releaseRes()
{
	//取消骑乘
	RideCallBackContext context;
	context.pRideNode = 0;
	m_pRideCallBack->onRide(context);

	if( isValidHandle( m_handle) ) //资源有效，释放资源
	{
		// hero的骑乘资源延迟回收
		bool bDelayRecycle = false;
		if (NULL != m_pOwner)
		{
			EntityViewImpl* pOwner = m_pOwner->getOwner();
			if (NULL != pOwner)
			{
				bDelayRecycle = pOwner->isMainPlayer();
			}
		}
		gGlobalClient->getResourceManager()->releaseResource(m_handle, bDelayRecycle);
		m_handle = INVALID_HANDLE;
	}
	m_pRideNode = 0;

	//移除映射的资源id
	MZIDMap::getInstance().remove(m_ulNewResId);
	m_ulNewResId = 0;
	m_bLoaded = false;
}


void RideManager::requestRes(int priority)
{
	if( !m_bIsRide )//没有骑乘，不能申请资源
		return;
	if( !m_bCanRequestRes)//不能申请资源
		return;

	//释放旧的资源
	releaseRes();

	//重新申请资源
	m_pRideNode = 0;
	m_ulNewResId = MZIDMap::getInstance().add(m_ulResId, 0);
	m_handle = gGlobalClient->getResourceManager()->requestResource(m_ulNewResId, typeResourceModel, false, priority);
	m_bLoaded = false;
}

void RideManager::setCanRequestRes(bool canRequest)
{
	m_bCanRequestRes = canRequest;
}

bool RideManager::isRide()
{
	return m_bIsRide;
}


void RideManager::update()
{
	if( isValidHandle(m_handle) && !m_bLoaded ) //句柄有效，且没有加载
	{
		IResourceNode* node = (IResourceNode*)getHandleData(m_handle);
		if( node)
		{
			IResource * res = node->getResource();
			if( res)
			{
				ModelNode * n = (ModelNode *)res->getInnerData();
				if( n )
				{
					m_bLoaded = true;
					m_pRideNode = n;
					//骑乘
					RideCallBackContext context;
					context.pRideNode = n;
					ConfigCreature * cc = ConfigCreatures::Instance()->getCreature(m_ulResId);
					if( cc) context.strBindPoint = cc->cRide.boneName;
					else context.strBindPoint.clear();
					context.ulRideResId = m_ulResId;
					m_pRideCallBack->onRide(context);
				}
			}
		}
	}
}

bool RideManager::ride(bool bRide, ulong resId)
{
	bool realNeedRide = false; //真正的是否骑乘命令
	if( bRide &&  0 != resId )
	{
		realNeedRide = true;
	}


	if( m_bIsRide && realNeedRide && resId == m_ulResId ) //都是骑乘，且资源id相同
	{
		return true;//什么都不用做
	}
	else if ( m_bIsRide && realNeedRide && resId != m_ulResId ) //此时是更换骑乘
	{
		//释放旧的资源
		releaseRes();	
		//设置生物id
		m_ulResId = resId;
		//重新申请资源
		requestRes(0);
		return true;
	}
	else if( m_bIsRide && !realNeedRide) //此时是下马
	{
		//释放资源
		releaseRes();

		//设置骑乘状态
		m_bIsRide = false;

		return true;
	}
	else if( !m_bIsRide && realNeedRide) //此时是上马
	{		
		m_bIsRide =true;
		m_ulResId = resId;
		//申请资源
		requestRes(0);
		return true;
	}
	else if( !m_bIsRide && !realNeedRide)//都是下马
	{
		return true;//什么都不用做
	}
	else
	{
		//不可能到这里了
		throw std::string("RideManager::ride::invalid flow");
		return true;
	}

}