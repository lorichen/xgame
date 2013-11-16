/*******************************************************************
** 文件名:	e:\ourgame\ZoneManager\src\ZoneManager\DGlobalMessage.h
** 版  权:	(C) 
** 
** 日  期:	2007/11/24  15:03
** 版  本:	1.0
** 描  述:	消息码定义
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
/*消息码层次
|－服务器消息码
|－－模块消息码
|－－－行为消息码 //*/

#pragma once

#include "DGlobalGame.h"

#pragma pack(1)

// 所有消息的消息头
struct SGameMsgHead
{
	// 源端点码
	BYTE		SrcEndPoint;

	// 目标端点码
	BYTE		DestEndPoint;	

	// 目标模块消息码
	WORD		wKeyModule;

	// 行为消息码
	WORD		wKeyAction;
};
// 以下消息体的添加主要是针对宠物攻击时向目标靠近发移动，使用技能等客户端和服务器消息交互的处理 zhangjie 2009/12/14
struct SMsgActionMaster_CS
{
	LONGLONG    uidActionMaster;
};

/***************************************************************/
//////////////////////// 服务器层的全局定义 /////////////////////
/***************************************************************/
enum
{ 
	// 未定义
	MSG_ENDPOINT_UNKNOW = 0,

	// 超级中心［简称：］
	MSG_ENDPOINT_SUPERCENTER,

	// 场景服务器［简称：S］
	MSG_ENDPOINT_ZONE,

	// 网关服务器［简称：G］
	MSG_ENDPOINT_GATEWAY,

	// 登录服务器［简称：L］
	MSG_ENDPOINT_LOGIN,

	// 世界服务器［简称：W］
	MSG_ENDPOINT_WORLD,

	// 客户端    ［简称：C］
	MSG_ENDPOINT_CLIENT,

	// 输入控制  ［简称：I］
	MSG_ENDPOINT_INPUT,	

	// 中心服务器［简称：E］
	MSG_ENDPOINT_CENTER,

	// 社会服务器［简称：O］
	MSG_ENDPOINT_SOCIAL,

	// 最大
	MSG_SERVERID_MAXID
};

/***************************************************************/
///////////////////// 目标模块层的全局定义 //////////////////////
/***************************************************************/
enum
{
	// 未定义
	MSG_MODULEID_UNKNOW = 0,

	// 地图
	MSG_MODULEID_MAP,

	// 实体属性
	MSG_MODULEID_PROP,

	// 实体行为
	MSG_MODULEID_ACTION,

	// 任务模块
	MSG_MODULEID_TASK,

	//超级中心
	MSG_MODULEID_SUPERCENTER,

	// （S）网关模块
	MSG_MODULEID_GATEWAY,

	// （S）中心服务器模块
	MSG_MODULEID_CENTERSERVER,

	// （S）聊天模块
	MSG_MODULEID_CHAT,

	// 好友模块
	MSG_MODULEID_FRIEND,

	// 家族模块
	MSG_MODULEID_FAMILY,

	// 组队模块
	MSG_MODULEID_TEAM,

	// 通用物品篮
	MSG_MODULEID_SKEP,

	// （S）登录模块
	MSG_MODULEID_LOGIN,

	// 交易模块
	MSG_MODULEID_TRADE,

	// （S）切换地图
	MSG_MODULEID_SWITCH,

	// 系统模块
	MSG_MODULEID_SYSTEM,

	//（S）副本相关操作
	MSG_MODULEID_INSTANCE_ZONE,

	// 摆摊模块
	MSG_MODULEID_STALL,

	//在线商城
	MSG_MODULEID_ONLINE_SHOP,

	// 军团模块
	MSG_MODULEID_ARMY,

	// 最大
	MSG_MODULEID_MAXID
};

/***************************************************************/
///////////////////// 场景服的行为层的消息码/////////////////////
// 上层的消息码必定是MSG_MODULEID_MAP
/***************************************************************/
// 通知客户端创建地图
//验证密码之后，在任何时候都可能被其他人使用同一帐号抢占式的登录，服务器发送MSG_LOGINSERVER_MESSAGE通知
//为了和MSG_LOGINSERVER_MESSAGE冲突，MSG_MAP_LOADMAP从100开始
#define MSG_MAP_LOADMAP											101
enum
{
	LOADMAP_LOGIN  = 1,
	LOADMAP_CHANGEZONE = 2,
	LOADMAP_CHANGESERVER = 3,
};
// 消息体
struct SMsgMapLoadMap_SC
{
	DWORD		dwMapID;		// 地图ID
	DWORD		dwZoneID;		// 场景ID
	POINT		ptTile;			// 主角中心点
	int         nLoadReason;
};

// 通知客户端释放场景
#define MSG_MAP_DESTORYMAP										102
// 消息体 
struct SMsgMapDestoryMap_SC
{

};

#define MSG_MAP_LOADMAPSUCCESS									103
// 消息体 
struct SMsgMapLoadMapSuccess_CS
{
	int         nLoadReason;
};
/***************************************************************/
///////////////////// 场景服的行为层的消息码/////////////////////
// 上层的消息码必定是MSG_MODULEID_PROP
/***************************************************************/
// 通知客户端创建实体
#define MSG_PROP_CREATEENTITY									1
// 消息体
struct SMsgPropCreateEntity_SC
{
	LONGLONG	uidEntity;
	char		nEntityClass;	// 实体类型
	char		nIsHero;		// 是否客户端主角：1：是，2：否
	DWORD		dwZoneID;
	
	// ......................   // 创建现场
};

///////////////////////////////////////////////////////////////////
// 通知客户端删除实体
#define MSG_PROP_DESTROYENTITY									2
struct SMsgPropDestroyEntity_SC
{
	LONGLONG	uidEntity;
};

///////////////////////////////////////////////////////////////////
// 同步客户端实体属性
#define MSG_PROP_UPDATEPROP										3
struct SMsgPropUpdateProp_SC
{
	LONGLONG	uidEntity;
	char		nUpdateMode;	// 更新模式：1、单个数值属性更新，2：整个属性更新，3：单个字符属性更新

	// ......................   // 更新现场
};
//////////////////////////////////////////////////////////////////////////
//by cjl,同步客户端实体其他属性
//依然使用SMsgPropUpdateProp_SC，但nUpdateMode意义不同了
//nUpdateMode要和UPDATE_MY_XXX枚举对应
#define MSG_PROP_UPDATEMYPROP									4
//end cjl

