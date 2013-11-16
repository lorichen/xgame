/******************************************************************
** 文件名:	ProgressBar.h
** 版  权:	(C)  
** 
** 日  期:	2008-4-8
** 版  本:	1.0
** 描  述:	进度条			
** 应  用:  
******************************************************************/
#pragma once

/** 进度结束的情况下
*/
struct IProgressSink
{
	virtual void done() = 0;
};

class ProgressBar : public SingletonEx<ProgressBar>, public IEventVoteSink
{
	/************************************************************************/
	/* event define                                                         */
	/************************************************************************/
	static const std::string		EventOnProgressEnter;   //show
	static const std::string		EventOnProgressLeave;	//hide
	static const std::string		EventOnProgresUpdate;   //update progress

	ulong			m_startTime;		//
	ulong			m_preTime ;         //previous update time
	ulong			m_delay;			//更新的频率
	ulong			m_total;			//进度总时间
	ulong			m_init;				//开始点,
	bool			m_working;			//进度条工作
    //  是否反转表示；
    bool m_bInverse;

	bool			m_autoDelete;       //自动删除

	IProgressSink*  m_pSink;
public:
	ProgressBar();
	~ProgressBar(){}

	//初始化一些事件等等
	void create();

	//  事件订阅
	void Subscriber();
	// 取消订阅处理
	void UnSubscriber();

	//代替时钟
	void update() ;
	
	/************************************************************************/
	/* event handler                                                        */
	/************************************************************************/

	void onProgressEnter(EventArgs& args);   //show
	void onProgressLeave(EventArgs& args);	//hide
	void onProgresUpdate(EventArgs& args);   //update progress

	/************************************************************************/
	/* operater functions                                                   */
	/************************************************************************/
	/** 设置进度
	@ProgressBarInitArgs 总时间
	@pSink 进度完成后调用
	@slapse 开始点,因为可能不是从起点开始,默认为0,start/time
	*/
	void setProgress(const CProgressBarInitArgs& ProgressBarInitArgs, IProgressSink* pSink = 0, ulong slapse = 0, bool autoDelete = false);

	/** 开启进度
	*/
	void start();

	/** 关闭进度
	*/
	void stop();
	
	//////////////////////////IEventVoteSink/////////////////////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  如果返回false : 则中断执行，否则继续向下执行
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool	OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

};