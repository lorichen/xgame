/*******************************************************************
** 文件名:	DStallMessage.h
** 版  权:	(C) 
** 
** 日  期:	2010/10/19  11:35
** 版  本:	1.0
** 描  述:	摊位消息定义
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#ifndef __DSTALL_MESSAGE_H__
#define __DSTALL_MESSAGE_H__
#pragma  once

#include "UserInfoDef.h"
#include <string>

const static dbid   STALL_DEFAULT_OWNERID = 0; 
const static std::string STALL_DEFAULT_OWNERNAME = "大宋行会"; 

const static int    STALL_DEFAULT_OriginalVendibility = 40 * 10000; // 40 金
const static int    STALL_DEFAULT_RENT = 5 * 100; // 5银

const static DWORD  STALL_FOREGIFT_MAX = MAX_PERSON_MONEY; //押金上限

const static DWORD  STALL_GET_RENT_INTERVAL = 3600000; //1h
const static DWORD  STALL_GET_REMOTETAX_INTERVAL = 3600000; //1h
const static int    STALL_REMOTE_TAX = 50000;//远程摊位管理费  5 gold/h

const static int PERSON_STALL_MAX_NUM = 48; // 玩家摊位最大容量
const static int PERSON_STALL_SELL_MAX_NUM = 24; // 玩家摊位出售位最大容量

#define STALL_CRYOUT_MSG_LEN					41 // 吆喝内容最大长度


const static DWORD  STALL_CRYOUT_TIME = 10000; //吆喝持续10s

//摊位状态
enum EStallStatus
{
	StallStatus_Primary		 = 0, //初级
	StallStatus_Intermediate = 1, //中级
	StallStatus_Advanced	 = 2, //高级
};

//价格类型
enum EStallPriceType
{
	StallPriceType_Money		 = 0, //金钱
	StallPriceType_GoldIngot	 = 1, //元宝
};

enum EViewRemoteStallType
{
	ViewRemoteStallType_Special		 = 1, //查看指定的远程摊位
	ViewRemoteStallType_Pre			 = 2, //查看上一个摊位
	ViewRemoteStallType_Next	     = 3, //查看下一个摊位
	ViewRemoteStallType_First	     = 4, //查看第一个摊位
};

struct SGoodsPrice
{
	DWORD dwPrice;
	ushort usPriceType;
	int nNum;
	SGoodsPrice()
	{
		dwPrice = 0;
		usPriceType = 0;
		nNum = 0;
	}
};


enum EStallTradeType
{
	StallTradeType_Sell = 1, //出售
	StallTradeType_Buy  = 2, //收购
};

struct STrade
{
	struct tm tradeTime;
	int nGoodsID;
	DWORD dwPrice;
	ushort usPriceType;
	ushort usType;// 1 出售 2 收购
	BYTE   bRemote;
	STrade()
	{
		bRemote = 0;
	}
};

struct SIncome
{
	ulong ulSellMoney;
	ulong ulSellGoldIngot;

	ulong ulBuyMoney;
	ulong ulBuyGoldIngot;
	SIncome()
	{
		memset(this, 0, sizeof(SIncome));
	}
};

struct SLeaveWord
{
	dbid dbidPerson;
	char szName[MAX_PERSONNAME_LEN];
	char szLeaveWord[STALL_LEAVEWORD_MSG_LEN];
};

enum EStallOperate
{
	StallOperate_Person = 1, //个人
	StallOperate_Corps  = 2, //军团
};

enum EStallProp
{
	StallProp_Rent		 =  1, //租金
	StallProp_Foregift   =  2, //押金
	StallProp_Status     =  3, //摊位状态
	StallProp_Income     =  4, //收益
};
struct StallInfo
{
	UID  uidStallMaster; // 对应摊位NPC的UID
	int  nStallID;    // 摊位ID，编号

	dbid dbidOwner;    // 摊主
	dbid dbidRenter;   // 租赁者（摆摊者）
	char szOwnerName[MAX_PERSONNAME_LEN];  // 摊主姓名
	char szRenterName[MAX_PERSONNAME_LEN]; // 租赁者姓名

	ushort uStallStatus; // 摊位状态

	int  nRent;       // 当前租金
	DWORD  dwIncome;     // 已获取收益(摆摊人上交的租金)
	
	// 市值 = 原始价值 + 押金 + 手续费
	int  nOriginalVendibility; // 原始价值
	DWORD dwForegift;            // 押金
	int  nPoundage;            // 手续费
	StallInfo()
	{
		memset(this, 0, sizeof(StallInfo));
		strcpy_s(szOwnerName, 16, STALL_DEFAULT_OWNERNAME.c_str());
		uStallStatus = StallStatus_Primary;
		dbidOwner = STALL_DEFAULT_OWNERID;
		nOriginalVendibility = STALL_DEFAULT_OriginalVendibility;
		nRent = STALL_DEFAULT_RENT;
	}
};

#define  StallMsg_OpenStall     1
struct SMsgStall_StallInfo_sc
{
	StallInfo stallInfo;
	uchar   uEndStall;
	SMsgStall_StallInfo_sc()
	{
		memset(this, 0, sizeof(SMsgStall_StallInfo_sc));
	}
};

#define  StallMsg_BuyStall			2
struct SMsgStall_StallOperate_cs
{
	DWORD dwZoneID;
	int nStallID;
	int nType;    // 1 个人，  2 军团
	int nRent;
	SMsgStall_StallOperate_cs()
	{
		dwZoneID = 0;
		nStallID = 0;
	}
};

#define  StallMsg_UpdateStallInfo   3

#define  StallMsg_KickOutRenter     4
struct SMsgStall_KickOut_sc
{
	ushort usResult;   //0 失败， 1成功
	SMsgStall_KickOut_sc()
	{
		usResult = 0;
	}
};

#define  StallMsg_ChangeRent        5

#define  StallMsg_UpdateProp        6
struct SMsgStall_UpdateProp_sc
{
	int nStallID;
	int nType;
	int nValue;
	DWORD dwValue;
	SMsgStall_UpdateProp_sc()
	{
		memset(this, 0, sizeof(SMsgStall_UpdateProp_sc));
	}
};

#define  StallMsg_ChangeForegift    7
struct SMsgStall_ChangeForegift_cs
{
	int nStallID;
	DWORD dwValue;
	BYTE bInc;  
	SMsgStall_ChangeForegift_cs()
	{
		memset(this, 0, sizeof(SMsgStall_ChangeForegift_cs));
	}
};

#define  StallMsg_StartStall        8
struct SMsgStall_StartStall_cs
{
	int nStallID;
	int nTime;
	ushort usRemote;
	ushort usDiffZoneServer; 
	SMsgStall_StartStall_cs()
	{
		memset(this, 0, sizeof(SMsgStall_StartStall_cs));
	}
};

//出售物品上架
#define  StallMsg_AddSellGoods        9
struct StallMsg_AddSellGoods_cs
{
	int nStallID;
	ulong ulSrcSkepID;
	int nSrcPlace;
	DWORD dwPrice;
	ushort usPriceType;
	int nNum;
	StallMsg_AddSellGoods_cs()
	{
		memset(this, 0, sizeof(StallMsg_AddSellGoods_cs));
	}
};

struct StallMsg_AddSellGoods_sc
{
	int nStallID;
	LONGLONG uGOODSID;
	int nPlace;
	DWORD dwPrice;
	ushort usPriceType;
	ulong ulSrcSkepID;
	int nSrcPlace;
	int nNum;
	StallMsg_AddSellGoods_sc()
	{
		memset(this, 0, sizeof(StallMsg_AddSellGoods_sc));
	}
};

#define  StallMsg_RemoveSellGoods        10
struct StallMsg_RemoveSellGoods_cs
{
	int nStallID;
	ulong ulSrcSkepID;
	int nSrcPlace;
	StallMsg_RemoveSellGoods_cs()
	{
		memset(this, 0, sizeof(StallMsg_RemoveSellGoods_cs));
	}
};

struct StallMsg_RemoveSellGoods_sc
{
	int nStallID;
	ulong ulSrcSkepID;
	int nSrcPlace;
	StallMsg_RemoveSellGoods_sc()
	{
		memset(this, 0, sizeof(StallMsg_RemoveSellGoods_sc));
	}
};

#define  StallMsg_AddBuyGoods			  11
struct StallMsg_AddBuyGoods_cs
{
	int nStallID;
	int nGoodsID;
	int nPlace;
	DWORD dwPrice;
	ushort usPriceType;
	int nNum;
	StallMsg_AddBuyGoods_cs()
	{
		memset(this, 0, sizeof(StallMsg_AddBuyGoods_cs));
	}
};

#define  StallMsg_RemoveBuyGoods			  12
struct StallMsg_RemoveBuyGoods_cs
{
	int nStallID;
	int nGoodsID;
	int nPlace;
	int nTargetPlace;
	StallMsg_RemoveBuyGoods_cs()
	{
		nStallID = 0;
		nGoodsID = 0;
		nPlace = -1;
		nTargetPlace = -1;
	}
};

#define  StallMsg_SubmitBuyGoods			  13
struct StallMsg_SubmitBuyGoods_cs
{
	int nStallID;
	StallMsg_SubmitBuyGoods_cs()
	{
		memset(this, 0, sizeof(StallMsg_SubmitBuyGoods_cs));
	}
};

#define  StallMsg_UndoBuyGoods				  14
struct StallMsg_UndoBuyGoods_cs
{
	int nStallID;
	ulong ulSrcSkepID;
	StallMsg_UndoBuyGoods_cs()
	{
		memset(this, 0, sizeof(StallMsg_UndoBuyGoods_cs));
	}
};

#define  StallMsg_UpdateTotalPrice			 15
struct StallMsg_UpdateTotalPrice_cs
{
	int nStallID;
	uint64 u64TotalMoney;
	uint64 u64TotalGoldIngot;
	StallMsg_UpdateTotalPrice_cs()
	{
		memset(this, 0, sizeof(StallMsg_UpdateTotalPrice_cs));
	}
};

#define  StallMsg_EndStall				  16
struct StallMsg_EndStall_cs
{
	int nStallID;
	StallMsg_EndStall_cs()
	{
		memset(this, 0, sizeof(StallMsg_EndStall_cs));
	}
};


#define  StallMsg_BuyStallGoods				  17
struct StallMsg_BuyStallGoods_cs
{
	int nStallID;
	ulong ulSrcSkepID;
	int nSrcPlace;
	int nNum;
	LONGLONG uID;
	int nTargetPlace;
	StallMsg_BuyStallGoods_cs()
	{
		memset(this, 0, sizeof(StallMsg_BuyStallGoods_cs));
		nTargetPlace = -1;
	}
};

#define  StallMsg_SellGoodsToStall				18
struct StallMsg_SellGoodsToStall_cs
{
	int nStallID;
	ulong ulSrcSkepID;
	int nSrcPlace;
	int nNum;
	StallMsg_SellGoodsToStall_cs()
	{
		memset(this, 0, sizeof(StallMsg_SellGoodsToStall_cs));
	}
};

#define  StallMsg_UpLevel						19
struct StallMsg_UpLevel_cs
{
	int nStallID;
	StallMsg_UpLevel_cs()
	{
		memset(this, 0, sizeof(StallMsg_UpLevel_cs));
	}
};

#define  StallMsg_GiveUp						20
struct StallMsg_GiveUp_cs
{
	int nStallID;
	StallMsg_GiveUp_cs()
	{
		memset(this, 0, sizeof(StallMsg_GiveUp_cs));
	}
};

#define  StallMsg_RecycleMoney					21
struct StallMsg_RecycleMoney_cs
{
	int nStallID;
	StallMsg_RecycleMoney_cs()
	{
		memset(this, 0, sizeof(StallMsg_RecycleMoney_cs));
	}
};

#define  StallMsg_UpdateRecycleMoney           22
struct StallMsg_UpdateRecycleMoney_sc
{
	int nStallID;
	dbid dbidPerson;
	uint64 u64RecycleMoney;
	StallMsg_UpdateRecycleMoney_sc()
	{
		memset(this, 0, sizeof(StallMsg_UpdateRecycleMoney_sc));
	}
};

#define  StallMsg_DrawIncome					23
struct StallMsg_DrawIncome_cs
{
	int nStallID;
	StallMsg_DrawIncome_cs()
	{
		memset(this, 0, sizeof(StallMsg_DrawIncome_cs));
	}
};

#define  StallMsg_CryOut						24
struct StallMsg_CryOut_cs
{
	int nStallID;
	char szCryOutMsg[STALL_CRYOUT_MSG_LEN];
	StallMsg_CryOut_cs(void)
	{
		memset(this, 0, sizeof(StallMsg_CryOut_cs));
	}
};

#define  StallMsg_LeaveWord						25
struct StallMsg_LeaveWord_cs
{
	int nStallID;
	char szMsg[STALL_LEAVEWORD_MSG_LEN];
	StallMsg_LeaveWord_cs(void)
	{
		memset(this, 0, sizeof(StallMsg_LeaveWord_cs));
	}
};

struct StallMsg_LeaveWord_sc
{
	int nStallID;
	ushort usNum;              //SLeaveWord 数目
	//... SLeaveWord 列表
	//SLeaveWord leaveWord;
};

#define  StallMsg_ClearLeaveWord						26
struct StallMsg_ClearLeaveWord_cs
{
	int nStallID;
};

struct StallMsg_ClearLeaveWord_sc
{
	int nStallID;
};

#define  StallMsg_RequestLeaveWord						27
struct StallMsg_RequestLeaveWord_cs
{
	int nStallID;
};

#define  StallMsg_Trade									28
struct StallMsg_Trade_sc
{
	int nStallID;
	STrade trade;
};

#define  StallMsg_RequestIncomeInfo						29
struct StallMsg_RequestIncomeInfo_cs
{
	int nStallID;
};

#define StallMsg_IncomeInfo                             30
struct StallMsg_IncomeInfo_sc
{
	int nStallID;
	DWORD dwRent;
};

#define StallMsg_RemoteStall                            31
struct StallMsg_RemoteStall_cs
{
	int nStallID;
	ushort usHour;
};

struct StallMsg_RemoteStall_sc
{
	int nStallID;
	ulong dbidRenter;
	ushort usHour;
	ushort usStart; // 1 开始 0 追加 
};

#define StallMsg_RegistRemoteStall                        32
struct StallMsg_RegistRemoteStall_ss
{
	int nStallID;
	ulong dbidRenter;
};

#define StallMsg_ViewRemoteStall						  33
struct StallMsg_ViewRemoteStall_cs
{
	int nStallID;  // 
	ushort usType;// 
};

struct StallMsg_ViewRemoteStall_ss
{
	int nStallID;  // 
	ulong ulViewerSvrID;
	ulong dbidViewer;
};

#define StallMsg_CreateStallMsg 						  39
struct StallMsg_CreateRemoteStall
{
	ulong dbidViewer;
	ulong ulSrvID;
	StallInfo stallInfo;

	//...     DBREQ_TABLE_GOODS 摊位的物品数据
};

#define StallMsg_RemoteBuyGoods							 40
struct StallMsg_RemoteBuyGoods_cs
{
	int nStallID;
	int nSrcPlace;
	int nNum;
	int nTargetPlace;
	StallMsg_RemoteBuyGoods_cs()
	{
		memset(this, 0, sizeof(StallMsg_RemoteBuyGoods_cs));
		nTargetPlace = -1;
	}
};

#define StallMsg_OperateStall							 40
struct StallMsg_OperateStall_sc
{
	int nStallID;
	ushort usType; 
	StallMsg_OperateStall_sc()
	{
		memset(this, 0, sizeof(StallMsg_OperateStall_sc));
	}
};

#define  StallMsg_UpdateSellGoodsNum					  41
struct StallMsg_UpdateSellGoodsNum_sc
{
	int nPlace;
	int nNum; 
	DWORD dwPrice;
	short usPriceType;
	StallMsg_UpdateSellGoodsNum_sc()
	{
		memset(this, 0, sizeof(StallMsg_UpdateSellGoodsNum_sc));
	}
};

#define  StallMsg_SellGoodsEmpty					  42
struct StallMsg_SellGoodsEmpty_sc
{
	int nStallID;
	ulong ulSrcSkepID;
	int nSrcPlace;
	StallMsg_SellGoodsEmpty_sc()
	{
		memset(this, 0, sizeof(StallMsg_SellGoodsEmpty_sc));
	}
};

#define  StallMsg_CloseStall						43
struct StallMsg_CloseStall_sc
{
	int nStallID;
	StallMsg_CloseStall_sc()
	{
		memset(this, 0, sizeof(StallMsg_CloseStall_sc));
	}
};
#endif//__DSTALL_MESSAGE_H__