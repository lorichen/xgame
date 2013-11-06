#ifndef __ChangePartManager_H__
#define __ChangePartManager_H__

#include "ISchemeCenter.h"

class VisualComponentMZ;

struct ChangePartCallBackContext
{
	ModelNode* pNode;//如果为0，表示取消节点
};

struct IChangePartCallBack
{
	virtual void onChangePart(ChangePartCallBackContext & context)=0;
};

class ChangePartManager
{
public:
	//逻辑相关
	void changePart(SPersonMainPartChangeContext & context);//更换部件
	void showPart( ulong ulPart, bool bShow);//显示部件
	void setPartScale(ulong part, float scale);//设置部件的缩放
	void setPartColor(ulong	part, const ColorValue& color);//设置部件颜色
	void setBodyColor(const ColorValue & color);//设置身体颜色
	const ColorValue & getBodyColor();//获取身体颜色
	void update();//更新

	//资源相关
	void setCanRequestRes(bool canRequest);//设置能否申请资源
	void requestRes(int priority);//申请资源
	void releaeRes();//释放资源

	//状态
	EWeaponClass getWeaponClass();//获取装备的武器类型
	enum WeaponPosition { WeaponPos_Hands, WeaponPos_Back, }; //武器位置
	void		setWeaponPos(WeaponPosition);//设置武器放置的位置
	void		showRibbonSystem(bool show);//显示飘带系统
	void		setWeaponAnimation(std::string ani);//设置武器动作
	bool		getPart(uint part, SEventPersonPerform_C& c);//获取部件
	bool		getPartShow(uint part, SEventPersonPerformShow_C& c);//获取部件的显示
	int			getCurrentCreatureId();//获取当前的生物资源id

public:
	ChangePartManager();
	void create(VisualComponentMZ* pOwner, IChangePartCallBack* pCallBack);
	~ChangePartManager();

private:
	//换装
	void changeArmet(SPersonMainPartChangeContext & context);//换头盔
	void changeSuitArmet(SPersonMainPartChangeContext & context);//换时装头盔
	void changeArmor(SPersonMainPartChangeContext & context);//换普通盔甲
	void changeSuitArmor(SPersonMainPartChangeContext& context);//换时装盔甲
	void changeWeapon(SPersonMainPartChangeContext & context);//换武器

	//控制部件显示
	void showArmet( bool bShow);	
	void showSuitArmet(bool bShow);
	void showBody( bool normal);//normal为true，表示普通装备，为false，表示时装
	void showWeapon( bool bShow);//变身时候需要屏蔽武器，by cjl

	//
	void onResLoad(ModelNode* pNode);
	
private:
	ulong getCurrentBody();//获取当前显示的主模型

private:
	//用于换装
	SPersonMainPartChangeContext	m_CurrentPart[EEntityPart_Max];//当前换装环境，时装盔甲是时装的身体，普通盔甲是普通装备的身体。
	bool							m_bShowPart[EEntityPart_Max];	//当前部件显示环境
	WeaponPosition					m_weaponBindPos;//武器的绑定位置
	bool							m_bShowRibbonSystem;//是否显示武器的飘带系统
	float							m_fWeaponScale;///武器的缩放
	ColorValue						m_vWeaponColor;//武器的颜色
	ColorValue						m_vArmetColor;//普通头盔的颜色
	ColorValue						m_vSuitArmetColor;//时装头盔的颜色
	ColorValue						m_vBodyColor;//身体颜色

	VisualComponentMZ*				m_pOwner;//所有者
	IChangePartCallBack*			m_pCallBack;//回调

	//用于资源的加载
	ulong							m_ulCurResId;//当前资源id,EntityViewImpl的resId,是时装盔甲和普通盔甲的默认资源id
	bool							m_bCanRequestRes;//能否申请资源
	ulong							m_ulNewResId;//资源id映射后产生的新资源id
	xs::handle						m_handle;//资源句柄
	bool							m_bLoaded;//资源是否已经加载
	ModelNode*						m_pModelNode;//当前节点
};


#endif