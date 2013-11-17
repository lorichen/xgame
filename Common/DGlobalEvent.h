/*******************************************************************
** 文件名:	e:\Rocket\Common\DGlobalEvent.h
** 版  权:	(C) 
** 
** 日  期:	2008/2/13  15:10
** 版  本:	1.0
** 描  述:	事件码定义
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#pragma pack(1)

//////////////////////////发送源类型 ///////////////////////////
/// 发送源类型
enum
{
	SOURCE_TYPE_UNKNOW = 0,			// 类型ID根
	SOURCE_TYPE_PERSON,				// 玩家实体
	SOURCE_TYPE_PET,				// 宠物实体
	SOURCE_TYPE_MONSTER,			// 怪物实体
	SOURCE_TYPE_GOODS,				// 物品实体
	SOURCE_TYPE_BOX,				// 宝箱实体
	SOURCE_TYPE_MAST,				// 旗杆实体
	SOURCE_TYPE_BUBLLE,				// 泡泡实体 add by zjp
	SOURCE_TYPE_FREEZE,				// 冷却系统
	SOURCE_TYPE_SKEP,				// 通用物品篮
	SOURCE_TYPE_SYSTEM,				// 系统事件
	SOURCE_TYPE_CHAT,				// 聊天事件
	SOURCE_TYPE_TEAM,				// 组队事件
	SOURCE_TYPE_INSTANCE_ZONE,      // 副本事件
	SOURCE_TYPE_FRIEND,             // 好友事件
	SOURCE_TYPE_STALL,              // 摆摊事件
	SOURCE_TYPE_FAMILY,             // 家族事件
	SOURCE_TYPE_ARMY,               // 军团事件
	SOURCE_TYPE_MAX,
};

//////////////////////////////////////////////////////////////////
// 定义规则 EVENT + 发送源层 + 具体涵义
//////////////////////////// 事件码 //////////////////////////////
/***************************************************************/
///////////////////////// 实体事件码 ////////////////////////////
/***************************************************************/
// 实体添加BUFF
#define EVENT_CREATURE_ADDBUFF									1
// 场景服
struct SEventCreatureAddBuff_S
{
	DWORD			dwBuffID;		// buff id
	DWORD			dwLevel;		// 等级
	LONGLONG		uidEffect;		// 本buff的作用对像
	LONGLONG		uidAdd;			// 本buff的添加者
	DWORD			dwBuffFlag;		// buff标志
};
// 客户端
struct SEventCreatureAddBuff_C
{
	DWORD			dwIndex;		// buff index
	LONGLONG        ulUid;          // buff Target uid
};

// 实体移除BUFF
#define EVENT_CREATURE_REMOVEBUFF								2
// 客户端
struct SEventCreatureRemoveBuff_C
{
	DWORD			dwIndex;		// buff index
	LONGLONG        ulUid;          // buff Target uid
};

// 生物死亡
#define EVENT_CREATURE_DIE										3
// 场景服
struct SEventCreatureDie_S
{
	LONGLONG		uidMurderer;	// 凶手
};

// 客户端
struct SEventCreatureDie_C
{

};

// 主动攻击
#define EVENT_CREATURE_ATTACK									4
// 场景服
struct SEventCreatureAttack_S
{
	LONGLONG		uidTarget;		// 攻击目标
	DWORD			dwUseSkillID;	// 使用技能ID
};

// 伤害他人
#define EVENT_CREATURE_HARM										5
// 场景服
struct SEventCreatureHarm_S
{
	LONGLONG		uidCasualty;	// 受害者
	DWORD			dwHPDamage;		// HP伤害
	DWORD			dwMPDamage;		// MP伤害		
};

struct SEventCreateHarm_C
{
	LONGLONG		uidSrc;			// 攻击者
	LONGLONG		uidDest;		// 被攻击者
};

// 被击
#define EVENT_CREATURE_INJURED									6
// 场景服
struct SEventCreatureInjured_S
{
	LONGLONG		uidMurderer;	// 凶手
	DWORD			dwHPDamage;		// HP伤害
	DWORD			dwMPDamage;		// MP伤害
};

// 客户端
struct SEventCreatureInjured_C
{
	LONGLONG		uidMurderer;	// 凶手	
};


// 改变位置，包括STOPHERE和切换地图
#define EVENT_CREATURE_CHANGELOC								7
// 场景服
struct SEventCreatureChangeLoc_S
{
	DWORD			dwOldZoneID;	// 旧场景ID
    xs::Point			ptOldTile;		// 旧Tile
	DWORD			dwNewZoneID;	// 新场景ID
	xs::Point			ptNewTile;		// 新Tile
	bool			bSameServer;	// 旧场景与新场景是否为同服务器
};

// 开始移动，移动过程中，不是此事件
#define EVENT_CREATURE_STARTMOVE								8
// 场景服
struct SEventCreatureStartMove_S
{

};

// 客户端
struct SEventCreatureStartMove_C
{

};

// 移动完成
#define EVENT_CREATURE_DONEMOVE									9
// 场景服
struct SEventCreatureDoneMove_S
{

};

// 客户端
struct SEventCreatureDoneMove_C
{

};

// 实体更新属性
#define EVENT_ENTITY_UPDATEPROP									10
// 客户端
struct SEventEntityUpdateProp_C
{
	LONGLONG uid ;  // 需要更新实体的UID(wzh 09.12.07)
};
// 服务器端，服务器端不会批量修改
struct SEventEntityUpdateProp_S
{
	LONGLONG uid;
	DWORD    dwPropID;
};

// 创建实体
#define EVENT_ENTITY_CREATEENTITY								11
// 场景服
struct SEventEntityCreateEntity_S
{
	LONGLONG		uidEntity;		// 实体uid
};
// 客户端
struct SEventEntityCreateEntity_C
{
	LONGLONG		uidEntity;		// 实体uid
};

// 删除实体
#define EVENT_ENTITY_DESTROYENTITY								12
// 场景服
struct SEventEntityDestroryEntity_S
{
	LONGLONG		uidEntity;		// 实体uid
};
// 客户端
struct SEventEntityDestroryEntity_C
{
	LONGLONG		uidEntity;		// 实体uid
};

// 使用非装备类物品
#define EVENT_ENTITY_USENONEQUIPABLEGOODS						13
// 客户端
struct SEventEntityUseNonequipableGoods_C
{
	LONGLONG	uidGoods;			// 物品UID
	LONGLONG    uidUser;            // 使用者UID 
	DWORD		dwSkepID;			// 物品篮ID
	DWORD		dwPlace;			// 物品位置
};

// 玩家上线
#define EVENT_PERSON_LOGIN										14
// 场景服
struct SEventPersonLogin_S
{
	LONGLONG		uidEntity;		// 实体uid
};

// 玩家下线
#define EVENT_PERSON_LOGOUT										15
// 场景服
struct SEventPersonLogout_S
{
	LONGLONG		uidEntity;		// 实体uid
};

// 切换PK模式
#define EVENT_PERSON_SWITCHPKMODE								16
// 客户端
struct SEventPersonSwitchPKMode_S
{
	int				nPKMode;		// PK模式
};

// 人物升级
#define EVENT_PERSON_UPGRADE									17
// 场景服
struct SEventPersonUpgrade_S
{
	// ...............
};

// 点击NPC
#define EVENT_PERSON_MEETNPC									18
// 场景服
struct SEventPersonMeetNpc_S
{
	LONGLONG		uidMaster;		// 谁点的NPC
	LONGLONG		uidNPC;			// 何个NPC
};

// 增加一宠物型召唤兽
#define EVENT_PERSON_PAWNADDPET									19
// 客户端
struct SEventPersonPawnAddPet_C
{
	LONGLONG		uidPawn;		// 召唤兽UID
	DWORD		dwMonsterID;		// 怪物ID
};

