/*******************************************************************
** 文件名:	e:\Rocket\Server\ZoneServer\Include\ISchemeCenter.h
** 版  权:	(C) 
** 
** 日  期:	2008/1/16  11:34
** 版  本:	1.0
** 描  述:	配置中心,服务器端差不多所有脚本都在这里载入或获取数据
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include <map>

#pragma pack(1)

//by cjl
//ChangeBody.csv
#define CHANGE_BODY_SKILL_NUM 5
struct SChangeBodySchemeInfo
{
	char szName[MAX_PERSONNAME_LEN];//名称
	int  time;//变身持续时间
	int  resID;//裸体资源ID
	int  icon;//头标，客户端有用，服务端没用的
	int  bufferID;//buff表的ID,客户端有用，服务端没用的
	int  skill[CHANGE_BODY_SKILL_NUM];//技能
	int  newPropID;//新属性的配置
	int  flag;//标识符
	int getSkillNum()
	{
		int num = 0;
		for (int i=0;i<CHANGE_BODY_SKILL_NUM;i++)
		{
			if (skill[i]>0)
			{
				num++;
			}
		}
		return num;
	}

	bool haveState(int state)
	{
		return (flag & state) == state;
	}

	SChangeBodySchemeInfo()
	{
		memset(this, 0, sizeof(SChangeBodySchemeInfo));
	}
};

//Present.csv 
#define PRESENT_GOODS_MAX_NUMBER 10
struct  SPresentSchemeInfoBase
{
	SPresentSchemeInfoBase():num(0){};
	long goods[PRESENT_GOODS_MAX_NUMBER];
	long goodnum[PRESENT_GOODS_MAX_NUMBER];
	char num;//真实的个数
	DWORD flag[PRESENT_GOODS_MAX_NUMBER];//标志位
};

struct SPresentSchemeInfo : public SPresentSchemeInfoBase
{
	SPresentSchemeInfo():SPresentSchemeInfoBase(),second(0){};
	int second;
};
//end cjl

// monster.csv
enum EMonster
{
	EMonster_ID,                             // 怪物ID
	EMonster_Name,							 // 编辑器名
	EMonster_Profession,					 // 职业名
	EMonster_Function,						 // 功能名字
	EMonster_GameName,						 // 游戏场景名
	EMonster_ResID,							 // 资源ID
	EMonster_FaceID,                         // 头像ID
	EMonster_Type,							 // 类型
	EMonster_Camp,							 // 阵营 	
	EMonster_Level,							 // 等级
	EMonster_Fight,							 // 战斗力
	EMonster_HP,							 // 生命值
	EMonster_HPRecoverRatio,				 // 生命回复百分比
	EMonster_HPRecoverInterval,				 // 生命回复间隔(毫秒)
	EMonster_MP,							 // 魔法值
	EMonster_MoveSpeed,						 // 移动速度
	EMonster_WalkSpeed,						 // 走路速度
	EMonster_EyeShot,						 // 视野	
	EMonster_ChaseEyeShot,					 // 追击视野
	EMonster_RetrieveDist,					 // 回归/追击距离
	EMonster_RadiateDist,					 // 召唤视野
	EMonster_IsRadiated,					 // 是否被召唤
	EMonster_PrizeStrategy,					 // 奖励拥有策略	
							 
	EMonster_MinPAttack,					 // 最小物理攻击
	EMonster_MaxPAttack,					 // 最大物理攻击
	EMonster_MinMAttack,					 // 最小法术攻击
	EMonster_MaxMAttack,					 // 最大法术攻击
							 
	EMonster_PDefend,						 // 物理防御	
	EMonster_MDefend,						 // 法术防御
	EMonster_PresentPer,					 // 命中概率
	EMonster_HedgePer,						 // 闪避概率
	EMonster_FatalPer,						 // 会心概率
							 
	EMonster_Present,						 // 命中值
	EMonster_Hedge,							 // 闪避值
	EMonster_PFatal,						 // 会心值
	EMonster_KickFatal,						 // 御劲值
	EMonster_AttackSpeed,					 // 攻击速度值
							 
	EMonster_UsableSkill1_ID,				 // 可使用的技能1 [技能id]
	EMonster_UsableSkill1_Per,				 // 可使用的技能1 [使用概率]
	EMonster_UsableSkill1_Time,				 // 可使用的技能1 [使用时间]

	EMonster_UsableSkill2_ID,				 // 可使用的技能2 [技能id]
	EMonster_UsableSkill2_Per,				 // 可使用的技能2 [使用概率]
	EMonster_UsableSkill2_Time,				 // 可使用的技能2 [使用时间]

	EMonster_UsableSkill3_ID,				 // 可使用的技能3 [技能id]
	EMonster_UsableSkill3_Per,				 // 可使用的技能3 [使用概率]
	EMonster_UsableSkill3_Time,				 // 可使用的技能3 [使用时间]

	EMonster_UsableSkill4_ID,				 // 可使用的技能4 [技能id]
	EMonster_UsableSkill4_Per,				 // 可使用的技能4 [使用概率]
	EMonster_UsableSkill4_Time,				 // 可使用的技能4 [使用时间]
							 
	EMonster_AIID,							 // 可配置的AI
	EMonster_FullControl,					 // 是否需要缺省的AI（回归，退出战斗）,任务npc完全处在程序的控制之中
	EMonster_IdleRandom,					 // 闲逛的概率
};
struct SMonsterSchemeInfo
{
	long			lMonsterID;						// 怪物ID
	char			szName[16];						// 编辑器名
	char			szProfession[32];				// 职业名
	char			szFunction[16];					// 功能名
	char			szGameName[16];					// 游戏场景名
	long			lResID;							// 资源ID
	long            lFaceID;                        // 头像ID
	long			lType;							// 类型
	long			lCamp;							// 阵营 	
	long			lLevel;							// 等级
	long			lFight;							// 战斗力
	long			lHP;							// 生命值
	long            lHPRecoverRatio;                // 生命回复百分比
	long            lHPRecoverInterval;             // 生命回复间隔(毫秒)
	long			lMP;							// 魔法值
	long			lMoveSpeed;						// 移动速度
	long            lWalkSpeed;                     // 走路速度
	long			lEyeShot;						// 视野	
	long			lChaseEyeShot;					// 追击视野
	long			lRetrieveDist;					// 回归/追击距离
	long            lRadiateDist;                   // 召唤视野
	long            lIsRadiated;                    // 是否被召唤
	long			lPrizeStrategy;					// 奖励拥有策略	

	long			lMinPAttack;					// 最小物理攻击
	long			lMaxPAttack;					// 最大物理攻击
	long			lMinMAttack;					// 最小法术攻击
	long			lMaxMAttack;					// 最大法术攻击

	long			lPDefend;						// 物理防御	
	long			lMDefend;						// 法术防御
	long			lPresentPer;					// 命中概率
	long			lHedgePer;						// 闪避概率
	long			lFatalPer;						// 会心概率

	long			lPresent;					    // 命中值
	long			lHedge;						    // 闪避值
	long			lPFatal;                        // 会心值
	long            lKickFatal;                     // 御劲值
	long            lAttackSpeed;                   // 攻击速度值

	long			lUsableSkill1[3];				// 可使用的技能1 [技能id, 使用概率, 使用时间]
	long			lUsableSkill2[3];				// 可使用的技能2 [技能id, 使用概率, 使用时间]
	long			lUsableSkill3[3];				// 可使用的技能3 [技能id, 使用概率, 使用时间]
	long			lUsableSkill4[3];				// 可使用的技能4 [技能id, 使用概率, 使用时间]

	long			lAIID;							// 可配置的AI
	long			lFullControl;					// 是否需要缺省的AI（回归，退出战斗）,任务npc完全处在程序的控制之中
	long			lIdleRandom;					// 闲逛的概率
};

// PersonBaseProp.csv
struct SPersonBasePropSchemeInfo
{
	long			lBaseMinPA;						//最小物理攻击值
	long			lBaseMaxPA;						//最大物理攻击值
	long			lBasePD;						//物理防御值
	long			lBaseMinMA;						//最小法术攻击值
	long			lBaseMaxMA;						//最大法术攻击值
	long			lBaseMD;						//法术防御值
	long			lBaseHP;						//生命值
	long			lBaseHPRecover;					//生命恢复值
	long			lBaseMP;						//法力值
	long			lBaseMPRecover;					//法力恢复值
	long            lBaseSP;						//体力值
	long            lBaseSPRecover;					//体力恢复值
	long			lBaseHedge;						//闪避值
	long			lBaseHedgeRatio;				//闪避率
	long			lBasePresent;					//命中值
	long			lBasePresentRatio;				//命中率
	long			lBaseFatalDamage;				//会心值
	long			lBaseFatalDamageRatio;			//会心率
	long			lBaseKickFatalDamage;			//御劲值
	long			lBaseAttackSpeed;				//攻击速度
	long			lBaseMoveSpeed;					//移动速度
};

// PetData.csv
struct SPetSchemeInfo
{
	long            lPetID;                         // 宠物ID
	long            lResID;                         // 资源ID
	char            szName[16];                     // 名字
	long			lGrade;							// 档次
	long            lImageID[3];                    // 贴图ID
	long            lEggImageID[3];                 // 未解封图标ID
	long            lBigImageID[3];					// 大贴图ID
	long			lLevel;							// 出生等级
	bool            bState;                         // 默认解封状态
	long            lTransUpper;                    // 转世极限次数 
	long            lLevelDiff;						// 出征等级差值
	long			lFightChummy;					// 出征需要的最小亲密度
	long            lLevelUpper;                    // 等级上限值
	long            lReleaseTime;                   // 解封时间
	bool            bRide;                          // 是否骑乘
	long            lRealization[2];                // 顿悟等级限制
	long            lMoveSpeed;                     // 移动速度
	long			lRideSpeed;						// 骑乘速度
	long            lWatchSize;                     // 视野范围

	long            lGeniusNumUpper;                // 天赋数上限
	long            lSkillNum;                      // 技能数
	long            lFeedType;                      // 喂食类型
	long            lFeedTime;                      // 喂食时间间隔
	long            lType;                          // 大类型标识
	bool            bCombine;                       // 能否合体
	long			lEvolutionPetID;				// 进化后的宠物
	char			szDesc[512];					// 描述

	long			lBoxResID;						// 盒子外观资源ID	
	long			lBoxRayResID;					// 盒子闪烁资源ID
	long			lFightEffectID;					// 出征特效
	long			lFightEffectDelay;				// 出征特效延时
	long			lRollbackEffectID;				// 召回特效
	long			lCombineEffectID;				// 合体特效

	long			lUsableSkill[3];				// 可使用的技能[技能id, 使用概率, 使用时间]
	long			lAIID;							// AI
	long			lIdleRandom;					// 闲逛概率	
	long			lPropID;						// 宠物战斗属性表格中的ID
	long			lCompdType;						// 合成类型
	long			lBindFlag;						// 绑定标志位	
	long			lUseTerm;						// 使用期限
	long			lPower;							// 威力类型
	int             iGeniusRandom[C_PET_GENIUS_SIZE];// 各天赋出现的概率
};

// PetPropData.csv
// 宠物战斗相关属性配置
///  成长率保留三位小数，因为是按整数保存，所以计算时要除以10000
struct SPetPropSchemeInfo
{
	long            lPetPropID;                     // 宠物属性ID
	char            szTypeDes[16];                  // 名字
	long            lMainProp[PET_TYPEPROP_MAX];	// 主属性
	long			lMinorProp[PET_TYPEPROP_MAX];	// 副属性

	long            lChummyUpper;                   // 亲密上限值
	long            lChummyDrop;                    // 亲密度死亡减少值
	long            lChummyInc;						// 亲密度恢复率

	long            lLuckyValueUpper;               // 幸运值上限

	long            lInitHpLower;                   // 初始生命下限
	long			lInitHpRandUpper;				// 初始生命随机值上限
	long            lInitHpUpper;                   // 初始生命上限

	long            lInitPAtcLower;                 // 初始物攻下限
	long			lInitPAtcRandUpper;				// 初始物攻击随机值上限
	long            lInitPAtcUpper;                 // 初始物攻上限

	long            lInitMAtcLower;                 // 初始魔攻下限
	long			lInitMAtcRandUpper;				// 初始魔攻击随机值上限
	long            lInitMAtcUpper;                 // 初始魔攻上限

	long            lInitPDefLower;                 // 初始物防下限
	long			lInitPDefRandUpper;				// 初始物防击随机值上限
	long            lInitPDefUpper;                 // 初始物防上限

	long            lInitMDefLower;                 // 初始魔防下限
	long			lInitMDefRandUpper;				// 初始魔防击随机值上限
	long            lInitMDefUpper;                 // 初始魔防上限

	long            lHPGrowthLower;                 // 生命成长率下限
	long			lHPGrowthRandUpper;				// 生命成长率随机值上限
	long            lHPGrowthUpper;                 // 生命成长率上限

	long            lPAtcGrowthLower;				// 物攻成长率下限
	long			lPAtcGrowthRandUpper;			// 物攻成长率随机值上限
	long            lPAtcGrowthUpper;				// 物攻成长率上限

	long            lMAtcGrowthLower;				// 魔攻成长率下限
	long			lMAtcGrowthRandUpper;			// 魔攻成长率随机值上限
	long            lMAtcGrowthUpper;				// 魔攻成长率上限

	long            lPDefGrowthLower;               // 防御成长率下限
	long			lPDefGrowthRandUpper;			// 防御成长率随机值上限
	long            lPDefGrowthUpper;               // 防御成长率上限

	long            lMDefGrowthLower;               // 魔防成长率下限
	long			lMDefGrowthRandUpper;			// 魔防成长率随机值上限
	long            lMDefGrowthUpper;               // 魔防成长率上限

	long			lInitHPUpperGrade;				// 初始生命值极限值评分
	long			lInitMinPAUpperGrade;			// 初始最小物攻极限值评分
	long			lInitMaxPAUpperGrade;			// 初始最大物攻极限值评分
	long			lInitMinMAUpperGrade;			// 初始最小法术攻击极限值评分
	long			lInitMaxMAUpperGrade;			// 初始最大法术攻击极限值评分
	long			lInitPDefUpperGrade;			// 初始物理防御极限值评分
	long			lInitMDefUpperGrade;			// 初始法术防御极限值评分
	long			lHPGrowthUpperGrade;			// 生命值成长率极限值评分
	long			lMinPAGrowthUpperGrade;			// 最小物理攻击成长率极限值评分
	long			lMaxPAGrowthUpperGrade;			// 最大物理攻击成长率极限值评分
	long			lMinMAGrowthUpperGrade;			// 最小法术攻击成长率极限值评分
	long			lMaxMAGrowthUpperGrade;			// 最大法术攻击成长率极限值评分
	long			lPDefGrowthUpperGrade;			// 物理防御成长率极限值评分
	long			lMDefGrowthUpperGrade;			// 法术防御成长率极限值评分
};

// 宠物阵法站位
struct SPetFormation
{
	long	lFormationID;							// 阵法ID
	char	szName[8];								// 阵法名称
	long	lRadius[3];								// 方位半径
	long	lAngle[3];								// 方位角度
	long    lPosFactor[8];							// 方位加成系数
	long    lExFactor;								// 控制系数	
	long	lKeyPos;								// 阵眼
	long	lActorPropID;							// 角色收益属性
	long    lPetPropID;								// 宠物对应属性
	float	fPetPosSkillFactor[8];					// add by zjp 宠物阵法技能的评分系数
	float	fPetPosSkillGrade[5];					// add by zjp 宠物阵法技能的梯度值
};

// 宠物跟等级挂钩的属性配置
struct SPetLevelPropInfo					
{
	long		lNextExp;						// 升下级经验
	long		lFatalPer;						// 会心率
	long		lFatal;                         // 会心值
	long		lPresent;					    // 命中值
	long		lPresentPer;					// 命中率
	long		lHedge;						    // 闪避值
	long		lHedgePer;						// 闪避率
	long		lKickFatal;                     // 御劲值
	long		lKickFatalPer;					// 御劲率
	long		lAttackSpeed;					// 攻击速度值

	SPetLevelPropInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// PetFilter.csv
struct SPetFilterInfo
{
	int             nID;                            // 筛选对应的配置ID
	int             nType;                          // 筛选类型(属于哪一类筛选)
	int             nTypeLevel;                     // 对应类型的筛选等级
	char            szName[16];                     // 对应筛选的名称
	int             nPlayerLevel;                   // 玩家等级
	int             nPlayerNazarite;                // 玩家修真等级
	int             nPlayerHomage;                  // 玩家官职
	int             nPetLevel;                      // 魂将等级
	int             nPetInsetLevel;                 // 魂将内丹等级
	int             nPetStar;                       // 魂将品质
	bool            bPetRide;                       // 魂将骑乘
	int             nPetType;                       // 魂将类型
	char            szPosAttribute[16];             // 阵法五行
	int             nNeedPetNum;                    // 阵法触发需要宠物数
	int             nPosPlace;                      // 阵法触发方位
	int             nHostPetLevel;                  // 主魂将等级
	int             HostPetPointLower;              // 主魂将评分下限
	int             nHostPetPointUpper;             // 主魂将评分上限
	int             nSecondPetLevel;                // 次魂将等级
	int             nSecondPetPoint;                // 次魂将评分
};
// 宠物合成配置评分规则
struct SPetCompdGradeRule
{
	long	lRefineID;						// 炼魂（合成）类型ID
	char    szName[16];						// 炼魂类型名
	long	lMainGrade;						// 主宠评分
	long	lMinorGrade;					// 副宠评分
	long	lMinorChummy;					// 副宠的亲密度
	short	nIsSameElement;					// 是否需要同相性（金木水火土）
	short   nNeedPropComp;					// 是否比较属性
	short	nNeedIDCard;					// 是否需要身份牌
	short	nNeedSameType;					// 同类型
	short	nNeedLuckyComp;					// 是否需要比较幸运值
	short   nIsTypeRelated;					// 是否跟具体类型有关
};

// 宠物合成配置等级规则
struct SPetCompdLevelRule
{
	short	nMainLevel;						// 主宠级别
	short	nMinorLevel;					// 副宠级别
};

// 物品配置基类
struct SGoodsSchemeInfo
{
	long			lGoodsID;						// 物品ID,所有
	char			szName[32];						// 名字

	long			lGoodsClass;					// 物品类（1：装备，2：物品）
	long			lGoodsSubClass;					// 物品子类（装备和药品各不相同）

	long			lAllowVocation;					// 使用职业
	long			lAllowLevel;					// 使用等级

	long            lPreCondition;                  // 前置条件

	long			lPileQty;						// 叠放数量

	long			lBuyCost;						// 买入基础价格
	long			lSaleCost;						// 卖出基础价络

	long			lBaseLevel;						// 基础物品档次
	long			lPickupBcastFlag;				// 拾取后广播标志
	long			lHoldMaxQty;					// 拥有最多个数
	long			lShortcutFlag;					// 能否放快捷栏
	long			lBindFlag;						// 绑定标志位	
	long			lUseTerm;						// 使用期限

	long			lIconID[3];						// 物品栏图片（正常、高亮、无效）
	long			lIconEffectID;					// 物品栏光效

	long			lBoxResID;						// 盒子外观资源ID	
	long			lBoxRayResID;					// 盒子闪烁资源ID

	char			szDesc[256];					// 描述

	long			lUseCount;						// 使用次数
	long			lGoldGotPrice;					// 元宝价格
	long			lLogoutAction;					// 登出行为，0表示登出不消失不掉落，1是登出消失，2是登出掉落。
	long			lChangeMapAction;				// 0表示离开当前地图不消失，1表示离开当前地图消失。
	long			lCanSaleToNPC;					// 能否与NPC交易
	long			lIsDieDrop;						// 死亡是否掉落
	long			lQualityLevel;					// 品质属性等级

	
};

//Equippment.csv
#define ESI_EQUIPLOCNUM							3//装备的可装备位置
#define ESI_EQUIPAPPIDLEFT						0//装备第一个资源id
#define ESI_EQUIPAPPIDRIGHT						1//装备第二个资源id
#define ESI_EQUIPAPPMAX							2//装备资源上限
#define ESI_SMELTSOULSTABLENUM					3//附魂强化稳定态数量
#define ESI_SMELTSOULMAXLEVEL					9//附魂强化级别数
#define ESI_SMELTSOULLEVELATTRIBNUM				4//附魂级别对应的属性数量
#define ESI_BRANDATTRIBMAXLEVEL					6//最大品质属性等级
#define ESI_MAXSLOT								4//装备最大孔数
#define ESI_EQUIP_BASIC_COMBAT_PROPNUM			4//装备基础属性的最大数量
#define ESI_EQUIP_BRAND_COMBAT_PROPNUM			4//装备品质属性的最大数量
struct SEquipmentSchemeInfo: public SGoodsSchemeInfo
{
	long			lAllowCamp;						//使用阵营
	long			lAllowGender;					//使用性别

	long			lSuitEquipID;					// 套装ID

	// note by zjp;对应于Equipment_BasicAttribute.csv里面每一行四个基础属性
	//装备的基础属性
	long			lBasicCombatPropID[ESI_EQUIP_BASIC_COMBAT_PROPNUM];	//基础属性id
	long			lBasicCombatPropVal[ESI_EQUIP_BASIC_COMBAT_PROPNUM];//基础属性值
	
	long			lCanRepair;						//是否可修理
	long			lMaxEndurance;					//最大耐久度

	long			lEquipLoc[ESI_EQUIPLOCNUM];				//可装备位置
	float			fBaseMaleZoom;							//男放大倍数
	uint			uiBaseMaleColor;						//男颜色
	long			lBaseMaleModelResID[ESI_EQUIPAPPMAX];	//男资源id
	long			lBaseMaleTextureResID[ESI_EQUIPAPPMAX]; //男贴图id

	float			fBaseFemaleZoom;						//女放大倍数
	uint			uiBaseFemaleColor;						//女基础颜色
	long			lBaseFelmaleModelResID[ESI_EQUIPAPPMAX];//女资源id
	long			lBaseFelmaleTextureResID[ESI_EQUIPAPPMAX];//女铁贴图id

	long			lNeedIdentify;							//是否需要鉴定,by cjl
	long			lBrandAttribNum;						//品质属性数量
	long			lBrandLevel;							//品质属性等级,历史原因 lQualityLevel 也是品质属性等级
	long			lBrandAttribID;							//品质属性ID，用于索引品质属性

	long			lMaxSlot;								//最大孔数

	long			lMaxSmeltSoulLevel;						//最大强化等级
	long			lSmeltSoulStablePropID[ESI_SMELTSOULSTABLENUM]; //附魂强化稳定态属性id
	long			lSmeltSoulStablePropValue[ESI_SMELTSOULSTABLENUM];//附魂强化稳定态属性值
	long			lSmeltSoulLevelPropID[ESI_SMELTSOULMAXLEVEL][ESI_SMELTSOULLEVELATTRIBNUM];//附魂强化级别数对应的属性id
	long			lSmeltSoulLevelPropValue[ESI_SMELTSOULMAXLEVEL][ESI_SMELTSOULLEVELATTRIBNUM];//附魂强化级别数对应的属性值

	long			lThrowAndDestroy;				//by cjl 丢弃就销毁

	SEquipmentSchemeInfo(void)
	{
		memset(this, 0, sizeof(SEquipmentSchemeInfo));
	}

	~SEquipmentSchemeInfo(void)
	{
		memset(this, 0, sizeof(SEquipmentSchemeInfo));
	}
};

//
//装备强化的稳定态配置
struct SEquipmentSmeltStalbeConfig
{
	long	lSmeltLevel;
	long	lStableLevel;	
};

//装备属性名和中文名字的映射表
struct STableEquipCombatPropID2PropName
{
	typedef std::map<int, std::string> MapInt2String;
	typedef std::map<std::string, int> MapString2Int;
	typedef std::map<int, int> MapInt2Int;//by cjl

	MapInt2String basic_id2name;//基础属性
	MapString2Int basic_name2id;//基础属性
	MapInt2Int	  basic_id2value;//by cjl,用于显示类型

	MapInt2String brand_id2name;//品质属性
	MapString2Int brand_name2id;//品质属性
	MapInt2Int    brand_id2value;//by cjl,用于显示类型

	MapInt2String smeltlevel_id2name;//炼魂属性(与级别相关)
	MapString2Int smeltlevel_name2id;//炼魂属性(与级别相关)
	MapInt2Int    smeltlevel_id2value;//by cjl,用于显示类型

	MapInt2String smeltstable_id2name;//炼魂属性(稳定态)
	MapString2Int smeltstable_name2id;//炼魂属性(稳定态)
	MapInt2Int    smeltstable_id2value;//by cjl,用于显示类型
};

//by cjl
//装备特效配置表
#define EEL_EFFECTLEVEL								10//装备特效的等级,第一个是默认的
struct SEquipmentAttachedEffects
{
	long	lID[EEL_EFFECTLEVEL];					  //特效id
};
//Equipment_BrandAttrib.csv
//装备品质属性配置表
#define EBA_BRANDATTRIB_AVALIBLENUM							4//品质数量,和服务端不同，只需要关系前4个
struct SEquipmentBrandAttribute
{
	long	lID;										//品质属性id
	long	lAvaliableNum;								//有效的品质属性数量(不一定满30个属性)
	long	lAttribName[EBA_BRANDATTRIB_AVALIBLENUM];			//属性名称
	long	lAttribValBound[EBA_BRANDATTRIB_AVALIBLENUM];	//属性值上下届
	//long	lAttribValUpperBound[EBA_BRANDATTRIB_AVALIBLENUM];	//属性值上届
	SEquipmentBrandAttribute(void) { memset(this,0,sizeof(SEquipmentBrandAttribute)); } 
	~SEquipmentBrandAttribute(void) { memset(this,0,sizeof(SEquipmentBrandAttribute)); }
};
//end cjl

//GemsCombinationEffect.csv
//宝石组合效果配置表
#define GCE_EFFECTRANK								3//组合属性有3阶
#define GCE_RANKBITNUM								3//有3位表示宝石等级，宝石有7个等级
#define GCE_EFFECTNAMELEN							16//组合效果的名字
struct SGemsCombinationEffect
{
	long	lEffectID;								//组合效果id，由不同的宝石按顺序生成
	long	lEffectLevel;							//组合的级别数
	char	cEffectName[GCE_EFFECTNAMELEN];			//组合的名称
	
	long			lPMinAttack;				// 增加物理最小攻击
	long			lPMaxAttack;				// 增加物理最大攻击
	long			lPDefend;					// 增加物理防御,没有最大最小值之分

	long			lMMinAttack;				// 增加法术最小攻击
	long			lMMaxAttack;				// 增加法术最大攻击
	long			lMDefend;					// 增加法术最大防御，没有最大值和最小值之分

	long			lPresent;					//增加命中概率
	long			lHedge;						//增加闪避概率

	long			lFatalDamage;				//增加暴击值
	long			lResist;					//增加格挡

	long			lHPLimmit;					//增加生命上限
	long			lMPLimmit;					//增加法力上限

	long			lMoveSpeed;					//增加移动速度百分比
	long			lAttackSpeed;				//增加攻击速度百分比

	long			lDirectPlusPDamage;			//增加无视物理防御的物理伤害
	long			lDirectPlusMDamage;			//增加无视法术防御的法术伤害
	long			lDirectMinusPDamage;		//直接减少所受物理伤害
	long			lDirectMinusMDamage;		//直接减少所收法术伤害		
	SGemsCombinationEffect(void) { memset(this,0, sizeof(SGemsCombinationEffect) );}
	~SGemsCombinationEffect(void) {}
};

// NonequipableGoods.csv
struct SNonequipableGoodsSchemeInfo : public SGoodsSchemeInfo
{
	long			lOnID;							// 作用ID
	long			lSkillRequirement;				// 技能要求
	long			lCanSaveToWare;					// 能否放入仓库
	long			lCanDrouOut;					// 能否丢弃
	long            lFlashID;                       // 使用物品后的光效

	// add by zjp;新增加的配置
	long			lCanUse;						// 是否可以使用
	long			lLoadTime;						// 使用时的读条时间
	long			lFunctionLevel;					// 作用ID等级
	long			lCDType;						// 使用时是单CD还是公共CD
	long			lCDTime;						// 使用时CD时间

	// zgz
	long            lPetID;							// 关联的宠物ID，使用此物品会产生一只宠物
};

// MapInfo.xml
struct SMapSchemeInfo
{
	long			lMapID;							// 地图ID
	char			szName[32];						// 地图名字
	long			lOpenZoneServerID;				// 开放场景服ID
	long			lStartLoadFlag;					// 启动是否载入， 1：载入。2：不载入
	long			lDynamicFlag;					// 是否动态载入， 1：动态。2：不动态
	long			lVestNation;					// 归属国家，0：表示中立。1、2、3.... ：国家ID
	char			szMapFile[MAX_PATH];			// 地图文件集合
	char			szWayFile[MAX_PATH];			// 用于读取各地图的自动寻路文件
	char			szWorldPicFile[MAX_PATH];		// 世界地图
	char			szMonsterBuildFile[MAX_PATH];	// 怪物生成配置文件
	char			szChunnelBuildFile[MAX_PATH];	// 传送门生成配置文件
	char			szBoxBuildFile[MAX_PATH];		// 草药，矿等等的生成配置文件
	long			lBackMusicID;					// 背景音乐id
	long			lGroupMapID;					// 组地图id

    long            lCellFlag;                      //该地图是否可以分线
	long            lCellCapacity;                  //分线时，每个场景最多可以容纳的人数
	long            lInstanceFlag;					//副本标志Instance 当出现该标志时忽略 分线标志cell    
	long			lInstanceKickNotify;			//副本踢人是否发生倒数提示
};

// Entity.xml
struct SPropCountRateSchemeInfo
{
	int				nConstit_ex_HP;					// 一点体质兑换多少生命值
	int				nConstit_ex_PD;					// 一点体质兑换多少物理防御
	int				nForce_ex_PA;					// 一点力量兑换多少物理攻击
	double			fCelerity_ex_PS;				// 一点敏捷兑换多少物理致命
	double			fCelerity_ex_Present;			// 一点敏捷兑换多少命中
	int				nIntellect_ex_MA;				// 一点智力兑换多少法术攻击
	int				nEnergy_ex_MD;					// 一点精神兑换多少法术防御
	int				nEnergy_ex_MP;					// 一点精神兑换多少法术值
	double			fEnergy_ex_MF;					// 一点精神兑换多少法术致命
	double			fEnergy_ex_Present;				// 一点精神兑换多少命中
	double			fDefault_base_Present;			// 默认基础命中率
	double			fDefault_base_Hedge;			// 默认基础闪避率
	double			fDefault_FatalPer;		        // 默认致命概率
	double			fDefault_base_Penetrate;		// 默认基础穿透
	double			fDefault_base_Resist;			// 默认基础格挡
};
//Aduio.XML
struct SAudioConfig 
{
	long            lMusicID;                      //音乐ID
	char            szMusicFileName[MAX_PATH];     //音乐文件
};
// ConfingAreaAudio.XML
struct SAreaAudioConfig
{
	int            nMapID;          // 地图ID
	int            nMusicID; 
	int            nMusicType;      // 声音类型
	int            nLoop;           // 循环次数    
    xs::Rect	       rectArea;		// 矩型坐标
};

//Creature.xml

struct ConfigRide//骑乘配置
{
	std::string boneName;//骑乘时绑定骨骼
	std::string rideAction;//骑乘动作
};

struct SCreatureConfig: public ConfigModelNode
{
	std::string resId;
	std::string basicCreatureName;
	ConfigRide  cRide;
};

/////////////////////换装////////////////////////////////////
//////////不需要导出到lua/////////////////////////////////////

/// 实体部件
enum EntityParts
{
	EEntityPart_Invalid = 0,	//无效值
	EEntityPart_Weapon,			//武器
	EEntityPart_Armet,			//头盔
	EEntityPart_Armor,			//盔甲
	EEntityPart_Cuff,			//手套
	EEntityPart_Shoes,			//鞋子
	EEntityPart_Necklace,		//项链
	EEntityPart_Ring,			//戒指
	EEntityPart_DecorationL,	//左挂坠
	EEntityPart_DecorationR,	//右挂坠
	EEntityPart_Suit_Armet, //时装头盔
	EEntityPart_Suit_Armor,//时装盔甲
	EEntityPart_Max,		//最大值，超出部分算扩展
};

//换装时，资源分两种，一种是主要资源，另外一种是附加资源，即附加的特效
enum // 换装时资源的上限
{
	EBindResNum_MainPart = 2,//主要资源的上限
	EBindResNum_Attachment = 6,//附加资源的上限
};

enum //资源的使用类型，目前只使用了模型绑定
{
	EBindType_Min,
	EBindType_Replace=0, //替换整个模型
	EBindType_SubNode, //绑定模型
	EBindType_Texture, //替换纹理（目前没用）
	EBindType_Max,
};

enum //在绑定模型的情况下，绑定点
{
	//以下是主要部件的绑定点
	EBindPoint_Min = 0, //
	EBindPoint_LeftHand ,//左手
	EBindPoint_RightHand ,//右手
	EBindPoint_LeftBack,//左背
	EBindPoint_RightBack,//右背
	EBindPoint_Head,//头部
	//以下是附加特效的绑定点
	EBindPoint_Attach_WeakRay,//外部弱光
	EBindPoint_Attach_StrongRay, //外部强光
	EBindPoint_Attach_FlowRay_1, //流动光1
	EBindPoint_Attach_FlowRay_2, //流动光2
	EBindPoint_Attach_RadiationRay, //膨胀光
	EBindPoint_Attach_FewFloatParticles, //少量漂浮粒子
	EBindPoint_Attach_SomeFloatParticles, //一些漂浮粒子
	EBindPoint_Attach_RadiationParticles, //发散粒子
	EBindPoint_Attach_RibbonParticles, //带有拖尾效果的粒子
	EBindPoint_Max,
};
enum //装备附加效果类型
{
	EAttachType_Min =0,
	EAttachType_SmeltSoul, //装备升级的绑定类型
	EAttachType_EmbedGems, //装备嵌入宝石
	EAttachType_Max,
};

enum EWeaponClass //武器类型
{
	EWeapon_NoWeapon,//没有武器
	EWeapon_FirstClass,//长武器，弓，书卷
	EWeapon_SecondClass,//短武器，弩，法杖
	EWeapon_Max,
};

struct SPersonMainPartChangeContext //人物主要装备换装环境
{
	ulong resId[EBindResNum_MainPart];//资源id
	bool perform;//true：穿上装备；false：卸下装备
	uchar part;//换装部件
	uchar bindType[EBindResNum_MainPart];//绑定方式
	uchar bindPoint[EBindResNum_MainPart];//绑定点
	uchar weaponclass;//对武器有效
	uchar weaponsubclass;//武器子类，对武器有效
};

struct SPersonAttachPartChangeContext//人物主要装备附加效果换装环境
{
	ulong resId[EBindResNum_Attachment];//资源id
	float	scale[EBindResNum_Attachment];//附加特效的缩放
	uint color[EBindResNum_Attachment];//绑定的颜色
	bool perform;//true：附加；false：取消附加
	uchar part;//绑定部件
	uchar attachType;//附加的类型
	uchar bindType[EBindResNum_Attachment];//绑定方式
	uchar bindPoint[EBindResNum_Attachment];//绑定点
};

struct SPersonPartDeformationContext
{
	bool deform;//是否变形
	ulong part;//绑定部件，目前只支持武器
	ulong resId[EBindResNum_MainPart];//变形使用的资源id
	//不需要绑定点和绑定类型，因为这是附加的
};

// add by zjp；特殊物品
#define SOUL_LEVEL_MAX						5//by cjl
struct SEspecilGood
{
	int		nGoodID;
	int		nProtect;
	bool	bSoul;
	int		nSoulLevel;		  // 魂石等级
	bool	bAssistantSmelt;
	float	nAddProbability; // 增加附魂成功率
	int		nReduceLevel;	// 附魂失败降低等级数
	int		nPillLevel;		//内丹等级//by cjl ,帮达勇补充
	bool	bPill;			//是否内丹//by cjl ,帮达勇补充
};

// add by zjp；装备等级-魂魄品质
struct SEquipLevelToQuality
{
	int nUpperLevel; // 等级上限
	int nQuality;	 // 魂魄品质
};

// add by zjp；炼魂材料
struct SSmeltInfo
{
	int nSmeltLevel;	// 炼魂级别
	std::string sName1;      // 魂魄名字1
	int    nNum1;		// 魂魄数量1
	std::string sName2;		// 魂魄名字2
	int    nNum2;		// 魂魄数量2
};

struct SSmeltSoulProbability
{
	int nSoulCount;
	int nProbability;
};

struct SGoodsType
{
	int         nGoodsID;           // goods ID
	char		szDesc[16];			// 描述
	SGoodsType()
	{
		memset(this, 0, sizeof(SGoodsType));
	}
};

struct SItemProp
{
	int  nItemID;
	char szName[32];
	SItemProp()
	{
		memset(this, 0, sizeof(SItemProp));
	}
};

struct SStallInfo
{
	int         nStallID;           // 摊位ID
	int			nMapID;				// 地图ID
	char		szStallName[32];	// 摊位名字

	//int			nStallFlagMonsterID;// 摊位旗怪物ID

	int         nPrimaryResID;		// 初级摊位NPCID
	int         nIntermediateResID; // 中级摊位NPCID
	int         nAdvanceResID;      // 高级摊位NPCID

// 	int         nFlagTileX;			// 摊位旗坐标X
// 	int         nFlagTileY;			// 摊位旗坐标Y
// 	int         nFlagAngle;			// 摊位旗方向

	int         nTileX;				// 坐标X
	int         nTileY;				// 坐标Y
	int         nAngle;				// 方向
};

struct SOnlineShopGoods
{
	int		iGoodsID;		//商品ID编号
	int		iItemID;		//物品ID（对应Equipment和Nonequipablegoods表中的物品ID）
	char    szGoodsName[24];	//名字最多9个汉字
	int		eGoodsType;		 //商品大类
	int		eGoodsSubType;	 //商品子类
	//与配置表不同的是，这里转为记录贩卖价格 与 原始价格
	int		iPriceBaobi;	//宝币价格
	int		iOriginalPriceBaobi; //原始宝币价格
	int		iPriceJiaozi;   //交子价格
	int		iOriginalPriceJiaozi;//原始交子价格
	const char* GetGoodsName(void) const
	{
		return szGoodsName;
	}
};

//--配置表中"折后价格"字段如果非0，则表示该商品为打折商品，
//--打折商品的价格显示为折后价格，而非原价。玩家购买时计算的也是折后价格。
//--为了界面统一这里伪造一个促销的大类 为99999
#define ONLINE_SHOP_PROMOTION_FAKE_TYPE 9999

///////////////////////////////////////////////////////////////

struct SMapWeather
{
	ulong ulWeatherMagicID;
	int	  nDurationTime;  // -1表示，在60~300直接随机
	int  nFade;
	int  nFall;
	SMapWeather()
	{
		ulWeatherMagicID = 0;
		nDurationTime = 0;
		nFade = 0;
		nFall = 0;
	}
};


///////////////////////////////////////////////////////////////////
// 配置中心
struct ISchemeCenter
{
	//by cjl
	/** 取得变身的配置信息
	@param  
	@return  
	*/
	virtual  SChangeBodySchemeInfo*			GetChangeBodySchemeInfo(int id) = 0;

	/** 取得新手礼包的配置信息
	@param  职业 
	@param  礼包等级,从1开始
	@return  
	*/	
	virtual SPresentSchemeInfo *			GetPresentSchemeInfo(int vocation, int PresentLevel) = 0;
	//end cjl

	/** 取得怪物的配置信息
	@param   
	@param   
	@return  
	*/	
	virtual SMonsterSchemeInfo *			GetMonsterSchemeInfo(long lMonsterID) = 0;

	/** 取得宠物的配置信息
	@param   
	@param   
	@return  
	*/	
	virtual SPetSchemeInfo*                GetPetSchemeInfo(long lPetID) = 0;

	/** 取得宠物的阵法配置信息
	@param   
	@param   
	@return  
	*/
	virtual SPetFormation *                 GetPetFormation(long lFormation) = 0;


	/** 取得物品的配置信息
	@param   
	@param   
	@return  
	*/
	virtual SGoodsSchemeInfo *				GetGoodsSchemeInfo(long lGoodsID) = 0;

	/** 取得装备的配置信息
	@param   lGoodsID ：物品ID
	@return  
	*/
	virtual SEquipmentSchemeInfo *			GetEquipmentSchemeInfo(long lGoodsID) = 0;

	/** 取得装备的附件品质属性配置信息 by cjl
	@param   lGoodsID ：物品ID
	@return  
	*/
	virtual SEquipmentBrandAttribute *			GetEquipmentBrandAttributesSchemeInfo(long lGoodsID) = 0;

	/**得到装备的附加特效
	@param
	@param
	@return
	*/
	virtual SEquipmentAttachedEffects * GetEquipmentAttachedEffects(long lAttachEffectID) = 0;

	/** 取得非装备类物品的配置信息
	@param   
	@param   
	@return  
	*/
	virtual SNonequipableGoodsSchemeInfo *			GetNonequipableGoodsSchemeInfo(long lGoodsID) = 0;

	/** 取得装备宝石镶嵌的组合效果
	@param   lGoodsID ：物品ID
	@return  
	*/
	virtual SGemsCombinationEffect * 		GetEquipmentEmbedGemsCombinationEffect(long lEffectID) = 0;

	/** 取得地图配置信息
	@param   
	@param   
	@return  
	*/
	virtual SMapSchemeInfo *				GetMapSchemeInfo(long lMapID) = 0;

	/**取得声音的配置信息
	@param   lMusicID：声音ID
	@param   
	@return  
	*/
	virtual SAudioConfig*                  GetAudioSchemeInfo(long lMusicID) = 0;

	///**取得场景的声音ID
	//@param  
	//@param   
	//@return  
	//*/
	virtual bool                 GetAreaAudioMusicID(int lMapID,xs::Point pt,int &nMusicID,int &nMusicType,int &nLoopType) = 0;

	/** 取得初始仇恨
	@param   nSelfCampID ：自已的阵营ID
	@param   nOtherCampID：对方的阵营ID
	@return  
	*/
	virtual int								GetInitEnmity(int nSelfCampID, int nOtherCampID) = 0;

	/** 取得人物基础属性
	@param   lLevel : 等级
	@param   lVocation : 职业
	@return  
	*/
	virtual SPersonBasePropSchemeInfo *		GetPersonBasePropSchemeInfo(long lLevel, long lVocation) = 0;

	/** 通过阵营ID去获取阵营名字
	@param   nCampID ：阵营ID
	@return  
	*/
	virtual std::string             GetCampNameByCampID(int nCampID) = 0;

	/** 取得“升下级经验”
	@param   
	@param   
	@return  
	*/
	virtual int								GetNextLevelExp(int nCurLevel) = 0;

	/** 取得生物移动速度
	@param   nSpeedLevel ：速度等级
	@param   
	@return  
	*/
	virtual int								GetCreatureMoveSpeed(int nSpeedLevel) = 0;

	/** 取得生物骑乘速度
	@param   nSpeedLevel ：速度等级
	@param   
	@return  
	*/
	virtual int								GetCreatureRideSpeed(int nSpeedLevel) = 0;


	/** 取得宠物筛选的数据
	@param   nType ：类型
	@param   nTypeLevel：等级
	@return  
	*/
	virtual SPetFilterInfo*                 GetPetFilterSchemeInfo(int nTypeID)= 0;

	/** 得到宠物升下级的经验
	@param   
	@param   
	@return  
	*/
	virtual SPetLevelPropInfo *				GetPetLevelPropInfo(int nCurLevel) = 0;

	/** 得到主要装备换装环境
	@param   
	@param   
	@return  
	*/
	virtual bool							GetPersonMainPartChangeContext(int nEquipID, int nGender, int nSmeltSoulLevel,SPersonMainPartChangeContext & context  )= 0;//by cjl,添加了当前强化等级


	/** 得到人物主要装备附加效果换装环境
	@param   
	@param   
	@return  
	*/
	virtual bool							  GetPersonAttachPartChangeContext(int nEquipID, int nAttachType, int nSmeltSoulLevel, int nEmbedGems, SPersonAttachPartChangeContext & context)= 0;
	

	/** 得到人物换装环境绑定点
	@param   
	@param   
	@return  
	*/
	virtual const char * getBindPoint(ulong bp) = 0;

	/** 取得宠物合成的评分限制规则
	@param   
	@param   
	@return  
	*/
	virtual SPetCompdGradeRule *           GetPetCompdGradeSchemeInfo(long lRefineID,long lGrade) = 0;

	/** 取得宠物合成的级别限制规则
	@param   
	@param   
	@return  
	*/
	virtual SPetCompdLevelRule *           GetPetCompdLevelSchemeInfo(long lLevel) = 0;

	/** 获得装备战斗属性id与名字的映射表
	@param   
	@param   
	@return  
	*/
	virtual const STableEquipCombatPropID2PropName * GetEquipTableCombatPropId2PropName() = 0;
	/** 取得宠物战斗属性
	@param   
	@param   
	@return  
	*/
	virtual SPetPropSchemeInfo *			GetPetPropSchemeInfo(long lPetID) = 0;

	/** 获得装备链魂稳定态配置
	@param   
	@param   
	@return  
	*/
	virtual const SEquipmentSmeltStalbeConfig * GetEquipSmeltStableConfig(int smeltlevel ) = 0;

	/** 获得特殊物品属性配置 add by zjp
	@param   
	@param   
	@return  
	*/
	virtual SEspecilGood* GetEspecialGood(int nGoodID) = 0;

	/** 获得炼魂魂魄等级 add by zjp
	@param   
	@param   
	@return  
	*/
	virtual int	 GetNeededQuality(int nLevel) = 0;

	/** 取得炼魂信息配置 add by zjp
	@param   
	@param   
	@return  
	*/
	virtual SSmeltInfo*  GetSmeltInfo(int nSmeltLevel) = 0;

	/** 取得非装备的炼魂信息配置 add by zjp
	@param   
	@param   
	@return  
	*/
	virtual SSmeltInfo*  GetSmeltNotWeaponInfo(int nSmeltLevel) = 0;

	/** 取得精炼装备金额 add by zjp
	@param   
	@param   
	@return  
	*/
	virtual int		    GetSmeltEquipFee(int nLevel) = 0;

	/** 取得重铸金额 add by zjp
	@param   
	@param   
	@return  
	*/
	virtual int         GetRebuildEquipFee(int nLevel) = 0;

	/** 取得魂魄升级费用
	@param   
	@param   
	@return  
	*/
	virtual int			GetSoulUpgradeFee(int nQuality) = 0;

	/** 通过名字取得魂魄物品ID
	@param   
	@param   
	@return  
	*/
	virtual int		   GetSoulIDByName(std::string name) = 0;

	/** 获取提纯成功率
	@param   
	@param   
	@return  
	*/
	virtual int		   GetSmeltSoulProbability(int nCount) = 0;

	
	/** 
	@param   
	@param   
	@return  
	*/
	virtual std::list<SItemProp>   GetItemPropList(int nType) = 0;
	
	/** 
	@param   
	@param   
	@return  
	*/
	virtual std::list<SGoodsType>*   GetGoodsTypeList(int nType, int nItemID) = 0;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual SStallInfo*				GetSchemeStall(int nStallID) = 0;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual const std::string & getResFromId(int id) = 0;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual int		getSoundIDFromId(int id) = 0;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual SCreatureConfig* getCreature(int id) = 0;

	/** 
	@param   
	@param   
	@return  
	*/
	virtual int						GetOnlineShopGoodsCount(int iType, int iSubType) const = 0;

	virtual const SOnlineShopGoods *GetOnlineShopGoodsByID(int iGoodsID) = 0;

	virtual void GetOnlineShopGoodsStart(void) = 0;
	virtual const SOnlineShopGoods * GetOnlineShopNextGoods(int iType, int iSubType) = 0;
	
	virtual bool                    GetRandeMapWeather(DWORD dwMapID, SMapWeather& mapWeather) = 0; 
};

#pragma pack()