// 最大
#define MSG_PROP_MAXID											128

/***************************************************************/
///////////////////// 场景服的系统层的消息码/////////////////////
// 上层的消息码必定是MSG_MODULEID_SYSTEM
/***************************************************************/
// 系统消息，客户端会弹出对话框
#define MSG_SYSTEM_TIPS											1
// 消息体
struct SMsgSystemTips_SC
{
	DWORD	dwErrorCode;		// 错误码
	int		nTipsLen;			// 错误信息长度
	// ...............			// 错误信息，支持HTML	
};


// 访问人物属性
#define MSG_SYSTEM_VISITOR										2
// 消息体
struct SMsgSystemVisitor_CO
{
	DWORD		dwVisitor;		// 谁访问
	DWORD		dwPDBID;		// 访问谁
};

struct SMsgSystemVisitor_OS
{
	DWORD		clientid;		// 谁访问
	DWORD		dwPDBID;		// 访问谁
};

struct SMsgSystemVisitor_SO
{
	DWORD		clientid;		// 谁访问
	DWORD		dwPDBID;		// 访问谁

	LONGLONG	uid;			// uid
	DWORD		dwZoneID;		// 场景ID
	DWORD		dwMapID;		// 地图ID

	// ExportHeroContext()		// 属性数据
};

struct SMsgSystemVisitor_OC
{	
	DWORD		dwPDBID;		// 访问谁

	LONGLONG	uid;			// uid
	DWORD		dwZoneID;		// 场景ID
	DWORD		dwMapID;		// 地图ID

	// ExportHeroContext()		// 属性数据
};

/***************************************************************/
///////////////////// 场景服的行为层的消息码/////////////////////
// 上层的消息码必定是MSG_MODULEID_ACTION
/***************************************************************/
// 从场景服发给客户端的消息必须继承此消息体
struct SMsgActionSCHead
{
	LONGLONG	uidEntity;		// 实体uid	
};

///////////////////////////////////////////////////////////////////
// 实体进入九宫格
#define	MSG_ACTION_ENTER										1
// 消息体
struct SMsgActionEnter_SS
{
	LONGLONG	uidMaster;
};

///////////////////////////////////////////////////////////////////
// 实体退出九宫格
#define	MSG_ACTION_EXIT											2
// 消息体
struct SMsgActionExit_SS
{
	LONGLONG	uidMaster;
};

///////////////////////////////////////////////////////////////////
// 生物移动消息
#define MSG_ACTION_MOVE											3

// 消息体
struct SMsgActionMove_CS
{
	DWORD		dwTimeMark;		// 时间戳

	DWORD		dwPathLen;		// 路径长度
	// ......................   // 路径节点，POINT * dwPathLen
};	

struct SMsgActionMove_SC
{
	bool        bAutoMove;      // 是否为自动寻路状态(针对人物的自动移动状态)
	DWORD		dwPathLen;		// 路径长度
	// ......................   // 路径节点，POINT * dwPathLen
};

struct SMsgActionMove_SS
{
	bool        bDelayCompensate;
	DWORD		dwPathLen;		// 路径长度
	// ......................   // 路径节点，POINT * dwPathLen
	SMsgActionMove_SS():
			bDelayCompensate(false),
			dwPathLen(0)
	{
	}
};

///////////////////////////////////////////////////////////////////
// 生物瞬间停在某个tile
#define MSG_ACTION_STOPHERE										4
// 消息体
// 如果要切换场景，此消息会智能处理
struct SMsgActionStopHere_SS
{
	DWORD		dwNewZoneID;	// 场景ID
	POINT		ptHere;			// 停留的位置	
};

struct SMsgActionStopHere_SC
{
	POINT		ptHere;			// 停留的位置

	char		szTTOO[128];	// 优化，未来要去掉
};

struct SMsgActionStopHere_CS
{
	POINT		ptHere;			// 停留的位置
};

///////////////////////////////////////////////////////////////////
// 实体增加了一个BUFF
#define MSG_ACTION_ADDBUFF										5
// 消息体
struct SMsgActionAddBuff_SC
{
	DWORD		dwIndex;		      // 序号
	DWORD		dwBuffID;		      // BUFF ID
	DWORD		dwLevel;		      // BUFF 等级
	DWORD		dwTime;			      // 时间长
	DWORD       dwCurOverlayTimes;    // Buff效果已经叠加的次数
	bool		bRandEffect;          // 是否有随机效果
};

///////////////////////////////////////////////////////////////////
// 实体移除了一个BUFF
#define MSG_ACTION_REMOVEBUFF									6
// 消息体
struct SMsgActionRemoveBuff_SC
{
	DWORD		dwIndex;		// 序号
};

///////////////////////////////////////////////////////////////////
// 实体死亡
#define MSG_ACTION_DIE											7
// 消息体
struct SMsgActionDie_SC
{

};

///////////////////////////////////////////////////////////////////
// 播 光效或音效
#define MSG_ACTION_PLAYEFFECT									8
// 消息体
struct SMsgActionPlayEffect_SC
{
	DWORD		dwFlashID;		// 出生光效
	DWORD		dwSoundID;		// 出生音效
};

///////////////////////////////////////////////////////////////////
// 使用非装备类物品
#define MSG_ACTION_USENONEQUIPABLEGOODS							9
// 消息体
struct SMsgActionUseNonequipableGoods_SS
{	
	LONGLONG	uidNonequipableGoods;	// 非装备类物品uid
	LONGLONG    uidUser;                // 使用者uid
};

////////////////////////// 技能消息码 ////////////////////////////
/// 使用技能(CC,CZ,ZZ)
/// 上下文：SkillContext
#define MSG_ACTION_USE_SKILL									10

/// 服务器发给客户端角色的技能列表数据(ZC)
/// 上下文：256字节技能列表
#define MSG_ACTION_UPDATE_SKILL_DATA							11

/// 服务器通知客户端蓄气完毕(ZC)
/// 上下文：无
#define MSG_ACTION_PREPARE_SKILL_OK								12

/// 服务器通知客户端可以使用XP技能了(怒气满了)
#define MSG_ACTION_CANUSE_XPSKILL								13


/// 服务器通知客户端技能被中断(ZC,ZZ)
/// 上下文：源UID
#define MSG_ACTION_BREAK_SKILL									14

/// 技能升级(学习也是升级)(CZ,ZC)
/// 上下文：4字节技能ID
#define MSG_ACTION_UPGRADE_SKILL								15

