/******************************************************************
** 文件名:	ProgressForm.h
** 版  权:	(C)  
** 
** 日  期:	2008-3-19
** 版  本:	1.0
** 描  述:				
** 应  用:  
******************************************************************/
#pragma once


class ProgressForm :
	public FormBase
{
public:
	//static const std::string EventProgressStart;	//加载进度开始事件
	//static const std::string EventProgressEnd;		//进度结束

private:
	/************************************************************************/
	/* constructor and deconstructor                                        */
	/************************************************************************/
public:
	ProgressForm();
	~ProgressForm(); 
	/************************************************************************/
	/* general functions                                                    */
	/************************************************************************/
public:

	/************************************************************************/
	/* interface functions                                                  */
	/************************************************************************/
public:
	virtual void onMessage(ulong actionId, SGameMsgHead* head, void* data, size_t len);

	virtual FormType getFormType(){ return progressForm;}

	/** 设置为当前
	*/
	virtual void onEnter();

	/** 代替时钟
	*/
	virtual void update();

	virtual void render(IRenderSystem *pRenderSystem);


	/** 设置为无效,为切换作准备
	*/
	virtual void onLeave();
};