//==========================================================================
/**
* @file	  : ActionList.h
* @author : 
* created : 2008-3-19   21:30
* purpose : 管理动作列表
*/
//==========================================================================

#ifndef __ACTIONLIST_H__
#define __ACTIONLIST_H__

struct ActionInfo
{
	ulong actionId;
	int loops;
};

class ActionList
{
	enum { MaxActionCount = 8 };

	ActionInfo	mActionList[MaxActionCount];
	int			mCur, mCount;
	bool		mIsMoving;	/// 是否正在移动

public:
	ActionList()
	{
		memset(mActionList, 0, sizeof(mActionList));
		mActionList[0].actionId = EntityAction_Stand;
		mActionList[0].loops = -1;
		mCur = 0;
		mCount = 1;
		mIsMoving = false;
	}

	
	// 返回1则立即修改动画
	//int setAction(ulong actionId, int loops, bool bForceStop = false)
	//{
	//	Assert(loops != 0);
	//	if (mCount > 0)
	//	{
	//		//if (mActionList[mCur].actionId == actionId && actionId == EntityAction_Stand) return 0; // 只对站立动作进行判断，否则连续使用两次相同的技能，后一次会没有动作
	//		if (mActionList[mCur].actionId == EntityAction_Death) 
	//			return 0;
	//		//if (mIsMoving && actionId != EntityAction_Walk && actionId != EntityAction_Run )return 0; 
	//		//if (mActionList[mCur].actionId == EntityAction_JumpStart||mActionList[mCur].actionId == EntityAction_JumpMidway||mActionList[mCur].actionId == EntityAction_JumpEnd) return 0;//跳跃的优先级
	//	}

	//	// 技能动作过程中受击时，技能动作不能被打断，屏蔽受击动作
	//	if (actionId == EntityAction_CombatWound && mActionList[mCur].actionId >= EntityAction_Attack)
	//	{
	//		return 0;
	//	}

	//	// 移动过程中，暂时屏蔽受击动作，因为会产生滑步现象
	//	if (actionId == EntityAction_CombatWound && mIsMoving)
	//	{
	//		return 0;
	//	}


	//	if (loops > 0) // 添加一个不循环的动作，立即更新动画
	//	{
	//		if (mActionList[mCur].actionId == EntityAction_Stun && actionId!=EntityAction_Death) return 0;

	//		// add by zjp
	//		//移动过程受击时不打断
	//		//if (actionId == EntityAction_CombatWound && mActionList[mCur].actionId == EntityAction_Run)
	//		//{
	//		//	mCount = 0;

	//		//	mActionList[mCount].actionId = actionId;
	//		//	mActionList[mCount].loops = loops;
	//		//	mCount++;

	//		//	if (actionId != EntityAction_Death && actionId != EntityAction_Stand)
	//		//	{
	//		//		mActionList[mCount].actionId = EntityAction_Run;
	//		//		mActionList[mCount].loops = -1;
	//		//		mCount++;
	//		//	}

	//		//	mCur = 0;
	//		//	return 1;
	//		//}
	//		//else
	//		{
	//			mCount = 0;

	//			mActionList[mCount].actionId = actionId;
	//			mActionList[mCount].loops = loops;
	//			mCount++;

	//			// 没必要添加站立动作，这个对站立NPC的动作会有影响，否则NPC会卡死不动，那些
	//			if (actionId != EntityAction_Death && actionId != EntityAction_Stand)
	//			{
	//				mActionList[mCount].actionId = EntityAction_Stand;
	//				mActionList[mCount].loops = -1;
	//				mCount++;
	//			}

	//			mCur = 0;
	//			return 1;
	//		}
	//	}
	//	else // 添加一个循环动作
	//	{
	//		// 使用剑震八方的时候，可以移动，但是不要打断其动作
	//		if ( !bForceStop && (actionId == EntityAction_Run || actionId == EntityAction_Walk || actionId == EntityAction_Stand) && mActionList[mCur].actionId == EntityAction_Skill_16)
	//		{
	//			return 2;
	//		}

	//		mCur = mCount = 0;
	//		mActionList[mCount].actionId = actionId;
	//		mActionList[mCount].loops = loops;
	//		mCount++;
	//		return 1;


	//		//if (mActionList[mCur].loops <= 0 ||
	//		//	actionId == EntityAction_Walk || // 移动优先级比较高
	//		//	actionId == EntityAction_Run) // clear and set
	//		//{
	//		//	mCur = mCount = 0;

	//		//	mActionList[mCount].actionId = actionId;
	//		//	mActionList[mCount].loops = loops;
	//		//	mCount++;

	//		//	return 1;
	//		//}
	//		//else // add
	//		//{
	//		//	if (mCount > mCur + 1 && mActionList[mCur].loops > 0) // 当前是不循环的，去掉中间循环的动画
	//		//	{
	//		//		int next = mCur + 1;
	//		//		while (mActionList[next].loops > 0)
	//		//			next++;
	//		//		mCount = next;
	//		//	}

	//		//	mActionList[mCount].actionId = actionId;
	//		//	mActionList[mCount].loops = loops;
	//		//	mCount++;

	//		//	if (mCount <= 1) return 1;
	//		//}
	//	}

	//	return 0;
	//}

	int setAction(ulong actionId, int loops, bool bForceStop = false)
	{
		Assert(loops != 0);
		if (mCount > 0)
		{
			if (mActionList[mCur].actionId == EntityAction_Death) 
				return 0;
		}

		// 技能动作过程中受击时，技能动作不能被打断，屏蔽受击动作
		if (actionId == EntityAction_CombatWound && mActionList[mCur].actionId >= EntityAction_Attack)
		{
			return 0;
		}

		// 移动过程中，暂时屏蔽受击动作，因为会产生滑步现象
		if (actionId == EntityAction_CombatWound && mIsMoving)
		{
			return 0;
		}


		if (loops > 0) // 添加一个不循环的动作，立即更新动画
		{
			if (mActionList[mCur].actionId == EntityAction_Stun && actionId!=EntityAction_Death) return 0;
			mCount = 0;
			mActionList[mCount].actionId = actionId;
			mActionList[mCount].loops = loops;
			mCount++;
			// 没必要添加站立动作，这个对站立NPC的动作会有影响，否则NPC会卡死不动，那些
			if (actionId != EntityAction_Death && actionId != EntityAction_Stand)
			{
				mActionList[mCount].actionId = EntityAction_Stand;
				mActionList[mCount].loops = -1;
				mCount++;
			}

			mCur = 0;
			return 1;
		}
		else // 添加一个循环动作
		{
			mCur = mCount = 0;
			mActionList[mCount].actionId = actionId;
			mActionList[mCount].loops = loops;
			mCount++;
			return 1;
		}

		return 0;
	}

	const ActionInfo& getCurActionInfo() const { return mActionList[mCur]; }

	const ActionInfo& nextAction()
	{
		if (mCur + 1 < mCount)
			mCur++;
		return mActionList[mCur];
	}

	bool hasNextAction()
	{
		return mCur + 1 < mCount;
	}

	inline ulong getCount() const
	{
		return mCount;
	}

	inline void setIsMoving(bool moving)
	{
		mIsMoving = moving;
	}

	inline bool isMoving() const
	{
		return mIsMoving;
	}

	// 复活的时候会调用
	void clear()
	{
		mCur = mCount = 0;
	}
	bool getAction(int index, ulong & actionId, int & loops)
	{
		if(index <0 || index >= mCount || index >= MaxActionCount ) return false;
		actionId = mActionList[index].actionId;
		loops = mActionList[index].loops;
		return true;
	}
};

#endif // __ACTIONLIST_H__