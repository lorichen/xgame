/*******************************************************************
** 文件名:	e:\Rocket\Server\EntityServer\Src\DEntityProp.h
** 版  权:	(C) 
** 
** 日  期:	2008/1/12  15:58
** 版  本:	1.0
** 描  述:	实体属性定义
** 应  用:  	
	
**************************** 修改记录 *******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
/*
实体属性处理是比较麻烦的。
一、未来需要添加新属性或者删除属性。有种解决方案就是使用属性集，动态组合。
	使用属性集效率不高，并且需要不停的new内存，不太好用，现在我们的游戏策
	划能在初期就固定属性，未来要添加或者删除的机会又不多。所以放弃属性集。
二、每个实体的属性要分广播属性（同一个九宫格里的终端都可见）、私有属性（只
	有自已终端可见）、其他属性（只放在服务器端，不发给终端）或数据库属性（
	存数据库），如果使用属性集，每次更新，都要去轮询一片，效率不高。像现在
	的方案，如果要取得某个实体的所有广播属性，直接copy前面一段数据即可，很快。
三、尤其是人物和怪物，因为有很多模块，比如状态和技能，都不会分开处理，此些
	模块都只见生物。虽然他们有共有属性，但也有不同的属性。如果用一套属性ID，
	怪物的属性数组太大，浪费内存。如果用两套属性ID，势必其他模块要分开处理。
	现在采取方案是外部统一，内部分开，到内部处理里，
	通过s_nCreatureID2MonsterIDTable转换。
//*/
#pragma once

//////////////////////////////部件ID定义///////////////////////////
enum
{
	// 部件ID定义规则（ENTITYPART + 应用到实体层 + 功能涵义）
	ENTITYPART_ENTITY_INVALID = 0,		// 无效部件ID
	ENTITYPART_ENTITY_BUFF,				// buff部件
	ENTITYPART_PERSON_BROADCAST,		// 广播部件
	
	ENTITYPART_MONSTER_AI,				// ＡＩ部件	
	ENTITYPART_MONSTER_HOLD,			// 归属部件

	ENTITYPART_CREATURE_MOVE,			// 移动部件	
	ENTITYPART_CREATURE_SKILL,			// 技能部件
	ENTITYPART_CREATURE_COMMON,			// 公共部件	
	ENTITYPART_CREATURE_RIDE,			// 骑乘部件
	ENTITYPART_CREATURE_CONJURE,		// 召唤部件
	ENTITYPART_CREATURE_CAMP,			// 阵营部件
	ENTITYPART_CREATURE_DIE,			// 死亡部件

	ENTITYPART_PERSON_CHAT,				// 聊天部件
	ENTITYPART_PERSON_FREEZE,			// 冷却部件
	ENTITYPART_PERSON_TEAM,				// 组队部件
	ENTITYPART_PERSON_STALL,			// 摆摊部件
	ENTITYPART_PERSON_FRIEND,			// 好友部件
	ENTITYPART_PERSON_PACKET,			// 包裹部件
	ENTITYPART_PERSON_EQUIP,			// 装备部件
	ENTITYPART_PERSON_WARE,				// 仓库部件
	ENTITYPART_PERSON_DIE,				// 死亡部件
	ENTITYPART_PERSON_PKMODE,			// ＰＫ部件	
	ENTITYPART_PERSON_CLAN,				// 氏族部件
	ENTITYPART_PERSON_NATION,			// 国家部件
	ENTITYPART_PERSON_HUN,				// 灵魂部件
	ENTITYPART_PERSON_PET,				// 宠物部件
	ENTITYPART_PERSON_PETWAREHOUSE,		// 宠物仓库部件
	ENTITYPART_PERSON_SHOP,				// 商店部件
    ENTITYPART_PERSON_TASK,				//  任务部件；
	ENTITYPART_PERSON_AMUSEMENT,        // 活动部件；
	ENTITYPART_PERSON_EQUIPSMELTSOUL,  // 装备炼魂部件
	ENTITYPART_PERSON_EQUIPEMBEDGEMS,  // 装备灵石镶嵌部件
	ENTITYPART_PERSON_EQUIPREBUILD,	   // 装备重铸部件	
	ENTITYPART_PERSON_COMPOSE,		   //合成部件  by ldy
	ENTITYPART_PET_INTENSIFY,		   //魂将圣化	by ldy
	ENTITYPART_PERSON_STATISTICS,      // 杀怪统计部件
	ENTITYPART_PERSON_COLLECTION,      // 采集部件

	ENTITYPART_ENTITY_MAXID,			// 最大部件ID
};



//////////////////////生物控制状态ID定义///////////////////////////


enum
{
	CREATURE_CONTROLSTATE_UNKNOW = 0,	// 未知的
	CREATURE_CONTROLSTATE_STAND,		// 站立
	CREATURE_CONTROLSTATE_SIT,			// 坐下
	CREATURE_CONTROLSTATE_RIDE,			// 骑马
	CREATURE_CONTROLSTATE_DIE,			// 死亡
	CREATURE_CONTROLSTATE_JUMP,         // 跳跃
	CREATURE_CONTROLSTATE_FLY,          // 飞行
	CREATURE_CONTROLSTATE_SWITCH,		// 切换服务器
	CREATURE_CONTROLSTATE_STALL,		// 摆摊
	CREATURE_CONTROLSTATE_TRADE,		// 交易
	CREATURE_CONTROLSTATE_CHANGEBODY,   // 变身
};
//by cjl 部件冻结、激活原因
enum
{
	FULL_FREEZE_PART_REASON_CHANGEBODY,		//变身引起的全冻结
	PART_FREEZE_PART_REASON_CHANGEBODY,		//变身引起的部分冻结，如技能部件
};
//end cjl

///////////////////////人物性别////////////////////////////////////
enum
{
	PERSON_SEX_MALE = 0,				// 男性
	PERSON_SEX_FEMALE,					// 女性
};

///////////////////////人物阵营ID////////////////////////////////////
enum
{
	PERSON_CAMP_KNIGHT = 1,				// 侠客
	PERSON_CAMP_FENCIBLE = 2,			// 宋军
	PERSON_CAMP_ONUS     = 3,           // 义军
	PERSON_CAMP_MAXID,
};

///////////////////////人物职业////////////////////////////////////
enum
{
	PERSON_VOCATION_MIN = -1,
	PERSON_VOCATION_WARRIOR,			// 战士
	PERSON_VOCATION_ARCHER,				// 弓手
	PERSON_VOCATION_SORCERER,			// 法师
	PERSON_VOCATION_MAX,				// 最大
};
//////////////////////////////人物属性/////////////////////////////
enum
{	
	CREATURE_PROP_PDBID =1,                                 // 角色数据库ID 

	CREATURE_PROP_SEX,                                      // 性别
	CREATURE_PROP_VOCATION,                                 // 职业
	CREATURE_PROP_LEVEL,                                    // 等级
	CREATURE_PROP_NATION,                                   // 国籍
	CREATURE_PROP_FAMILY,									// 家族
	CREATURE_PROP_ARMY,										// 军团
	CREATURE_PROP_PROVINCE,                                 // 角色的省份
	CREATURE_PROP_CITY,                                     // 角色所在的城市
	CREATURE_PROP_CONSTELLATION,                            // 角色的星座
	CREATURE_PROP_BLOODTYPE,                                // 角色的血型
	CREATURE_PROP_CUR_HP,                                   // 当前生命值  //人物实际(最终)生命上限
	CREATURE_PROP_CUR_MAX_HP,                               // 生命值上限
	CREATURE_PROP_CUR_MP,                                   // 当前法术值
	CREATURE_PROP_CUR_MAX_MP,                               // 法术值上限  //人物实际(最终)内力上限
	CREATURE_PROP_CUR_SP,									// 当前体力值
	CREATURE_PROP_CUR_MAX_SP,								// 体力值上限	
	CREATURE_PROP_CUR_MOVE_SPEED,                           // 当前移动速度
	CREATURE_PROP_CUR_WALK_SPEED,                           // 走路速度
	CREATURE_PROP_CUR_MOVE_STYLE,                           // 当前移动方式

	CREATURE_PROP_CUR_MIN_P_A,                              // 当前最小物理攻击
	CREATURE_PROP_CUR_MAX_P_A,                              // 当前最大物理攻击
	CREATURE_PROP_CUR_MIN_M_A,                              // 当前最小法术攻击
	CREATURE_PROP_CUR_MAX_M_A,                              // 当前最大法术攻击
	CREATURE_PROP_CUR_P_D,                                  // 人物实际(最终)物理防御值
	CREATURE_PROP_CUR_M_D,                                  // 人物实际(最终)法术防御

	CREATURE_PROP_CUR_FATAL,                                // 人物实际(最终)会心值，用于显示
	CREATURE_PROP_CUR_KICK_FATAL,                           // 人物实际(最终)御劲值，用于显示
	CREATURE_PROP_CUR_PRESENT,                              // 人物实际(最终)命中值，用于显示
	CREATURE_PROP_CUR_HEDGE,                                // 人物实际(最终)闪避值,用于显示
	CREATURE_PROP_CUR_FIGHT,                                // 人物实际(最终)战斗力
	CREATURE_PROP_FACEID,                                   // 头像ID
	CREATURE_PROP_IGNO_BLCOK,                               // 是否忽略阻挡
	
	CREATURE_PERSON_BROADCAST,                              // 人物以上为广播属性
	/////////////////////////////////////////////////

	CREATURE_PROP_PURSE_MONEY,                              // 身上金钱
	CREATURE_PROP_PURSE_MONEY_INCOME_CHECK,                 // crr放到身上的金钱，接受收益检查。增加这个ID仅仅是为了不破坏SetNumProp的回调支持，方便防沉迷系统处理金钱收益
															//好处是可以统一在回调的地方统一处理,方便日后其他的扩充
															//坏处是m_nNumProp[CREATURE_PROP_PURSE_MONEY_ENTHRALl_PROTECT]其对应的值暂时无意义
	CREATURE_PROP_PURSE_GOLDINGOT,                          // 身上元宝 公司数据库中的宝币
	CREATURE_PROP_PURSE_JIAOZI,                             // 身上交子
	CREATURE_PROP_USABLEBAG,								// 可用行囊
	CREATURE_PROP_WARE_MONEY,                               // 仓库金钱
	CREATURE_PROP_WARE_LOCK,                                // 仓库是否被锁
	CREATURE_PROP_WARE_lOCKTIME,                            // 锁定时间
	CREATURE_PROP_EXP,                                      // 经验
	CREATURE_PROP_SKILLPOINT,                               // 技能点
	CREATURE_PROP_SKILLPOINT_CONSUME_BRANCH_FIRST,          // 第一个技能分支上消耗的技能点
	CREATURE_PROP_SKILLPOINT_CONSUME_BRANCH_SECOND,         // 第二个技能分支上消耗的技能点
	CREATURE_PROP_SKILL_CONSUMEMP_INC_PER,                  // 所有的技能消耗增加百分比
	CREATURE_PROP_BASE_MAX_HP,                              // 基础生命值上限	
	CREATURE_PROP_BASE_MAX_MP,                              // 基础法术值上限
	CREATURE_PROP_BASE_MAX_SP,                              // 基础体力值上限
	CREATURE_PROP_CUR_ATTACK_SPEED,                         // 人物实际(最终)攻击速度值
	CREATURE_PROP_NORMAL_ATTACK_CONSUMEMP_PER,              // 每次普通攻击都会消耗法力值的百分比
	//CREATURE_PROP_NORMAL_ATTACK_RATE_PER,                 // 普通攻击速度增加百分比
	CREATURE_PROP_MAGIC_BALLOON_NUM,                        // 玩家身上聚集的灵气球的数目

	CREATURE_PROP_ENTHRALL_PROTECT_INCOME_PERCENT,			//防沉迷保护 当前收益百分比,正常情况下为100 ，在线3-5小时为50  5小时候为0

	CREATURE_PERSON_PRIVATE,                                // 人物以上为私有属性
	/////////////////////////////////////////////////

	CREATURE_PROP_PETS_STAR_FIGHT,                          // 所有出征宠物星级提高的战斗力
	CREATURE_PROP_PLUS_PM_SHIELD_ABSORB_REMAIN,             // 护盾剩余吸收伤害值

	CREATURE_PROP_SKILL_DAMAGE_FIRST,                       // 对目标生命值低于某一个与目标自身相关的百分比值时技能和攻击造成的伤害提高百分比，针对“无为”和“直击弱点”常驻型被动技能
	CREATURE_PROP_SKILL_DAMAGE_MACRAZYDAMAGEGETMP,          // 任何法术产生暴击后，都会返还该法术所消耗法力值的X%给施法者

	CREATURE_PROP_GET_MP_BY_HEDGE_PER,                      // 每次闪避都可以恢复生命值上限的百分之多少的生命值
	CREATURE_PROP_PLUS_NORMAL_ATTACK_PER,                   // 普通攻击附带额外攻击力百分比
	CREATURE_PROP_CHANGE_BUFF_TIME_PER,                     // 受到所有减速度，定身，晕眩效果的持续时间增加百分比 
	CREATURE_PROP_IGNORE_MD_PER,                            // 使你的任何法术X%几率无视对方的法术防御
	CREATURE_PROP_DEST_DAMAGE2SRC_PER,                      // 有X%的几率将伤害的100%反射给攻击者（玩家自己不会受到当次伤害）
	CREATURE_PROP_ADDMP_BY_CRAZYDAMAGE_PER,                 // 任何技能造成暴击后补充相当于总法力值X%的法力值，最大恢复不可以超过这个法术消耗法力的80%

	//装备带来的属性
	//装备基础的
	//武器特有的
	CREATURE_PROP_EQUIP_WEAPON_BASE_MIN_P_A,				//武器基础最小物理攻击值			(=武器天生最小物理攻击值 + 强化改变武器天生最小物理攻击值)
	CREATURE_PROP_EQUIP_WEAPON_BASE_MAX_P_A,				//武器基础最大物理攻击值			(=武器天生最大物理攻击值 + 强化改变武器天生最大物理攻击值 )
	CREATURE_PROP_EQUIP_WEAPON_BASE_MIN_M_A,				//武器基础最小法术攻击值			(=武器天生最小法术攻击值 + 强化改变武器天生最小法术攻击值)
	CREATURE_PROP_EQUIP_WEAPON_BASE_MAX_M_A,				//武器基础最大法术攻击值			(=武器天生最大法术攻击值 + 强化改变武器天生最大法术攻击值)

	//其它装备带来的
	CREATURE_PROP_EQUIP_BASE_MIN_P_A,                       //装备基础最小物理攻击值			(=装备天生最小物理攻击值 + 强化改变装备天生最小物理攻击值)
	CREATURE_PROP_EQUIP_BASE_MAX_P_A,                       //装备基础最大物理攻击值			(=装备天生最大物理攻击值 + 强化改变装备天生最大物理攻击值 )
	CREATURE_PROP_EQUIP_BASE_P_D,                           //装备基础物理防御值				(=装备天生物理防御值 + 强化改变装备天生物理防御值)
	CREATURE_PROP_EQUIP_BASE_MIN_M_A,                       //装备基础最小法术攻击值			(=装备天生最小法术攻击值 + 强化改变装备天生最小法术攻击值)
	CREATURE_PROP_EQUIP_BASE_MAX_M_A,                       //装备基础最大法术攻击值			(=装备天生最大法术攻击值 + 强化改变装备天生最大法术攻击值)
	CREATURE_PROP_EQUIP_BASE_M_D,                           //装备基础法术防御值				(=装备天生法术防御值 + 强化改变装备天生法术防御值)

	CREATURE_PROP_EQUIP_BASE_MAX_HP,                        //装备基础最大生命值				(=装备天生最大生命值 + 强化改变装备天生最大生命值)
	CREATURE_PROP_EQUIP_BASE_MAX_MP,                        //装备基础最大内力值				(=装备天生最大内力值 + 强化改变装备天生最大内力值)
	CREATURE_PROP_EQUIP_BASE_MAX_SP,                        //装备基础最大体力值				(=装备天生最大体力值 + 强化改变装备天生最大体力值)
	CREATURE_PROP_EQUIP_BASE_HEDGE,                         //装备基础闪避值					(=装备天生闪避值 + 强化改变装备天生闪避值)
	CREATURE_PROP_EQUIP_BASE_PRESENT,                       //装备基础命中值					(=装备天生命中值 + 强化改变装备天生命中值)
	CREATURE_PROP_EQUIP_BASE_FATAL,                         //装备基础会心值					(=装备天生会心值 + 强化改变装备天生会心值)
	CREATURE_PROP_EQUIP_BASE_KICK_FATAL,                    //装备基础御劲值					(=装备天生御劲值 + 强化改变装备天生御劲值)

