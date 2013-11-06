/*******************************************************************
** 文件名:	e:\Rocket\Common\DGlobalMisc.h
** 版  权:	(C) 
** 
** 日  期:	2008/2/26  21:02
** 版  本:	1.0
** 描  述:	全局比较杂的东西
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "DEntityProp.h"

#pragma pack(1)

/////////////////////////// 效果组定义  ////////////////////////////
enum
{
	EEffectGroupID_NULL            = 0,

	/******************zhangjie new******************/
	// 增加HP上限
	EN_EFFECTGROUPID_BASE_MAX_HP,// 增加人物自身最大生命值
	EN_EFFECTGROUPID_BASE_MAX_HP_RATIO,// 增加人物自身最大生命比值（万分比）
	EN_EFFECTGROUPID_SUM_MAX_HP_RATIO,// 增加人物当前的最大生命比值（万分比）

	// MP上限
	EN_EFFECTGROUPID_BASE_MAX_MP,// 增加人物自身最大内力值
	EN_EFFECTGROUPID_BASE_MAX_MP_RATIO,// 增加人物自身最大内力比值（万分比）
	EN_EFFECTGROUPID_SUM_MAX_MP_RATIO,// 增加人物当前的最大内力比值（万分比）

	// 最小物理攻击
	EN_EFFECTGROUPID_WEAPON_MIN_P_A_RATIO,// 增加武器的物理攻击最小比值（万分比）
	EN_EFFECTGROUPID_BASE_MIN_P_A,// 增加自身的物理攻击最小值
	EN_EFFECTGROUPID_BASE_MIN_P_A_RATIO,// 增加自身的物理攻击最小比值（万分比）
	EN_EFFECTGROUPID_SUM_MIN_P_A_RATIO,// 增加当前的物理攻击最小比值（万分比）

	// 最大物理攻击
	EN_EFFECTGROUPID_WEAPON_MAX_P_A_RATIO,// 增加武器的物理攻击最大比值（万分比）
	EN_EFFECTGROUPID_BASE_MAX_P_A,// 增加自身的物理攻击最大值
	EN_EFFECTGROUPID_BASE_MAX_P_A_RATIO,// 增加自身的物理攻击最大比值（万分比）
	EN_EFFECTGROUPID_SUM_MAX_P_A_RATIO,// 增加当前的物理攻击最大比值（万分比）

	// 物理攻击(保留)
	EN_EFFECTGROUPID_WEAPON_P_A_RATIO,// 增加武器的物理攻击比值（万分比），最大最小同时增加
	EN_EFFECTGROUPID_BASE_P_A,// 增加自身的物理攻击值，最大最小同时增加
	EN_EFFECTGROUPID_BASE_P_A_RATIO,// 增加自身的物理攻击比值（万分比），最大最小同时增加
	EN_EFFECTGROUPID_SUM_P_A_RATIO,// 增加当前的物理攻击最大比值（万分比），最大最小同时增加

	// 最小法术攻击
	EN_EFFECTGROUPID_WEAPON_MIN_M_A_RATIO,// 增加武器的法术攻击最小比值（万分比）
	EN_EFFECTGROUPID_BASE_MIN_M_A,// 增加自身的法术攻击最小值
	EN_EFFECTGROUPID_BASE_MIN_M_A_RATIO,// 增加自身的法术攻击最小比值（万分比）
	EN_EFFECTGROUPID_SUM_MIN_M_A_RATIO,// 增加当前的法术攻击最小比值（万分比）

	// 最大法术攻击
	EN_EFFECTGROUPID_WEAPON_MAX_M_A_RATIO,// 增加武器的法术攻击最大比值（万分比）
	EN_EFFECTGROUPID_BASE_MAX_M_A,// 增加自身的法术攻击最大值
	EN_EFFECTGROUPID_BASE_MAX_M_A_RATIO,// 增加自身的法术攻击最大比值（万分比）
	EN_EFFECTGROUPID_SUM_MAX_M_A_RATIO,// 增加当前的法术攻击最大比值（万分比）

	// 法术攻击(保留)
	EN_EFFECTGROUPID_WEAPON_M_A_RATIO,// 增加武器的法术攻击比值（万分比），最大最小同时增加
	EN_EFFECTGROUPID_BASE_M_A,// 增加自身的法术攻击值，最大最小同时增加
	EN_EFFECTGROUPID_BASE_M_A_RATIO,// 增加自身的法术攻击比值（万分比），最大最小同时增加
	EN_EFFECTGROUPID_SUM_M_A_RATIO,// 增加当前的法术攻击比值（万分比，最大最小同时增加）

	// 物理防御值 
	EN_EFFECTGROUPID_BASE_P_D,// 增加自身的物理防御值
	EN_EFFECTGROUPID_BASE_P_D_RATIO,// 增加自身的物理防御比值（万分比）
	EN_EFFECTGROUPID_SUM_P_D_RATIO,// 增加当前的物理防御比值（万分比）

	// 法术防御值
	EN_EFFECTGROUPID_BASE_M_D,// 增加自身的法术防御值
	EN_EFFECTGROUPID_BASE_M_D_RATIO,// 增加自身的法术防御比值（万分比）
	EN_EFFECTGROUPID_SUM_M_D_RATIO,// 增加当前的法术防御比值（万分比）

	// 命中
	EN_EFFECTGROUPID_PRESENT,// 增加命中值
	EN_EFFECTGROUPID_PRESENT_RATIO,// 增加命中率

	// 闪避
	EN_EFFECTGROUPID_HEDGE,// 增加闪避值
	EN_EFFECTGROUPID_HEDGE_RATIO,// 增加闪避率

	// 会心
	EN_EFFECTGROUPID_FATAL,// 增加会心值
	EN_EFFECTGROUPID_FATAL_RATIO,// 增加会心率

	// 御劲
	EN_EFFECTGROUPID_KICK_FATAL,// 增加御劲值

	// 移动速度
	EN_EFFECTGROUPID_MOVE_SPEED,// 移动速度
	EN_EFFECTGROUPID_MOVE_SPEED_RATIO,// 移动速度比值（万分比）

	// 攻击速度 
	EN_EFFECTGROUPID_ATTACK_SPEED,// 攻击速度值
	EN_EFFECTGROUPID_ATTACK_SPEED_RATIO,// 攻击速度比值（万分比）

	// 生命回复 
	EN_EFFECTGROUPID_HP_RECOVER,// 增加生命回复速度值
	EN_EFFECTGROUPID_HP_RECOVER_RATIO,// 增加生命回复速度比值（万分比）

	// 内力回复
	EN_EFFECTGROUPID_MP_RECOVER,// 增加内力回复速度值
	EN_EFFECTGROUPID_MP_RECOVER_RATIO,// 增加内力回复速度比值（万分比）

	// 穿刺攻击
	EN_EFFECTGROUPID_PENETRATE,// 穿刺（绝对)伤害值

	// 抵抗穿刺
	EN_EFFECTGROUPID_KICK_PENETRATE,// 抵抗穿刺（抵抗绝对)伤害值
	EN_EFFECTGROUPID_KICK_PENETRATE_RATIO,// 抵抗穿刺（抵抗绝对)伤害比值（万分比）

	// 会心伤害
	EN_EFFECTGROUPID_FATAL_DAMAGE_RATIO,// 增加会心伤害比值（万分比)

	// 会心伤害减免
	EN_EFFECTGROUPID_KICK_FATAL_DAMAGE_RATIO,// 抵抗会心伤害比值（万分比)

	// 忽略物理防御
	EN_EFFECTGROUPID_IGNORE_P_D,// 忽略物理防御值
	EN_EFFECTGROUPID_IGNORE_P_D_RATIO,// 忽略物理防御比值（万分比）

	// 忽略法术防御
	EN_EFFECTGROUPID_IGNORE_M_D,// 忽略法术防御值
	EN_EFFECTGROUPID_IGNORE_M_D_RATIO,// 忽略法术防御比值（万分比）

	// 物理伤害减免
	EN_EFFECTGROUPID_REDUCE_P_DAMAGE,// 物理伤害减免值
	EN_EFFECTGROUPID_REDUCE_P_DAMAGE_RATIO,// 物理伤害减免比值（万分比）

	// 法术伤害减免
	EN_EFFECTGROUPID_REDUCE_M_DAMAGE,// 法术伤害减免值
	EN_EFFECTGROUPID_REDUCE_M_DAMAGE_RATIO,// 法术伤害减免比值（万分比）

	// old
	EEffectGroupID_Unable_Move,						// 无法移动
	EEffectGroupID_Unable_Attack,					// 无法攻击
	EEffectGroupID_Unable_UseGoods,					// 无法使用物品
	EEffectGroupID_Unable_UseSkill,					// 禁魔

	EEffectGroupID_Immunity_Damage,					// 免疫任何伤害
	EEffectGroupID_Immunity_BreakPreSkill,			// 防止施法打断	

	EEffectGroupID_FuriousMove,						// 疯跑
	EEffectGroupID_BreakSkill,						// 打断当前施法

	EEffectGroupID_Immunity_Buff,					// 免疫某种类型的BUFF
	EEffectGroupID_Clean_HurtBuff,					// 清除有害BUFF	
	EEffectGroupID_ImposeOn_Buff,					// 产生某种类型的BUFF	

	/*---------------------------增加伤害比值--------------------------------*/
	EN_EFFECTGROUPID_INCREASE_P_DAMAGE_RATIO,       //物理伤害增加比值（万分比）
	EN_EFFECTGROUPID_INCREASE_M_DAMAGE_RATIO,       // 法术伤害增加比值（万分比）

	/*---------------------------增加所有的技能MP消耗比值--------------------------------*/
	EN_EFFECTGROUPID_INCREASE_SKILL_CONSUMEMP_RATIO,       // 增加所有的技能MP消耗比值

	/*---------------------------增加玩家身上聚集的灵气球的数目--------------------------------*/
	EEffectGroupID_Magic_Balloon_Num,               // 玩家身上聚集的灵气球的数目

	/*---------------------------护盾--------------------------------*/
	EEffectGroupID_PM_Shield_Absorb_Remain,         // 护盾吸收伤害值增加，由护盾吸收相当于自身法力值上限X%的伤害计算得到

	/*---------------------------持续自动增加X--------------------------------*/
	EEffectGroupID_Durative_AddHP,					// 持续自动增加n点生命
	EEffectGroupID_Durative_AddMP,					// 持续自动增加n点法术
	EEffectGroupID_Durative_AddHP_Per,				// 持续自动增加％生命
	EEffectGroupID_Durative_AddMP_Per,				// 持续自动增加％法术
	EEffectGroupID_Durative_AddExp,					// 持续自动增加n点经验
	EEffectGroupID_Durative_AddMPAndHP_Per,         // 持续自动增加％法术，同时增加HP相对于本次增加MP值％

	/*---------------------------普通攻击附带额外攻击力--------------------------------*/
	EEffectGroupID_Plus_Normal_Attack_Per,          // 普通攻击附带额外攻击力百分比
	EEffectGroupID_Normal_Attack_ConsumeMP_Per,     // 每次普通攻击都会消耗法力值的百分比

	/*---------------------------反弹--------------------------------*/
	EN_EFFECTGROUPID_DEST_DAMAGE2SRC_PER,           // 有X%的几率将伤害的100%反射给攻击者（玩家自己不会受到当次伤害）
	EN_EFFECTGROUPID_REBOUND_DAMAGE_RATIO,          // 技能和BUFF（包括常驻被动技能）带来的反弹伤害百分比

	EEffectGroupID_RandomAttack,					// add by zjp.随机攻击伤害

	/*---------------------------持续自动伤害类--------------------------------*/
	EN_EFFECTGROUPID_PATTACK_PDAMAGE,               // 取物理攻击力，持续自动物理伤害
	EN_EFFECTGROUPID_MATTACK_MDAMAGE,               // 取法术攻击力，持续自动法术伤害
	EN_EFFECTGROUPID_PATTACK_MDAMAGE,               // 取物理攻击力，持续自动法术伤害
	EN_EFFECTGROUPID_MATTACK_PDAMAGE,               // 取法术攻击力，持续自动物理伤害
	EN_EFFECTGROUPID_HPATTACK_PDAMAGE,              // 取相当于施法者当前生命值上限，持续自动法术伤害

	/*--------------------用攻击方或被击方某个属性去改变被击方的某个属性-----------------------*/
	EN_EFFECTGROUPID_ADD_MaxPA_SD,                  // 目标实际最大物理攻击增加一定数值，该数值等于攻击方自身基础最大法术攻击的X%
	EN_EFFECTGROUPID_ADD_MinPA_SD,                  // 目标实际最小物理攻击增加一定数值，该数值等于攻击方自身基础最大法术攻击的X%
	EN_EFFECTGROUPID_ADD_MaxMA_SD,                  // 目标实际最大法术攻击增加一定数值，该数值等于攻击方自身基础最大法术攻击的X%
	EN_EFFECTGROUPID_ADD_MinMA_SD,                  // 目标实际最小法术攻击增加一定数值，该数值等于攻击方自身基础最大法术攻击的X%
	EN_EFFECTGROUPID_ADD_PD_MD_SD,                  // 目标实际物理防御增加一定数值，该数值等于攻击方自身基础法术防御的X%。
	EN_EFFECTGROUPID_ADD_MD_MD_SD,                  // 目标实际法术防御增加一定数值，该数值等于攻击方自身基础法术防御的X%。

	/*-------------------------免疫某个ID的BUFF-------------------------------------------*/
	EEffectGroupID_Immunity_BuffID,					// 免疫某个ID的BUFF
	EEffectGroupID_Immunity_BeCrazyDamage,          // 免疫被暴击

	// 定时的AOE
	EEffectGoupID_AoeDamage,						// 定时的AOE

	EEffectGoupID_KICKFATALRATIO,					// 改变御劲率

	EEffectGoupID_ChangeMoveSpeedByTime,			// 递减的改变移动速度

	EEffectGroupID_Bubble,							// XP泡泡

	EEffectGroupID_MaxID,							// 最大
};

