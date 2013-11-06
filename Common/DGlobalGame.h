/*******************************************************************
** 文件名:	e:\ourgame\ZoneManager\include\DGlobalGame.h
** 版  权:	(C) 
** 
** 日  期:	2007/11/15  16:10
** 版  本:	1.0
** 描  述:	游戏的全局必定义
** 应  用:  服务器、客户端
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#pragma pack(1)

//////////////////////// 全局宏 //////////////////////////////////
// 无效uid
#define INVALID_UID							0

// 无效地图id
#define INVALID_MAPID						0

// 无效场景id
#define INVALID_ZONEID						0

// 无效的角色DBID
#define INVALID_PDBID						0

// 无效的用户DBID
#define INVALID_UDBID						0

// 无效的UID SerialNo
#define INVALID_UIDSERIALNO					0

// 无效的距离，肯定超过了测量范围
#define INVALID_DISTANCE					0x7FFFFFFF

// 无效的BUFF ID
#define INVALID_BUFF_ID						0

// 无效的效果ID
#define INVALID_EFFECT_ID					0

// 无效的虚拟物品篮ID
#define INVALID_SKEP_ID						0

// 无效阵营ID
#define INVALID_CAMP_ID						0

// 无效阵营的队ID
#define INVALID_CAMP_TEAMID					0

// 无效家族ID
#define INVALID_FAMILY_ID					0

// 无效活动ID
#define INVALID_AMUSEMENT_ID				0

// 最大地图数
#define MAP_MAXCOUNT						256

// 玩家名字长度
#define MAX_PERSONNAME_LEN					(16 + 1)

// 装备最大精炼等级（白装为0档，剩下还有九档可精炼）
#define EQUIPMENT_SMELT_MAXLEVEL			10

// 生物移动速度最多等级（0 ~ 9）（包括骑乘）
#define CREATURE_MOVESPEED_MAXLEVEL			10

//人物可以参与pk的最小等级
#define  PK_LEVEL_LIMIT						30

// 人物最大等级
#define PERSON_MAX_LEVEL					120

// 实体有效交互距离
#define ENTITYCOMMUNICATE_VALIDDISTANCE		7

// 装备的最多附加属性个数
#define EQUIPMENT_EFFECT_MAXCOUNT			16

// 人物包裹篮黙认大小
#define PERSON_PACKET_DEFAULTSIZE			99

//人物包裹篮黙认大小(不包括行囊位）
#define PERSON_PACKET_NONBAG_DEFAULTSIZE    96

//行囊的最大尺寸
#define  PERSON_PACKET_BAG_MAXCAPACITY		24

//人物包裹篮未加载行囊默认大小
#define PERSON_PACKET_NONBAGBLOCK_DEFAULTSIZE  32

//人物仓库篮未加载行囊默认大小
#define PERSON_LIB_NONBAGBLOCK_DEFAULTSIZE  24

// 商店物品篮黙认大小
#define SHOPMALL_SKEP_DEFAULTSIZE			60

// 任务回购物品篮黙认大小
#define PERSON_SHOP_SKEP_DEFAULTSIZE		24

// 人物仓库篮的默认大小
#define PERSON_WAREHOUSE_DEFAULTSIZE		99

// 人物宠物篮的默认大小
#define PERSON_PETLIST_DEFAULTSIZE			24

// 宠物列表的默认大小
#define PET_LORD_PACK_DEFAULTSIZE			12

// 宠物骑宠的默认大小 zgz
#define PET_RIDE_PACK_DEFAULTSIZE			12

// 宠物自动释放技能槽的大小
#define PET_ATUO_SKILL_DEFAULTSIZE          4

// 装备炼魂栏的大小
#define PERSON_EQUIPSMELTSOUL_SKEPSIZE		5 // modify by zjp；只有5个槽

// 装备灵石镶嵌栏的大小
#define PERSON_EQUIPEMBEDGEMS_SKEPSIZE		15

// modify by zjp；现在的策划文档，重铸界面只需要3个slot
// 装备重铸栏的大小
#define PERSON_EQUIPREBUILD_SKEPSIZE		3
//by ldy 内丹合成需要16个slot
#define PERSON_COMPOSE_SKEPSIZE				16
//by ldy, 魂将圣化需要17个slot
#define SOUL_INTENSIFY_SKEPSIZE				17
//by ldy 玄晶可以用于合成的最高等级
#define MAX_PILL_COMPOSE_LEVEL				13
//by ldy,魂将强化的最高圣化级别					
#define MAX_PETINTENSIFY_SANCTIFY			16
//by ldy,魂将极限圣化星级
#define MAX_PET_START						40

// 盒子权限保护时间
#define BOX_POPEDOMPROTECT_TIME				(1 * 1000 * 30)

// 盒子生命周期
#define BOX_LIFECYCLE_TIME					(1 * 1000 * 60)

// 盒子打开停留时间，过后将强制关闭
#define BOX_OPENSETTLE_TIME					(20 * 1000)

// 装备耐久多少点一档
#define EQUIPMENT_ENDURE_POINT2RANK			256

// 界面数据长度
#define UIDATA_LEN							140


//仓库密码最大长度
#define MAX_WAREHOUSEPASSWORD_LEN			16

//仓库与背包金钱上限
#define MAX_PERSON_MONEY					(20 * 10000 * 10000)			

// 队伍成员最多个数
#define TEAM_MEMBER_MAXCOUNT				5

// 无效的队伍ID
#define INVALID_TEAM_ID						0

// Buff效果已经叠加的最大次数
#define MAX_BUFF_OVERLAY_TIMES				100

// 放大比率比(万分比)
#define TENTHOUSAND_MULTIPLE		        10000.00

// 放大比率比(百分比)
#define HUNDRED_MULTIPLE		            100.00

//装备镶嵌的宝石的最大数量
#define EQUIP_EMBED_GEMS_MAX_NUM			4 //宏DBDEF_MAX_EQUIP_EMBEDGEMS要与这里相等

//物品炼魂添加属性的最大数量
#define	EQUIP_SMELT_SOUL_ATTRIB_MAX_NUM		5//宏DBDEF_MAX_EQUIP_SMELTSOULATTRIB要与这里相等

//装备的最多拥有的基础品质属性
#define EQUIP_BASE_ATTRIB_MAX_NUM			4//宏DBDEF_MAX_EQUIP_BASEBRANDATTRIB要与这里相等

//装备刻字的字数上限
#define EQUIP_LETTERING_MAX_NUM				71//宏DBDEF_MAX_EQUIP_LETTERING要与这里相等

//装备修理最低价格
#define EQUIP_REPAIR_MIN_PRICE				1

//装备鉴定的价格
#define EQUIP_IDENTIFY_PRICE				0.1f

// 通用装备篮的大小
#define COM_EQUIPSKEP_SIZE                  11

// 人物个性签名的长度
#define ACTOR_SIGNATURE_MAX_LEN				71

// 宠物战斗属性总数
#define  PET_TYPEPROP_MAX					7

// 采集最大距离
#define  COLLECT_MAX_DISTANCE               2

#define STALL_LEAVEWORD_MSG_LEN					61 // 留言内容最大长度

// 骑宠无限使用
#define  PETRIDE_UNLIMITEDLY				-1

// 家族名称最大长度
const int FAMILY_NAME_LEN = 16;

// 军团名称最大长度
const int ARMY_NAME_LEN = 16;

// 保存商城历史购买记录数
#define ONLINE_SHOP_MAX_HISTORY_PAID_RECORDS 10

// 洗天赋需要的物品(洗髓丹) add wangtao
const int C_RESET_GENIUS_GOOD = 4004;

// 加入阵营需要人物角色10级
const int C_JOIN_CAMP_NEED_PERSON_LV = 10;

// petdata里天赋概率，数组最大长度,单下标为天赋ID，偶下标为天赋出现的概率
const int C_PET_GENIUS_SIZE = 20;
////////////////////////// UID ////////////////////////////////////
typedef LONGLONG UID;
struct ___UID
{
	DWORD dwSerialNo;						// 序列号，不可逆，单通过此ID也可以标识对像
	DWORD dwHandle;							// 对像句柄，可重复的
};

// 分解出序列号
#define ANALYZEUID_SERIALNO(uid)			(((___UID *)&uid)->dwSerialNo)

// 分解出句柄
#define ANALYZEUID_HANDLE(uid)				(((___UID *)&uid)->dwHandle)

/////////////////////// 实体 //////////////////////////////////
// 实体类型
enum
{
	/*
					  00000000 00000001		// 人物 （1）
					  00000000 00000010		// 怪物 （2）
					  00000000 00000011		// 装备 （3）
					  00000000 00000100		// 非装备类物品 （4）
					  00000000 00000101		// 宝箱 （5）
					  00000000 00000110		// 旗杆 （6）

					1 00000000 00000000		// 是否为生物
				   10 00000000 00000000		// 是否为物品
	11111111 11111111 11111111 11111111		// .....	
	//*/

	tEntity_Class_Person = 0x0001,			// 人物
	tEntity_Class_Monster = 0x0002,			// 怪物	
	tEntity_Class_Pet = 0x0003,				// 宠物
	tEntity_Class_Equipment = 0x0004,		// 装备
	tEntity_Class_NonequipableGoods = 0x00005,		// 非装备类物品
	tEntity_Class_Box = 0x00006,			// 宝箱
	tEntity_Class_Mast = 0x00007,			// 旗杆 （摊位、传送门... ...）
	tEntity_Class_Bubble = 0x00008,			// 泡泡 add by zjp

	// -- 以下是外部无需用到的，都是通过接口实现 ----
	tEntity_IsClass_Creature = 0x10000,		// 是否为生物
	tEntity_IsClass_Goods = 0x20000,		// 是否为物品
};