	//装备附加的
	CREATURE_PROP_EQUIP_PLUS_MIN_P_A,                       //装备附加改变最小物理攻击值
	CREATURE_PROP_EQUIP_PLUS_MIN_P_A_RATIO,                 //装备附加改变人物最小物理攻击百分比	（万分比）
	CREATURE_PROP_EQUIP_PLUS_MIN_P_A_RATIO_2,               //装备附加改变当前最小物理攻击百分比	（万分比）
	CREATURE_PROP_EQUIP_PLUS_MAX_P_A,                       //装备附加改变最大物理攻击值
	CREATURE_PROP_EQUIP_PLUS_MAX_P_A_RATIO,                 //装备附加改变人物最大物理攻击百分比	（万分比）
	CREATURE_PROP_EQUIP_PLUS_MAX_P_A_RATIO_2,               //装备附加改变当前最大物理攻击百分比	（万分比）
	CREATURE_PROP_EQUIP_PLUS_P_D,                           //装备附加改变物理防御值
	CREATURE_PROP_EQUIP_PLUS_P_D_RATIO,                     //装备附加改变人物物理防御百分比		（万分比）
	CREATURE_PROP_EQUIP_PLUS_P_D_RATIO_2,                   //装备附加改变当前物理防御百分比		（万分比）

	CREATURE_PROP_EQUIP_PLUS_MIN_M_A,                       //装备附加改变最小法术攻击值
	CREATURE_PROP_EQUIP_PLUS_MIN_M_A_RATIO,                 //装备附加改变人物最小法术攻击百分比	（万分比）
	CREATURE_PROP_EQUIP_PLUS_MIN_M_A_RATIO_2,               //装备附加改变当前最小法术攻击百分比	（万分比）
	CREATURE_PROP_EQUIP_PLUS_MAX_M_A,                       //装备附加改变最大法术攻击值
	CREATURE_PROP_EQUIP_PLUS_MAX_M_A_RATIO,                 //装备附加改变人物最大法术攻击百分比	（万分比）
	CREATURE_PROP_EQUIP_PLUS_MAX_M_A_RATIO_2,               //装备附加改变当前最大法术攻击百分比	（万分比）
	CREATURE_PROP_EQUIP_PLUS_M_D,                           //装备附加改变法术防御值
	CREATURE_PROP_EQUIP_PLUS_M_D_RATIO,                     //装备附加改变人物法术防御百分比		（万分比）
	CREATURE_PROP_EQUIP_PLUS_M_D_RATIO_2,                   //装备附加改变当前法术防御百分比		（万分比）

	CREATURE_PROP_EQUIP_PLUS_MAX_HP,                        //装备附加改变最大生命值
	CREATURE_PROP_EQUIP_PLUS_MAX_HP_RATIO,                  //装备附加改变人物最大生命百分比		（万分比）
	CREATURE_PROP_EQUIP_PLUS_MAX_HP_RATIO_2,                //装备附加改变当前最大生命百分比		（万分比）
	CREATURE_PROP_EQUIP_PLUS_MAX_MP,                        //装备附加改变最大内力值
	CREATURE_PROP_EQUIP_PLUS_MAX_MP_RATIO,                  //装备附加改变人物最大内力百分比		（万分比）
	CREATURE_PROP_EQUIP_PLUS_MAX_MP_RATIO_2,                //装备附加改变当前最大内力百分比		（万分比）
	CREATURE_PROP_EQUIP_PLUS_MAX_SP,                        //装备附加改变最大体力值
	CREATURE_PROP_EQUIP_PLUS_MAX_SP_RATIO,                  //装备附加改变人物最大体力百分比		（万分比）
	CREATURE_PROP_EQUIP_PLUS_MAX_SP_RATIO_2,                //装备附加改变当前最大体力百分比		（万分比）

	CREATURE_PROP_EQUIP_PLUS_HP_RECOVER,                    //装备附加生命回复值
	CREATURE_PROP_EQUIP_PLUS_HP_RECOVER_RATIO,              //装备附加改变生命回复百分比			（万分比）
	CREATURE_PROP_EQUIP_PLUS_MP_RECOVER,                    //装备附加内力回复值
	CREATURE_PROP_EQUIP_PLUS_MP_RECOVER_RATIO,              //装备附加改变内力回复百分比			（万分比）
	CREATURE_PROP_EQUIP_PLUS_SP_RECOVER,                    //装备附加体力回复值
	CREATURE_PROP_EQUIP_PLUS_SP_RECOVER_RATIO,              //装备附加改变体力回复百分比			（万分比）

	CREATURE_PROP_EQUIP_PLUS_HEDGE,                         //装备附加闪避值
	CREATURE_PROP_EQUIP_PLUS_HEDGE_RATIO,                   //装备附加改变闪避率
	CREATURE_PROP_EQUIP_PLUS_PRESENT,                       //装备附加命中值
	CREATURE_PROP_EQUIP_PLUS_PRESENT_RATIO,                 //装备附加改变命中率

	CREATURE_PROP_EQUIP_PLUS_FATAL,                         //装备附加会心值
	CREATURE_PROP_EQUIP_PLUS_FATAL_RATIO,                   //装备附加改变会心率
	CREATURE_PROP_EQUIP_PLUS_KICK_FATAL,                    //装备附加御劲值

	CREATURE_PROP_EQUIP_PLUS_FATAL_DAMAGE_RATIO,            //装备附加会心伤害提高百分比			（万分比)
	CREATURE_PROP_EQUIP_PLUS_KICK_FATAL_DAMAGE_RATIO,       //装备附加会心伤害减免百分比			（万分比)

	CREATURE_PROP_EQUIP_PLUS_ATTACK_SPEED,                  //装备附加攻击速度值
	CREATURE_PROP_EQUIP_PLUS_ATTACK_SPEED_RATIO,            //装备附加改变攻击速度百分比			（万分比）
	CREATURE_PROP_EQUIP_PLUS_MOVE_SPEED,                    //装备附加移动速度值
	CREATURE_PROP_EQUIP_PLUS_MOVE_SPEED_RATIO,              //装备附加改变移动速度百分比			（万分比）

	CREATURE_PROP_EQUIP_PLUS_IGNORE_P_D,                    //装备附加忽略物理防御值
	CREATURE_PROP_EQUIP_PLUS_IGNORE_P_D_RATIO,              //装备附加忽略物理防御百分比			（万分比）
	CREATURE_PROP_EQUIP_PLUS_IGNORE_M_D,                    //装备附加忽略法术防御值
	CREATURE_PROP_EQUIP_PLUS_IGNORE_M_D_RATIO,              //装备附加忽略法术防御百分比			（万分比）

	CREATURE_PROP_EQUIP_PLUS_REDUCE_P_DAMAGE,               //装备附加物理伤害减免值
	CREATURE_PROP_EQUIP_PLUS_REDUCE_P_DAMAGE_RATIO,         //装备附加物理伤害减免百分比			（万分比）
	CREATURE_PROP_EQUIP_PLUS_REDUCE_M_DAMAGE,               //装备附加法术伤害减免值
	CREATURE_PROP_EQUIP_PLUS_REDUCE_M_DAMAGE_RATIO,         //装备附加法术伤害减免百分比			（万分比）

	CREATURE_PROP_EQUIP_PLUS_INCREASE_P_DAMAGE,             //装备附加物理伤害增加值
	CREATURE_PROP_EQUIP_PLUS_INCREASE_P_DAMAGE_RATIO,       //装备附加物理伤害增加百分比			（万分比）
	CREATURE_PROP_EQUIP_PLUS_INCREASE_M_DAMAGE,             //装备附加法术伤害增加值
	CREATURE_PROP_EQUIP_PLUS_INCREASE_M_DAMAGE_RATIO,       //装备附加法术伤害增加百分比			（万分比）
	CREATURE_PROP_EQUIP_PLUS_REBOUND_DAMAGE_RATIO,          //装备附加反弹伤害百分比				（万分比）

	CREATURE_PROP_EQUIP_PLUS_PENETRATE,                     //装备附加穿刺攻击值
	CREATURE_PROP_EQUIP_PLUS_KICK_PENETRATE,                //装备附加穿刺抵抗值
	CREATURE_PROP_EQUIP_PLUS_KICK_PENETRATE_RATIO,          //装备附加穿刺抵抗百分比				（万分比）

	CREATURE_PROP_EQUIP_PLUS_FIGHT,							//装备带来的人物的战斗力

	CREATURE_PROP_CUR_HP_RECOVER,                           //人物实际(最终)生命回复速度
	CREATURE_PROP_CUR_MP_RECOVER,                           //人物实际(最终)内力回复速度
	CREATURE_PROP_CUR_SP_RECOVER,                           //人物实际(最终)体力回复速度

	CREATURE_PROP_CUR_HEDGE_RATIO,                          //人物实际(最终)闪避率
	CREATURE_PROP_CUR_PRESENT_RATIO,                        //人物实际(最终)命中率

	CREATURE_PROP_CUR_FATAL_RATIO,                          //人物实际(最终)会心率
	CREATURE_PROP_CUR_KICK_FATAL_RATIO,                     //人物实际(最终)御劲率

	CREATURE_PROP_CUR_FATAL_DAMAGE_RATIO,                   //人物实际(最终)增加会心伤害比值（万分比)
	CREATURE_PROP_CUR_KICK_FATAL_DAMAGE_RATIO,              //人物实际(最终)抵抗会心伤害比值（万分比)
	
	//CREATURE_PROP_CUR_ATTACK_SPEED,				        //人物实际(最终)攻击速度值

	CREATURE_PROP_CUR_IGNORE_P_D,                           //人物实际(最终)忽略物理防御值
	CREATURE_PROP_CUR_IGNORE_P_D_RATIO,                     //人物实际(最终)忽略物理防御比值（万分比）
	CREATURE_PROP_CUR_IGNORE_M_D,                           //人物实际(最终)忽略法术防御值
	CREATURE_PROP_CUR_IGNORE_M_D_RATIO,                     //人物实际(最终)忽略法术防御比值（万分比）

	CREATURE_PROP_CUR_REDUCE_P_DAMAGE,                      //人物实际(最终)物理伤害减免值
	CREATURE_PROP_CUR_REDUCE_P_DAMAGE_RATIO,                //人物实际(最终)物理伤害减免比值（万分比）
	CREATURE_PROP_CUR_REDUCE_M_DAMAGE,                      //人物实际(最终)法术伤害减免值
	CREATURE_PROP_CUR_REDUCE_M_DAMAGE_RATIO,                //人物实际(最终)法术伤害减免比值（万分比）

	CREATURE_PROP_CUR_INCREASE_P_DAMAGE,                    //人物实际(最终)物理伤害增加值
	CREATURE_PROP_CUR_INCREASE_P_DAMAGE_RATIO,              //人物实际(最终)物理伤害增加比值（万分比）
	CREATURE_PROP_CUR_INCREASE_M_DAMAGE,                    //人物实际(最终)法术伤害增加值
	CREATURE_PROP_CUR_INCREASE_M_DAMAGE_RATIO,              //人物实际(最终)法术伤害增加比值（万分比）
	CREATURE_PROP_CUR_REBOUND_DAMAGE_RATIO,                 // 人物实际(最终)反弹伤害百分比

	CREATURE_PROP_CUR_PENETRATE,                            //人物实际(最终)穿刺（绝对)伤害值
	CREATURE_PROP_CUR_KICK_PENETRATE,                       //人物实际(最终)抵抗穿刺（抵抗绝对)伤害值
	CREATURE_PROP_CUR_KICK_PENETRATE_RATIO,                 //人物实际(最终)抵抗穿刺（抵抗绝对)伤害比值（万分比）

	//技能和BUFF（非常驻被动技能除外）带来的属性加成

	//对武器起作用的buff
	CREATURE_PROP_SKILL_WEAPON_MIN_P_A_RATIO,               //技能和BUFF（非常驻被动技能除外）带来的增加武器的物理攻击最小比值（万分比）
	CREATURE_PROP_SKILL_WEAPON_MAX_P_A_RATIO,               //技能和BUFF（非常驻被动技能除外）带来的增加武器的物理攻击最大比值（万分比）
	CREATURE_PROP_SKILL_WEAPON_MIN_M_A_RATIO,               //技能和BUFF（非常驻被动技能除外）带来的增加武器的法术攻击最小比值（万分比）
	CREATURE_PROP_SKILL_WEAPON_MAX_M_A_RATIO,               //技能和BUFF（非常驻被动技能除外）带来的增加武器的法术攻击最大比值（万分比）

	//物理/法术 的防防御/攻击 跟自身相关的
	CREATURE_PROP_SKILL_BASE_MIN_P_A,                       //技能和BUFF（非常驻被动技能除外）带来的增加自身的物理攻击最小值
	CREATURE_PROP_SKILL_BASE_MIN_P_A_RATIO,                 //技能和BUFF（非常驻被动技能除外）带来的增加自身的物理攻击最小比值（万分比）
	CREATURE_PROP_SKILL_BASE_MAX_P_A,                       //技能和BUFF（非常驻被动技能除外）带来的增加自身的物理攻击最大值
	CREATURE_PROP_SKILL_BASE_MAX_P_A_RATIO,                 //技能和BUFF（非常驻被动技能除外）带来的增加自身的物理攻击最大比值（万分比）

	CREATURE_PROP_SKILL_BASE_MIN_M_A,                       //技能和BUFF（非常驻被动技能除外）带来的增加自身的法术攻击最小值
	CREATURE_PROP_SKILL_BASE_MIN_M_A_RATIO,                 //技能和BUFF（非常驻被动技能除外）带来的增加自身的法术攻击最小比值（万分比）
	CREATURE_PROP_SKILL_BASE_MAX_M_A,                       //技能和BUFF（非常驻被动技能除外）带来的增加自身的法术攻击最大值
	CREATURE_PROP_SKILL_BASE_MAX_M_A_RATIO,	                //技能和BUFF（非常驻被动技能除外）带来的增加自身的法术攻击最大比值（万分比）

	CREATURE_PROP_SKILL_BASE_P_D,                           //技能和BUFF（非常驻被动技能除外）带来的增加自身的物理防御值
	CREATURE_PROP_SKILL_BASE_P_D_RATIO,                     //技能和BUFF（非常驻被动技能除外）带来的增加自身的物理防御比值（万分比）

	CREATURE_PROP_SKILL_BASE_M_D,                           //技能和BUFF（非常驻被动技能除外）带来的增加自身的法术防御值
	CREATURE_PROP_SKILL_BASE_M_D_RATIO,                     //技能和BUFF（非常驻被动技能除外）带来的增加自身的法术防御比值（万分比）

	//物理/法术 的防防御/攻击 跟当前相关的
	CREATURE_PROP_SKILL_SUM_MIN_P_A_RATIO,                  //技能和BUFF（非常驻被动技能除外）带来的增加当前的物理攻击最小比值（万分比）
	CREATURE_PROP_SKILL_SUM_MAX_P_A_RATIO,                  //技能和BUFF（非常驻被动技能除外）带来的增加当前的物理攻击最大比值（万分比）
	CREATURE_PROP_SKILL_SUM_MIN_M_A_RATIO,                  //技能和BUFF（非常驻被动技能除外）带来的增加当前的法术攻击最小比值（万分比）
	CREATURE_PROP_SKILL_SUM_MAX_M_A_RATIO,                  //技能和BUFF（非常驻被动技能除外）带来的增加当前的法术攻击最大比值（万分比）
	CREATURE_PROP_SKILL_SUM_P_D_RATIO,                      //技能和BUFF（非常驻被动技能除外）带来的增加当前的物理防御比值（万分比）
	CREATURE_PROP_SKILL_SUM_M_D_RATIO,                      //技能和BUFF（非常驻被动技能除外）带来的增加当前的法术防御比值（万分比）

	//对HP/MP相关的
	CREATURE_PROP_SKILL_BASE_MAX_HP,                        //技能和BUFF（非常驻被动技能除外）带来的增加人物自身最大生命值
	CREATURE_PROP_SKILL_BASE_MAX_HP_RATIO,                  //技能和BUFF（非常驻被动技能除外）带来的增加人物自身最大生命比值（万分比）
	CREATURE_PROP_SKILL_BASE_MAX_MP,                        //技能和BUFF（非常驻被动技能除外）带来的增加人物自身最大内力值
	CREATURE_PROP_SKILL_BASE_MAX_MP_RATIO,                  //技能和BUFF（非常驻被动技能除外）带来的增加人物自身最大内力比值（万分比）
	CREATURE_PROP_SKILL_BASE_MAX_SP,                        //技能和BUFF（非常驻被动技能除外）带来的增加人物自身最大体力值
	CREATURE_PROP_SKILL_BASE_MAX_SP_RATIO,                  //技能和BUFF（非常驻被动技能除外）带来的增加人物自身最大体力比值（万分比）
	CREATURE_PROP_SKILL_SUM_MAX_HP_RATIO,                   //技能和BUFF（非常驻被动技能除外）带来的增加人物当前的最大生命比值（万分比）
	CREATURE_PROP_SKILL_SUM_MAX_MP_RATIO,                   //技能和BUFF（非常驻被动技能除外）带来的增加人物当前的最大内力比值（万分比）
	CREATURE_PROP_SKILL_SUM_MAX_SP_RATIO,                   //技能和BUFF（非常驻被动技能除外）带来的增加人物当前的最大体力比值（万分比）