/// 服务器通知客户端技能使用失败(ZC)
/// 上下文：错误原因 UseSkillResult
#define MSG_ACTION_SKILL_FAILED									16

/// 服务器通知客户端XP技能超时不能再使用了(怒气减为0)
#define MSG_ACTION_XPSKILL_TIMEOUT								17
struct SMsgActionXPSkillTimeout_SC
{	
	LONGLONG	uidMaster;
	char        cXPStateFlag;
};

/// 客户端通知服务器XP在开始使用，以便服务器开始计时
#define MSG_ACTION_XPSKILL_BEGIN								18
struct SMsgActionSkillBegin_CS
{	
	LONGLONG	uidMaster;
	short       sSkillId;
};

///服务器通知客户端XP可以开始蓄气了
#define MSG_ACTION_XPSKILL_CANBOOT								19

////////////////////////// 伤害消息码 ////////////////////////////
/// 准备伤害
/// 上下文：DamageCalcContext
#define MSG_ACTION_PREPARE_DAMAGE								20

/// 伤害
/// 上下文：DamageContext
#define MSG_ACTION_DAMAGE										21

/// 准备治疗
/// 上下文：CureCalcContext
#define MSG_ACTION_PREPARE_CURE									22

/// 治疗
/// 上下文：CureContext
#define MSG_ACTION_CURE											23

// 当前提供正在使用的XP技能
#define MSG_ACTION_CURRENT_XP                                   30

#define MSG_ACTION_REFRESHSKILL									31
struct SMsgActionRefreshSkill
{
	int  nFreezeID;
};

// 采集物品特效播放
#define MSG_ACTION_PLAY_COLLECTEFFECT                           34
struct SMsgActionCollectEffectPlay
{	
	int      	nMonsterID;
	bool        bPlay;
};
// 准备采集读条
#define MSG_ACTION_PREPARE_COLLECT                              35
struct SMsgActionPrepareCollect
{	
	LONGLONG	uidTarget;
	long        lTargetType;
};

// 采集
#define MSG_ACTION_COLLECT                                      36

#define MSG_ACTION_BREAK_COLLECT                                37

/// 服务器通知客户端采集读条完毕(ZC)
#define MSG_ACTION_PREPARE_COLLECT_OK                           38

/// 服务器通知客户端采集失败(ZC)
#define MSG_ACTION_COLLECT_FAILED                               39

/////////////////////////////////////////////////////////////////
// 创建物品篮
#define MSG_ACTION_CREATESKEP									40
// 消息体
struct SMsgActionCreateSkep_SC
{	
	// 现场为 SBuildSkepClientContext
};

///////////////////////////////////////////////////////////////////
// 跟商人买物品
#define MSG_ACTION_BUYGOODS										41
// 消息体
struct SMsgActionBuyGoods_CS
{
	LONGLONG		uidNPC;		// NPC商人
	DWORD			dwSkepID;	// 通用物品篮ID

	LONGLONG		uidGoods;	// 买入物品
	int				nBuyNum;	// 买个数

	int				nPlace;		// 放置位置
};

///////////////////////////////////////////////////////////////////
// 向商人卖物品
#define MSG_ACTION_SALEGOODS									42
// 消息体
struct SMsgActionSaleGoods_CS
{
	LONGLONG		uidNPC;		// NPC商人
	DWORD			dwSkepID;	// 通用物品篮ID
	LONGLONG		uidGoods;	// 出售的物品
};

///////////////////////////////////////////////////////////////////
// 点击NPC对话
#define MSG_ACTION_MEETNPC										43
// 消息体
struct SMsgActionMeetNPC_CS
{
	LONGLONG		uidNPC;		// NPC的UID
};

///////////////////////////////////////////////////////////////////
// 人物外形
#define MSG_ACTION_PERSONFORM									44
// 消息体
struct SMsgActionPersonForm_SC
{
	// .....SPersonFormData		// 一段数据
};

///////////////////////////////////////////////////////////////////
// 观察怪物的归属
#define MSG_ACTION_OBSERVERHOLD									45
// 消息体
struct SMsgActionObserverHold_SC
{
	LONGLONG		uidMonster;	// 何个怪
	LONGLONG		uidHolder;	// 拥有者
	int				nTeamID;	// 队伍ID
};

struct SMsgActionObserverHold_CS
{
	LONGLONG		uidMonster;	// 何个怪
	bool			bObserver;	// 观察/取消
};

///////////////////////////////////////////////////////////////////
// 打开宝箱
#define MSG_ACTION_OPENBOX										46
// 消息体
struct SMsgActionOpenBox_CS
{
	LONGLONG		uidBox;		// 宝箱
};

///////////////////////////////////////////////////////////////////
// 关闭宝箱
#define MSG_ACTION_CLOSEBOX										47
// 消息体
struct SMsgActionCloseBox_CS
{
	LONGLONG		uidBox;		// 宝箱
};

///////////////////////////////////////////////////////////////////
// 修改阵营ID
#define MSG_ACTION_CHANGECAMP									48
// 消息体
struct SMsgActionChangeCamp_SC
{
	int				nCampID;	// 阵营ID
};

///////////////////////////////////////////////////////////////////
// 切换PK模式
#define MSG_ACTION_SWITCHPKMODE									49
// 消息体
struct SMsgActionSwitchPKMode_CS
{
	int				nPKMode;	// PK模式
};

struct SMsgActionSwitchPKMode_SC
{
	int				nPKMode;	// PK模式
};

///////////////////////////////////////////////////////////////////
// 修改PK名字颜色
#define MSG_ACTION_PKNAMECOLOR									50
// 消息体
struct SMsgActionPKNameColor_SC
{
	int				nPKColor;	// 名字颜色
	bool			bGrayName;	// 是否灰名
};

///////////////////////////////////////////////////////////////////
// 人物升级
#define MSG_ACTION_UPGRADE										51
// 消息体
struct SMsgActionUpgrade_CS
{
	LONGLONG lUpgrdaeUid; //升级生物的UID
	// .................
};	

///////////////////////////////////////////////////////////////////
// 冷却初始化数据
#define MSG_ACTION_INITFREEZE									52
// 消息体
struct SMsgActionInitFreeze_SC
{
	struct SNodeContext
	{
		BYTE		byClassID;	// 冷却类ID	
		int			nFreezeID;	// 冷却ID
		int			nFreezeTime;// 冷却剩余时间（毫秒）
	};

	// .................		// 多个SNodeContext
};

