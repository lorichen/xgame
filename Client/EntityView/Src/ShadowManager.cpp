//==========================================================================
/**
* @file	  : ShadowManager.cpp
* @author : 
* created : 2008-4-28   17:07
*/
//==========================================================================

#include "stdafx.h"
#include "ShadowManager.h"
#include "EntityViewImpl.h"


ShadowManager::ShadowManager()
: mNeedDelete(false), mObserver(0), mView(0), mStartTick(0)
{
	m_LifeTime = 150;
	m_CreateIntval = 10;
	m_AllTime = 150;
	m_DelayStart = 0;//前面停止时间
	m_DelayEnd = 0;
}

bool ShadowManager::create(EntityView* view, const POINT& ptBegin, const POINT& ptEnd)
{
	mView = view;
	mDeltaAlpha = 1.f / m_LifeTime;
	//mTick = mStartTick = gGlobalClient->getTimeStamp();
	gGlobalClient->getSceneManager()->tile2Space(ptBegin, mStartPos);
	gGlobalClient->getSceneManager()->tile2Space(ptEnd, mEndPos);
	mDistance = mEndPos - mStartPos;
	mMoveSpeed = mDistance.length() / (m_AllTime-m_DelayStart-m_DelayEnd);
	mDistance.normalize();
	return true;
}
void ShadowManager::SetTime(ulong LifeTime,
	ulong CreateIntval,
	ulong AllTime,
	ulong DelayStart,
	ulong DelayEnd)
{
	 m_LifeTime = LifeTime;
	 m_CreateIntval = CreateIntval;
	 m_AllTime = AllTime;
	 m_DelayStart = DelayStart;
	 m_DelayEnd = DelayEnd;
}
bool ShadowManager::update(ModelNode* pShadowNode,ulong tick, ulong deltaTick)
{
	if (mNeedDelete)
	{
		((EntityViewImpl*)mView)->setSpace(mEndPos);
		delete this;
		return false;
	}

	if (mStartTick == 0) mTick = mStartTick = tick;

	for (ShadowList::iterator it=mList.begin(); it!=mList.end();)
	{
		ulong deltaTick = tick - (*it).startTick;
		if (deltaTick > m_LifeTime)
			it = mList.erase(it);
		else
			++it;
	}

	if (mList.empty() && tick - mStartTick > m_AllTime)
	{
		if (mObserver)
			mObserver->onFinished();
		else
			setNeedDelete();
		return true;
	}

	if (tick - mStartTick <= m_AllTime)
	{
		long count = (mTick - mStartTick) / m_CreateIntval;
		long cur = (long)mList.size();
		if (count > cur)
		{
			ulong delta = deltaTick / (count-cur);
			for (int i=0; i<count-cur; i++)
				createShadow(pShadowNode,mTick + delta);
		}
	}

	mTick = tick;
	mDeltaTick = deltaTick;
	return true;
}

void ShadowManager::render(IRenderSystem* pRenderSystem, ModelNode* pShadowNode)
{
	if( !pShadowNode || pShadowNode != mNode)
		return;

	ColorValue color(1.0f,1.0f,1.0f,1.f);
	for (ShadowList::iterator it=mList.begin(); it!=mList.end(); ++it)
	{
		ulong deltaTick = mTick - (*it).startTick;
		color.a = 1.f - deltaTick * mDeltaAlpha;

		Matrix4 mtxWorld = pRenderSystem->getWorldMatrix();
		Matrix4 mtxTranslation;
		mtxTranslation.makeTrans((*it).pos - mNode->getPosition());
		pRenderSystem->setWorldMatrix(mtxWorld * mtxTranslation);
		mNode->updateColor(pRenderSystem, color);

		//用当前的动作姿态去渲染
		mNode->renderByTime(pRenderSystem, true,(*it).curTime);
		pRenderSystem->setWorldMatrix(mtxWorld);
	}
}

void ShadowManager::createShadow(ModelNode* pNode, ulong tick)
{
	if( 0 == pNode || pNode != mNode)
		return;

	mList.push_back(Shadow());
	Shadow& s = mList.back();
	ulong deltaTick = mTick - mStartTick;
	s.startTick = mTick;
	if(deltaTick<m_DelayStart)
		s.pos = mStartPos;
	else
		s.pos = mStartPos + mDistance * (deltaTick-m_DelayStart) * mMoveSpeed;
	if(deltaTick>m_AllTime-m_DelayEnd)
		s.pos  = mEndPos;
	((EntityViewImpl*)mView)->setSpace(s.pos);
	s.curTime  = mNode->getModelInstance()->getAnimationTime()->current;
}


