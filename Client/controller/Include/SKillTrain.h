/******************************************************************
** 文件名:	SKillTrain.h
** 版  权:	(C)  
** 
** 日  期:	2008-4-22
** 版  本:	1.0
** 描  述:	技能学习			
** 应  用:  
******************************************************************/
#pragma once

class SKillTrain : public SingletonEx<SKillTrain>
{
	struct _SkillItem_
	{
		SkillId id;
		std::string name;
		int maxLevel; //最高级别
	};

	/************************************************************************/
	/* event define                                                          */
	/************************************************************************/
	static std::string EventOnSkillTrainEnter; //打开技能窗口
	static std::string EventOnSkillTrainLeave; //关闭技能窗口
	static std::string EventOnAddSkillTainItem;//添加技能项
	static std::string EventOnUpdateSkillTainItem;//更新技能项
	static std::string EventOnDeleteSkillTrainItem;//删除技能相
	static std::string EventOnSelectSkillTrainItem;//选择技能项，包括选择和取消选择

	typedef std::vector<_SkillItem_>			LTrainSkillList;

	LTrainSkillList							m_vlist;

	int		m_currentCurrent;

public:
	SKillTrain();
	~SKillTrain();


	void create();

	void open();
	void close();

	void select(ulong idx);

	void train();

	void readSkillList();


	void upgrade(const ushort& mainId);

	/************************************************************************/
	/* event handler                                                         */
	/************************************************************************/
	void onSkillTrainEnter(EventArgs& args); //打开技能窗口
	void onSkillTrainLeave(EventArgs& args); //关闭技能窗口
	void onAddSkillTainItem(EventArgs& args);//添加技能项
	void onDeleteSkillTrainItem(EventArgs& args);//删除技能相
	void onSelectSkillTrainItem(EventArgs& args);//选择技能项，包括选择和取消选择
	void onUpdateSkillTainItem(EventArgs& args);

};