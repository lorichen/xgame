//==========================================================================
/**
* @file	  : GlobalClient.h
* @author : 
* created : 2008-1-14   15:51
* purpose : 客户端全局对象实现
*/
//==========================================================================

#ifndef __GLOBALCLIENT_H__
#define __GLOBALCLIENT_H__

#include "IGlobalClient.h"

class GlobalClient : public IGlobalClient
{
	Timer			mTimer;					/// 时钟
	std::string		mWorkDir;				/// 工作目录
	void*			mhWnd;					/// 窗口句柄
	IFileSystem*	mFileSystemRoot;		/// 目录态文件系统，指向工作目录
	uint			mTimeStampServer;		/// 服务器同步过来的时间戳
	uint			mTimeStampClient;		/// 服务器时间戳同步到客户端时的tick
	handle			mPlayer;				/// 玩家实体
	handle			mTarget;				/// 玩家选择的目标实体

	// 3D
	IRenderEngine*	mRenderEngine;			/// 渲染引擎
	IFont*			mFonts[maxFontCount];	/// 系统预置字体
	Viewport*		mViewport;
	Camera*			mCamera;
	Light*			mLight;

	// service
	//MessageDispatch		mMsgDispatch;			/// 消息分发器
	IResourceManager*	mResourceManager;		/// 资源管理器
	IEntityFactory*		mEntityFactory;			/// 实体工厂
	ISceneManager2*		mSceneManager;			/// 场景管理器
	IEventEngine*		mEventEngine;			/// 事件服务
	ISchemeEngine*		mSchemeEngine;			/// 配置服务
	//IConnection*		mNetConnection;			/// 网络连接
	IEntityClient*		mEntityClient;			/// 实体管理
	//IEffectClient*		mEffectClient;			/// 效果客户端
	//IBuffClient*		mBuffClient;			/// Buff客户端
	//ISkillManager*		mSkillManager;			/// 技能管理器
	//IFreezeClient*		mFreezeClient;			/// 冷却系统
	//IChatClient*		mChatClient;			/// 聊天客户端
	//ISkepClient*		mSkepClient;			/// 通用物品栏
	//IFormManager*		mFormManager;			/// 控制器 
	//IPKModeClient*		mPKModeClient;			/// PK客户端
	//ITeamClient*		mTeamClient;			/// 组队客户端
	//IFriendClient*		mFriendClient;			/// 好友客户端
	//IGoodsFunctionClient*	mGoodsFunctionClient;
	//ITradeClient*		mTradeClient;			/// 交易客户端
	IConfigManager*		mConfigManager;			/// 客户端配置
	IServerListConfig*  mServerListConfig;      /// 服务器列表配置
	IAudioEngine*       mAudioEngine;           /// 声音
    //  任务管理器；
	//ITaskClient* m_pTaskClient;

	TimerAxis*			mTimerAxis;				/// 时间轴
	ILuaEngine*			mLuaEngine;				/// Lua引擎

	//TxGuiRenderer*		mGuiRenderer;			/// GUI渲染器

		
	//IProgressManager *	mProgressManager;		//进度管理器


	//GameForm*			frmGame; // TODO remove!!!

public:
	GlobalClient();
	virtual ~GlobalClient();

	bool create(void* hwnd);
	void close();

	Viewport* getViewPort(){return mViewport;}
	Camera*	  getCamera(){return mCamera;}
	// IGlobalClient
public:
	/// 获取工作目录(exe所在目录)
	virtual const std::string& getWorkDir() const
	{ return mWorkDir; }

	virtual void* getHWND() const
	{ return mhWnd; }

	/// 获取目录态顶层文件系统
	virtual IFileSystem* getFileSystemRoot() const
	{ return mFileSystemRoot; }

	/// 获取时间戳
	virtual ulong getTimeStamp() const
	{ return (mTimer.getTime() - mTimeStampClient + mTimeStampServer); }

	/// 设置时间戳
	virtual void setTimeStamp(ulong ts)
	{ mTimeStampServer = ts; mTimeStampClient = mTimer.getTime(); }

    virtual ulong getTimeSecond() const
    {
        return 0;
    }
    
	/// 获取主角
	virtual handle getPlayer() const
	{ return mPlayer; }

	/// 设置主角
	virtual void setPlayer(handle entity)
	{ mPlayer = entity; }

	/// 获取目标实体对象
	virtual handle getTarget() const
	{ return mTarget; }