	CREATURE_PROP_SKILL_HP_RECOVER,                         //技能和BUFF（非常驻被动技能除外）带来的增加生命回复速度值
	CREATURE_PROP_SKILL_HP_RECOVER_RATIO,                   //技能和BUFF（非常驻被动技能除外）带来的增加生命回复速度比值（万分比）
	CREATURE_PROP_SKILL_MP_RECOVER,                         //技能和BUFF（非常驻被动技能除外）带来的增加内力回复速度值
	CREATURE_PROP_SKILL_MP_RECOVER_RATIO,                   //技能和BUFF（非常驻被动技能除外）带来的增加内力回复速度比值（万分比）
	CREATURE_PROP_SKILL_SP_RECOVER,                         //技能和BUFF（非常驻被动技能除外）带来的增加体力回复速度值
	CREATURE_PROP_SKILL_SP_RECOVER_RATIO,                   //技能和BUFF（非常驻被动技能除外）带来的增加体力回复速度比值（万分比）

	CREATURE_PROP_SKILL_HEDGE,                              //技能和BUFF（非常驻被动技能除外）带来的增加闪避值
	CREATURE_PROP_SKILL_HEDGE_RATIO,                        //技能和BUFF（非常驻被动技能除外）带来的增加闪避率
	CREATURE_PROP_SKILL_PRESENT,                            //技能和BUFF（非常驻被动技能除外）带来的增加命中值
	CREATURE_PROP_SKILL_PRESENT_RATIO,                      //技能和BUFF（非常驻被动技能除外）带来的增加命中率

	CREATURE_PROP_SKILL_FATAL,                              //技能和BUFF（非常驻被动技能除外）带来的增加会心值
	CREATURE_PROP_SKILL_FATAL_RATIO,                        //技能和BUFF（非常驻被动技能除外）带来的增加会心率
	CREATURE_PROP_SKILL_KICK_FATAL,                         //技能和BUFF（非常驻被动技能除外）带来的增加御劲值

	CREATURE_PROP_SKILL_FATAL_DAMAGE_RATIO,                 //技能和BUFF（非常驻被动技能除外）带来的增加会心伤害比值（万分比)
	CREATURE_PROP_SKILL_KICK_FATAL_DAMAGE_RATIO,            //技能和BUFF（非常驻被动技能除外）带来的抵抗会心伤害比值（万分比)

	CREATURE_PROP_SKILL_ATTACK_SPEED,                       //技能和BUFF（非常驻被动技能除外）带来的攻击速度值
	CREATURE_PROP_SKILL_ATTACK_SPEED_RATIO,                 //技能和BUFF（非常驻被动技能除外）带来的攻击速度比值（万分比）
	CREATURE_PROP_SKILL_MOVE_SPEED,                         //技能和BUFF（非常驻被动技能除外）带来的移动速度
	CREATURE_PROP_SKILL_MOVE_SPEED_RATIO,                   //技能和BUFF（非常驻被动技能除外）带来的移动速度比值（万分比）

	CREATURE_PROP_SKILL_IGNORE_P_D,                         //技能和BUFF（非常驻被动技能除外）带来的忽略物理防御值
	CREATURE_PROP_SKILL_IGNORE_P_D_RATIO,                   //技能和BUFF（非常驻被动技能除外）带来的忽略物理防御比值（万分比）
	CREATURE_PROP_SKILL_IGNORE_M_D,                         //技能和BUFF（非常驻被动技能除外）带来的忽略法术防御值
	CREATURE_PROP_SKILL_IGNORE_M_D_RATIO,                   //技能和BUFF（非常驻被动技能除外）带来的忽略法术防御比值（万分比）

	CREATURE_PROP_SKILL_REDUCE_P_DAMAGE,                    //技能和BUFF（包括常驻被动技能）带来的物理伤害减免值
	CREATURE_PROP_SKILL_REDUCE_P_DAMAGE_RATIO,              //技能和BUFF（包括常驻被动技能）带来的物理伤害减免比值（万分比）
	CREATURE_PROP_SKILL_REDUCE_M_DAMAGE,                    //技能和BUFF（包括常驻被动技能）带来的法术伤害减免值
	CREATURE_PROP_SKILL_REDUCE_M_DAMAGE_RATIO,              //技能和BUFF（包括常驻被动技能）带来的法术伤害减免比值（万分比）

	CREATURE_PROP_SKILL_INCREASE_P_DAMAGE,                  //技能和BUFF（包括常驻被动技能）带来的物理伤害增加值
	CREATURE_PROP_SKILL_INCREASE_P_DAMAGE_RATIO,            //技能和BUFF（包括常驻被动技能）带来的物理伤害增加比值（万分比）
	CREATURE_PROP_SKILL_INCREASE_M_DAMAGE,                  //技能和BUFF（包括常驻被动技能）带来的法术伤害增加值
	CREATURE_PROP_SKILL_INCREASE_M_DAMAGE_RATIO,            //技能和BUFF（包括常驻被动技能）带来的法术伤害增加比值（万分比）
	CREATURE_PROP_SKILL_REBOUND_DAMAGE_RATIO,               // 技能和BUFF（包括常驻被动技能）带来的反弹伤害百分比

	CREATURE_PROP_SKILL_PENETRATE,                          //技能和BUFF（非常驻被动技能除外）带来的穿刺（绝对)伤害值
	CREATURE_PROP_SKILL_KICK_PENETRATE,                     //技能和BUFF（非常驻被动技能除外）带来的抵抗穿刺（抵抗绝对)伤害值
	CREATURE_PROP_SKILL_KICK_PENETRATE_RATIO,               //技能和BUFF（非常驻被动技能除外）带来的抵抗穿刺（抵抗绝对)伤害比值（万分比）

	CREATURE_PROP_SKILLREBOUND,                             //技能和BUFF（非常驻被动技能除外）带来的伤害反弹比值（万分比）

	//物理/法术 的防御/攻击 额外改变值
	CREATURE_PROP_PLUS_MIN_P_A,                             // 额外改变最小物理攻击值
	CREATURE_PROP_PLUS_MAX_P_A,                             // 额外改变最大物理攻击值
	CREATURE_PROP_PLUS_MIN_M_A,                             // 额外改变最小法术攻击值
	CREATURE_PROP_PLUS_MAX_M_A,                             // 额外改变最大法术攻击值
	CREATURE_PROP_PLUS_P_D,                                 // 额外改变物理防御值
	CREATURE_PROP_PLUS_M_D,                                 // 额外改变法术防御值

	//常驻技能技能带来的属性加成
	CREATURE_PROP_PERMANENT_SKILL_BASE_MIN_P_A,				//常驻技能带来的增加自身的物理攻击最小值
	CREATURE_PROP_PERMANENT_SKILL_BASE_MIN_P_A_RATIO,       //常驻技能带来的增加自身的物理攻击最小比值（万分比）
	CREATURE_PROP_PERMANENT_SKILL_BASE_MAX_P_A,				//常驻技能带来的增加自身的物理攻击最大值
	CREATURE_PROP_PERMANENT_SKILL_BASE_MAX_P_A_RATIO,		//常驻技能带来的增加自身的物理攻击最大比值（万分比）

	CREATURE_PROP_PERMANENT_SKILL_BASE_MIN_M_A,				//常驻技能带来的增加自身的法术攻击最小值
	CREATURE_PROP_PERMANENT_SKILL_BASE_MIN_M_A_RATIO,		//常驻技能带来的增加自身的法术攻击最小比值（万分比）
	CREATURE_PROP_PERMANENT_SKILL_BASE_MAX_M_A,				//常驻技能带来的增加自身的法术攻击最大值
	CREATURE_PROP_PERMANENT_SKILL_BASE_MAX_M_A_RATIO,		//常驻技能带来的增加自身的法术攻击最大比值（万分比）

	CREATURE_PROP_PERMANENT_SKILL_BASE_P_D,					//常驻技能带来的增加自身的物理防御值
	CREATURE_PROP_PERMANENT_SKILL_BASE_P_D_RATIO,			//常驻技能带来的增加自身的物理防御比值（万分比）

	CREATURE_PROP_PERMANENT_SKILL_BASE_M_D,					//常驻技能带来的增加自身的法术防御值
	CREATURE_PROP_PERMANENT_SKILL_BASE_M_D_RATIO,			//常驻技能带来的增加自身的法术防御比值（万分比）

	CREATURE_PROP_PERMANENT_SKILL_BASE_MAX_HP,				//常驻技能带来的增加最大生命值
	CREATURE_PROP_PERMANENT_SKILL_BASE_MAX_HP_RATIO,		//常驻技能带来的增加最大生命比值（万分比）
	CREATURE_PROP_PERMANENT_SKILL_BASE_MAX_MP,				//常驻技能带来的增加最大内力值
	CREATURE_PROP_PERMANENT_SKILL_BASE_MAX_MP_RATIO,		//常驻技能带来的增加最大内力比值（万分比）
	CREATURE_PROP_PERMANENT_SKILL_BASE_MAX_SP,				//常驻技能带来的增加最大体力值
	CREATURE_PROP_PERMANENT_SKILL_BASE_MAX_SP_RATIO,		//常驻技能带来的增加最大体力比值（万分比）

	CREATURE_PROP_PERMANENT_SKILL_BASE_HP_RECOVER,			//常驻技能带来的增加生命回复速度值
	CREATURE_PROP_PERMANENT_SKILL_BASE_HP_RECOVER_RATIO,	//常驻技能带来的增加生命回复速度比值（万分比）
	CREATURE_PROP_PERMANENT_SKILL_BASE_MP_RECOVER,			//常驻技能带来的增加内力回复速度值
	CREATURE_PROP_PERMANENT_SKILL_BASE_MP_RECOVER_RATIO,	//常驻技能带来的增加内力回复速度比值（万分比）
	CREATURE_PROP_PERMANENT_SKILL_BASE_SP_RECOVER,			//常驻技能带来的增加体力回复速度值
	CREATURE_PROP_PERMANENT_SKILL_BASE_SP_RECOVER_RATIO,	//常驻技能带来的增加体力回复速度比值（万分比）

	CREATURE_PROP_PERMANENT_SKILL_BASE_HEDGE,				//常驻技能带来的增加闪避值
	CREATURE_PROP_PERMANENT_SKILL_BASE_HEDGE_RATIO,			//常驻技能带来的增加闪避率
	CREATURE_PROP_PERMANENT_SKILL_BASE_PRESENT,				//常驻技能带来的增加命中值
	CREATURE_PROP_PERMANENT_SKILL_BASE_PRESENT_RATIO,		//常驻技能带来的增加命中率

	CREATURE_PROP_PERMANENT_SKILL_BASE_FATAL,				//常驻技能带来的增加会心值
	CREATURE_PROP_PERMANENT_SKILL_BASE_FATAL_RATIO,			//常驻技能带来的增加会心率
	CREATURE_PROP_PERMANENT_SKILL_BASE_KICK_FATAL,			//常驻技能带来的增加御劲值

	CREATURE_PROP_PERMANENT_SKILL_ATTACK_SPEED,				//常驻技能带来的攻击速度值
	CREATURE_PROP_PERMANENT_SKILL_ATTACK_SPEED_RATIO,		//常驻技能带来的攻击速度比值（万分比）
	CREATURE_PROP_PERMANENT_SKILL_MOVE_SPEED,				//常驻技能带来的移动速度
	CREATURE_PROP_PERMANENT_SKILL_MOVE_SPEED_RATIO,			//常驻技能带来的移动速度比值（万分比）

	CREATURE_PROP_SUCK,										//吸血万分比 add by zjp
	CREATURE_PROP_SKILL_KICK_FATAL_RATIO,					//技能改变人物的御劲率 
	CREATURE_PROP_SKILL_FAN_TANG_RATIO,						//技能反弹伤害万分比

	CREATURE_PROP_VIPPOPEDOM,                               // VIP权限

	CREATURE_MONSTER_ID,                                    // 宠物，怪物ID ********* （宠物，怪物专用）

	CREATURE_PROP_ZQ_ACCOUNT_ID,							//中青通行证，在公司数据库中的帐号ID
	//by cjl
	CREATURE_PROP_EQUIP_ADATP_NUM,							//装备适合的个数
	CREATURE_PROP_EQUIP_ADATP_FIGHT,						//装备适合的战斗力加成
	CREATURE_PROP_EQUIP_BRAND_NUM,							//装备品质加成的个数
	CREATURE_PROP_EQUIP_BRAND_FIGHT,						//装备品质加成的战斗力加成
	CREATURE_PROP_EQUIP_SLOTQTY_NUM,						//装备带洞的个数
	CREATURE_PROP_EQUIP_SLOTQTY_FIGHT,						//装备洞数的战斗力加成
	CREATURE_PROP_EQUIP_STRENGTH_NUM,						//装备强化的个数
	CREATURE_PROP_EQUIP_STRENGTH_FIGHT,						//装备强化的战斗力加成
	//end cjl

	/* 1、如果有大小比，最小值ID必须排在最大值ID前一位。
	2、只有当前值，才需要大小比。
	3、当前值，一般都是禁止修改的，因为都是基本值和附加值计算出来的，要改就改附加值就好了。
	4、附加值，都是允许为负数的，因为BUFF为数值有加有减的。//*/

	CREATURE_PROP_MAXID,                                    // 最大属性ID	
};

//by cjl
///////////////////////人物属性表对应的偏移量///////////////////////
enum
{
	CREATURE_PROP_OFFSET_PDBID = CREATURE_PROP_PDBID+1,										// 角色数据库ID ，开始
	CREATURE_PROP_OFFSET_BROADCAST = CREATURE_PERSON_BROADCAST-1,							// 人物以上为广播属性 ，结束
	
	CREATURE_PROP_OFFSET_MAX_HP = CREATURE_PROP_BASE_MAX_HP,								// 基础生命值上限 ， 开始
	CREATURE_PROP_OFFSET_CUR_ATTACK_SPEED = CREATURE_PROP_CUR_ATTACK_SPEED,					// 人物实际(最终)攻击速度值， 结束
	
	CREATURE_PROP_OFFSET_PERSON_PRIVATE = CREATURE_PERSON_PRIVATE+1,						// 人物以上为私有属性，开始
	CREATURE_PROP_OFFSET_ADDMP_BY_CRAZYDAMAGE_PE = CREATURE_PROP_ADDMP_BY_CRAZYDAMAGE_PER,  // 任何技能造成暴击后补充相当于总法力值X%的法力值，最大恢复不可以超过这个法术消耗法力的80%,结束
	
	CREATURE_PROP_OFFSET_EQUIP_PLUS_FIGHT = CREATURE_PROP_EQUIP_PLUS_FIGHT,					//装备带来的人物的战斗力，开始
	CREATURE_PROP_OFFSET_CUR_KICK_PENETRATE_RATIO = CREATURE_PROP_CUR_KICK_PENETRATE_RATIO, //人物实际(最终)抵抗穿刺（抵抗绝对)伤害比值（万分比）， 结束

	CREATURE_PROP_OFFSET_PLUS_MIN_P_A = CREATURE_PROP_PLUS_MIN_P_A,                         // 额外改变最小物理攻击值，开始
	CREATURE_PROP_OFFSET_PLUS_M_D = CREATURE_PROP_PLUS_M_D,									// 额外改变法术防御值， 结束
};	

/////////////////////////////人物其他属性///////////////////////////
enum//更新其他属性使用
{
	UPDATE_MY_PRESENT_PROP = 0,								// 新手礼包相关属性,进入场景时候发送
	UPDATE_MY_PRESENT_RESULT,								// 更新礼包结果
	UPDATE_MY_FIGHT_DISPLAY,								// 战斗力显示数据
	UPDATE_MY_CHANGE_BODY_DATA,								// 变身外貌数据
	UPDATE_MY_MAXID,										// 最大属性ID
};
#define PERSON_PRESENT_LEVEL_KEY      "礼包等级"		   //永久绑定
#define PERSON_PRESENT_TOTAL_TIME_KEY "礼包已积累时间"	   //永久绑定
#define PERSON_PRESENT_TIME_KEY       "礼包当前时间"       //临时绑定

enum //do something 使用
{
	PERSON_GET_MY_PRESENT = 0,								//获取礼包
	PERSON_GET_MY_PRESENT_RESULT,                           //获取礼包结果
	PERSON_GET_CHANGEBODY_COMMON_DATA,						//备份并修改普通变身数据
	PERSON_SET_CHANGEBODY_COMMON_DATA,						//设置普通变身数据
	PERSON_GET_CHANGEBODY_DATA,								//备份并修改高级变身数据
	PERSON_SET_CHANGEBODY_DATA,								//设置高级变身数据
	PERSON_SET_CHANGEBODY_DATA_FOR_COPY,					//设置备份数据
	PERSON_DO_SOMETHING_MAX,
};
//end cjl


//////////////////////////////人物装备位置//////////////////////////
enum
{
	PERSON_EQUIPPLACE_RESERVE = 0,		// 保留/无效值
	PERSON_EQUIPPLACE_WEAPON,			// 武器  
	PERSON_EQUIPPLACE_ARMET,			// 头盔
	PERSON_EQUIPPLACE_ARMOR,			// 盔甲
	PERSON_EQUIPPLACE_CUFF,				// 护腕 
	PERSON_EQUIPPLACE_SHOES,			// 鞋子
	PERSON_EQUIPPLACE_NECKLACE,			// 项链
	PERSON_EQUIPPLACE_RING,				// 戒指
	PERSON_EQUIPPLACE_DECORATION_L,		// 左挂坠
	PERSON_EQUIPPLACE_DECORATION_R,		// 右挂坠
	PERSON_EQUIPPLACE_SUIT_ARMET,		// 时装头盔
	PERSON_EQUIPPLACE_SUIT_ARMOR,		// 时装盔甲
	PERSON_EQUIPPLACE_MAX,				// 最大	
};

