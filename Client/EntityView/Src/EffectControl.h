//==========================================================================
/**
* @file	  : EffectControl.h
* @author : 
* created : 2008-4-6   17:06
* purpose : 
*/
//==========================================================================

#ifndef __EFFECTCONTROL_H__
#define __EFFECTCONTROL_H__

#include "MagicView.h"
#include "IEntityFactory.h"
#include "OffsetArray.h"
#include "ShadowManager.h"
#include "JumpManager.h"
#include "SkillViewData.h"

class EffectControl;
struct IEffectControlObserver
{
	virtual void onDestroy(EffectControl*) = 0;
};

class EffectControl : public IEffectObserver
{
	IEffectControlObserver*	mObserver;
	handle					mSrc;
	ulong					mViewId;
protected:
	handle					mHandle;

public:
	EffectControl() : mObserver(0), mSrc(INVALID_HANDLE), mViewId(0) {}
	virtual ~EffectControl()
	{
	}
	virtual void release()
	{
		if (mObserver)
		{
			mObserver->onDestroy(this);
		}

		if (mHandle != INVALID_HANDLE) 
			closeHandle(mHandle); 
		delete this;
	}
	void CreateHandle()
	{
		mHandle = createHandle((ulong)this);
	}
	void setObserver(IEffectControlObserver* ob)	{ mObserver = ob; }
	void setSrc(handle src)			{ mSrc = src; }
	handle getSrc() const			{ return mSrc; }
	void setViewId(ulong viewId)	{ mViewId = viewId; }
	ulong getViewId() const			{ return mViewId; }
	virtual void onFinished(MagicView* ev) {}
	MagicView* createGeneralEffect(const MagicContext_General& context);
};


class EffectControlManager : public IEffectControlObserver, public SingletonEx<EffectControlManager>
{
	typedef std::list<EffectControl*>	EffectControlList;
	EffectControlList	mList;
public:
	~EffectControlManager()
	{
		for (EffectControlList::iterator it=mList.begin(); it!=mList.end(); ++it)
		{
			(*it)->setObserver(NULL);
			(*it)->release();
		}
		mList.clear();
	}
	void add(EffectControl* ctrl)
	{
		if (ctrl)
		{
			ctrl->setObserver(this);
			mList.push_back(ctrl);
		}
	}

	// 删除一个特效，该特效的创建者为src，ID为viewId
	void remove(handle src, ulong viewId)
	{
		for (EffectControlList::iterator it=mList.begin(); it!=mList.end(); ++it)
		{
			if ((*it)->getSrc() == src && (*it)->getViewId() == viewId)
			{
				EffectControl* ctrl = (*it);
				mList.erase(it);
				ctrl->setObserver(NULL);
				ctrl->release();
				return;
			}
		}
	}

	// 引导技能创建多个特效，退出引导状态时要全部删除
	void removeAll(handle src, ulong viewId)
	{
		struct EffectControlComparison: binary_function<EffectControl*, pair<handle,ulong>, bool>
		{
			bool operator()(EffectControl*& val,pair<handle,ulong> key) const
			{
				bool bResult = (val->getSrc() == key.first && val->getViewId() == key.second );
				if (bResult)
				{
					val->setObserver(NULL);
					val->release();
				}
				return bResult;
			}
		};

		pair<handle,ulong>(src,viewId);
		EffectControlList::iterator newEnd = std::remove_if(mList.begin(),mList.end(),bind2nd(EffectControlComparison(), pair<handle,ulong>(src,viewId)));
		mList.erase(newEnd, mList.end());	
	}

	virtual void onDestroy(EffectControl* ctrl)
	{
		EffectControlList::iterator it = std::find(mList.begin(), mList.end(), ctrl);
		if (it != mList.end())
		{
			mList.erase(it);
			return;
		}
	}
};

/// 通用类效果上下文
struct EffectContext_General
{
	ulong		magicId;/// 光效Id
	int			loops;	/// 光效循环次数
	ulong		angle;	/// 光效的角度(当脚本设定矫正光效角度时才有意义)
	handle		owner;	/// 动画拥有者(不一定是技能发起者)
	POINT		tile;	/// 动画位置（如果不是绑定到EntityView对象的话）

	EffectContext_General() : magicId(0), loops(0), angle(0), owner(0) { tile.x = tile.y = 0;}
};