// 释放一宠物型召唤兽
#define EVENT_PERSON_PAWNKILLPET								20
// 客户端
struct SEventPersonPawnKillPet_C
{
	LONGLONG		uidPawn;		// 召唤兽UID
	DWORD		dwMonsterID;		// 怪物ID
};

// 设置人物的装备炼魂栏的当前子栏
#define EVENT_PERSON_EQUIPSMELTSOULCURSUBSKEP					21
struct SEventPersonEquipSmeltSoulCurSubskep_S
{
	ushort usCurSubskep;
};

// 设置人物的装备宝石镶嵌栏的当前子栏
#define EVENT_PERSON_EQUIPEMBEDGEMSCURSUBSKEP					22
struct SEventPersonEquipEmbedGemsCurSubskep_S					
{
	ushort usCurSubskep;
};

// 设置人物的装备重铸栏的当前子栏
#define EVENT_PERSON_EQUIPREBUILDCURSUBSKEP						23
struct SEventPersonEquipRebuildCurSubskep_S
{
	ushort usCurSubskep;
};

// 暴击，闪避共用一个结构
#define  EVENT_CREATURE_CRAZYDAMAGE								24

// 被暴击
#define  EVENT_CREATURE_BECRAZYHIT								25

// 闪避，暴击共用一个结构
#define EVENT_CREATURE_HEDGE									26

// 被闪避
#define EVENT_CREATURE_BEHEDGE									27

struct SEventCreatureDamageAction_S
{
	LONGLONG uidMurderer;
	LONGLONG uidCasualty;
};

// 设置主人事件
#define EVENT_CREATURE_SETLEADER								29
struct SEventCreatureSetLeader_S
{
	LONGLONG uidMaster;
	LONGLONG uidSlave;
};

// 生物复活
#define EVENT_CREATURE_RELIVE                                   30

// 生物移动速度或移动方式改变
#define EVENT_CREATURE_CHANGESPEED                              31


// 跳地图从一台服务器服务器转入到另一台服务器，转入时类似玩家上线 EVENT_PERSON_LOGIN
#define EVENT_PERSON_CHANGE_SERVER_LOGIN						32

// 跳地图从一台服务器服务器转入到另一台服务器， 转出时类似玩家下线 EVENT_PERSON_LOGOUT
#define EVENT_PERSON_CHANGE_SERVER_LOGOUT						33
struct CEventPersonChangeServerLogOut_S
{
	LONGLONG		uidEntity;		// 实体uid
	DWORD           dwDstServerID;  // 目标服务器ID
};

// 切换阵法
#define EVENT_PET_SETFORMATION									34
struct CEventSetPetFormation_C
{
	int     nFormation;
};	

// 候选阵法列表更新
#define EVENT_PET_FORMATIONLIST									35
struct CEventPetFormationList_C
{
	bool	nFormationEnable[6];
};

// 自动移动事件
#define  EVENT_PERSON_AUTOMOVE                                  36
struct CEventPersonAutoMove_C
{
	bool  bAtuoMove;
	int  nStartMoveMapID; // 起始地图ID
	int  nCurStart_x;     // 当前起始位置X
	int  nCurStart_y;     // 当前起始位置X
	int  m_nAutoMoveMapID; // 寻路最终的地图ID
	int  nEndAutoMove_x;   // 寻路最终的坐标
	int  nEndAutoMove_y;   // 
};

// 绘制自动移动路径
#define  EVENT_PERSON_DRAWAUTOMOVE								37
struct CEventPersonDrawAutoMove_C
{
	bool bTaskTrace; //任务追踪 
	xs::Point ptDest;
};
// 绘制地图上队员图标
#define  EVENT_PERSON_DRAWTEAMMEMBERPLACE						38
struct CEventTeamViewMemeberMapPlace_C
{
	int     nMapId;           // 需要显示地图ID
	int     nMemberCount;     // 存在的队员个数
};

/************************************************************************/
/*  宠物通用行为事件，宠物的行为一般都跟UI相关，客户端是包裹ID跟place,服务器端
则是主人UID和宠物uid*/
/************************************************************************/
// 宠物出征
#define EVENT_PET_CONJURE										39

// 宠物召回
#define EVENT_PET_CALLBACK										40

// 宠物合体														   
#define  EVENT_PET_COMBINE										41

// 宠物解体
#define  EVENT_PET_DISBAND										42

// 宠物骑乘
#define  EVENT_PET_RIDE											43

// 宠物下马
#define  EVENT_PET_DISMOUNT										44

// 宠物改名
#define  EVENT_PET_CHANGENAME									45

// 改变主人事件
#define EVENT_CREATURE_CHANGELEADER								46

// 服务器端事件体，请从此类派生
struct SEventPetAction_S
{
	LONGLONG uidPet;		
	LONGLONG uidMaster;

	SEventPetAction_S():uidPet(INVALID_UID),
						uidMaster(INVALID_UID)
	{}
};

// 客户端事件体，请从此类派生
struct SEventPetAction_C : SEventPetAction_S
{
	ulong	 ulSkepID;
	int		 iSrcPlace;
	bool	 bIsPlaceChanged;

	SEventPetAction_C():ulSkepID(INVALID_SKEP_ID),
						iSrcPlace(-1),
						bIsPlaceChanged(false)
	{}
};

// 添加宠物自动释放技能事件
#define  EVENT_PET_ADDAUTOSKILL									47
struct  SEventPetActionAddAutoSkill_C
{
	LONGLONG uidPet;
	int      nSkillId[4];
};

//人物换装事件
#define	EVENT_PERSON_PERFORM									48
struct	SEventPersonPerform_C
{
	bool	perform;//true：穿上装备；false：卸下装备
	uchar	part;//换装部件
	ulong	resId[2];//资源id
	char	bindPoint[2][32];//绑定点
	char	animation[32];//当前动画
	uchar	weaponsubclass;//武器子类，对武器有效
};

//装备显示事件
#define	EVENT_PERSON_PERFORM_SHOW								49
struct SEventPersonPerformShow_C
{
	uchar	part;//部件
	bool	show;//是否显示
};

// TileChange
#define EVENT_CREATURE_TILECHANGE								50
#define EVENT_ADDGOODS_SUCCESS                                  51
struct SEventAddGoodsSuccess_C
{
	LONGLONG uidItem;
	bool     bSystemAdd;    //是否为系统添加
};

#define EVENT_PET_RIDE_DROPOUT									52
struct SEventRidePetDropOut_C
{
	int nPlace;
};

// 通知客户端九宫格创建实体事件
#define EVENT_ENTITY_ENTER_MAPGRID                              53
struct SEventEntityEnterMapGrid_S
{
	LONGLONG   uidIncomer;
	LONGLONG   uidActor;
};

//人物换装预览事件
#define	EVENT_PERSON_PERFORM_PREVIEW							 54
struct	SEventPersonPerformPreview_C
{
	bool	perform;//true：穿上装备；false：卸下装备
	uchar	part;//换装部件
	ulong	resId[2];//资源id
	char	bindPoint[2][32];//绑定点
	char	animation[32];//当前动画
	uchar	weaponsubclass;//武器子类，对武器有效
};

//装备预览显示事件
#define	EVENT_PERSON_PERFORM_PREVIEW_SHOW						55
struct SEventPersonPerformPreviewShow_C
{
	uchar	part;//部件
	bool	show;//是否显示
};

//装备预览重置事件
#define EVENT_PERSON_PERFORM_PREVIEW_RESET						 56

