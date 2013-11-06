//==========================================================================
/**
* @file	  : EntityComponent.h
* @author : 
* created : 2008-1-25   17:40
* purpose : 实体组件
*/
//==========================================================================

#ifndef __ENTITYCOMPONENT_H__
#define __ENTITYCOMPONENT_H__



typedef ulong ComponentID;

enum
{
	ComponentIDVisual,	/// 可视化组件
	ComponentIDMove,	/// 移动组件
	ComponentIDState,	/// 状态组件

	MaxComponentIDCount,
};


class EntityViewImpl;

class EntityComponent
{
	EntityViewImpl* mOwner; /// 组件拥有者对象
public:
	EntityComponent() : mOwner(NULL)		{ }
	virtual ~EntityComponent()				{ }

	inline void setOwner(EntityViewImpl* owner)	{ mOwner = owner; }
	inline EntityViewImpl* getOwner() const	{ return mOwner; };

	/// 这里发生在new对象后的初始化过程
	virtual void create()					{}

	/// 这里发生在delete对象前的清理过程
	virtual void close()					{}

	/// 处理命令，如果处理了，就返回true，否则返回false
	virtual bool handleCommand(ulong cmd, ulong param1, ulong param2)	{ return false; }

	/// 处理组件间的消息
	virtual void handleMessage(ulong msgId, ulong param1, ulong param2)	{ }
};


#endif // __ENTITYCOMPONENT_H__