/////////////////////////// 冷却组定义  ////////////////////////////
enum
{
	EFreeze_ClassID_Skill = 1,						// 技能冷却组
	EFreeze_ClassID_Goods,							// 物品冷却组
	EFreeze_ClassID_Chat,							// 聊天冷却组
	EFreeze_ClassID_Task,							// 任务冷却组

	EFreeze_ClassID_Max,							// 最大
};

/////////////////////////// 物品篮类型  ////////////////////////////
enum
{
	ESkepType_Unknow = 0,							// 未知物品篮
	ESkepType_Real,									// 实在物品篮
	ESkeyType_Link,									// 链接物品篮
	ESkeyType_Pet,									// 宠物蓝
};

/////////////////////////// 物品篮权限  ////////////////////////////
enum
{
	/*
					  00000000 00000001				// 链接权限
					  00000000 00000010				// 添加权限
					  00000000 00000100				// 移除权限
					  00000000 00001000				// 使用权限
					  00000000 00010000				// 拆分权限
					  00000000 00100000				// 合并权限
					  00000000 01000000				// 观察权限
	11111111 11111111 11111111 11111111				// .....	
	//*/
	ESkepPopedom_Link = 0x0001,						// 链接权限
	ESkepPopedom_Add = 0x0002,						// 添加权限
	ESkepPopedom_Remove = 0x0004,					// 移除权限
	ESkepPopedom_Use = 0x0008,						// 使用权限
	ESkepPopedom_Split = 0x0010,					// 拆分权限
	ESkepPopedom_Unite = 0x0020,					// 合并权限
	ESkepPopedom_Observe = 0x0040,					// 观察权限
};

