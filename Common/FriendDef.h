//==========================================================================
/**
* @file	  : FriendDef.h
* @author : pk <pk163@163.com>
* created : 2008-6-16   10:48
* purpose : 好友系统相关定义
*/
//==========================================================================

#ifndef __FRIENDDEF_H__
#define __FRIENDDEF_H__

#include "UserInfoDef.h"
#include <string>
using namespace std;

const static DWORD FRIEND_INVITE_INTERVAL_MIN = 30000; //好友邀请操作最小时间间隔 30000ms

const static DWORD FRIEND_ACCEPT_INVITE_INTERVAL_MAX = 60; //好友接受邀请超时时间 60s

const static DWORD FRIENDPOINT_TRADE_NUM_PERDAY = 4;//交易加友好度，每天最多4次

const static int FRIEND_DEFAULT_GROUPID = 1; //默认好友分组ID
const static string FRIEND_DEFAULT_GROUPNAME = "好友"; //默认好友分组名

const static int FRIEND_GROUP_NUM_MAX = 5;//1个默认分组 + 4个自定义分组

#define MAX_FRIENDGROUP_NAME 12

enum 
{
	MAX_FRIEDN_NUM = 100,                /// 最大好友数 
	MAX_ENEMY_NUM  = 150,				/// 最大仇人数 
	MAX_RECENT_NUM = 100,               /// 最近联系人最大数
	MAX_SHIELD_NUM = 150,               /// 最多屏蔽玩家数
};

/// 好友关系
enum FriendRelation
{
	FriendRelation_Buddy      =  1,  				/// 好友
	FriendRelation_Stranger,			/// 陌生人
	FriendRelation_Blacklist,			/// 黑名单
	FriendRelation_Enemy,				/// 仇人
	FriendRelation_Shield,              /// 屏蔽的好友或仇人
};

enum RemoveFriendReason
{
	Friend_Logoff      =   1 //好友下线
};

enum AddEnemyReason
{
	Murder   = 1,  //被杀
	Manu     = 2,  //手动添加
};

enum  FriendOperate
{
	FriendOperate_StartInvite = 1,
	FriendOperate_Chat		  = 2,
	FriendOperate_Trade		  = 3,
	FriendOperate_Interchange = 4,
};

enum FriendPointTimeInterval
{
	FriendPoint_Interval_Chat    = 7200,   // 密语、聊天 2小时一次 单位：s
	FriendPoint_Interval_Trade   = 86400,  // 交易 每天4次    单位：s
	FriendPoint_Interval_Team    = 1440000,// 组队 每24分种 增加一点 单位：ms
};

enum FriendPointADD
{
	FriendPoint_Chat    = 1, // 密语、聊天 增加1点
	FriendPoint_Trade   = 1, // 交易 增加1点
	FriendPoint_TEAM	= 1, // 组队 增加一点

	FriendPoint_BADPK   = 100, // 恶意PK，减少100点 暂定	
	FriendPoint_MAX     = 1500,
};
#pragma pack(push)
#pragma pack(1)

/// 用户信息
struct FriendGroupInfo
{
	DWORD     GroupID;         /// 分组ID
	char	  GroupName[MAX_PERSONNAME_LEN + 1];	/// GroupNmae
	int       nMemberNum;
	int       nOnlineNum;
	FriendGroupInfo()
	{
		memset(this, 0, sizeof(FriendGroupInfo));
	}

	void OnAddFriend(bool bOnline)
	{
		++nMemberNum;
		if (bOnline)
		{
			++nOnlineNum;
		}
		if (nOnlineNum > nMemberNum)
		{
			nOnlineNum = nMemberNum;
		}
	}

	void OnRemoveFriend(bool bOnline)
	{	
		if (nMemberNum > 0)
		{
			--nMemberNum;
		}
		if (bOnline && nOnlineNum > 0)
		{
			--nOnlineNum;
		}	

		if (nOnlineNum < 0)
		{
			nOnlineNum = 0;
		}
	}
};

/// 好友列表中的好友记录
struct FriendRecord
{
	ulong   FriendID;                           /// 好友ID
	ushort	Relation;							/// 好友关系
	int     FriendGroup;                        /// 好友组别
	ulong   FriendPoint;                        /// 友好度
	ushort  TeamAddPoint;                       /// 组队增加的友好度
	ushort  TalkAddPoint;                       /// 聊天增加的友好度
	ushort  TradeAddPoint;                      /// 交易增加的友好度
	ushort  InterchangeAddPoint;                /// 切磋增加的友好度
	ushort  GiveGoodsAddPoint;                  /// 赠送增加友好度点数
	ushort  UseGoodsAddPoint;                   /// 使用物品增加的友好度

