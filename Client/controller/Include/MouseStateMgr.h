/******************************************************************
** 文件名:	MouseStateMgr.h
** 版  权:	(C)  
** 
** 日  期:	2008-3-27
** 版  本:	1.0
** 描  述:	鼠标状态管理			
** 应  用:  
******************************************************************/
#pragma once

/************************************************************************/
/* 鼠标状态                                                                     */
/************************************************************************/
enum _EState
{
	stateAttack = 100,//攻击状态
	stateTalk,//交谈状态
	stateOpen,//包裹
	stateIdle,//鼠标在地图非动位置
	stateSkill,//使用技能
	stateCollect,//采集

	//以下为UI状态
	stateUINormal = 1000,//正常状态
	stateUIPick,//抓取物品
    //  从快捷栏替换起某个Item，添加该状态的目的是为了实现策划需求：如果从快捷栏上拾起了Item，再在快捷栏上点击时，
    //  则将Item放置到被点击的位置，同时如果被点击位置的Item有效，则将被点击位Item粘附到鼠标上；
    //  由于现有的依附处理是基于包裹ID和Item在包裹中的位置的，而此时需要粘附的Item的位置已经放置了新的Item，所以这
    //  里先根据被点击位置的Item来获取其在某个包裹中的某个位置所对应的Item，再使用这个对应的Item的位置信息来实现粘
    //  附效果，但是此时粘附在鼠标上的Item由于从表现逻辑上来讲是从快捷栏上拾起的，因此如果要对这个Item进行放置操作，
    //  只能将其放置到快捷栏上；由于此时已经不应该根据被粘附Item所对应的包裹来判断其拾起位置，因此引入了如下状态的
    //  定义：当处于stateReplaceFromShortCut状态时，如果进行左键点击，则将拾起被点击位置的Item（如果其有效），如果
    //  点击位置是在快捷栏上，还会将之前粘附在鼠标上的Item放置到被点击位置，其余操作都将取消粘附；
    stateReplaceFromShortCut,
	stateUISplit,//拆分
	stateUIRepair,//修理
	stateUIDrag,//拖动状态
	stateAutoMove,//自动移动
	stateInviteTeam,//队伍邀请
	stateUIIdentify,//装备鉴定
	stateResetUIsize,  // 重设大小窗口
	stateLearnSkill,   // 宠物学习技能时的鼠标状态
	stateUnknow
};


#define  MaxSize  24
struct _MouseState
{
	_EState state;
	char szName[MaxSize];
};



class MouseCursorState //鼠标光标状态
{
	std::string m_name;  //状态 
	std::string m_cursorImage;//光标图
public:
	MouseCursorState(const std::string& ){ m_cursorImage.clear();}
	const std::string& getName() const { return m_name;}
	const std::string& getCursorImage() const
	{
		return m_cursorImage;
	}

	void setCurrorImage(const std::string& cursorImage)
	{
		m_cursorImage = cursorImage;
	}
	virtual void onEnter();
	virtual void onLeave();
};

class MouseCursorStateIdentify: public MouseCursorState
{
public:
	MouseCursorStateIdentify(const std::string& name ):MouseCursorState(name) {}
	virtual void onEnter();
	virtual void onLeave();
};

class MouseCursorStateRepair: public MouseCursorState
{
public:
	MouseCursorStateRepair(const std::string& name ):MouseCursorState(name) {}
	virtual void onEnter();
	virtual void onLeave();
};

class MouseCursorStateMgr : public SingletonEx<MouseCursorStateMgr>
{
	typedef HashMap<_EState,MouseCursorState*> MouseStateMap;
	typedef HashMap<std::string,MouseStateMap::iterator> NameMapIerator;
	MouseStateMap			m_mouseStateMap;
	NameMapIerator			m_nameMapIterator;


	_EState	m_currentState;

	POINT		m_screenPt;				//保存鼠标点击开箱子,物品袋子之类东西,保存