/////////////////////////// 物品篮名字  ////////////////////////////
enum
{
	ESkepName_UnKnow = 0,							// 未知的
	ESkepName_Packet,								// 包裹篮	
	ESkepName_WareHouse,							// 仓库篮
	ESkepName_Equip,								// 装备篮
	ESkepName_Trade,								// 交易篮	
	ESkepName_Stall,								// 摊位篮
	ESkepName_Shop,									// 商店篮
	ESkepName_Auction,								// 拍卖篮
	ESkepName_Hock,									// 寄售篮
	ESkepName_Box,									// 宝箱篮
	ESkepName_Pets,									// 宠物篮
	ESkepName_PetLib,                               // 宠物仓库篮
	ESkepName_EquipSmeltSoul,						// 装备附魂或魂魄提纯物品篮
	ESkepName_EquipEmbedGems,						// 装备灵石镶嵌篮
	ESkepName_EquipRebuild,							// 装备重铸篮
	ESkepName_Compose,								//内丹合成	by ldy
	ESkepName_Intensify,							//魂将圣化 by ldy
	ESKepName_Max,									// 篮的上界
};

//////////////////////// 怪物归属策略  ///////////////////////////
enum
{
	EMonster_HoldStrategy_Simple = 1,				// 简单策略
	EMonster_HoldStrategy_LastAttack,				// 最后一次攻击权
	EMonster_HoldStrategy_DurativeAttack,			// 持续攻击权
	EMonster_HoldStrategy_Born,						// 出生归属
	EMonster_HoldStrategy_System,					// 系统归属
};