	/// 设置目标实体对象
	virtual void setTarget(handle entity)
	{ mTarget = entity; }


	// -----------------------服务模块----------------------
	/// 声音
	virtual IAudioEngine* getAudioEngine() const
	{
		return mAudioEngine;
	}
	/// 获取渲染引擎
	virtual IRenderEngine* getRenderEngine() const
	{ return mRenderEngine; }

	/// 获取渲染系统
	virtual IRenderSystem* getRenderSystem() const
	{ return mRenderEngine ? mRenderEngine->getRenderSystem() : 0; }

	/// 获取默认字体
	virtual IFont* getFont(int type = fontDefault) const;
    
    
    virtual IEventEngine* getEventEngine() const
	{ return mEventEngine; }
    
	virtual ISchemeEngine* getSchemeEngine() const
	{ return mSchemeEngine; }
    
	virtual TimerAxis* getTimerAxis() const
	{ return mTimerAxis; }
    
    
    virtual IEntityClient* getEntityClient() const
	{ return mEntityClient; }
    
    
    virtual ILuaEngine* getLuaEngine() const
	{ return mLuaEngine; }
    
    
    virtual IMessageDispatch* getMessageDispatch() const
	//{ return const_cast<IMessageDispatch*>(static_cast<const IMessageDispatch*>(&mMsgDispatch)); }
	{
		return 0;
	}
    
	virtual IInputMessageManager* getInputMessageManager() const
	//{ return static_cast<IInputMessageManager*>(InputMessageManager::getInstancePtr()); }
	{
		return 0;
	}

	virtual IResourceManager* getResourceManager() const
	{ return mResourceManager; }

	virtual IEntityFactory* getEntityFactory() const
	{ return mEntityFactory; }

	virtual ISceneManager2* getSceneManager() const
	{ return mSceneManager; }

	
	virtual IConnection* getNetConnection() const
	{
        return 0;//mNetConnection;
    }
	
	virtual IEffectClient* getEffectClient() const
	{return 0;}//{ return mEffectClient; } 

	virtual IBuffClient* getBuffClient() const
	{return 0;}//{ return mBuffClient; }

	virtual ISkillManager* getSkillManager() const
	{return 0;}//{ return mSkillManager; }

	virtual IFreezeClient* getFreezeClient() const
	{return 0;}//{ return mFreezeClient; }
    
    virtual ICollectionClient* getCollectionClient() const
    {return 0;}

	virtual IChatClient* getChatClient() const
	{return 0;}//{ return mChatClient; }

	virtual ISkepClient* getSkepClient() const
	{return 0;}//{ return mSkepClient;}

	virtual IFormManager* getFormManager() const
	{return 0;}//{ return mFormManager;}

	virtual IPKModeClient* getPKModeClient() const
	{return 0;}//{ return mPKModeClient; }

	virtual ITeamClient* getTeamClient() const
	{return 0;}//{ return mTeamClient; }

	virtual IFriendClient* getFriendClient() const
	{return 0;}//{ return mFriendClient; }
    
    virtual IStallClient* getStallClient() const {return 0;}
    
    virtual IFamilyClient* getFamilyClient() const {return 0;}
    
    virtual IArmyClient* getArmyClient() const {return 0;}

	virtual IGoodsFunctionClient* getGoodsFunctionClient() const
	{return 0;}//{ return mGoodsFunctionClient; }

	virtual ITradeClient* getTradeClient() const
	{return 0;}//{ return mTradeClient; }

    virtual ITaskClient* GetTaskClient() const
    {return 0;}//{return m_pTaskClient;}

	virtual IConfigManager* getConfigManager() const
	{return 0;}//{return mConfigManager;}

	virtual IServerListConfig * getServerListConfig() const
	{return 0;}//{return mServerListConfig;}

	//退出登录
	virtual void Logout(void);

	//复位游戏逻辑相关的组件，如ITaskClient , IFriendClient等等等等，用于重新选择服务器
	virtual bool ResetCompentsForSelectServer(void);

	//复位游戏逻辑相关的组件，如ITaskClient , IFriendClient等等等等，用于重新选择角色
	virtual bool ResetCompentsForSelectActor(void);

	//获取进度管理器
	virtual IProgressManager * getProgressManager() const
	{
		return 0;
		//return mProgressManager;
	}

    virtual IHardwareCursorCallBack* getHarewareCursorCallBack() const {return 0;}
private:
	bool init3D(void* hwnd);
	bool initUI();

};


#endif // __GLOBALCLIENT_H__