	// 按照新需求，屏蔽统一在聊天模块做了，这里的没用了 [4/15/2011 zgz]
	bool    bShield;                            /// 是否被屏蔽 

	bool    bRecently;                          /// 是否为最近
	ushort  MurderNumber;                       /// 被谋杀次数
	ulong	FaceID;					            /// 头像
	int		Level;								/// 等级
	uchar	Profession;							/// 职业
	bool	Online;					            /// 是否在线
	int     nMapID;                             /// 地图ID
	char	Name[32];							/// 用户名
	uchar	Nation;							    /// 国家
	FriendRecord()
	{
		memset(this, 0 ,sizeof(FriendRecord));
	}
};

#pragma pack(pop)

////////////////////////// 好友消息定义//////////////////////
// 客户端发给社会服务器
/// 更新用户信息(CS,SC)
#define   FriendMsg_UpdateFriendInfo                 1

struct  SMsgFriend_UpdateFirendGroup_SC  
{
	DWORD   dwActor;  
	int      nCount;
	SMsgFriend_UpdateFirendGroup_SC(void)
	{
		memset(this, 0, sizeof(*this));
	};
};    

/// 更新系统配置
#define   FriendMsg_UpdateSysConfig                  2

/// 更新好友关系(添加或修改)
#define   FriendMsg_UpdateFriendRelation             3
   
/// 删除好友 (dbid actorId, dbid friendId)
#define   FriendMsg_RemoveFriend                     4

struct  SMsgFriend_RemoveFriend_SC  
{
	dbid userPID;
	dbid firendPID;
	SMsgFriend_RemoveFriend_SC(void)
	{
		memset(this, 0, sizeof(*this));
	};
};  
/// 请求好友信息
#define   FriendMsg_RequestFriendInfo                5

/// 发送聊天消息
#define   FriendMsg_SendMsg                          6

// 社会服务器发给客户端
/// 更新好友列表
#define   FriendMsg_UpdateFriendList                 7

struct    SMsgFriend_UpdateFirendlist_OS
{
	DWORD   dwActor;                      // 需要更新谁的好友列表
    int     nCount;                       // 好友个数
	SMsgFriend_UpdateFirendlist_OS(void)
	{
		memset(this, 0, sizeof(*this));
	};
};
struct   SMsgFriend_UpdateFirendlist_SC
{
	DWORD   dwActor;                      // 需要更新谁的好友列表
	int     nCount;                       // 好友个数
	SMsgFriend_UpdateFirendlist_SC(void)
	{
		memset(this, 0, sizeof(*this));
	};
};
/// 反馈好友信息
#define   FriendMsg_ResponseFriendInfo               8

/// 转发聊天消息
#define   FriendMsg_MsgArrived                       9

/// 上线通知
#define   FriendMsg_FriendLogin                        10

struct  SMsgFriend_FriendLogin_SC  
{		
	dbid userPID;
	dbid friendPID;
	SMsgFriend_FriendLogin_SC()
	{
		memset(this, 0, sizeof(*this));
	}
};

/// 下线通知
#define   FriendMsg_FriendLogoff                       11
struct  SMsgFriend_FriendLogoff_SC  
{		
		dbid userPID;
		dbid friendPID;
		SMsgFriend_FriendLogoff_SC()
		{
			memset(this, 0, sizeof(*this));
		}
};

/// 邀请添加好友
#define   FriendMsg_ApplyAddFriend			         12

struct  SMsgFriend_AppAddFriend_SC  
{
	dbid   appUserID;                   /// 邀请者PID
	char   name[MAX_PERSONNAME_LEN + 1];	/// 邀请者名字
	ulong  level;						/// 邀请者等级
	int    nCreatureVocation;           /// 邀请者职业
	SMsgFriend_AppAddFriend_SC(void)
	{
		memset(this, 0, sizeof(*this));
	};
};    