////////////////////////// ＰＫ模式 ///////////////////////////////
enum
{
	EPKMode_Name_Safe = 0,								// 安全模式
	//EPKMode_Name_Blind,								// 全体模式
	//EPKMode_Name_Team,								// 队伍模式
	//EPKMode_Name_Clan,								// 氏族模式
	//EPKMode_Name_Nation,								// 国家模式
	//EPKMode_Name_Justice,								// 善恶模式
	//EPKMode_Name_Camp,								// 阵营模式
	EPKMode_Name_Good,									//善人模式
	EPKMode_Name_Bad,									//恶人模式 

	EPKMode_Name_Max,									// .......
};

////////////////////////// ＰＫ区域 ///////////////////////////////
enum
{
	EPK_AreaName_General = 0,						// 普通区
	EPK_AreaName_Arena,								// 切磋区
	EPK_AreaName_Safe,								// 安全区	
	EPK_AreaName_Risky,								// 危险区

	EPK_AreaName_Stall = 20,						// 摆摊区
};

enum
{
	EPK_Setting_NotDecide = 0,							//由之后的规则(如pk模式)决定
	EPK_Setting_CanAttack,								//可以pk
	EPK_Setting_BanAttack,								//禁止pk
};

////////////////////////// 玩家名字颜色 ///////////////////////////
enum
{
	EPK_NameColor_Blue = 0,							// 蓝名
	EPK_NameColor_White,							// 白名	
	EPK_NameColor_Yellow,							// 黄名
	EPK_NameColor_Red,								// 红名

