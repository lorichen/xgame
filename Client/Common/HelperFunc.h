//==========================================================================
/**
* @file	  : HelperFunc.h
* @author : 
* created : 2008-2-19   15:05
* purpose : 辅助函数
*/
//==========================================================================

#ifndef __HELPERFUNC_H__
#define __HELPERFUNC_H__

#include "Radial.h"

typedef ISceneManager2	ITileContainer;
typedef Tile			TileObj;
#define GetTile			getTile
typedef IEntityClient	IEntityManager;

#define isTileValid(tile)		(tile->isValid())
#define isTileBlock(tile)		(isTileMoveBlock(tile) || isTileFlyBlock(tile) || isTileSpreadBlock(tile))
#define isTileMoveBlock(tile)	(tile->isBlock() != 0)
#define isTileFlyBlock(tile)	(tile->isFlyBlock() != 0)
#define isTileSpreadBlock(tile)	(tile->isSpreadBlock() != 0)
#define isTileBlock_Logic(tile)			(isTileMoveBlock_Logic(tile) || isTileFlyBlock_Logic(tile) || isTileSpreadBlock_Logic(tile))
#define isTileMoveBlock_Logic(tile)		(tile->hasFlag(Tile::tGroundBlock_Logic))
#define isTileFlyBlock_Logic(tile)		(tile->hasFlag(Tile::tFlyBlock_Logic))
#define isTileSpreadBlock_Logic(tile)	(tile->hasFlag(Tile::tSpreadBlock_Logic))

#define getISkillManager	gGlobalClient->getSkillManager
#define getISchemeEngine	gGlobalClient->getSchemeEngine
#define getIEventEngine		gGlobalClient->getEventEngine
#define getITimerAxis		gGlobalClient->getTimerAxis
#define getIZoneManager		//gGlobalClient->GetZoneManager
#define getIAttackFactory	//gGlobalClient->GetAttackFactory
#define getINetConnection(crt)	gGlobalClient->getNetConnection()

/// 在两个数字间产生随机数（含两数据）
inline int getRand(int _min, int _max)
{
	static CRandGenerator sRandGenerator;
	if (_max > _min)
		return _min + (int)(sRandGenerator.GetDWORD() % (_max - _min + 1));
	else if (_max < _min)
		return _max + (int)(sRandGenerator.GetDWORD() % (_min - _max + 1));
	return _max;
}

inline ICreature* getCreature(IEntity* entity)
{
	return (entity && entity->GetEntityClass()->IsCreature()) ? static_cast<ICreature*>(entity) : NULL;
}

inline IPerson* getPerson(IEntity* entity)
{
	return (entity && entity->GetEntityClass()->IsPerson()) ? static_cast<IPerson*>(entity) : NULL;
}

inline bool isDeadCreature(ICreature* crt)
{
	if (crt == NULL)
	{
		return true;
	}

	if (crt->GetEntityClass()->IsPet())
	{
		if (crt->GetNumProp(CREATURE_PET_PROP_DEATH) != 0)
		{
			return true;
		}
	}

	return false;
}

inline ICreature* getValidCreature(IEntity* entity)
{
	return (entity && entity->GetEntityClass()->IsCreature() && 
		!isDeadCreature(static_cast<ICreature*>(entity))) ?
		static_cast<ICreature*>(entity) : NULL;
}

inline IEntityManager* getEntityManager()
{
	return gGlobalClient->getEntityClient();
}


inline int getDistance(const POINT& from, const POINT& to)
{
	return gGlobalClient->getSceneManager()->tileDistance(from, to);
}

inline ITileContainer* getTileContainer(IEntity* entity)
{
	return gGlobalClient->getSceneManager();
}

inline IEntity* uid2Entity(UID uid)
{
	IEntityManager* entityManager = getEntityManager();
	return (entityManager) ? entityManager->Get(uid) : NULL;
}

