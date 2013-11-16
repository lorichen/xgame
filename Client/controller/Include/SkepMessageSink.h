/******************************************************************
** 文件名:	SkepMessageSink.h
** 版  权:	(C)  
** 
** 日  期:	2008-3-19
** 版  本:	1.0
** 描  述:	物品栏管理器			
** 应  用:  
******************************************************************/
#pragma once

class SkepMessageSink: public SingletonEx<SkepMessageSink>,public IMessageExecuteSink
{
	// 当前的模型ID
	ulong			m_curAniID;

	// 当前查看的性别
	ulong           m_ulSex;

	// 当前查看的模型的裸模ID
	ulong           m_ulNakedResId;

	/************************************************************************/
	/* event define                                                         */
	/************************************************************************/
public:
	static const std::string  EventCreateSkep;  //通知创建物品栏
	static const std::string  EquipInfo;		//查看它人装备显示栏
	static const std::string  PropInfo;			// 查看属性资料

	/************************************************************************/
	/* constructor and deconstructor                                       */
	/************************************************************************/
public:
	SkepMessageSink();
	~SkepMessageSink();
	/************************************************************************/
	/* interface function                                                   */
	/************************************************************************/
	/** 
	@param   dwMsgID ：消息ID
	@param   pszMsg ：消息结构
	@param   nLen ：消息长度
	@return  
	*/
	virtual void OnExecute(DWORD dwMsgID, SGameMsgHead * pGameMsgHead, LPCSTR pszMsg, int nLen);
	// 旋转
	void         RotatePersonView(float fAngle);

	//
	void		OnClose();
};