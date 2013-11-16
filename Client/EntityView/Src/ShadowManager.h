//==========================================================================
/**
* @file	  : ShadowManager.h
* @author : 
* created : 2008-4-28   15:27
* purpose : 冲锋时的影子管理
*/
//==========================================================================

#ifndef __SHADOWMANAGER_H__
#define __SHADOWMANAGER_H__

struct IShadowObserver
{
	virtual void onFinished() = 0;
};

class ShadowManager
{
	struct Shadow
	{
		Vector3	pos;
		ulong	startTick;
		//add by yhc 记住当前影子的动作时间
		float   curTime;
	};

	typedef std::list<Shadow>	ShadowList;
	ModelNode*			mNode;//需要产生影子的节点
	ShadowList			mList;
	ulong				mTick;
	ulong				mStartTick;
	ulong				mDeltaTick;
	float				mDeltaAlpha;
	Vector3				mStartPos, mEndPos, mDistance;
	float				mMoveSpeed;
	EntityView*			mView;
	bool				mNeedDelete;
	IShadowObserver*	mObserver;

public:
	ShadowManager();
	void setObserver(IShadowObserver* ob)	{ mObserver = ob; }
	void setNeedDelete()					{ mNeedDelete = true; }
	void setModelNode(ModelNode* node)		{ mNode = node; }

	bool create(EntityView* view, const xs::Point& ptBegin, const xs::Point& ptEnd);
	bool update(ModelNode* pShadowNode, ulong tick, ulong deltaTick);
	void render(IRenderSystem* pRenderSystem, ModelNode* pShadowNode);
	void ShadowManager::SetTime(ulong LifeTime,ulong CreateIntval,ulong AllTime,ulong DelayStart,ulong DelayEnd);

private:
	void createShadow(ModelNode* pNode, ulong tick);

	ulong m_LifeTime;
	ulong m_CreateIntval;
	ulong m_AllTime;
	ulong m_DelayStart;
	ulong m_DelayEnd;
};

#endif // __SHADOWMANAGER_H__