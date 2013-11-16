/******************************************************************
** 文件名:	TooltipHelper.h
** 版  权:	(C)  
** 
** 日  期:	2008-4-16
** 版  本:	1.0
** 描  述:	tooltip helper			
** 应  用:  
******************************************************************/
#pragma once

struct SEquipmentViewInfo;
struct SMsgTeamViewTeamMemberData_SC;
struct SEventTeamViewMemberData_C;

enum ShopType
{
	NonShop = 0,	// 不是商店
	ShopCommon,		// 商店通用
	ShopReclaim,	// 商店回购
	ShopMax,		
};

//颜色定义
#define TT_COLOR_WHITE      "#FFFFFF"
//#define TT_COLOR_RED		"#FF0000"
//#define TT_COLOR_GREEN		"#00FF00"
//#define TT_COLOR_BLUE		"#0000FF"
//#define TT_COLOR_PURPLE		"#8080FF"
//#define TT_COLOR_ORANGE		"#FFFF00"
#define TT_COLOR_GOLDYELLOW "#FFFF80"
#define TT_COLOR_YELLOW		"#FFFF80"
#define TT_COLOR_SMALT		"#0080FF"
#define TT_COLOR_HOAR		"#EEEEEE"

//定义装备显示需要的颜色
#define TT_COLOR_CYAN			"#00FF89"
#define TT_COLOR_BLUE			"#0056FF"
#define TT_COLOR_GOLDEN			"#FFDE03"
#define TT_COLOR_DARK_GLODEN	"#B5973B"
#define TT_COLOR_ORANGE			"#FF7100"
#define TT_COLOR_FRESH_GREEN	"#00FF00"
#define TT_COLOR_GREEN			"#96E24C"
#define TT_COLOR_RED			"#FF001E"
#define TT_COLOR_PINK			"#FF69FF"
#define TT_COLOR_PURPLE			"#D300ED"
#define TT_COLOR_GRAY			"#A09F92"

class TooltipHelper
{
public:
	static const std::string HtmlStart;
	static const std::string HtmlEnd;
	static const std::string strVocation[PERSON_VOCATION_MAX+1];
	static const std::string strEquipClass[EQUIPMENT_SUBCLASS_MAX];
	static const std::string strEquipBrandColor[GOODS_BRAND_LEVEL_MAX_LEVEL];
	static const std::string strEquipBrandName[GOODS_BRAND_LEVEL_MAX_LEVEL];
	static const std::string strEquipSmeltSoulMilesStoneName[EQUIP_SMELT_SOUL_ATTRIB_MAX_NUM];
	static const std::string strPetPropType[2]; // 战士；法师型；
	static const std::string strPetType[2]; // 战士；法师型；
	static const std::string strPetGradeType[5]; // 一到五档次；
	

	// 外部通用接口
	static std::string getTooltip(IEntity *pEntity,ShopType isShop = NonShop,bool isCooling = false);

	//buff tool tip
	static std::string getBuffTooltip(IBuff *pBuf);

	/************************************************************************/
	/* 非装备物品的提示														*/
	/************************************************************************/
	static std::string getGoodsTooltip(IGoods *pEntity,ShopType isShop = NonShop,bool isCooling = false);

    //  获取指定ID的物品的提示信息；
    //  注意：这是一个临时实现，是为了测试在任务系统的相关界面中，能否正常显示任务物品的提示信息；
    //  完善的处理待物品系统的需求明确后再进行实现；
	static std::string getGoodsTooltip(long lGoodsID,DWORD flag = 0);

	//需要显示的东西与TooltipHelper::getGoodsTooltip 不完全相同
	//这里根据需要自己构造
	//NOTE：传进来的iOnlineShopGoodsID 是商城的商品ID
	static std::string getOnlineShopGoodsTooltip(int iOnlineShopGoodsID);

	//获取简化的提示信息
	//NOTE：传进来的iOnlineShopGoodsID 是商城的商品ID
	static std::string getOnlineShopGoodsShortCutTooltip(int iOnlineShopGoodsID, float fhtmlTableWidth);

	// 骑宠物品的提示信息
	static std::string getRidePetGoodsTooltip(IGoods *pEntity);

