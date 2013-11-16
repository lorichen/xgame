/******************************************************************
** 文件名:	CoolingMgr.h
** 版  权:	(C)  
** 
** 日  期:	2008-4-18
** 版  本:	1.0
** 描  述:	冷却管理			
** 应  用:  
******************************************************************/

#include "CoolingObject.h"
#include "CoolingSubscriber.h"

#pragma once

class CoolingMgr :public SingletonEx<CoolingMgr>
{
	typedef HashMap<ulong,CoolingObject*> CoolingObjectMap;
	CoolingObjectMap		m_coolingObjectMap[EFreeze_ClassID_Max];
	ulong					m_coolingObjectMax;

	CoolingSubscriber*		m_pCoolingSubscriber;

public:
	CoolingMgr();
	~CoolingMgr();


	void addCoolingObject(ulong type,ulong id,ulong ltime);
	//判断某个对象是否已经冷却完毕
	bool isCompleted(ulong id,ulong type);

	/**取得物品的冷却信息
	@param id 物品ID或者冷却ID
	@param type 类型,参考冷却类型
	@param ltime 冷却时间
	@param liveTime 剩下的时间
	@return 如果成功查询到返回true,否则false
	*/
	bool getCoolingInfo(const ulong id,const ulong type,ulong& ltime,ulong& liveTime);

	void update();

	void init();
	void clear();
};