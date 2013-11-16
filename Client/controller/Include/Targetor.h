/******************************************************************
** 文件名:	Targetor.h
** 版  权:	(C)  
** 
** 日  期:	2008-4-3
** 版  本:	1.0
** 描  述:	目标状态管理			
** 应  用:  
******************************************************************/
#pragma once

#include "TimerHandler.h"

class TargetorMgr:
	public SingletonEx<TargetorMgr>,
	public IMessageExecuteSink,
	public IEventExecuteSink,
	public TimerHandler
{
	UID		m_targetUID;    //目标UID

	bool    m_bPermanentSelect; // 是否为永久选中

	UID		m_boxUID;		//目标宝箱

//	EffectContext m_ec;     //脚底光效
	/************************************************************************/
	/* Event define                                                         */
	/************************************************************************/
public:
	static const std::string		EventOnTargetEnter;  //选定目标
	static const std::string		EventOnTargetLeave;	 //失去目标
	static const std::string		EventOnTargetSwitch; //改变目标
	/************************************************************************/
	/* constructor and deconstructor                                        */
	/************************************************************************/
public:
	TargetorMgr();
	~TargetorMgr();

	void create();
	void close();



	/************************************************************************/
	/* event functions                                                           */
	/************************************************************************/
public:
	/** 得到目标事件
	*/
	void onTargetorEnter(EventArgs& args);

	/** 失去目标事件
	*/
	void onTargetorLeave(EventArgs& args);

	/** 改变目标事件
	*/
	void onTargetorSwitch(EventArgs& args);

	virtual void OnTimer( unsigned long dwTimerID );

public:
	/** 设置目标
	*/
	void setTarget(const UID& uid, bool bPermanentSelect = false, bool bNotShowTargetFrame = false, bool bNotShowEffect = false);
	void setTarget(IEntity* pEntity, bool bPermanentSelect = false, bool bNotShowTargetFrame = false, bool bNotShowEffect = false);
	const UID getTarget() const;

	//void SetPermanentSelect(bool bPermanentSelect);

	const bool TargetorMgr::IsPermanentSelect(void) const;

    //  记录鼠标当前位置；
    void SetGoundTile(const POINT& pointTile);
    //  获取鼠标当前位置；
    const POINT& GetGroundTile() const;

	void lost();	

	void updateProperty(IEntity* pEntity);

	/** 玩家处于非行走状态时,当选中某对象时,角色应转身朝向这个对象
	这样的处理可以强化选中目标的响应 
	如果不这样处理,选中以后立即攻击,就会发现角色猛转身去攻击
	*/
	void startTurnHead();

	void stopTurnHead();

	/** 鼠标悬停时要绘制宝箱的名字
	*/
	void SelectBox(LONGLONG uidBox);

	void UnSelectedBox();

	/************************************************************************/
	/* 请求交易                                                                     */
	/************************************************************************/
	// 此处添加
	void requestExchange(LONGLONG uid);

	/************************************************************************/
	/* interface functions                                                  */
	/************************************************************************/
public:
	virtual void	OnExecute(DWORD dwMsgID, SGameMsgHead * pGameMsgHead, LPCSTR pszMsg, int nLen);

	virtual void	OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

private:
	void capture(bool bNotShowTargetFrame, bool bNotShowEffect);
protected:
    //  鼠标当前位置，当鼠标在地面移动时记录鼠标当前所在位置的Tile；
    POINT m_pointGroundTile;
};