	EPK_NameColor_Max,								// ...
};

////////////////////////// 死亡惩罚原因 //////////////////////////
enum
{
	EDiePunish_Type_Absolve = 0,					// 死亡不掉装
	EDiePunish_Type_AreaGeneral,					// 普通区死亡
	EDiePunish_Type_AreaRisky,						// 危险区死亡

	EDiePunish_Type_Max,							// ....
};

// add by zjp；
////////////////////////// 死亡方式 //////////////////////////
enum
{
	EDead_Mode_NORMAL = 0,							// 常规死亡，被npc所杀
	EDead_Mode_PK,									// PK死亡

	EDEAD_MODE_MAX,									// ....
};


////////////////////////// 组队分配模式 //////////////////////////
enum
{
	ETeamPrize_AllotMode_Turn = 0,					// 轮流拾取
	ETeamPrize_AllotMode_Free,						// 自由拾取
	ETeamPrize_AllotMode_Captain,					// 队长拾取
	ETeamPrize_AllotMode_Rand,                      // 随机拾取

	ETeamPrize_Allot_Max,							// .....
};

/////////////////////////// 玩家外形数据 ////////////////////////
enum
{
	tVisible_Reserve = 0x0001,	//保留，无效值
	tVisible_Weapon = 0x0002,	//控制武器显示
	tVisible_Armet = 0x0004,
	tVisible_Armor = 0x0008,	
	tVisible_Cuff = 0x0010,
	tVisible_Shoes = 0x0020,		
	tVisible_Necklace = 0x0040,	
	tVisible_Ring = 0x0080,
	tVisible_DecorationL = 0x0100,
	tVisible_DecorationR = 0x0200,
	tVisible_SuitArmet = 0x0400,
	tVisible_SuitArmor = 0x0800,
};
struct SPersonFormData
{
	DWORD	GoodsID[PERSON_EQUIPPLACE_MAX];			// 物品ID	
	BYTE	ExtendedReason[PERSON_EQUIPPLACE_MAX];	// 扩展外形原因	
	WORD	VisibleFlag;//控制装备的显示,这个flag只是对头盔（包括时装头盔)和衣甲（包括时装衣甲)有效

	SPersonFormData(void)
	{
		memset(this, 0, sizeof(SPersonFormData));
	}
};

/////////////////////////// 系统错误码 ////////////////////////////
enum
{
	ESystem_Error_LoginError = 0,					// 登录错误
	ESystem_Error_DuplicateLogin,				// 重复登陆 
	ESystem_Error_ZoneSelectError,					// 选择人物错误
	ESystem_Error_ZoneInvalidData,					// 非法数据
	ESystem_Error_ZoneChangeZone,					// 切换场景
	ESystem_Error_CloseZoneServer,                  // 场景服关闭
};