//////////////////////////////装备损坏原因//////////////////////////////////////
enum
{
	DEDUCT_ENDURE_REASON_MIN = 0,
	DEDUCT_ENDURE_REASON_ATTACK,		//攻击
	DEDUCT_ENDURE_REASON_INJURED,		//受伤
	DEDUCT_ENDURE_REASON_PVE_DIE,		//pve死亡
	DEDUCT_ENDURE_REASON_MAX,			
};

#if 0
////////////////////////////////怪物属性/////////////////////////////
//enum
//{
//	MONSTER_TYPE_GENERAL = 1,			// 普通怪		（只攻击人）
//	MONSTER_TYPE_POLISH,				// 精英怪		（只攻击人）
//	MONSTER_TYPE_RARE,					// 稀有怪		（只攻击人）
//	MONSTER_TYPE_BOSS,					// BOSS怪		（只攻击人）
//
//	MONSTER_TYPE_VALOR,					// 勇猛怪		（人和怪都会攻击）
//
//	MONSTER_TYPE_LIFE,					// 生灵			（小猫、小狗等）
//
//	MONSTER_TYPE_NPCSAFE,				// 不可攻击NPC	（人和怪都不会攻击）
//	MONSTER_TYPE_NPCATTACK,				// 可攻击NPC	（只攻击怪）
//
//	MONSTER_TYPE_GUARD,					// 守卫			（攻击红名和非阵营的人和怪）
//
//	MONSTER_TYPE_PAWNLUO,				// 爪牙型召唤兽 （可同时拥有多个）
//	MONSTER_TYPE_PAWNPET,				// 宠物型召唤兽	（只允许拥有一个，有固定目标环）
//
//	MONSTER_TYPE_GUYONG,				// 雇佣怪		（跟勇猛怪没区别，只是由AI召出）
//
//	MONSTER_TYPE_STALL,				    // 摊位NPC		（用于摆摊）
//};
#endif

//////////////////////////////怪物属性/////////////////////////////
enum
{
/*  勇猛怪
	1．	可以攻击人、宠物、怪物。
	2．	可以被人、宠物、怪物攻击。
	3．	鼠标移动到该目标上根据阵营仇恨判断：如果中立或仇恨显示“刀”图标，如果友好显示“箭头”图标
*/
	MONSTER_TYPE_VALOR = 1,

/*  BOSS怪
	1．	可以攻击人、宠物、怪物。
	2．	可以被人、宠物、怪物攻击。
	3．	鼠标移动到该目标上根据阵营仇恨判断：如果中立或仇恨显示“刀”图标，如果友好显示“箭头”图标

*/
	MONSTER_TYPE_BOSS,

/*  可攻击NPC
	1．	可以攻击人、宠物、怪物。
	2．	可以被人、宠物、怪物攻击。
	3．鼠标移动到该目标上根据阵营仇恨判断：如果仇恨显示“刀”图标，如果中立或友好显示“对话”图标
*/
	MONSTER_TYPE_NPCATTACK,

/*  守卫
	1．可以攻击人、宠物、怪物。
	2．可以被人、宠物、怪物攻击。
	3．必定攻击红名玩家。
	4．人和宠物不能主动攻击守卫，只能在守卫攻击后反击。
	5．鼠标移动到该目标上显示“箭头”图标，受到守卫攻击后显示“刀”图标
*/
	MONSTER_TYPE_GUARD,

/*  生灵（小猫、小狗等）
	1．	不可以攻击人、宠物、怪物。
	2．	不可以被人、宠物、怪物攻击。
	3．	鼠标移动到该目标上无法选中。
*/
	MONSTER_TYPE_LIFE,

/*  不可攻击NPC
	1．不可以攻击人、宠物、怪物。
	2．不可以被人、宠物、怪物攻击。
	3．鼠标移动到该目标上显示“对话”图标
*/
	MONSTER_TYPE_NPCSAFE,

/* 采集NPC
	1．不可以攻击人、宠物、怪物。
	2．不可以被人、宠物、怪物攻击。
	3．鼠标移动到该目标上显示“采集”图标
*/
	MONSTER_TYPE_NPCCOLLECTION,

// 摊位NPC		（用于摆摊）
	MONSTER_TYPE_STALL,

/* 精英怪，同boss
	1．不可以攻击人、宠物、怪物。
	2．不可以被人、宠物、怪物攻击。
	3．鼠标移动到该目标上显示“采集”图标
*/
	MONSTER_TYPE_ELITE,	
};

////////////////////////////////////////////////////////////////////
// 请不要直接用这套属性ID去操作怪物属性，用CREATURE_PROP_CUR_HP此类
// 的ID。怪物实体内部会自动转换，这套ID怪物实体外部根据涉及不到，申
// 明在此处，主要是客户端也要使用，并且让外部了解怪物支持哪些属性。
///////////////////////////////////////////////////////////////////
/*****************************************************************/
/******** 请不要直接用这套属性ID去操作怪物属性 *******************/
enum
{
	MONSTER_PROP_MONSTERID = 1,                    // 怪物ID
	MONSTER_PROP_CUR_HP,                           // 当前生命值
	MONSTER_PROP_MAX_HP,                           // 生命值上限
	MONSTER_PROP_CUR_MP,                           // 当前法术值
	MONSTER_PROP_MAX_MP,                           // 法术值上限
	MONSTER_PROP_CUR_MOVE_SPEED,                   // 当前移动速度
	MONSTER_PROP_CUR_WALK_SPEED,                   // 当前走路速度
	MONSTER_PROP_CUR_MOVE_STYLE,                   // 当前移动方式
	MONSTER_PROP_IGNO_BLCOK,                       // 是否忽略阻挡

	MONSTER_PROP_BROADCAST,                        // 以上为广播属性
	/////////////////////////////////////////////////

	MONSTER_PROP_CUR_MIN_P_A,                      // 当前最小物理攻击
	MONSTER_PROP_CUR_MAX_P_A,                      // 当前最大物理攻击
	MONSTER_PROP_CUR_MIN_M_A,                      // 当前最小法术攻击
	MONSTER_PROP_CUR_MAX_M_A,                      // 当前最大法术攻击

	MONSTER_PROP_CUR_P_D,                          // 当前物理防御 
	MONSTER_PROP_CUR_M_D,                          // 当前法术防御

	//怪物实际(最终)属性值
	MONSTER_PROP_LEVEL,                            // 怪物等级
	MONSTER_PROP_CUR_FIGHT,                        // 怪物实际(最终战斗力)
	MONSTER_PROP_CUR_HEDGE,                        // 怪物实际(最终)闪避值,用于显示
	MONSTER_PROP_CUR_HEDGE_RATIO,                  // 怪物实际(最终)闪避率
	MONSTER_PROP_CUR_PRESENT,                      // 怪物实际(最终)命中值，用于显示
	MONSTER_PROP_CUR_PRESENT_RATIO,                // 怪物实际(最终)命中率

	MONSTER_PROP_CUR_FATAL,                        // 怪物实际(最终)会心值，用于显示
	MONSTER_PROP_CUR_FATAL_RATIO,                  // 怪物实际(最终)会心率
	MONSTER_PROP_CUR_KICK_FATAL,                   // 怪物实际(最终)御劲值，用于显示
	MONSTER_PROP_CUR_KICK_FATAL_RATIO,             // 怪物实际(最终)御劲率

	MONSTER_PROP_CUR_FATAL_DAMAGE_RATIO,           // 怪物实际(最终)增加会心伤害比值（万分比)
	MONSTER_PROP_CUR_KICK_FATAL_DAMAGE_RATIO,      // 怪物实际(最终)抵抗会心伤害比值（万分比)

	MONSTER_PROP_CUR_ATTACK_SPEED,                 // 怪物实际(最终)攻击速度值

	MONSTER_PROP_CUR_IGNORE_P_D,                   // 怪物实际(最终)忽略物理防御值
	MONSTER_PROP_CUR_IGNORE_P_D_RATIO,             // 怪物实际(最终)忽略物理防御比值（万分比）
	MONSTER_PROP_CUR_IGNORE_M_D,                   // 怪物实际(最终)忽略法术防御值
	MONSTER_PROP_CUR_IGNORE_M_D_RATIO,             // 怪物实际(最终)忽略法术防御比值（万分比）

	MONSTER_PROP_CUR_REDUCE_P_DAMAGE,              // 怪物实际(最终)物理伤害减免值
	MONSTER_PROP_CUR_REDUCE_P_DAMAGE_RATIO,        // 怪物实际(最终)物理伤害减免比值（万分比）
	MONSTER_PROP_CUR_REDUCE_M_DAMAGE,              // 怪物实际(最终)法术伤害减免值
	MONSTER_PROP_CUR_REDUCE_M_DAMAGE_RATIO,        // 怪物实际(最终)法术伤害减免比值（万分比）

	MONSTER_PROP_CUR_INCREASE_P_DAMAGE,            // 怪物实际(最终)物理伤害增加值
	MONSTER_PROP_CUR_INCREASE_P_DAMAGE_RATIO,      // 怪物实际(最终)物理伤害增加比值（万分比）
	MONSTER_PROP_CUR_INCREASE_M_DAMAGE,            // 怪物实际(最终)法术伤害增加值
	MONSTER_PROP_CUR_INCREASE_M_DAMAGE_RATIO,      // 怪物实际(最终)法术伤害增加比值（万分比）

	// 反弹伤害百分比
	MONSTER_PROP_CUR_REBOUND_DAMAGE_RATIO,         // 怪物实际(最终)反弹伤害百分比

	MONSTER_PROP_CUR_PENETRATE,                    // 怪物实际(最终)穿刺（绝对)伤害值
	MONSTER_PROP_CUR_KICK_PENETRATE,               // 怪物实际(最终)抵抗穿刺（抵抗绝对)伤害值
	MONSTER_PROP_CUR_KICK_PENETRATE_RATIO,         // 怪物实际(最终)抵抗穿刺（抵抗绝对)伤害比值（万分比）
	MONSTER_PROP_CUR_HP_RECOVER,                   // 怪物实际(最终)生命回复速度

	//技能和BUFF（非常驻被动技能除外）带来的属性加成
	//物理/法术 的防防御/攻击 跟自身相关的
	MONSTER_PROP_SKILL_BASE_MIN_P_A,               // 技能和BUFF（非常驻被动技能除外）带来的增加自身的物理攻击最小值
	MONSTER_PROP_SKILL_BASE_MIN_P_A_RATIO,         // 技能和BUFF（非常驻被动技能除外）带来的增加自身的物理攻击最小比值（万分比）
	MONSTER_PROP_SKILL_BASE_MAX_P_A,               // 技能和BUFF（非常驻被动技能除外）带来的增加自身的物理攻击最大值
	MONSTER_PROP_SKILL_BASE_MAX_P_A_RATIO,         // 技能和BUFF（非常驻被动技能除外）带来的增加自身的物理攻击最大比值（万分比）

	MONSTER_PROP_SKILL_BASE_MIN_M_A,               // 技能和BUFF（非常驻被动技能除外）带来的增加自身的法术攻击最小值
	MONSTER_PROP_SKILL_BASE_MIN_M_A_RATIO,         // 技能和BUFF（非常驻被动技能除外）带来的增加自身的法术攻击最小比值（万分比）
	MONSTER_PROP_SKILL_BASE_MAX_M_A,               // 技能和BUFF（非常驻被动技能除外）带来的增加自身的法术攻击最大值
	MONSTER_PROP_SKILL_BASE_MAX_M_A_RATIO,         // 技能和BUFF（非常驻被动技能除外）带来的增加自身的法术攻击最大比值（万分比）

	MONSTER_PROP_SKILL_BASE_P_D,                   // 技能和BUFF（非常驻被动技能除外）带来的增加自身的物理防御值
	MONSTER_PROP_SKILL_BASE_P_D_RATIO,             // 技能和BUFF（非常驻被动技能除外）带来的增加自身的物理防御比值（万分比）

	MONSTER_PROP_SKILL_BASE_M_D,                   // 技能和BUFF（非常驻被动技能除外）带来的增加自身的法术防御值
	MONSTER_PROP_SKILL_BASE_M_D_RATIO,             // 技能和BUFF（非常驻被动技能除外）带来的增加自身的法术防御比值（万分比）

	//物理/法术 的防防御/攻击 跟当前相关的
	MONSTER_PROP_SKILL_SUM_MIN_P_A_RATIO,          // 技能和BUFF（非常驻被动技能除外）带来的增加当前的物理攻击最小比值（万分比）
	MONSTER_PROP_SKILL_SUM_MAX_P_A_RATIO,          // 技能和BUFF（非常驻被动技能除外）带来的增加当前的物理攻击最大比值（万分比）
	MONSTER_PROP_SKILL_SUM_MIN_M_A_RATIO,          // 技能和BUFF（非常驻被动技能除外）带来的增加当前的法术攻击最小比值（万分比）
	MONSTER_PROP_SKILL_SUM_MAX_M_A_RATIO,          // 技能和BUFF（非常驻被动技能除外）带来的增加当前的法术攻击最大比值（万分比）
	MONSTER_PROP_SKILL_SUM_P_D_RATIO,              // 技能和BUFF（非常驻被动技能除外）带来的增加当前的物理防御比值（万分比）
	MONSTER_PROP_SKILL_SUM_M_D_RATIO,              // 技能和BUFF（非常驻被动技能除外）带来的增加当前的法术防御比值（万分比）

	//对HP/MP相关的
	MONSTER_PROP_SKILL_BASE_MAX_HP,                // 技能和BUFF（非常驻被动技能除外）带来的增加怪物自身最大生命值
	MONSTER_PROP_SKILL_BASE_MAX_HP_RATIO,          // 技能和BUFF（非常驻被动技能除外）带来的增加怪物自身最大生命比值（万分比）
	MONSTER_PROP_SKILL_BASE_MAX_MP,                // 技能和BUFF（非常驻被动技能除外）带来的增加怪物自身最大内力值
	MONSTER_PROP_SKILL_BASE_MAX_MP_RATIO,          // 技能和BUFF（非常驻被动技能除外）带来的增加怪物自身最大内力比值（万分比）
	MONSTER_PROP_SKILL_SUM_MAX_HP_RATIO,           // 技能和BUFF（非常驻被动技能除外）带来的增加怪物当前的最大生命比值（万分比）
	MONSTER_PROP_SKILL_SUM_MAX_MP_RATIO,           // 技能和BUFF（非常驻被动技能除外）带来的增加怪物当前的最大内力比值（万分比）

	MONSTER_PROP_SKILL_HEDGE,                      // 技能和BUFF（非常驻被动技能除外）带来的增加闪避值
	MONSTER_PROP_SKILL_HEDGE_RATIO,                // 技能和BUFF（非常驻被动技能除外）带来的增加闪避率
	MONSTER_PROP_SKILL_PRESENT,                    // 技能和BUFF（非常驻被动技能除外）带来的增加命中值
	MONSTER_PROP_SKILL_PRESENT_RATIO,              // 技能和BUFF（非常驻被动技能除外）带来的增加命中率

	MONSTER_PROP_SKILL_FATAL,                      // 技能和BUFF（非常驻被动技能除外）带来的增加会心值
	MONSTER_PROP_SKILL_FATAL_RATIO,                // 技能和BUFF（非常驻被动技能除外）带来的增加会心率
	MONSTER_PROP_SKILL_KICK_FATAL,                 // 技能和BUFF（非常驻被动技能除外）带来的增加御劲值

	MONSTER_PROP_SKILL_FATAL_DAMAGE_RATIO,         // 技能和BUFF（非常驻被动技能除外）带来的增加会心伤害比值（万分比)
	MONSTER_PROP_SKILL_KICK_FATAL_DAMAGE_RATIO,    // 技能和BUFF（非常驻被动技能除外）带来的抵抗会心伤害比值（万分比)

	MONSTER_PROP_SKILL_ATTACK_SPEED,               // 技能和BUFF（非常驻被动技能除外）带来的攻击速度值
	MONSTER_PROP_SKILL_ATTACK_SPEED_RATIO,         // 技能和BUFF（非常驻被动技能除外）带来的攻击速度比值（万分比）
	MONSTER_PROP_SKILL_MOVE_SPEED,                 // 技能和BUFF（非常驻被动技能除外）带来的移动速度
	MONSTER_PROP_SKILL_MOVE_SPEED_RATIO,           // 技能和BUFF（非常驻被动技能除外）带来的移动速度比值（万分比）

	MONSTER_PROP_SKILL_IGNORE_P_D,                 // 技能和BUFF（非常驻被动技能除外）带来的忽略物理防御值
	MONSTER_PROP_SKILL_IGNORE_P_D_RATIO,           // 技能和BUFF（非常驻被动技能除外）带来的忽略物理防御比值（万分比）
	MONSTER_PROP_SKILL_IGNORE_M_D,                 // 技能和BUFF（非常驻被动技能除外）带来的忽略法术防御值
	MONSTER_PROP_SKILL_IGNORE_M_D_RATIO,           // 技能和BUFF（非常驻被动技能除外）带来的忽略法术防御比值（万分比）

