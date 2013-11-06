#ifndef __RideManager_H__
#define __RideManager_H__

class VisualComponentMZ;

struct RideCallBackContext
{
	ModelNode* pRideNode;//pRideNode=0,表示下马，pRideNode!=0,表示上马
	std::string strBindPoint;//骑乘绑定点
	ulong		ulRideResId;//骑乘资源id
};

struct IRideCallBack
{
	virtual void onRide(RideCallBackContext& context)=0;
};

class RideManager
{
public:
	bool ride(bool bRide, ulong resId);//骑乘
	void update();//更新
	void releaseRes();//释放资源
	void requestRes(int priority);//申请资源
	void setCanRequestRes(bool canRequest);//能否申请资源
	bool isRide();	//是否处于骑乘状态
public:
	RideManager();
	void create(VisualComponentMZ* pOwner, IRideCallBack* pCallBack);
	~RideManager();

private:
	//处理逻辑
	IRideCallBack*		m_pRideCallBack;//骑乘回调函数
	VisualComponentMZ*	m_pOwner;		//从属的VisualComponentMZ
	bool				m_bIsRide;		//是否处于骑乘状态
	ulong				m_ulResId;		//骑乘资源id，如果m_bIsRide=false，m_ulResId=0；如果m_bIsRide=true，m_ulResId!=0;

	//处理资源
	bool				m_bCanRequestRes;//能否加载资源
	ModelNode*			m_pRideNode;	//骑乘节点，处于骑乘状态，并且资源已经加载好了，才非0，否则为0
	ulong				m_ulNewResId;	//经过资源分配器分配的资源ID
	xs::handle			m_handle;		//资源handle	
	bool				m_bLoaded;		//资源是否加载了
};

#endif