//by cjl
//通知客户端更新了新手礼包属性
#define EVENT_PERSON_PRESENT_UPDATE                             57
//收到了获取礼包的结果
#define EVENT_PERSON_PRESENT_RESULT                             58
struct SEventPersonPresent_C 
{
	//当事件为EVENT_PERSON_PRESENT_UPDATE时，result没意义
	//当事件为EVENT_PERSON_PRESENT_RESULT时
	//result == 1  升级成功，
	//result == 2  礼包发送完毕，
	//result == 3  背包已满，
	//result == 4  其他原因而失败，
	int result;
};

//最终战斗力改变，受到装备、等级、宠物变化的影响
#define EVENT_PERSON_FIGHT_CHANG								59

//客户端的鉴定结果事件
#define EVENT_PERSON_IDENTIFY_EQUIP					            60

//装备的炼魂等级发生变化
#define EVENT_EQUIP_SOUL_LEVEL_UPDATE							61
struct SEventEquipSoulLevelUpdate_C
{
	bool bOK;
};
//装备重铸成功
#define EVENT_EQUIP_REBUILD_SUCCEED								62
//end cjl

// 永久删除魂将
#define EVETN_PET_PERMANENT_DELETE								63
struct SEventPetPermanentDelete_S
{
	LONGLONG uidPet;
};

#define EVENT_PET_BORN											64
struct SEventPetBorn_S
{
	LONGLONG uidPet;
};

//by cjl
//变身事件
#define EVENT_CREATURE_CHANGEBODY								65
struct SEventChangeBody_C
{
	//目标面板
	//destForm = 0  表示通知当前变身的modeId，
	//destForm = 1	技能面板
	//destForm = 2	XP
	//destForm = 3	buff
	//destForm = 4	宠物
	//destForm = 5	新技能
	int destForm;

	int	oper;//操作，一般来说，0为冻结，1为激活，对于buff来说，是time的意思，对应destForm = 0表示modeId
};
//end cjl

/***************************************************************/
///////////////////// 冷却系统事件码 ////////////////////////////
/***************************************************************/
// 开始冷却,客户端用来转圈的
#define EVENT_FREEZE_START										200
// 客户端
struct SEventFreezeStart_C
{
	DWORD			dwClassID;		// 冷却类
	DWORD			dwFreezeID;		// 冷却ID
	DWORD			dwFreezeTime;	// 冷却时长
};

/***************************************************************/
/////////////////// 通用物品篮事件码 ////////////////////////////
/***************************************************************/
// 向通用物品篮添加物品
#define EVENT_SKEP_ADDGOODS										210
// 场景服
struct SEventSkepAddGoods_S
{
	LONGLONG		uidMaster;		// 添加者
	LONGLONG		uidGoods;		// 物品UID
	DWORD			dwSkepID;		// 新添加的物品篮ID
	int				nPlace;			// 新添加的位置

	DWORD			dwSrcSkepID;	// 源物品篮ID
};

// 向通用物品篮删除物品
#define EVENT_SKEP_REMOVEGOODS									211
// 场景服
struct SEventSkepRemoveGoods_S
{
	LONGLONG		uidMaster;		// 添加者
	LONGLONG		uidGoods;		// 物品UID
	DWORD			dwSkepID;		// 源物品篮ID
	int				nPlace;			// 位置

	DWORD			dwTargetSkepID;	// 准备移往的物品篮ID
	int				nTargetPlace;	// 准备移往的位置
	bool			bDropOut;		// 是否是丢弃
	// 后两个变量，只有否决事件才有值,并且也只有link操作时才有，
	// 其他情况为无效值

	SEventSkepRemoveGoods_S():uidMaster(INVALID_UID),
							uidGoods(INVALID_UID),
							dwSkepID(INVALID_SKEP_ID),
							nPlace(-1),
							dwTargetSkepID(INVALID_SKEP_ID),
							nTargetPlace(-1),
							bDropOut(false)
	{

	}

};

// 通用物品篮拆分物品
#define EVENT_SKEP_SPLITGOODS									212
// 场景服
struct SEventSkepSplitGoods_S
{
	LONGLONG		uidOperator;	// 操作者
	LONGLONG		uidGoods;		// 物品UID

	DWORD			dwSkepID;		// 物品篮ID
	DWORD			dwPlace;		// 位置	

	DWORD			dwSplitNum;		// 拆分数量		
};

// 客户端
struct SEventSkepSplitGoods_C
{
	LONGLONG		uidGoods;		// 物品UID

	DWORD			dwSkepID;		// 物品篮ID
	DWORD			dwPlace;		// 位置	

	DWORD			dwSplitNum;		// 拆分数量		
};


// 通用物品篮内某位置发生了变化
#define EVENT_SKEP_PLACECHANGE									213
// 客户端
struct SEventSkepPlaceChange_C
{
	DWORD			dwSkepID;		// 物品篮ID
	DWORD			dwPlace;		// 位置
};

// 通用物品篮内修改了容量
#define EVENT_SKEP_CHANGESIZE									214
// 客户端
struct SEventSkepChangeSize_C
{
	DWORD			dwSkepID;		// 物品篮ID
	DWORD			dwMaxSize;		// 大小
};

// 通用物品篮内交换物品
#define EVENT_SKEP_EXCHANGE										215
// 服务器，只有投票事件
struct SEventSkepExchange_S
{
	DWORD			dwTargetSkepID;	// 目标物品篮ID
};

// 设置物品篮的当前子篮
#define EVENT_SKEP_SETCURSUBSKEP								216
//客户端,主要针对装备升级
struct SEventSkepSetCurSubskep_C								
{
	DWORD			dwSkepID;		//物品篮ID
	DWORD			dwCurSubskepName;//被设置的当前子篮的名字
};

// 开启行囊
#define EVENT_SKEP_OPENBAG										217
//客户端
struct SEventSkepOpenBag_C								
{
	DWORD			dwSkepID;	// 物品篮ID
	DWORD			dwPlace;	// 位置
};

// 通用物品篮内某位置发生了变化前
#define EVENT_SKEP_PRE_PLACECHANGE								218

// 通用物品蓝添加了添加/删除了一个观察者
#define EVENT_SKEP_DONEOBSERVER									219
struct SEventSkepUpdateObserver_S
{
	DWORD			dwSkepID;	// 物品栏ID
	LONGLONG		uidObserver;	
	bool			bIsAdd;
};
struct SEventSkepObserver_C
{
	DWORD			dwSkepID;
	DWORD			dwSkepName;
};

// 通用物品栏创建
#define  EVENT_SKEP_CREATE										220
struct SEventSkepCreate_C
{
	DWORD			dwSkepName;
};

//by cjl
//销毁物品，注意和EVENT_SKEP_REMOVEGOODS不同
#define EVENT_SKEP_DESTROYGOODS									221
struct SEventSkepDestroyGoods_C
{
	DWORD		dwSkepID;			// 栏ID
	DWORD		dwPlace;			// 位置
	LONGLONG	uidGoods;			// 物品UID
	bool        bDestory;           // 丢弃销毁；
	bool        bPet;               // 是否是魂将；
};
//end cjl

/***************************************************************/
///////////////////////// 坐事件码 ////////////////////////////
/***************************************************************/
// 打断坐下
#define EVENT_CREATURE_SIT_BREAK								280

/***************************************************************/
///////////////////////// 系统事件码 ////////////////////////////
/***************************************************************/
// 构建了场景
#define EVENT_SYSTEM_BUILDZONE									300
// 场景服
struct SEventSystemBuildZone_S
{
	DWORD			dwMapID;		// 地图ID
	DWORD			dwZoneID;		// 场景ID
};