////////////////////////广播对像类型///////////////////////////////
// 广播对像类型，用于IGameZone::BroadcastMessage
enum
{
	/*
					  00000000 00000001		// 广播给人物
					  00000000 00000010		// 广播给怪物
					  00000000 00000100		// 广播给宠物
					  00000000 00001000		// 广播给其他
	11111111 11111111 11111111 11111111		// .....	
	//*/
	tBroadcast_Entity_Person = 0x0001,		// 广播给人物
	tBroadcast_Entity_Monster = 0x0002,		// 广播给怪物
	tBroadcast_Entity_Pet	= 0x0004,		// 广播给宠物
	tBroadcast_Entity_Other = 0x0008,		// 广播给其他
};

////////////////////////生物属生同步方式////////////////////////////
// 生物属生同步方式
enum
{
	/*
					00000000 00000001		// 同步单个属性
					00000000 00000010		// 同步私有属性（只对自已客户端）
					00000000 00000011		// 同步广播属性
	11111111 11111111 11111111 11111111		// .....	
	//*/ // 同步到自已客户端，只支持人物，怪物没有自已客户端
	tCreature_SyncProp_Single = 0x0001,		// 同步单个属性（如果是广播属性，则会广播，如果是私有属性，则同步到自已客户端）
	tCreature_SyncProp_Private = 0x0002,	// 同步私有属性（只有人物支持）
	tCreature_SyncProp_Broadcast = 0x0003,	// 同步广播属性（只有人物支持）
};

