/*******************************************************************
** 文件名:	e:\CoGame\TaskServer\src\TaskServer\DTaskServer.h
** 版  权:	(C) 
** 
** 日  期:	2007/8/24  16:16
** 版  本:	1.0
** 描  述:	任务服务器定义
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

/***************************************************************/
///////////////////// 场景服的行为层的消息码/////////////////////
// 上层的消息码必定是MSG_MODULEID_TASK
/***************************************************************/
enum 
{
	TASK_MEET_NPC = 0,  // NPC
	TASK_USE_ITEM = 1,  // 使用物品触发
	TASK_EXPLORD  = 2,  // 探索触发
	TASK_SHARE = 3,     // 任务共享面板
};
// 显示任务信息
#define MSG_TASK_SHOWINFO										1
struct SMsgTaskShowInfo_SC
{
	//int			nBodyLen;			// 消息内容长度
	// .......................		// 消息内容
	//  任务ID；
	WORD m_wTaskID;
    //  相关NPC；
    int m_nNPCID;
	//  对话信息索引；
	uint m_unDialogIndex;
    //  对话模式；
    uchar m_ucMode;
	// 对话框打开方式
	uchar m_ucOpenType;
};

// 客户端响应
#define MSG_TASK_RESPOND										2
struct SMsgTaskRespond_CS
{
	char		szLuaFunName[128];	// lua函数名
	int			nParamLen;			// 参数内容字节数
	int			nContextLen;		// 上下文内容字节数
	// ...................			// 参数内容: ID'#'值'#' + ID'#'值'#'
	// ..................			// 上下文内容: ID'#'值'#' + ID'#'值'#'
};

// 关闭客户端任务信息窗口
#define MSG_TASK_CLOSEINFOWND									3
struct SMsgTaskCloseInfoWnd_SC
{

};

// 客户端登录后，请求的任务日志
#define MSG_TASK_LOGIN											4
struct SMsgTaskLoginInfo_CS
{
	
};

// 增加一条任务日志
#define MSG_TASK_ADDRECORD										5
struct SMsgTaskAddRecord_SC
{
	DWORD		dwTaskID;			// 任务ID
	DWORD       dwTraceIndex;       // 跟踪索引值
	// ..................			// 日志内容
};

// 移除一条任务日志
#define MSG_TASK_REMOVERECORD									6
struct SMsgTaskRemoveRecord
{
	DWORD		dwTaskID;			// 任务ID
};
struct SMsgTaskRemoveRecord_SC
{
	DWORD		dwTaskID;			// 任务ID
};

// 移除所有任务日志
#define MSG_TASK_REMOVEALLRECORD								7
struct SMsgTaskRemoveAllRecord_SC
{
};

// 执行一段脚本
#define MSG_TASK_RUNSCRIPT										8

//  显示对话信息；
#define MSG_DIALOG_SHOWINFO										9
struct SMsgDialogShowInfo_SC
{
	int			nBodyLen;			// 消息内容长度
	// .......................		// 消息内容
};

//  客户端接受任务的请求；
#define MSG_TASK_ACCEPT_REQUEST 10
struct SMsgTaskAcceptRequest_CS
{
	WORD m_wTaskID;
};

//  更新一条任务日志；
#define MSG_TASK_UPDATERECORD 11
struct SMsgTaskUpdateRecord_SC
{
    //  任务ID；
    DWORD dwTaskID;
    //  数据索引；
    uchar ucIndex;
    //  数据内容；
    ushort usValue;
};

//  客户端接受任务的请求；
#define MSG_TASK_REPORT_REQUEST 12
struct SMsgTaskReportRequest_CS
{
    WORD m_wTaskID;
    int m_nTaskParam;
};

//  服务端发给客户端的完成任务的响应；
#define MSG_TASK_ACCOMPLISH 13
struct SMsgTaskAccomplish_SC
{
    WORD m_wTaskID;
};

//  服务端发给客户端的已接任务的信息；
#define  MSG_TASK_INITIAL_HOLD_TASK 14
struct SMsgTaskInitialHoldTask_SC
{
    int m_nTaskDataLength; 
};

//  服务端发给客户端的已完成任务的信息；
#define MSG_TASK_INITIAL_ACCOMPLISHED_TASK 15
struct SMsgTaskInitialAccomplishedTask_SC
{
    int m_nTaskDataLength; 
};

// 通用窗口的消息
#define  MSG_TASK_NOTIFY_CREATE_COMMWND 16

// 返回通用窗口的结果
#define  MSG_TASK_CHOSEN_RESULT  17

