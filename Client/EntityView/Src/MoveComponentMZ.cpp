//==========================================================================
/**
* @file	  : MoveComponentMZ.cpp
* @author : 
* created : 2008-1-29   11:04
*/
//==========================================================================

#include "stdafx.h"
#include "MoveComponentMZ.h"
#include "IAudioEngine.h"
#include "IFormManager.h"

//#define TIME_UNIT	16
#	define     MAX_MOVE_STEP     16

MoveComponentMZ::MoveComponentMZ():
mIsMoving(false),m_nPlayDelay(0)
{
}

MoveComponentMZ::~MoveComponentMZ()
{
	stopMove();
}
void MoveComponentMZ::create()
{
	mBaseTimePerTile = 300.0f;
	mTimePerTile = 300.0f;
	mOldTicks = 0;
	mTicks = 0;
}

void MoveComponentMZ::close()
{

}

bool MoveComponentMZ::handleCommand(ulong cmd, ulong param1, ulong param2)
{
	switch (cmd)
	{
	case EntityCommand_Move:
		{
			if (getOwner()->isMainPlayer())
			{
				breakable;
			}

			const xs::Point* pathList = (const xs::Point*)param1;
			size_t count = (size_t)param2;

			if (!pathList) return false;

			if (move(pathList, count))
			{
				mIsMoving = true;
				getOwner()->onMessage(msgMoveStart);
			}
		}
		break;
	case EntityCommand_Stand:
		if (getOwner()->isMainPlayer())
		{
			breakable;
		}
		if (param1 != 0) // 传入了新的tile坐标，可用于异常站立或瞬移
		{
			const xs::Point& newTile = *(const xs::Point*)param1;
			const xs::Point& ptTile = getOwner()->getTile();
			if (ptTile.x != newTile.x || ptTile.y != newTile.y)
			{
#if 0
				if (getOwner()->isMainPlayer())
				{
					Error("EntityCommand_Stand:"
						<<ptTile.x<<","<<ptTile.y<<"--->"
						<<newTile.x<<","<<newTile.y<<endl);
				}
				else
				{
					if (getOwner()->isMainPlayer())
					{
						Warning("MainPlayer EntityCommand_Stand:"
							<<ptTile.x<<","<<ptTile.y<<"--->"
							<<newTile.x<<","<<newTile.y<<endl);
					}
					else
					{
						Warning("NoMainPlayer EntityCommand_Stand:"
							<<ptTile.x<<","<<ptTile.y<<"--->"
							<<newTile.x<<","<<newTile.y<<endl);
					}
				}
#endif
				gGlobalClient->getSceneManager()->moveEntity(ptTile, newTile, getOwner());
			}
		}

		if (mIsMoving)
		{
			if (getOwner()->getEntityViewObserver())
				getOwner()->getEntityViewObserver()->onMoveBreak(getOwner());
		}

		stopMove();
		break;
	case EntityCommand_IsMoving:
		{
			*(bool*)param1 = mIsMoving;
			if (mIsMoving)
			{
				m_nPlayDelay = m_nPlayDelay + 1;
				if (m_nPlayDelay == 25)
				{
					// 模型走路的声音
					IFormManager* pFormMange = gGlobalClient->getFormManager();
					const ConfigCreature* mConfig = ConfigCreatures::Instance()->getCreature(getOwner()->getResId());
					if (pFormMange && mConfig->aa.soundMove != 0) pFormMange->PlaySound(mConfig->aa.soundMove,0,1,SOUNDRES_TYPE_SOUND);
					m_nPlayDelay = 0;
				}
			}          
		}
		break;
	case EntityCommand_NextMovingTile:
		{
			return GetNextMovingTile((xs::Point*)param1);
		}

		break;
	}

	return MoveComponent::handleCommand(cmd, param1, param2);
}

void MoveComponentMZ::handleMessage(ulong msgId, ulong param1, ulong param2)
{
	switch (msgId)
	{
	case msgMoveSpeedChanged:
		{
			//按速度比算一下
			if(mTicks>0)
			{
				mTicks = mTicks * param1/mBaseTimePerTile;
			}

			mBaseTimePerTile = param1;
			
			
			if(gGlobalClient->getSceneManager()&&gGlobalClient->getSceneManager()->getRunType() != RUN_TYPE_EDITOR)
			{
				IEntity* pEntity = (IEntity*)getOwner()->getUserData();
				if(pEntity != NULL && pEntity->GetEntityClass()->IsCreature())
				{
					if (((ICreature*)pEntity)->GetNumProp(CREATURE_PROP_CUR_MOVE_STYLE) == EMoveStyle_Run)
					{
						//因为服务器的定时器不准确,服务器的定时器大概每跨一个格子慢20ms,服务器越卡这个补偿要越大,现在客户端要比服务器的速度慢一点
						mBaseTimePerTile += 20;
					}
					else
					{
						mBaseTimePerTile += 10;
					}
				}
			}
		
			mTimePerTile = mBaseTimePerTile;
		}
	}

	MoveComponent::handleMessage(msgId, param1, param2);
}

