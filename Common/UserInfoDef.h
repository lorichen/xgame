//==========================================================================
/**
* @file	  : UserInfoDef.h
* @author : pk <pk163@163.com>
* created : 2008-6-11   21:48
* purpose : 社会服务器用户信息定义
*/
//==========================================================================

#ifndef __USERINFODEF_H__
#define __USERINFODEF_H__

#include "DGlobalGame.h" // for MAX_PERSONNAME_LEN

/// 挂接在用户管理器上面的用户数据
enum UserDataType
{
	UserData_Chat,		/// 聊天系统数据
	UserData_Friend,	/// 好友系统数据
	UserData_Team,		/// 组队系统数据
	UserData_Nation,	/// 国家系统数据
	UserData_Tribe,		/// 氏族系统数据

	MaxUserDataCount,
};

///用户的属性数据
enum UerPropData
{
	User_Prop_Level = 1,      /// 用户的等级  
	User_Prop_Army ,          /// 用户的军团ID
	User_Prop_Family,         /// 用户的世家ID
	User_Prop_Team,           /// 用户的队伍ID
	User_Prop_Mapid,          /// 用户的地图ID
	User_Prop_Right,          /// 用户的权限

	User_Prop_MAX,            /// 最大
}; 
#pragma pack(push)
#pragma pack(1)

typedef ulong dbid;

const static int PERSON_SAVE_INTERVAL_DEAFULT = 3600000; //1000*60*60 60分钟 

/// 基本用户数据
struct UserInfo
{
	dbid		id;							/// 用户的DBID
	UID			uid;						/// 用户的UID
	ulong		clientId;					/// 客户端Id
	ulong		svrId;						/// 服务器Id

	char		name[MAX_PERSONNAME_LEN];	/// 用户的名字
	ulong		level;						/// 用户等级
	ulong       faceid;                     /// 用户头像id
	ulong		army;						/// 用户所在军团id
	ulong		family;						/// 用户所在世家id
	ulong		team;						/// 用户所在队伍id
	ulong       mapid;                      /// 用户所在地图id
	ulong		right;						/// 用户权限
	ulong       index;                      /// 用户所需要更新的属性索引
	int         nCreatureVocation;          /// 职业
	int			nSex;                       /// 性别
	int         nCamp;						/// 阵营

};

#pragma pack(pop)

#endif // __USERINFODEF_H__