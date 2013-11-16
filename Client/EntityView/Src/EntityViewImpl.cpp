//==========================================================================
/**
* @file	  : EntityViewImpl.cpp
* @author : 
* created : 2008-1-30   13:35
*/
//==========================================================================

#include "stdafx.h"
#include "EntityComponent.h"
#include "EntityViewImpl.h"
#include "VisualComponent.h"
#include "MoveComponent.h"
//#include "StateComponent.h"

EntityViewImpl::EntityViewImpl() : mNeedDelete(false), mCalcWorld(true)
{
	//memset(&mComps, 0, sizeof(mComps));
	memset(mComps, 0, sizeof(mComps));
}

EntityViewImpl::~EntityViewImpl()
{
    //  将魔法效果的清除处理放置到实体视图被销毁时进行，这样做能够保证魔法效果的动画不会在实体死亡时会立刻中
    //  断，以保证动画的连贯性；
    onCommand(EntityCommand_ClearMagic, 0, 0);

	//gGlobalClient->getSceneManager()->removeEntity(getTile(), this);
	for (int i=0; i<MaxComponentIDCount; i++)
	{
		safeDelete(mComps[i]);
	}
}

void EntityViewImpl::setTile(const xs::Point& ptTile)
{
	if (ptTile.x != m_ptTile.x || ptTile.y != m_ptTile.y)
	{
		m_ptTile = ptTile;
		onMessage(msgTileChanged);
	}
}

bool EntityViewImpl::onCommand(ulong cmd, ulong param1, ulong param2)
{
	bool ret = false;
	for (int i=0; i<MaxComponentIDCount; i++)
	{
		if (mComps[i] && mComps[i]->handleCommand(cmd, param1, param2))
			ret = true;
	}
	return ret;
}

void EntityViewImpl::onEnterViewport(int priority)
{
	getComponent<VisualComponent>(ComponentIDVisual)->onEnterViewport(priority);
}

void EntityViewImpl::onLeaveViewport()
{
	getComponent<VisualComponent>(ComponentIDVisual)->onLeaveViewport();
}

void EntityViewImpl::drawPickObject(IRenderSystem* pRenderSystem)
{
	getComponent<VisualComponent>(ComponentIDVisual)->drawPickObject(pRenderSystem);
}

void EntityViewImpl::drawShadow(IRenderSystem* pRenderSystem)
{
	getComponent<VisualComponent>(ComponentIDVisual)->drawShadow(pRenderSystem);
}

void EntityViewImpl::draw(IRenderSystem* pRenderSystem)
{
	getComponent<VisualComponent>(ComponentIDVisual)->draw(pRenderSystem);
}

void EntityViewImpl::drawTopMost(IRenderSystem* pRenderSystem)
{
	getComponent<VisualComponent>(ComponentIDVisual)->drawTopMost(pRenderSystem);
}

bool EntityViewImpl::update(float tick, float deltaTick, IRenderSystem* pRenderSystem)
{
	if (getComponent<MoveComponent>(ComponentIDMove))
		getComponent<MoveComponent>(ComponentIDMove)->update(tick, deltaTick);
	return getComponent<VisualComponent>(ComponentIDVisual)->update(tick, deltaTick, pRenderSystem);
}

const xs::Rect& EntityViewImpl::getShowRect() const
{
	return getComponent<VisualComponent>(ComponentIDVisual)->getShowRect();
}

OccupantTileList* EntityViewImpl::getOccupantTileList() const
{
	return getComponent<VisualComponent>(ComponentIDVisual)->getOccupantTileList();
}

size_t EntityViewImpl::onLoad(Stream* stream, size_t len)
{
	return getComponent<VisualComponent>(ComponentIDVisual)->onLoad(stream, len);
}

size_t EntityViewImpl::onSave(Stream* stream) const
{
	return getComponent<VisualComponent>(ComponentIDVisual)->onSave(stream);
}

bool EntityViewImpl::isTransparent(int x, int y)
{
	return getComponent<VisualComponent>(ComponentIDVisual)->isTransparent(x, y);
}

void EntityViewImpl::onAddEntity()
{
	getComponent<VisualComponent>(ComponentIDVisual)->onAddEntity();
}

void EntityViewImpl::onRemoveEntity()
{
	getComponent<VisualComponent>(ComponentIDVisual)->onRemoveEntity();
}

void EntityViewImpl::setAngle(long angle)
{
	if (angle != mAngle)
	{
		/*这里有个bug
		mAngle为无符号，angle为有符号。
		当angle为负数时，赋值会出错
		*/
		//mAngle = angle;
		//while (mAngle < 0) mAngle += 360;
		//while (mAngle >= 360) mAngle -= 360;
		while( angle < 0 ) angle += 360;
		while( angle >= 360 ) angle -= 360;
		mAngle = angle;
		onMessage(msgDirChanged);
	}
}

void EntityViewImpl::setSpace(const Vector3& space)
{
	mSpace = space;
	onMessage(msgPosChanged);
}

// 组件之间的消息
void EntityViewImpl::onMessage(ulong msgId, ulong param1, ulong param2)
{
	for (int i=0; i<MaxComponentIDCount; i++)
	{
		if (mComps[i])
			mComps[i]->handleMessage(msgId, param1, param2);
	}
}

void EntityViewImpl::drawAlwayseTopMost(IRenderSystem* pRenderSystem)
{
	getComponent<VisualComponent>(ComponentIDVisual)->drawAlwayseTopMost(pRenderSystem);
}
void EntityViewImpl::drawTopMostEffect(IRenderSystem* pRenderSystem)
{
	getComponent<VisualComponent>(ComponentIDVisual)->drawTopMostEffect(pRenderSystem);
}

const AABB& EntityViewImpl::GetAABB()
{
	return getComponent<VisualComponent>(ComponentIDVisual)->GetAABB();
}

const Matrix4& EntityViewImpl::GetFullTransform()
{
	return getComponent<VisualComponent>(ComponentIDVisual)->GetFullTransform();
}


// 受击点为包围盒的中点
xs::Point EntityViewImpl::getAttackPos()
{
	/*xs::Point ptScreen;
	gGlobalClient->getSceneManager()->world2Screen(getOwner()->getWorld(), ptScreen);
	xs::Rect rc = entity->getShowRect();
	::OffsetRect(&rc, ptScreen.x, ptScreen.y);
	Rect rect;
	rect.left = rc.left;
	rect.top = rc.top;
	rect.right = rc.right - 1;
	rect.bottom = rc.bottom - 1;*/


	xs::Rect rc = getShowRect();
	xs::Point ptCenter;
	ptCenter.x  = (rc.right+rc.left)/2;
	ptCenter.y  = (rc.bottom+rc.top)/2;
	return ptCenter;
}