/// 通用类控制器
//template<class TAttackView, class TAttackContext, class TDamageView, class TDamageContext>
class EffectControl_General : public EffectControl
{
public:
	EffectControl_General() { }
	virtual ~EffectControl_General() 
	{
	}

	bool create(const EffectContext_General& context)
	{
		MagicContext_General mc;
		mc.magicId = context.magicId;
		mc.loops = context.loops;
		mc.angle = context.angle;
		mc.owner = context.owner;
		mc.tile = context.tile;
		MagicView* magic = (MagicView*)gGlobalClient->getEntityFactory()->createMagic(MagicType_General, &mc, sizeof(mc));
		if (!magic)
		{
			return false;
		}
		CreateHandle();
		magic->setEffectObserver(mHandle);

		return true;
	}

	void close() { }

	virtual void onFinished(MagicView* ev)
	{
		if (ev != NULL)
		{
			ev->release();
		}
		
		/*
		必须在此处调用以下函数来删除特效管理链表中的停止播放的特效，否则会导致通用动画效果
		在MagicView中已经释放掉但没有在特效管理链表(EffectControlManager mList)中删掉，导致mList
		越来越大 BUG Fixed by zhangjie 20100520
		*/
		release();
	}
};

/// 飞行类效果上下文
struct EffectContext_Fly : public EffectContext_General
{
	float		moveSpeed;		/// 移动速度
	handle		src;			/// 源
	handle		target;			/// 目标
	ulong		idExplode;		/// 爆炸光效的Id
    //  爆炸光效的目标：如果为0，则爆炸光效绑定在target所在位置的地面上；
    handle		hExplodeTarget;
	int			loopsExplode;	/// 爆炸光效的循环次数
    //  技能上下文：供爆炸光效使用；
    SkillEffectContext ExplodeEffectContext;
};

/// 瞬移类效果上下文
struct EffectContext_RapidMove : public EffectContext_General
{
    AttackContext ac;	/// 攻击上下文
};

/// 飞行类控制器
//template<class TAttackView, class TAttackContext, class TDamageView, class TDamageContext>
class EffectControl_Fly : public EffectControl
{
	MagicViewWithMoveTrack*		mFly;
    ulong mAngle;
	ulong						mIdExplode;
	int							mLoopsExplode;
	handle						mTarget;
    //  飞行时的目标和爆炸光效的绑定目标可以是不同的；
    handle                      mExplodeTarget;
    //  爆炸光效使用的攻击上下文；
    SkillEffectContext mExplodeEffectContext;

public:
	EffectControl_Fly() : mFly(0) { }
	virtual ~EffectControl_Fly() { close(); }

	bool create(const EffectContext_Fly& context)
	{
		if (context.magicId == 0 || !context.src || !context.target)
			return false;

        mAngle = context.angle;
		mIdExplode = context.idExplode;
		mLoopsExplode = context.loopsExplode;
		mTarget = context.target;
        mExplodeTarget = context.hExplodeTarget;
        mExplodeEffectContext = context.ExplodeEffectContext;

		Assert(mLoopsExplode == 1);

		EntityView* src = (EntityView*)getHandleData(context.src);
		EntityView* target = (EntityView*)getHandleData(context.target);
		if (!src || !target) return false;

		MagicContext_MoveTrack mc;
		mc.magicId = context.magicId;
		mc.loops = context.loops;
		mc.angle = 0; // 里面会计数
		mc.owner = 0;
		mc.tile = src->getTile();
		mc.moveSpeed = context.moveSpeed;
		mc.src = context.src;
		mc.target = context.target;
		mFly = (MagicViewWithMoveTrack*)gGlobalClient->getEntityFactory()
			->createMagic(MagicType_MoveTrack, &mc, sizeof(mc));
		if (!mFly)
			return false;

		CreateHandle();
		mFly->setEffectObserver(mHandle);

		return mFly->onCommand(EntityCommand_MoveTrack);
	}

	void close() ;//{ if (mFly) mFly->release(); }
	void onFinished(MagicView* ev);
};




/// 生命期类效果上下文
struct EffectContext_LifeTime : public EffectContext_General
{
	ulong	life;		/// 生命期(毫秒)
};

/// 生命期类控制器
//template<class TAttackView, class TAttackContext, class TDamageView, class TDamageContext>
class EffectControl_LifeTime : public EffectControl, public TimerHandler
{
	MagicView*				mEffect;
public:
	EffectControl_LifeTime() : mEffect(0) { }
	virtual ~EffectControl_LifeTime() { close(); }