// 客户端
struct SEventSystemBuildZone_C
{
	DWORD			dwMapID;		// 地图ID
	DWORD			dwZoneID;		// 场景ID
};

// 销毁了场景
#define EVENT_SYSTEM_DESTORYZONE								301
// 场景服
struct SEventSystemDestoryZone_S
{
	DWORD			dwMapID;		// 地图ID
	DWORD			dwZoneID;		// 场景ID
};

// 客户端
struct SEventSystemDestoryZone_C
{

};



// 角色离开场景
#define EVENT_SYSTEM_LEAVEZONE								   302
// 场景服
struct SEventSystemLeaveZone_S
{
	LONGLONG        uID;            // 离开者
	DWORD			dwMapID;		// 地图ID
	DWORD			dwZoneID;		// 场景ID
};

// 客户端
struct SEventSystemLeaveZone_C
{

};

// 场景服务器
#define EVENT_SYSTEM_CLOSEZONESERVER					        304
// 场景服
struct SEventSystemCloseZoneServer_S
{
	
};

// 客户端
struct SEventSystemCloseZoneServer_C
{

};

// 关闭场景服务器命令 (还没有真正的关闭)
#define EVENT_SYSTEM_CLOSEZONESERVERORDER					    305

//保存玩家数据
#define EVENT_SYSTEM_SAVEPERSONTODB                             306
struct SEventSystemSavePersonToDB_S
{
	ulong ulUserPID;
};

//change zone
#define EVENT_SYSTEM_CHANGEZONE		                            307
struct SEventSystemChangeZone_S
{
	DWORD dwPDBID;
	DWORD dwDstZoneID;
	xs::Point ptDstTile;
};

#define EVENT_SYSTEM_LOADINGFINISH		                         308
struct SEventSystemLoadingFinish_C
{
	int nReason;
};

// 开启场景服务器
#define EVENT_SYSTEM_STARTZONESERVER							  309

// 系统设置改变
#define EVENT_SYSTEM_SYSSETCHANGE    							  310
/***************************************************************/
///////////////////// 聊天系统事件码 ////////////////////////////
/***************************************************************/
// 聊天消息到达(仅客户端)	上下文：格式化的html字符串
#define EVENT_CHAT_MSG_ARRIVED									350

struct SEventChat_Msg_Arrived_S
{	
	ulong sender;
	ulong target;
	ulong channelType;
};

#define EVENT_CHAT_SEND_MSG                                     351
struct SEventChat_Send_Msg_C
{	
	ulong channelType;
	ulong target;
};

#define EVENT_CHAT_SHIELD                                       352
struct SEventChat_Shield_C
{	
	ulong target;
	ulong ulInsertPos;
	short bShield;
};

#define EVENT_CHAT_SHIELDSTATECHANGE                             353
struct SEventChat_ShieldChange_C
{
	ulong target;
	ulong ulNewInsertPos;
};
/***************************************************************/
///////////////////// 主角引导条事件码 //////////////////////////
/***************************************************************/
/// 主角的技能引导开始(上下文:时间间隔（毫秒）)
#define EVENT_SKILL_CHANNEL_START								400

/// 主角的技能引导结束(无上下文)
#define EVENT_SKILL_CHANNEL_END									401

/// 主角技能升级(上下文:ushort 技能大类ID)
#define EVENT_SKILL_UPGRADE										402

// 用于技能数据的更新(目前用于宠物技能删除后 同步修改UI)
#define EVENT_SKILL_UPDATE                                      403
struct SEventSkillAction_C
{	
	LONGLONG	uid;		
};

/// 可以使用XP技能了(怒气满了)
#define EVENT_XPSKILL_CANUSE								    404
struct SEventXPSkillCanUse_C
{	
	LONGLONG	uidMaster;		
};

/// XP技能超时不能再使用了(怒气减为0)
#define EVENT_XPSKILL_TIMEOUT								   405
struct SEventXPSkillTimeout_C
{	
	LONGLONG	uidMaster;
	char        cXPStateFlag;
};

/// 客户端通知服务器XP在开始使用了，以便服务器开始计时
#define EVENT_XPSKILL_BEGIN								       406
struct SEventXPSkillBegin_C
{	
	LONGLONG	uidMaster;
	short       sSkillId;
};

///服务器通知客户端XP可以开始蓄气了
#define EVENT_XPSKILL_CANBOOT							   407
struct SEventXPSkillCanBoot_C
{	
	LONGLONG	uidMaster;
};

// 用于技能经验的更新(并同步等级)                                          
#define EVENT_SKILL_EXP_UPDATE                                 408

/***************************************************************/
////////////////////// 采集系统事件码 /////////////////////////
/***************************************************************/
// 准备采集读条
#define EVENT_COLLECT_PREPARE                                  450

// 开始采集读条
#define EVENT_COLLECT_WAIT                                     451

// 开始采集
#define EVENT_COLLECT_START                                    452
struct SEventCollect_S
{
	LONGLONG		uidMaster;               // 谁点的采集对象
	LONGLONG		uidMonster;              // 哪个采集对象
};

// 泡泡爆炸 add by zjp
#define EVENT_BUBBLE_BOMB									   499
struct SEventBubbleBomb
{
	xs::Point		BombCenterTile;
};

/***************************************************************/
////////////////////// 主角交易事件码 ///////////////////////////
/***************************************************************/
// 请求交易
#define EVENT_TRADE_REQUEST										500
struct SEventTradeRequest_C
{	
	LONGLONG	uidDemander;		// 谁请求交易
};

// 交易开始
#define EVENT_TRADE_GO											501
struct SEventTradeGo_C
{	
	LONGLONG	uidTarget;			// 和谁交易	
	ulong		skepId;				// 自己的交易物品栏ID	
	ulong		skepIdTarget;		// 对方的交易物品栏ID
};

// 取消交易
#define EVENT_TRADE_CANCEL										502
struct SEventTradeCancel_C
{	
	LONGLONG	uid;				// 谁取消了交易,可能是自己
};

// 加锁或者解锁
#define EVENT_TRADE_LOCK										503
struct SEventTradeLock_C
{	
	bool		myself;				// 自己的状态还是对方的状态	
	bool		lock;				// 锁定还是解锁
};

// 点了确定
#define EVENT_TRADE_CONFIRM										504
struct SEventTradeConfirm_C
{	
	bool		myself;				// 自己确定还是对方确定
};

// 交易完成
#define EVENT_TRADE_FINISH										505

// 前面一个定义是发送无源指针的，
// 这个是根据交易双方单独发送的，这样可以节省不必要的调用
#define EVENT_TRADE_FINISH_SRC									506
struct SEventTradeFinish_C
{
};

struct SEventTradeFinish_S
{
	ulong ulPersonPID;
	ulong ulNegotiantPID;	
};

// 对方改变金钱数目
#define EVENT_TRADE_UPDATE_MONEY								507
struct SEventTradeUpdateMoney_C
{
	ulong		money;
};

// 对方改变元宝数目
#define EVENT_TRADE_UPDATE_GOLD									508
struct SEventTradeUpdateGold_C
{
	ulong		gold;
};

/***************************************************************/
////////////////////// 主角组队事件码 ///////////////////////////
/***************************************************************/
// 销毁队伍事件
#define  EVENT_TEAM_DESTROY                                 538
struct SEventTeamDestroy_S
{
	DWORD	 dwTeamID;					// 队伍ID
	DWORD    dwCaptainDBID;             // 队长ID
};
// 创建队伍事件
#define  EVENT_TEAM_CREATE                                  539
struct SEventTeamCreate_S 
{
	char        szName[32];                 // 队伍名称
	DWORD		dwPDBID;					// PDBID
	DWORD		dwTeamID;					// 队伍ID
	DWORD		dwCaptain;					// 队长
	DWORD       dwTeamMode;                 // 队伍类型
	DWORD		dwGoodsAllotMode;			// 物品分配模式
	DWORD       dwPetAllotMode;             // 宠物分配模式
	bool        bPrivateOpen;               // 私人专属队伍
};

