//////////////////////////////////////////////////////////////////////////
//  该文件对如下内容进行了声明：
//      CTaskInfo： 定义任务信息的结构；
//      EEntityTaskSign： 定义实体的任务标记；
//      ITaskClient： 任务管理功能的接口；
//      ITaskPart： 任务部件的接口；
//      CreateTaskClientProc： 用于创建ITaskClient接口的宏；
//      CloseTaskClient： 用于释放TaskPart.dll的宏；

#pragma once

#include "IEntityClient.h"
#include "../Common/GlobalDefine.h"
#include "../Common/DGlobalEvent.h"
#include <map>

class ITaskPart;
class IAmusementPart;

//////////////////////////////////////////////////////////////////////////
//  已接任务的数据的定义；

#pragma pack(1)
typedef struct CHoldTaskData
{
public:
    CHoldTaskData()
    {
        memset(this, 0, sizeof(CHoldTaskData));
    }

    //  索引的有效性由调用者保证；
    unsigned short& operator[](unsigned char ucIndex)
    {
		static  unsigned short ucTemp = 0;
        if (ucIndex >= TASK_REQUIREMENT_MAX_COUNT)
        {
            ASSERT(false);
			return ucTemp;
        }
        return TaskData[ucIndex];
    }

//private:
    //  任务数据表：每个元素对应一个任务需求的完成度，所对应的具体的任务需求由LUA脚本负责确定；
    //  index：任务需求的索引：由LUA脚本保证其与任务需求相对应的正确性；
    //  value：任务需求的完成度；
    unsigned short TaskData[TASK_REQUIREMENT_MAX_COUNT];

	// 任务跟踪索引
	uchar   ucTaskTraceIndex;
} TARRAY_HOLDTASK_DATA;

#pragma pack()
//  已接任务的记录结构；
//  key：任务ID；
//  value：任务数据；
typedef std::map< unsigned short, TARRAY_HOLDTASK_DATA > TMAP_HOLDTASK_INFO;

//////////////////////////////////////////////////////////////////////////
//  CTaskInfo类的声明：定义任务信息的结构；
struct CTaskInfo
{
    //  任务名称；
    std::string m_strTaskName;
    //  任务描述；
    std::string m_strTaskDescription;
    //  任务需求；
    std::string m_strTaskRequirement;
    //  任务所在区域；
    std::string m_strTaskArea;
	//	派发任务实体
	std::string m_strTaskDispatched;
    //  任务等级；
    int m_nTaskLevel;
    //  可以接受任务的最低等级；
    int m_nMinLevel;
    //  可以接受任务的最高等级；
    int m_nMaxLevel;
	//	前置任务
	std::string m_strPreTasks;
	// 可选前置任务
	std::string m_strChosePreTasks;
	// 阵营任务标识
	std::string m_strNationTasks;
	//	任务难度；
	int	m_nTaskDifficulty;
	//  触发类型
	int m_nTouchOffType;
	//  任务是否循环；
	int m_nTaskRepetition;
	//	门派限制
	int	m_nVocation;
	// 任务是否可共享
	int m_nShare;
	// 任务是否可删除
	int m_nDelete;
	// 任务剩余时间
	int m_nLeavingTime;
	//  任务提示图片的名字
	std::string m_strToolTipImageName;
    //  任务奖励；
    CTaskPrize m_TaskPrize;
};

//  任务信息表；
//  Key：任务ID；
//  Value：任务信息；
typedef map< WORD, CTaskInfo > TMAP_TASKINFO_SCHEME;

//////////////////////////////////////////////////////////////////////////
//  EEntityTaskSign的声明：定义实体的任务标记；
enum EEntityTaskSign
{
    ETS_LOWWER_BOUND = -1,

    //  无任务标记；
    ETS_NoTaskSign,
    //  有可接受任务；
    ETS_HasAcceptableTask,
	//  有可接受的循环任务
	ETS_HasAcceptableRepetitionTask,
    //  有已完成任务；
    ETS_HasAccomplishedTask,
	//  有已完成的循环任务
	ETS_HasAccomplishedRepetitionTask,
    //  有未完成任务；
    ETS_HasUnfinishedTask,

    ETS_UPPER_BOUND,
};

//////////////////////////////////////////////////////////////////////////
//  CAmusemetInfo声明；定义活动信息的结构；
struct CAmusementInfo
{
	//  名称；
	std::string m_strAmusementName;