	bool create(const EffectContext_LifeTime& context)
	{
		if (context.magicId == 0 || context.life == 0)
			return false;

		Assert(context.loops <= 0);

		MagicContext_General mc;
		mc.magicId = context.magicId;
		mc.loops = context.loops;
		mc.angle = context.angle;
		mc.owner = context.owner;
		mc.tile = context.tile;

		mEffect = (MagicView*)gGlobalClient->getEntityFactory()
			->createMagic(MagicType_General, &mc, sizeof(mc));
		if (mEffect)
			gGlobalClient->getTimerAxis()->SetTimer(0, context.life, this, 1, __FILE__);

		CreateHandle();
		return mEffect != 0;
	}

	void close()
	{
		gGlobalClient->getTimerAxis()->KillTimer(0, this);
		if (mEffect) mEffect->release();
	}
	virtual void OnTimer(unsigned long dwTimerID)
	{
		if (mEffect)
			mEffect->release();
		release();
	}
};

//  多次播放类效果上下文；
template< class TEffectControlContext >
struct EffectContext_MultiTimes : public EffectContext_General
{
    //  总时间，单位：ms；
    ulong m_ulDuration;
    //  每次之间的间隔，单位：ms；
	ulong m_ulInterval;
    TEffectControlContext m_EffectControlContext;
};

/// 多次类控制器
//template<class TAttackView, class TAttackContext, class TDamageView, class TDamageContext>
template< class TEffectControlContext, class TEffectControlType >
class EffectControl_MultiTimes : public EffectControl, public TimerHandler
{
#if 0
	MagicContext_General	mContext;
	ulong	mTimes;
#endif
    EffectContext_MultiTimes< TEffectControlContext > m_Context;

public:
    EffectControl_MultiTimes()
    {
    }
	virtual ~EffectControl_MultiTimes()
    {
        close();
    }

public:
	bool create(const EffectContext_MultiTimes< TEffectControlContext >& context)
	{
#if 0
		if (context.magicId == 0 || context.times == 0 || 
			(context.times > 1 && context.interval == 0))
			return false;

		mContext.magicId = context.magicId;
		mContext.loops = context.loops;
		mContext.angle = context.angle;
		mContext.owner = context.owner;
		mContext.tile = context.tile;
		mTimes = context.times;

		Assert(context.loops >= 1);

		if (context.times > 1)
			gGlobalClient->getTimerAxis()->SetTimer(0, context.interval, this, context.times-1, __FILE__);

		OnTimer(0);

#endif
        m_Context = context;

        //  先创建一次特效；
        CreateEffect();

        //  启动控制多次创建特效的定时器；
        if (m_Context.m_ulDuration > 0)
        {
            gGlobalClient->getTimerAxis()->SetTimer(0, m_Context.m_ulInterval, this);
        }
		return true;
	}

	void close()
	{
		gGlobalClient->getTimerAxis()->KillTimer(0, this);
	}

	virtual void OnTimer(unsigned long dwTimerID)
	{
        //  创建一次特效；
        CreateEffect();
	}

    void CreateEffect()
    {
        TEffectControlType* pEffectControl = new TEffectControlType();
        if (!pEffectControl->create(m_Context.m_EffectControlContext))
        {
            ASSERT(false);
            delete pEffectControl;
            return;
        }

        //pEffectControl->setSrc(m_Context.m_EffectControlContext.src);
        //pEffectControl->setViewId(m_Context.m_nEffectID);
        //EffectControlManager::getInstance().add(pEffectControl);
    }
};



/// 区域类效果上下文
struct EffectContext_Area : public EffectContext_General
{
	const IntArray* arr;/// 区域
	ulong	interval;	/// 每次之间的间隔
	ulong	life;		/// 生命期(毫秒)
};