// 角色沿着指定的路径移动
// nodes: 路径列表指针(xs::Point*);
// count: 路径列表节点数目
bool MoveComponentMZ::move(const xs::Point* nodes, size_t count)
{
	//IEntity* pEntity = (IEntity*)getOwner()->getUserData();
	//if(pEntity&&!pEntity->GetEntityClass()->IsPerson())
	//	Info("move="<<getTickCount());

	// 检查参数
	if (!nodes || count < 2) // 路径只有一个节点(那就是当前TILE点)
		return false;

//	mPileTicks = 0;

	// 是否是从原地开始走路，否则可能是服务器强制拉人
	{
		const xs::Point& ptNewTile = nodes[0];
		const xs::Point& ptCurTile = getOwner()->getTile();
		
		if (ptCurTile.x != ptNewTile.x || ptCurTile.y != ptNewTile.y) // 强制拉人
		{
			if(!mPathList.empty()&&( mPathList.next().x == ptNewTile.x && mPathList.next().y == ptNewTile.y) )
			{
				//在移动中的下一个tile,还是合法的
				int i=0;
			}
			else
			{
				if (getOwner()->isMainPlayer())
				{
					Warning("Entity has moved forcibly by Server:"
						<<ptCurTile.x<<","<<ptCurTile.y<<"--->"
						<<ptNewTile.x<<","<<ptNewTile.y<<endl);
				}

				if (gGlobalClient->getSceneManager()->moveEntity(ptCurTile, ptNewTile, getOwner()))
				{
					mPathList.clear(); // 清除上次的路径列表
					Vector3 curPos;
					gGlobalClient->getSceneManager()->tile2Space(ptNewTile, curPos);

					getOwner()->setSpace(curPos);
				}
				else
				{
					Error("moveEntity failed"<<endl);
				}
			}
			
		}
	}

	if (!mPathList.empty()) // 上一次的还没走完
	{
		const xs::Point& ptCurTile = getOwner()->getTile();
		const xs::Point& back = mPathList.back();
		if (nodes[count-1].x == back.x && nodes[count-1].y == back.y
			&& nodes[0].x == ptCurTile.x && nodes[0].y == ptCurTile.y) // 目标相同，忽略
		{
			return true;
		}
		else // 看来有可能要往回走了(如果是顺着以前的方向，则不需要回走)
		{
			//这个要求寻路的时候是按下一个目标点进行寻的路
			if(nodes[0].x == mPathList.next().x && nodes[0].y == mPathList.next().y )
			{
				mPathList.trimRight(mPathList.current() + 1); // 裁掉先前路径的当前TILE以后的所有TILE
				mPathList.add(nodes, count);	// 加入新的路径
				return true;
			}
			/*
			else
			{
				//mPathList.trimRight(mPathList.current());
				mPathList.clear();
				mPathList.add(nodes, count);	// 加入新的路径
				resetMoveInfo();
				return true;
			}*/
		}
	}
	//else
	{
		// 保存路径列表
		mPathList.clear();
		mPathList.add(nodes, count);
	}

	//定时器是导致抖动原因
	//gGlobalClient->getTimerAxis()->SetTimer(0, 1, this);

	resetMoveInfo();
	//mIsMoving = true;

	return true;
}



void MoveComponentMZ::resetMoveInfo()
{
	//Info("resetMoveInfo");
	if(mPathList.empty())
		mTicks = 0;
	//else
	//	Info("mTicks = "<<mTicks<<endl);

	mOldTicks = gGlobalClient->getTimeStamp();
	gGlobalClient->getSceneManager()->tile2Space(mPathList.cur(), mStartPos);
	gGlobalClient->getSceneManager()->tile2Space(mPathList.next(), mNextPos);

	//走的时候斜跨*1.414
	mTimePerTile = mBaseTimePerTile*1.414f;

	//IEntity* pEntity = (IEntity*)getOwner()->getUserData();
	//if(pEntity&&pEntity->GetEntityClass()->IsPerson())
	//{
		if(mPathList.cur().x == mPathList.next().x || mPathList.cur().y == mPathList.next().y)
			mTimePerTile = mBaseTimePerTile;
	//}


	//如果是在原地踏步，不需要改变方向
	if(mPathList.cur().x != mPathList.next().x || mPathList.cur().y != mPathList.next().y)
	{
		long angle = calcAngle8_tile(mPathList.cur(), mPathList.next());
		getOwner()->setAngle(angle);
	}	
}