	/************************************************************************/
	/* 装备物品的提示														*/
	/************************************************************************/
	static std::string getEquipTooltip(SEquipmentSchemeInfo* pEquipInfo);

	static std::string getEquipTooltip(IEquipment *pEntity,ShopType isShop = NonShop,bool isCooling = false);

	// 行囊的提示信息，行囊属于装备，但是它的信息需要重新定制
	static std::string getBagTooltip(IEquipment *pEntity,ShopType isShop,bool isCooling);

	//normal equipment tooltip
	static std::string getCommonEquipTooltip(IEquipment *pEntity,ShopType isShop,bool isCooling);

	/************************************************************************/
	/* 宠物相关提示															*/
	/************************************************************************/
	static std::string getPetSkillBookToolTip(IGoods *pEntity,ShopType isShop = NonShop,bool isCooling = false);

	static std::string getPetSkillBookToolTip(long lGoodsID);
    
	static std::string getPetToolTip(long lPetID);

	static std::string getPetTooltip(IPet *pEntity,bool isRelease = false);

	static std::string getPetTooltipRelease(IPet *pEntity,bool bRelease,int nTime,int nTimeLeaving);

	static std::string getRidePetTooltip(IPet* pEntity);

	static std::string getPetinIntensifyToolTip(IPet* pEntify, int increaSanctify, int nowStart);

	/************************************************************************/
	/* 任务模块															*/
	/************************************************************************/
	// 获取指定ID奖励的提示信息；
	static std::string getPriseToolTip(long lGoodsID,DWORD flag = 0);

	/************************************************************************/
	/* 聊天模块的特殊tooltip												*/
	/************************************************************************/
	static std::string getEquipTooltipInChatList(IEquipment *pEntity,ShopType isShop = NonShop,bool isCooling = false);

	static std::string getGoodsTooltipInChatList(IGoods *pEntity,ShopType isShop = NonShop,bool isCooling = false);

	/************************************************************************/
	/* 技能模块的tooltip													*/
	/************************************************************************/
	// 获取指定等级的Tip,模拟加天赋时，没按确定前，角色其实还没有学会相应等级的技能
	// 添加lv参数，指要获取的等级信息
	static std::string getSkillTooltip(const SkillId& id, int lv=0, int tempGeniusPoint=0);

	// 等级小于10级时，天赋切页的tip内容
	static std::string GetGeniusPageTooltipText();
	
	// 技能放在快捷栏时的提示
	static std::string getSkillShortcutTip(const SkillId& id);

	static std::string getPetSkillToolTip(IPet *pEntity,const SkillId& id);
	static std::string getPetSkillShortCutTip(IPet *pEntity,const SkillId& id);

	// 阵法技能的提示
	static std::string getPosSkillToolTip(int nID);
	/************************************************************************/
	/* pk模块的tooltip                                                      */
	/************************************************************************/
	static std::string getPKModeToolTip(int nMode, bool isActive);

	/************************************************************************/
	/* html的特殊定制														*/
	/************************************************************************/
	static std::string GetEquipToolTipByLinkInfo(const std::string & linkinfo);

	static std::string getGoodsTooltipByLinkInfo(const std::string & linkinfo);

	static std::string parseDesc(const char* pDesc);

	static std::string FormatHtmlEquipInfo(SEquipmentViewInfo *pInfo);

	/************************************************************************/
	/* 内部功能函数															*/
	/************************************************************************/
	static std::string getProfessionName(int nProfession);	

	static bool getGemsDamageProp(long gemsid, long & propid, long val);

	static std::string GetCombatHtmlInfo(SMsgActionPropInfo *pViewInfo);

public:
	static int s_fontWidth;	
	
	static const string s_imageGold,s_imageSilver,s_imageCopper;

private:
	static float GetPkNameColorBase();

	static std::string	 GetBindDescribe(IEntity* pEntity);

	static std::string   GetBindDescribebyID(int goodid,int blandflag);

	static bool  HasBindFlage(int blandflag,int flag);

	static bool  HasGoodsFlag(int goodflag,int flag);

	static std::string GetGoodSubClass(int nSubClass);

	static std::string  GetUseTerm(long lTime);

public:
	static std::string  GetGoodPrice(long lPrice);
};