///////////////////////////////////////////////////////////////////
// 界面数据
#define MSG_ACTION_UIDATA										53
// 消息体
struct SMsgActionUIData_SC
{	
	BYTE			byData[UIDATA_LEN];// 界面数据
};

struct SMsgActionUIData_CS
{
	BYTE			byData[UIDATA_LEN];// 界面数据
};

///////////////////////////////////////////////////////////////////
// 客户端准备移动
#define MSG_ACTION_PREPMOVE										54
// 消息体
struct SMsgActionPrepMove_CC
{	
	DWORD			dwPathLen;		// 路径长度
	POINT *			pPath;			// 路径
};

///////////////////////////////////////////////////////////////////
// 实体复活
#define MSG_ACTION_RELIVE										55
// 消息体
struct SMsgActionRelive_SC
{
	LONGLONG uid;//.................
};

///////////////////////////////////////////////////////////////////
// 客户端站立当前位置
#define MSG_ACTION_STAND										56
// 消息体
struct SMsgActionStand_CC
{
	
};

///////////////////////////////////////////////////////////////////
// 增加一召唤兽
#define MSG_ACTION_PAWNADD										57
// 消息体
struct SMsgActionPawnAdd_SC
{	
	LONGLONG		uidPawn;		// 召唤兽
	DWORD			dwMonsterID;	// 怪物ID
};

///////////////////////////////////////////////////////////////////
// 杀掉一召唤兽
#define MSG_ACTION_PAWNKILL										58
// 消息体
struct SMsgActionPawnKill_SC
{
	LONGLONG		uidPawn;		// 召唤兽
};

///////////////////////////////////////////////////////////////////
// 召唤兽设置自己的头目
#define MSG_ACTION_PAWNLEADER									59
// 消息体
struct SMsgActionPawnLeader_SC
{
	LONGLONG		uidLeader;		// 头目
	char szLeader[MAX_PERSONNAME_LEN];
};

///////////////////////////////////////////////////////////////////
// 重设仓库密码
#define MSG_ACTION_RESETWAREHOUSEPASSWORD						60
// 消息体
struct SMsgActionResetWareHousePassword_SC
{
	LONGLONG		uid;		// 主角	
	char oldPassword[MAX_WAREHOUSEPASSWORD_LEN];
	char newPassword[MAX_WAREHOUSEPASSWORD_LEN];

	SMsgActionResetWareHousePassword_SC(void)
	{
		memset(this, 0, sizeof(SMsgActionResetWareHousePassword_SC));
	}
};


///////////////////////////////////////////////////////////////////
// 使用仓库
#define MSG_ACTION_OPENWAREHOUSE								61
// 消息体
struct SMsgActionOpenWareHouse_SC
{
	LONGLONG		uid;		// 主角
	char password[MAX_WAREHOUSEPASSWORD_LEN];
};

///////////////////////////////////////////////////////////////////
// 存取仓库金钱
#define MSG_ACTION_SAVEORGETWAREHOUSEMONEY						62
// 消息体
struct SMsgActionSaveOrGetMoney_SC
{
	LONGLONG		uid;		// 主角
	int             amount;		// 金钱数量
};

///////////////////////////////////////////////////////////////////
// 仓库操作通知
#define MSG_ACTION_WAREHOUSE_NOTIFICATION						63


///////////////////////////////////////////////////////////////////
// 宠物死亡
#define MSG_ACTION_PETKILL										64
// 消息体
struct SMsgActionPetKill_SC
{
	LONGLONG		uidPawn;		// 召唤兽
};

///////////////////////////////////////////////////////////////////
// 召回宠物
#define MSG_ACTION_PETCALLBACK									65

// 召唤宠物
#define MSG_ACTION_PETCONJURE									66
struct SMsgActionPetConjure_SC
{
	LONGLONG		uidPet;
	int             nSrcPlace;   // 源位子(以修改对应UI按钮)
	bool			isSuccess;
};

struct SMsgActionPetConjure_CS
{
	LONGLONG		uidPet;
	int             nSrcPlace;   // 源位子(以修改对应UI按钮)
};

// 宠物合体
#define MSG_ACTION_PETCOMBINATION								67
// 宠物骑乘
#define MSG_ACTION_PETRIDE										68
// 消息体
struct SMsgActionPetGeneral_SC
{
	LONGLONG		uidPet;		
	int             nSrcPlace;   // 源位子(以修改对应UI按钮)
};

struct SMsgActionPetRide_SC
{
	LONGLONG	uidPet;
	int         nSrcPlace;   // 源位子(以修改对应UI按钮)
	ulong		lPetID;		 // 宠物模型ID，其他客户端需要
};

///////////////////////////////////////////////////////////////////
// 控制装备栏的装备是否显示
#define MSG_ACTION_EQUIPVISIBLE									69
struct SMsgActionEquipVisible_CS
{
	BYTE	equip;//指明控制显示的装备
	BYTE	visible;//控制装备显示
	///消息体可以连续
};
//#define MSG_ACTION_SHOWGOODS									69
////消息体
//struct SMsgActionShowGoods_SC
//{
//	int nPlace ; //物品在物品栏的位置
//	bool bShow;//是否显示
//	///消息体可以连续。。。。。
//};

///////////////////////////////////////////////////////////////////
// 获取某个属性的所有排行
#define MSG_ACTION_SINGLEPETBILLBOARD							70
//消息体
struct SMsgActionReqSinglePetBillboard_SC
{
	LONGLONG uidPet;
};

///////////////////////////////////////////////////////////////////
// 获取某个宠物的所有属性的所有排行
#define MSG_ACTION_PETBILLBOARD									71
//消息体
struct SMsgActionReqPetBillboard_SC
{
	LONGLONG uidPet;
	ulong petPropID;
};

struct SMsgActionPetBillboardTable_SC
{
	long	pos;
	char	szPetName[MAX_PERSONNAME_LEN];
	char	szMasterName[MAX_PERSONNAME_LEN];
	short	stars;		
	long	propValue;
	bool	bHightlight;
	SMsgActionPetBillboardTable_SC()
	{
		memset(this,0,sizeof(SMsgActionPetBillboardTable_SC));
	}
};

///////////////////////////////////////////////////////////////////
// 获取某个宠物的鉴定评分
#define MSG_ACTION_PETAPPRAISAL									72

struct SMsgActionPetAppraisal_SC
{
	typedef std::pair<ulong,ulong>			PetPropGrade;
	typedef std::pair<ulong,PetPropGrade>	PetGrowthGrade;

