//==========================================================================
/**
* @file	  : EntityDef.h
* @author : 
* created : 2008-1-21   9:58
* purpose : 实体相关定义
*/
//==========================================================================

#ifndef __ENTITYDEF_H__
#define __ENTITYDEF_H__

#include "DGlobalGame.h"	// for UID typedef


/// 实体类型
enum EntityType
{
	typeUnknow = 0,		/// 未知实体
	typeStatic,			/// 静态实体
	typeCage,			/// 笼子（用于编辑器放怪）
	typeWaypoint,		/// 路点（用于编辑器）
	typeSpile,			/// 

	typePerson,			///人物
	typePet,			///宠物
	typeMonster,		///怪物
	typeEffect,			/// 光效
	typeBox,			/// 盒子
	typeBubble,			/// 泡泡 add by zjp

	typeMaxCount,
};


/// 实体的排序等级
enum SortLevel
{
	lvlNoSort			= 0,		/// 不参与排序
	lvlSpecificBlock	= 2,		/// 特殊块
	lvlLowest			= 10,		/// 最低等级
	lvlMidle			= 100,		/// 中间等级
	lvlHight			= 1000,		/// 高等级
	lvlHighest			= 10000		/// 最高等级
};


/// 实体标志
enum EntityFlags
{
	// FlagVisible仅代表view在显示列表中，并不表示该view可见；FlagInScreen一般就表示该View可见
	flagVisible		= 0x1,			/// 可见（在显示列表中）
	flagSelectable	= 0x2,			/// 可选择
	flagSelected	= 0x4,			/// 被选中
	flagSelectedForMapEdit	= 0x8,	/// 被选中（用于地图编辑器）
	flagHighlight	= 0x10,			/// 是否高亮
	flag2D			= 0x20,			/// 2D类型
	flag3D			= 0x40,			/// 3D类型
	flagDrawName	= 0x100,		/// 绘制实体名字
	flagDrawResId	= 0x200,		/// 绘制实体资源Id
	flagDrawHP		= 0x400,		/// 绘制实体HP
	flagDrawMP		= 0x800,		/// 绘制实体MP
	flagInScreen	= 0x1000,		/// 标识对象是否在屏幕范围之内
	flagInMap		= 0x2000,		/// 标识对象是否在地图上
	flagFade		= 0x4000,		/// 是否支持淡入淡出处理         
	flagNoLogic		= 0x8000,		/// 是否是非逻辑对象
	flagDie         = 0x10000,      /// 是否是死亡对象(wan)
	flagDrawTitle	= 0x20000,		/// 称号
	flagDrawTextGroup	= 0x40000,		/// 文字组
	flagDrawTeam		= 0x80000,		/// 小队旗帜
	flagDrawFinishTaskEffect = 0x100000,        /// 特效标识
	flagDrawAutoMoveEffect = 0x200000,         /// 自动寻路特效标识
	flagDrawFinishAutoMoveEffect = 0x400000,   /// 自动寻路结束标识
	flagDrawTaskIcon  = 0x800000,            /// 任务图标
	flagDrawBoxMouseMove = 0x1000000,		/// 鼠标悬停绘制宝箱名
	flagMapItem          = 0x2000000,		/// 地图物件
	flagUnRender		 = 0x4000000,		/// flagVisible无法控制实体是否渲染，所以新增这个flag
	flagUnRenderShadow	 = 0x8000000,		/// 是否绘制阴影
	flagGlobalSceneMagic = 0x10000000,		/// 是否全场景的特效
	flagFallMagic		 = 0x20000000,		/// 是否下落的特效，例如：雨、雪

	///zgz 立即释放，不做延迟处理。原因：原先地图上的实体在切换地图时没有正确释放掉，现在增加此标志使其立即释放避免内存泄露
	/// 请慎用此标志! 其他场合如果需要立即释放，请确认实体指针已从场景管理中清除干净避免客户端崩溃。
	flagReleaseImmediate = 0x40000000,		

	flagDrawStall        = 0x80000000,      /// 摊位标识	
};

#if 0  //谢学辉注释，20090923