// 探索任务触发的消息框
#define  MSG_TASK_NOTIFY_EXPLORD_CREATE 18

// 返回探索任务选择的结果
#define MSG_TASK_EXPLORD_CHOSEN_RESULT  19

// 发送共享任务消息
#define MSG_TASK_SHARE    20

// 发送接受共享任务消息  
#define MSG_TASK_SHARE_ACCEPT 21

// 发送创建通用的，物品，探索，共享，任务的描述的信息窗口
#define MSG_TASK_CREATE_COM_DSC_WND 22

// 自动寻路完成后；自动弹出对话框消息
#define  MSG_TASK_CREATE_TASKDIALOGBOX 23
struct  SMsgTaskCreateTaskDialogBox_SC
{
	char		szNPCName[128];	// 名字
	DWORD       dwTaskID;
};

// 更新任务跟踪索引下标
#define MSG_TASK_UPDATE_TASKTRACEINDEX 24
struct  SMsgTaskUpdateTaskTraceIndex_SC
{
	int      nTaskID;
	int      nIndex;
	bool     bAutoTrace;
};

//通用输入对话框
#define  MSG_DIALOG_COMMINPUT                               25
struct SMsgSystemCommInput_SC
{
	char  szContent[128];        // 对话框输入提示信息
	char  szCallBackFunc[64];    // 回调
	ushort usInput;              // 是否需要玩家输入
	int   nTime;

	SMsgSystemCommInput_SC(void)
	{
		memset(this, 0, sizeof(SMsgSystemCommInput_SC));
	}
};

struct SMsgSystemCommInput_CS
{
	char  szContent[64];         // 玩家的输入, 回调时作为参数
	char  szCallBackFunc[64];    // 回调
	ushort usInput;              // 是否有玩家输入
	ushort usResult;
	SMsgSystemCommInput_CS(void)
	{
		memset(this, 0, sizeof(SMsgSystemCommInput_CS));
	}
};
// 删除已完成任务数据
#define  MSG_TASK_COMPELETE_DELETE          26
struct SMsgTaskCompeleteDelete_SC
{
	 WORD  wTaskID;
};

// 飞行任务弹对话框
#define  MSG_TASK_FLYRIDE_CREATEDIALOGBOX     27
struct SMsgTaskFlyRideCreateDialogBox_CS
{
	WORD  wTaskID;
	WORD  wNpcID;
	WORD  wSpeed;
};

//by cjl
// 向服务器获取物品
#define MSG_TASK_GET_PRESENT                   28
struct SMsgTaskPresent_CS
{
	int nPresentLevel;//用来验证的
};
//end cjl

// 向客户端发送需要显示的任务图片
#define MSG_TASK_SEND_IMAGEINFO                 29
struct SMsgTaskImageInfo_SC
{
	char  szImageName[64];       // 图片名字；
	SMsgTaskImageInfo_SC()
	{
		memset(this, 0 ,sizeof(SMsgTaskImageInfo_SC));
	}
};

// 目前还在利用该消息的结构；不删除改消息的 定义；WZH
// 切换场景服务器，已接任务数据发送信息
//  服务端发给客户端的已接任务的信息；
#define  MSG_TASK_HOLD_TASK_TOSERVER             30
struct SMsgTaskInitialHoldTask_SS
{
	DWORD dwHolder;
	int   nTaskCount;
};  

// 切换场景服务器，已接任务数据发送信息
//  服务端发给客户端的已完成任务的信息；
#define  MSG_TASK_FINISHED_TASK_TOSERVER         31
struct SMsgTaskInitialFinishedTask_SS
{
	DWORD  dwHoldPDID;
	DWORD  dwTaskCounts;
};  

// 定时器任务到时间；通知服务器
#define  MSG_TASK_OTIMER_RESPOND				 32
struct SMsgTaskInitialOnTimerRespondTask_CS
{
	DWORD  dwTaskID;
};

// 关闭定任务定时器计时；
#define MSG_TASK_CLOSE_CLIENT_COUNTDOWN			  33
struct SMsgTaskCloseClientCountDown_SC
{
	DWORD  dwTaskID;
};

// 初始化玩家活动数据；
#define MSG_TASK_INIT_AMUSEMENT			          34
struct SMsgTaskInitAmusement_SC
{
	DWORD  dwPDBID;
};

// 更新玩家活动数据；
#define MSG_TASK_UPDATE_AMUSEMENT			       35
struct SMsgTaskUpdateAmusement_SC
{
	DWORD  dwPDBID;                    // 玩家ID；
	char   szAmusementName[256];       // 名字；
	int    nKeyValue;                  // 值；
};
#pragma pack()
