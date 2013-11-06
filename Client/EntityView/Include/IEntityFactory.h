//==========================================================================
/**
* @file	  : IEntityFactory.h
* @author : 
* created : 2008-1-19   12:53
* purpose : 实体工厂接口
*/
//==========================================================================

#ifndef __IENTITYFACTORY_H__
#define __IENTITYFACTORY_H__

#include "IEntityView.h"

/// 文字浮动时运动的类型
enum FlowType
{
	FlowType_Liner,			/// 匀速运动
	FlowType_Accelerative,	/// 匀加速运动
	MaxFlowTypeCount,
};

/// 坐标定位类型
enum CoordAlign
{
	CoordAlign_Screen,		/// 以屏幕位置定位
	CoordAlign_World,		/// 以世界坐标定位
	MaxCoordAlignCount,
};

enum FlowAreaType
{
	FlowArea_Top,			/// 角色上方
	FlowArea_Bottom,		/// 角色下方
	FlowArea_Right,			/// 角色右方
	FlowArea_Mouse,			/// 鼠标当前位置
	MaxFlowAreaCount		/// 最大数
};

/// 浮动文字上下文
struct FlowTextContext
{
	struct EFFECTSCALE
	{
		float		firstScale;
		float		finalScale;	
		ulong		effectTime;	/// 动画效果时间，这个时间一般很短，针对字体的放大缩小
	};

	POINT		pos;		/// 初始位置(根据align而填写)
	union{
		POINT		posOffset;	/// 相对初始位置的偏移量
		struct{
			float		v0_x;		/// x方向上的初始速度
			float		v0_y;		/// y方向上的初始速度
			float		a_x;		/// x方向上的加速度
			float		a_y;		/// y方向上的加速度
		};
	};
	std::string	text;		/// 浮动文字
	ColorValue	color;		/// 文字颜色   
	float		lastAlpha;	/// 最终的alpha值
	ulong		totalTime;	/// 文字浮动总时间
	FlowType	flowType;	/// 文字浮动类型，如匀速浮动，抛物线浮动(目前不针对alpha)
	CoordAlign	align;		/// 坐标定位类型

	DWORD		font;		/// 字体
	EFFECTSCALE scale;	
};

class MagicView;
/// 实体工厂接口
struct IEntityFactory
{
	/** 创建实体对象
	@param entityType	实体类型，参考枚举EntityType
	@param resType		资源类型，参考ResourceType枚举
	@param stream		实体创建时的上下文
	@param len			上下文长度
	*/
	virtual EntityView* createEntity(ulong entityType, ulong resType, Stream* stream, size_t len) = 0;

	/** 创建实体对象
	@param entityType	实体类型，参考枚举EntityType
	@param resType		资源类型，参考ResourceType枚举
	@param buf			实体创建时的上下文
	@param len			上下文长度
	*/
	virtual EntityView* createEntity(ulong entityType, ulong resType, const uchar* buf, size_t len) = 0;

	/** 浮动文字
	@param ptScreen		浮动文字开始处的屏幕坐标
	@param text			浮动的文字内容
	@param totalTime	文字浮动的时间周期
	@param lastX		浮动文字最后的x位置（相对于开始点）
	@param lastY		浮动文字最后的y位置（相对于开始点）
	@param flowType		浮动类型（线性:0 抛物线:1）
	*/
	virtual void flowText(const FlowTextContext& context) = 0;


	/** 在指定区域漂浮文字（用于角色旁边的信息显示）
	@param area		指定的区域，参考 FlowArea 枚举定义
	@param text		要显示的文本
	@param color	该文本的颜色
	*/
	virtual void flowAreaText(int area, const std::string& text, const ColorValue& color = ColorValue(1,1,1),const DWORD font = 0) = 0;

	virtual const std::string GetLastFlowString(FlowAreaType area) = 0;

	/** 更新浮动文字
	@param deltaTime	相对上次更新之间的时间差
	*/
	virtual void updateFlowText(ulong deltaTime) = 0;

	/** 更新系统提示信息
	@param deltaTime	相对上次更新之间的时间差
	*/
	virtual void updateSystemTips(ulong deltaTime) = 0;

	/** 渲染浮动文字
	@param pRenderSystem	渲染系统
	*/
	virtual void renderFlowText(IRenderSystem* pRenderSystem) = 0;

	/** 渲染系统提示信息
	@param pRenderSystem	渲染系统
	*/
	virtual void renderSystemTips(IRenderSystem* pRenderSystem) = 0;

	/** 创建魔法对象
	@param type		魔法对象类型，如普通类，跟踪类等等
	@param context	创建上下文
	@param len		上下文长度
	*/
	//virtual bool createEffect(ulong type, const void* context, size_t len) = 0;
	virtual EntityView* createMagic(ulong type, const void* context, size_t len) const = 0;

	//////////////////////////////////////////////////////////////////////////
	// 下面4个接口是对EntityView的扩充，使外面的调用更方便
	/** 创建Buff效果
	@param view		拥有该Buff效果的EntityView对象
	@param effId	是EffectView.xls脚本中定义的Buff效果id
	*/
	virtual bool createBuffEffect(EntityView* view, ulong effId) = 0;
	virtual bool createEffect(ulong effId, const POINT& ptTile) = 0;

	/** 关闭Buff效果
	@param view		拥有该Buff效果的EntityView对象
	@param effId	是EffectView.xls脚本中定义的Buff效果id
	*/
	virtual void closeBuffEffect(EntityView* view, ulong effId) = 0;

	/** 创建技能准备效果
	@param context 是技能准备上下文
	*/
	virtual bool createSkillReadyEffect(const SkillEffectContext& context, bool bPlayAction, bool bHasInterval) = 0;