/// 实体部件
enum EntityParts
{
	EntityPart_Body,		/// 躯干，身体，裸体，整体(非生物一般只有躯干，如树，房子，地上物品)

	// 下面适合角色和怪物
	EntityPart_Armet,		/// 头盔，帽子，角
	EntityPart_Jacket,		/// 上衣，夹克，衬衣，背心
	EntityPart_Pants,		/// 裤子，短裤
	EntityPart_Shoes,		/// 鞋子，履
	EntityPart_Manteau,		/// 披风，背部，翅膀
	EntityPart_RWeapon,		/// 右手武器，如刀，剑(角色类适用以上各部件)

	// 下面适合怪物
	EntityPart_Sash,		/// 护肩，肩夹
	EntityPart_Girdle,		/// 腰带
	EntityPart_Cuff,		/// 护腕，护手，手套
	EntityPart_Tail,		/// 尾巴
	EntityPart_LRWeapon,	/// 双手武器，如双手锤，双手矛，弓，双手刀
	EntityPart_LWeapon,		/// 左手武器，如盾

	// 保留
	EntityPart_Reserve1,	/// 附件1——增补部件，部件数不够用时，或个别怪物不便分类的特殊部件可使用此类别，下同
	EntityPart_Reserve2,	/// 附件2
	EntityPart_Reserve3,	/// 附件3

	// 还需要增加受击挂接点，脚底光效点，头顶光效点，中间环绕光效点

	EntityPart_Max,
};
#endif


/// 实体命令
enum EntityCommand
{
	// 外观类
	EntityCommand_SetPart,			///设置部件，换装 (ulong part, ulong resId)，如果resId为0表示仅仅取下部件
	EntityCommand_GetPart,			///获取部件,获取换装设置(ulong part,SEventPersonPerform_C* pContext)
	EntityCommand_SetPartScale,		///设置部件的缩放比例，只对模型有效 (ulong part, float scale)
	EntityCommand_SetPartColor,		///设置部件的颜色 (ulong part, const ColorValue* color)
	EntityCommand_ShowPart,			////控制是否显示装备(ulong part, ulong show)
	EntityCommand_GetPartShow,		///获取是否显示装备(ulong part, uchar * pShow)

	EntityCommand_PartDeformation,//装备变形命令
	EntityCommand_PartAttachEffect,//装备附加特效

	EntityCommand_AttachMagic,		/// 在实体(主要指生物)上吸附一个光效 (const MagicView* magic, 0)
    //  增加一个与实体相关的光效，注意：该类光效并没有吸附在实体上；
    EntityCommand_AddMagic,
	EntityCommand_DetachMagic,		/// 取消实体(主要指生物)身上指定的光效 (ulong id, int loops)
    //  移除一个与实体相关的光效，注意：该类光效并没有吸附在实体上；
    EntityCommand_RemoveMagic,
	EntityCommand_ClearMagic,		/// 清除实体身上的所有光效(一般发生在实体死亡时) (0, 0)
	EntityCommand_AddModifier,		/// 添加修改器 (const Modifier* modifier, 0)
	EntityCommand_RemoveModifiers,	/// 移除修改器 (ulong key, 0)

