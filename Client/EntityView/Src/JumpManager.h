//==========================================================================
/**
* @file	  : JumpManager.h
* @author : 
* created : 2009-12-12   15:27
* purpose : 跳跃管理
*/
//==========================================================================

#ifndef __JUMPMANAGER_H__
#define __JUMPMANAGER_H__

class EntityViewImpl;
enum JUMPSTATE
{
	JUMPSTATE_READY,
	JUMPSTATE_STATR,
	JUMPSTATE_MIDWAY,
	JUMPSTATE_END,
	JUMPSTATE_FINISH
};

struct IJumpObserver
{
	virtual void onFinished() = 0;
};

// 管理跳跃的状态和数据
class JumpManager
{
	ulong				mStartTick;
	Vector3				mStartPos, mEndPos, mDistance;
	float				m_a,m_x1,m_x2;//抛物线方程系数
	float				m_jumpHeight; //跳跃的当前高度
	ulong				m_AllTick;//跳跃总时间长
	float				m_MoveSpeed;//移动速度
	xs::Point			    mTargetTile;
	IJumpObserver*	    mObserver;

	ulong				mAniTick;//当前动作时间

	JUMPSTATE mState;
public:
	JumpManager();
	void setObserver(IJumpObserver* ob)	{ mObserver = ob; }
	bool create(const xs::Point& ptBegin, const xs::Point& ptEnd);
	bool update(ulong tick, ulong deltaTick,EntityViewImpl* pView);
	bool IsEnd(ulong tick);
	JUMPSTATE GetJumpState();
	void SetMoveSpeed(float moveSpeed);
	void JumpEnd(EntityViewImpl* pView);
	int  GetAllTicks(){return m_AllTick;}
	float GetjumpHeight(){return m_jumpHeight;}
	//ulong GetAniTime(){return mAniTick;}
};

#endif // __SHADOWMANAGER_H__