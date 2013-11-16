/******************************************************************
** 文件名:	UIStateManager.h
** 版  权:	(C)  
** 
** 日  期:	2008-3-14
** 版  本:	1.0
** 描  述:	窗口模态管理器			
** 应  用:  
******************************************************************/
#include "IFormManager.h"
#pragma  once

class FormManager;

extern FormManager * gFormManager;

enum FormType
{
	selectServerForm, //选择服务器
	loginForm,	//登陆
	selectActorForm,//角色选择
	progressForm,   //进度
	createActorForm,//创建角色
	gameForm	//游戏
};
//form基类
class FormBase:
	public InputMessageHandler,
	public IMessageHandler, 
	public IConnectionEventHandler
{
public:
	FormBase():m_isConnectedError(false){}
	virtual ~FormBase(){}
	/** 得到窗体的类型
	*/
	virtual FormType getFormType() = 0;

	/** 设置为当前
	*/
	virtual void onEnter() = 0;

	/** 代替时钟
	*/
	virtual void update() = 0;

	virtual void render(IRenderSystem *pRenderSystem) = 0;

	/** 设置为无效,为切换作准备
	*/
	virtual void onLeave() = 0;


	/** 判断是否连接错误
	*/
	bool isConnectedError() { return m_isConnectedError == true;}

	/** 加载地图
	*/
	virtual bool loadMap(ulong actionId, SGameMsgHead* head, void* data, size_t len);


/************************************************************************/
/* interface                                                            */
/************************************************************************/

	virtual void onMouseMessage(MouseMessage& msg){};
	virtual void onKeyMessage(KeyMessage& msg){};
	virtual void OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler ){}

	virtual void OnConnected( IConnection * conn );
	virtual void OnDisconnected( IConnection * conn,DWORD reason );
	virtual void OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen );
	virtual void OnError( IConnection * conn,DWORD dwErrorCode );
	virtual void onMessage(ulong actionId, SGameMsgHead* head, void* data, size_t len);
	virtual const std::string& GetErrorAndQuitEventForUI(void) {return EventActorErrorAndQuit;}
protected:
		bool m_isConnectedError;
		static const std::string EventActorErrorAndQuit;
};

class FormManager: public IFormManager,
				   public EventSet
{
	
	
	typedef std::list<FormBase*> LFormBase;
	
	LFormBase		m_lFormBase;  //form 管理列表

	FormType		m_currentFormType;

	FormBase*		m_currentForm;	
	Actor*			m_pActor;

	/************************************************************************/
	/* constructor and deconstructor                                        */
	/************************************************************************/
public:
	FormManager();
	~FormManager();

	/** 创建
	*/
	bool create();

	/** add Event
	*/
	void addFormEvent(const std::string & eventName);

	/** 切换到指定的form.
	*/
	void switchToForm(FormType formType);

	/** 取得最近的form,注意检查,因为可能返回空值
	*/
	FormBase *getCurrentForm() { return m_currentForm ;}

	void fillActorInfo(SMsgLoginServer_ActorInfo*ssa);

	SMsgLoginServer_ActorInfo* getActorInfo(); 

	bool setCurrentActor( const unsigned long & index);
	const unsigned long getCurrentActor();

	//void setCurrentActor(const unsigned long& actorIndex){ if ( m_pActor) m_pActor->setCurrentActorIdx(actorIndex);}
	//const unsigned long getCurrentActor() const { return m_pActor->getCurrentActorIdx();}

	Actor* getActor() const { return m_pActor; }
	/************************************************************************/
	/* 接口实现                                                              */
	/************************************************************************/
public:
	/**选择服务器
	/*
	*/
	virtual void toSelectServer();

	/** 登陆
	*/
	virtual void toLogin();

	/** 角色选择
	*/
	virtual void toSelectActor();

	virtual void toCreateActor();

	/** 游戏
	*/
	virtual void toGame() ;

	//进度条
	virtual void toProgress() ;

	virtual void update() ;

	virtual void render(IRenderSystem *pRenderSystem);

	/** 释放
	*/
	virtual void release();//

	/** 游戏状态
	*/
	virtual bool isGameState();

	/** 登陆状态
	*/
	virtual bool isLoginState();

	/** 角色选择状态
	*/
	virtual bool isSelectState();

	/** 创建角色状态
	*/
	virtual bool isCreateActorState();

	//virtual bool shortcut(int shortcutkey);

	virtual void useSkill(const ulong& id,const ulong& subId){} 

	/** 播放音乐的接口
	*/
	virtual void PlaySound(DWORD dwSID, int nLoop, float fVolume,int nSountType);

	/** 停止所有的声音
	*/
	virtual bool StopAllSound(bool bMute);

	virtual bool StopSoundBySID(DWORD dwSID);

	/** 敏感词汇；暴露给聊天窗口使用
	*/
	virtual const std::vector<std::string> & GetBadWordsList();

	/** 在移动时需要去判断是否关闭该交互窗口
	*/
	virtual void CloseTaskFormWinodow(int x,int y,bool bCloseForce);

	/** 设置窗口最小化
	*/
	virtual void SetIconMainWindow(bool bIcon);

	virtual int  addDida(int nLoc, LPCSTR pszNote,int slapse);
	virtual void removeDida(int nIndex);


	//处理玩家发起的退出游戏客户端操作
	virtual void HandleQuitExeOperate(void);

	virtual EFormManangerError GetErrorState(void) const;

	/** UpdteSyetemSet 更新系统控制
	*/
	virtual void UpdateSyetemSet(void);
public:
	void getServerInfo(std::string & strArea, std::string & strServer);


};