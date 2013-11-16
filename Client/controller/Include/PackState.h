/******************************************************************
** 文件名:	PackState.h
** 版  权:	(C)  
** 
** 日  期:	2008-3-31
** 版  本:	1.0
** 描  述:	物品栏状态			
** 应  用:  
******************************************************************/
#pragma once

class PackState
{
	bool	m_isOpen;
	UID		m_uidNpc;	
	ulong	m_ulPackID;

	bool	m_observed; //

	bool	m_isMine;

	std::list<int> m_lUnits;

public:
	PackState(ulong ulPacked,bool isMine):m_observed(false),m_isMine(isMine){ m_ulPackID = ulPacked;m_lUnits.clear();}
	~PackState(){deselectAll();};

	/************************************************************************/
	/* 外部状态调用                                                         */
	/************************************************************************/
public:
	/** 判断指定的包是否打开（只显示）
	*/
	bool isPackOpen(){ return m_isOpen; }
	

	void setObserved(){ m_observed = true;}
	bool isObserved() { return m_observed;}

	bool isMine() {return m_isMine;}

	/** 设置关闭状态
	@param isOpen 设置状态
	*/
	void setPackState(bool isOpen){ m_isOpen = isOpen; }
	void setOwner(const UID uid){ m_uidNpc = uid;}


	const UID& getOwner() const { return m_uidNpc;}

	/** 选择项
	*/
	void select(int iPlace);

	/** 取消选择
	*/
	void deselect(int iPlace);

	/** 取消选择所有
	*/
	void deselectAll();

	void cooling(const ulong& id);
	std::string getTooltip(const ulong& id);

	void updateTooltip();
	void updateTooltip(LONGLONG uid);//更新包裹篮里面某一个物品的tooltip
};