	/** 关闭技能准备时的光效
	@param view 技能使用者
	@param id	来自于Skill.xls中的视图ID
	*/
	virtual void closeSkillReadyEffect(EntityView* pSourceView, EntityView* pDestView, ulong id) = 0;

    /** 创建技能引导效果
    @param context 是技能准备上下文
    */
    virtual bool CreateSkillBootEffect(const SkillEffectContext& context) = 0;

    /** 关闭技能引导时的光效
    @param view 技能使用者
    @param id	来自于Skill.xls中的视图ID
    */
    virtual void CloseSkillBootEffect(EntityView* pSourceView, ulong ulSkillViewID) = 0;

	/** 创建受击特效
	@param   
	@param   
	@return  
	*/
	virtual bool CreateBeHitEffect(const SkillEffectContext& context) = 0;

	/** 创建受击源到目标飞行特效
	@param   
	@param   
	@return  
	*/
	virtual bool CreateBeHitFlyEffect(const SkillEffectContext& context) = 0;

	/** 创建跟踪特效
	@param   
	@param   
	@return  
	*/
	virtual bool createSkillTrackEffect(const SkillEffectContext& context) = 0;


	/** 创建技能攻击效果
	@param context 是技能攻击上下文
	*/
	virtual bool createSkillAttackEffect(const SkillEffectContext& context) = 0;

	/** 关闭技能攻击时的光效
	@param view		技能使用者
	@param viewId	来自于SkillView.xls中的视图ID
	*/
	virtual void closeSkillAttackEffect(EntityView* view, ulong viewId) = 0;

	// 创建游戏场景播放的特效，例如下雨、下雪，同一个ID的场景特效不能重复存在。 zgz [4/18/2011 zgz]
	virtual MagicView* CreateSceneMagic(const SceneMagicContext& context) = 0;
	// 删除游戏场景播放的特效 
	virtual bool CloseSceneMagic(MagicView* mv) = 0;

	/// 释放实体工厂
	virtual void release() = 0;

	//预加载资源模型数据
	//add by yhc 2009.11.11
	virtual std::string GetFileNameByCreatureId(long id) = 0;
	virtual std::string GetFileNameByResId(long id) = 0;

	// 获取模型动作准确时间
	virtual int GetAnimationTime(const RequestAnimationContext& context) = 0;

	/** 创建振屏效果
	@param   
	@param   
	@return  
	*/
	virtual void CreateVibrate(ulong ulViewID) = 0;

	// 预加载技能特效
	virtual void PreLoadSkillEffect(int nViewID) = 0;

	//获取Skill相关数据 add wangtao
	virtual uint GetSkillViewDataId(uint array, uint set) = 0;
	virtual const std::string& GetSkillViewDataName(uint id) = 0;
	virtual bool GetSkillViewDataArrayIsValid(uint array) = 0;
	virtual bool GetSkillViewDataRowIsValid(uint array, uint set) = 0;
	virtual uint GetMagicDataId(uint array, uint set) = 0;
	virtual const std::string& GetMagicDataName(uint id) = 0;
	virtual bool GetMagicDataArrayIsValid(uint array) = 0;
	virtual bool GetMagicDataRowIsValid(uint array, uint set) = 0;
	virtual const uint GetConfigCreatureResMaxId() = 0;
	virtual const uint GetConfigCreatureResId(uint id) = 0;
	virtual const std::string GetConfigCreatureResName(uint id) = 0;
	virtual const std::string GetSkillViewAttackAct(uint id) = 0;
	virtual const uint GetSkillViewSwing(uint id) = 0;
	virtual const float GetSkillViewFlySpeed(uint id) = 0;
	virtual const uint GetSkillViewEmitEffect(uint id) = 0;
	virtual const uint GetSkillViewBlastEffect(uint id) = 0;
	virtual const uint GetSkillViewHitEffect(uint id) = 0;
	virtual const uint GetSkillViewFlyEffect(uint id) = 0;
	virtual const uint GetSkillViewHitFlyEffect(uint id) = 0;
	virtual const uint GetSkillViewReadyEffect(uint id) = 0;
	virtual const float GetMagicAniSpeed(uint id) = 0;
	virtual const uint GetMagicAniTrans(uint id) = 0;
	virtual const std::string GetMagicAniOffset(uint id) = 0;
	virtual const float GetMagicAniScale(uint id) = 0;
	virtual const uint GetMagicResId(uint id) = 0;
	virtual void SetSkillViewData(uint id, std::string attackAct, uint swing, float flySpeed, 
		uint emitEffect, uint blastEffect, uint hitEffect, uint flyEffect,
		uint hitFlyEffect, uint readyEffect) = 0;
	virtual void SetMagicData(uint id, float aniSpeed, uint aniTrans, std::string aniOffset,
		float aniScale, uint resId) = 0;
	virtual void SaveMagicDataToCsv(const char* szFileName) = 0;
	virtual void SaveSkillViewDataToCsv(const char* szFileName) = 0;
};


#if defined(_LIB) || defined(ENTITYVIEW_STATIC_LIB)/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(xs_cev))
extern "C" IEntityFactory* LoadCEV(void);
#	define	CreateEntityFactoryProc	LoadCEV
#	define  CloseEntityFactory()
#else /// 动态库版本
typedef IEntityFactory* (RKT_CDECL *procCreateEntityFactory)(void);
#	define	CreateEntityFactoryProc	DllApi<procCreateEntityFactory>::load(MAKE_DLL_NAME(xs_cev), "LoadCEV")
#	define  CloseEntityFactory()	DllApi<procCreateEntityFactory>::freelib()
#endif


#endif // __IENTITYFACTORY_H__