/// 获取射线上最后一个友好的TILE（即ptTo的前面一个非阻挡的TILE）
inline bool getFriendTile_Radial(const POINT& ptFrom, const POINT& ptTo, ITileContainer* tileContainer, POINT& pt)
{
	if (ptFrom.x==ptTo.x && ptFrom.y==ptTo.y)
	{
		pt.x=ptFrom.x;
		pt.y=ptFrom.y;
		return true;
	}
	Radial line(ptTo.x, ptTo.y, ptFrom.x, ptFrom.y);

	POINT ptTempTile = ptTo;
	while (1)
	{
		line.move(ptTempTile);

		if (ptTempTile.x == ptFrom.x && ptTempTile.y == ptFrom.y)
			return false;

		TileObj* tile = tileContainer->GetTile(ptTempTile);
		if (!tile || !isTileValid(tile))
			continue;

		bool isBlock = isTileMoveBlock(tile) || isTileMoveBlock_Logic(tile);
		if (!isBlock)
			break;
	}

	pt = ptTempTile;
	return true;
}

// 获取ptTo->ptFrom方向上第一个阻挡点前的非阻挡点
inline bool getFriendTile_Last(const POINT& ptFrom, const POINT& ptTo, ITileContainer* tileContainer, POINT& pt)
{
	if (ptFrom.x==ptTo.x && ptFrom.y==ptTo.y)
	{
		pt.x=ptFrom.x;
		pt.y=ptFrom.y;
		return true;
	}
	Radial line(ptTo.x, ptTo.y, ptFrom.x, ptFrom.y);

	const int nCount = 1000;
	POINT ptTempTile = ptTo;
	POINT ptRet;
	int i=0;
	while (1)
	{
		ptRet = ptTempTile;
		line.move(ptTempTile);

		if (ptTempTile.x == ptFrom.x && ptTempTile.y == ptFrom.y)
		{
			ptRet = ptTempTile;
			break;
		}

		TileObj* tile = tileContainer->GetTile(ptTempTile);
		if (!tile || !isTileValid(tile))
			break;

		bool isBlock = isTileMoveBlock(tile) || isTileMoveBlock_Logic(tile);
		if (isBlock)
			break;
		if (i++==nCount)
		{
			ErrorLn("是否有可能是死循环;ptFrom.x:"<<ptFrom.x<<"ptFrom.y:"<<ptFrom.y<<"ptTo.x:"<<ptTo.x<<"ptTo.y:"<<ptTo.y);
			ptRet = ptFrom;
			break;
		}
	}

	pt = ptRet;
	return true;
}

/// 根据线段的两端点，计算有最大距离限制的点
inline POINT calcLimitedPoint_Line(const POINT& ptFrom, const POINT& ptTo, ulong maxDist)
{
	if (maxDist == 0) return ptFrom;
	ulong dist = (ulong)getDistance(ptFrom, ptTo);
	if (dist <= maxDist)
		return ptTo;

	float rate = (float)maxDist / dist;
	int deltaX = ptTo.x - ptFrom.x;
	int deltaY = ptTo.y - ptFrom.y;
	int dx = (deltaX < 0) ? -deltaX : deltaX;
	int dy = (deltaY < 0) ? -deltaY : deltaY;

	POINT pt;
	pt.x = (dx == 0) ? ptFrom.x : ptFrom.x + (long)(rate * dx + 0.5f);
	pt.y = (dy == 0) ? ptFrom.y : ptFrom.y + (long)(rate * dy + 0.5f);

	if (deltaX < 0) pt.x = -pt.x;
	if (deltaY < 0) pt.y = -pt.y;
	return pt;
}

