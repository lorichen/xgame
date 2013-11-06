/*******************************************************************
** 文件名:	e:\Rocket\Common\DTeamModule.h
** 版  权:	(C) 
** 创建人:	pdata(Forlion Long)（Forlion@126.com）
** 日  期:	2008/6/4  15:02
** 版  本:	1.0
** 描  述:	组队模块定义	
** 应  用:	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "DGlobalGame.h"
#include <list>
#include <vector>
using namespace std;

#pragma pack(1)
/******************************************************************/
///////////////////// 组队的公共枚举定义 ////////////////////////
/******************************************************************/
///////////////////////////////////////////////////////////////////
enum 
{
	TEAMDATA_TEAM_TEAMDEFAULT =0,
	TEAMDATA_TEAM_TEAMTYPE = 1,
	TEAMDATA_TEAM_GOODSALLOTMODE,
	TEAMDATA_TEAM_PETSALLOTMODE,
	TEAMDATA_TEAM_TEAMPRIVATE,
};
enum 
{
	TEAMMODE_TASK = 0,     // 任务
	TEAMMODE_ECTYPAL,      // 副本
	TEAMMODE_BATTLEFIELD,  // 战场
	TEAMMODE_TOUPGRADE,    // 练级
	TEAMMODE_NORMAL,       // 普通
	TEAMMODE_MAXID,        // 最大ID  
};
enum
{
	GOODSALLOTMODE_W_FREE = 0, // 白装自由拾取
	GOODSALLOTMODE_G_FREE,     // 绿装自由拾取
	GOODSALLOTMODE_B_FREE,     // 蓝装自由拾取
	GOODSALLOTMODE_P_FREE,     // 紫装自由拾取
	GOODSALLOTMODE_ALLFREE,    // 自由拾取
	GOODSALLOTMODE_MAXID,   // 最大ID  
};
enum
{
	PETALLOTMODE_AVERAGE = 0, // 平均分配
	PETALLOTMODE_FREE,        // 自由拾取
	PETALLOTMODE_MAXID,       // 最大ID
};

// 枚举出分配的物品类型
enum 
{
	RANDOM_GOODS_DEFAULT = 0,
	RANDOM_GOODS_PET,
	RANDOM_GOODS_GOODS,
	RANDOM_GOODS_EQUIP,
	RANDOM_GOODS_MAX,
};
#define    MAX_TEAMNAME_LEN            20
/******************************************************************/
///////////////////// 组队的装备查看结构体定义 /////////////////////
/******************************************************************/
// 目前将此结构暂时定义在组队模块，
///////////////////////////////////////////////////////////////////

//装备相关
#	define	MAX_EQUIP_EMBEDGEMS	4
#	define	MAX_EQUIP_BASEBRANDATTRIB	4
#	define	MAX_EQUIP_SMELTSOULATTRIB	5
#	define	MAX_EQUIP_LETTERING	71

struct SEquipmentViewInfo
{
	int  nEquipID;                      // 装备ID
	int  nEquipSmeltSoulLevel;			// 物品链魂强化等级
    int  nEquipBind;                    // 绑定
	int  nEquipEndureCur;               // 当前耐久度
	int  nEquipEndureMax;               // 最大耐久
	int  nEquipIsChecked;               // 物品是否已经鉴定
	int  nEquipBrandBaseLevelID[MAX_EQUIP_BASEBRANDATTRIB];        // 物品品质属性 id
	int  nEquipBrandBaseLevelVAL[MAX_EQUIP_BASEBRANDATTRIB];       // 物品品质属性 值
	int  nEquipSmeltSoulMileStoneBaseID[MAX_EQUIP_SMELTSOULATTRIB]; // 物品强化等级里程碑id
	int  nEquipSmeltSoulMileStoneBaseVAL[MAX_EQUIP_SMELTSOULATTRIB];// 物品强化等级里程碑 值
	int  nEquipSlotQTY;					                            // 插槽数
	int  nEquipGemsEmbededNum;     // 灵石数目
	int  nEquipGemsBaseID[MAX_EQUIP_EMBEDGEMS];         // 镶嵌的灵石的id
	int  nEquipGemsCombindID;                                 // 组合后的属性值
    char szLetter[MAX_EQUIP_LETTERING];                 // 刻字
	SEquipmentViewInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
typedef vector<SEquipmentViewInfo>   TVECTOR_EQUIPMENTVIEWINFO;

/******************************************************************/
///////////////////// 组队的公共结构体定义 ////////////////////////
/******************************************************************/
///////////////////////////////////////////////////////////////////
struct SMemberInfo
{
	DWORD		dwPDBID;					// pdbid
	DWORD       dwLevel;                    // 等级
	DWORD       dwVocation;                 // 职业
	DWORD       dwFightValue;               // 战斗力
	char		szName[MAX_PERSONNAME_LEN];	// 名字
	long		lFaceID;					// 头像id
	bool		bOnline;					// 在线与否

	SMemberInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
typedef list<SMemberInfo>	TLIST_MEMBERINFO;

/******************************************************************/
///////////////////////// 组队的消息码定义 ////////////////////////
// 上层的消息码必定是MSG_MODULEID_TEAM
/******************************************************************/
///////////////////////////////////////////////////////////////////
// 邀请或者申请组队
#define	MSG_TEAM_INVITE											1
// 消息体
struct SMsgTeamInvite_CS
{
	DWORD		dwInvitee;					// 被邀请者
	bool        bInvite;                    // 是否为邀请
	
	SMsgTeamInvite_CS(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

struct SMsgTeamInvite_SO
{
	DWORD		dwInvitee;					// 被邀请者

	char		szName[MAX_PERSONNAME_LEN];	// 被邀请者名字

	SMemberInfo request;					// 邀请者
    
	bool        bInvite;                    // 是邀请还是申请

	SMsgTeamInvite_SO(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 组队的滴答消息
#define MSG_TEAM_DIDA											2
// 消息体
#define DIDATXT_DEFAULTLEN	512
struct SMsgTeamDida_SC
{
	DWORD       dwPBIDRquest;               // 被邀请者PBID
	int			nStayTime;					// 停留时间
	char		szHtml[DIDATXT_DEFAULTLEN];	// dida内容

	SMsgTeamDida_SC(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

struct SMsgTeamDida_OS
{
	DWORD		dwPDBID;					// 发给谁
	DWORD       dwPBIDRquest;               // 被邀请者PBID
	int			nStayTime;					// 停留时间
	char		szHtml[DIDATXT_DEFAULTLEN];	// dida内容

	SMsgTeamDida_OS(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

///////////////////////////////////////////////////////////////////
// 创建队伍
#define MSG_TEAM_CREATETEAM										3
#define TEAMNAME_LEN	32
// 消息体
struct SMsgTeamCreateTeam_OS
{
	char        szName[TEAMNAME_LEN];       // 队伍名称
	DWORD		dwTeamID;					// 队伍ID
	DWORD		dwCaptain;					// 队长UID
	DWORD       dwTeamMode;                 // 队伍类型
	DWORD		dwGoodsAllotMode;			// 物品分配模式
	DWORD       dwPetAllotMode;             // 宠物分配模式
	bool         bPrivateOpen;              // 私人专属队伍
	SMemberInfo  creater;                   // 创建者的信息
	SMsgTeamCreateTeam_OS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
struct SMsgTeamCreateTeam_CS       // 客户端-服务器
{
	char  szName[TEAMNAME_LEN];    // 队伍名字
	DWORD dwCaptain;               // 队长UID
	DWORD dwTeamMode;              // 队伍类型   
	DWORD dwGoodsAllotMode;        // 物品分配模式
	DWORD dwPetAllotMode;          // 宠物分配模式
	bool  bPrivateOpen;            // 私人专属队伍
	SMsgTeamCreateTeam_CS(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

struct SMsgTeamCreateTeam_SC      // 服务器-客户端
{
	char  szName[TEAMNAME_LEN];    // 队伍名字
	DWORD dwTeamID;
	DWORD dwCaptain;               // 队长UID
	DWORD dwTeamMode;              // 队伍类型   
	DWORD dwGoodsAllotMode;        // 物品分配模式
	DWORD dwPetAllotMode;          // 宠物分配模式
	bool  bPrivateOpen;            // 私人专属队伍
	SMsgTeamCreateTeam_SC(void)
	{
		memset(this, 0, sizeof(*this));
	};
};
struct SMsgTeamCreateTeam_SO       // 服务器-社会服
{
	char  szName[TEAMNAME_LEN];     // 队伍名字
	DWORD dwCaptain;                // 队长UID
	DWORD dwTeamMode;               // 队伍模式
	DWORD dwGoodsAllotMode;         // 物品分配模式
	DWORD dwPetAllotMode;           // 宠物分配模式
	bool  bPrivateOpen;             // 私人专属队伍
	SMemberInfo  creater;           // 创建者的信息
	SMsgTeamCreateTeam_SO(void)
	{
		memset(this, 0, sizeof(*this));
	};
};
///////////////////////////////////////////////////////////////////
// 销毁队伍
#define MSG_TEAM_DESTROYTEAM									4
// 消息体
struct SMsgTeamDestroyTeam_OS
{
	DWORD		dwTeamID;					// 队伍ID

	SMsgTeamDestroyTeam_OS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 队员变化
#define MSG_TEAM_CHANGEMEMBER									5
// 消息体
struct SMsgTeamChangeMember_OS
{
	DWORD		dwTeamID;					// 队伍ID
	int			nCount;						// 成员个数
	// nCount * SMemberInfo					// 成员数据

	SMsgTeamChangeMember_OS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SMsgTeamChangeMember_SC
{
	DWORD		dwTeamID;					// 队伍ID
	int			nCount;						// 成员个数
	// nCount * SMemberInfo					// 成员数据

	SMsgTeamChangeMember_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 队长变化
#define MSG_TEAM_CHANGECAPTAIN									6
// 消息体

struct SMsgTeamChangeCaptain_OS
{
	DWORD		dwTeamID;					// 队伍ID
	DWORD		dwCaptain;					// 队长
	DWORD       dwOldCaptain ;              // 前任队长

	SMsgTeamChangeCaptain_OS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SMsgTeamChangeCaptain_SC
{
	DWORD		dwTeamID;					// 队伍ID
	DWORD		dwCaptain;					// 队长

	SMsgTeamChangeCaptain_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 分配模式变化
#define MSG_TEAM_CHANGEALLOTMODE								7
// 消息体
struct SMsgTeamChangeAllotMode_OS
{
	DWORD		dwTeamID;					// 队伍ID
	long		lAllotMode;					// 分配模式

	SMsgTeamChangeAllotMode_OS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SMsgTeamChangeAllotMode_SC
{
	DWORD		dwTeamID;					// 队伍ID
	long		lAllotMode;					// 分配模式

	SMsgTeamChangeAllotMode_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SMsgTeamChangeAllotMode_CS
{
	long		lAllotMode;					// 分配模式

	SMsgTeamChangeAllotMode_CS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SMsgTeamChangeAllotMode_SO
{
	DWORD		dwPDBID;					// 客户端PDBID
	long		lAllotMode;					// 分配模式

	SMsgTeamChangeAllotMode_SO(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


///////////////////////////////////////////////////////////////////
// 转发消息
#define MSG_TEAM_TRANSMIT										8
// 消息体
struct SMsgTeamTransmit_OS
{
	DWORD		dwPDBID;					// 客户端PDBID
	long		lMsgLen;					// 消息长度
	// ..............						// 消息内容

	SMsgTeamTransmit_OS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 客户端对各种请求/申请的链接
#define MSG_TEAM_INVITELINK										9
// 消息体
struct SMsgTeamInviteLink_SO
{	
	DWORD		dwInviteID;					// 请求ID
	bool        bOnTimer;                   // 是否是因为计数器到时了；
	bool		bSelect;					// 同意/拒绝

	SMemberInfo select;						// 选择者

	SMsgTeamInviteLink_SO(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 转发聊天信息
#define MSG_TEAM_TRANSCHATMESSAGE								10
// 消息
struct SMsgTeamTransChatMessage_OS
{
	DWORD		dwPDBID;					// PDBID
	int			nChannelType;				// 聊天频道
	int			nPos;						// 位置
	int			nChatLen;					// 内容长度
	// ...............						// 内容	

	SMsgTeamTransChatMessage_OS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 加入队伍
#define  MSG_TEAM_ENTERTEAM										11
// 消息体
struct SMsgTeamEnterTeam_OS
{
	char        szName[TEAMNAME_LEN];       // 队伍名称
	DWORD		dwPDBID;					// PDBID
	DWORD		dwTeamID;					// 队伍ID
	DWORD		dwCaptain;					// 队长
	DWORD       dwTeamMode;                 // 队伍类型
	DWORD		dwGoodsAllotMode;			// 物品分配模式
	DWORD       dwPetAllotMode;             // 宠物分配模式
	bool        bPrivateOpen;               // 私人专属队伍
	

	SMsgTeamEnterTeam_OS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SMsgTeamEnterTeam_SC
{
	char        szName[TEAMNAME_LEN];       // 队伍名称
	DWORD		dwTeamID;					// 队伍ID
	DWORD		dwCaptain;					// 队长
	DWORD       dwTeamMode;                 // 队伍类型
	DWORD		dwGoodsAllotMode;			// 物品分配模式
	DWORD       dwPetAllotMode;             // 宠物分配模式
	bool        bPrivateOpen;               // 私人专属队伍
	SMsgTeamEnterTeam_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 退出队伍
#define MSG_TEAM_QUITTEAM										12
// 消息体
struct SMsgTeamQuitTeam_CS
{
	bool  bLogOut;                          // 是否为下线退队
};

struct SMsgTeamQuitTeam_SO
{
	DWORD		dwPDBID;					// PDBID
	bool        bLogOut;                    // 是否为下线退队
	SMsgTeamQuitTeam_SO(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SMsgTeamQuitTeam_OS
{	
	DWORD		dwPDBID;					// PDBID
	DWORD		dwTeamID;					// 队伍ID
	bool        bLogOut;                    // 是否为下线退队
	SMsgTeamQuitTeam_OS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SMsgTeamQuitTeam_SC
{
	DWORD		dwPDBID;					// PDBID
	DWORD		dwTeamID;					// 队伍ID
	bool        bLogOut;					// 是否为下线退队
	SMsgTeamQuitTeam_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 开除成员
#define MSG_TEAM_DISMISSMEMBER									13
// 消息体
struct SMsgTeamDismissMember_CS
{
	DWORD		dwPDBID;					// 被开除者

	SMsgTeamDismissMember_CS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SMsgTeamDismissMember_SO
{
	DWORD		dwCaptain;					// 队长
	DWORD		dwPDBID;					// 被开除者

	SMsgTeamDismissMember_SO(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 客户端对投票的链接
#define MSG_TEAM_VOTELINK										14
// 消息体
struct SMsgTeamVoteLink_SO
{	
	DWORD		dwPDBID;					// 客户端
	int			nVoteID;					// 投票ID
	bool		bSelect;					// 同意/拒绝	

	SMsgTeamVoteLink_SO(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

///////////////////////////////////////////////////////////////////
// 任命为队长
#define MSG_TEAM_APPOINTCAPTAIN									15
// 消息体
struct SMsgTeamAppointCaptain_CS
{
	DWORD		dwPDBID;					// 任命谁

	SMsgTeamAppointCaptain_CS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};	

struct SMsgTeamAppointCaptain_SO
{
	DWORD		dwClient;					// 客户端
	DWORD		dwPDBID;					// 任命谁

	SMsgTeamAppointCaptain_SO(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
///////////////////////////////////////////////////////////////////
// 搜索队伍
#define MSG_TEAM_SEARCH                                          16

struct TeamSearchInfo
{
	char  szName[TEAMNAME_LEN];     // 队伍名字
	DWORD dwTeamMode;               // 队伍模式
	DWORD dwCaptain;                // 队长UID
	char  szCaptainName[MAX_PERSONNAME_LEN];     // 队长名字
	int   nCaptainLevel;             // 队长等级
	int   nCaptainVocation;           // 队长职业	
	int   nMemberNum;               // 队员人数

	// 队伍信息
	TeamSearchInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
typedef vector<TeamSearchInfo>  TVECTOR_SEARCHINFO;

struct SMsgTeamSearch_CS
{
	DWORD		dwPDBID;					// 搜索者

	SMsgTeamSearch_CS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

struct SMsgTeamSearch_SC
{
	DWORD		dwPDBID;					// 搜索者
	int         nCount;                    // 队伍数目
	 // 队伍信息
	SMsgTeamSearch_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
///////////////////////////////////////////////////////////////////
// 申请加入队伍
#define MSG_TEAM_APPLY                                          17

struct SMsgTeamApply_CS
{
	DWORD		dwPDBID;					// 申请者
	int         dwCaptain;                  // 队长
	// 队伍信息
	SMsgTeamApply_CS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
///////////////////////////////////////////////////////////////////

// 本队队伍信息查询
#define  MSG_TEAM_SELFTEAMMSGVIEW                               18
struct SMsgTeamSelfMsgView_CS
{
	DWORD		dwPDBID;					// 申请者
	DWORD       dwTeamID;                   // 队伍ID
	bool        bClick;                     // 点击显示
	// 队伍信息
	SMsgTeamSelfMsgView_CS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
struct SMsgTeamSelfMsgView_SC
{
	DWORD		dwPDBID;					// 申请者
	DWORD       dwCaptainID;                // 
	DWORD       dwCaptainArmy;             
	DWORD       dwCaptainHomage;
	bool        bClick;
	int         nCount;
	// 队伍信息
	SMsgTeamSelfMsgView_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
///////////////////////////////////////////////////////////////////

// 跟随队长
#define  MSG_TEAM_FOLLOWCAPTAIN                               19
struct SMsgTeamFollowCaptain_CS
{
	DWORD		dwPDBID;					// 申请者
	DWORD       dwTeamID;                   // 队伍ID
	// 队伍信息
	SMsgTeamFollowCaptain_CS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
struct SMsgTeamFollowCaptain_SC
{
	DWORD		dwPDBID;					// 申请者
	LONGLONG    lCaptainUID;                // 队长UID
	DWORD       dwTeamID;                   // 队伍ID
	// 队伍信息
	SMsgTeamFollowCaptain_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
//////////////////////////////////////////////////////////////////
// 宠物分配模式变化
#define MSG_TEAM_CHANGEPETALLOTMODE								20
// 消息体
struct SMsgTeamChangePetAllotMode_OS
{
	DWORD		dwTeamID;					// 队伍ID
	long		lAllotMode;					// 分配模式

	SMsgTeamChangePetAllotMode_OS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
struct SMsgTeamChangePetAllotMode_SC
{
	DWORD		dwTeamID;					// 队伍ID
	long		lAllotMode;					// 分配模式

	SMsgTeamChangePetAllotMode_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////
// 修改队伍名字
#define MSG_TEAM_CHANGETEAMNAME								21
// 消息体
struct SMsgTeamChangeTeamName_OS
{
	DWORD		dwTeamID;					// 队伍ID
	char        szName[TEAMNAME_LEN];       // 队伍名称

	SMsgTeamChangeTeamName_OS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
struct SMsgTeamChangeTeamName_SC
{
	DWORD		dwTeamID;					// 队伍ID
	char        szName[TEAMNAME_LEN];       // 队伍名称

	SMsgTeamChangeTeamName_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////
// 修改队伍私有属性
#define MSG_TEAM_CHANGETEAMPRIVATE						     22
// 消息体
struct SMsgTeamChangeTeamPrivate_OS
{
	DWORD		dwTeamID;					// 队伍ID
	bool        bPrivate;

	SMsgTeamChangeTeamPrivate_OS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
struct SMsgTeamChangeTeamPrivate_SC
{
	DWORD		dwTeamID;					// 队伍ID
	bool        bPrivate;

	SMsgTeamChangeTeamPrivate_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
//////////////////////////////////////////////////////////////////
// 修改队伍类型
#define MSG_TEAM_CHANGETEAMMODE						     23
// 消息体
struct SMsgTeamChangeTeamMode_OS
{
	DWORD		dwTeamID;					// 队伍ID
	long        lTeamMode;                  // 队伍类型

	SMsgTeamChangeTeamMode_OS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
struct SMsgTeamChangeTeamMode_SC
{
	DWORD		dwTeamID;					// 队伍ID
	long        lTeamMode;                  // 队伍类型

	SMsgTeamChangeTeamMode_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
//////////////////////////////////////////////////////////////////
// 其他队伍信息查询
#define  MSG_TEAM_OTHERTEAMMSGVIEW                          24
struct SMsgTeamOtherMsgView_CS
{
	DWORD		dwPDBID;					// 查看者
	DWORD       dwCaptain;                  // 队长ID
	// 队伍信息
	SMsgTeamOtherMsgView_CS(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
struct SMsgTeamOtherMsgView_SC
{
	DWORD		dwPDBID;			// 查看者
	char        szName[TEAMNAME_LEN];     // 队伍名字
	DWORD       dwTeamMode;               // 队伍模式
	DWORD       dwGoodsAllotMode;         // 物品分配模式
	DWORD       dwPetAllotMode;           // 宠物分配模式
	bool        bPrivate;                 // 是否私有
	DWORD       dwCaptainID;              // 队长ID
	DWORD       dwCaptainArmy;             
	DWORD       dwCaptainHomage;
	int         nCount;                    // 队员数目
	// 队伍信息
	SMsgTeamOtherMsgView_SC(void)
	{
		memset(this, 0, sizeof(*this));
	}
};
//////////////////////////////////////////////////////////////////
// 向有效队员发送掷骰子界面
#define  MSG_TEAM_RANDOMDIDA                          25

struct SMsgTeamRandomDida_SC
{
	DWORD		dwPDBID;					// 发给谁
	DWORD       dwTeamID;                   // 队伍ID
	DWORD       dwGoodsID;                  // ID
	int			nStayTime;					// 停留时间
	int         nRandomNum;                 // 随机数
	LONGLONG    lGoodsUid;                  // 对应物品
	int         nReandomGoodType;           // 物品类型
	long        lImageID;                   // 图片ID

	SMsgTeamRandomDida_SC(void)
	{
		memset(this, 0, sizeof(*this));
	};
};
struct SMsgTeamRandomDida_CS
{
	DWORD		dwPDBID;					// 发给谁
	DWORD       dwTeamID;                   // 队伍ID；
	LONGLONG    lGoodsUid;                  // 所要拾取的物品
	bool        bNeed;                      // 是否需要
	SMsgTeamRandomDida_CS(void)
	{
		memset(this, 0, sizeof(*this));
	};
};
//////////////////////////////////////////////////////////////////
struct SMsgTeamViewTeamMemberData_SC
{
	DWORD		dwViewPDBID;			    // 查看者
	DWORD       dwReviewPDBID;              // 被查看者
	char        szName[TEAMNAME_LEN];       // 被查看者的名字
	DWORD       dwLevel;                    // 等级
	DWORD       dwSex;                      // 性别
	DWORD       dwVocation;                 // 职业
	DWORD       dwCurHP;                    // 当前生命值
	DWORD       dwMaxHP;                    // 最大生命值
	DWORD       dwCurMP;                    // 当前魔法值
	DWORD       dwMaxMP;                    // 最大魔法值
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
	LONGLONG    uID;                        // UID
	SMsgTeamViewTeamMemberData_SC(void)
	{
		memset(this, 0, sizeof(*this));
	};
};
//////////////////////////////////////////////////////////////////
// 查看队员在地图的显示信息
// 队员信息结构
struct TeamMemberMapPlace 
{
	char		szName[MAX_PERSONNAME_LEN];	// 名字
	POINT       ptTitle;                    // 位置
	bool        bCaptain;                   // 是否为队长
};
typedef vector<TeamMemberMapPlace>   TVECTOR_TEAMMEMBERMAPPLACE; 
#define  MSG_TEAM_VIEWMEMBER_MAPPLACE                          26
struct SMsgTeamViewMemeberMapPlace_CS
{
	int     nMapId;           // 需要显示地图ID
};
struct SMsgTeamViewMemeberMapPlace_SC
{
	int     nMapId;           // 需要显示地图ID
	int     nMemberCount;     // 存在的队员个数
};

// 去取得其他队伍数据 
#define  MSG_TEAM_GETOTHER_TEAMDATA               27
struct SMsgTeamGetOtherTeamData_CS
{
	DWORD     dwViewerPDBID;       // 查看者
	DWORD     dwPDBID;             // 被查看的人物ID；
	WORD      wDataType;           // 数据类型
};
struct SMsgTeamGetOtherTeamData_SC
{
	DWORD     dwPDBID;             // 被查看的人物ID；
	WORD      wDataType;           // 数据类型
	int		  nData;               // 
};

// 去取得目标队伍数据 
#define  MSG_TEAM_GETTARGET_TEAMDATA               28
struct SMsgTeamGetTargetTeamData_CS
{
	DWORD     dwTargetPDBID;              // 目标ID
	char      szName[TEAMNAME_LEN];       // 目标名字
	int       nFaceID;                    // 目标头像ID
};
struct SMsgTeamGetTargetTeamData_SC
{
	DWORD     dwTargetPDBID;              // 目标ID
	char      szName[TEAMNAME_LEN];       // 目标名字
	int       nFaceID;                    // 目标头像ID
	bool      bInTeam;                    // 是否在队伍
	bool      bCaptain;                   // 是否为队长
};

// 邀请DIDA繁忙的消息提示 
#define  MSG_TEAM_INVITE_DIDABUSY             29
struct SMsgTeamInviteDidaBusy_CS
{
	DWORD     dwTargetPDBID;              // 目标ID
	char      sztoolTip[256];             // tip内容；
};

// 更新队伍人物属性值
#define MSG_TEAM_UPDATE_PROPVALUE              30
struct  SMsgTeamUpdatePropValue_SO
{
	DWORD     dwPDBID;     // 需要更新的人物ID
	DWORD     dwLevel;     // 等级
	DWORD     dwCurHP;     // 气血
	DWORD     dwMaxHP;     // 最大气血上限
	DWORD     dwCurMP;     // MP
	DWORD     dwMaxMP;     // MAXMP
};
struct  SMsgTeamUpdatePropValue_OC
{
	DWORD     dwPDBID;     // 需要更新的人物ID
	DWORD     dwLevel;     // 等级
	DWORD     dwCurHP;     // 气血
	DWORD     dwMaxHP;     // 最大气血上限
	DWORD     dwCurMP;     // MP
	DWORD     dwMaxMP;     // MAXMP
};

// 成员更新以后的加入
#define  MSG_TEAM_ENTERTEAM_UPDATA				31

#define  MSG_TEAM_QUITTEAM_UPDATA				32


#pragma pack()