////////////////////////////物品装备绑定方式////////////////////////
// add by zjp
enum
{
	eBind_NOT_BIND,					// 不绑定
	eBind_GET_BIND,					// 获取绑定
	eBind_EQUIP_BIND,				// 装备绑定
};

/////////////////////////// 物品绑定标志位 /////////////////////////
enum
{
	/*
					  00000000 00000001		// 是否会获取绑定？
					  00000000 00000010		// 死亡是否会掉落？
					  00000000 00000100		// 是否允许丢弃？
					  00000000 00001000		// 是否允许销毁？
					  00000000 00010000		// 是否允许出售给NPC？
					  00000000 00100000		// 是否允许放个人仓库？
					  00000000 01000000		// 是否允许放氏族仓库？
	11111111 11111111 11111111 11111111		// .....	
	//*/
	tGoods_BindFlag_Hold = 0x001,			// 是否已经绑定
	tGoods_BindFlag_Trade = 0x002,			// 可否交易
	tGoods_BindFlag_Stall =  0x004,			// 可否摆摊出售
	tGoods_BindFlag_Throw = 0x008,			// 是否允许丢弃
	tGoods_BindFlag_Auction = 0x010,		// 可否拍卖
	tGoods_BindFlag_Mail = 0x020,			// 可否邮寄
	tGoods_BindFlag_DieDrop = 0x040,		// 可否会掉落
	tGoods_BindFlag_SaleToNPC = 0x080,		// 是否允许出售给NPC？
	tGoods_BindFlag_Smelt = 0x100,			// 可否参与打造
	tGoods_BindFlag_Split = 0x200,			// 可否拆分
	tGoods_BindFlag_WareHouse = 0x400,		// 是否允许放仓库？
	tGoods_BindFlag_Destroy = 0x800,		// 是否允许丢弃就销毁？by cjl
};

