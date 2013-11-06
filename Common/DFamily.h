/*******************************************************************
** 文件名:	DFamily.h
** 版  权:	(C) 
** 
** 日  期:	2011/02/17  14:00
** 版  本:	1.0
** 描  述:	摊位消息定义
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#ifndef __DFAMILY_H__
#define __DFAMILY_H__
#pragma  once

#include "UserInfoDef.h"

// 创建家族最低30级
#define FAMILY_CREATE_LEV_MIN          30 

// 创建家族最少1w金
#define FAMILY_CREATE_MONEY_MIN        100000000 

//家族邀请超时30s
#define FAMILY_INVITE_TIMEOUT	       30000 

struct SFamilyCreateContent
{
	dbid dbidCreate;									
	char szFamilyName[FAMILY_NAME_LEN + 1];
};

struct SFamilyInfo
{
	dbid dbidLeader;									
	int   nLevel;     //家族等级
	DWORD dwCamp;      //家族阵营
	char szFamilyName[FAMILY_NAME_LEN + 1];
	SFamilyInfo()
	{
		memset(this, 0, sizeof(SFamilyInfo));
	}
};

// 家族成员信息
struct SFamilyMemberInfo
{
	dbid dbidMember;      // PID
	std::string strName;  // 名称
	int nProfession;      // 职业
	int nLevel;           // 等级
	int nBattlePower;     // 战斗力
	bool bOnline;
	int nLastOfflineTime;
	SFamilyMemberInfo()
	{
		dbidMember = 0;
		nProfession = 0;
		nLevel = 0;
		nBattlePower = -1;
		bOnline = false;
		nLastOfflineTime = 0;
	}
};

// 创建家族
#define FamilyMsg_CreateFamily           1 
struct SMsgFamily_CreateFamily_cs
{
	dbid dbidCreate;									
	char szFamilyName[FAMILY_NAME_LEN + 1];
};

// 更新家族信息
#define FamilyMsg_UpdateFamily           2 
struct SMsgFamily_UpdateFamily_sc
{
	SFamilyInfo familyInfo;
};

#define FamilyMsg_AddMember				3 
struct SMsgFamily_AddMember_sc
{
	dbid dbidLeader;      // PID
	dbid dbidMember;      // PID
	char szName[MAX_PERSONNAME_LEN];  // 名称
	int nProfession;      // 职业
	int nLevel;           // 等级
	int nBattlePower;     // 战斗力
	int nLastOfflineTime;
	uchar bOnline;
};

#define FamilyMsg_Result				4
enum EFamilyResult
{
	EFamilyResult_Create  = 1,
};
struct SMsgFamily_Result_ss
{
	int nType;
	int nResult; // 1成功, 0失败
	DWORD dwParam;
};

#define FamilyMsg_Disband				5
struct SMsgFamily_Disband_ss
{
	dbid dbidLeader;      // PID
};

#define FamilyMsg_RecruitMember			6
struct SMsgFamily_RecruitMember_cs
{
	dbid dbidLeader;      // PID
	char szName[MAX_PERSONNAME_LEN];  // 名称
};

#define FamilyMsg_InviteJoin			7
struct SMsgFamily_InviteJoin_sc
{
	dbid dbidInviter;      // PID
	char szInviterName[MAX_PERSONNAME_LEN];  // 族长名称
	char szFamilyName[FAMILY_NAME_LEN + 1];  // 家族名称
};

#define FamilyMsg_ReplyInvite			8
struct SMsgFamily_ReplyInvite_cs
{
	dbid dbidPeroson;      // PID
	dbid dbidInviter;
	int  nResult;          // 1同意，0拒绝
	int  nBattlePower;
};

#define FamilyMsg_UpdateMember			9

#define FamilyMsg_QuitFamily			10
struct SMsgFamily_QuitFamily_cs
{
	dbid dbidLeader;
	dbid dbidPerson; 
	int  nFireOut;  // 开除
};

#define FamilyMsg_Demise				11
struct SMsgFamily_Demise_cs
{
	dbid dbidLeader;
	dbid dbidPerson; 
};

#define FamilyMsg_SetOnline				12
struct SMsgFamily_SetOnline_cs
{
	dbid dbidLeader;
	dbid dbidPerson; 
	int  nLastOffLineTime;
	uchar bOnline;
};

#define FamilyMsg_UpdateBattlePower		13
struct SMsgFamily_UpdateBattlePower_ss
{
	dbid dbidLeader;
	dbid dbidPerson; 
	int  nBattlePower;
};

#define FamilyMsg_QueryBattlePower		14
struct SMsgFamily_QueryBattlePower_ss
{
	dbid dbidLeader;
	dbid dbidPerson; 
};

#define FamilyMsg_TimeStamp				15
struct SMsgFamily_TimeStamp_sc
{
	ulong uTS; //32位无符号数可以保存136年的时间间隔(即到2106年)
};

#define FamilyMsg_JoinArmy				16
struct SMsgFamily_JoinArmy_ss
{
	dbid dbidFamilyLeader;
	dbid dbidArmyLeader;
};
#endif //__DFAMILY_H__