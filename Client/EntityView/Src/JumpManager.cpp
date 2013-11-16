//==========================================================================
/**
* @file	  : JumpManager.cpp
* @author : 
* created : 2008-4-28   17:07
*/
//==========================================================================

#include "stdafx.h"
#include "JumpManager.h"
#include "EntityViewImpl.h"
#include "../../../Base/EventEngine/Include/IEventEngine.h"

//起跳
#define StartJump  333
//落地
#define EndJump    333
//恢复站立姿态
#define GetBack    333

JumpManager::JumpManager()
{
	mObserver = NULL;
}

bool JumpManager::create(const xs::Point& ptBegin, const xs::Point& ptEnd)
{
	mTargetTile = ptEnd;
	gGlobalClient->getSceneManager()->tile2Space(ptBegin, mStartPos);
	gGlobalClient->getSceneManager()->tile2Space(ptEnd, mEndPos);

	Info("jumpstartTile"<<ptBegin.x<<ptBegin.y<<endl);
	Info("jumpEndTile"<<ptEnd.x<<ptEnd.y<<endl);
	//测试抛物线方程
	//(2)知道抛物线的与x轴的两个交点(x1,0),(x2,0),并知道抛物线过某一个点(m,n)
	//设抛物线的方程为y=a(x-x1)(x-x2),然后将点(m,n)代入去求得二次项系数a

	//设x = 10,y=100; x1=0,x2 = 20;

	m_x1 = 0;//Math::Sqrt( mStartPos.x*mStartPos.x + mStartPos.z*mStartPos.z) ;
	m_x2 = (mEndPos - mStartPos).length();//Math::Sqrt( mEndPos.x*mEndPos.x + mEndPos.z*mEndPos.z) ;

	

	//最高的space坐标
	float m = 50.0f+m_x2/5.0f;//8.0f*Math::Sqrt( (ptEnd.x-ptBegin.x)*(ptEnd.x-ptBegin.x) + (ptEnd.y-ptBegin.y)*(ptEnd.y-ptBegin.y) );
	//Info("m="<<m<<endl);
	if(m>200.0f)
		m = 200.0f;
	if(m<50)
		m = 50;
	float n = m_x2/2;//(m_x1+m_x2)/2;

	//不能除零
	if(n!=0&&n!=m_x2)
		m_a = m/n/(n-m_x2);
	else
		m_a = -1;
	m_jumpHeight = 0;

	mStartTick = -1;
	mAniTick = 0;

	mState = JUMPSTATE_READY;
	return true;
}
void JumpManager::SetMoveSpeed(float moveSpeed)
{
	m_MoveSpeed = moveSpeed*2;
	mDistance = mEndPos - mStartPos;
	m_AllTick = mDistance.length() / m_MoveSpeed;
	if( m_AllTick < StartJump + EndJump )
		m_AllTick = StartJump + EndJump;
}
bool JumpManager::update(ulong tick, ulong deltaTick,EntityViewImpl* pView)
{
	if(mStartTick==-1)
	{
		mStartTick = tick;
	}

	//当前处于什么跳跃状态
	JUMPSTATE state = JUMPSTATE_MIDWAY;
	ulong jumpTick = tick - mStartTick;
	if(jumpTick<StartJump)
		state = JUMPSTATE_STATR;
	if(jumpTick>m_AllTick-EndJump)
		state = JUMPSTATE_END;
	if(jumpTick>m_AllTick+GetBack)
		state = JUMPSTATE_FINISH;

	
	float f=(float)jumpTick/(float)m_AllTick;
	//Info("JumpManager::update:"<<f<<endl);
	if(f>1)
		f = 1;
	Vector3 pos = mStartPos + f*mDistance;//jumpTick/m_AllTick*mDistance;

	float x = f*mDistance.length();//Math::Sqrt( pos.x*pos.x + pos.z*pos.z) ;
	m_jumpHeight = m_a*(x-m_x1)*(x-m_x2);
	//Info(m_jumpHeight<<endl);
	//变为正的
	if(m_jumpHeight<0)
		m_jumpHeight = -m_jumpHeight;

	if(m_jumpHeight>200)
		m_jumpHeight = 200;

	if(m_jumpHeight>0&&m_jumpHeight<=200)
		pos.y+=m_jumpHeight;

	//if( state == JUMPSTATE_MIDWAY)
	//{
	//上升过程
	//美术动作已经控制了高度
	/*
	if(jumpTick<m_AllTick/2)
		pos.y += jumpTick*0.1;
	else
		pos.y += (m_AllTick - jumpTick )*0.1;//下降过程
		*/
	/*if(state == JUMPSTATE_END)
	{
		pos = mEndPos;
	}*/

	//本状态动作时间,计算头顶的时候用,
	if(state==JUMPSTATE_STATR)
		mAniTick = jumpTick;
	if(state==JUMPSTATE_MIDWAY)
		mAniTick = jumpTick-StartJump;
	if(state==JUMPSTATE_END)
		mAniTick = jumpTick+EndJump-m_AllTick;

	pView->setSpace(pos);
	mState = state;
	return true;
}
JUMPSTATE JumpManager::GetJumpState()
{
	return mState;
}
//是否结束
bool JumpManager::IsEnd(ulong tick)
{
	if(tick>mStartTick+m_AllTick)
	{
		return true;
	}
	else
		return false;
}

//
void JumpManager::JumpEnd(EntityViewImpl* pView)
{
	//跳跃结束
	gGlobalClient->getSceneManager()->moveEntity(pView->getTile(), mTargetTile, pView);

	// 跳跃的动作控制和技能的不同，时间无法配置，做特殊的处理，在跳跃结束之后技能回到normal态
	IEntity *pEntity = (IEntity *)pView->getUserData();
	if (pEntity == NULL)
	{
		return;
	}
	UID uid = pEntity->GetUID();

	obuf512 ob;
	SMsgActionJump Jump;
	Jump.ptTarget = mTargetTile;
	fillActionData(ob, pEntity, MSG_ACTION_JUMP, (void*)&Jump, sizeof(SMsgActionJump));
	syncUpdate(ob, pEntity);

	gGlobalClient->getEventEngine()->FireExecute(EVENT_SKILL_JUMPFINISH, 
		pEntity->GetEventSourceType(), 
		ANALYZEUID_SERIALNO(uid),
		NULL,
		0);

	if (mObserver != NULL)
	{
		mObserver->onFinished();
	}
}