	                                    //保存鼠标的位置,这个位置作为窗口显示位置参考
public:
	void setMouseCursorPosition(const POINT& pt){ m_screenPt = pt; }

	const POINT& getMouseCursorPosition() const { return m_screenPt;}

	/************************************************************************/
	/* event define                                                         */
	/************************************************************************/
public:
	static const std::string EventOnMouseStateChanged;  //状态改变
	/************************************************************************/
	/* mouse manager                                                        */
	/************************************************************************/
public:
	MouseCursorStateMgr();
	~MouseCursorStateMgr();

	/************************************************************************/
	/* instance                                                             */
	/************************************************************************/

	void _free();
	/************************************************************************/
	/* functions                                                            */
	/************************************************************************/
public:
	/** 初始化事件
	*/
	void addEvent();

	/** 设置状态光标
	*/
	bool setStateCursor(const std::string& stateName,const std::string& cursorImage);

	/** 得到最近的状态
	*/
	const _EState& getCurrentState() const { return m_currentState;}

	/** 切换到某个状态
	*/
	bool switchToState(_EState state);

	/************************************************************************/
	/* export for accesable                                                    */
	/************************************************************************/	
	bool toAttack(){ return switchToState(stateAttack);}//攻击状态
	bool toTalk(){ return switchToState(stateTalk);}//交谈状态
	bool toOpen(){ return switchToState(stateOpen);}//包裹
	bool toIdle(){ return switchToState(stateIdle);}//鼠标在地图非动位置
	bool toSkill(){ return switchToState(stateSkill);}//使用技能
	bool toCollect(){ return switchToState(stateCollect);}//采集
	//以下为UI状态
	bool toNormal(){ return switchToState(stateUINormal);}//正常状态
	bool toPick(){ return switchToState(stateUIPick);}//抓取物品
    //  从快捷栏替换起一个Item；
    bool toReplaceFromShortCut()
    {
        return switchToState(stateReplaceFromShortCut);
    }
	bool toRepair(){ return switchToState(stateUIRepair);}//修理
	bool toDrag(){ return switchToState(stateUIDrag);}//拖动状态
	bool toSplit(){ return switchToState(stateUISplit);}//拆分状态
	bool toIdentify(){ return switchToState(stateUIIdentify);}//进入物品鉴定状态

	bool toInvite(){return switchToState(stateInviteTeam);} //邀请状态
	bool toResetUIsize(){return switchToState(stateResetUIsize);} //鼠标改变窗体大小
	bool toLearnSkill(){return switchToState(stateLearnSkill);}  //学习宠物技能


	bool isAttack(){ return m_currentState == stateAttack;}//攻击状态
	bool isTalk(){ return m_currentState == stateTalk;}//交谈状态
	bool isOpen(){ return m_currentState == stateOpen;}//包裹
	bool isIdle(){ return m_currentState == stateIdle;}//鼠标在地图非动位置
	bool isSkill(){ return m_currentState == stateSkill;}//使用技能
	// 采集
	bool isCollect()
	{
		return m_currentState == stateCollect;
	}

	//以下为UI状态
	bool isNormal(){ return m_currentState == stateUINormal;}//正常状态
	bool isPick(){ return m_currentState == stateUIPick;}//抓取物品
    //  从快捷栏替换起一个Item；
    bool isReplaceFromShortCut()
    {
        return m_currentState == stateReplaceFromShortCut;
    }
	bool isRepair(){ return m_currentState == stateUIRepair;}//修理
	bool isDrag(){ return m_currentState == stateUIDrag;}//拖动状态
	bool isSplit(){ return m_currentState == stateUISplit;}//拆分状态
	bool isIdentify() { return m_currentState == stateUIIdentify;}//装备鉴定

	bool isInvite(){return m_currentState == stateInviteTeam;} //邀请状态
	bool isResetUIsize(){return m_currentState == stateResetUIsize;} // 是否为重设UI大小
	bool isLearnSkill(){return m_currentState == stateLearnSkill;}  // 是否为学习技能状态
};



