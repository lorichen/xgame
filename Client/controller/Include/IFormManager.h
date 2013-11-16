/******************************************************************
** 文件名:	IFormManager.h
** 版  权:	(C)  
** 
** 日  期:	2008-3-14
** 版  本:	1.0
** 描  述:	窗口模态管理器接口			
** 应  用:  
******************************************************************/
#pragma  once

enum EFormManangerError
{
	E_FORM_MANAGER_NO_ERROR = 0,
	E_FORM_MANAGER_NETWORK_DISCONNECT
};

struct IFormManager
{
	//选择服务器
	virtual void toSelectServer() = 0;

	///** 登陆
	//*/
	virtual void toLogin() = 0;

	///** 角色选择
	//*/
	virtual void toSelectActor() = 0;

	///** 创建角色界面
	//*/
	virtual void toCreateActor() = 0;

	///** 游戏
	//*/
	virtual void toGame() = 0;

	//进度条
	virtual void toProgress() = 0;

	/** update
	*/
	virtual void update() = 0;

	/** render
	*/
	virtual void render(IRenderSystem *pRenderSystem) = 0;


	/** 释放
	*/
	virtual void release() = 0;//

	/** 游戏状态
	*/
	virtual bool isGameState() = 0;

	/** 登陆状态
	*/
	virtual bool isLoginState() = 0;

	/** 角色选择状态
	*/
	virtual bool isSelectState() = 0;

	/** 创建角色状态
	*/
	virtual bool isCreateActorState() = 0;

	virtual int  addDida(int nLoc, LPCSTR pszNote,int slapse)  = 0;
	virtual void removeDida(int nIndex) = 0;

	/** 快捷功能
	*/
	//virtual bool shortcut(int shortcutkey) = 0;

	/** 给物品栏模块提供一个使用技能的接口
	*/
	virtual void useSkill(const ulong& id,const ulong& subId) = 0;

	/** 播放音乐的接口
	*/
	virtual void PlaySound(DWORD dwSID, int nLoop, float fVolume,int nSountType) = 0;

	/** 停止所有的声音
	*/
	virtual bool StopAllSound(bool bMute) = 0;

	/** 停止声音
	*/
	virtual bool StopSoundBySID(DWORD dwSID) = 0;

	/** 敏感词汇；暴露给聊天窗口使用
	*/
	virtual const std::vector<std::string> & GetBadWordsList() = 0;

	/** 在移动时需要去判断是否关闭该交互窗口
	*/
    virtual void CloseTaskFormWinodow(int x,int y,bool bCloseForce) = 0;

	//处理玩家发起的退出游戏客户端操作
	virtual void HandleQuitExeOperate(void) = 0;

	/** 设置窗口最小化
	*/
	virtual void SetIconMainWindow(bool bIcon) = 0;

	/** 获取错误状态，如网络连接错误
	*/
	virtual EFormManangerError GetErrorState(void) const = 0;

	/** UpdateSyetemSet 更新系统控制
	*/
	virtual void UpdateSyetemSet(void) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(CONTROLLERGAME_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(xs_cfm))
extern "C" IFormManager * LoadCFM(void);
#	define	CreateFormManagerProc	LoadCFM
#else														/// 动态库版本
typedef IFormManager * (RKT_CDECL *procCreateFormManager)(void);
#	define	CreateFormManagerProc	DllApi<procCreateFormManager>::load(MAKE_DLL_NAME(xs_cfm), "LoadCFM")
#endif
