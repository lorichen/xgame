/******************************************************************
** 文件名:	shortcut.h
** 版  权:	(C)  
** 
** 日  期:	2008-4-6
** 版  本:	1.0
** 描  述:	快捷功能管理			
** 应  用:  
******************************************************************/
#include "ShortcutItem.h"

#pragma once

class ShortcutMgr : public TimerHandler,public SingletonEx<ShortcutMgr>, public IEventExecuteSink
{
	/************************************************************************/
	/* 事件定义                                                             */
	/************************************************************************/

	static const std::string		EventOnInitShortcutSlot;            //当初始化快捷功能条目
	static const std::string		EventOnUpdateShortcutPlace;         //当插入一个条目
	static const std::string		EventOnShortcutStateChanged;        //选中状态
	static const std::string		EventOnShorcutEnable;		        //使有效	

	static const std::string		EventOnShortcutSelected;            //选择当前技能的时候
	static const std::string		EventOnShortcutCooling;             //选择当前技能的时候
	static const std::string		EventOnShortcutEndCooling;          // 拖动前，目标槽位有技能时，则将该目标槽位关联的UI CD转圈效果取消
	static const std::string		EventOnShortcutItemDragCooling;     // 拖动后UI CD没转完时接着转围
	static const std::string		EventOnShortcutFrameShine;      // 快捷键按下后，当前按下的快捷槽位边框会亮一会 
	static const std::string		EventOnShorcutInitFinish;			// 初始化完成	

	static const std::string		EventOnShorcutOtherColling;			//在这里处理一些没有注册的冻结的事件，如变身后的技能，没有注册进快捷栏的by cjl

	bool					m_isEventInit ;

	uchar					m_data[UIDATA_LEN + 1]; //保存数据

	ulong					m_timerId;

	ulong					m_delay;

	typedef std::vector<ShortcutItem*> vShortcut;

	typedef HashMap<int,int>			ShortcutKeyMap;

	vShortcut		m_vShortcut;			// 快捷功能槽管理表

	ShortcutKeyMap	m_shortcutKeyMap;		// 快捷键表

	int				m_total;				// 总数

	int				m_current;				// 当前活动技能

	ulong			m_shortcutID;

    int				m_currentPage;			// 当前有效技能页面

	int				m_pkMode;

	bool            m_isAlready;

	/************************************************************************/
	/* constructor and deconstructor                                       */
	/************************************************************************/
public:
	ShortcutMgr();
	~ShortcutMgr();

	void stopTimer();
	void startTimer();


	void setDataState(bool setting){ m_isAlready = setting;}
	bool isDataAlready() { return m_isAlready;}


	void setPKMode(int mode);

	void clear();

	/** 根据位置索引取出对应的Item指针
	*/
	ShortcutItem* getShortcutItem(int pos);

	ShortcutItem* findSpace();

	ShortcutItem* findSkillItem(const SkillId& id);

	void  shineSkillItem(const SkillId& id);

	//  事件订阅
	void Subscriber();
	// 取消订阅处理
	void UnSubscriber();

	/////////////////////////////IEventExecuteSink///////////////////////////////////////////////
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	


	/************************************************************************/
	/* operator functions                                                   */
	/************************************************************************/
public:
	/** 初始化
	@param total 快捷功能槽的总数
	*/
	void init(int total);

	/** 设置当前活动技能
	*/
	void setCurrent(int current);

	/** 取得当前活动技能
	*/
	int getCurrent() const;

	/** 设置Item状态
	*/
	void setShortcutItemEnable(int pos,bool setting);

	/** 快捷键盘
	*/
	bool shortcutKey(int key);

	/** 增加一个快捷键
	@key 快捷键值
	@pos 对应的功能槽
	*/
	void addShortcutKey(int key,int pos);

    /************************************************************************/
	/* event handle                                                          */
	/************************************************************************/
	void onInitShortcutSlot(EventArgs& args);

	void onUpdateShortcutPlace(EventArgs& args);


	void onShortcutStateChanged(EventArgs& args);

	void onShortcutSelected(EventArgs& args);

	void onShorcutEnable(EventArgs& args); //选中状态

	void onShortcutCooling(EventArgs& args);

	void OnShortcutItemDragCooling(EventArgs& args);

	void OnShortcutEndCooling(EventArgs& args);

	void OnShortcutFrameShine(EventArgs& args);

	void initShortcutSlot();

	/** 判断是不是shortslot or item
	*/
	bool isShortcut(ulong ulID) { return m_shortcutID == ulID;}

	/** 左键处理事件
	*/
	bool leftClick(int nPos);

    //  右键处理；
    bool RightClick(int nPos);

    /** 删除指定位置的项
	*/
	void remove(int pos);

	/** 删除所有的Item
	*/
	void removeAllItem();

	/** update all item
	*/
	void updateAllItems();

	void UpdatePageItems();

	void restore(uchar* pData,int len);
	void store();

	ulong getID() const { return m_shortcutID;}

	void selectCurrent(bool bSelected);

	/** 设置当前技能页面
	*/
	void setShortcutPage(const int current)
	{ 
		m_currentPage = current;
		UpdatePageItems();
	}

	/** 获得当前技能页面
	*/
	int getShortcutPage()
	{
		return m_currentPage;
	}

    //  使用指定的物品；
    void UseItem(int nItemPos);

    /************************************************************************/
	/* 冷却                                                                 */
	/************************************************************************/
	void coolingSkill(const ulong& id);

	void coolingOtherSkill(ulong cd, const ulong& id);//by cjl

	void coolingGoods(const ulong& id);

	void updateItem(int type);

	// 更新快捷栏宠物技能数据
	void UpdatePetSkillItem(int nIndex,LONGLONG uidPet);

	/************************************************************************/
	/*  快捷栏目的保存和读取                                                */
	/************************************************************************/
	bool read();	

	void drawUI();

	/************************************************************************/
	/* timer                                                                 */
	/************************************************************************/
	virtual void OnTimer( unsigned long dwTimerID );

private:
	int GetPagePos(int nPos);

	void OnRidePetDropOut(LPCSTR pszContext, int nLen);
};
