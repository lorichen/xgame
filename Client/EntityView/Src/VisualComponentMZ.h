//==========================================================================
/**
* @file	  : VisualComponentMZ.h
* @author : 
* created : 2008-1-30   12:56
* purpose : 可视化组件(MZ)
*/
//==========================================================================

#ifndef __VISUALCOMPONENTMZ_H__
#define __VISUALCOMPONENTMZ_H__

#include "VisualComponent.h"
#include "ActionList.h"
#include "MagicView.h"
#include "ShadowManager.h"
#include "Modifier.h"
#include "ConfigCreatures.h"
#include "ConfigCreatureRes.h"
#include "JumpManager.h"
#include "ISchemeCenter.h"
#include "VisualComponentState.h"
#include "RideManager.h"
#include "ChangePartManager.h"

struct ModelNodeStateRecorder //记录组件的节点的各种状态
{
	//骑宠相关
	ModelNode*	m_pRideNode;//骑宠节点
	std::string m_strBindPoint;//骑乘绑定点

	//换装相关
	ModelNode*	m_pBodyNode;//换装节点

	//最终渲染和更新用的节点
	ModelNode*	m_pCompositeNode;//合成后的节点

	//通用状态
	const ConfigCreature*	m_pConfig;			/// 生物配置
	const ConfigCreature*	m_pRideConfig;		/// 骑乘配置
	float					m_fScale;			///模型在初始缩放比例的基础上的整体缩放比例
	float					m_fAniSpeed;		///模型的动画速度
	ulong					m_ulMoveSpeed;		/// 移动速度
	ulong					m_ulTilePerTile;	/// 移动一个TILE的时间
	ulong					m_ulSitState;       /// 坐下（0：没坐下，1：正在坐，2，已坐下）
	bool					m_bMoveStyleChange; /// 移动方式是否改变
	long					m_lCurAngle;		///当前旋转角度
	bool					m_bPetDie;//
	ulong					m_curActionId;	//当前的动作
	ulong					m_curRepActionId;//当前替换动作id
	int						m_curActionLoops;//当前动作循环次数

public: //构造函数
	ModelNodeStateRecorder():m_pRideNode(0),
		m_strBindPoint(),
		m_pBodyNode(0),
		m_pCompositeNode(0),
		m_pConfig(0),
		m_pRideConfig(0),
		m_fScale(1.0f),
		m_fAniSpeed(1.0f),
		m_ulMoveSpeed(EMOVESPEED_NORAML),
		m_ulTilePerTile(EMOVETIMEPERTILE_NORMAL),
		m_ulSitState(EN_SIT_NONE),
		m_bMoveStyleChange(false),
		m_bPetDie(false),
		m_lCurAngle(0),
		m_curActionId(EntityAction_Stand),
		m_curRepActionId(EntityAction_Walk),
		m_curActionLoops(-1)
	{
	}
};

class VisualComponentMZ : public VisualComponent, public IAnimationCallback, public IRideCallBack, public IChangePartCallBack
{
	ActionList						mActionList;		///动作列表
	ShadowManager*					mShadowManager;		///冲锋时的影子管理
	CEffectObjHolder				mAttachObjHolder;	///光效吸附对象
	ModifierList					mModifierList;		///修改器列表
	CVisualComponentStateMgr		m_stateMgr;			//生物状态管理器
	RideManager						mRideManager;		//骑乘管理器
	ChangePartManager				mChangePartManager; //换装管理器	
	ModelNodeStateRecorder			mModelNodeState;	//保存节点状态
	JumpManager *					m_pJumpManager;		//跳跃管理器
	bool							m_bUseBigTexture;
public:
	VisualComponentMZ();
	virtual ~VisualComponentMZ();

	virtual void create();
	virtual void close();

	virtual bool handleCommand(ulong cmd, ulong param1, ulong param2);
	virtual void onEnterViewport(int priority = 0);
	virtual void onLeaveViewport();
	virtual void drawPickObject(IRenderSystem* pRenderSystem);
	virtual void drawShadow(IRenderSystem* pRenderSystem);
	virtual void draw(IRenderSystem* pRenderSystem);
	virtual bool update(float tick, float deltaTick, IRenderSystem* pRenderSystem);	
	virtual size_t onLoad(Stream* stream, size_t len);
	virtual size_t onSave(Stream* stream) const;
	virtual void onRemoveEntity();
	virtual void handleMessage(ulong msgId, ulong param1, ulong param2);
	virtual const AABB& GetAABB();
	virtual const Matrix4& GetFullTransform();
	virtual const xs::Rect& getShowRect() const;

public:
	ulong getCurrentAction();

	//by yhc ,在选择角色的时候使用高精度纹理
	//学辉建议的方式是使用ConfigCreature.xml生成一个高精度的生物ID,和另外再用一张表将高精度和非高精度表的ID对应起来,
	//这种方式就是策划需要配置的东西太多,但是代码符合以前的的一些规范
	//现在这种是直接用代码去换纹理,这种方式是比较别扭,但是不用配置任何东西
	void UseBigTexture();

	// 屏蔽周边玩家；
	bool ShieldAroundPlayer();

private:
	void requestRes(int priority);//申请资源
	void releaseRes();//释放资源

private:
	bool setAnimate();//设置动作
	void setAniSpeed(float speed);
	bool setAction(ulong actionId, int loops, ulong replaceactionid = 0, /*是否忽略actionlist的同步*/bool ignoreactionlistsyn = false, bool bForceStopStand = false);
	void setWeaponPos();//设置武器的位置
	void setWeaponAction();//设置武器动作
	bool changePartAttchEffect( SPersonAttachPartChangeContext & context);//处理装备附加效果
	bool deformPart(SPersonPartDeformationContext & context);//处理装备变形
	int CheckAngleDir(long & lFrom,long & lTo);
	inline void resetMoveAction(bool ignoreactionlistsyn = false );
	void GetBoneSpace(Vector3& v, const char *p);

	//回调函数
	virtual void onOneCycle(const std::string& animation,int loopTimes);
	virtual void onRide(RideCallBackContext& context);
	virtual void onChangePart(ChangePartCallBackContext & context);

private:
	inline ModelNode* getModelNode(){ return mModelNodeState.m_pCompositeNode; };//获取模型节点

	// 获取模型动作的精确时间
	int GetAnimationTime(int nActID);

	// 获取位移的时间
	int GetJumpTime(xs::Point ptSrc, xs::Point ptTarget);

//将要被删除的函数
private:
	//void onResLoaded(ModelNode* node);


};

#endif // __VISUALCOMPONENT_H__