bool MoveComponentMZ::update(ulong curTicks, ulong deltaTicks)
{
	if(mOldTicks>0)
		deltaTicks = gGlobalClient->getTimeStamp() - mOldTicks;

	// 此处在最小化切换画面时；会出现deltaTicks >1000的情况；导致自动寻路过程中；
	// 服务器强制拉人；所以讲deltaTicks限制成一个合里的范围；（WZH）

	if(deltaTicks>300)
		deltaTicks = 300;
	mOldTicks = curTicks;
	
	//  如果实体视图已经被标记为即将删除，则不再进行任何处理；
    if (getOwner()->isNeedDelete())
    {
        return false;
    }

	if (!mIsMoving) 
		return true;
	if(getOwner()->isMainPlayer())
	{
		//Info("delta:"<<deltaTicks<<endl);
		breakable;
	}
	moveStep(deltaTicks);
	return true;
}

bool MoveComponentMZ::GetNextMovingTile(xs::Point * pt)
{
	if(!mPathList.empty()&& pt )
		*pt = mPathList.next();
	return mIsMoving;
}
xs::Point MoveComponentMZ::GetPixelSpeed(long angle,ulong period)
{
	xs::Point ptRet;
	ptRet.x = 0;
	ptRet.y = 0;
	if(angle==0)
	{
		ptRet.x = 320*period/mTimePerTile;
		ptRet.y = 160*period/mTimePerTile;
	}

	if(angle==45)
	{
		ptRet.x = 640*period/mTimePerTile;
		ptRet.y = 0; 
	}
	if(angle==90)
	{
		ptRet.x = 320*period/mTimePerTile;
		ptRet.y = -(160*period/mTimePerTile);
	}
	if(angle==135)
	{
		ptRet.x = 0;
		ptRet.y = -(320*period/mTimePerTile);
	}
	if(angle==180)
	{
		ptRet.x = -(320*period/mTimePerTile);
		ptRet.y = -(160*period/mTimePerTile);
	}
	
	if(angle==225)
	{
		ptRet.x = -(640*period/mTimePerTile);
		ptRet.y = 0;
	}
	if(angle==270)
	{
		ptRet.x = -(320*period/mTimePerTile);
		ptRet.y = 160*period/mTimePerTile;
	}
	if(angle==315)
	{
		ptRet.x = 0;
		ptRet.y = 320*period/mTimePerTile;
	}

	return ptRet;
}

#define min(a,b)            (((a) < (b)) ? (a) : (b))

