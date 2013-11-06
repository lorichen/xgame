/*******************************************************************
** 文件名:	DArmy.h
** 版  权:	(C) 
** 
** 日  期:	2011/06/16  14:00
** 版  本:	1.0
** 描  述:	军团相关定义
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#ifndef __DARMY_H__
#define __DARMY_H__
#pragma  once

#include "UserInfoDef.h"


// 创建军团最少100金
#define ARMY_CREATE_MONEY_MIN        1000000

struct SArmyInfo
{
	dbid dbidLeader;									
	char szName[ARMY_NAME_LEN + 1];
	SArmyInfo()
	{
		memset(this, 0, sizeof(SArmyInfo));
	}
};

// 军团成员信息
struct SArmyMemberInfo
{
	dbid dbidMember;      // PID
	std::string strName;  // 名称
	int nProfession;      // 职业
	int nLevel;           // 等级
	dbid dbidFamilyLeader; //家族族长PID
	std::string strFamilyName;  // 家族名称
	bool bOnline;
	int nLastOfflineTime;
	SArmyMemberInfo()
	{
		dbidMember = 0;
		nProfession = 0;
		nLevel = 0;
		dbidFamilyLeader = 0;
		bOnline = false;
		nLastOfflineTime = 0;
	}
};

// 创建军团
#define ArmyMsg_Create           1 
struct SMsgArmy_Create_cs
{
	dbid dbidPerson;									
	char szArmyName[ARMY_NAME_LEN + 1];
	bool bSubmit;
	SMsgArmy_Create_cs()
	{
		memset(this, 0, sizeof(SMsgArmy_Create_cs));
	}
};

#define ArmyMsg_CreateResult    2
struct SMsgArmy_CreateResult_sc
{
	short result;
	char szArmyName[ARMY_NAME_LEN + 1];
};

#define ArmyMsg_UpdateArmyInfo 3
struct SMsgArmy_UpdateArmyInfo
{
	SArmyInfo armyInfo;
};

#define ArmyMsg_AddMember      4
struct SMsgArmy_AddMember
{
	SArmyMemberInfo memberInfo;
};

#endif //__DARMY_H__