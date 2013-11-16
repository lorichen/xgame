/******************************************************************
** 文件名:	PickupForm.h
** 版  权:	(C)  
** 
** 日  期:	2008-4-8
** 版  本:	1.0
** 描  述:	掉落			
** 应  用:  
******************************************************************/
#pragma once

class PickupForm : public TimerHandler,public IEventExecuteSink,public SingletonEx<PickupForm>
{
	enum
	{
		ITEM_TYPE_GOODS = 1,
		ITEM_TYPE_PETS  = 2,
	};
	/************************************************************************/
	/* 事件定义                                                             */
	/************************************************************************/
	static const	std::string EventOnAddGoodsSuccess;     // 去绘制效果
	static const    std::string EventOpenItemHelpToolTipForm; // 是否需要打开提示面板
	/************************************************************************/
	/* data member                                                          */
	/************************************************************************/
	int		m_iCount;     //总数
	UID		m_uid;			//宝箱的UID
	bool	m_isOpen;		//开取还是关闭
	ulong	m_timeId;
	bool	m_running;
public:
	PickupForm();

	~PickupForm();

	void Init();

	void UnInit();

	void setUID(const UID& uid);

	void setOpenState(bool setting);

	bool isOpen(){ return m_isOpen ;}
	void setCount(int count) { m_iCount = count ;}

	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	/************************************************************************/
	/* event handler                                                        */
	/************************************************************************/
	void open(const UID& uid);
	void close();

	bool pick(ulong ulPackID,int iPos,int iTartPos = -1 );

	void reset();

	void startTimer();
	void endTimer();

	virtual void OnTimer( unsigned long dwTimerID );
	
	bool CanPick();

	void  Update();
private:

	// 响应物品拾取成功事件
	void                    OnEventAddGoodsSuccess(LPCSTR pszContext, int nLen);

	//生成提示语句文本
	string                  GetAddItemToolTip(IEntity* pEntity);

	//添加物品时需要在包裹内添加相关提示；
	int                    AddEquipIconInBagEffectTooltip(IEquipment *pEntity);

};