/// 接受好友邀请
#define   FriendMsg_AcceptApply			             13
struct  SMsgFriend_AcceptApply_cs  
{
	dbid   acceptID;                   /// 接受邀请者PID
	dbid   inviteID;                   /// 邀请者PID 
	char   name[MAX_PERSONNAME_LEN + 1];	/// 邀请者名字
	SMsgFriend_AcceptApply_cs(void)
	{
		memset(this, 0, sizeof(*this));
	};
};    


#define  FriendMsg_AddFriend                        14
struct  SMsgFriend_AddFriend_sc  
{
	dbid userPID;
	FriendRecord friendRecode;        // 好友记录
	SMsgFriend_AddFriend_sc(void)
	{
		memset(this, 0, sizeof(*this));
	};
};    

// 好友查找
#define  FriendMsg_SearchFriend                      15

//客户端向社会服务器发送请求
struct  SMsgFriend_SearchFriend_cs  
{
	dbid   searcherID;                   /// 查询者PID
	dbid   personID;
	char szfriendName[MAX_PERSONNAME_LEN + 1];        
	SMsgFriend_SearchFriend_cs(void)
	{
		memset(this, 0, sizeof(*this));
	};
};  

// 社会服务器转发给场景服进行处理
struct  SMsgFriend_SearchFriend_ss  
{
	dbid   searcherID;                   /// 查询者PID
	char szfriendName[MAX_PERSONNAME_LEN + 1];        
	SMsgFriend_SearchFriend_ss(void)
	{
		memset(this, 0, sizeof(*this));
	};
};  

struct SMsgFriend_SearchRecord
{
	dbid personPID;                     //PID
	char szName[MAX_PERSONNAME_LEN + 1];    // 姓名    
	int  nLevel;						// 等级
	int  nSex;							// 性别
	int  nVocation;						// 职业
	int  nFaceID;                         // 头像

};

// 查询结果
struct  SMsgFriend_SearchFriend_sc 
{	
	int nCount;                         // 结果个数
	//...变长的记录
	SMsgFriend_SearchFriend_sc(void)
	{
		memset(this, 0, sizeof(*this));
	};
}; 

#define  FriendMsg_SearchFriendByMapID              16
struct  SMsgFriend_SearchFriendByMapID_ss  
{
	dbid   searcherID;                   /// 查询者PID
	DWORD   dwMapID;                   
	SMsgFriend_SearchFriendByMapID_ss(void)
	{
		memset(this, 0, sizeof(*this));
	};
}; 

// 创建好友分组
#define  FriendMsg_AddGroup                           17
struct  SMsgFriend_AddGroup_cs  
{
	dbid   userPID;								/// 创建者PID
	int   nGroupID;
	char   szGroupName[MAX_PERSONNAME_LEN + 1];     // 组名                
	SMsgFriend_AddGroup_cs(void)
	{
		memset(this, 0, sizeof(*this));
	};
}; 

struct  SMsgFriend_AddGroup_sc  
{
	int   nGroupID;								/// 
	char    szGroupName[MAX_PERSONNAME_LEN + 1];     // 组名                
	SMsgFriend_AddGroup_sc(void)
	{
		memset(this, 0, sizeof(*this));
	};
}; 