	//  类型；
	int m_nAmusementType;

	//  人数；
	int m_nAmusementPlayerCount;

	//  提示区域；
	std::string m_strAmusementMapArea;

	// 间隔时间段；
	int   m_nAmusementTimeSpace;

	// 开放时间；
	std::string m_strStartTime;

	// 开放日期；
	std::string m_strStartDay;

	//  图片ID；
	int m_nImageIconID;

	// 活动等级；
	int m_nNeedLevel;

	//  可以接受的最低等级；
	int m_nMinLevel;

	//  可以接受的最高等级；
	int m_nMaxLevel;

	//  可完成次数；
	int m_nReportNumber;

	//	难度；
	int	m_nAmusementDifficulty;

	//  开启时间描述；
	std::string m_strStartTimeDescription;

	//	派发NPC；
	std::string m_strAmusementDispatched;

	//  活动奖励；
	std::string m_strAmusementPrize;

	//  描述；
	std::string m_strDescription;
};

//  活动信息表；
//  Key：活动ID；
//  Value：活动信息；
typedef map<WORD, CAmusementInfo> TMAP_AMUSEMENTINFO_SCHEME;
//////////////////////////////////////////////////////////////////////////
//  CAmusementPetInfo声明；定义活动信息的结构；
struct CAmusementPetInfo
{
	//  名称；
	std::string m_strAmusementPetName;

	//  类型；
	int m_nAmusementType;

	// 魂将刷新地图ID；
	int m_nAmusementPetMapID;

	//  提示区域；
	std::string m_strAmusementPetMapArea;

	// 魂将等级；
	int m_nPetLevel;

	// 是否提示；
	int m_nNotice;

	// 提示内容；
	std::string m_strNoticeContext;

	// 刷新时间；
	int m_nReFreshTime;

	// 开放时间；
	std::string m_strStartTime;

	// 开放日期；
	std::string m_strStartDay;

	//  图片的名字；
	int  m_nImageIconID;

	//  可以接受的最低等级；
	int m_nMinLevel;

	//  可以接受的最高等级；
	int m_nMaxLevel;

	//  音效ID；
	int m_nSoundID;

	//	难度；
	int	m_nDifficulty;

	//  开启时间描述；
	std::string m_strStartTimeDescription;

	//	地点；
	std::string m_strDispatchedMap;

	//  活动奖励；
	std::string m_strAmusementPetPrize;

	//  描述；
	std::string m_strDescription;
};

//  活动魂将信息表；
//  Key：活动魂将ID；
//  Value：活动魂将信息；
typedef map<WORD, CAmusementPetInfo> TMAP_AMUSEMENTPETINFO_SCHEME;
//////////////////////////////////////////////////////////////////////////
//  ITaskClient接口的声明；
class ITaskClient
{
public:
    //  释放自身的函数；
    virtual void Release() = 0;

    //  创建任务部件；
    virtual ITaskPart* CreateTaskPart() = 0;

	//  创建活动部件；
	virtual IAmusementPart* CreateAmusementPart() = 0;

    //  获取指定任务的信息；
    virtual CTaskInfo* GetTaskInfo(WORD wTaskID) = 0;

	//	获取所有任务信息
	virtual TMAP_TASKINFO_SCHEME& GetTaskList() = 0;

	//  获取指定活动的信息；
	virtual CAmusementInfo* GetAmusementInfo(WORD wAmusementID) = 0;

	// 获取指定活动的所有信息
	virtual TMAP_AMUSEMENTINFO_SCHEME& GetAmusementList() = 0;

	//  获取指定野外魂将的信息；
	virtual CAmusementPetInfo* GetAmusementPetInfo(WORD wAmusementID) = 0;

	// 获取野外魂将的所有信息
	virtual TMAP_AMUSEMENTPETINFO_SCHEME& GetAmusementPetList() = 0;

    //  获取指定实体的可派发任务；
    virtual vector< WORD >* GetEntityDispatchedTask(int nEntityID) = 0;

    //  获取指定实体的可交任务；
    virtual vector< WORD >* GetEntityAccomplishedTask(int nEntityID) = 0;

    //  获取指定任务指定步骤中指定NPC的对话信息；
    virtual std::string GetDialogInfo(WORD wTaskID, uint unDialogIndex) = 0;

    //  获取指定实体的任务标记；
    virtual EEntityTaskSign GetEntityTaskSign(LONGLONG uidEntity) = 0;
};