// 生物行走一步
void MoveComponentMZ::moveStep(ulong period)
{
	//TODO 是否忽略阻挡
	bool bIgnoBlock = false;
	IEntity* pEntity = (IEntity*)getOwner()->getUserData();
	if (NULL != pEntity)
	{
		if (pEntity->GetNumProp(CREATURE_PROP_IGNO_BLCOK) > 0)
		{
			bIgnoBlock = true;
		}
	}

	if (mNeedWait)
	{
		Tile* pTile = gGlobalClient->getSceneManager()->getTile(mPathList.next());
		
		if (!bIgnoBlock && pTile->isBlock())
			return;
		else
			mNeedWait = false;
	}
	//if(period>50)
		//Info("period:"<<period<<endl);
	mTicks += period;

	if (mTicks >= mTimePerTile) // 到达下一个TILE中心点
	{
		// 路径递增一次
		++mPathList;

		if (mPathList.isLast()) // 到了终点
		{	
			getOwner()->setSpace(mNextPos);
			stopMove();
			//Info("越过中心点 - 结束"<<endl);
		}
		else
		{
			IPerson * pHero = gGlobalClient->getEntityClient()->GetHero();
			IEntity* pEntity = (IEntity*)getOwner()->getUserData();
			if(pHero && pEntity && pHero->GetUID() == pEntity->GetUID())
			{
				// 优先判断是否关闭交互窗口(在移动过程中进行判断)
				xs::Point ptMapLoc = pHero->GetMapLoc();
				IFormManager* pFromManager = gGlobalClient->getFormManager();
				if (pFromManager)
				{
					pFromManager->CloseTaskFormWinodow(ptMapLoc.x ,ptMapLoc.y,false);
				}

				if (mPathList.current()% (MAX_MOVE_STEP-1) == 0 && mPathList.current() != 0 )
				{ 
					SGameMsgHead gamemsghead;
					gamemsghead.SrcEndPoint = MSG_ENDPOINT_CLIENT;
					gamemsghead.DestEndPoint = MSG_ENDPOINT_ZONE;
					gamemsghead.wKeyModule = MSG_MODULEID_ACTION;
					gamemsghead.wKeyAction = MSG_ACTION_MOVE;

					SMsgActionMaster_CS msgmovemaster;
					msgmovemaster.uidActionMaster = pEntity->GetUID();

					SMsgActionMove_CS msgactionmove;
					ulong ulsize = mPathList.size()- mPathList.current();

					msgactionmove.dwPathLen = min(ulsize,MAX_MOVE_STEP);

					obuf512 obuf;
					obuf.push_back(&gamemsghead, sizeof(gamemsghead));
					obuf.push_back(&msgmovemaster, sizeof(msgmovemaster));
					obuf.push_back(&msgactionmove, sizeof(msgactionmove));
					obuf.push_back(&mPathList.cur(),msgactionmove.dwPathLen * sizeof(xs::Point));
					IConnection * pConn = gGlobalClient->getNetConnection();
					if( pConn) 
					{
						pConn->SendData(obuf.data(), obuf.size());
					}
				}
			}
			//end

			ulong delta = mTicks - (ulong)mTimePerTile;
			mTicks = 0;
			resetMoveInfo();
			
			if (delta > 0)
			{
				//Info("越过了路径目标点"<<endl);
				moveStep(delta);
			}
				//Info("越过中心点 - 方向改变"<<endl);
			if (getOwner()->getEntityViewObserver())
				getOwner()->getEntityViewObserver()->onTileChanged(getOwner(), getOwner()->getTile());
		}
		return;
	}
	if (mTicks >= mTimePerTile/2) // 越过TILE交界处
	{
		Tile* pTile = gGlobalClient->getSceneManager()->getTile(mPathList.next());
		if (!bIgnoBlock && pTile->isBlock())
		{
			//Info("Need Wait!"<<endl);
			mNeedWait = true;
			return;
		}

		//Info("越过边界("<<mPathList.cur().x<<","<<mPathList.cur().y<<")->("
		//	<<mPathList.next().x<<","<<mPathList.next().y<<")"<<endl);
		// 移动实体在地图上的TILE位置
		if(getOwner()->isMainPlayer())
		{
			breakable;
		}
		getOwner()->mCalcWorld = false;
		gGlobalClient->getSceneManager()->moveEntity(mPathList.cur(), mPathList.next(), getOwner());

		getOwner()->mCalcWorld = true;
	}

	//如果是在原地踏步，不需要改变坐标
	if(mPathList.cur().x != mPathList.next().x || mPathList.cur().y != mPathList.next().y)
	{
		//不使用空间坐标了,采用2D屏幕象素坐标
		long angle = calcAngle8_tile(mPathList.cur(), mPathList.next()); //方向
		xs::Point ptStart;
		gGlobalClient->getSceneManager()->space2World(mStartPos,ptStart);
		//Info("ptStart:"<<ptStart.x<<","<<ptStart.y<<endl);

		xs::Point ptAdd = GetPixelSpeed(angle,mTicks);

		//四舍五入
		int nAdd = 0;
		if(ptAdd.x>0)
		{
			if(ptAdd.x%10>=5)
				nAdd = 1;
		}
		else
		{
			if(ptAdd.x%10<=-5)
				nAdd = -1;
		}

		ptStart.x +=ptAdd.x/10+nAdd;// + ptAdd.x%10>=5?1:0;
		nAdd = 0;
		if(ptAdd.y>0)
		{
			if(ptAdd.y%10>=5)
				nAdd = 1;
		}
		else
		{
			if(ptAdd.y%10<=-5)
				nAdd = -1;
		}

		ptStart.y +=ptAdd.y/10+nAdd;// + ptAdd.y%10>=5?1:0;
		//Info("ptAdd:"<<ptAdd.x<<","<<ptAdd.y<<endl);
		Vector3 curPos;
		gGlobalClient->getSceneManager()->world2Space(ptStart,curPos);
		getOwner()->setSpace(curPos);
	}	
}


void MoveComponentMZ::stopMove()
{
	if (gGlobalClient->getSceneManager()->getRunType() == RUN_TYPE_EDITOR)
	{
		return;
	}
	
	IEntity* pEntity = (IEntity*)getOwner()->getUserData();
	//if(pEntity&&!pEntity->GetEntityClass()->IsPerson())
	//	Info("stop="<<getTickCount());

	mNeedWait = false;
	mPathList.clear();
	//gGlobalClient->getTimerAxis()->KillTimer(0, this);
	mIsMoving = false;
	getOwner()->onMessage(msgMoveFinished);
	if (getOwner()->getEntityViewObserver())
		getOwner()->getEntityViewObserver()->onMoveFinished(getOwner());
	mTicks = 0;
}

