/******************************************************************
** 文件名:	LoginForm.h
** 版  权:	(C)  
** 
** 日  期:	2008-3-14
** 版  本:	1.0
** 描  述:	物品栏管理器			
** 应  用:  
******************************************************************/
#pragma  once


class LoginForm :
	public FormBase
{
	

	/************************************************************************/
	/* 事件定义                                                              */
	/************************************************************************/
public:
    
	static const std::string EventLoginEnter; //通知打开登陆界面
	static const std::string EventLoginLeave; //通知打开登陆界面
	static const std::string EventLoginOk; //登陆成功
	static const std::string EventLoginFail; //登陆失败

	static const std::string EventOnLoginEnable; //按钮有效
	static const std::string EventOnLoginDisable; //按钮无效
	static const std::string EventOnReloadUserName;

	//属性
	static const std::string PropShowGameMakersWndName;//显示游戏制作人窗口,这个窗口暂时只是用作标记是否应该显示游戏制作人
	static const std::string PropGameMakersList;//游戏制作人列表

	static const std::string EventLoginQueueUpdate; //登录排队

	std::string m_userName;
	std::string m_password;

	/************************************************************************/
	/* constructor and deconstructor                                       */
	/************************************************************************/
public:
	LoginForm();
	virtual ~LoginForm();

	/************************************************************************/
	/* base class virtual function                                          */
	/************************************************************************/
public:
	virtual FormType getFormType();

	/** 设置为当前
	*/
	virtual void onEnter();

	/** 设置为无效,为切换作准备
	*/
	virtual void onLeave();

	virtual void update();

	virtual void render(IRenderSystem *pRenderSystem);

	void   sendLogin();

	void   sendRequestLogin();
	
	/************************************************************************/
	/* UI事件处理                                                           */
	/************************************************************************/

	/** 登陆服务器
	*/
	void onLogin(const std::string &username,const std::string& password);

	/** 当登陆界面上点cancel的时候
	*/
	void onCancel();

	//设置属性
	bool	SetStringProp(const std::string & propName, const std::string & propValue);
	
	/************************************************************************/
	/* message and event                                                     */
	/************************************************************************/
public:
	virtual void onMessage(ulong actionId, SGameMsgHead* head, void* data, size_t len);
	virtual void OnDisconnected( IConnection * conn,DWORD reason );
	virtual void OnError( IConnection * conn,DWORD dwErrorCode );
	virtual void OnConnected( IConnection * conn );
	int CharToHex( unsigned char  D[],unsigned char  S[], long size_d);

private:
	ImageRect*	m_pBg;
	bool m_bConnectedServer;

	uchar tmpkey[33];  //保存登录的会话密钥
	char *m_Key;

	std::string m_szShowGameMakersWndName;
	std::vector< std::string> m_GamemakersList;
	float m_fGameMakersListShift;
	float m_fGameMakersListLineWidth;
};