	MONSTER_PROP_SKILL_REDUCE_P_DAMAGE,            // 技能和BUFF（包括常驻被动技能）带来的物理伤害减免值
	MONSTER_PROP_SKILL_REDUCE_P_DAMAGE_RATIO,      // 技能和BUFF（包括常驻被动技能）带来的物理伤害减免比值（万分比）
	MONSTER_PROP_SKILL_REDUCE_M_DAMAGE,            // 技能和BUFF（包括常驻被动技能）带来的法术伤害减免值
	MONSTER_PROP_SKILL_REDUCE_M_DAMAGE_RATIO,      // 技能和BUFF（包括常驻被动技能）带来的法术伤害减免比值（万分比）

	MONSTER_PROP_SKILL_INCREASE_P_DAMAGE,          // 技能和BUFF（包括常驻被动技能）带来的物理伤害增加值
	MONSTER_PROP_SKILL_INCREASE_P_DAMAGE_RATIO,    // 技能和BUFF（包括常驻被动技能）带来的物理伤害增加比值（万分比）
	MONSTER_PROP_SKILL_INCREASE_M_DAMAGE,          // 技能和BUFF（包括常驻被动技能）带来的法术伤害增加值
	MONSTER_PROP_SKILL_INCREASE_M_DAMAGE_RATIO,    // 技能和BUFF（包括常驻被动技能）带来的法术伤害增加比值（万分比）

	// 反弹伤害百分比
	MONSTER_PROP_SKILL_REBOUND_DAMAGE_RATIO,       // 技能和BUFF（包括常驻被动技能）带来的反弹伤害百分比


	MONSTER_PROP_SKILL_PENETRATE,				   // 技能和BUFF（非常驻被动技能除外）带来的穿刺（绝对)伤害值
	MONSTER_PROP_SKILL_KICK_PENETRATE,			   // 技能和BUFF（非常驻被动技能除外）带来的抵抗穿刺（抵抗绝对)伤害值
	MONSTER_PROP_SKILL_KICK_PENETRATE_RATIO,	   // 技能和BUFF（非常驻被动技能除外）带来的抵抗穿刺（抵抗绝对)伤害比值（万分比）

	MONSTER_PROP_SKILLREBOUND,                     // 技能和BUFF（非常驻被动技能除外）带来的伤害反弹比值（万分比）

	//物理/法术 的防御/攻击 额外改变值
	MONSTER_PROP_PLUS_MIN_P_A,                     // 额外改变最小物理攻击值
	MONSTER_PROP_PLUS_MAX_P_A,                     // 额外改变最大物理攻击值
	MONSTER_PROP_PLUS_MIN_M_A,                     // 额外改变最小法术攻击值
	MONSTER_PROP_PLUS_MAX_M_A,                     // 额外改变最大法术攻击值
	MONSTER_PROP_PLUS_P_D,                         // 额外改变物理防御值
	MONSTER_PROP_PLUS_M_D,                         // 额外改变法术防御值

	/* 1、如果有大小比，最小值ID必须排在最大值ID前一位。
	   2、只有当前值，才需要大小比。
	   3、当前值，一般都是禁止修改的，因为都是基本值和附加值计算出来的，要改就改附加值就好了。
	   4、附加值，都是允许为负数的，因为BUFF为数值有加有减的。//*/

	MONSTER_PROP_MAXID,                            // 最大属性ID
};

//
// 宠物属性
// 
enum
{
	PET_PROP_PDBID = 1,				// 宠物数据库ID
	PET_PROP_PETID ,				// 宠物类型ID	
	PET_PROP_SID,					// 身份牌	
	PET_PROP_ELEMENT,				// 五行属性	
	PET_PROP_GENIUS,				// 天赋
	PET_PROP_LEVEL,					// 等级
	PET_PROP_STARS,					// 星级，品质
	PET_PROP_TITLE,					// 头衔，封号	
	PET_PROP_CUR_HP,				// 当前生命值
	PET_PROP_MAX_HP,				// 生命值上限
	PET_PROP_CUR_MP,				// 当前法术值
	PET_PROP_MAX_MP,				// 法术值上限
	PET_PROP_CUR_MOVE_SPEED,		// 当前移动速度
	PET_PROP_CUR_WALK_SPEED,		// 当前走路速度
	PET_PROP_CUR_MOVE_STYLE,        // 当前移动方式
	PET_PROP_DEATH,					// 是否死亡

	PET_PROP_BROADCAST,				// 以上为广播属性
	/////////////////////////////////////////////////
	PET_PROP_BASE_MAX_HP,			// 宠物基础最大生命值

	PET_PROP_RELEASE_STATE,			// 解封或未解封
	PET_PROP_CHUMMY,				// 亲密度
	PET_PROP_CUR_BIRTH_TIMES,		// 转世次数
	PET_PROP_POTENTIAL,				// 潜力值
	PET_PROP_RELEASE_START_TIME,	// 解封时间
	PET_PROP_POSITION,				// 阵法，方位
	PET_PROP_REALIZATION_TIMES,	    // 顿悟次数	
	PET_PROP_EXP,					// 经验

	PET_PROP_CUR_MIN_P_A,			// 当前最小物理攻击
	PET_PROP_CUR_MAX_P_A,			// 当前最大物理攻击

	PET_PROP_CUR_MIN_M_A,			// 当前最小法术攻击
	PET_PROP_CUR_MAX_M_A,			// 当前最大法术攻击

	PET_PROP_CUR_P_D,				// 当前物理防御
	PET_PROP_CUR_M_D,				// 当前法术防御

	PET_PROP_CUR_FATAL_PER,			// 当前暴击概率	
	PET_PROP_CUR_HIT_PER,			// 当前命中概率	
	PET_PROP_CUR_HEDGE_PER,			// 当前闪避概率

	PET_PROP_HP_GROWTH_PER,			// 生命成长率

	PET_PROP_MIN_P_A_GROWTH_PER,	// 最小物理攻击成长率
	PET_PROP_MAX_P_A_GROWTH_PER,	// 最大物理攻击成长率
	PET_PROP_MIN_M_A_GROWTH_PER,	// 最小魔法攻击成长率
	PET_PROP_MAX_M_A_GROWTH_PER,	// 最大魔法攻击成长率
	PET_PROP_P_DEF_GROWTH_PER,		// 物理防御成长率
	PET_PROP_M_DEF_GROWTH_PER,		// 法术防御成长率

	PET_PROP_LUCK,					// 幸运值
	PET_PROP_FEED_STARTTIME,		// 开始喂食
	PET_PROP_SUB_EXP,				// 内丹经验
	PET_PROP_BIND,					// 绑定标志
	PET_PROP_ACTION_STATE,			// 出征，骑乘等行为标志

	PET_PROP_INIT_HP,				// 初始生命值，合体时需要使用
	PET_PROP_INIT_MIN_P_A,			// 初始最小物理攻击，合体时会使用
	PET_PROP_INIT_MAX_P_A,			// 初始最大物理攻击，合体时会使用
	PET_PROP_INIT_MIN_M_A,			// 初始最小法术攻击，合体时会使用
	PET_PROP_INIT_MAX_M_A,			// 初始最大法术攻击，合体时会使用
	PET_PROP_INIT_P_D,				// 初始物理防御，合体时会使用
	PET_PROP_INIT_M_D,				// 初始法术防御，合体时会使用

	PET_PROP_AWORDTIME,				// 获得时间(单位分) 格林尼治时间
	PET_PROP_SANCTIFY,				// 圣化级别 ldy
	PET_PROP_PRIVATE,				// 以上为宠物私有属性
	////////////////////////////////////////////////////////////////
	//宠物实际(最终)属性值
	PET_PROP_CUR_HP_RECOVERY,					//宠物实际（最终）回复速度
	PET_PROP_CUR_FIGHT,                         //宠物实际(最终战斗力)
	PET_PROP_CUR_HEDGE,							//宠物实际(最终)闪避值,用于显示
	PET_PROP_CUR_HEDGE_RATIO,					//宠物实际(最终)闪避率
	PET_PROP_CUR_PRESENT,						//宠物实际(最终)命中值，用于显示
	PET_PROP_CUR_PRESENT_RATIO,					//宠物实际(最终)命中率

	PET_PROP_CUR_FATAL,							//宠物实际(最终)会心值，用于显示
	PET_PROP_CUR_FATAL_RATIO,					//宠物实际(最终)会心率
	PET_PROP_CUR_KICK_FATAL,					//宠物实际(最终)御劲值，用于显示
	PET_PROP_CUR_KICK_FATAL_RATIO,				//宠物实际(最终)御劲率

	PET_PROP_CUR_FATAL_DAMAGE_RATIO,			//宠物实际(最终)增加会心伤害比值（万分比)
	PET_PROP_CUR_KICK_FATAL_DAMAGE_RATIO,		//宠物实际(最终)抵抗会心伤害比值（万分比)

	PET_PROP_CUR_ATTACK_SPEED,					//宠物实际(最终)攻击速度值

	PET_PROP_CUR_IGNORE_P_D,					//宠物实际(最终)忽略物理防御值
	PET_PROP_CUR_IGNORE_P_D_RATIO,				//宠物实际(最终)忽略物理防御比值（万分比）
	PET_PROP_CUR_IGNORE_M_D,					//宠物实际(最终)忽略法术防御值
	PET_PROP_CUR_IGNORE_M_D_RATIO,				//宠物实际(最终)忽略法术防御比值（万分比）

	PET_PROP_CUR_REDUCE_P_DAMAGE,				//宠物实际(最终)物理伤害减免值
	PET_PROP_CUR_REDUCE_P_DAMAGE_RATIO,			//宠物实际(最终)物理伤害减免比值（万分比）
	PET_PROP_CUR_REDUCE_M_DAMAGE,				//宠物实际(最终)法术伤害减免值
	PET_PROP_CUR_REDUCE_M_DAMAGE_RATIO,			//宠物实际(最终)法术伤害减免比值（万分比）

	PET_PROP_CUR_INCREASE_P_DAMAGE,				//宠物实际(最终)物理伤害增加值
	PET_PROP_CUR_INCREASE_P_DAMAGE_RATIO,		//宠物实际(最终)物理伤害增加比值（万分比）
	PET_PROP_CUR_INCREASE_M_DAMAGE,				//宠物实际(最终)法术伤害增加值
	PET_PROP_CUR_INCREASE_M_DAMAGE_RATIO,		//宠物实际(最终)法术伤害增加比值（万分比）

	// 反弹伤害百分比
	PET_PROP_CUR_REBOUND_DAMAGE_RATIO,			//宠物实际(最终)反弹伤害百分比

	PET_PROP_CUR_PENETRATE,						//宠物实际(最终)穿刺（绝对)伤害值
	PET_PROP_CUR_KICK_PENETRATE,				//宠物实际(最终)抵抗穿刺（抵抗绝对)伤害值
	PET_PROP_CUR_KICK_PENETRATE_RATIO,			//宠物实际(最终)抵抗穿刺（抵抗绝对)伤害比值（万分比）

	//技能和BUFF（非常驻被动技能除外）带来的属性加成
	//物理/法术 的防防御/攻击 跟自身相关的
	PET_PROP_SKILL_BASE_MIN_P_A,				//技能和BUFF（非常驻被动技能除外）带来的增加自身的物理攻击最小值
	PET_PROP_SKILL_BASE_MIN_P_A_RATIO,			//技能和BUFF（非常驻被动技能除外）带来的增加自身的物理攻击最小比值（万分比）
	PET_PROP_SKILL_BASE_MAX_P_A,				//技能和BUFF（非常驻被动技能除外）带来的增加自身的物理攻击最大值
	PET_PROP_SKILL_BASE_MAX_P_A_RATIO,			//技能和BUFF（非常驻被动技能除外）带来的增加自身的物理攻击最大比值（万分比）

	PET_PROP_SKILL_BASE_MIN_M_A,				//技能和BUFF（非常驻被动技能除外）带来的增加自身的法术攻击最小值
	PET_PROP_SKILL_BASE_MIN_M_A_RATIO,			//技能和BUFF（非常驻被动技能除外）带来的增加自身的法术攻击最小比值（万分比）
	PET_PROP_SKILL_BASE_MAX_M_A,				//技能和BUFF（非常驻被动技能除外）带来的增加自身的法术攻击最大值
	PET_PROP_SKILL_BASE_MAX_M_A_RATIO,			//技能和BUFF（非常驻被动技能除外）带来的增加自身的法术攻击最大比值（万分比）

	PET_PROP_SKILL_BASE_P_D,					//技能和BUFF（非常驻被动技能除外）带来的增加自身的物理防御值
	PET_PROP_SKILL_BASE_P_D_RATIO,				//技能和BUFF（非常驻被动技能除外）带来的增加自身的物理防御比值（万分比）

	PET_PROP_SKILL_BASE_M_D,					//技能和BUFF（非常驻被动技能除外）带来的增加自身的法术防御值
	PET_PROP_SKILL_BASE_M_D_RATIO,				//技能和BUFF（非常驻被动技能除外）带来的增加自身的法术防御比值（万分比）

	//物理/法术 的防防御/攻击 跟当前相关的
	PET_PROP_SKILL_SUM_MIN_P_A_RATIO,			//技能和BUFF（非常驻被动技能除外）带来的增加当前的物理攻击最小比值（万分比）
	PET_PROP_SKILL_SUM_MAX_P_A_RATIO,			//技能和BUFF（非常驻被动技能除外）带来的增加当前的物理攻击最大比值（万分比）
	PET_PROP_SKILL_SUM_MIN_M_A_RATIO,			//技能和BUFF（非常驻被动技能除外）带来的增加当前的法术攻击最小比值（万分比）
	PET_PROP_SKILL_SUM_MAX_M_A_RATIO,			//技能和BUFF（非常驻被动技能除外）带来的增加当前的法术攻击最大比值（万分比）
	PET_PROP_SKILL_SUM_P_D_RATIO,				//技能和BUFF（非常驻被动技能除外）带来的增加当前的物理防御比值（万分比）
	PET_PROP_SKILL_SUM_M_D_RATIO,				//技能和BUFF（非常驻被动技能除外）带来的增加当前的法术防御比值（万分比）

	//对HP/MP相关的
	PET_PROP_SKILL_BASE_MAX_HP,					//技能和BUFF（非常驻被动技能除外）带来的增加宠物自身最大生命值
	PET_PROP_SKILL_BASE_MAX_HP_RATIO,			//技能和BUFF（非常驻被动技能除外）带来的增加宠物自身最大生命比值（万分比）
	PET_PROP_SKILL_SUM_MAX_HP_RATIO,			//技能和BUFF（非常驻被动技能除外）带来的增加宠物当前的最大生命比值（万分比）

	PET_PROP_SKILL_HP_RECOVERY,					//技能和BUFF（非常驻被动技能除外）带来的增加生命回复速度值
	PET_PROP_SKILL_HP_RECOVERY_RATIO,			//技能和BUFF（非常驻被动技能除外）带来的增加生命回复速度比值（万分比）

	//对战斗相关的
	PET_PROP_SKILL_HEDGE,						//技能和BUFF（非常驻被动技能除外）带来的增加闪避值
	PET_PROP_SKILL_HEDGE_RATIO,					//技能和BUFF（非常驻被动技能除外）带来的增加闪避率
	PET_PROP_SKILL_PRESENT,						//技能和BUFF（非常驻被动技能除外）带来的增加命中值
	PET_PROP_SKILL_PRESENT_RATIO,				//技能和BUFF（非常驻被动技能除外）带来的增加命中率

	PET_PROP_SKILL_FATAL,						//技能和BUFF（非常驻被动技能除外）带来的增加会心值
	PET_PROP_SKILL_FATAL_RATIO,					//技能和BUFF（非常驻被动技能除外）带来的增加会心率
	PET_PROP_SKILL_KICK_FATAL,					//技能和BUFF（非常驻被动技能除外）带来的增加御劲值

	PET_PROP_SKILL_FATAL_DAMAGE_RATIO,			//技能和BUFF（非常驻被动技能除外）带来的增加会心伤害比值（万分比)
	PET_PROP_SKILL_KICK_FATAL_DAMAGE_RATIO,		//技能和BUFF（非常驻被动技能除外）带来的抵抗会心伤害比值（万分比)

	PET_PROP_SKILL_ATTACK_SPEED,				//技能和BUFF（非常驻被动技能除外）带来的攻击速度值
	PET_PROP_SKILL_ATTACK_SPEED_RATIO,			//技能和BUFF（非常驻被动技能除外）带来的攻击速度比值（万分比）
	PET_PROP_SKILL_MOVE_SPEED,					//技能和BUFF（非常驻被动技能除外）带来的移动速度
	PET_PROP_SKILL_MOVE_SPEED_RATIO,			//技能和BUFF（非常驻被动技能除外）带来的移动速度比值（万分比）

	PET_PROP_SKILL_IGNORE_P_D,					//技能和BUFF（非常驻被动技能除外）带来的忽略物理防御值
	PET_PROP_SKILL_IGNORE_P_D_RATIO,			//技能和BUFF（非常驻被动技能除外）带来的忽略物理防御比值（万分比）
	PET_PROP_SKILL_IGNORE_M_D,					//技能和BUFF（非常驻被动技能除外）带来的忽略法术防御值
	PET_PROP_SKILL_IGNORE_M_D_RATIO,			//技能和BUFF（非常驻被动技能除外）带来的忽略法术防御比值（万分比）