	LONGLONG uidPet;	
	long			grade_Total_InitProp;		// 初始属性总分
	long			grade_Total_GrowthProp;		// 成长率总分
	long			grade_Total;				// 总分

	// 转世次数
	PetPropGrade	grade_TransCount;

	// 初始属性
	PetPropGrade	grade_InitHP;
	PetPropGrade	grade_InitMin_M_A;
	PetPropGrade	grade_InitMax_M_A;
	PetPropGrade	grade_InitMin_P_A;
	PetPropGrade	grade_InitMax_P_A;
	PetPropGrade	grade_Init_P_Defence;
	PetPropGrade	grade_Init_M_Defence;

	// 成长率
	PetGrowthGrade	grade_Growth_Rate[PET_TYPEPROP_MAX];

	SMsgActionPetAppraisal_SC(void)
	{
		memset(this, 0, sizeof(SMsgActionPetAppraisal_SC));
	}
};
// 修改宠物名字
#define  MSG_ACTION_CHANGPETNAME                                73
struct  SMgActionPetNameChange_SC
{
	int             nPlace;
	LONGLONG		uidPet;		// UID
	char			szPetName[MAX_PERSONNAME_LEN];
};

// 宠物解体(用SMsgActionPetGeneral_SC 结构)
#define  MSG_ACTION_DISBANDPET                                  74
// 宠物下马 (用SMsgActionPetRide_SC 结构)
#define  MSG_ACTION_DISMOUNTPET                                 75
// 宠物自动释放技能添加消息
#define MSG_ACTION_ADDAUTOSKILL                                 76
// 宠物自动释放技能移除消息
#define MSG_ACTION_REMOVEAUTOSKILL                              77
struct SMgActionPetSkillCommon_SC
{ 
	LONGLONG lUid;         // 技能释放者UID
	int  nSkillId;         // 自动释放技能ID数组
	bool bSuccess;         // 成功
}; 
struct SMgActionPetAtuoSkill_CS
{
	LONGLONG lUid;         // 技能释放者UID
	int  nSkillId;         // 自动释放技能ID数组
	int  nPlace;           // 对应的位子
};
struct SMgActionPetAtuoSkill_SC
{
	LONGLONG lUid;         // 技能释放者UID
	int  nSkillId;         // 自动释放技能ID数组
	int  nPlace;           // 对应的位子
};
#define  MSG_ACTION_PET_FORMATION                               78
struct SMgActionPetSetFormation_CS
{
	int		nformation;		// 切换的阵法
};
struct SMgActionPetSetFormation_SC
{
	int		nformation;		// 阵法
	bool	bIsActived;		// 是否激活
	int		nGrade[5];		// 阵法等级	add by zjp
};

// 宠物解封消息
#define  MSG_ACTION_PETRELEASE_START                            79
struct SMgActionPetRelease_SC 
{
	LONGLONG   uid;         // UID
	ulong      lPackId;     // 包裹ID
	int        nPlace;      // 位置
};

// 宠物进化
#define MSG_ACTION_PETEVOLUTION									80
struct SMgActionPetEvolution_SC
{
	DWORD			dwSkepID;	// 通用物品篮ID
	LONGLONG		uidPet;
	ulong			PetID;
};

// 宠物进化
#define MSG_ACTION_SETAUTOSORT									81
struct SMgActionPetAutoSort_SC
{
	bool			bAutoSort;
};
// 宠物技能学习(SMgActionPetSkillCommunal_SC)
#define MSG_ACTION_PETLEARN_SKILL								82
// 宠物技能删除(SMgActionPetSkillCommunal_SC)
#define MSG_ACTION_PETDELECT_SKILL								83

#define MSG_ACTION_EQUIPSMELTSOUL								84
struct SMsgActionEquipSmeltSoul_CS
{
	uchar	ucType;//链魂的消息类型,见DGlobalMis
	int		nPerNum;//每次使用的数量
	int		nCount;//提炼次数
	int		nLevel;// add by zjp;魂石等级
	int		nSoulNum1; // 装备炼魂消耗的魂魄数 add by zjp
	int		nSoulNum2;
};

#define MSG_ACTION_EQUIPEMBEDGEMS								85
struct SMsgActionEquipEmbedGems_CS
{
	uchar  ucType;//灵石镶嵌的消息类型,见DGlobalMis
	uchar  ucDetachSlot;//用于装备除灵
};

#define MSG_ACTION_EQUIPREBUILD									86
struct SMsgActionEquipRebuild_CS
{
	enum { RESERVE_ATTRIB_NUM_MAX = 4,};// modify by zjp
	uchar ucType; //装备重铸的消息类型,见DGlobalMis
	uchar ucReserveAttribNum;//保留的品质属性数量
	uchar ucReserveBrandAttrib[RESERVE_ATTRIB_NUM_MAX];//保留的品质属性
};

#define MSG_ACTION_REPAIREQUIPMENT								87
struct SMsgActionRepairEquipment_CS
{
	ulong	ulSkepID;//装备篮id
	int		iPlace;//装备所在篮的位置,iPlace表示修理所有装备
};

//by cjl
#define MSG_ACTION_IDENTIFYEQUIPMENT							88
struct SMsgActionIdentifyEquipment_CS//客户端发给服务端的消息结构
{
	ulong	ulSkepID;//装备所在的篮的id
	int		iPlace;//装备所在篮的位置,iPlace表示修理所有装备，当用于鉴定时，为PACKET_ALL_EQUIP_PLACE表示鉴定所有
	//可能需要扩展
	int    iUserID;//使用者ID，npc时为-1，道具时为它的id
};

struct SMsgActionIdentifyEquipment_SC//验证结果
{
	//-5 使用道具鉴定单个装备
	//-2 其他错误
	//-1 没有可鉴定的（用于鉴定全部）
	// 0 鉴定成功
	// 1 获取不了物品
	// 2 不是装备
	// 3 已经鉴定过了
	// 4 表里面没有装备的数据
	// 5 不够钱
	int res;
	int iUserID;//使用者ID
	int needMoney;
	int equipmentID;
};

#define MSG_ACTION_CHANGE_BODY									89
struct SMgActionChangeBody_SC
{
	int id;  //模型id
};
//end cjl

// 打开宠物合成面板
#define MSG_ACTION_OPENPETCOMPOUND								90
struct SMsgActionNPCOpenForm_SC
{
	LONGLONG uidNPC;
};