/// 区域类控制器
//template<class TAttackView, class TAttackContext, class TDamageView, class TDamageContext>
class EffectControl_Area : public EffectControl, public TimerHandler
{
	EffectContext_Area	mContext;
	ulong mCounts;
	ulong mMaxLoopCount;	/// 防止do-while死循环
public:
	EffectControl_Area() : mCounts(0), mMaxLoopCount(400) { }
	virtual ~EffectControl_Area()	{ close(); }

public:
	bool create(const EffectContext_Area& context)
	{
		if (context.magicId == 0 || context.arr->count == 0 
			|| context.interval == 0 || context.life == 0/* || context.owner != 0*/)
			return false;

		if (context.arr->count == 1)
		{
			if (context.arr->data[0] < 3 || context.arr->data[0] > 12)
				return false;
		}

		memcpy(&mContext, &context, sizeof(EffectContext_Area));

		if (mContext.arr->count == 1) // 圆形
			mMaxLoopCount = arrSizeCircle[mContext.arr->data[0]];
		else if (mContext.arr->count == 2) // 矩形
			mMaxLoopCount = mContext.arr->data[0] * mContext.arr->data[1];

		mCounts = mContext.life / mContext.interval;
		gGlobalClient->getTimerAxis()->SetTimer(0, mContext.interval, this, INFINITY_CALL, __FILE__);
		gGlobalClient->getTimerAxis()->SetTimer(1, mContext.life, this, 1, __FILE__);
		OnTimer(0);

		return true;
	}
	void close()
	{
		gGlobalClient->getTimerAxis()->KillTimer(0, this);
		gGlobalClient->getTimerAxis()->KillTimer(1, this);
	}

	virtual void OnTimer(unsigned long dwTimerID)
	{
		if (dwTimerID == 1)
		{
			release();
			return;
		}

		POINT pt;
		Tile* tile = 0;
		ulong loops = 0;

		do
		{
			loops++;
			if (mContext.arr->count == 1) // 圆形
			{
				const TileOffset* arr = arrOffsetCircle[mContext.arr->data[0]];
				const ulong size = arrSizeCircle[mContext.arr->data[0]];
				TileOffset offset = arr[rand() % size];
				pt.x = mContext.tile.x + offset.x;
				pt.y = mContext.tile.y + offset.y;
			}
			else if (mContext.arr->count == 2) // 矩形
			{
				pt.x = mContext.tile.x - (mContext.arr->data[0]/2) + rand() % mContext.arr->data[0];
				pt.y = mContext.tile.y - (mContext.arr->data[1]/2) + rand() % mContext.arr->data[1];
			}
			tile = gGlobalClient->getSceneManager()->getTile(pt);
		} while (loops < mMaxLoopCount && (!tile || !tile->isValid() || tile->isBlock()));

		MagicContext_General mc;
		mc.magicId = mContext.magicId;
		mc.loops = mContext.loops;
		mc.angle = mContext.angle;
		mc.owner = 0;
		mc.tile = pt;

		if (gGlobalClient->getEntityFactory()->createMagic(MagicType_General, &mc, sizeof(mc)))
			mCounts--;
	}/*
	virtual void onFinished(MagicView* ev)
	{
		if (ev)
			ev->release();

		if (--mCounts == 0)
			release();
	}*/
};





/// 多向类效果上下文
struct EffectContext_MultiDir: public EffectContext_General
{
	ulong	dirs;		/// 方向数
	float	moveSpeed;	/// 移动速度
	POINT	pt;			/// 开始位置
	ulong	width;		/// 单向宽度
	ulong	maxDist;	/// 最大距离
	ulong	blockType;	/// 阻挡类型
};