	PET_PROP_SKILL_REDUCE_P_DAMAGE,				//技能和BUFF（包括常驻被动技能）带来的物理伤害减免值
	PET_PROP_SKILL_REDUCE_P_DAMAGE_RATIO,		//技能和BUFF（包括常驻被动技能）带来的物理伤害减免比值（万分比）
	PET_PROP_SKILL_REDUCE_M_DAMAGE,				//技能和BUFF（包括常驻被动技能）带来的法术伤害减免值
	PET_PROP_SKILL_REDUCE_M_DAMAGE_RATIO,		//技能和BUFF（包括常驻被动技能）带来的法术伤害减免比值（万分比）

	PET_PROP_SKILL_INCREASE_P_DAMAGE,			//技能和BUFF（包括常驻被动技能）带来的物理伤害增加值
	PET_PROP_SKILL_INCREASE_P_DAMAGE_RATIO,		//技能和BUFF（包括常驻被动技能）带来的物理伤害增加比值（万分比）
	PET_PROP_SKILL_INCREASE_M_DAMAGE,			//技能和BUFF（包括常驻被动技能）带来的法术伤害增加值
	PET_PROP_SKILL_INCREASE_M_DAMAGE_RATIO,		//技能和BUFF（包括常驻被动技能）带来的法术伤害增加比值（万分比）

	// 反弹伤害百分比
	PET_PROP_SKILL_REBOUND_DAMAGE_RATIO,		//技能和BUFF（包括常驻被动技能）带来的反弹伤害百分比
	PET_PROP_SKILL_PENETRATE,					//技能和BUFF（非常驻被动技能除外）带来的穿刺（绝对)伤害值
	PET_PROP_SKILL_KICK_PENETRATE,				//技能和BUFF（非常驻被动技能除外）带来的抵抗穿刺（抵抗绝对)伤害值
	PET_PROP_SKILL_KICK_PENETRATE_RATIO,		//技能和BUFF（非常驻被动技能除外）带来的抵抗穿刺（抵抗绝对)伤害比值（万分比）

	PET_PROP_SKILLREBOUND,						//技能和BUFF（非常驻被动技能除外）带来的伤害反弹比值（万分比）

	//物理/法术 的防御/攻击 额外改变值
	PET_PROP_PLUS_MIN_P_A,                      // 额外改变最小物理攻击值
	PET_PROP_PLUS_MAX_P_A,                      // 额外改变最大物理攻击值
	PET_PROP_PLUS_MIN_M_A,                      // 额外改变最小法术攻击值
	PET_PROP_PLUS_MAX_M_A,                      // 额外改变最大法术攻击值
	PET_PROP_PLUS_P_D,                          // 额外改变物理防御值
	PET_PROP_PLUS_M_D,                          // 额外改变法术防御值

	/* 1、如果有大小比，最小值ID必须排在最大值ID前一位。
	2、只有当前值，才需要大小比。
	3、当前值，一般都是禁止修改的，因为都是基本值和附加值计算出来的，要改就改附加值就好了。
	4、附加值，都是允许为负数的，因为BUFF为数值有加有减的。*/

	PET_PROP_MAXID,					// 最大属性ID
}; 
//#define PET_POEWR_FLOAT 10000.0f      //bu cjl 宠物的power要除以这个值，然后变成浮点

// 这个枚举是为了取出人物属性中非公共部件。宠物用不到的属性映射, 换个名称对应成宠物的其他属性。
// 外部调用宠物的专用属性时，以便于外部调用时能清晰的表述是宠物的何种属性。以达到减少人物属性枚举的数目。
// 但是会在外部出现两个不同名称的枚举共同指向同一份数据，在的到数据时要区分是哪类实体的调用，进而找到正确的数据
// 此处的设计优劣 暂定(WZH 09.12.09) 
enum CREATURE_TO_PET
{
	CREATURE_PET_PROP_SID = CREATURE_PROP_SEX,													// 宠物身份牌 <=> 人物性别										
    CREATURE_PET_PROP_GENIUS = CREATURE_PROP_NATION,											// 天赋 <=> 人物国籍
	CREATURE_PET_PROP_ELEMENT = CREATURE_PROP_PROVINCE,											// 五行属性 <=> 人物省份
	CREATURE_PET_PROP_SANCTIFY = CREATURE_PROP_CONSTELLATION,									// 圣化级别 <=> 人物星座 ldy
	CREATURE_PET_PROP_STARS = CREATURE_PROP_FACEID,												// 星级，品质 <=> 人物头像
	CREATURE_PET_PROP_HOMAGE = CREATURE_PROP_PURSE_MONEY,										// 头衔，封号 <=> 身上金钱
	CREATURE_PET_PROP_DEATH = CREATURE_PROP_PURSE_GOLDINGOT,									// 死亡状态 <=> 身上元宝 
	
	CREATURE_PET_PROP_RELEASE_STATE = CREATURE_PROP_USABLEBAG,									// 解封或未解封 <=> 可用行囊
	CREATURE_PET_PROP_CHUMMY = CREATURE_PROP_WARE_MONEY,										// 亲密度 <=> 仓库是否被锁
	CREATURE_PET_PROP_CUR_BIRTH_TIMES = CREATURE_PROP_WARE_LOCK,								// 当前转世次数 <=> 仓库密码	
	CREATURE_PET_PROP_POTENTIAL = CREATURE_PROP_SKILLPOINT,										// 潜力值<=>技能点
	CREATURE_PET_PROP_RELEASE_START_TIME = CREATURE_PROP_SKILLPOINT_CONSUME_BRANCH_FIRST,		// 解封开始时间<=>第一个技能分支上消耗的技能点
	CREATURE_PET_PROP_POSITION = CREATURE_PROP_SKILLPOINT_CONSUME_BRANCH_SECOND,				// 阵法 方位 <=> 第二个技能分支上消耗的技能点
	CREATURE_PET_PROP_REALIZATION_TIMES = CREATURE_PROP_EQUIP_BASE_P_D,							// 顿悟次数<=> 白装最小法术防御
	CREATURE_PET_PROP_HP_GROWTH_PER = CREATURE_PROP_EQUIP_BASE_MIN_M_A,							// 生命成长率<=>基础体质
	CREATURE_PET_PROP_MIN_P_A_GROWTH_PER = CREATURE_PROP_EQUIP_BASE_MAX_M_A,						// 最小物理攻击成长率<=>基础力量
	CREATURE_PET_PROP_MAX_P_A_GROWTH_PER = CREATURE_PROP_EQUIP_BASE_M_D,							// 最大物理攻击成长率<=>基础敏捷
	CREATURE_PET_PROP_MIN_M_A_GROWTH_PER = CREATURE_PROP_EQUIP_BASE_MAX_HP,						// 最小魔法攻击成长率<=>基础智力
	CREATURE_PET_PROP_MAX_M_A_GROWTH_PER = CREATURE_PROP_EQUIP_BASE_MAX_MP,						// 最大魔法攻击成长率<=>基础精神				
	CREATURE_PET_PROP_P_DEF_GROWTH_PER = CREATURE_PROP_EQUIP_BASE_HEDGE,						// 物理防御成长率<=>基础最小物理攻击
	CREATURE_PET_PROP_M_DEF_GROWTH_PER = CREATURE_PROP_EQUIP_BASE_PRESENT,						// 法术防御成长率<=>基础最大物理攻击
	CREATURE_PET_PROP_LUCK = CREATURE_PROP_EQUIP_BASE_FATAL,									// 幸运值<=>基础最小法术攻击
	CREATURE_PET_PROP_FEED_STARTTIME = CREATURE_PROP_EQUIP_BASE_KICK_FATAL,						// 开始喂食<=>基础最大法术攻击
	CREATURE_PET_PROP_INIT_HP  = CREATURE_PROP_EQUIP_PLUS_MIN_P_A,								// 初始生命值<=>基础最小物理防御
	CREATURE_PET_PROP_INIT_MIN_P_A = CREATURE_PROP_EQUIP_PLUS_MIN_P_A_RATIO,					// 初始最小物理攻击<=>基础最大物理防御
    CREATURE_PET_PROP_INIT_MAX_P_A = CREATURE_PROP_EQUIP_PLUS_MIN_P_A_RATIO_2,					// 初始最大物理攻击<=>基础最小法术防御
    CREATURE_PET_PROP_INIT_MIN_M_A = CREATURE_PROP_EQUIP_PLUS_MAX_P_A,							// 初始最小法术攻击<=>基础最大法术防御
	CREATURE_PET_PROP_INIT_MAX_M_A = CREATURE_PROP_EQUIP_PLUS_MAX_P_A_RATIO,					// 初始最大法术攻击<=>附加体质
    CREATURE_PET_PROP_INIT_P_D = CREATURE_PROP_EQUIP_PLUS_MAX_P_A_RATIO_2,						// 初始物理防御<=>附加力量
	CREATURE_PET_PROP_INIT_M_D = CREATURE_PROP_EQUIP_PLUS_P_D,									// 初始法术防御<=>附加敏捷
    CREATURE_PET_PROP_INIT_HIT_PET = CREATURE_PROP_EQUIP_PLUS_P_D_RATIO,						// 初始命中概率<=>附加智力
    CREATURE_PET_PROP_INIT_HEDGE_PET = CREATURE_PROP_EQUIP_PLUS_P_D_RATIO_2,					// 初始闪避概率<=>附加精神
	CREATURE_PET_PROP_INIT_FATAL_PER = CREATURE_PROP_EQUIP_PLUS_MIN_M_A,						// 初始暴击概率<=>附加所有属性（体质，力量，敏捷，智力，精神）
	CREATURE_PET_PROP_SUB_EXP = CREATURE_PROP_EQUIP_PLUS_MIN_M_A_RATIO ,						// 内丹经验 <=> 白装最小物理攻击 
	CREATURE_PET_PROP_ACTIONSTATE = CREATURE_PROP_EQUIP_PLUS_MIN_M_A_RATIO_2 ,							// 绑定标志 <-> 白装最大物理攻击
	CREATURE_PET_PROP_AWORDTIME = CREATURE_PROP_EQUIP_PLUS_MAX_M_A,								// 获得时间<=>装备附加改变最大法术攻击值

	CREATURE_PET_PROP_MAXID = CREATURE_PROP_MAXID,												// 最大属性ID
};

/////////////////////////////宠物炼魂（合成）////////////////////
enum
{
	PET_MAINPROP_COMPOUND = 1,		//主属性炼魂
	PET_MINORPROP_COMPOUND  ,		//副属性炼魂
	PET_INITPROP_COMPOUND  ,		//初始属性炼魂
	PET_LUCKYPROP_COMPOUND  ,		//幸运值炼魂
	PET_IDCARDPROP_COMPOUND  ,		//身份牌炼魂
};


//////////////////////////// 物品类 ////////////////////////////////
//  物品基本分类；
enum EGOODS_CLASS
{
	GOODS_CLASS_MIN   = 0,

	GOODS_CLASS_EQUIPMENT,                          // 装备
	GOODS_CLASS_NONEQUIPABLEGOODS,                  // 非装备类物品

	GOODS_CLASS_MAX,
};

//  装备子类；
enum EEQUIPMENT_SUBCLASS
{
	EQUIPMENT_SUBCLASS_MIN = 0,
	EQUIPMENT_SUBCLASS_WARRIOR_WEAPON_2_CLASS,	//短兵EQUIPMENT_SUBCLASS_SORCERER_SINGLE_WEAPON
	EQUIPMENT_SUBCLASS_WARRIOR_WEAPON_1_CLASS,	//长兵 EQUIPMENT_SUBCLASS_WARRIOR_SINGLE_WEAPON
	EQUIPMENT_SUBCLASS_ARCHER_WEAPON_1_CLASS,	//弓系 EQUIPMENT_SUBCLASS_ARCHER_DOUBLE_WEAPON
	EQUIPMENT_SUBCLASS_ARCHER_WEAPON_2_CLASS,	//弩系 EQUIPMENT_SUBCLASS_ARCHER_SINGLE_WEAPON
	EQUIPMENT_SUBCLASS_SORCERER_WEAPON_1_CLASS,	//书卷 EQUIPMENT_SUBCLASS_SORCERER_DOUBLE_WEAPON
	EQUIPMENT_SUBCLASS_SORCERER_WEAPON_2_CLASS,	//法杖 EQUIPMENT_SUBCLASS_SORCERER_SINGLE_WEAPON
	EQUIPMENT_SUBCLASS_ARMET,					//头盔	
	EQUIPMENT_SUBCLASS_ARMOR,					//衣甲
	EQUIPMENT_SUBCLASS_SHOES,					//鞋子
	EQUIPMENT_SUBCLASS_CUFF,					//护腕
	EQUIPMENT_SUBCLASS_RING,					//戒指
	EQUIPMENT_SUBCLASS_DECORATION,				//挂坠
	EQUIPMENT_SUBCLASS_NECKLACE,				//项链
	EQUIPMENT_SUBCLASS_SUITARMET,				//头部时装
	EQUIPMENT_SUBCLASS_SUITARMOR,				//身体时装
	EQUIPMENT_SUBCLASS_GEMS,					//灵石
	EQUIPMENT_SUBCLASS_BAG,						//行囊
	EQUIPMENT_SUBCLASS_OTHER,					//其他
	EQUIPMENT_SUBCLASS_MAX,						//最大
};

//  物品子类(NonequipableGoods.csv表)；
enum ENONEQUIPABLEGOODS_SUBCLASS
{
	NONEQUIPABLEGOODS_SUBCLASS_MIN    = 0,

	NONEQUIPABLEGOODS_SUBCLASS_LEECHDOM,             // 药品
	NONEQUIPABLEGOODS_SUBCLASS_SKILLBOOK,            // 技能书
	NONEQUIPABLEGOODS_SUBCLASS_PETSKILLBOOK,         // 宠物技能书(wzh)
	NONEQUIPABLEGOODS_SUBCLASS_TASKGOOD,			 // 任务物品
	NONEQUIPABLEGOODS_SUBCLASS_ORE,				     // 矿石
	NONEQUIPABLEGOODS_SUBCLASS_HERBAGE,              // 草药
	NONEQUIPABLEGOODS_SUBCLASS_WOOD,				 // 木材
	NONEQUIPABLEGOODS_SUBCLASS_MATERIAL,             // 材料
	NONEQUIPABLEGOODS_SUBCLASS_MICS,                 // 杂物
	NONEQUIPABLEGOODS_SUBCLASS_STONE,				 // 灵石
	NONEQUIPABLEGOODS_SUBCLASS_SOLE,				 // 魂魄
	NONEQUIPABLEGOODS_SUBCLASS_PROP,				 // 道具
	NONEQUIPABLEGOODS_SUBCLASS_SECRET,               // 配方
	NONEQUIPABLEGOODS_SUBCLASS_SPAR,                 // 玄晶
	NONEQUIPABLEGOODS_SUBCLASS_RIDE,				 // 骑宠 zgz
	NONEQUIPABLEGOODS_SUBCLASS_OTHER,                // 其他

	NONEQUIPABLEGOODS_SUBCLASS_MAX
};

//////////////////////////// 武器/防具 /////////////////////////////
//武器的变形暂时没有考虑
enum
{
	EQUIPMENT_FUNCTIONTYPE_WEAPON = 1,	// 武器
	EQUIPMENT_FUNCTIONTYPE_ARMOUR,		// 防具
};

//////////////////////////////装备品质属性级别//////////////////////////////
enum
{
	GOODS_BRAND_LEVEL_NONE_LEVEL	= 0,		//无品质属性
	GOODS_BRAND_LEVEL_GRAY_LEVEL,				//灰色品质属性
	GOODS_BRAND_LEVEL_WHITE_LEVEL,				//白色品质属性
	GOODS_BRAND_LEVEL_GREEN_LEVEL,				//绿色品质属性
	GOODS_BRAND_LEVEL_BLUE_LEVEL,				//蓝色品质属性
	GOODS_BRAND_LEVEL_VIOLET_LEVEL,				//紫色属性品质
	GOODS_BRAND_LEVEL_GOLDEN_LEVEL,				//金色品质属性
	GOODS_BRAND_LEVEL_MAX_LEVEL,				//最大品质属性
};
//////////////////////////////装备炼魂级别/////////////////////////////////////
enum
{
	EQUIPMENT_SMELT_SOUL_LEVEL_0 = 0,				//炼魂级别0
	EQUIPMENT_SMELT_SOUL_LEVEL_1,
	EQUIPMENT_SMELT_SOUL_LEVEL_2,
	EQUIPMENT_SMELT_SOUL_LEVEL_3,
	EQUIPMENT_SMELT_SOUL_LEVEL_4,
	EQUIPMENT_SMELT_SOUL_LEVEL_5,
	EQUIPMENT_SMELT_SOUL_LEVEL_6,
	EQUIPMENT_SMELT_SOUL_LEVEL_7,
	EQUIPMENT_SMELT_SOUL_LEVEL_8,
	EQUIPMENT_SMELT_SOUL_LEVEL_9,
	EQUIPMENT_SMELT_SOUL_LEVEL_MAX,

};