	// 动作类
	EntityCommand_ForceStop,		/// add by zjp强制停止站立，停止引导动作
	EntityCommand_Stand,			/// 站立 (const xs::Point* tile=0, 0)
	EntityCommand_Fallow,			/// 休闲 (ulong fallowId, 0)
	EntityCommand_Move,				/// 移动，不关心是行走还是奔跑 (const xs::Point* pathList, size_t count)
	EntityCommand_RapidMoveStart,	/// 快速移动(瞬移、冲锋)开始 (ShadowManager* mgr, 0)
	EntityCommand_RapidMoveEnd,		/// 快速移动(瞬移、冲锋)结束
	EntityCommand_AttackReady,		/// 攻击准备 (const AttackContext* context, 0)
	EntityCommand_Attack,			/// 物理攻击 (const AttackContext* context, 0)
	//EntityCommand_SpellReady,		/// 法术准备（聚气） (ulong actionId, 0)
	//EntityCommand_Spell,			/// 施法 (ulong actionId, 0)
	EntityCommand_Wound,			/// 受击 (0, 0)
	EntityCommand_Death,			/// 死亡 (0, 0)
	EntityCommand_Stun,				/// 眩晕 (0, 0)
	EntityCommand_Sitdown,			/// 坐下 (0, 0)
	EntityCommand_Siting,			/// 坐 (0, 0)
	EntityCommand_GetSitState,      /// 获取坐的状态
	EntityCommand_Collect,			/// 采集 (0, 0)
	EntityCommand_ShowRibbon,       /// 是否显示武器飘带
	EntityCommand_SetMount,			/// 设置骑（下）马 (ulong mount, ulong actionIndex) mount为1即上马，为0即下马,actionIndex为使用第几套动作，以0为基数
	EntityCommand_GetMount,			/// 获取是否是骑马姿态 (ulong* mount, 0) mount为1即在马上，为0即没有骑马
	EntityCommand_SetDefaultSpeed,  /// 设置默认速度（跑步，走路）     
	EntityCommand_SetMoveStyle,		/// 设置移动类型 (ulong style, 0) style为0即行走，为1即奔跑
	EntityCommand_GetMoveStyle,		/// 获取移动类型 (ulong* style, 0) style为0即行走，为1即奔跑
	EntityCommand_Relive,			/// 实体复活 (0, 0)
	EntityCommand_IsMoving,			/// 实体是否正在移动 (bool* isMoving, 0)
	EntityCommand_ChangeModel,		/// 换模型


	// 主模型控制
	EntityCommand_SetScale,			/// 设置主模型的缩放比例(默认为1) (const float* scale, 0) 第二个参数为0表示不相对当前缩放比进行缩放，为1表示相对当前缩放比进行缩放
	EntityCommand_GetScale,			/// 获取主模型的缩放比例 (float* scale, 0)
	EntityCommand_SetMoveSpeed,		/// 设置主模型的移动速度 (ulong tileMoveTime, 0)，这里的移动速度是移动档次对应的两个Tile中心点的移动时间（毫秒）
	EntityCommand_GetMoveSpeed,		/// 获取主模型的移动速度 (ulong* tileMoveTime, 0)
	//EntityCommand_SetAniSpeed,		/// 设置主模型动画的速度(默认为1) (const float* speed, 0)
	//EntityCommand_GetAniSpeed,		/// 获取主模型动画的速度 (float* speed, 0)
	EntityCommand_SetColor,			/// 设置主模型的颜色 (const ColorValue* color, 0)
	EntityCommand_GetColor,			/// 获取主模型的颜色 (ColorValue* color, 0)

	// Topmost类
	EntityCommand_SetName,			/// 设置名称 (const char* name, size_t len)
	EntityCommand_GetName,			/// 获取名字 (std::string* name, 0)
	EntityCommand_SetNameColor,		/// 设置名称的颜色 (const ColorValue* color, 0)
	EntityCommand_GetNameColor,		/// 获取名称的颜色 (ColorValue* color, 0)
	EntityCommand_SetHPInfo,		/// 设置HP信息 (ulong cur, ulong max)
	EntityCommand_SetMPInfo,		/// 设置MP信息 (ulong cur, ulong max)
	EntityCommand_SetSpInfo,		/// 设置SP体力信息(ulong cur, ulong max)
	EntityCommand_ChangeLayout,		/// 改变topmost信息中的显示和不显示的标识(ulong hideFlags, ulong showFlags)，参考TopmostFlags枚举
	EntityCommand_SetTitle,			/// 设置称号
	EntityCommand_AddGroupText,		/// 添加文字显示组
	EntityCommand_ClearGroupText,	/// 清空文字显示组

    //  设置任务标记；
    EntityCommand_SetTaskSign,

	// 光效类
	EntityCommand_MoveRadial,		/// 射线移动，用于光效 (const xs::Point* ptBegin, const xs::Point* ptEnd)
	EntityCommand_MoveTrack,		/// 移动并跟踪，用于光效，如火球 (IEntity* source, IEntity* target)
	EntityCommand_Jump,				/// 跳跃
	EntityCommand_NextMovingTile,
	EntityCommand_GetAnimationTime, //  获取实体的动作时间
	EntityCommand_GetJumpTime,		//  获取实体的位移时间
	EntityCommand_UseBigTexture,
	EntityCommand_GetBoneSpace,		// add by zjp;获取骨骼的space坐标 
	EntityCommand_Max,
};