#define FriendMsg_ChangeGroupName                    18
struct SMsgFriend_ChangeGroupName_cs
{
	dbid   userPID;								    ///玩家PID 
	int  nGroupID;                                // 分组ID 
	char   szGroupName[MAX_PERSONNAME_LEN + 1];     // 组名                
	SMsgFriend_ChangeGroupName_cs(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define FriendMsg_MoveFriend						 19
struct SMsgFriend_MoveFriend_cs
{
	dbid   userPID;								    ///玩家PID 
	dbid   friendPID;								// 好友PID	
	int  nGroupID;                                // 分组ID 
	SMsgFriend_MoveFriend_cs(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

struct SMsgFriend_MoveFriend_sc
{
	dbid   userPID;
	dbid   friendPID;								// 好友PID	
	int  nGroupID;                                // 分组ID 
	SMsgFriend_MoveFriend_sc(void)
	{
		memset(this, 0, sizeof(*this));
	};
};
#define FriendMsg_RemoveGroup						 20
struct SMsgFriend__RemoveGroup_cs
{
	dbid userPID;
	int nGroupID;
	SMsgFriend__RemoveGroup_cs()
	{
		memset(this, 0 ,sizeof(SMsgFriend__RemoveGroup_cs));
	}
};

struct SMsgFriend__RemoveGroup_sc
{
	int nGroupID;
	SMsgFriend__RemoveGroup_sc()
	{
		memset(this, 0 ,sizeof(SMsgFriend__RemoveGroup_sc));
	}
};

#define FriendMsg_ViewDetail                          21
struct SMsgFriend__ViewDetail_cs
{
	dbid userPID;
	dbid playerPID;
	SMsgFriend__ViewDetail_cs()
	{
		memset(this, 0 ,sizeof(SMsgFriend__ViewDetail_cs));
	}
};

struct SMsgFriend__ViewDetail_sc
{
	dbid playerPID;
	char szPlayerName[MAX_PERSONNAME_LEN];
	int  nLevel;
	int  nVoctaion;
	int  nMapID;
	SMsgFriend__ViewDetail_sc()
	{
		memset(this, 0 ,sizeof(SMsgFriend__ViewDetail_sc));
	}
};

#define FriendMsg_Update                             22
struct SMsgFriend__Update_sc
{
	dbid userPID;
	FriendRecord friendRecode;        // 好友记录
	SMsgFriend__Update_sc(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define  FriendMsg_AddEnemy                         23
struct SMsgFriend__AddEnemy_sc
{
	FriendRecord friendRecode;        
	SMsgFriend__AddEnemy_sc(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define  FriendMsg_PlayerDie                         24
struct SMsgFriend__PlayerDie_ss
{
	dbid playerPID;   
	dbid MurdererPID;	// 凶手
	SMsgFriend__PlayerDie_ss(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define FriendMsg_AskAddEnemy						25
struct SMsgFriend__AskAddEnemy_sc
{
	dbid MurdererPID;	// 凶手
	SMsgFriend__AskAddEnemy_sc(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

struct SMsgFriend__AgreeAddEnemy_cs
{
	dbid userPID;
	dbid tagetPID;
	SMsgFriend__AgreeAddEnemy_cs(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define FriendMsg_RemoveEnemy						26
struct SMsgFriend__RemoveEnemy_cs
{
	dbid userPID;	// 
	dbid enemyPID;
	SMsgFriend__RemoveEnemy_cs(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

struct SMsgFriend__RemoveEnemy_sc
{
	dbid enemyPID;
	SMsgFriend__RemoveEnemy_sc(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define FriendMsg_Shield						27
struct SMsgFriend__Shield_cs
{
	dbid userPID;	// 
	dbid playerPID;
	bool bShield;
	ushort relation;
	SMsgFriend__Shield_cs(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define FriendMsg_TradeFinish						28
struct SMsgFriend__TradeFinish_ss
{
	dbid userPID;	// 
	dbid playerPID;
	SMsgFriend__TradeFinish_ss(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define  FriendMsg_EnterTeam                        29
struct SMsgFriend__TeamComm_ss
{
	dbid userPID;	// 
	DWORD  dwTeamID;
	SMsgFriend__TeamComm_ss(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

#define  FriendMsg_QuitTeam                        30

#define  FriendMsg_UpdateFriendPoint               31
struct SMsgFriend__UpdateFriendPoint_s
{
	dbid userPID;	// 
	dbid friendPID;
	ulong ulPoint;
	bool bBoth;
	SMsgFriend__UpdateFriendPoint_s(void)
	{
		memset(this, 0, sizeof(*this));
	};
};

/// 拒绝好友邀请
#define   FriendMsg_RefuseApply			             32
struct  SMsgFriend_RefuseApply_cs  
{
	dbid   refuseID;                   /// 拒绝邀请者PID
	dbid   inviteID;                   /// 邀请者PID 
	SMsgFriend_RefuseApply_cs(void)
	{
		memset(this, 0, sizeof(*this));
	};
};    


/// 查询当前地图仇人信息
#define   FriendMsg_UpdateMapEnemy			         33
struct  SMsgFriend_UpdateMapEnemy_cs  
{
	dbid   userPID;                  
	ulong  ulMapID;
	SMsgFriend_UpdateMapEnemy_cs(void)
	{
		memset(this, 0, sizeof(*this));
	};
};  

struct  SMsgFriend_UpdateMapEnemy_ss
{
		dbid   userPID;   
		ulong  ulMapID;
		int    nLen;
		//...
};

struct SEnemyMapPlace 
{
	char		szName[MAX_PERSONNAME_LEN];	// 名字
	POINT       ptTitle;                    // 位置
};
#endif // __FRIENDDEF_H__