//////////////////////////////装备战斗属性名//////////////////////////
#define EQUIP_COMBAT_PROP_ID2PROP( id )		(  (id) <= EQUIP_COMBAT_PROP_ID_MIN  \
											? ( EQUIP_COMBAT_PROP_ID_MIN ) : \
											(  (id) >= EQUIP_COMBAT_PROP_ID_MAX ? \
											( EQUIP_COMBAT_PROP_ID_MIN ): \
											( (id) - EQUIP_COMBAT_PROP_ID_MIN + EQUIP_PROP_CUR_BASE) ) )

enum
{
	//装备带来的属性
	EQUIP_COMBAT_PROP_ID_MIN = 0,						//下界
	//装备基础的
	EQUIP_COMBAT_PROP_ID_BASE_MIN_P_A,					//装备(武器)基础最小物理攻击值
	EQUIP_COMBAT_PROP_ID_BASE_MAX_P_A,					//装备(武器)基础最大物理攻击值
	EQUIP_COMBAT_PROP_ID_BASE_P_D,						//装备基础物理防御值		
	EQUIP_COMBAT_PROP_ID_BASE_MIN_M_A,					//装备(武器)基础最小法术攻击值	
	EQUIP_COMBAT_PROP_ID_BASE_MAX_M_A,					//装备(武器)基础最大法术攻击值
	EQUIP_COMBAT_PROP_ID_BASE_M_D,						//装备基础法术防御值		
	EQUIP_COMBAT_PROP_ID_BASE_MAX_HP,					//装备基础最大生命值
	EQUIP_COMBAT_PROP_ID_BASE_MAX_MP,					//装备基础最大内力值		
	EQUIP_COMBAT_PROP_ID_BASE_HEDGE,					//装备基础闪避值
	EQUIP_COMBAT_PROP_ID_BASE_PRESENT,					//装备基础命中值		
	EQUIP_COMBAT_PROP_ID_BASE_FATAL,					//装备基础会心值		
	EQUIP_COMBAT_PROP_ID_BASE_KICK_FATAL,				//装备基础御劲值
	//装备附加的
	EQUIP_COMBAT_PROP_ID_PLUS_MIN_P_A,					//装备附加改变最小物理攻击值		
	EQUIP_COMBAT_PROP_ID_PLUS_MIN_P_A_RATIO,			//装备附加改变人物最小物理攻击百分比	（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_MIN_P_A_RATIO_2,			//装备附加改变当前最小物理攻击百分比	（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_MAX_P_A,					//装备附加改变最大物理攻击值
	EQUIP_COMBAT_PROP_ID_PLUS_MAX_P_A_RATIO,			//装备附加改变人物最大物理攻击百分比	（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_MAX_P_A_RATIO_2,			//装备附加改变当前最大物理攻击百分比	（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_P_D,						//装备附加改变物理防御值
	EQUIP_COMBAT_PROP_ID_PLUS_P_D_RATIO,				//装备附加改变人物物理防御百分比		（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_P_D_RATIO_2,				//装备附加改变当前物理防御百分比		（万分比）

	EQUIP_COMBAT_PROP_ID_PLUS_MIN_M_A,					//装备附加改变最小法术攻击值
	EQUIP_COMBAT_PROP_ID_PLUS_MIN_M_A_RATIO,			//装备附加改变人物最小法术攻击百分比	（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_MIN_M_A_RATIO_2,			//装备附加改变当前最小法术攻击百分比	（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_MAX_M_A,					//装备附加改变最大法术攻击值
	EQUIP_COMBAT_PROP_ID_PLUS_MAX_M_A_RATIO,			//装备附加改变人物最大法术攻击百分比	（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_MAX_M_A_RATIO_2,			//装备附加改变当前最大法术攻击百分比	（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_M_D,						//装备附加改变法术防御值
	EQUIP_COMBAT_PROP_ID_PLUS_M_D_RATIO,				//装备附加改变人物法术防御百分比		（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_M_D_RATIO_2,				//装备附加改变当前法术防御百分比		（万分比）

	EQUIP_COMBAT_PROP_ID_PLUS_MAX_HP,					//装备附加改变最大生命值
	EQUIP_COMBAT_PROP_ID_PLUS_MAX_HP_RATIO,				//装备附加改变人物最大生命百分比		（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_MAX_HP_RATIO_2,			//装备附加改变当前最大生命百分比		（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_MAX_MP,					//装备附加改变最大内力值
	EQUIP_COMBAT_PROP_ID_PLUS_MAX_MP_RATIO,				//装备附加改变人物最大内力百分比		（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_MAX_MP_RATIO_2,			//装备附加改变当前最大内力百分比		（万分比）

	EQUIP_COMBAT_PROP_ID_PLUS_HP_RECOVER,				//装备附加生命回复值
	EQUIP_COMBAT_PROP_ID_PLUS_HP_RECOVER_RATIO,			//装备附加改变生命回复百分比			（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_MP_RECOVER,				//装备附加内力回复值
	EQUIP_COMBAT_PROP_ID_PLUS_MP_RECOVER_RATIO,			//装备附加改变内力回复百分比			（万分比）

	EQUIP_COMBAT_PROP_ID_PLUS_HEDGE,					//装备附加闪避值
	EQUIP_COMBAT_PROP_ID_PLUS_HEDGE_RATIO,				//装备附加改变闪避率
	EQUIP_COMBAT_PROP_ID_PLUS_PRESENT,					//装备附加命中值
	EQUIP_COMBAT_PROP_ID_PLUS_PRESENT_RATIO,			//装备附加改变命中率

	EQUIP_COMBAT_PROP_ID_PLUS_FATAL,					//装备附加会心值
	EQUIP_COMBAT_PROP_ID_PLUS_FATAL_RATIO,				//装备附加改变会心率
	EQUIP_COMBAT_PROP_ID_PLUS_KICK_FATAL,				//装备附加御劲值

	EQUIP_COMBAT_PROP_ID_PLUS_FATAL_DAMAGE_RATIO,		//装备附加会心伤害提高百分比			（万分比)
	EQUIP_COMBAT_PROP_ID_PLUS_KICK_FATAL_DAMAGE_RATIO,	//装备附加会心伤害减免百分比			（万分比)
	
	EQUIP_COMBAT_PROP_ID_PLUS_ATTACK_SPEED,				//装备附加攻击速度值
	EQUIP_COMBAT_PROP_ID_PLUS_ATTACK_SPEED_RATIO,		//装备附加改变攻击速度百分比			（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_MOVE_SPEED,				//装备附加移动速度值
	EQUIP_COMBAT_PROP_ID_PLUS_MOVE_SPEED_RATIO,			//装备附加改变移动速度百分比			（万分比）

	EQUIP_COMBAT_PROP_ID_PLUS_IGNORE_P_D,				//装备附加忽略物理防御值
	EQUIP_COMBAT_PROP_ID_PLUS_IGNORE_P_D_RATIO,			//装备附加忽略物理防御百分比			（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_IGNORE_M_D,				//装备附加忽略法术防御值
	EQUIP_COMBAT_PROP_ID_PLUS_IGNORE_M_D_RATIO,			//装备附加忽略法术防御百分比			（万分比）

	EQUIP_COMBAT_PROP_ID_PLUS_REDUCE_P_DAMAGE,			//装备附加物理伤害减免值
	EQUIP_COMBAT_PROP_ID_PLUS_REDUCE_P_DAMAGE_RATIO,	//装备附加物理伤害减免百分比			（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_REDUCE_M_DAMAGE,			//装备附加法术伤害减免值
	EQUIP_COMBAT_PROP_ID_PLUS_REDUCE_M_DAMAGE_RATIO,	//装备附加法术伤害减免百分比			（万分比）

	EQUIP_COMBAT_PROP_ID_PLUS_INCREASE_P_DAMAGE,		//装备附加物理伤害增加值
	EQUIP_COMBAT_PROP_ID_PLUS_INCREASE_P_DAMAGE_RATIO,	//装备附加物理伤害增加百分比			（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_INCREASE_M_DAMAGE,		//装备附加法术伤害增加值
	EQUIP_COMBAT_PROP_ID_PLUS_INCREASE_M_DAMAGE_RATIO,	//装备附加法术伤害增加百分比			（万分比）
	EQUIP_COMBAT_PROP_ID_PLUS_REBOUND_DAMAGE_RATIO,		//装备附加反弹伤害百分比				（万分比）

	EQUIP_COMBAT_PROP_ID_PLUS_PENETRATE,				//装备附加穿刺攻击值
	EQUIP_COMBAT_PROP_ID_PLUS_KICK_PENETRATE,			//装备附加穿刺抵抗值
	EQUIP_COMBAT_PROP_ID_PLUS_KICK_PENETRATE_RATIO,		//装备附加穿刺抵抗百分比				（万分比）

	EQUIP_COMBAT_PROP_ID_BASE_FIGHT,					//装备固有的战斗力

	EQUIP_COMBAT_PROP_ID_MAX,							 //上界
};


////////////////////////////////宝石种类/////////////////////////////
enum
{	
	Gem_Type_Min = -1,				//下界
	Gem_Type_Zhong,					//忠
	Gem_Type_Xiao,					//孝
	Gem_Type_Ren,					//仁,
	Gem_Type_Yi,					//义
	Gem_Type_Li,					//礼
	Gem_Type_Zhi,					//智
	Gem_Type_Xin,					//信
	Gem_Type_Yong,					//勇
	Gem_Type_Max,					//上界
};
///////////////////////////////宝石等级////////////////////////////////////
enum
{
	Gem_Level_Min = 0,				//下界
	Gem_Level_1,					//1级
	Gem_Level_2,					//2级
	Gem_Level_3,					//3级
	Gem_Level_Max,					//上界
};

//////////////////////////////非装备物品属性/////////////////////////////
enum
{
	GOODS_PROP_GOODSID = 1,				// 物品ID
	GOODS_PROP_QTY,						// 数量	
	GOODS_PROP_BIND,					// 绑定标志
	GOODS_PROP_ENDURECUR,				// 当前耐久
	GOODS_PROP_ENDUREMAX,				// 最大耐久
	GOODS_PROP_SLOTQTY,					// 插槽数
	GOODS_PROP_BROADCAST,				// 以上为广播属性	
	GOODS_PROP_MAXID,					// 最大属性ID
	GOODS_PROP_EFFECTID,				// 附加属性
};

//////////////////////////////装备物品属性/////////////////////////////
//包括非装备物品的属性
enum
{
	EQUIP_PROP_GOODSID = GOODS_PROP_GOODSID,				// 物品ID
	EQUIP_PROP_QTY = GOODS_PROP_QTY,						// 数量	
	EQUIP_PROP_BIND = GOODS_PROP_BIND,						// 绑定标志
	EQUIP_PROP_ENDURECUR = GOODS_PROP_ENDURECUR,			// 当前耐久
	EQUIP_PROP_ENDUREMAX = GOODS_PROP_ENDUREMAX,			// 最大耐久
	EQUIP_PROP_SLOTQTY = GOODS_PROP_SLOTQTY,				// 插槽数
	EQUIP_PROP_BROADCAST_PLACEHOLDER = GOODS_PROP_BROADCAST,// 以上为广播属性	
	EQUIP_PROP_MAXID_PLACEHOLDER = GOODS_PROP_MAXID,		// 最大属性ID
	EQUIP_PROP_EFFECTID_PLACEHOLDER = GOODS_PROP_EFFECTID,	// 附加属性

	//装备带来的属性
	EQUIP_PROP_CUR_BASE = EQUIP_PROP_EFFECTID_PLACEHOLDER +1,//装备增加的属性的基数
	//装备基础的
	EQUIP_PROP_CUR_BASE_MIN_P_A,					//装备(武器)基础最小物理攻击值			(=装备(武器)天生最小物理攻击值 + 强化改变装备(武器)天生最小物理攻击值)
	EQUIP_PROP_CUR_BASE_MAX_P_A,					//装备(武器)基础最大物理攻击值			(=装备(武器)天生最大物理攻击值 + 强化改变装备(武器)天生最大物理攻击值 )
	EQUIP_PROP_CUR_BASE_P_D,						//装备基础物理防御值					(=装备天生物理防御值 + 强化改变装备天生物理防御值)
	EQUIP_PROP_CUR_BASE_MIN_M_A,					//装备(武器)基础最小法术攻击值			(=装备(武器)天生最小法术攻击值 + 强化改变装备(武器)天生最小法术攻击值)
	EQUIP_PROP_CUR_BASE_MAX_M_A,					//装备(武器)基础最大法术攻击值			(=装备(武器)天生最大法术攻击值 + 强化改变装备(武器)天生最大法术攻击值)
	EQUIP_PROP_CUR_BASE_M_D,						//装备基础法术防御值					(=装备天生法术防御值 + 强化改变装备天生法术防御值)
	EQUIP_PROP_CUR_BASE_MAX_HP,						//装备基础最大生命值					(=装备天生最大生命值 + 强化改变装备天生最大生命值)
	EQUIP_PROP_CUR_BASE_MAX_MP,						//装备基础最大内力值					(=装备天生最大内力值 + 强化改变装备天生最大内力值)
	EQUIP_PROP_CUR_BASE_HEDGE,						//装备基础闪避值						(=装备天生闪避值 + 强化改变装备天生闪避值)
	EQUIP_PROP_CUR_BASE_PRESENT,					//装备基础命中值						(=装备天生命中值 + 强化改变装备天生命中值)
	EQUIP_PROP_CUR_BASE_FATAL,						//装备基础会心值						(=装备天生会心值 + 强化改变装备天生会心值)
	EQUIP_PROP_CUR_BASE_KICK_FATAL,					//装备基础御劲值						(=装备天生御劲值 + 强化改变装备天生御劲值)
	//装备附加的
	EQUIP_PROP_CUR_PLUS_MIN_P_A,					//装备附加改变最小物理攻击值		
	EQUIP_PROP_CUR_PLUS_MIN_P_A_RATIO,				//装备附加改变人物最小物理攻击百分比	（万分比）
	EQUIP_PROP_CUR_PLUS_MIN_P_A_RATIO_2,			//装备附加改变当前最小物理攻击百分比	（万分比）
	EQUIP_PROP_CUR_PLUS_MAX_P_A,					//装备附加改变最大物理攻击值
	EQUIP_PROP_CUR_PLUS_MAX_P_A_RATIO,				//装备附加改变人物最大物理攻击百分比	（万分比）
	EQUIP_PROP_CUR_PLUS_MAX_P_A_RATIO_2,			//装备附加改变当前最大物理攻击百分比	（万分比）
	EQUIP_PROP_CUR_PLUS_P_D,						//装备附加改变物理防御值
	EQUIP_PROP_CUR_PLUS_P_D_RATIO,					//装备附加改变人物物理防御百分比		（万分比）
	EQUIP_PROP_CUR_PLUS_P_D_RATIO_2,				//装备附加改变当前物理防御百分比		（万分比）

	EQUIP_PROP_CUR_PLUS_MIN_M_A,					//装备附加改变最小法术攻击值
	EQUIP_PROP_CUR_PLUS_MIN_M_A_RATIO,				//装备附加改变人物最小法术攻击百分比	（万分比）
	EQUIP_PROP_CUR_PLUS_MIN_M_A_RATIO_2,			//装备附加改变当前最小法术攻击百分比	（万分比）
	EQUIP_PROP_CUR_PLUS_MAX_M_A,					//装备附加改变最大法术攻击值
	EQUIP_PROP_CUR_PLUS_MAX_M_A_RATIO,				//装备附加改变人物最大法术攻击百分比	（万分比）
	EQUIP_PROP_CUR_PLUS_MAX_M_A_RATIO_2,			//装备附加改变当前最大法术攻击百分比	（万分比）
	EQUIP_PROP_CUR_PLUS_M_D,						//装备附加改变法术防御值
	EQUIP_PROP_CUR_PLUS_M_D_RATIO,					//装备附加改变人物法术防御百分比		（万分比）
	EQUIP_PROP_CUR_PLUS_M_D_RATIO_2,				//装备附加改变当前法术防御百分比		（万分比）

	EQUIP_PROP_CUR_PLUS_MAX_HP,						//装备附加改变最大生命值
	EQUIP_PROP_CUR_PLUS_MAX_HP_RATIO,				//装备附加改变人物最大生命百分比		（万分比）
	EQUIP_PROP_CUR_PLUS_MAX_HP_RATIO_2,				//装备附加改变当前最大生命百分比		（万分比）
	EQUIP_PROP_CUR_PLUS_MAX_MP,						//装备附加改变最大内力值
	EQUIP_PROP_CUR_PLUS_MAX_MP_RATIO,				//装备附加改变人物最大内力百分比		（万分比）
	EQUIP_PROP_CUR_PLUS_MAX_MP_RATIO_2,				//装备附加改变当前最大内力百分比		（万分比）

	EQUIP_PROP_CUR_PLUS_HP_RECOVER,					//装备附加生命回复值
	EQUIP_PROP_CUR_PLUS_HP_RECOVER_RATIO,			//装备附加改变生命回复百分比			（万分比）
	EQUIP_PROP_CUR_PLUS_MP_RECOVER,					//装备附加内力回复值
	EQUIP_PROP_CUR_PLUS_MP_RECOVER_RATIO,			//装备附加改变内力回复百分比			（万分比）