// 宠物合成
#define MSG_ACTION_PETCOMPOUND									91
struct SMsgActionPetCompound_CS
{
	LONGLONG uidmainpet;	// 主宠
	LONGLONG uidminorpet;	// 副宠
	DWORD	 dwtype;		// 炼魂（合成）类型
	bool	 bUseGoods;		// 使用道具
};

struct SMsgActionPetCompound_SC
{
	struct CompdResultItem
	{
		DWORD dwPropID;
		long  lOrgValue;
		long  lCurValue;

		CompdResultItem(DWORD _dwPropID,long _lOrgValue,long _lCurValue)
		{
			dwPropID = _dwPropID;
			lOrgValue = _lOrgValue;
			lCurValue = _lCurValue;
		}	

		CompdResultItem():dwPropID(0),lOrgValue(0),lCurValue(0)
		{}
	};

	DWORD dwError;
	LONGLONG  uidPet;;
	CompdResultItem results[14];	// 炼魂结果 初始属性 + 成长率属性

	SMsgActionPetCompound_SC()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 主人命令宠物做某事
#define MSG_ACTION_PET_COMMAND									92

// 主人取消命令宠物
#define MSG_ACTION_PET_CANCELCOMMAND							93
struct SMsgActionPetCommand_CS
{
	DWORD		dwCmdID;
	LONGLONG	uidPet;
	LONGLONG	uidMaster;
	LONGLONG	uidTarget;
	POINT		ptTarget;
};

struct SMsgActionPetCommand_SC
{
};

// 通知客户端删除自己的宠物实体
#define MSG_ACTION_PET_DELETE									94
struct SMsgActionPetDelete
{
	LONGLONG uidPet;
};

// 客户端主角切换移动方式
#define MSG_ACTION_CHANGEMOVESTYLE								95
struct SMsgActionChangeMoveStyle_CS
{
	// 新的移动方式
	char    cMoveStyle;
};

///////////////////////////////////////////////////////////////////
// 播 光效或音效
#define MSG_ACTION_PLAYGENERALEFFECT							96
// 消息体
struct SMsgActionPlayGeneralEffect_SC
{
	DWORD		dwFlashID;		// 光效
	DWORD		dwSoundID;		// 音效
	DWORD		dwZoneID;		// 场景
	DWORD		dwMapID;		// 地图
	POINT		ptLoc;			// 地点
};

// add by zjp.泡泡爆炸
#define  MSG_ACTION_BUBBLE_BOMB									97
struct SMsgActionBubbleBomb_SC
{
	DWORD		dwZoneID;		// 场景
	POINT		ptLoc;			// 爆炸地点
	short       sSkillId;		        // 技能大类ID
	short       sSkillLevel;	        // 子技能Id，用于角色时指等级
};

// 播空闲动作
#define  MSG_ACTION_IDLE										98
struct SMsgActonIdle_SS
{
};

struct SMsgActonIdle_SC
{
};

// add by zjp.鬼魂攻击
#define  MSG_ACTION_GHOST_ATTACK								99
struct SMsgActionBubbleBomb_SS
{

};


// 自动寻路消息
#define  MSG_ACTION_AUTOMOVE									100
struct SMsgActionAutoMove_SS
{
	int  nStartMoveMapID;
	int  nCurStart_x;   // 当前起始位置X
	int  nCurStart_y;   // 当前起始位置X
	int  nCurEnd_x;     // 当前需要结束的X
	int  nCurEnd_y;     // 当前需要结束的Y

	int  nEndMoveMapID; // 传送点到达的地图ID
	int  nEndPoint_x;   // 传送点到达的坐标点
	int  nEndPoint_y;   // 