//////////////////////////装备炼魂篮子篮名///////////////////////////
enum
{
	EEquipSmeltName_SoulStoneUpgrade = 0,//魂石合成
	EEquipSmeltName_SmeltSoul, //装备炼魂
	EEquipSmeltName_Max,
};

//////////////////////////装备炼魂篮位置名称///////////////////////////
enum
{
	EEquipSmeltSoulSkepPlace_SoulStoneUpgradePlaceStart = 0, //魂石合成的起始位置
	EEquipSmeltSoulSkepPlace_SoulStoneUpgradePlaceEnd = 8,   //魂石合成的结束位置
	EEquipSmeltSoulSkepPlace_UpgradeMainMtlStart = 0,		//魂石升级起始位置
	EEquipSmeltSoulSkepPlace_UpgradeMainMtlEnd = 5,          //魂石升级结束位置
	EEquipSmeltSoulSkepPlace_UpgradeMainMtl0 = 0,				 
	EEquipSmeltSoulSkepPlace_UpgradeMainMtl1 = 1,				 
	EEquipSmeltSoulSkepPlace_UpgradeMainMtl2 = 2,
	EEquipSmeltSoulSkepPlace_UpgradeMainMtl3 = 3,
	EEquipSmeltSoulSkepPlace_UpgradeMainMtl4 = 4,
	EEquipSmeltSoulSkepPlace_UpgradeSupMtl0 = 5,
	EEquipSmeltSoulSkepPlace_UpgradeSupMtl1 = 6,
	EEquipSmeltSoulSkepPlace_UpgradeSupMtl2 = 7,

	EEquipSmeltSoulSkepPlace_EquipSmeltSoulPlaceStart = 1,   //装备炼魂的起始位置
	EEquipSmeltSoulSkepPlace_EquipSmeltSoulPlaceEnd = 5,	// 装备炼魂的结束位置
	EEquipSmeltSoulSkepPlace_SmeltEquipment = 8,
	EEquipSmeltSoulSkepPlace_SmeltMainMtl0 = 9,
	EEquipSmeltSoulSkepPlace_SmeltMainMtl1 = 10,
	EEquipSmeltSoulSkepPlace_SmeltMainMtl2 = 11,
	EEquipSmeltSoulSkepPlace_SmeltMainMtl3 = 12,
	EEquipSmeltSoulSkepPlace_SmeltSupMtl0 = 13,
	EEquipSmeltSoulSkepPlace_SmeltSupMtl1 = 14,
	EEquipSmeltSoulSkepPlace_SmeltSupMtl2 = 15,
};

////////////////////////装备灵石镶嵌篮子篮名//////////////////////////////////
enum
{
	EEquipEmbedGemsName_GemDrillSlot = 0,    //装备钻孔
	EEquipEmbedGemsName_GemUpgrade,   //宝石合成
	EEquipEmbedGemsName_EmbedGems,   //装备镶嵌宝石
	EEquipEmbedGemsName_DetachGems,  //装备去除镶嵌的宝石
	EEquipEmbedGemsName_Max,
};

//////////////////////////装备灵石镶嵌篮位置名称///////////////////////////
enum
{
	EEquipEmbedGemsSkepPlace_EquipDrillSlotPlaceStart = 0, //装备凿孔子篮开始位置
	EEquipEmbedGemsSkepPlace_EquipDrillSlotPlaceEnd   = 2, //装备凿孔子篮结束位置
	EEquipEmbedGemsSkepPlace_EquipDrillSlot = 0, //凿孔的位置
	EEquipEmbedGemsSkepPlace_DrillSlotMtl = 1, //凿孔材料位置

	EEquipEmbedGemsSkepPlace_GemsUpgradePlaceStart = 2, //灵石融合子篮开始位置
	EEquipEmbedGemsSkepPlace_GemsUpgradePlaceEnd   = 11, //灵石融合子篮结束位置
	EEquipEmbedGemsSkepPlace_GemsFusionAgent = 2, //灵石融合剂
	EEquipEmbedGemsSkepPlace_FusionGem0	=3,       //待融合的灵石
	EEquipEmbedGemsSkepPlace_FusionGem1	=4,
	EEquipEmbedGemsSkepPlace_FusionGem2	=5,
	EEquipEmbedGemsSkepPlace_FusionGem3	=6,
	EEquipEmbedGemsSkepPlace_FusionGem4	=7,
	EEquipEmbedGemsSkepPlace_FusionSupMtl0	=8,    //宝石融合辅助材料
	EEquipEmbedGemsSkepPlace_FusionSupMtl1  =9,
	EEquipEmbedGemsSkepPlace_FusionSupMtl2  =10,