// 改变队长
#define  EVENT_TEAM_CHANGECAPTAIN                           540
struct SEventTeamChangeCaptain_S
{
	DWORD  dwCaptainPDBID;
	DWORD  dwTeamID;
	DWORD  dwOldCaptainPDBID;
};

// 加入了队伍
#define EVENT_TEAM_PERSONJOIN                               541

struct SEventTeamComm_S
{
	DWORD  dwPDBID;
	DWORD  dwCaptainDBID;
	DWORD  dwTeamID;
};
// 离开队伍
#define EVENT_TEAM_PERSONQUIT                               542
struct SEventTeamQuitTeam_S
{
	DWORD  dwPDBID;
	DWORD  dwTeamID;
	bool   blogOut;
};

// 绘制random事件
#define  EVENT_TEAM_RANDOMDIDA                               543
struct SEventTeamRandomDida_C
{
	DWORD		dwPDBID;  // 发给谁
	DWORD    dwTeamID;    // 队伍ID；
	DWORD    dwGoodsID;   // ID   
	int      nStayTime;   // 事件长度 
	int      nNumber;     // 产生的随机数
	LONGLONG UidGoods;    // 物品UID 
	int      nReandomGoodType; // 物品类型
	long     lImageID;    //　对应物品的图片ID
};	
// 跟随队长事件
#define  EVENT_TEAM_FOLLOWCAPTAIN                            544
struct SEventTeamFollowCaptain_C
{
      LONGLONG uid;
};
// 本队信息查询事件
#define  EVENT_TEAM_SELFMESSAGEVIEW                           545
struct SEventTeamMessageView_C	// 队伍信息
{
	char  szName[32];     // 队伍名字
	DWORD dwTeamMode;               // 队伍模式
	DWORD dwGoodsAllotMode;         // 队伍分配模式
	DWORD dwPetAllotMode;           // 魂石分配模式
	DWORD dwCaptainID;              // 队长ID
	DWORD dwMemberCount;            // 成员数目
	DWORD dwCaptainArmyID;          // 所属军团
	DWORD dwCaptainHomage ;         //　所属爵位
	bool  bPrivate;                // 是否私有
	bool  bClick;
	SEventTeamMessageView_C(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
// 申请加入队伍事件
#define EVENT_TEAM_APPLY                                       546
struct SEventTeamApply_C
{
   char szTeamApply[512];         // 内容
   int  nDialoc;                  // 滴答内容添加的位置
   SEventTeamApply_C(void)
   {
	   memset(this, 0, sizeof(*this));
   };
};
// 搜索队伍List 事件
#define EVENT_TEAM_SEARCH                                       547

struct SEventTeamSearch_C
{
	DWORD dwPDBID; 
	int nCount; 
	// 队伍个数
};

// 界面关闭组队
#define EVENT_TEAM_CLOSEUI										548
struct SEventTeamCloseUI_C
{	
};

// 增加队友
#define EVENT_TEAM_ADDMEMBER									549
struct SEventTeamAddMember_C
{
	DWORD		dwPDBID;
	DWORD       dwLevel;                    // 等级
	char		szName[MAX_PERSONNAME_LEN];
	long		lFaceID;
	bool		bOnline;
	bool		bCaptain;
};

// 移除队友
#define EVENT_TEAM_REMOVEMEMBER									550
struct SEventTeamRemoveMember_C
{
	DWORD		dwPDBID;		// pdbid
};

// 更改队友数据
#define EVENT_TEAM_CHANGEMEMBER									551
struct SEventTeamChangeMember_C
{
	DWORD		dwPDBID;
	DWORD       dwLevel;
	char		szName[MAX_PERSONNAME_LEN];
	long		lFaceID;
	bool		bOnline;
	bool		bCaptain;
};

//添加虚拟拷贝物品
#define  EVENT_ADD_DUMMYCOPY                                    554
struct SEventSkepAddDummyCopy_S
{
	LONGLONG		uidMaster;		// 添加者
	LONGLONG		uidGoods;		// 物品UID
	DWORD			dwDCSkepID;		// 虚拟添加的物品篮ID
	int				nDCPlace;		// 虚拟添加的位置

	DWORD			dwSrcSkepID;	// 源物品篮ID
};

//移除虚拟拷贝物品
#define  EVENT_REMOVE_DUMMYCOPY                                 555
struct SEventSkepRemoveDummyCopy_S
{
	LONGLONG		uidMaster;		// 添加者
	LONGLONG		uidGoods;		// 物品UID
	DWORD			dwSkepID;		// 源物品篮ID
	int				nPlace;			// 位置

	DWORD			dwDCSkepID;	// 虚拟拷贝的物品篮ID
	int				nDCPlace;	// 虚拟拷贝的显示位置
};
//宠物篮的位置更新
#define  EVENT_SKPE_PET_PLACECHANGE                                 556
struct SEventSkepPetPlaceChange_C
{
	DWORD			dwSkepID;		 // 宠物篮ID
	DWORD			dwPlace;		 // 源位置
	int             nExProp;		 // 附加属性，目前专用于阵法和封号
	int				nExPropValue;	 // 附加属性值，目前专用于阵法和封号
	LONGLONG        uidGoods;        // 宠物UID 
	bool	        bLink;	         // 是否链接
};
// 宠物升级事件 
#define  EVENT_PET_UPGRADE                                          557
struct SEventPetUpgrade_S
{
	// ...............
};

// 宠物阵法状态更新
#define  EVENT_PET_UPDATA_FORMATION                                 558
struct SEventPetFormation_C
{
	int formation;
};

/***************************************************************/
////////////////////// 任务系统事件码 ///////////////////////////
/***************************************************************/
//  玩家可以接受的任务的最大数目；
#define PERSON_HOLD_TASK_MAX_COUNT 25

//  每个任务的最大需求数；
#define TASK_REQUIREMENT_MAX_COUNT 15

//  已接任务的数据读取完成；
#define EVENT_READ_HOLDTASK_DATA 601
//  已完成任务的数据读取完成；
#define EVENT_READ_ACCOMPLISHEDTASK_DATA 602
struct SEventSystemReadTaskData_S
{
    DWORD dwHolder;
};

//  添加一条任务记录；
#define EVENT_TASK_ADDRECORD 603
struct CEventTaskAddRecord_C
{
    WORD m_wTaskID;
	WORD m_wTraceIndex;
};

//  更新一条任务记录；
#define EVENT_TASK_UPDATERECORD 604
struct CEventTaskUpdateRecord_C
{
    WORD m_wTaskID;
	uchar ucIndex;
    ushort usValue;
};

//  完成一个任务；
#define EVENT_TASK_ACCOMPLISH 605
struct CEventTaskAccomplish_C
{
    WORD m_wTaskID;
};

//  初始化已接任务信息；
#define EVENT_TASK_INITIAL_HOLD_TASK 606

//	初始化可接任务信息；
#define EVENT_TASK_INITIAL_CANACCEPT_TASK 607

// 移除一个任务 
#define EVENT_TASK_REMOVE_TASK       608
struct CEventTaskRemove_C
{
	WORD m_wTaskID;
};

// 任务飞行
#define EVENT_TASK_FLYRIDE_TASK       609
struct CEventTaskFlyRide_C
{
	WORD m_wTaskID;
	WORD m_wNPCID;
	WORD m_wModelID;
	WORD m_wCurSpeed;
};

// 任务提示图片
#define  EVENT_TASK_SEND_IMAGEINFO    610
struct CEventTaskImageInfo_C
{
	char  szImageName[64];       // 图片名字；
};

/////////////////活动中，野外魂将刷新/////////
// Boss刷新提示事件
#define  EVENT_TASK_NOTICE_BOSSREFRESH    611
struct CEventTaskNoticeBossRefresh_C
{
	char  szBossName[64];       // Boss名字；
};

/////////////////活动中，玩家绑定数读取完成/////////
// 玩家绑定数读取完成
#define  EVENT_TASK_NOTICE_BINDDATA_COMPLETE    612
struct CEventTaskNoticeBindDataComplete_S
{

};

/////////////////活动中，野外魂将状态/////////
// Boss刷新提示事件
#define  EVENT_TASK_UPDATE_AMUSEMENTBOSS    613
struct CEventTaskUpdateAmusementBoss_C
{
	char  szBossName[64];       // Boss名字；
	bool  bDeath;
};
/***************************************************************/
////////////////////// 副本事件码 ///////////////////////////
/***************************************************************/
//玩家进入副本次数同步
//当玩家进入副本之后，副本场景服务器LUA会调用设置玩家进入某副本地图的次数
//而进入副本之前需要在普通场景服读取已经进入该副本的次数，作为显示和逻辑判断
//故需要在两边服务器做数据同步

#define	EVENT_SYNC_RESET_INSTANCE_MAP_ENTER_TIMES_ALL		699
struct SEventSyncResetInstanceMapEnterTimesAll
{
	int nMapID;
};


#define	EVENT_SYNCHRONIZE_INSTANCE_MAP_ENTER_TIMES		700
struct SEventSynchronizeInstanceMapEnterTimes
{
	int nActorDBID;
	int nMapID;
	int nTimes;
};


//更改当前副本场景的关键人物ID//团队副本中，队长改变了需要通知到副本场景
#define	EVENT_SET_INSTANCE_ZONE_KEY_ACTOR				701
struct SEventSetInstanceZoneKeyActor
{
	int nOldKeyActorDBID;
	int nNewKeyActorDBID;
	int nMapID;
};


//重置副本,强迫副本内全部玩家离开某当前副本场景，回到进入副本之前的地图及位置
#define EVENT_RESET_INSTANCE_ZONE						702
struct SEventResetInstanceZone
{
	int nKeyActorDBID;
	int nMapID;
};

//重置以自己为关键人物的所有副本
#define EVENT_RESET_ALL_INSTANCE_ZONE					703
struct SEventResetAllInstanceZone
{
	int nActorDBID;
};


enum EInstanceKickReason//被踢出副本的原因
{
	E_INSTANCE_ZONE_BEING_RESET,					//副本即将被重置，如例行的整点重置
	E_INSTANCE_ZONE_HAS_DESTROYED,					//副本内无人太久，副本已经被销毁，服务器挂了重启后，需要把人踢出去
	E_INSTANCE_ZONE_NOT_MEMBER_OF_KEY_ACTOR,        //并不是副本关键人物的集体里的成员，比如团队副本离开队伍
	E_INSTANCE_ZONE_INVALID_REASON,					//无效理由，只是用作判断是否真的要踢人的开关
};

#define BACKMAP_WAIT_SECONDS 30
//计时开始，30秒钟后，强迫某玩家离开某当前副本场景，回到进入副本之前的地图及位置，(比如玩家离开队伍之类时调用)

#define EVENT_START_TIMER_KICK_PERSON_TO_BACKMAP		704
struct SEventStartTimerKickPersonToBackMap
{
	int nActorDBID;
	enum EInstanceKickReason eResult;
	int nWait;     //等待时间，30s
};

//强迫某玩家离开某当前副本场景，回到进入副本之前的地图及位置，(比如玩家离开队伍之类时调用)
#define EVENT_KICK_PERSON_TO_BACKMAP_NOTIFY				705 //踢人之前发送通知 "您不属于当前副本，10后将自动离开！";//副本整点重置通知 XXX副本将在XXX秒后被重置......
#define EVENT_KICK_PERSON_TO_BACKMAP					706
struct SEventKickPersonToBackMap
{
	int nActorDBID;
};
#define EVENT_STOP_TIMER_KICK_PERSON_TO_BACKMAP          707 //停止离开副本的定时提醒
struct SEventStopTimerKickPersonToBackMap
{
	int nActorDBID;
};

#define  EVENT_ADD_NORMAL_ATTACK						708
struct SEventAddNormalAttack
{
	UID	 uidTarget;
};

// 计算暴击之前发送事件，主要是确定是否免疫暴击
#define  EVENT_CREATURE_BEFORECRAZYDAMAGE               709

// 自动回复
#define  EVENT_CREATURE_NEED_RECOVER                    710

// 采集跳跃事件
#define  EVENT_CREATURE_COLLECT_JUMP                    711
struct SEventActionCollectJump_C
{
	int nEndPtX;
	int nEndPtY;
	int nTaskID;
};

// NPC头顶绘制特效提示
#define  EVENT_PLAY_EFFECT_TOPMOST                    712
struct SEventActionPlayEffectTopMost_C
{
	char      szTipText[64];             // 提示文字
	LONGLONG  uidMonster;
	ulong     ulEffectID;
	bool      bNeedClick;
};

// NPC头顶绘制特效提示
#define  EVENT_STOP_EFFECT_TOPMOST                    713
struct SEventActionStopEffectTopMost_C
{
	LONGLONG  uidMonster;
	ulong     ulEffectID;
};
/***************************************************************/
//////////////////////好友系统事件码 ///////////////////////////
/***************************************************************/
#define EVENT_FRIEND_INVITE                              801
struct  SEventFriend_AppAddFriend
{
	ulong   appUserID;                   /// 邀请者PID
	char   name[MAX_PERSONNAME_LEN];	/// 邀请者名字
	ulong  level;						/// 邀请者等级
	int    nCreatureVocation;           /// 邀请者职业
	SEventFriend_AppAddFriend(void)
	{
		memset(this, 0, sizeof(*this));
	};
};    

#define EVENT_FRIEND_REMOVEFRIEND									802
struct  SEventFriend_RemoveFriend
{
	ulong friendPID;
	int  nGroupID;
	char   name[MAX_PERSONNAME_LEN];	/// 名字
	SEventFriend_RemoveFriend(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define EVENT_FRIEND_ADDGROUP										803
struct  SEventFriend_AddGroup
{
	int  nGroupID;
	SEventFriend_AddGroup(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define  EVENT_FRIEND_CHANGEGROUPNAME                           804

#define  EVENT_FRIEND_MOVEFRIEND								805
struct  SEventFriend_MoveFriend
{	
	ulong friendPID;
	int  nOldGroupID;
	int  nNewGroupID;
	char   name[MAX_PERSONNAME_LEN];	/// 名字
	SEventFriend_MoveFriend(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define  EVENT_FRIEND_UPDATEALLFRIEND							806
struct  SEventFriend_UpdateAll
{
	ushort usRelation;
	SEventFriend_UpdateAll(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define  EVENT_FRIEND_FRIENDLOGOFF								807
struct  SEventFriend_FriendLOGOFF
{	
	ulong userPID;
	ulong friendPID;
	int   nGroupID;
	ushort usRelation;
	SEventFriend_FriendLOGOFF(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define  EVENT_FRIEND_UPDATE									808
struct  SEventFriend_FriendUpdate
{	
	ulong userPID;
	ulong friendPID;
	SEventFriend_FriendUpdate(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define  EVENT_FRIEND_ASKADDENEMY							     809

#define  EVENT_FRIEND_REMOVEENEMY							     810

#define EVENT_FRIEND_AddRecentComm                               811
struct  SEventFriend_AddRecentComm
{	
	ulong oldFirstPID;
	ulong curFirstPID;
	SEventFriend_AddRecentComm(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define EVENT_FRIEND_UPDATEPOINT								812

#define EVENT_FRIEND_LEVELCHANGE								813
struct  SEventFriend_LevelChange
{	
	ulong ulUserPID;
	ulong ulFriendPID;
	int   nOldLevel;
	int   nCurLevel;
	SEventFriend_LevelChange(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define  EVENT_FRIEND_FRIENDLogin								814
struct  SEventFriend_FriendLogin
{	
	ulong userPID;
	ulong friendPID;
	int   nGroupID;
	ushort usRelation;
	SEventFriend_FriendLogin(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define  EVENT_FRIEND_UPDATEGROUP								815
struct  SEventFriend_UpdateGroup
{	
	ulong friendPID;
	int   nGroupID;
	ushort usRelation;
	SEventFriend_UpdateGroup(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define EVENT_FRIEND_SHIELD										 816
struct  SEventFriend_Shield
{
	ulong friendPID;
	ushort usShield; //0 屏蔽 1 解除屏蔽
	SEventFriend_Shield(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

#define  EVENT_FRIEND_ADDFRIEND								  817
struct  SEventFriend_AddFriend
{
	ulong personID;
	ulong friendPID;
	int   nGroupID;
	ushort usRelation;
	SEventFriend_AddFriend(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define  EVENT_FRIEND_UpdateEnemyMapPos						    818

#define  EVENT_STALL_OPENSTALLFLAG								 850
struct  SEventStall_OpenStallFlag
{	
	int   nStallID;
	SEventStall_OpenStallFlag(void)
	{
		memset(this, 0, sizeof(SEventStall_OpenStallFlag));
	};
};

#define  EVENT_STALL_UPDATESTALLINFO						     851
struct  SEventStall_UpdateStallInfo
{	
	int   nStallID;
	int	  nPropType;
	int   nValue;
	DWORD dwValue;
	uchar uEndStall;
	SEventStall_UpdateStallInfo(void)
	{
		memset(this, 0, sizeof(SEventStall_UpdateStallInfo));
	};
};

#define  EVENT_STALL_STARTSTALL									852
struct  SEventStall_StartStart
{	
	int   nStallID;
	ushort usRemote;
	SEventStall_StartStart(void)
	{
		memset(this, 0, sizeof(SEventStall_StartStart));
	};
};

#define EVENT_STALL_DRAGOUT										853
struct  SEventStall_DragOut
{	
	DWORD   dwSkepID;
	int     nPlace;
	int     nTargetPlace;
	SEventStall_DragOut(void)
	{
		dwSkepID = 0;
		nPlace = -1;
		nTargetPlace = -1;
	};
};

#define  EVENT_STALL_AddSellGoods					     854
struct  SEventStall_UpdatePrice
{
	int nStallID;
	LONGLONG uGOODSID;
	int nPlace;
	DWORD dwPrice;
	ushort usPriceType;
	ulong ulSrcPackID;
	int nSrcPlace;
	int nNum;
	SEventStall_UpdatePrice()
	{
		memset(this, 0, sizeof(SEventStall_UpdatePrice));
	}
};

#define  EVENT_STALL_AddBuyGoods	    					     855
struct  SEventStall_AddBuyGoods
{
	int nGoodsID;
	int nNum;
	int nTargetPlace;
	DWORD dwPrice;
	ushort usPriceType;
	SEventStall_AddBuyGoods()
	{
		memset(this, 0, sizeof(SEventStall_AddBuyGoods));
	}
};

#define  EVENT_STALL_RemoveBuyGoods	    					     856
struct  SEventStall_RemoveBuyGoods
{
	int nGoodsID;
	SEventStall_RemoveBuyGoods()
	{
		memset(this, 0, sizeof(SEventStall_RemoveBuyGoods));
	}
};

#define  EVENT_STALL_UndoBuyGoods	    					     857
struct  SEventStall_UndoBuyGoods
{
	ulong ulSrcSkepID;
	SEventStall_UndoBuyGoods()
	{
		memset(this, 0, sizeof(SEventStall_UndoBuyGoods));
	}
};

#define  EVENT_STALL_UpdateTotalPrice							 858
struct  SEventStall_UpdateTotalPrice				
{
	uint64 u64TotalMoney;
	uint64 u64TotalGoldIngot;
	SEventStall_UpdateTotalPrice()
	{
		memset(this, 0, sizeof(SEventStall_UpdateTotalPrice));
	}
};

#define  EVENT_STALL_ENDSTALL									  859
struct  SEventStall_EndStall
{	
	int   nStallID;
	SEventStall_EndStall(void)
	{
		memset(this, 0, sizeof(SEventStall_EndStall));
	};
};

#define  EVENT_STALL_RemoveSellGoods								860
struct  SEventStall_RemoveSellGoods
{	
	ulong ulSrcSkepID;
	int nSrcPlace;
	SEventStall_RemoveSellGoods(void)
	{
		memset(this, 0, sizeof(SEventStall_RemoveSellGoods));
	};
};

#define  EVENT_STALL_STATUSCHANGE									 861
struct  SEventStall_StatusChange
{	
	int nStallID;
	ushort usLastStatus;
	SEventStall_StatusChange(void)
	{
		memset(this, 0, sizeof(SEventStall_StatusChange));
	};
};

#define  EVENT_STALL_UPDATERECYCLEMONEY									862
struct  SEventStall_UpdateRecycleMoney
{	
	int nStallID;
	ulong dbidPerson;
	uint64 ulMoney;
	SEventStall_UpdateRecycleMoney(void)
	{
		memset(this, 0, sizeof(SEventStall_UpdateRecycleMoney));
	};
};

#define  EVENT_STALL_ADDLEAVEWORD							            863
struct  SEventStall_AddLeaveWord	
{
	int nStallID;
	ulong dbidPerson;
	char  szName[MAX_PERSONNAME_LEN];
	char  szLeaveWord[STALL_LEAVEWORD_MSG_LEN];
	SEventStall_AddLeaveWord(void)
	{
		memset(this, 0, sizeof(SEventStall_AddLeaveWord));
	};
};

#define  EVENT_STALL_CLEARLEAVEWORD							            864
struct  SEventStall_ClearLeaveWord	
{
	int nStallID;
};

#define  EVENT_STALL_LASTSTALLCHANGE							        865
struct SEventStall_LastStallChange
{
	ulong dbidPerson;
	int nLastStallID;
	int nCurStallID;
};

#define  EVENT_STALL_UPDATEINCOMEINFO							        866
struct SEventStall_UpdateIncomeInfo
{
	int nStallID;
	ulong ulRent;	
};

#define  EVENT_STALL_OPERATESTALL							           867
struct SEventStall_OperateStall
{
	int nStallID;
	int nMapID;
	int nTileX;
	int nTileY;
	ushort usType;	
};

#define  EVENT_STALL_UpdateSellGoodsNum									868
struct  SEventStall_UpdateSellGoodsNum	
{	
	int nPlace;
	int nNum;
	DWORD dwPrice;
	ushort usPriceType;
	SEventStall_UpdateSellGoodsNum(void)
	{
		memset(this, 0, sizeof(SEventStall_UpdateSellGoodsNum));
	};
};

#define  EVENT_STALL_SyncBuyGoods									    869


#define EVENT_USE_SKILL												    870
struct SEventUseSkill
{
	UID		srcUID;					// 发起者
	UID		destUID;				// 承受者
	int		nSkillID;				// 技能ID
	int		nSubID;					// 技能等级
	SEventUseSkill()
	{
		memset(this, 0, sizeof(SEventUseSkill));
	}
};

#define  EVENT_SKILL_DISPLAYDAMAGEINFO							         871

#define  EVENT_SKILL_JUMPFINISH									         872
#define  EVENT_SKILL_COLLECTJUMPFINISH                                   873

#define  EVENT_SKILL_ATTACK												 874
struct SEventAttack
{
	UID		srcUID;			// 攻击者
	UID		destUID;		// 承受者
	int		nSkillID;		// 技能ID
	int		nSubID;			// 技能等级
};



#define  EVENT_ENTITY_BINDTOOLTP										 900

#define  EVENT_FAMILY_UPDATEFAMILY	                                     901
struct SEventUpdateFamily
{
	ulong dbidLeader;
};

#define  EVENT_FAMILY_ADDMEMBER	                                         902
struct SEventAddFamilyMember
{
	ulong dbidLeader;
	ulong dbidMember;
};

#define  EVENT_FAMILY_DISBANDFAMILY                                      903

#define  EVENT_FAMILY_JOINFAMILY                                         904
struct SEventJoinFamily
{
	ulong dbidPerson;
	ulong dbidLeader;	
};

#define  EVENT_FAMILY_QUITFAMILY                                         905
struct SEventQuitFamily
{
	ulong dbidPerson;
};

#define  EVENT_FAMILY_INVITEJOIN                                         906
struct SEventInviteJoin
{
	ulong dbidInviter;
	char szInviterName[MAX_PERSONNAME_LEN];  // 族长名称
	char szFamilyName[FAMILY_NAME_LEN + 1];  // 家族名称
};

#define  EVENT_FAMILY_UPDATEMEMBER	                                      907

#define  EVENT_FAMILY_DEMISE	                                          908
struct SEventFamilyDemise
{
	ulong dbidLeader;	
};


#define  EVENT_FAMILY_MEMBERONLINECHANGE	                              909
struct SEventMemberOnlineChange
{
	ulong dbidLeader;	
	ulong dbidPerson;
	uchar  bOnline;
};


// 坐下
#define  EVENT_ACTION_SIT											910

// 定身眩晕控制类事件
#define  EVENT_ACTION_STUN											911
struct SEventStun
{

};

#define EVENT_ACTION_ANIMATION										912
struct SEventAnimation
{
	int	 nActionID;
};

/***************************************************************/
////////////////////// 技能事件码 ///////////////////////////
/***************************************************************/
// new Skill
struct SEVENTSKILLHEAD
{
	int			nSkillID;
	LONGLONG	uidSrc;
	LONGLONG	uidDest;
};

#define  EVEN_SKILL_REQUESTSKILLCONTION							    950
struct SEventSkillConditionData : public SEVENTSKILLHEAD
{
};

// ===============攻击触发================
// 攻击落空
#define  EVENT_SKILL_MISS											951
struct SEventSkillMiss : public SEVENTSKILLHEAD
{
};

// 命中
#define  EVENT_SKILL_HIT											952
struct SEventSkillHit : public SEVENTSKILLHEAD
{
};

// 暴击
#define  EVENT_SKILL_DOUBLE											953
struct SEventSkillDouble : public SEVENTSKILLHEAD
{
};

// =============被击触发===================
// 闪避
#define  EVENT_SKILL_DUCK											954
struct SEventSkillDuck : public SEVENTSKILLHEAD
{
};

// 被命中
#define  EVENT_SKILL_BEHIT											955
struct SEventSkillBeHit : public SEVENTSKILLHEAD
{
};

// 被暴击
#define  EVENT_SKILL_BEDOUBLE										956
struct SEventSkillBeDouble : public SEVENTSKILLHEAD
{
};

// ===========死亡触发=================
// 自己死亡
#define  EVENT_SKILL_SELFDEAD										957
struct SEventSkillSelfDead : public SEVENTSKILLHEAD
{
};

// 目标死亡
#define  EVENT_SKILL_TARGETDEAD										958
struct SEventSkillTargetDead : public SEVENTSKILLHEAD
{
};

#define  EVENT_SKILL_FINISH										959
struct SEventSkillFinish : public SEVENTSKILLHEAD
{

};

// ==========生命值触发==============
#define EVENT_SKILL_SELFHP										960
struct SEventSkillSelfHP : public SEVENTSKILLHEAD
{

};

#define EVENT_SKILL_TARGETHP			961
struct SEventSkillTargetHP : public SEVENTSKILLHEAD
{

};

// =========当前内力值触发=============
#define  EVENT_SKILL_SELFMP				962
struct SEventSkillCurSelfMP : public SEVENTSKILLHEAD
{
	int nChangePropVal;
};

#define EVENT_SKILL_TARGETMP			963
struct SEventSkillTargetMP : public SEVENTSKILLHEAD
{

};

// =========距离触发==============
#define EVENT_SKILL_DISTANCE			964
struct SEventSkillDistance : public SEVENTSKILLHEAD
{

};

// =======添加控制Debuff======
#define EVENT_SKILL_ADDCONTROLBUFF		965
struct SEventSkillBuff : public SEVENTSKILLHEAD
{
	int nBuffTime;
};

// =======伤害事件===========
#define EVENT_SKILL_DAMAGE				966
struct SEventSkillDamage : public SEVENTSKILLHEAD
{
	int nDamage;
};

// ======激活技能=========
#define  EVENT_SKILL_ACTIVE			967
struct SEventSkillActive : public SEVENTSKILLHEAD
{

};

// ====生命上限改变触发=====
#define  EVENT_SKILL_MAX_HP_CHANGE		968
struct SEventSkillMaxHPChange : public SEVENTSKILLHEAD
{
	int nChangePropValue;
};

// ====技能消耗触发=======
#define  EVENT_SKILL_CONSUME			969
struct SEventSkillConsume : public SEVENTSKILLHEAD
{

};

// 离开队伍
#define  EVENT_SKILL_QUIT_TEAM			970		

// 加入队伍
#define  EVENT_SKILL_ENTER_TEAM			971

// 跳地图 
#define  EVENT_SKILL_CHANGE_MAP			972
struct SEventSkillChangeMap : public SEVENTSKILLHEAD
{
	DWORD dwTeamID;
	DWORD dwPDBID;
	DWORD dwOldMapID;
	DWORD dwNewMapID;
};

// 装备物品
#define  EVENT_EQUIP_EQUIPMENT			973

// 暴击后,后续攻击触发
#define  EVENT_SKILL_END_AROSE			974

#define  EVENT_SKILL_CONSUME_CHANGE     975
struct SEventChangeSkillConsume
{
	int		nMPConsumeChangePct;		// 技能消耗改变比
	SEventChangeSkillConsume()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 后续攻击触发
#define  EVETN_SKILL_HIT_END_AROSE		976

// 真元护盾吸收完伤害
#define  EVENT_SKILL_ABSORB_DAMAGE		977


// 军团创建
#define  EVENT_ARMY_CREATE			    978
struct SEventArmyCreate
{
	DWORD dwLeaderID;
	char szName[ARMY_NAME_LEN + 1];
	SEventArmyCreate()
	{
		memset(this, 0, sizeof(SEventArmyCreate));
	}
};

#define  EVENT_FAMILY_JOINARMY	         979
struct SEventFamilyJoinArmy
{
	ulong dbidArmyLeader;	
};

//打开加入阵营面板
#define Event_ACTION_JOIN_CAMP_WND       980

//打开改变阵营面板
#define Event_ACTION_CHANGE_CAMP_WND	 981

// 最大事件码
#define EVENT_ALL_MAXID					1000

#pragma pack()