//////////////////////////////////////////////////////////////////////////
//  EEntityAmusementType的声明：定义实体的活动类型；
enum EEntityAmusementType
{
	EAT_LOWWER_BOUND = 0,

	//  定期活动；
	EAT_TYPE_ONTIME,
	//  每日活动；
	EAT_TYPE_ONDAY,
	//  循环活动；
	EAT_TYPE_LOOP,
	//  副本活动；
	EAT_TYPE_COPY,
	//  野外魂将
	EAT_TYPE_BOSS,

	EAT_UPPER_BOUND,
};

//////////////////////////////////////////////////////////////////////////
//  ITaskPart接口的声明；
class ITaskPart : public IEntityPart
{
public:
    //  获取任务记录；
    virtual const TMAP_HOLDTASK_INFO& GetTastRecord() const = 0;

	//	获取可接任务记录；
	virtual const set<WORD>& GetCanAcceptTaskRecord() const = 0;

    //  初始化任务列表；
    virtual void InitHoldTaskList(LPCSTR lpszTaskList, int nTaskListLength) = 0;

    //  初始化已完成任务列表；
    virtual void InitAccomplishedTaskList(LPCSTR lpszTaskList, int nTaskListLength) = 0;

    //  添加一条任务记录；
    virtual void AddTask(WORD wTaskID,WORD wTraceIndex = 0) = 0;

    //  删除指定的任务记录；
    virtual void RemoveTask(WORD wTaskID) = 0;

	virtual void OnRemoveTask(WORD wTaskID) = 0;

    //  更新指定的任务记录；
    virtual void UpdateTask(WORD wTaskID, uchar ucIndex, ushort usValue) = 0;

    //  完成指定的任务；
    virtual void AccomplishTask(WORD wTaskID) = 0;

	// 删除已经完成的任务
	virtual void DeleteTaskCompelete(WORD wTaskID) = 0;

    //  能否接受指定的任务；
    virtual bool CanAccept(WORD wTaskID) = 0;

    //  指定任务是否已经做过；
    virtual bool IsAccomplishedTask(WORD wTaskID) = 0;

	// 发送提示图片
	virtual bool SendTaskImageToolTipByName(LPCSTR pContext) = 0; 
};

//////////////////////////////////////////////////////////////////////////
//  IAmusementPart接口的声明；
class IAmusementPart : public IEntityPart
{
public:
	////  获取记录；
	//virtual const TMAP_HOLDTASK_INFO& GetTastRecord() const = 0;

	////	获取可接任务记录；
	//virtual const set<WORD>& GetCanAcceptTaskRecord() const = 0;

	////  初始化活动列表；
	virtual void InitAmusementList(LPCSTR lpszAmusementList, int nAmusementListLength) = 0;

	////  添加一条活动记录；
	virtual void AddAmusement(WORD wAmusementID,WORD wIndex = 0) = 0;

	////  删除指定的活动记录；
	virtual void RemoveAmusement(WORD wAmusementID) = 0;

	////  更新指定的活动记录；
	virtual void UpdateAmusement(WORD wAmusementID,ushort usValue) = 0;

	////  完成指定的活动；
	virtual void AccomplishAmusement(WORD wAmusementID) = 0;

	//// 删除已经完成的活动
	virtual void DeleteAmusementCompelete(WORD wAmusementID) = 0;

	////  能否接受指定的活动；
	virtual bool CanAccept(WORD wAmusementID) = 0;

	////  指定活动是否已经做过；
	virtual bool IsAccomplishedAmusement(WORD wAmusementID) = 0;

};

//////////////////////////////////////////////////////////////////////////
//  导出函数的定义；
//  静态库版本；
#if defined(_LIB) || defined(TASKCLIENT_STATIC_LIB)
#	pragma comment(lib, MAKE_LIB_NAME(xs_ctk))
extern "C" ITaskClient* LoadCTK(void);
#	define	CreateTaskClientProc	LoadCTK
#	define  CloseTaskClient()
//  动态库版本；
#else
typedef ITaskClient* (RKT_CDECL *procCreateTaskClient)(void);
//  用于创建ITaskClient接口；
#define CreateTaskClientProc DllApi<procCreateTaskClient>::load(MAKE_DLL_NAME(xs_ctk), "LoadCTK")
//  用于释放TaskPart.dll；
#define CloseTaskClient() DllApi<procCreateTaskClient>::freelib()
#endif
