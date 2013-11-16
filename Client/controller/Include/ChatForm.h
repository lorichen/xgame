/******************************************************************
** 文件名:	ChatForm.h
** 版  权:	(C)  
** 
** 日  期:	2008-3-21
** 版  本:	1.0
** 描  述:	聊天控制			
** 应  用:  
******************************************************************/
#pragma once

enum 
{
	CheckChannel_Map = 1,	    /// 地图频道
	CheckChannel_Around,		/// 周围频道
	CheckChannel_Army,		    /// 军团频道
	CheckChannel_Family,		/// 世家频道
	CheckChannel_Team,		    /// 队伍频道
	CheckChannel_Private,	    /// 密语频道
	CheckChannel_All,	        /// 全选频道

	MaxCheckChannelCount,	    /// 最大频道数
};
class ChatForm : public SingletonEx<ChatForm>,public IEventExecuteSink
{
	ulong			m_currentChannel;		//当前的频道
	ulong			m_currentTarget;		//当前的聊天目标
	bool			m_isSubscriber;
	/************************************************************************/
	/* event define                                                         */
	/************************************************************************/
public:
	static const std::string		EventOnChatMessageUpdate;	//更新聊天信息
	static const std::string		EventOnChannelChange;		//改变当前聊天频道
	static const std::string		EventOnTargetChange;		//当聊天对象改变,私聊
	static const std::string		EventOnSetChannel;			//通知UI当前频道,其实初始化输入框子提示部分
	static const std::string        EventOnUpdateChatBoxSize;   //更新聊天窗的大小
	static const std::string        EventOnSetChatBgShow;       //显示出聊天背景

public:
	ChatForm();
	~ChatForm();

	//  事件订阅
	void Subscriber();

	// 取消订阅处理
	void UnSubscriber();

	// Update
	void Update();

	/** 发送聊天消息
	*/
	void sendChatMessage(const char* msg);

	/**设置当前的频道
	*/
	void setChannel(const std::string& channelName);

	/**设置当前屏蔽的频道
	*/
	void setChannelFilter(int nIndex,bool bSelect,bool bAllSelect = false);

	/**设置当前聊天窗口的原始大小
	*/
	void SetChatBoxInitSize(int right,int top);

	/**
	*/
    void UpdateChatBoxSize(int nPosX,int nPosY);

	/** 得到鼠标是否按下
	*/
	bool GetMousePressed();

	/** 设置鼠标是否按下
	*/
	void SetMousePressed(bool bmousePressed);

	/** IEventExecuteSink
	*/
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);
	/************************************************************************/
	/* event handle                                                         */
	/************************************************************************/

	void onChatMessageUpdate(EventArgs& args);
	void onChannelChange(EventArgs& args);		//改变当前聊天频道
	void onTargetChange(EventArgs& args);		//当聊天对象改变,私聊
	void onSetChannel(EventArgs& args);			//通知UI当前频道,其实初始化输入框子提示部分

private:

	 bool     m_bCheck[MaxCheckChannelCount];

	 int      m_nRight;

	 int      m_nTop;

	 bool     m_bMousePressed;
};