/// 多向类控制器
//template<class TAttackView, class TAttackContext, class TDamageView, class TDamageContext>
class EffectControl_MultiDir : public EffectControl
{
	ulong	mDirs;
	ulong	mWidth;
	ulong	mCount;
public:
	EffectControl_MultiDir() { }
	virtual ~EffectControl_MultiDir(){ close(); }

public:
	bool create(const EffectContext_MultiDir& context)
	{
		if (context.magicId == 0 || context.owner == 0 
			|| context.dirs == 0 || context.moveSpeed <= 0.f
			|| context.pt.x == 0 || context.pt.y == 0
			|| context.maxDist == 0)
			return false;

		mDirs = context.dirs;
		mWidth = context.width;
		mCount = mDirs * mWidth;

		if (mWidth > 5) mWidth = 5;
		if (mWidth == 0) mWidth = 1;
		if (!(mWidth & 1)) return false;

		static POINT offset[eDirectionCount] = {
			{ 1,  0}, { 1,  1}, { 0,  1}, {-1,  1},
			{-1,  0}, {-1, -1}, { 0, -1}, { 1, -1},
		};

		EntityView* owner = (EntityView*)getHandleData(context.owner);
		if (!owner) return false;

		// 施法者面向
		ulong dir = angle2Dir(owner->getAngle()); //calcDir8_tile(owner->getTile(), context.tile);
		MagicContext_MoveRadial mc;
		mc.magicId = context.magicId;
		mc.loops = context.loops;
		mc.angle = 0; // 内部计算
		mc.owner = 0;
		mc.tile = context.tile;
		mc.moveSpeed = context.moveSpeed;
		mc.blockType = context.blockType;

		POINT ptTarget;
		switch (mDirs)
		{
		case 1: // 中心点前方
			{
				const TileOffset* arrOff = 0;
				if (mWidth == 1)
					arrOff = &arrOffsetRadial[dir];
				else if (mWidth == 3)
					arrOff = &arrOffsetRadialOrigin_3[dir][0];
				else if (mWidth == 5)
					arrOff = &arrOffsetRadialOrigin_5[dir][0];

				ptTarget.x = mc.tile.x + offset[dir].x * (context.maxDist - 1);
				ptTarget.y = mc.tile.y + offset[dir].y * (context.maxDist - 1);

				POINT ptFrom, ptTo;
				for (ulong j=0; j<mWidth; j++)
				{
					ptFrom.x = context.tile.x + arrOff[j].x;
					ptFrom.y = context.tile.y + arrOff[j].y;
					ptTo.x = ptTarget.x + arrOff[j].x;
					ptTo.y = ptTarget.y + arrOff[j].y;
					MagicViewWithMoveRadial* magic = (MagicViewWithMoveRadial*)gGlobalClient->getEntityFactory()->createMagic(MagicType_MoveRadial, &mc, sizeof(mc));
					if (magic)
						magic->onCommand(EntityCommand_MoveRadial, (ulong)&ptFrom, (ulong)&ptTo);
				}
			}
			break;		
		case 3: // 中心点前方3方向
			for (ulong i=0; i<mDirs; i++)
			{
				int newDir = dir + i - 1;
				if (newDir < 0) newDir += eDirectionCount;
				if (newDir >= eDirectionCount) newDir -= eDirectionCount;

				const TileOffset* arrOff = 0;
				if (mWidth == 1)
					arrOff = &arrOffsetRadial[newDir];
				else if (mWidth == 3)
					arrOff = &arrOffsetRadialOrigin_3[newDir][0];
				else if (mWidth == 5)
					arrOff = &arrOffsetRadialOrigin_5[newDir][0];

				ptTarget.x = mc.tile.x + offset[newDir].x * (context.maxDist - 1);
				ptTarget.y = mc.tile.y + offset[newDir].y * (context.maxDist - 1);

				POINT ptFrom, ptTo;
				for (ulong j=0; j<mWidth; j++)
				{
					ptFrom.x = context.tile.x + arrOff[j].x;
					ptFrom.y = context.tile.y + arrOff[j].y;
					ptTo.x = ptTarget.x + arrOff[j].x;
					ptTo.y = ptTarget.y + arrOff[j].y;
					MagicViewWithMoveRadial* magic = (MagicViewWithMoveRadial*)gGlobalClient->getEntityFactory()->createMagic(MagicType_MoveRadial, &mc, sizeof(mc));
					if (magic)
						magic->onCommand(EntityCommand_MoveRadial, (ulong)&ptFrom, (ulong)&ptTo);
				}
			}
			break;
		case 4: // 中心点4方向
			for (ulong i=0; i<mDirs; i++)
			{
				int newDir = dir + i * 2;
				if (newDir < 0) newDir += eDirectionCount;
				if (newDir >= eDirectionCount) newDir -= eDirectionCount;

				const TileOffset* arrOff = 0;
				if (mWidth == 1)
					arrOff = &arrOffsetRadial[newDir];
				else if (mWidth == 3)
					arrOff = &arrOffsetRadialOrigin_3[newDir][0];
				else if (mWidth == 5)
					arrOff = &arrOffsetRadialOrigin_5[newDir][0];

				ptTarget.x = mc.tile.x + offset[newDir].x * (context.maxDist - 1);
				ptTarget.y = mc.tile.y + offset[newDir].y * (context.maxDist - 1);

				POINT ptFrom, ptTo;
				for (ulong j=0; j<mWidth; j++)
				{
					ptFrom.x = context.tile.x + arrOff[j].x;
					ptFrom.y = context.tile.y + arrOff[j].y;
					ptTo.x = ptTarget.x + arrOff[j].x;
					ptTo.y = ptTarget.y + arrOff[j].y;
					MagicViewWithMoveRadial* magic = (MagicViewWithMoveRadial*)gGlobalClient->getEntityFactory()->createMagic(MagicType_MoveRadial, &mc, sizeof(mc));
					if (magic)
						magic->onCommand(EntityCommand_MoveRadial, (ulong)&ptFrom, (ulong)&ptTo);
				}
			}
			break;
		case 8: // 中心点8方向
			for (ulong i=0; i<mDirs; i++)
			{
				const TileOffset* arrOff = 0;
				if (mWidth == 1)
					arrOff = &arrOffsetRadial[i];
				else if (mWidth == 3)
					arrOff = &arrOffsetRadialOrigin_3[i][0];
				else if (mWidth == 5)
					arrOff = &arrOffsetRadialOrigin_5[i][0];

				ptTarget.x = mc.tile.x + offset[i].x * (context.maxDist - 1);
				ptTarget.y = mc.tile.y + offset[i].y * (context.maxDist - 1);

				POINT ptFrom, ptTo;
				for (ulong j=0; j<mWidth; j++)
				{
					ptFrom.x = context.tile.x + arrOff[j].x;
					ptFrom.y = context.tile.y + arrOff[j].y;
					ptTo.x = ptTarget.x + arrOff[j].x;
					ptTo.y = ptTarget.y + arrOff[j].y;
					MagicViewWithMoveRadial* magic = (MagicViewWithMoveRadial*)gGlobalClient->getEntityFactory()->createMagic(MagicType_MoveRadial, &mc, sizeof(mc));
					if (magic)
						magic->onCommand(EntityCommand_MoveRadial, (ulong)&ptFrom, (ulong)&ptTo);
				}
			}
			break;
		default:
			return false;
		}

		return true;
	}
	void close() { }
	virtual void onFinished(MagicView* ev)
	{
		if (ev) ev->release();

		if (--mCount == 0)
			release();
	}
};