/** 针对“击退箭”
@param     ptFrom :  源点
@param     ptTo   :  目标点
@param     nLen   :  击退的距离
return     POINT  :  击退后目标所在的点
*/
inline POINT GetLineLastTile(const POINT& ptFrom, const POINT& ptTo, int nLen)
{
	POINT ptRet;

	// ptFrom和ptTo是同一个点
	if ((ptFrom.x == ptTo.x) && (ptFrom.y == ptTo.y))
	{
		ptRet.x = ptFrom.x;
		ptRet.y = ptTo.y + nLen;
	}

	if (ptFrom.x == ptTo.x)
	{
		// 源和目标在同一个点上在此处也可以单独考虑，if (ptFrom.y == ptTo.y)，打出来的直接可以是随机的
		int nYMin = ptFrom.y;

		// 如果源点在目标点的下方，自己所在的点也算上
		if (ptFrom.y > ptTo.y)
		{
			ptRet.x = ptFrom.x;
			ptRet.y = ptTo.y - nLen;
		}
		else
		{
			ptRet.x = ptFrom.x;
			ptRet.y = ptTo.y + nLen;
		}
	}
	else
	{
		if (ptFrom.y == ptTo.y)
		{
			// 如果源点在目标点的右方，自己所在的点也算上
			if (ptFrom.x > ptTo.x)
			{
				ptRet.x = ptTo.x - nLen;
				ptRet.y = ptFrom.y;
			}
			else
			{
				ptRet.x = ptTo.x + nLen;
				ptRet.y = ptFrom.y;
			}
		}
		else
		{
			// 过源和目标两点方程y = kx + b; k为斜率
			float k =  float(ptTo.y - ptFrom.y) / (ptTo.x - ptFrom.x);
			float absk = fabs(k);
			float b = float(ptFrom.y * ptTo.x - ptFrom.x * ptTo.y) / (ptTo.x - ptFrom.x);

			// 斜率 <= 1，按x递增，求y的近似值
			if (absk <= 1)
			{
				// 如果源点在目标点的右方，自己所在的点也算上
				if ((ptFrom.y > ptTo.y && k > 0) || (ptFrom.y < ptTo.y && k < 0))
				{
					ptRet.x = ptTo.x - nLen;
				}
				else
				{
					ptRet.x = ptTo.x + nLen;
				}

				// 由x求y，四舍五入
				ptRet.y = LONG(k * ptRet.x + b + 0.5); 
			}
			else
			{
				// 如果源点在目标点的下方，自己所在的点也算上
				if (ptFrom.y > ptTo.y)
				{
					ptRet.y = ptTo.y - nLen;
				}
				else
				{
					ptRet.y = ptTo.y + nLen;
				}

				// 由y求x，四舍五入
				ptRet.x = LONG( (ptRet.y - b) / k + 0.5 );
			}
		}
	}

	return ptRet;
}

#define fillData(ob, modId, msgId, buf, len) \
	SGameMsgHead* ___msgHead = (SGameMsgHead*)ob.data();\
	___msgHead->SrcEndPoint = MSG_ENDPOINT_CLIENT;\
	___msgHead->DestEndPoint = MSG_ENDPOINT_ZONE;\
	___msgHead->wKeyModule = modId;\
	___msgHead->wKeyAction = (ushort)msgId;\
	ob.offset(sizeof(SGameMsgHead));\
	if ((buf) && (len))\
	ob.push_back((buf), (len))


#define fillDataEx(ob, destEP, modId, msgId, buf, len) \
	SGameMsgHead* ___msgHead = (SGameMsgHead*)ob.data();\
	___msgHead->SrcEndPoint = MSG_ENDPOINT_CLIENT;\
	___msgHead->DestEndPoint = destEP;\
	___msgHead->wKeyModule = modId;\
	___msgHead->wKeyAction = (ushort)msgId;\
	ob.offset(sizeof(SGameMsgHead));\
	if ((buf) && (len))\
	ob.push_back((buf), (len))

// 客户端发给服务器的Action头不需要
#define fillActionData(ob, crt, msgId, buf, len) fillData(ob, MSG_MODULEID_ACTION, msgId, buf, len)


#define broadcastData(ob, crt, excludeSelf) \
	if (ob.good()) {\
		getGameZone(crt)->BroadcastData(\
			(crt)->GetMapLoc(), \
			ob.data(),\
			ob.size(), \
			excludeSelf ? (crt) : 0);\
	} else {\
		Assert(ob.good());\
	}

#define syncUpdate(ob, crt) \
	if (ob.good()) {\
		getINetConnection(crt)->SendData(\
			ob.data(),\
			ob.size());\
	} else {\
		Assert(ob.good());\
	}

#define setEntityNumProp(entity, propId, val, synch) entity->SetNumProp(propId, val);

inline void sendEnmityMessage(ICreature* srcCreature, ICreature* destCreature, int enmity)
{
}




#endif // __HELPERFUNC_H__