	EEquipEmbedGemsSkepPlace_EmbedGemsPlaceStart = 11 , //装备镶嵌灵石子篮起始位置
	EEquipEmbedGemsSkepPlace_EmbedGemsPlaceEnd  =13,     //装备镶嵌灵石子篮结束位置
	EEquipEmbedGemsSkepPlace_EquipEmbedGems = 11,  //镶嵌宝石的装备
	EEquipEmbedGemsSkepPlace_GemsEmbed      = 12,  //嵌入的灵石

	EEquipEmbedGemsSkepPlace_DetachGemsPlaceStart = 13 , //装备去除灵石子篮起始位置
	EEquipEmbedGemsSkepPlace_DetachGemsPlaceEnd  =15,     //装备去除灵石子篮结束位置
	EEquipEmbedGemsSkepPlace_EquipDetachGems =13,  //除灵的装备
	EEquipEmbedGemsSkepPlace_DetachGemsMtl   =14,  //除灵的材料
};

///////////////////////装备重铸篮子篮名/////////////////////////////////////////
enum
{
	EEquipRebuildName_Rebuild = 0,   //装备重铸
	EEquipRebuildName_Max,
};

//////////////////////////装备重铸篮名位置名称///////////////////////////
enum
{
	EEquipRebuildSkepPlace_RebuildPlaceStart = 0, //装备重铸子篮开始位置
	EEquipRebuildSkepPlace_RebuildPlaceEnd   = 3, //装备重铸子篮结束位置 modify by zjp 现在的栏子大小为3

	EEquipRebuildSkepPlace_RebuildEquip		= 0, //重铸的装备

	EEquipRebuildSkepPlace_RebuildMainMtl0  = 1, //主要的重铸材料
	EEquipRebuildSkepPlace_RebuildMainMtl1  = 2,
	EEquipRebuildSkepPlace_RebuildMainMtl2  = 3,

	EEquipRebuildSkepPlace_RebuildSupMtl0 = 4,   //辅助的重铸材料
	EEquipRebuildSkepPlace_RebuildSupMtl1 = 5, 

	EEquipRebuildSkepPlace_AttribReserveMtl0 = 6,//保留品质的材料
	EEquipRebuildSkepPlace_AttribReserveMtl1 = 7,
	EEquipRebuildSkepPlace_AttribReserveMtl2 = 8,
};

///////////////////////////////合成栏/////////////////////////////////
enum ComposeSkep
{
	ComposeName_Confirm = 0,					//合成确认
	ComposeName_Max,
};

enum 
{
	ComposeSkepPlace_Start = 0,
	ComposeSkepPlace_End = 16,
};

///////////////////////////////魂将圣化栏/////////////////////////////////
enum
{
	PetIntensifySkepPlace_PillStart = 0,			//内丹子栏开始
	PetIntensifySkepPlace_PillEnd = 15,			//内丹子栏结束
	PetIntensifySkepPlace_Pet = 16,				//魂将子栏

};

enum 
{
	PetIntensifyConfirm = 0,				//魂将圣化确认
	PetIntensifyOp_Max,
};

///////////////////////////////技能相关/////////////////////////////////
enum ESKILLPOINT_CONSUME
{
	SKILLPOINT_CONSUME_BRANCH_FIRST = 1,	        // 第一个技能分支上消耗的技能点
	SKILLPOINT_CONSUME_BRANCH_SECOND,	            // 第二个技能分支上消耗的技能点
};

