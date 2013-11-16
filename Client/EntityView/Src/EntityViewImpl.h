//==========================================================================
/**
* @file	  : EntityViewImpl.h
* @author : 
* created : 2008-1-30   13:34
* purpose : 实体对象的实现
*/
//==========================================================================

#ifndef __ENTITYVIEWIMPL_H__
#define __ENTITYVIEWIMPL_H__

#include "IEntityView.h"
#include "EntityComponent.h"

enum ComponentMessage
{
	msgTileChanged,		/// TILE位置改变 (0,0)
	msgDirChanged,		/// 方向改变 (0,0)
	msgPosChanged,		/// space位置改变 (0, 0)
	msgStateChanged,	/// 状态改变 (int state, 0)
	msgMoveStart,		/// 移动开始 (0,0)
	msgMoveFinished,	/// 移动完成 (0,0)
	msgAniFinished,		/// 动画播放完成 (0,0)
	msgMoveSpeedChanged,/// 移动速度改变 (const float* moveSpeed,0)
	msgMoveStyleChanged,/// 移动类型改变 (0,0)
	msgMountChanged,	/// 骑乘类型改变

	MaxComponentMessage,
};

class EntityViewImpl : public EntityView
{
	EntityComponent*	mComps[MaxComponentIDCount]; /// 组件列表,创建时确保都有效
	bool				mNeedDelete;
public:
	bool				mCalcWorld;

public:
	EntityViewImpl();
	virtual ~EntityViewImpl();

public:
	void setComponent(ComponentID id, EntityComponent* comp)
	{ 
		Assert(id < MaxComponentIDCount); 
		if (id >= ComponentIDVisual && id < MaxComponentIDCount)
		{
			mComps[id] = comp; 
		}		
	}

	template <class T>
	inline T* getComponent(ComponentID id) const
	{ Assert(id < MaxComponentIDCount); return static_cast<T*>(mComps[id]); }

	// IEntityView
public:
	virtual void release(){
		//if (hasFlag(flagInMap))
		//	gGlobalClient->getSceneManager()->removeEntity(getTile(), this);
		//setUserData(0);

		// 原先地图上的物件在跳地图时采用延迟释放，但是场景管理已经清空了会造成无法正确释放，这里立即释放掉  [5/5/2011 zgz]
		if (hasFlag(flagReleaseImmediate))
		{
			delete this;
			return;
		}

		// zgz 原先这里只检查Fade标记是不够的，如果实体不在场景管理的显示列表或淡出列表，则会有可能无法真正删除造成内存泄露
		ISceneManager2* pSceneManager = gGlobalClient->getSceneManager();
		if (NULL != pSceneManager && pSceneManager->IsReferencedBySceneManager(this) && hasFlag(flagFade))
		{
			setNeedDelete();
		}
		else
			delete this;
	}
	virtual void setTile(const xs::Point& ptTile);
	virtual bool onCommand(ulong cmd, ulong param1 = 0, ulong param2 = 0);
	virtual void onEnterViewport(int priority = 0);
	virtual void onLeaveViewport();
	virtual void drawPickObject(IRenderSystem* pRenderSystem);
	virtual void drawShadow(IRenderSystem* pRenderSystem);
	virtual void draw(IRenderSystem* pRenderSystem);
	virtual void drawTopMost(IRenderSystem* pRenderSystem);
	virtual bool update(float tick, float deltaTick, IRenderSystem* pRenderSystem);
	virtual const xs::Rect& getShowRect() const;
	virtual OccupantTileList* getOccupantTileList() const;
	virtual size_t onLoad(Stream* stream, size_t len);
	virtual size_t onSave(Stream* stream) const;
	virtual bool isTransparent(int x, int y);
	virtual void onAddEntity();
	virtual void onRemoveEntity();
	virtual void drawAlwayseTopMost(IRenderSystem* pRenderSystem);
	/// 绘制顶层特效
	virtual void drawTopMostEffect(IRenderSystem* pRenderSystem);

	// 获取受击点
	virtual xs::Point getAttackPos();

public:
	void setResType(int type)				{ mResType = type; }
	void setResId(ulong id)					{ mResId = id; }
	void setArea(const xs::Rect& rc)			{ m_rcArea = rc; }
	void setSortLeft(const xs::Point& pt)		{ m_ptLeft = pt; }
	void setSortRight(const xs::Point& pt)		{ m_ptRight = pt; }
	virtual void setAngle(long angle);
	virtual void setSpace(const Vector3& space);
	void setOccupantValue(ulong occVal)		{ mOccVal = occVal; }
	void setNeedDelete(bool del = true)					{ mNeedDelete = del; }
	bool isNeedDelete() const				{ return mNeedDelete; }

	// 组件消息
	void onMessage(ulong msgId, ulong param1 = 0, ulong param2 = 0);

	bool isMainPlayer() const { return this == (EntityView*)getHandleData(gGlobalClient->getPlayer()); }

	virtual const AABB& GetAABB();
	virtual const Matrix4& GetFullTransform();
};

#endif // __ENTITYVIEWIMPL_H__