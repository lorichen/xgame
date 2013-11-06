//==========================================================================
/**
* @file	  : IGlobalClient.h
* @author : 
* created : 2008-1-14   15:47
* purpose : 客户端全局对象
*/
//==========================================================================

#ifndef __IGLOBALCLIENT_H__
#define __IGLOBALCLIENT_H__

#include <string>

//现在的客户端各模块耦合性太强，无法通过函数调用实现页面的跳转
//目前想到的方案：通过发送窗口消息，作为系统菜单的实现机制
enum
{
	WM_BACK_TO_SELECT_SERVER = WM_USER + 0,
	WM_BACK_TO_RE_LOGIN = WM_USER + 1,			//将来策划可能要求转到登录界面
	WM_BACK_TO_SELECT_ACTOR = WM_USER + 2,
};

namespace xs {
	struct IEventEngine;
	struct ISchemeEngine;
	struct ILuaEngine;
	struct IResourceManager;
	struct IConnection;
}

struct ISceneManager2;
struct IMessageDispatch;
struct IInputMessageManager;
struct IEntityFactory;
struct IEntityClient;
struct IEffectClient;
struct IBuffClient;
struct ISkillManager;
struct IFreezeClient;
struct IChatClient;
struct ISkepClient;
struct IFormManager;
struct IPKModeClient;
struct IGoodsFunctionClient;
struct ITradeClient;
struct IConfigManager;
struct IServerListConfig;
struct ITeamClient;
struct IFriendClient;
struct IAudioEngine;
class ITaskClient;
struct IProgressManager;
struct IStallClient;
struct ICollectionClient;
struct IHardwareCursorCallBack;
struct IFamilyClient;
struct IArmyClient;


//#define MEM_LEAK_TEST
#ifdef MEM_LEAK_TEST
	// 调试内存泄露 [5/5/2011 zgz]
	enum EMemCreateType
	{
		MemType_MapEffect = 1, // 地表特效(用flagMapItem标记)
		MemType_MapStatic = 2, // 静态实体
		MemType_Occupant  = 3, // 占位信息
	};
	class EntityView;
#endif

/// 为系统定义的几种预置字体
enum SysFont
{
	fontDefault = 0,
	fontEffect,			// 特效字体
	fontHeadEffect,		// 头顶特效
	fontEffect_XK,      // 特效字体2
	maxFontCount,
};

struct IGlobalClient
{

#ifdef RKT_UI_TEST
	//virtual bool isUITest() const = 0;
#endif
	/// 获取工作目录(exe所在目录)
	virtual const std::string& getWorkDir() const = 0;

	/// 获取窗口句柄
	virtual void* getHWND() const = 0;

	/// 获取目录态顶层文件系统
	virtual IFileSystem* getFileSystemRoot() const = 0;

	/// 获取时间戳
	virtual ulong getTimeStamp() const = 0;

	/// 设置时间戳
	virtual void setTimeStamp(ulong ts) = 0;

	/// 获取时间（秒为单位）
	virtual ulong getTimeSecond() const = 0;

	/// 设置时间（秒为单位；格林尼治时间）
	virtual void setTimeSecond(ulong ts) = 0;

	/// 获取主角
	virtual handle getPlayer() const = 0;

	/// 设置主角
	virtual void setPlayer(handle entity) = 0;

	/// 获取目标实体对象
	virtual handle getTarget() const = 0;

	/// 设置目标实体对象
	virtual void setTarget(handle entity) = 0;


	// -----------------------服务模块----------------------
	///声音
	virtual IAudioEngine* getAudioEngine() const = 0;
	/// 获取渲染引擎
	virtual IRenderEngine* getRenderEngine() const = 0;

	/// 获取渲染系统
	virtual IRenderSystem* getRenderSystem() const = 0;

	/// 获取系统预置字体
	virtual xs::IFont* getFont(int type = fontDefault) const = 0;


	/// 获取事件服务器
	virtual IEventEngine* getEventEngine() const = 0;

	/// 获取配置引擎
	virtual ISchemeEngine* getSchemeEngine() const = 0;

	/// 获取时间轴
	virtual TimerAxis* getTimerAxis() const = 0;

	/// 获取实体管理器
	virtual IEntityClient* getEntityClient() const = 0;

	/// 获取脚本引擎
	virtual ILuaEngine* getLuaEngine() const = 0;

	/// 获取消息分发器
	virtual IMessageDispatch* getMessageDispatch() const = 0;

	/// 获取输入消息管理器
	virtual IInputMessageManager* getInputMessageManager() const = 0;

	/// 获取资源管理器
	virtual IResourceManager* getResourceManager() const = 0;

	/// 获取实体视图工厂
	virtual IEntityFactory* getEntityFactory() const = 0;

	/// 获取场景管理器
	virtual ISceneManager2* getSceneManager() const = 0;

	/// 获取网络连接
	virtual IConnection* getNetConnection() const = 0;

	/// 获取效果客户端
	virtual IEffectClient* getEffectClient() const = 0;

	/// 获取Buff客户端
	virtual IBuffClient* getBuffClient() const = 0;

	/// 获取技能管理器
	virtual ISkillManager* getSkillManager() const = 0;

	/// 获取冷却系统
	virtual IFreezeClient* getFreezeClient() const = 0;

	/// 获取采集系统
	virtual ICollectionClient* getCollectionClient() const = 0;

	/// 获取聊天客户端
	virtual IChatClient* getChatClient() const = 0;

	/// 获取通用物品栏
	virtual ISkepClient* getSkepClient() const = 0;

	/// 获得Form管理器 
	virtual IFormManager* getFormManager() const = 0;

	/// 获得PK客户端
	virtual IPKModeClient* getPKModeClient() const = 0;

	/// 获得组队客户端
	virtual ITeamClient* getTeamClient() const = 0;

	/// 获得好友客户端
	virtual IFriendClient* getFriendClient() const = 0;

	/// 获得摆摊客户端
	virtual IStallClient* getStallClient() const = 0;

	/// 获得家族客户端
	virtual IFamilyClient* getFamilyClient() const = 0;

	/// 获得军团客户端
	virtual IArmyClient* getArmyClient() const = 0;

	/// 
	virtual IGoodsFunctionClient* getGoodsFunctionClient() const = 0;

	/// 获得交易客户端
	virtual ITradeClient* getTradeClient() const = 0;

    //  获得任务客户端；
    virtual ITaskClient* GetTaskClient() const = 0;

	// 客户端配置，类似保存用户名、屏幕分辨率等等
	virtual IConfigManager* getConfigManager() const = 0;

	//读取服务器列表
	virtual IServerListConfig * getServerListConfig() const = 0;

	//退出登录
	virtual void Logout(void) = 0;

	//复位游戏逻辑相关的组件，如ITaskClient , IFriendClient等等等等，用于重新选择服务器
	virtual bool ResetCompentsForSelectServer(void) = 0;

	//复位游戏逻辑相关的组件，如ITaskClient , IFriendClient等等等等，用于重新选择角色
	virtual bool ResetCompentsForSelectActor(void) = 0;

	//获取进度管理器
	virtual IProgressManager * getProgressManager() const = 0;

	//获取硬件鼠标消息回调
	virtual IHardwareCursorCallBack* getHarewareCursorCallBack() const = 0;

#ifdef MEM_LEAK_TEST
	virtual void CreateEntityView(EntityView* entityView, EMemCreateType type) const = 0;	
	virtual void ReleaseEntityView(EntityView* entityView) const = 0;	
#endif

};

#define gGlobalClient	((IGlobalClient*)::xs::getGlobal())



#endif // __IGLOBALCLIENT_H__