///////////////////////////////移动相关/////////////////////////////////
// 表示移动一个tile所花费的时间（毫秒），值越小表示移动速度越快，修改以前的速度等级制是因为策划要求将速度量化，在技能使用时可以提高速度百分比
enum EMOVETILECOSTTIME
{
	EMOVETILECOSTTIME_MIN            = 1,           // 生物移动表示移动一个tile所花费的时间最小值
	EMOVETILECOSTTIME_PERSON_INIT    = 200,	        // 人物初始基本速度
	EMOVETILECOSTTIME_MONSTER_INIT   = 260,	        // 怪物初始基本速度
	EMOVETILECOSTTIME_PET_INIT       = 260,	        // 宠物初始基本速度
	EMOVETILECOSTTIME_PERSON_WALK    = 530,	        // 生物走路时的速度
	EMOVETILECOSTTIME_MONSTER_WALK   = 530,	        // 怪物走路时的速度
	EMOVETILECOSTTIME_PET_WALK       = 530,	        // 宠物走路时的速度
	EMOVETILECOSTTIME_CREATURE_MAX   = 50000,       // 可以移动时最慢速度
};

// 表示移动一个tile所花费的时间（毫秒），值越小表示移动速度越快
enum EMOVETIMEPERTILE
{
	EMOVETIMEPERTILE_MIN       = 1,                 // 移动一个tile所花费的时间最小值
	EMOVETIMEPERTILE_NORMAL    = 200,	            // 人物默认的跨格时间(用此时间做标准与移动速度转化)
	EMOVETIMEPERTILE_MAX       = 99999,             // 可以移动时最慢跨格时间（对应最慢速度）
};

// 移动速度
enum EMOVESPEED
{
	EMOVESPEED_MIN              = 1,                // 最慢的移动速度（不包括定身）
	EMOVESPEED_NORAML           = 300,              // 人物默认的移动速度(用此速度做标准与跨格时间转化)
	EMOVESPEED_MAX              = 99999,            // 最快的移动速度
};

// 移动模式
enum EMoveStyle
{
	EMoveStyle_Walk,                                // 走                                                   
	EMoveStyle_Run,                                 // 跑
	EMoveStyle_Sitdown,                             // 坐下动作
	EMoveStyle_Sit,                                 // 坐

	EMoveStyle_Max,                                 // 最大
};

// 两次攻击间的时间间隔(毫秒)
enum EATTACKINTERVAL
{
	EATTACKINTERVAL_MIN          = 1,                // 两次攻击间的最短时间间隔
	EATTACKINTERVAL_NORMAL       = 1000,             // 标准攻击速度，1.0秒攻击一次
	EATTACKINTERVAL_MAX          = 99999,            // 两次攻击间的最长时间间隔
};
// 攻击速度值
enum EATTACKSPEED
{
	EATTACKSPEED_MIN              = 1,                 // 攻击速度最小值
	EATTACKSPEED_NORMAL           = 210,	           // 标准的攻击速度值
	EATTACKSPEED_MAX              = 99999,             // 攻击速度最大值
};

// 普通攻击冷ID
enum EAttackFreezeID
{
	EAttackFreezeID_Normal       = 1999,               // 人物普通攻击冷却ID
};

// 声音声效
enum  
{
	Sound_BackGround_1 = 1,
	Sound_BackGround_2,
	Sound_BackGround_3,
	Sound_BackGround_4,
	Sound_BackGround_5,
	Sound_BackGround_6,
	Sound_BackGround_7,
	Sound_SystemSound_Icon1 = 3001,
	Sound_SystemSound_Icon2,
	Sound_SystemSound_Icon3,
	Sound_SystemSound_Icon4,
	Sound_SystemSound_Icon5,
	Sound_SystemSound_EnterGame,
	Sound_SystemSound_LeaveGame,
	Sound_SystemSound_OpenSkep,
	Sound_SystemSound_CloseSkep,
	Sound_SystemSound_BuyGoods,
	Sound_SystemSound_SellGoods,
	Sound_SystemSound_Invite,
	Sound_SystemSound_Succe,
	Sound_SystemSound_Delect,
	Sound_SystemSound_Failed,
	Sound_SystemSound_QequestTask,
	Sound_SystemSound_FinishTask,
	Sound_SystemSound_FailedTask,
	Sound_SystemSound_Upgrade,
	Sound_SystemSound_SkillUpgrade,
	Sound_SystemSound_FalseOperate,
	Sound_SystemSound_SkillBreak,
	Sound_SystemSound_SystemTip,
	Sound_SystemSound_PrivateMsg,
	Sound_SystemSound_TradeLock,
	Sound_SystemSound_DrapIcon,
};
// 特效ID（指定播放的特效ID枚举）
enum
{
	Effect_FinishTask_EffectID = 1005,
	Effect_ArrivedPlace_EffectID = 1006,
	Effect_AutoMove_EffectID = 1007,
};
#pragma pack()