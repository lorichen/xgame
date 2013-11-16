/******************************************************************
** 文件名:	SelectServerForm.h
** 版  权:	(C)  
** 
** 日  期:	2010-5-17
** 版  本:	1.0
** 描  述:	服务器选择页面
** 应  用:  
******************************************************************/
#pragma  once


class SelectServerForm :
	public FormBase
{
	

	/************************************************************************/
	/* 事件定义                                                              */
	/************************************************************************/
public:
    
	static const std::string EventSelectServerEnter; //通知打开服务器选择界面
	static const std::string EventSelectServerLeave; //通知离开务器选择界面
	static const std::string EventSelectServerOk;    //成功
	static const std::string EventSelectServerCancel; //失败

	static const std::string EventOnSelectServerEnable; //按钮有效
	static const std::string EventOnSelectServerDisable; //按钮无效
	static const std::string EventUpdateServerListStatus;//更新服务器状态列表

	/************************************************************************/
	/* constructor and deconstructor                                       */
	/************************************************************************/
public:
	SelectServerForm();
	virtual ~SelectServerForm();

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

	//连接到super center
	bool ConnectSuperCenter(const std::string & strIP, WORD wPort);

	void DisConnectSuperCenter(void);
	
	/************************************************************************/
	/* UI事件处理                                                           */
	/************************************************************************/

	/** 选择登陆服务器
	*/
	void onSelectServer(const std::string &strArea,const std::string& strServer);

	/** 当选择登录服务器的界面上点cancel的时候
	*/
	void onCancel();
	
	/************************************************************************/
	/* message and event                                                     */
	/************************************************************************/
public:
	virtual void onMessage(ulong actionId, SGameMsgHead* head, void* data, size_t len);
	virtual void OnDisconnected( IConnection * conn,DWORD reason );
	virtual void OnError( IConnection * conn,DWORD dwErrorCode );
	virtual void OnConnected( IConnection * conn );
public:
	void getServerInfo(std::string & strArea, std::string & strServer);

private:
	ImageRect*	m_pBg;
	std::string m_strArea;
	std::string m_strServer;

	IConnection*		m_SuperCenterConnection;	/// 连接超级中心的网络连接，目前用于获取服务器列表状态

};