/// 瞬移类控制器
class EffectControl_RapidMove : public EffectControl, public IShadowObserver
{
	ShadowManager*	mShadowManager;
	handle			mOwner;
	POINT			mTargetTile;
	AttackContext	mAttackContext;
public:
	bool create(const EffectContext_RapidMove& context)
	{
		if (context.owner == 0 || context.tile.x == 0 || context.tile.y == 0)
			return false;

		mOwner = context.owner;
		EntityView* owner = (EntityView*)getHandleData(context.owner);
		if (!owner)
			return false;

		mAttackContext = context.ac;
		mTargetTile = context.tile;

		mShadowManager = new ShadowManager();
		mShadowManager->setObserver(this);
		if (!mShadowManager->create(owner, owner->getTile(), mTargetTile))
		{
			delete mShadowManager;
			return false;
		}

		return owner->onCommand(EntityCommand_RapidMoveStart, (ulong)mShadowManager);
	}
	void close() { }
	virtual void onFinished()
	{
		EntityView* owner = (EntityView*)getHandleData(mOwner);
		gGlobalClient->getSceneManager()->moveEntity(owner->getTile(), mTargetTile, owner);
		// 停止移动
		owner->onCommand(EntityCommand_Stand);
		owner->onCommand(EntityCommand_RapidMoveEnd);
		if (mAttackContext.actionId != 0)
		{
			owner->onCommand(EntityCommand_Attack, (ulong)&mAttackContext);
		}
		release();
	}

};

//跳跃类效果
class EffectControl_Jump : public EffectControl, public IJumpObserver
{
	POINT			mTargetTile;
	handle			mOwner;
	JumpManager*	pJumpManager;
public:
	bool create(const EffectContext_RapidMove& context)
	{
		if (context.owner == 0 || context.tile.x == 0 || context.tile.y == 0)
			return false;
		
		mOwner = context.owner;
		EntityView* owner = (EntityView*)getHandleData(context.owner);
		if (!owner)
			return false;

		mTargetTile = context.tile;

		pJumpManager = new JumpManager();
		pJumpManager->setObserver(this);
		if (!pJumpManager->create(owner->getTile(), mTargetTile))
		{
			delete pJumpManager;
			return false;
		}
		owner->onCommand(EntityCommand_SetMount, 0);
		return owner->onCommand(EntityCommand_Jump, (ulong)pJumpManager);		
	}
	void close() { }
	virtual void onFinished()
	{
		//跳跃结束
		//EntityView* owner = (EntityView*)getHandleData(mOwner);
		//gGlobalClient->getSceneManager()->moveEntity(owner->getTile(), mTargetTile, owner);
		release();
	}
};
//

#endif // __EFFECTCONTROL_H__