	EQUIP_PROP_CUR_PLUS_HEDGE,						//装备附加闪避值
	EQUIP_PROP_CUR_PLUS_HEDGE_RATIO,				//装备附加改变闪避率
	EQUIP_PROP_CUR_PLUS_PRESENT,					//装备附加命中值
	EQUIP_PROP_CUR_PLUS_PRESENT_RATIO,				//装备附加改变命中率

	EQUIP_PROP_CUR_PLUS_FATAL,						//装备附加会心值
	EQUIP_PROP_CUR_PLUS_FATAL_RATIO,				//装备附加改变会心率
	EQUIP_PROP_CUR_PLUS_KICK_FATAL,					//装备附加御劲值

	EQUIP_PROP_CUR_PLUS_FATAL_DAMAGE_RATIO,			//装备附加会心伤害提高百分比			（万分比)
	EQUIP_PROP_CUR_PLUS_KICK_FATAL_DAMAGE_RATIO,	//装备附加会心伤害减免百分比			（万分比)
	
	EQUIP_PROP_CUR_PLUS_ATTACK_SPEED,				//装备附加攻击速度值
	EQUIP_PROP_CUR_PLUS_ATTACK_SPEED_RATIO,			//装备附加改变攻击速度百分比			（万分比）
	EQUIP_PROP_CUR_PLUS_MOVE_SPEED,					//装备附加移动速度值
	EQUIP_PROP_CUR_PLUS_MOVE_SPEED_RATIO,			//装备附加改变移动速度百分比			（万分比）

	EQUIP_PROP_CUR_PLUS_IGNORE_P_D,					//装备附加忽略物理防御值
	EQUIP_PROP_CUR_PLUS_IGNORE_P_D_RATIO,			//装备附加忽略物理防御百分比			（万分比）
	EQUIP_PROP_CUR_PLUS_IGNORE_M_D,					//装备附加忽略法术防御值
	EQUIP_PROP_CUR_PLUS_IGNORE_M_D_RATIO,			//装备附加忽略法术防御百分比			（万分比）

	EQUIP_PROP_CUR_PLUS_REDUCE_P_DAMAGE,			//装备附加物理伤害减免值
	EQUIP_PROP_CUR_PLUS_REDUCE_P_DAMAGE_RATIO,		//装备附加物理伤害减免百分比			（万分比）
	EQUIP_PROP_CUR_PLUS_REDUCE_M_DAMAGE,			//装备附加法术伤害减免值
	EQUIP_PROP_CUR_PLUS_REDUCE_M_DAMAGE_RATIO,		//装备附加法术伤害减免百分比			（万分比）

	EQUIP_PROP_CUR_PLUS_INCREASE_P_DAMAGE,			//装备附加物理伤害增加值
	EQUIP_PROP_CUR_PLUS_INCREASE_P_DAMAGE_RATIO,	//装备附加物理伤害增加百分比			（万分比）
	EQUIP_PROP_CUR_PLUS_INCREASE_M_DAMAGE,			//装备附加法术伤害增加值
	EQUIP_PROP_CUR_PLUS_INCREASE_M_DAMAGE_RATIO,	//装备附加法术伤害增加百分比			（万分比）
	EQUIP_PROP_CUR_PLUS_REBOUND_DAMAGE_RATIO,		//装备附加反弹伤害百分比				（万分比）

	EQUIP_PROP_CUR_PLUS_PENETRATE,					//装备附加穿刺攻击值
	EQUIP_PROP_CUR_PLUS_KICK_PENETRATE,				//装备附加穿刺抵抗值
	EQUIP_PROP_CUR_PLUS_KICK_PENETRATE_RATIO,		//装备附加穿刺抵抗百分比				（万分比）

	EQUIP_PROP_CUR_BASE_FIGHT,						//装备固有的战斗力

	EQUIP_PROP_CUR_MAX,			 //装备增加的属性的上界

	EQUIP_PROP_BRAND_BASE_LEVEL_ID,		//物品品质属性1 id
	EQUIP_PROP_BRAND_LEVEL_2_ID,		//物品品质属性2 id
	EQUIP_PROP_BRAND_LEVEL_3_ID,		//物品品质属性3 id
	EQUIP_PROP_BRAND_LEVEL_4_ID,		//物品品质属性4 id
	EQUIP_PROP_BRAND_MAX_LEVEL_ID,		//物品品质属性id最大值

	EQUIP_PROP_BRAND_BASE_LEVEL_VAL,	//物品品质属性1 值	
	EQUIP_PROP_BRAND_LEVEL_2_VAL,		//物品品质属性2 值	
	EQUIP_PROP_BRAND_LEVEL_3_VAL,		//物品品质属性3 值	
	EQUIP_PROP_BRAND_LEVEL_4_VAL,		//物品品质属性4 值
	EQUIP_PROP_BRAND_MAX_LEVEL_VAL,		//物品品质属性值界

	EQUIP_PROP_SMELT_SOUL_LEVEL,		//物品链魂强化等级

	EQUIP_PROP_GEMS_EMBEDED_NUM,				//镶嵌宝石的数量

	EQUIP_PROP_GEMS_BASE_ID,			//镶嵌的第1个宝石的id
	EQUIP_PROP_GEMS_2_ID,				//镶嵌的第2个宝石的id
	EQUIP_PROP_GEMS_3_ID,				//镶嵌的第3个宝石的id
	EQUIP_PROP_GEMS_4_ID,				//镶嵌的第4个宝石的id
	EQUIP_PROP_GEMS_MAX_ID,				//镶嵌的宝石id上界
	EQUIP_PROP_GEMS_COMBIND_ID,			//宝石组合后的附加属性id

	EQUIP_PROP_EXPIRE_DATE,				//物品过期日期
	EQUIP_PROP_USING_TIMES,				//物品使用次数
	EQUIP_PROP_IS_CHECKED,				//物品是否已经鉴定


	EQUIP_PROP_BROADCAST,				// 以上为广播属性	

	EQUIP_PROP_MAXID,					// 最大属性ID

	EQUIP_PROP_EFFECTID,				// 附加属性
	EQUIP_PROP_LETTERING,				// 武器刻字
};

#define PACKET_ALL_EQUIP_PLACE        -2//by cjl,表示鉴定所有装备的位置，必须小于-1

//////////////////////////////宝箱属性/////////////////////////////
enum
{
	BOX_STATE_CLOSE = 0,				// 关闭
	//BOX_STATE_OPEN,						// 打开
};

enum
{
	BOX_TYPE_PACK = 0,					// 包裹（人物或怪物死亡掉下来的包裹）
	BOX_TYPE_HERBALMEDICINE,			// 草药	
	BOX_TYPE_MINERALRESOURCES,			// 矿藏
	BOX_TYPE_PRECIOUSDEPOSITS,			// 宝藏
};

enum
{
	BOX_BORNREASON_UNKNOW = 0,			// 未知原因
	BOX_BORNREASON_MONSTERDIE,			// 怪物死亡
	BOX_BORNREASON_PERSONDIE,			// 人物死亡
	BOX_BORNREASON_PERSONTHROW,			// 人物扔东西
	BOX_BORNREASON_SYSTEM,				// 系统生成的
};

enum
{
	BOX_PROP_RESID = 1,					// 盒子外观资源ID
	BOX_PROP_RAYRESID,					// 盒子闪烁资源ID
	BOX_PROP_STATE,						// 状态：打开，关闭
	BOX_PROP_TYPE,						// 盒子类型

	BOX_PROP_OPENNEEDTIME,				// 打开所需时间（需要多久才能打开）

	BOX_PROP_GOODSID,					// 宝箱里物品的配置ID，客户端根据这个显示不同的颜色

	BOX_PROP_BROADCAST,					// 以上为广播属性

	BOX_PROP_BORNREASON,				// 出生原因：怪物掉装，人物扔装，人物死亡，草药等	

	BOX_PROP_MAXID,						// 最大属性ID

	BOX_PROP_CLEARPOPEDOM,				// 清掉权限
};

//////////////////////////////旗杆属性/////////////////////////////
enum
{
	MAST_TYPE_CHUNNEL = 1,				// 传送门
	MAST_TYPE_STALL,					// 摊位
};

enum
{
	MAST_PROP_RESID = 1,				// 旗杆外观资源
	MAST_PROP_TYPE,						// 类型：传送门，摊位

	MAST_PROP_BROADCAST,				// 以上为广播属性

	MAST_PROP_MAXID,					// 最大属性ID
};

//////////////////////////////泡泡属性/////////////////////////
// add by zjp.
enum
{
	BUBBLE_PROP_RESID = 1,				// 泡泡外观资源
	BUBBLE_PROP_TYPE,					// 类型

	BUBBLE_PROP_BROADCAST,				// 以上为广播属性

	BUBBLE_PROP_MAXID,					// 最大属性ID
};

/************************************************************************/
/*AI 事件枚举
*/
/************************************************************************/
enum
{
	AI_EVENT_BORN         =   0 ,// 出生	0	目标是自己
	AI_EVENT_BEHIT              ,// 被击	1	目标是当前攻击自己的对象
	AI_EVENT_ATTACK             ,// 攻击	2	目标是当前攻击对象
	AI_EVENT_COMMAND            ,// 命令事件	3	目标是发命令的对象
	AI_EVENT_THINK              ,// 定时事件	4	目标是自己
	AI_EVENT_DIE                ,// 死亡事件	5	目标是杀死自己的对象
	AI_EVENT_MASTER             ,// 主人命令事件	6	主人命令事件由主人发出，仅其宠物才能收到，事件锁定目标为主人当前选中的目标（若主人为系统生物则目标为该生物正在攻击的对象）
	AI_EVENT_MASTER_TIMER       ,// 主人随机事件	7	主人随机事件是一种每隔500ms固定对宠物触发的事件，事件目标锁定为主人
	AI_EVENT_FRIEND_TIMER       ,// 友好目标随机事件	8	每个500ms触发一次的随机事件，每次获得一个自身仇恨列表中友好对象作为AI目标，每触发一次更换一个友好对象，遍历一次后再从头遍历；
	AI_EVENT_PREATTACK          ,// 目标蓄气事件	9	目标是正在蓄气的对象
	AI_EVENT_RETRIEVE           ,// 回归	10	目标是自己
	AI_EVENT_EXIT_BATTLE        ,// 退出战斗 11 目标自己
	AI_EVENT_RETRIEVE_OK        ,// 回归结束 12	目标是自己
	AI_EVENT_ESCAPE             ,// 逃跑事件 13 目标自己
	AI_EVENT_ESCAPE_OK          ,// 逃跑结束 14 目标是自己
	AI_EVENT_TALK				,// 喊话
	AI_EVENT_CRAZY_ATTACK		,// 暴击
	AI_EVENT_CRAZY_BEHIT		,// 被暴击
	AI_EVENT_HEDGE				,// 闪避
	AI_EVENT_BEHEDGE			,// 被闪避	
	AI_EVENT_DYNAMIC			,// 动态加载了决策
	AI_EVENT_MASTER_CMD_TIMER	,// 主人强制命令
	AI_EVENT_SELF_SUB			,// 以上的是订阅本身的事件

	///////以下必须订阅到主人身上，未召唤宠物是没有AI部件的，所以在召唤出来的时候才能订阅事件。宠物召唤出来之后必须马上设置它的主人。
	AI_EVENT_MASTERSUB_CRAZY_ATTACK		,// 主人暴击事件
	AI_EVENT_MASTERSUB_CRAZY_BEHIT		,// 主人被暴击	
	AI_EVENT_MASTERSUB_BEHIT			,// 主人被击事件
	AI_EVENT_MASTERSUB_HEDGE			,// 主人闪避事件
	AI_EVENT_MASTERSUB_BEHEDGE			,// 主人被闪避

	///////以下为主人控制命令，命令要成对，开始对应结束
	AI_EVENT_MASTERCMD_ATTACK			,// 主人命令攻击
	AI_EVENT_MASTERCMD_STOPATTACK		,// 主人命令停止攻击
	AI_EVENT_MASTERCMD_PICKUP			,// 主人命令拾取
	AI_EVENT_MASTERCMD_STOPPICK			,// 主人命令停止拾取
	AI_EVENT_MASTER_OK					,// 主人命令完成
};

/// 实体动作(按照策划文档定义)
enum EntityAction
{
	EntityAction_Min = -1,			// 动作最小值；
	EntityAction_Stand,				//站立动作
	EntityAction_Stand_1,			//站立动作(怪物第二套)
	EntityAction_Walk,				//行走动作
	EntityAction_Run,				//跑步
	EntityAction_SitingDown,			//坐下过程
	EntityAction_Siting,				//坐
	EntityAction_JumpStart,			//跳跃开始
	EntityAction_JumpMidway,			//跳跃中
	EntityAction_JumpEnd,			//跳跃结束
	EntityAction_CombatWound,		//战斗受伤
	EntityAction_Death,				//死亡
	EntityAction_Stun,				//晕眩
	EntityAction_Attack,				//第一套普通攻击
	EntityAction_Attack_1,			//第二套普通攻击
	EntityAction_Collect,			//采集
	EntityAction_Reserve_2,			//预留给非技能的动作
	EntityAction_Reserve_3,			//预留给非技能的动作
	EntityAction_Reserve_4,			//预留给非技能的动作
	EntityAction_Reserve_5,			//预留给非技能的动作
	EntityAction_Reserve_6,			//预留给非技能的动作
	EntityAction_Reserve_7,			//预留给非技能的动作
	EntityAction_Skill_1,	
	EntityAction_Skill_2,	
	EntityAction_Skill_3,	
	EntityAction_Skill_4,	
	EntityAction_Skill_5,	
	EntityAction_Skill_6,	
	EntityAction_Skill_7,	
	EntityAction_Skill_8,	
	EntityAction_Skill_9,	
	EntityAction_Skill_10,	
	EntityAction_Skill_11,	
	EntityAction_Skill_12,	
	EntityAction_Skill_13,	
	EntityAction_Skill_14,	
	EntityAction_Skill_15,	
	EntityAction_Skill_16,	
	EntityAction_Skill_17,	
	EntityAction_Skill_18,	
	EntityAction_Skill_19,	
	EntityAction_Skill_20,	
	EntityAction_Skill_21,	
	EntityAction_Skill_22,	
	EntityAction_Skill_23,	
	EntityAction_Skill_24,	
	EntityAction_Skill_25,	
	EntityAction_Skill_26,	
	EntityAction_Skill_27,	
	EntityAction_Skill_28,	
	EntityAction_Skill_29,	
	EntityAction_Skill_30,	
	EntityAction_Skill_31,	
	EntityAction_Skill_32,	
	EntityAction_Skill_33,	
	EntityAction_Skill_34,	
	EntityAction_Skill_35,	
	EntityAction_Skill_36,	
	EntityAction_Skill_37,	
	EntityAction_Skill_38,	
	EntityAction_Skill_39,	
	EntityAction_Skill_40,	
	EntityAction_Skill_41,	
	EntityAction_Skill_42,	
	EntityAction_Skill_43,	
	EntityAction_Skill_44,	
	EntityAction_Skill_45,	
	EntityAction_Skill_46,	
	EntityAction_Skill_47,	
	EntityAction_Skill_48,	
	EntityAction_Skill_49,	
	EntityAction_Skill_50,	
	EntityAction_Skill_51,	
	EntityAction_Skill_52,	
	EntityAction_Skill_53,	
	EntityAction_Skill_54,	
	EntityAction_Skill_55,	
	EntityAction_Skill_56,	
	EntityAction_Skill_57,	
	EntityAction_Skill_58,	
	EntityAction_Skill_59,	
	EntityAction_Skill_60,	
	EntityAction_Skill_61,	
	EntityAction_Skill_62,	
	EntityAction_Skill_63,	
	EntityAction_Skill_64,	
	EntityAction_Skill_65,	
	EntityAction_Skill_66,	
	EntityAction_Skill_67,	
	EntityAction_Skill_68,	
	EntityAction_Skill_69,	
	EntityAction_Skill_70,	
	EntityAction_Skill_71,	
	EntityAction_Skill_72,	
	EntityAction_Skill_73,	
	EntityAction_Skill_74,	
	EntityAction_Skill_75,	
	EntityAction_Skill_76,	
	EntityAction_Skill_77,	
	EntityAction_Skill_78,	
	EntityAction_Skill_79,	
	EntityAction_Skill_80,	
	EntityAction_Skill_81,	
	EntityAction_Skill_82,	
	EntityAction_Skill_83,	
	EntityAction_Skill_84,	
	EntityAction_Skill_85,	
	EntityAction_Skill_86,	
	EntityAction_Skill_87,	
	EntityAction_Skill_88,	
	EntityAction_Skill_89,	
	EntityAction_Skill_90,	
	EntityAction_Skill_91,	
	EntityAction_Skill_92,	
	EntityAction_Skill_93,	
	EntityAction_Skill_94,	
	EntityAction_Skill_95,	
	EntityAction_Skill_96,	
	EntityAction_Skill_97,	
	EntityAction_Skill_98,	
	EntityAction_Skill_99,	
	EntityAction_Skill_100,	
	EntityAction_Max,			//动作最大值
};

//by cjl
//替身规则
enum 
{
	ICreatureDouble_ChangeBody_Rule = 0,//变身规则
	ICreatureDouble_Rule_Max,
};
//end cjl