/******************************************************************
** 文件名:	SkillForm.h
** 版  权:	(C)  
** 
** 日  期:	2008-4-7
** 版  本:	1.0
** 描  述:	技能窗口			
** 应  用:  
******************************************************************/
#pragma once

enum EXPProgress
{
	EXPProgress_Full      = 1,        // XP技能槽设置为满状态
	EXPProgress_Empty     = 2,        // XP技能槽设置为空状态
	EXPProgress_BeginDec  = 3,        // XP技能槽进度开始递减
	EXPProgress_PersonDie = 4,        // 人物死亡XP技能槽状态
};

//	面板类型
enum eSkillFormType
{
	eBranchOne = 1,
	eBranchTwo,
	eBranchThree,
	eGenius,
	eMaxPage,
};

//技能信息
struct _SkillInfo_
{	
	int			nSlotIndex;	//放置天赋的槽索引；
    uint        unBranch;	//技能分支；	
	int			index;		//在职业技能列表中的索引
	ulong		maxLevel;	//最大级别
	std::string name;		//名称
	std::string normalImage;
	std::string hoverImage;
	std::string disableImage;
	std::string normalImageExt;	//第二形态
	std::string hoverImageExt;
	std::string disableImageExt;
};

class SKillForm : public SingletonEx<SKillForm>, public IEventExecuteSink
{
	//显示
	static const std::string	EventOnSkillFormEnter;

	//关闭
	static const std::string    EventOnSkillFormLeave; 

	//更新
	static const std::string	EventOnUpdateSkill;

	//更新天赋点数
	static const std::string    EventOnUpdateGeniusPoints;

	//初始化系名称和页码
	static const std::string    EventOnSkillFormBranchInit;

 	// 显示XP图标
 	static const std::string	EventOnShowXPSkill; 	

 	// 始化XP栏
 	static const std::string	EventOnInitXPSlot; 

	// 等级未满10级时，天赋切页的tip提示
	static const std::string    EventOnGeniusTip;

	// 技能冷却
	static const std::string	EventOnCoolingSkill;

	//技能item标识ID
	ulong						m_slotID;							

public:
	SKillForm();
	~SKillForm(){}

	//  事件订阅和取消订阅处理；
	void Subscriber();

	void UnSubscriber();

	void ReadSkillAndGeniusInfo();

	void OpenSkillForm();

	void CloseSkillForm();

	void Update();

	void UpdateGeniusPoints();

	void UpgradeSkill(bool isSkill, int nSlotIndex, int upCount);

	std::string GetTooltip(const int& pos, bool bShortCut = false);

	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    //  在SKillForm类以外的实现中，要尽量记录技能的索引，并通过索引来对技能
    //  进行处理，以保持逻辑上的清晰，因为槽位这个概念毕竟只在SkillForm中才是有意义的；
 	bool GetGeniusInfoBySlotPos(const int& pos, _SkillInfo_& info) const;
	//	根据UI位置找技能
	bool GetSkillInfoBySlotPos(const int& pos, _SkillInfo_& info) const;
	//  根据技能索引查找技能与天赋
	bool GetSkillInfoByIndex(int nSkillIndex, _SkillInfo_& SkillInfo) const;

     //  切换技能分支；
     void SwitchSkillBranch(int unNewSkillBranchIndex, bool bUpdate);

	 //某系翻页的最大数
	 void CountSkillBranchPageNum(SkillBranchInitArgs& args);
	 int CountMaxPage(const int nums);

	 //技能系面板上翻页
	 void SetCurBranchCurPage(int curPage, bool bUpdate);

	 //左键点击技能，拖动操作，比如拖到快捷栏
	 bool LeftClicked(int pos);

 	 bool leftClickedXPSkill(int pos);

	//是否为skillform模块
 	bool isSkillSlot(ulong id) { return m_slotID == id;}

 	bool isSkillSlotXP(ulong id) { return m_slotXPID == id;}

	//区分是宠物还是人物(默认为0宠物使用技能时传对应宠物的UID)
	void UseSkill(const int& idx, LONGLONG Useruid = 0);

	void UseTileSkill(POINT pt, bool bXP);

 	int getSkillXPImage(SkillId* pSkillList, std::string& tooltip, int nIndex);

	//洗天赋
	void ResetGeniusPoints();

	//取学习技能Lv级需要的money
	int GetLearnSkillNeedMoney(int nSlotIndex);

	// 取天赋有tip内容
	std::string GetGeniusTip(int nSlotIndex, int geniusLv, int tempGeniusPoint);

	// 是否开启天赋面板
	bool IsEanbleGeniusWnd();

	// 等级小于10级时，天赋切页的tip内容
	std::string GetGeniusPageTooltipText();

	// 技能冷却
	void SKillForm::coolingSkill(const ulong& id);

	bool	IsSkillTargetValid(SkillContext *pContext);

protected:
	void SetArgs(SkillUpdateArgs& args, const _SkillInfo_& info);

 	// 处理EVENT_XPSKILL_CANUSE事件
 	void ProcEventXPSkillCanUse(void);
 
 	// 处理EVENT_XPSKILL_TIMEOUT事件
 	void ProcEventXPSkillTimeout(LPCSTR pszContext, int nLen);
 
 	// 处理EVENT_XPSKILL_CANBOOT事件
 	void ProcEventXPSkillCanBoot(void);

	// 技能排序，已学会的在前面,学会中等级要求低的在前面, 未学会的也一样排序
	void SortSkill(int branch);

	// 判断是否达到下一级的学习等级
	bool NextLevelIsEnable(SkillId skillId);

protected:
	std::vector<_SkillInfo_>  m_SkillsBranchOne;	//第1系
	std::vector<_SkillInfo_>  m_SkillsBranchTwo;	//第2系
	std::vector<_SkillInfo_>  m_Genius;				//职业所有天赋
	std::vector<_SkillInfo_>  m_XP;                 //XP技能
	std::vector<string>       m_SkillBranchName;	//技能分支名称；

	int						  m_CurType;			//当前选择（第一系，弟二系，其它技能，天赋）                   
	int                       m_CurBranchCurPage;   //当前系的当前页
	std::vector<int>          m_BranchMaxPage;		//每系的面板最大页数
	ulong					  m_ulTaskID;			//保存采集跳跃释放时所绑定的任务ID

 	ulong					  m_curXPSlotID;		//XP技能槽UI特效模型ID 	
 	ulong					  m_ulXPProgressBar;	//XP技能畜气满后播放的特效 
 	ulong					  m_slotXPID;           //XP技能标识ID
};