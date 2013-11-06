/*******************************************************************
** 文件名:	e:\Rocket\Server\SkepServer\Include\DSkepServer.h
** 版  权:	(C) 
** 
** 日  期:	2008/3/3  22:28
** 版  本:	1.0
** 描  述:	通用虚拟物品篮定义
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "DGlobalGame.h"
#include "DGlobalMessage.h"

#pragma pack(1)

// 创建通用物品篮现场
#define SKEPNAME_MAX_LEN	32
struct SBuildSkepClientContext
{
	DWORD		dwSkepID;						// 物品篮ID
	DWORD		dwSkepType;						// 物品篮类型
	DWORD		dwSkepName;						// 物品篮名字
	DWORD		dwMaxSize;						// 物品篮大小
	DWORD		dwPopedom;						// 物品篮权限
	DWORD		dwUsableBags;					// 开启行囊数--仓库,背包		
	char		szSkepName[SKEPNAME_MAX_LEN];	// 物品篮名字

	// .....									// 上层自已扩展数据

	SBuildSkepClientContext(void)
	{
		memset(this, 0, sizeof(SBuildSkepClientContext));
	}
};

/******************************************************************/
///////////////////// 场景服的行为层的消息码////////////////////////
// 上层的消息码必定是MSG_MODULEID_SKEP
/******************************************************************/
// 场景服与客户端之间消息，必须继承此消息头
struct SMsgSkepCSCHead
{
	DWORD		dwSkepID;			// 物品篮ID
};

///////////////////////////////////////////////////////////////////
// 链接物品（链接，叠加）
#define	MSG_SKEP_LINK												1
// 消息体
struct SMsgSkepLink_CS
{
	DWORD		dwSrcSkepID;		// 源物品篮ID
	DWORD		dwSrcPlace;			// 源位置

	LONGLONG	uidGoods;			// 物品UID
	int			nNewPlace;			// 新位置（如果为-1，则自动找位置）	

	SMsgSkepLink_CS(void)
	{
		memset(this, 0, sizeof(SMsgSkepLink_CS));
	};
};

///////////////////////////////////////////////////////////////////
// 拆分物品
#define MSG_SKEP_SPLIT												2
// 消息体
struct SMsgSkepSplit_CS
{
	DWORD		dwPlace;			// 位置
	LONGLONG	uidGoods;			// 物品UID

	int			nSplitNum;			// 拆分出来的数量

	SMsgSkepSplit_CS(void)
	{
		memset(this, 0, sizeof(SMsgSkepSplit_CS));
	}
};

///////////////////////////////////////////////////////////////////
// 使用物品
#define MSG_SKEP_USE												3
// 消息体
struct SMsgSkepUse_CS
{
	DWORD		dwPlace;			// 位置
	LONGLONG	uidGoods;			// 物品UID
    LONGLONG    uidUser;            // 使用者UID
	SMsgSkepUse_CS(void)
	{
		memset(this, 0, sizeof(SMsgSkepUse_CS));
	}
};

///////////////////////////////////////////////////////////////////
// 观察者
#define MSG_SKEP_OBSERVER											4
// 消息体
struct SMsgSkepObserver_CS
{
	bool		bOpen;				// true：观察 flase：取消观察

	SMsgSkepObserver_CS(void)
	{
		memset(this, 0, sizeof(SMsgSkepObserver_CS));
	}
};

// 消息体
struct SMsgSkepObserver_SC
{
	bool		bSuccess;			// true：观察成功 flase：观察失败

	SMsgSkepObserver_SC(void)
	{
		memset(this, 0, sizeof(SMsgSkepObserver_CS));
	}
};


///////////////////////////////////////////////////////////////////
// 同步客户端
#define MSG_SKEP_SYNC												5
// 消息体
struct SMsgSkepSync_SC
{
	struct SMsgSyncSkepGoods
	{
		DWORD	dwPlace;			// 位置
		LONGLONG uidGoods;			// 物品UID（为无效UID，表示删除此位置）
		DWORD   dwRelaySkepID;		// 交互的物品篮ID
		bool	bLink;				// 是否链接
	};

	// ................				// 多个SMsgSyncSkepGoods
};

///////////////////////////////////////////////////////////////////
// 物品篮修改大小
#define MSG_SKEP_CHANGESIZE											6
// 消息体
struct SMsgSkepChangeSize_SC
{
	int			nMaxSize;			// 物品栏大小
};

///////////////////////////////////////////////////////////////////
// 同步物品篮权限
#define MSG_SKEP_SYNCPOPEDOM										7
// 消息体
struct SMsgSkepSyncPopedom_SC
{
	DWORD		dwPopedom;			// 新权限
};

///////////////////////////////////////////////////////////////////
// 整理物品篮
#define MSG_SKEP_REALIGNMENT										8
// 消息体
struct SMsgSkepReAlignment_SC
{
	DWORD		dwPopedom;			// 待定 lilinll77
};

///////////////////////////////////////////////////////////////////
//物品栏中子物品栏的显示和隐藏 用于装备的附魂，宝石镶嵌，和重铸
#define MSG_SKEP_SETCURSUBSKEP                                      9
//消息体
struct SMsgSkepSetCurSubskep_SC
{
	ushort ucSubskepName; //子物品栏名字
	bool bShow;//显示或者隐藏
	//一个消息里面能够包含多个这样的消息结构
};
///////////////////////////////////////////////////////////////////
//丢弃物品(宠物)
#define MSG_SKEP_DROPOUT											10
//消息体
struct SMsgSkepDropOut_CS
{
	DWORD		dwPlace;			// 位置
	LONGLONG	uidGoods;			// 物品UID
	bool		bDestroy;			// by cjl是否直接销毁

	SMsgSkepDropOut_CS(void)
	{
		memset(this,0,sizeof(SMsgSkepDropOut_CS));
	};
};

// 使用复活卷轴
#define  MSG_SKEP_USE_RELIVE_BOOK									11

///////////////////////////////////////////////////////////////////
//开启行囊
#define MSG_SKEP_OPENBAG		                                    12
//消息体
struct SMsgSkepOpenBag_CS
{
	DWORD		dwPlace;			// 位置
};
struct SMsgSkepOpenBag_SC
{
	DWORD		dwPlace;			// 位置
};

// 复活卷轴的在配置表中的物品属性ID
#define  RELIVE_BOOK_GOOD_ID								4000

#pragma pack()