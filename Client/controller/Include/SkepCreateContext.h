/******************************************************************
** 文件名:	SkepCreateContext.h
** 版  权:	(C)  
** 
** 日  期:	2008-3-19
** 版  本:	1.0
** 描  述:	物品栏信息包装			
** 应  用:  
******************************************************************/
#pragma once

class SkepCreateContext
{
	std::string m_skepTitle;					// 物品栏的名称
	std::string m_customInfo;					// 客户信息,保存到UI物品栏信息里边
	ulong		m_ulSkepID;						// 物品篮ID
	ulong		m_ulSkepType;					// 物品篮类型
	ulong		m_ulSkepName;					// 物品篮名字
	ulong		m_ulMaxSize;					// 物品篮大小
	ulong		m_ulUsableBags;					// 可能行囊
	std::string m_UnifiedPosition;				// 窗口显示参考位置("{{0,0},{0,0}}")

public:
	SkepCreateContext(SBuildSkepClientContext* pContext):
	  m_skepTitle(pContext->szSkepName),
	  m_ulSkepID(pContext->dwSkepID),
	  m_ulSkepType(pContext->dwSkepType),
	  m_ulSkepName(pContext->dwSkepName),
	  m_ulMaxSize(pContext->dwMaxSize),
	  m_ulUsableBags(pContext->dwUsableBags)
	  {
		  char szBuf[256];
		  sprintf(szBuf,"%d",m_ulSkepID);
		  m_customInfo = szBuf;
	  }
	  SkepCreateContext(const std::string& skepTitle,ulong skepID,ulong skepType,ulong skepName,ulong maxSize,ulong usableBags):
	  m_skepTitle(skepTitle),
	  m_ulSkepID(skepID),
	  m_ulSkepType(skepType),
	  m_ulSkepName(skepName),
	  m_ulMaxSize(maxSize),
	  m_ulUsableBags(usableBags)
	  {

	  }
	  ~SkepCreateContext(){}

	  void setUnifiedPosition(const std::string& strUnifiedPosition) { m_UnifiedPosition = strUnifiedPosition; }

	  const std::string& getUnifiedPostion() const { return m_UnifiedPosition; }

	
	  const std::string&		getSkepTitle()	const	{ return m_skepTitle;}
	  const unsigned long&		getSkepSize()	const	{ return m_ulMaxSize;}
	  const std::string&		getCustomInfo()	const	{ return m_customInfo;}
	  const unsigned long&		getUsableBags()	const	{ return m_ulUsableBags;}

	  const bool isPacket() const { return m_ulSkepName == ESkepName_Packet;}	// 包裹篮
	  const bool isEquip() const { return m_ulSkepName == ESkepName_Equip;}		// 装备篮
	  const bool isLib() const { return m_ulSkepName == ESkepName_WareHouse;}	// 仓库篮
	  const bool isTrade() const { return m_ulSkepName == ESkepName_Trade;}		// 交易篮
	  const bool isStall() const { return m_ulSkepName == ESkepName_Stall;}		// 摊位篮
	  const bool isShop() const { return m_ulSkepName == ESkepName_Shop;}		// 商店篮
	  const bool isHock() const { return m_ulSkepName == ESkepName_Hock;}		// 拍卖篮
	  const bool isBox() const { return m_ulSkepName == ESkepName_Box;}			// 宝箱篮
	  const bool isAuction() const { return m_ulSkepName == ESkepName_Auction;}	// 寄售篮
	  const bool isPet() const {return m_ulSkepName == ESkepName_Pets;}         // 宠物蓝
	  const bool isPetLib() const {return m_ulSkepName == ESkepName_PetLib;}    // 宠物仓库篮
	  const bool isEquipSmeltSoul() const { return m_ulSkepName == ESkepName_EquipSmeltSoul; }	// 装备炼魂物品篮
	  const bool isEquipEmbedGems() const { return m_ulSkepName == ESkepName_EquipEmbedGems; }	// 装备灵石镶嵌物品篮
	  const bool isEquipRebuild() const { return m_ulSkepName == ESkepName_EquipRebuild; }		// 装备重铸物品篮
	  const bool isComposePill() const { return m_ulSkepName == ESkepName_Compose; }// 内丹合成  //by cjl ,达勇没有上传，我补一下
	  const bool isPetIntensify() const {return m_ulSkepName == ESkepName_Intensify;}	//魂将圣化 by ldy
};