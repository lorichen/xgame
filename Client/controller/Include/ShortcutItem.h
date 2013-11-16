/******************************************************************
** 文件名:	ShortcutItem.h
** 版  权:	(C)  
** 
** 日  期:	2008-4-9
** 版  本:	1.0
** 描  述:	快捷栏目ITem			
** 应  用:  
******************************************************************/
#pragma once

class ShortcutItem
{
public:
    //  Item类型；
    enum EItemType
    {
        EIT_LOWER_BOUND = -1,

        EIT_Skill,
        EIT_NO_Skill,

		EIT_RIDEPET,   // 骑宠 zgz

        EIT_UPPER_BOUND,
    };

public:
	ShortcutItem(int pos, EItemType type, ulong id, int index);
	ShortcutItem(ShortcutItem* item);
	ShortcutItem();
	~ShortcutItem(){}

	/************************************************************************/
	/* operate functions                                                    */
	/************************************************************************/
public:
	void setPos(int pos)	
	{ 
		m_pos		= pos;	
	}

	void setType(EItemType type)
	{ 
		m_eItemType = type; 
	}

	void setID(ulong id)
	{ 
		m_id		= id;	
	}

	void setIndex(const int& idx)
	{ 
		m_index = idx;
	}

	//设置(wzh09.12.08)
	void setUserUid(LONGLONG uid)
	{
		m_lUserUid = uid;
	}

	void setShine() 
	{
		m_isShine = true;
	}

	const int getPos() const 
	{ 
		return m_pos;
	}

	const EItemType getType() const 
	{ 
		return m_eItemType;
	}

	const int getIndex() const 
	{ 
		return m_index;
	}

	// 得到使用者UID(WZH.09.12.08)
	const LONGLONG getUserUid() const
	{
		return m_lUserUid;
	}

	ulong getID();

	ulong getSubID();

	ulong getCoolingId();

	std::string getTooltip();

	void cooling();

	void ShortcutItemDragCooling(void);

	void ShortcutEndCooling(/*int nPlace*/);

	void GetCoolingInfo(ulong& uCoolingTime, ulong& uLiveTime);

	void clear();

    inline bool IsValidItem() const
    {
        return ((m_eItemType > EIT_LOWER_BOUND) && (m_eItemType < EIT_UPPER_BOUND));
    }

	
	/************************************************************************/
	/* Item事件                                                             */
	/************************************************************************/
public:
	void Use();
	void setEnable(bool enable);
	bool getEnable() const 
	{ 
		return m_enable;
	}

	void Init();

	void onUpdate(bool bPetSkill = false);//更新

	void select();
	void deselect();

	// add by zjp；
	void SetPetSkill(bool bPetSkill){m_bPetSkill = bPetSkill;}
	bool IsPetSkill(){return m_bPetSkill;}
	void SetPetSkillPetIndex(int nSkillPetIndex)
	{
		m_nSkillPetIndex = nSkillPetIndex;
	}
	int  GetPetSkillPetIndex()
	{
		return m_nSkillPetIndex;
	}

protected:
    int		m_pos;		//索引位置,所有快捷功能槽统一管理,统一编号
    EItemType m_eItemType;		//技能还是物品 0---技能 1--非技能 2--骑宠 -1 无效
    ulong	m_id;		//物品ID或者技能主ID
    bool	m_enable;	//能不能使用
    int		m_index;	//物品索引或者技能索引
    bool	m_isShine;  //闪动
	LONGLONG m_lUserUid; // 快捷栏技能对应的技能的使用者UID(WZH 09.12.08); 对于骑宠则记录下骑宠的UID(zgz 2011.01.26)

	// add by zjp；区分是否为宠物技能
	bool    m_bPetSkill; // 是否为宠物技能 
	int     m_nSkillPetIndex; // 技能对应的宠物位置索引
	string  m_sToolTip;	 // 提示
};