// 坐
enum EN_SIT
{
	EN_SIT_NONE = 0,                // 没有坐下
	EN_SIT_ACTION,                  // 正在坐
	EN_SIT_DONE,                    // 已坐下
};

/// 魔法对象的类型
enum MagicType
{
	MagicType_General,		/// 通用魔法
	MagicType_MoveRadial,	/// 射线移动魔法
	MagicType_MoveTrack,	/// 跟踪魔法
};

/// 光效控制器类型
enum EffectType
{
	EffectType_General,		/// 通用动画效果
	EffectType_Fly,			/// 飞行类动画效果（追踪特效）
	EffectType_LifeTime,	/// 生命期类动画效果（这个暂时没用到）
	EffectType_MultiTimes,	/// 多次类动画效果（这个暂时没用到）
	EffectType_Area,		/// 区域类动画效果（比如法师全体落雷）
	EffectType_MultiDir,	/// 多方向类动画效果（这个暂时没用到）
	EffectType_RapidMove,	/// 瞬移类动画效果（比如冲锋）
	EffectType_DestRapidMove,	/// 目标瞬移类动画效果（比如捆仙锁）
	EffectType_SrcDestRapidMove, /// 源和目标瞬移类动画效果（比如倒转乾坤）
	EffectType_Jump,		//跳跃类动画效果 add by yhc
};



#pragma pack(push)
#pragma pack(1)
/// 攻击命令上下文
struct AttackContext
{
	ulong actionId;			/// 动作Id
	ulong loops;			/// 动画循环次数
    //  动画速度；
    float fAnimateSpeed;
	bool  isSpellAttack;	/// 是否是法术攻击
};

/// 创建技能效果的上下文
struct SkillEffectContext
{
	ulong			id;				/// 效果Id
	handle			src;			/// 技能效果发起者
	handle			target;			/// 技能效果承受者，可以为 NULL
    xs::Point			ptCenter;		/// 技能效果中心点(施放位置)，对于瞬移类效果，是指瞬移目标瞬移后到达的位置；
	xs::Point			ptTarget;		/// 目标点
	int				loops;			/// 光效循环次数
    //  记录光效的持续时间；
    int nDuration;
	bool			isSpellSkill;	/// 是否是法术技能
	int				selectType;		/// 技能选择类型(0-不选择 1-选择实体 2-选择Tile)
};

/// 创建全场景特效的上下文
struct SceneMagicContext
{
	ulong magicID;
	int   nFade;
	int   nFall;
};

// 查询动作时间的上下文
struct RequestAnimationContext
{
	ulong		    id;
	handle			src;
};

#pragma pack(pop)

//模型子模型命名
enum ModelSubMeshForChangePart
{
	MSFCP_BODY = 0,
	MSFCP_FACE = 1,
	MSFCP_HAIR = 2,
	MSFCP_MAX = 3,
};

//动作映射需要的上下文

//可视化组件的状态，用于动作控制
enum EVisualComponentState 
{
	EVCS_OnPeace=0,//非战斗状态
	EVCS_OnFight,//战斗状态
	EVCS_Max,//
};

//动作映射上下文
struct ActionMapContext
{
	EVisualComponentState state;//状态
	uchar weapon;//武器类型,对应EWeaponClass
	bool iscreature;//是否人物
	bool ismount;//是否骑乘
};

// 头顶文字
struct TextLine
{
	DWORD	   txFont;				// 字体		
	ColorValue txColor;				// 文字颜色
	autostring txStr;				// 文字内容
	std::string     txPicture;		// 图片
	DWORD	   txEffectID;			// 特效ID

	TextLine():txFont(0),txEffectID(0){}
};

// 武器飘带
enum EShowRibbon
{
	EShowRibbon_Stop,   // 停止播放武器飘带
	EShowRibbon_Start,   // 开始播放武器飘带
};
#endif // __ENTITYDEF_H__