	int  m_nAutoMoveMapID; // 寻路最终的地图ID
	int  nEndAutoMove_x;   // 寻路最终的坐标
	int  nEndAutoMove_y;   // 
};
struct SMsgActionAutoMove_SC
{
	bool m_AutoMove;      // 是否为自动寻路
	int  nStartMoveMapID; // 起始地图ID
	int  nCurStart_x;     // 当前起始位置X
	int  nCurStart_y;     // 当前起始位置X
	int  m_nAutoMoveMapID; // 寻路最终的地图ID
	int  nEndAutoMove_x;   // 寻路最终的坐标
	int  nEndAutoMove_y;   // 
};

// 绘制自动寻路路径
#define  MSG_ACTION_DRAWAUTOMOVEPATH                             101
struct  SMsgActionDrawAutoMovePath_SC
{
	bool       bTaskTrace; //任务追踪
	POINT      ptDest;
};

// add by zjp；死亡复活方式
#define  MSG_ACTION_HOME_RELIVE									102
#define  MSG_ACTION_LOCAL_RELIVE								103
#define  MSG_ACTION_MAP_RELIVE									104

//更新角色基本信息请求
#define	MSG_ACTION_UPDATEBASICINFO								105
struct	SMsgActionUpdateBasicInfo_CS
{
	int	province;//省份
	int city;//城市		
	int constellation;//星座
	int bloodtype;//血型
	char signature[ACTOR_SIGNATURE_MAX_LEN];//签名
};

// 获取它人装备信息
#define MSG_ACTION_REQUESTEQUIPINFO								106
struct SMsgActionRequestEquipInfo_CS
{
	ulong ulUserPID;
	ulong ulPIDDest;  // 目标的pid
};

struct SMsgActionRequestEquipInfo_SS
{
	ulong ulUserPID;
	ulong ulPIDDest;  // 目标的pid
};

#define MSG_ACTION_PROPINFO										107	
struct SMsgActionPropInfo
{
	DWORD		dwViewPDBID;			    // 查看者
	DWORD       dwReviewPDBID;              // 被查看者
	char        szName[32];					// 被查看者的名字
	DWORD       dwLevel;                    // 等级
	DWORD       dwSex;                      // 性别
	DWORD       dwVocation;                 // 职业
	DWORD       dwNation;                   //  国籍（阵营）
	DWORD       dwNakedResId;               // 裸体模型ID
	DWORD       dwCurHP;                    // 当前生命值
	DWORD       dwMaxHP;                    // 最大生命值
	DWORD       dwCurMP;                    // 当前魔法值
	DWORD       dwMaxMP;                    // 最大魔法值
	DWORD       dwCurSP;                    // 当前体力值
	DWORD       dwMaxSP;                    // 最大体力值
	DWORD       dwMinPActValue;             // 最小攻击力
	DWORD       dwMaxPActValue;             // 最大攻击力
	DWORD       dwPDefence;                 // 物理防御值
	DWORD       dwMinMActValue;             // 最小魔攻
	DWORD       dwMaxMActValue;             // 最大魔攻
	DWORD       dwMDefence;                 // 魔法防御值
	DWORD       dwSpeed;                    // 移动速度
	DWORD       dwFatal;                    // 会心值
	DWORD		dwKickFatal;				// 御劲值
	DWORD       dwPersent;                  // 命中值
	DWORD       dwHedge;                    // 闪避值
	DWORD       dwFrightValue;              // 战斗力
	DWORD       dwFaceID;                   // 头像ID
	DWORD       dwProvince;                 // 省份
	DWORD       dwCity;                     // 城市
	DWORD       dwConstellation;            // 星座
	DWORD       dwBloodType;                // 血型
	char        Signature[256];             // 签名
	LONGLONG    uID;                        // UID
	SMsgActionPropInfo(void)
	{
		memset(this, 0, sizeof(*this));
	};
};
#define MSG_ACTION_EQUIPINFO									108
struct SMsgActionEquipInfo_SC
{
	LONGLONG       uID;                        // 
	DWORD		dwViewPDBID;			    // 查看者
	DWORD       dwReviewPDBID;              // 被查看者
	DWORD       dwPlace;                    // 被查看的装备的位子	
	SMsgActionEquipInfo_SC(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define  MSG_ACTION_CLOSESHOP									109
struct SMsgActionCloseShop_SC
{
};

#define  MSG_ACTION_JUMP										110
struct SMsgActionJump
{
	xs::Point		ptTarget;				 // 目标位置
};

//内丹合成预测信息，  by ldy
#define MSG_ACTION_COMPOSE									112
struct SProbability
{
	int	iComposeLevel;			//合成内丹等级
	int fProbability;			//合成几率
};
struct SMsgActionComposePill_CS
{
	DWORD			dwSkepID;	//物品栏ID
	int				iFee;		//合成费用
	SProbability	Probability[2];//合成内丹
};
//内丹合成确认
#define MSG_ACTION_COMPOSE_CONFIRM							113
struct SMsgActionComposeConfirm_CS
{
	uchar			ucType;
};

//魂将圣化预测信息， by ldy
#define MSG_ACTION_INTENSIFY								114
struct SMsgActionPetIntensify_CS
{
	DWORD		dwSkepID;		//物品栏ID
	int			iFee;			//圣化费用
	int			iProbability;	//圣化成功率
	int			iStart;			//圣化后魂将星级
};

//魂将圣化确认， by ldy
#define MSG_ACTION_INTENSIFY_CONFIRM						115	
struct SMsgActionPetIntensifyConfirm_CS
{
	uchar			ucType;
};

// 同步客户端的宠物出征数目
#define MSG_ACTION_PET_FIHGT_NUM							116	
struct SMsgActionPetFightNum_SC
{
	uchar ucFightNum;
};

// 测试用 用来显示范围性技能的tile格子
#define MSG_ACTION_SKILL_TILE								117
struct SMsgActionSkillTile
{
	POINT pt;
};

// 宠物技能经验更新
#define MSG_ACTION_PET_UPDATE_SKILL_EXPRIENCE					127
struct SMsgActionPetUpdateSkillExprience
{
	UID uidPet; // 宠物的UID
	short exprience; // 宠物更新后的技能经验
	short sSkillId; // 宠物的当前使用技能ID
	short sSkillLevel; // 宠物当前技能的等级
};

// 宠物解封消息
#define  MSG_ACTION_PETRELEASE_FINISH                             128

// 修改宠物阵法方位
#define	MSG_ACTION_PET_SETPROP									129
struct SMsgActionPetSetProp_CS
{
	LONGLONG uidPet;
	DWORD dwPropID;
	DWORD dwPropValue;
};

// 填充宠物自动释放技能数据
#define  MSG_ACTION_PET_ADDAUTOSKILL                           130
struct SMsgActionPetAddAutoSkill_SC
{
	LONGLONG uidPet;
	int      nSkillId[PET_ATUO_SKILL_DEFAULTSIZE];
};

// 切换跟随状态去改变人物的移动速度；
#define MSG_ACTION_CHANGE_FOLLOWSPEED                           131
struct SMsgActionChangeFollowSpeed_CS
{
	LONGLONG  uidPerson;
	bool      bFollow;
	int       nSpeed;
};


// 播 动作
#define MSG_ACTION_DOCOMMAND									132
struct SMsgActionDoCommand_sc
{
	int       nCommand;
};

// 
#define MSG_ACTION_HIDE										133
struct SMsgActionHide_sc
{
	ushort usHide; // 1 hide， 0 unhide
};

// 拾取物品成功通知客户端去绘制拾取效果
#define MSG_ACTION_ADDGOODS_SUCCESS							 134
struct SMsgActionAddGoodsSuccess_SC
{
	LONGLONG uidItem;
	bool     bSystemAdd;    //是否为系统添加
};

#define MSG_ACTION_FLY_RIDE                                  135

#define MSG_ACTION_DIS_FLY_RIDE                              136
struct SMsgActionFlyRide_SC
{
	ulong		lFlyPetID;		 // 宠物模型ID，其他客户端需要
	ulong       lTaskID;         // 相关的任务ID；
	ulong       lNpcID;          // 相关的NPCID；
	ulong       lCurMoveSpeed;   // 人物当前的移动速度；
};

//丢弃骑宠
#define MSG_ACTION_DROPRIDEPET								   137
struct SMsgActionDropRidePet_cs
{
	int nPlace;
};

struct SMsgActionDropRidePet_sc
{
	int nPlace;
};

// 采集NPC跳跃
#define  MSG_ACTION_COLLECT_JUMP                             138
struct SMsgActionCollectJump_SC
{
	int nEndPtX;
	int nEndPtY;
	int nTaskID;
};

struct SMsgActionCollectJump_CS
{
	LONGLONG uidPerson;
	int nTaskID;
};
// 播放NPC头顶特效
#define  MSG_ACTION_PLAY_EFFECT_TOPMOST                      139
struct SMsgActionPlayEffectTopMost_SC
{
	char      szTipText[64];             // 提示文字
	LONGLONG  uidMonster;
	ulong     ulEffectID;
	bool      bNeedClick;
};

// 播放NPC头顶特效
#define  MSG_ACTION_STOP_EFFECT_TOPMOST                      140
struct SMsgActionStopEffectTopMost_SC
{
	LONGLONG  uidMonster;
	ulong     ulEffectID;
};

// 停止 光效
#define MSG_ACTION_STOPEFFECT								 141
// 消息体
struct SMsgActionStopEffect_SC
{
	DWORD		dwFlashID;		// 光效
};

// 宠物过期验证
#define MSG_ACTION_CHECKPETRIDE								 142
// 消息体
struct SMsgActionCheckPetRide_cs
{
	LONGLONG	uidPet;
	int         nSrcPlace;   
};

struct SMsgActionCheckPetRide_sc
{
	LONGLONG	uidPet;
	int         nPlace;
	short		check;  
};
//登录服知会场景服，未成年保护提示
#define MSG_ACTION_ENTHRALL_PROTECT_TIPS					143
struct SMsgActionEnthrallProtectTips
{
	DWORD dwDBID;
	int iSumSeconds;
	int iIncomePercent;//防沉迷保护 当前收益百分比,正常情况下为100 ，在线3-5小时为50  5小时候为0
};

//洗天赋
#define MSG_ACTION_RESET_GENIUS                             144

//打开加入阵营面板
#define MSG_ACTION_JOIN_CAMP_WND							145
struct SMsgActionCampOrbit
{
	int nSong;
	int nYi;
};

//打开改变阵营面板
#define MSG_ACTION_CHANGE_CAMP_WND							146
struct SMsgActionCampChange
{
	int nEddToStrongMoney;		// 弱转强需要的money
	int nStrongToEddMoney;		// 强转弱需要的money
	int nStrongCampID;			// 强势阵营ID
	SMsgActionCampOrbit orbit;
};


//场景服向登录服请求查询在线人数
#define MSG_ACTION_QUERY_CLIENT_COUNT						147
struct SMsgActionQueryClientCount_ZL
{
	DWORD	dwClientID;
};

#define MAX_ACTORNAME_LEN									32
//场景服向登录服请求修改角色名
#define MSG_ACTION_ACTOR_RENAME								148
struct SMsgActionActorReName_ZL			
{
	DWORD dwClientID;
	int	  nActorID;
	char  szActorName[MAX_ACTORNAME_LEN];
	char  szNewActorName[MAX_ACTORNAME_LEN];
};

#define MSG_ACTION_BREAKPETRIDE								 149
#define MSG_ACTION_OPENPACKET								 150//by cjl

///////////////////////////////////////////////////////////////////
// 最大消息码
#define MSG_ACTION_MAXID									 151



//////////////////////////////////////////////////////////////////
//副本
#define MSG_RESET_ALL_INSTANCE_ZONE                            1




/**				客户端主动发起退出登录，			
@name         : 退出登录
注意，根据当前服务器的架构，该消息其实是发给网关MSG_ENDPOINT_GATEWAY，网关需解析该条消息，以便断开与客户端的连接
@brief        : 
@return       : 
**/
#	define    MSG_CLIENT_LOGOUT           34


/* 客户端主动从游戏返回角色选择，现有的角色游戏逻辑需中断，重新返回角色信息
*/
#	define   MSG_RE_SELECT_ACTOR         35
struct SMsgReSelectActor_SS
{
	DWORD dwClientID; //客户端网关处ID
};

struct SMsgDynamicFollow_S
{
	ULONGLONG uidTarget;
	int       nFollowEyeShot;
	int		  nFollowRange;
};

struct SMsgDynamicPatrol_S
{
	BYTE      bLoop;
	BYTE      bForceChange;
	BYTE      bIgnoblock;
	int       nRange;
	int       nPatrolPointNum;
	int		  nLen;
	//...
	SMsgDynamicPatrol_S()
	{
		memset(this, 0, sizeof(SMsgDynamicPatrol_S));
	}
};

/////////////////////////////////////////////////////////////////
//super center

#define MSG_SERVERLIST_STATUS_REQUEST  10001

#define MSG_SERVERLIST_STATUS_RESPONSE  10002
struct SResponseServerListStatus
{
	DWORD dwAddr;
	WORD  wPort;
	DWORD dwOnlineNum;

	bool operator < (const SResponseServerListStatus& other)
	{
		if (dwAddr < other.dwAddr)
		{
			true;
		}
		else if (dwAddr == other.dwAddr)
		{
			return wPort < other.wPort;
		}

		return false;
	}

	bool operator ==(const SResponseServerListStatus& other)
	{
		return dwAddr == other.dwAddr && wPort == other.wPort;
	}
};

//////////////////////////////////////////////////////////////////
//商城
#define MSG_ONLINE_SHOP_BUY_GOODS_REQUEST		  20001
struct SOnlineShopBuyGoodsReqest
{
	int iGooodsID;
	int iCount;
};

#define MSG_ONLINE_SHOP_BUY_GOODS_RESPONSE		  20002
const int CONST_ONLINE_SHOP_MAX_GOODS_QUANTITY = 999;
enum EOnlineShopBuyGoodsResult
{
	E_ONLINE_SHOP_BUY_GOODS_RESULT_SUCCESS = 0,
	E_ONLINE_SHOP_BUY_GOODS_RESULT_INPUT_ERROR,
	E_ONLINE_SHOP_BUY_GOODS_RESULT_GOODS_NOT_FOUND,
	E_ONLINE_SHOP_BUY_GOODS_RESULT_NEED_MORE_BAOBI,
	E_ONLINE_SHOP_BUY_GOODS_RESULT_NEED_MORE_JIAOZI,
	E_ONLINE_SHOP_BUY_GOODS_RESULT_PACKET_FULL,
	E_ONLINE_SHOP_BUY_GOODS_RESULT_DATABASE_ERROR,
	E_ONLINE_SHOP_BUY_GOODS_RESULT_PROGRAM_LOGIC_ERROR,//不应该出现的编程逻辑错误

	E_ONLINE_SHOP_BUY_GOODS_RESULT_MAX_ENUM,//枚举最大值
};
struct SOnlineShopBuyGoodsResponse
{
	int iGooodsID;
	int iCount;
	EOnlineShopBuyGoodsResult eResult;
};

//获取历史购买记录
#define MSG_ONLINE_SHOP_GET_HISTORY_REQUEST 20003

#define MSG_ONLINE_SHOP_GET_HISTORY_RESPONSE 20004
struct SOnlineShopHistoryBuyResponse
{
	int   GoodsIDArray[ONLINE_SHOP_MAX_HISTORY_PAID_RECORDS];
};

//////////////////////////////////////////////////////////////////
#pragma pack()