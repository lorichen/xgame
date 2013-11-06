//==========================================================================
/**
* @file	  : MoveComponentMZ.h
* @author : 
* created : 2008-1-25   17:40
* purpose : 移动组件(MZ)
*/
//==========================================================================

#ifndef __MOVECOMPONENTMZ_H__
#define __MOVECOMPONENTMZ_H__

#include "EntityViewImpl.h"
#include "MoveComponent.h"
#include "ISceneManager2.h"
#include "PathNodes.h"
#include "ConfigCreatures.h"
#include "ConfigCreatureRes.h"
/**
改进意见：在没走完，重新走路或因为攻击而被迫停止时，将新路径或停止位置缓存，在通过tile中心点时再启用
*/


class MoveComponentMZ : public MoveComponent
{
private:
	PathNodes<POINT>    mPathList;			/// 路径缓冲
	float               mBaseTimePerTile;   //基础时间
	float				mTimePerTile;		//现在改成速度是移动一个tile所需要的时间/// 移动速度 计算方法：speed = 32*1.414/两个tile间的时间
	bool                mNeedWait;			/// 是否因为前面有人而需要等待?


	ulong			    mTicks,mOldTicks;   /// tick数(方向改变后重置)
	Vector3             mStartPos;				/// 开始处的位置(方向改变后重置)
	Vector3             mNextPos;				/// 下一个位置

	bool                mIsMoving;				/// 是否正在移动

	// 是否播放移动音效
	int                 m_nPlayDelay;
public:
	MoveComponentMZ();
	virtual ~MoveComponentMZ();

	virtual void create();
	virtual void close();

	virtual bool update(ulong curTicks, ulong deltaTicks);
	virtual bool handleCommand(ulong cmd, ulong param1, ulong param2);
	virtual void handleMessage(ulong msgId, ulong param1, ulong param2);


	bool move(const POINT* nodes, size_t count);

	// 移动一小步
	void moveStep(ulong period);

	void resetMoveInfo();
	void stopMove();
	bool isMoving(){return mIsMoving;}

	bool GetNextMovingTile(POINT *pt);
	//根据象素移动速度,获取当前象素位移
	POINT GetPixelSpeed(long angle,ulong period);
};


#endif // __MOVECOMPONENTMZ_H__