//by cjl
enum
{
	tChangeBody_BindFlag_Pet	 = 0x001, //是否可携带宠物
	tChangeBody_BindFlag_Buffer  = 0x002, //是否携带buffer
	tChangeBody_BindFlag_Common  = 0x004, //是否普通变身
	tChangeBody_BindFlag_XP		 = 0x008, //是否能放XP
};
//end cjl

// add by zjp；绑定状态对应属性集(只有两种状态，并且策划要求每种状态对应的属性相同)
enum 
{
	eBind_Attribute =   tGoods_BindFlag_Hold+
						tGoods_BindFlag_SaleToNPC+
						tGoods_BindFlag_Smelt+
						tGoods_BindFlag_Split+
						tGoods_BindFlag_Throw+		//by cjl
						tGoods_BindFlag_WareHouse,

	eNoBind_Attribute =   tGoods_BindFlag_Trade+
						  tGoods_BindFlag_Stall+
						  tGoods_BindFlag_Throw+
						  tGoods_BindFlag_Auction+
						  tGoods_BindFlag_Mail+
						  tGoods_BindFlag_DieDrop+
						  tGoods_BindFlag_SaleToNPC+
						  tGoods_BindFlag_Smelt+
						  tGoods_BindFlag_Split+
						  tGoods_BindFlag_WareHouse,
};

//////////////////////// 玩家ＶＩＰ权限 //////////////////////////
enum
{
	tVIP_Popedom_Kickout = 0x001,			// 踢人
	tVIP_Popedom_Estop = 0x002,				// 禁言
	tVIP_Popedom_Forbid_Login = 0x004,		// 禁止登录
	tVIP_Popedom_Through = 0x008,			// 定点传送
	tVIP_Popedom_Call_In = 0x010,			// 拉人
	tVIP_Popedom_Transmission = 0x020,		// 传送指定角色身边
	tVIP_Popedom_Hide = 0x040,				// 隐身
	tVIP_Popedom_First_Login = 0x080,		// 优先登录
	tVIP_Popedom_Free_Through = 0x100,		// 免费传送
	tVIP_Popedom_Free_WorldChat = 0x200,	// 世界发言不需要钱
	tVIP_Popedom_Special_Channel = 0x400,	// 特殊频道发言
	tVIP_Popedom_Consult_TaskData = 0x800,	// 查询任务数据
	tVIP_Popedom_Change_TaskData = 0x1000,	// 修改交互数据值
	tVIP_Popedom_Consult_SvrState = 0x2000,	// 查询当前服务器状况
};

///////////////////////////// 搜路选项 //////////////////////////
enum
{
	FINDPATH_OPTION_EMPTY      =0x00,		// 最精简的搜路
	FINDPATH_OPTION_OPTIMIZE   =0x01,		// 优化路径
	FINDPATH_OPTION_NEAREST    =0x02,		// 如果到不了最终点则移动到最近的点	
};

////////////////////////////// 魂类型 //////////////////////////
enum
{
	tLingHun_State_Ming = 0x00,				// 命魂态
	tLingHun_State_Di,						// 地魂态
	tLingHun_State_Tian,					// 天魂态
	tLingHun_State_Max,						// 最大
};

enum
{
	EN_BUFF_MAX_OVERLAY    =  100,          // BUFF允许最大叠加次数
	EN_BUFF_MAX_COEXIST    =  100,          // BUFF允许最大并存个数
};

enum
{
	tPet_State_Fighting = 0x001,		// 宠物是否出征？			
	tPet_State_Riding = 0x002,			// 宠物是否骑乘？
	tPet_State_Combining = 0x004,		// 宠物是否合体？
	tPet_State_InterimCallBack = 0x008, // 宠物是否临时召回？
};

#pragma pack()