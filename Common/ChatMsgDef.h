//==========================================================================
/**
* @file	  : ChatMsgDef.h
* @author : 
* created : 2008-3-7   18:50
* purpose : 聊天消息定义
*/
//==========================================================================

#ifndef __CHATMSGDEF_H__
#define __CHATMSGDEF_H__

/**
	聊天模块关心所有玩家的下列事件：

	登录/登出/迁移
	加入/退出国家
	加入/退出氏族
	加入/退出队伍
	升级消息
	权限改变消息
*/

#define MAX_CHAT_MSG_LENGTH		3072
#define MAX_CHAT_PAOPAO_DIS       18

/// 系统信息（含聊天信息）位置定义
enum InfoPos
{
	InfoPos_ChatWindow,				/// 聊天框(公共信息)
	InfoPos_FightChatWindow,		/// 聊天框(战斗信息)
	InfoPos_ScreenTopAD,			/// 屏幕置顶信息显示(置顶滚动信息)
	InfoPos_ScreenAboveTip,			/// 屏幕上方信息提示(比角色上方高的 信息显示)
	InfoPos_ScreenRightAboveTip_1,	 /// 屏幕右上方信息提示
	InfoPos_ScreenRightAboveTip_2,   /// 屏幕右上方信息提示2
	InfoPos_ActorAbove,		 /// 角色上方
	InfoPos_ActorUnder,		 /// 角色下方
	InfoPos_ActorRight,		 /// 角色右方
	InfoPos_ScreenRight,	 /// 屏幕右方
	InfoPos_MouseInfo,		 /// 鼠标跟踪信息
	MaxInfoPosCount,		/// 最大位置数
};

/// 聊天频道定义
enum ChatChannelType
{
	ChatChannel_Map = 1,	/// 地图频道
	ChatChannel_Around,		/// 周围频道
	ChatChannel_Army,		/// 军团频道
	ChatChannel_Family,		/// 世家频道
	ChatChannel_Team,		/// 队伍频道
	ChatChannel_Private,	/// 密语频道
	ChatChannel_Tips,		/// 系统提示频道，只发给自己的客户端
	ChatChannel_System,	    /// 系统频道
	ChatChannel_World,	    /// 世界频道

	MaxChatChannelCount,	/// 最大频道数
};

/// 聊天消息定义
enum
{
	//////////////////////////////////////////////////////////////////////////
	// 客户端 -> 聊天服务器(注：经网关服务器中转)
	//////////////////////////////////////////////////////////////////////////

	/// 发送聊天信息	上下文：(dbid sender, ulong channelType, [dbid target,] const char* chatText)
	/// 如果是私聊频道，需要填充target
	ChatMsg_Send,


	/// 修改聊天设置，如退订某频道消息 上下文：(dbid user, ulong channelType, bool val)
	ChatMsg_Config,




	//////////////////////////////////////////////////////////////////////////
	// 聊天服务器或场景服务器 -> 客户端(注：经网关服务器中转)
	//////////////////////////////////////////////////////////////////////////

	/// 聊天信息到达	上下文：(dbid sender, UID uid, ulong channelType, const char* name, const char* chatText)
	ChatMsg_Arrived,

	/// 聊天失败		上下文：(dbid uid, const char* failReason)
	ChatMsg_Failed,

	/// 系统提示信息到达(先放聊天模块来实现)	上下文：(ulong pos, const char* tips, ulong userData)
	ChatMsg_SystemInfo,

	/// 系统广播信息到达()
	ChatMsg_SystembroadcastInfo,

	/// 系统提示信息关闭操作(策划需求，服务端控制关闭某些特定位置的提示信息；)
	ChatMsg_CloseSystemInfo,

	//////////////////////////////////////////////////////////////////////////
	// 场景服务器 -> 聊天服务器(注：经中心服务器中转)
	//////////////////////////////////////////////////////////////////////////

	///	用户登录	上下文：(UseraInfo* info)
	ChatMsg_UserLogin,

	///	用户登出	上下文：(dbid user)
	ChatMsg_UserLogoff,

	///	用户切换场景登录	上下文：(UseraInfo* info)
	ChatMsg_UserChangeServerLogin,

	///	用户切换场景登出	上下文：(dbid user)
	ChatMsg_UserChangeServerLogoff,

	/// 用户信息改变	上下文：(UseraInfo* info) 主要包括：变更国家，变更氏族，加入脱离退伍，用户升级，权限变更，场景切换
	ChatMsg_InfoChanged,

	/// 系统向指定的用户列表广播信息，如队友捡到物品信息	上下文：(dbid user, ulong chatChannelType, ulong infoPos, const char* text, ulong userData)
	ChatMsg_Broadcast,


	/// 系统广播通知所有玩家；Boss刷新了！
	ChatMsg_NoticeBossRefresh,

	//////////////////////////////////////////////////////////////////////////
	// 聊天服务器 -> 场景服务器
	//////////////////////////////////////////////////////////////////////////

	/// 9宫格广播消息（适合普通频道）	上下文：(dbid id, const char* chatText)
	ChatMsg_Broadcast_9,

	/// 请求所有在线用户列表（如果聊天服务器后启动或重启需要） 上下文：无
	ChatMsg_OnlineUserInfo,

	/// 扣除点数（有的聊天需要收费，所以。。。）
	/// TODO 

	ChatMsg_Shield,
	ChatMsg_Shield_UserState,
	ChatMsg_Update_AmusementBoss, // 通知野外Boss
};


struct SMsgChat_Shield_cs
{
	ulong userPID;	 
	ulong playerPID;
	short bShield;
	char  szName[MAX_PERSONNAME_LEN];
	SMsgChat_Shield_cs()
	{
		memset(this, 0, sizeof(SMsgChat_Shield_cs));
	}
};

struct SMsgChat_Shield_sc
{
	ulong playerPID;
	short bShield;
};

struct SMsgChat_Shield_UserState_sc
{
	ulong playerPID;
	bool  bOnline;
};


struct SShieldRecord
{
	ulong playerPID;
	char  szName[MAX_PERSONNAME_LEN];		
	bool  Online;	
	SShieldRecord()
	{
		playerPID= 0;
		szName[0] = '\0';
		Online = false;
	}
};
#endif // __CHATMSGDEF_H__