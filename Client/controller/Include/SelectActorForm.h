/******************************************************************
** 文件名:	SelectActorForm.h
** 版  权:	(C)  
** 
** 日  期:	2008-3-14
** 版  本:	1.0
** 描  述:	角色创建,选择			
** 应  用:  
******************************************************************/
#pragma  once

class AnimationCallback : public IAnimationCallback
{
	ModelNode*	m_pModel;
public:
	AnimationCallback(ModelNode *pModel);
	~AnimationCallback();

	ModelNode*	getModelNode();
public:
	virtual void onOneCycle(const std::string& animation,int loopTimes);
};

class SelectActorForm :
	public FormBase
{

	/************************************************************************/
	/* 事件定义                                                              */
	/************************************************************************/
public:

	static const std::string EventSelectActorEnter; //通知打开登陆界面
	static const std::string EventSelectActorLeave; //通知关闭选择界面
	static const std::string EventEnterGame; //进入游戏	
	static const std::string EventBack; //返回到登陆界面
	static const std::string EventActorListChanged;//角色列表更新
	static const std::string EventDeleteActorFail;//删除角色失败
	static const std::string EventRecoverActorFail;//恢复角色失败
	static const std::string EventSelectActorGeneralFail;//选择角色通用错误
	static const std::string EventSelectActorErrorAndQuit;//必须重新登录的错误
	static const std::string EventSelectedActorInfo;	//当前选中的对象数据

public:
	/************************************************************************/
	/*属性设置																*/
	/************************************************************************/
	static const std::string PropActorViewWindow;//角色显示窗口
	static const std::string PropRotateSelectedActor;//选择选择的角色
	static const std::string PropServerInfo;//服务器信息

	/************************************************************************/
	/* constructor and deconstructor                                       */
	/************************************************************************/
public:
	SelectActorForm();
	virtual ~SelectActorForm();

	/************************************************************************/
	/* base class virtual function                                          */
	/************************************************************************/
	virtual const std::string& GetErrorAndQuitEventForUI(void) {return EventSelectActorErrorAndQuit; }
public:
	virtual FormType getFormType();

	/** 设置为当前
	*/
	virtual void EnterGame();

	/** 设置为无效,为切换作准备
	*/
	virtual void onLeave();

	virtual void update();

	virtual void render(IRenderSystem *pRenderSystem);

	/** 开始loading
	*/
	void loadStart();

	/** 结束loading
	*/
	void loadEnd();

	/************************************************************************/
	/* UI事件处理                                                           */
	/************************************************************************/

	/** 选择角色之后进入游戏
	*/
	void onEnter();

	/** 创建角色
	@param name 角色名称
	*/
	void onToCreate();

	/** 删除角色
	*/
	void onDelete();


	void GetSelectActorInfo();

	/**恢复角色
	*/
	void RecoverActor();

	
	/** 当角色界面上点cancel的时候或者ESC
	*/
	void onCancel();

	/** update all actor
	*/
	void onUpdateActor();

	/************************************************************************/
	/* message and event                                                     */
	/************************************************************************/
public:
	virtual void onMessage(ulong actionId, SGameMsgHead* head, void* data, size_t len);

	/***************************************************************************/
	/* UI交互																*/
	/****************************************************************************/
	bool	SetStringProp(const std::string & propName, const std::string & propValue);
	bool	SetNumProp(const std::string & propName, int propValue);
	int		GetNumProp( const std::string & propName, int param);
	std::string GetStringProp( const std::string propName, int param);

private:
	ImageRect*	m_pBg;
	Camera*		m_pCamera;
	Viewport*	m_pViewport;
	std::list<AnimationCallback*>	m_vCallback;

	std::string m_szActorViewWindow;
	int			m_actorRotateAngle;

	// 背景特效；
	xs::ModelNode* m_pBgEffect;
	EntityView*		m_pBgEffectView;
};