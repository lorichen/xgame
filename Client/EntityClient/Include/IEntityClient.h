/*******************************************************************
 ** 文件名:	e:\Rocket\Client\EntityClient\Include\IEntityClient.h
 ** 版  权:	(C)
 **
 ** 日  期:	2008/1/22  14:46
 ** 版  本:	1.0
 ** 描  述:	实体客户端
 ** 应  用:
 
 **************************** 修改记录 ******************************
 ** 修改人:
 ** 日  期:
 ** 描  述:
 ********************************************************************/
#pragma once


// 申明
struct IMessageVoteSink;
struct IMessageExecuteSink;
struct IEntityClass;
struct IEntity;
struct ICreature;
struct IPerson;
struct IMonster;
struct IGoods;
struct IEquipment;
struct INonequipableGoods;
struct IBox;
struct IMast;
struct IEntityPart;
struct IEntityClient;
class  EntityView;
struct ISchemeCenter;
struct SGameMsgHead;
struct IItemVisitor;
struct IEffect;
struct ISkepGoods;
struct IPet;
struct IPetPart;
/////////////////////////////消息投票sink///////////////////////////////
struct IMessageVoteSink
{
	/**
     @param   dwMsgID ：消息ID
     @param   pszMsg ：消息结构
     @param   nLen ：消息长度
     @return  返回false，表示截断
     */
	virtual bool				OnVote(DWORD dwMsgID, SGameMsgHead * pGameMsgHead, LPCSTR pszMsg, int nLen) = 0;
};

/////////////////////////////消息执行sink///////////////////////////////
struct IMessageExecuteSink
{
	/**
     @param   dwMsgID ：消息ID
     @param   pszMsg ：消息结构
     @param   nLen ：消息长度
     @return
     */
	virtual void				OnExecute(DWORD dwMsgID, SGameMsgHead * pGameMsgHead, LPCSTR pszMsg, int nLen) = 0;
};

/////////////////////////////实体类型///////////////////////////////
struct IEntityClass
{
	/** 是否为人物
     @param
     @param
     @return
     */
	virtual bool				IsPerson(void) = 0;
    
	/** 是否为怪物
     @param
     @param
     @return
     */
	virtual bool				IsMonster(void) = 0;
	/** 是否为宠物
     @param
     @param
     @return
     */
	virtual bool               IsPet(void) = 0;
    
	/** 是否为装备
     @param
     @param
     @return
     */
	virtual bool				IsEquipment(void) = 0;
    
	/** 是否为非装备类物品
     @param
     @param
     @return
     */
	virtual bool				IsNonequipableGoods(void) = 0;
    
	/** 是否为宝箱
     @param
     @param
     @return
     */
	virtual bool				IsBox(void) = 0;
    
	/** 是否为旗杆（摊位、传送门... ...）
     @param
     @param
     @return
     */
	virtual bool				IsMast(void) = 0;
    
	/** 是否为生物
     @param
     @param
     @return
     */
	virtual bool				IsCreature(void) = 0;
    
	/** 是否为物品
     @param
     @param
     @return
     */
	virtual bool				IsGoods(void) = 0;
    
	/** 取得游戏类型（tEntity_Class_Person， tEntity_Class_Monster，tEntity_Class_Equipment，tEntity_Class_NonequipableGoods）
     @param
     @param
     @return
     */
	virtual DWORD				Class(void) = 0;
};

//////////////////////////////实体//////////////////////////////////
struct IEntity
{
	/** 释放
     @param
     @param
     @return
     */
	virtual void				Release(void) = 0;
    
	/** 取得实体类型
     @param
     @param
     @return
     */
	virtual IEntityClass *		GetEntityClass(void) = 0;
    
	/** 取得UID
     @param
     @param
     @return
     */
	virtual LONGLONG			GetUID(void) = 0;
    
	/** 取得所在的地图位置
     @param
     @param
     @return
     */
	virtual xs::Point				GetMapLoc(void) = 0;
    
	/** 设置数值属性
     @param   nValue ：属性值
     @param
     @return
     */
	virtual bool				SetNumProp(DWORD dwPropID, int nValue) = 0;
    
	/** 取得数值属性
     @param
     @param
     @return
     */
	virtual int					GetNumProp(DWORD dwPropID) = 0;
    
	/** 设置字符属性
     @param   pszValue ：属性值
     @param
     @return
     */
	virtual bool				SetStrProp(DWORD dwPropID, LPCSTR pszValue, int nLen) = 0;
    
	/** 批量更新属性
     @param
     @param
     @return
     */
	virtual bool				BatchUpdateProp(LPCSTR pszProp, int nLen) = 0;
    
	/** 消息
     @param
     @param
     @return
     */
	virtual void				OnMessage(DWORD dwMsgID, SGameMsgHead * pGameMsgHead, LPCSTR pszMsg, int nLen) = 0;
    
	/** 订阅投票消息
     @param
     @param
     @return
     */
	virtual bool				Subscibe(DWORD dwMsgID, IMessageVoteSink * pVoteSink, LPCSTR pszDesc) = 0;
    
	/** 取消订阅投票消息
     @param
     @param
     @return
     */
	virtual bool				UnSubscibe(DWORD dwMsgID, IMessageVoteSink * pVoteSink) = 0;
    
	/** 订阅执行消息
     @param
     @param
     @return
     */
	virtual bool				Subscibe(DWORD dwMsgID, IMessageExecuteSink * pExecuteSink, LPCSTR pszDesc) = 0;
    
	/** 取消订阅执行消息
     @param
     @param
     @return
     */
	virtual bool				UnSubscibe(DWORD dwMsgID, IMessageExecuteSink * pExecuteSink) = 0;
    
	/** 取得实体视图对象接口
     @param
     @param
     @return
     */
	virtual EntityView *		GetEntityView(void) = 0;
    
	/** 取得事件源类型,SOURCE_TYPE_PERSON, SOURCE_TYPE_MONSTER ... ...
     @param
     @param
     @return
     */
	virtual BYTE				GetEventSourceType(void) = 0;
    
	/**
     @param
     @param
     @return
     */
	virtual void				Visit(IItemVisitor * pVisitor) = 0;
    
	/** 是否能交易，宠物，装备跟一般物品都可以交易
     @param
     @param
     @return
     */
	virtual bool				CanTrade(ISkepGoods * pSkepGoods,char * errorMsg = NULL) = 0;
};

////////////////////////////生物对像////////////////////////////////////
struct ICreature : public IEntity
{
	/** 增加实体部件
     @param
     @param
     @return
     */
	virtual bool				AddEntityPart(IEntityPart * pEntityPart) = 0;
    
	/** 移除实体部件
     @param
     @param
     @return
     */
	virtual bool				RemoveEntityPart(DWORD dwPartID) = 0;
    
	/** 取得实体部件
     @param
     @param
     @return
     */
	virtual IEntityPart *		GetEntityPart(DWORD dwPartID) = 0;
    
	/** 是否正在移动
     @param
     @param
     @return
     */
	virtual bool				IsMoving(void) = 0;
    
	/** 获取头像ID
     @param
     @param
     @return
     */
	virtual int                 GetFaceID(void) = 0;
    
	/** by cjl 变身，只涉及到外观和名字
     @param   -1表示还原、否则就根据表来修改名字
     @param	 nakeResID不为0时，表示修改备份裸体资源
     @param   变身剩余时间，为0时则表示读表
     @return
     */
	virtual bool ChangeBody(int modeID, int nakeResID = 0, int t = 0) = 0;
};

////////////////////////////人物对像////////////////////////////////////
//by cjl
struct SPresentSchemeInfoBase;
//end cjl
struct IPerson : public ICreature
{
	/** 取得人物名字
     @param
     @param
     @return
     */
	virtual LPCSTR				GetName(void) = 0;
    
	/** 调整当前控制状态
     @param
     @param
     @return
     */
	virtual bool				GotoState(DWORD dwState, LPCSTR pContext, int nLen) = 0;
    
	/** 取得当前控制状态
     @param
     @param
     @return
     */
	virtual DWORD				GetCurState(void) = 0;
    
	/** 客户端只能主动修改主角的移动方式
     @param
     @param
     @return
     */
	virtual void               UpdateMoveStyle(char nMoveStyle) = 0;
    
	/** 获取个性签名
     */
	virtual LPCSTR				GetSignature(void) = 0;
    
	/** 获取头像ID
     @param
     @param
     @return
     */
	virtual int                 GetFaceID(void) = 0;
    
	/** 获取跟随状态
     @param
     @param
     @return
     */
	virtual bool                GetFollowState(void) = 0;
    
	/** 设置跟随状态
     @param
     @param
     @return
     */
	virtual void                SetFollowState(bool bstate,int nHeroSpeed = 0,int nFollowSpeed = 0) = 0;
    
	/** 通过阵营ID去设置 阵营图片
     @param
     @param
     @return
     */
	virtual void                SetCampPicture(int nCampID) = 0;
    
	/** 设置飞行状态
     @param
     @param
     @return
     */
	virtual void				SetFLyState(bool bFly) = 0;
    
	/** 获取飞行状态
     @param
     @param
     @return
     */
	virtual bool				GetFLyState() = 0;
    
	/** 获取飞行模型加载状态
     @param
     @param
     @return
     */
	virtual bool				GetFLyModelLoadState() = 0;
    
	/**
     @param
     @param
     @return
     */
	virtual ulong                GetRidingPet() = 0;
    
	/** 骑乘模型加载完毕；去开始寻路事件触发；
     @param
     @param
     @return
     */
	virtual bool				OnResLoadedToMove() = 0;
    
	/** 设置飞行状态坐标点值；
     @param
     @param
     @return
     */
	virtual void				SetFLyToMovePoint(xs::Point pt) = 0;
    
    
	//by cjl
	/** 获取新手礼包属性
     @param
     @return
     */
	virtual int						GetPresentProp(int id) = 0;
    
	/** 获取角色目前可以获取的物品ids
     @param
     @return
     */
	virtual SPresentSchemeInfoBase*  GetPresentGoodsID() = 0;
    
	/** 获取战斗力显示属性
     @param
     @return
     */
	virtual int						GetFightDisplayProp(int id) = 0;
	//end cjl
    
	/** 设置人物死亡状态；
     @param
     @return
     */
	virtual void                     SetPersonDieFlage(bool bDie) = 0;
    
	/** 获取人物死亡状态；
     @param
     @return
     */
	virtual bool                     GetPersonDieFlage() = 0;
};

////////////////////////////人物对像////////////////////////////////////
struct IMonster : public ICreature
{
	/**
     @param
     @param
     @return
     */
	virtual LPCSTR				GetName(void) = 0;
    
	/**
     @param
     @param
     @return
     */
	virtual void				SetName(LPCSTR pszName) = 0;
    
	/** 获取头像ID
     @param
     @param
     @return
     */
	virtual int                 GetFaceID(void) = 0;
    
	/** 创建怪物身上的采集特效；
     @param
     @param
     @return
     */
	virtual void                CreateCollectionEffect() = 0;
    
	/** 关闭怪物身上的采集特效；
     @param
     @param
     @return
     */
	virtual void				CloseCollectionEffect() = 0;
};

////////////////////////////物品对像////////////////////////////////////
struct IGoods : public IEntity
{
	/** 是否有某绑定标志
     @param
     @param
     @return
     */
	virtual bool				IsHasBindFlag(DWORD dwBindFlag) = 0;
    
	/** 能否放置
     @param
     @param
     @return
     */
	virtual bool				CanPlace(ISkepGoods*pSkepGoods,ulong ulPackIDTarget,int& iPlaceTarget) = 0;
    
	/** 能否丢弃
     @param
     @param
     @return
     */
	virtual bool				CanDropOut(ISkepGoods * pSkepGoods) = 0;
};

////////////////////////////宠物对象////////////////////////////////////
struct IPet:public ICreature
{
	/**
     @param
     @param
     @return
     */
	virtual LPCSTR				 GetName(void) = 0;
	/**
     @param
     @param
     @return
     */
	virtual void				 SetName(LPCSTR pszName) = 0;
    
	/** 宠物在魂将列表内的移动控制
     @param
     @param
     @return
     */
	virtual bool				CanPlace(ISkepGoods*pSkepGoods,ulong ulPackIDTarget,int& iPlaceTarget) = 0;
    
	/** 宠物升级判断
     @param
     @param
     @return
     */
	virtual bool				PetUpgrade(DWORD dwPropID, int nValue) = 0;
    
	/**
     @param
     @param
     @return
     */
	virtual LONGLONG			GetLeader(void) = 0;
    
	/** 获取头像ID
     @param
     @param
     @return
     */
	virtual int                 GetFaceID(void) = 0;
    
	/** 是否处于某种状态（出征，骑乘）
     @param
     @param
     @return
     */
	virtual bool				IsHasStateFlag(DWORD dwState) = 0;
    
	/** 能否丢弃
     @param
     @param
     @return
     */
	virtual bool				CanDropOut(ISkepGoods * pSkepGoods) = 0;
    
	/** 是否有某绑定标志
     @param
     @param
     @return
     */
	virtual bool				IsHasBindFlag(DWORD dwBindFlag) = 0;
};
////////////////////////////装备对像////////////////////////////////////
struct IEquipment : public IGoods
{
	/** 装备
     @param
     @param
     @return
     */
	virtual bool				OnEquip(ICreature * pCreature) = 0;
    
	/** 脱下
     @param
     @param
     @return
     */
	virtual bool				UnEquip(ICreature * pCreature) = 0;
    
	/** 获得效果
     @param
     @param
     @return
     */
	virtual IEffect *			GetEffect(int nIndex) = 0;
    
	/** 能否装备
     @param
     @param
     @return
     */
	virtual bool				CanEquip(int nPlace) = 0;
    
	/**获得装备包裹ID
     @param
     @param
     @return
     */
	virtual DWORD				GetEquipPackID(void) = 0;
    
	/**获得装备刻字
     @param
     @param
     @return
     */
	virtual bool				GetLettering(char * pszValue, int  nLen) = 0;
    
	/**获得装备品质等级
     @param
     @param
     @return
     */
	virtual int				GetBrandLevel() = 0;
};

////////////////////////////非装备类物品对像////////////////////////////////////
struct INonequipableGoods : public IGoods
{
	/** 能否使用
     @param
     @param
     @return
     */
	virtual bool				CanUse(void) = 0;
	/** 能否使用2
     @param
     @param
     @return
     */
	virtual bool                PetCanUse(LONGLONG uid) = 0;
    
};

////////////////////////////宝箱对像////////////////////////////////////
struct IBox : public IEntity
{
	/** 取得名字
     @param
     @param
     @return
     */
	virtual LPCSTR				GetName(void) = 0;
    
	/** 是否有权限
     @param
     @param
     @return
     */
	virtual bool				IsMaster(long lPDBID) = 0;
};

////////////////////////////旗杆对像////////////////////////////////////
struct IMast : public IEntity
{
	/** 取得名字
     @param
     @param
     @return
     */
	virtual LPCSTR				GetName(void) = 0;
};

/////////////////////////////泡泡对象///////////////////////////////////
// add by zjp.
struct IBubble : public IEntity
{
	
};

////////////////////////////实体部件////////////////////////////////////
struct IEntityPart
{
	/** 释放,会释放内存
     @param
     @param
     @return
     */
	virtual void				Release(void) = 0;
    
	/** 创建，重新启用，也会调此接口
     @param
     @param
     @return
     */
	virtual bool				Create(IEntity * pMaster, LPCSTR pszData, int nLen) = 0;
    
	/** 取是部件ID
     @param
     @param
     @return
     */
	virtual DWORD				GetPartID(void) = 0;
    
	/** 消息
     @param
     @param
     @return
     */
	virtual	int					OnMessage(DWORD dwMsgID, LPCSTR pszMsg, int nLen) = 0;
    
	/** 取是主人
     @param
     @param
     @return
     */
	virtual IEntity *			GetMaster(void) = 0;
    
	/** 激活部件
     @param
     @param
     @return
     */
	virtual bool				Active(LPCSTR pszContext, int nLen) = 0;
    
	/** 冻结部件
     @param   比如当生物转到坐下状态，就需冻结移动部件
     @param
     @return
     */
	virtual bool				Freeze(LPCSTR pszContext, int nLen) = 0;
};

/////////////////////////////公共部件//////////////////////////////////////////
struct ICommonPart : public IEntityPart
{
	/** 获取装备数据
     @param
     @param
     @return
     */
	virtual bool  getFormData( void * pFormData) = 0;
    
	/** 飞行自动寻路
     @param
     @param
     @return
     */
	virtual bool  SendAutoMoveEventByFly(ulong ulMapID,xs::Point ptEndPoint) = 0;
};

////////////////////////////阵营部件//////////////////////////////////////
struct ICampPart : public IEntityPart
{
	/** 取得阵营ID
     @param
     @param
     @return
     */
	virtual int					GetCampID(void) = 0;
    
	/** 能否攻击，＊＊＊＊只适用于怪物，宠物攻击，人物攻击取PK部件判断
     @param
     @param
     @return
     */
	virtual bool                CanAttack(ICreature * pTargetCreature) = 0;
};

/////////////////////////////////////////////////////////////////////////
struct IConjurePart : public IEntityPart
{
	/**
     @param
     @param
     @return
     */
	virtual LONGLONG			GetLeader(void) = 0;
};

////////////////////////////////////////////////////////////////////////
struct IPetPart : public IConjurePart
{
	/** 出征
     @param
     @param
     @return
     */
	virtual bool				ConjurePet(LONGLONG petID,int nPlace) = 0;
    
	/** 召回
     @param
     @param
     @return
     */
	virtual bool				CallBackPet(LONGLONG petID,int nPlace) = 0;
    
	/** 解体
     @param
     @param
     @return
     */
	virtual void               DisbandPet(LONGLONG petID,int nPlace) = 0;
    
	/** 下马
     @param
     @param
     @return
     */
	virtual bool               DismountPet(LONGLONG petID,int nPlace) = 0;
    
	/** 骑乘
     @param
     @param
     @return
     */
	virtual bool				RidePet(LONGLONG petID,int nPlace) = 0;
    
	/** 骑乘检查
     @param
     @param
     @return
     */
	virtual bool				CanRidePet(LONGLONG petID,int nPlace) = 0;
    
	/** 合体
     @param
     @param
     @return
     */
	virtual void				CombinePet(LONGLONG petID,int nPlace) = 0;
    
	/** 改名
     @param
     @param
     @return
     */
	virtual void                ChangePetName(int nPlace,LONGLONG petID,const std::string &strName,bool hasBadWords = false) = 0;
    
	/** 阵法开启
     @param
     @param
     @return
     */
	virtual void                 SetFormation(int formation) = 0;
    
	/** 设置头目
     @param
     @param
     @return
     */
	virtual void				 SetLeader(LONGLONG uid) = 0;
    
	/** 阵法列表
     @param
     @param
     @return
     */
	virtual void				 RefreshFormation() = 0;
    
	/** 获取宠物列表
     @param
     @param
     @return
     */
	virtual list<LONGLONG> *	GetPetList(void) = 0;
    
	/** 获取当前阵法
     @param
     @param
     @return
     */
	virtual int					GetCurrentFormation() = 0;
    
	/** 获取阵法等级
     @param
     @param
     @return
     */
	virtual short				GetPosSkillGrade(int id) = 0;
    
	/** 命令宠物
     @param
     @param
     @return
     */
	virtual void				SendCommand(LPCSTR pszData, int nLen) = 0;
    
	/** 取消命令宠物
     @param
     @param
     @return
     */
	virtual void				CancelCommand(LPCSTR pszData, int nLen) = 0;
    
	/** 客户端请求开始解封宠物
     @param
     @param
     @return
     */
	virtual void				ReqStartPetRelease(LONGLONG petID,int nPlace) = 0;
    
	/** 获得当前骑乘宠物
     @param
     @param
     @return
     */
	virtual void				GetRidePet(LONGLONG& petID, int& nPlace) = 0;
};
//////////////////////////////实体客户端//////////////////////////////////
struct IEntityClient
{
	/** 释放
     @param
     @param
     @return
     */
	virtual void				Release(void) = 0;
    
	/** 设置当前的场景ID
     @param
     @param
     @return
     */
	virtual void				SetZoneID(DWORD dwZoneID)	= 0;
    
	/** 取得当前的场景ID
     @param
     @param
     @return
     */
	virtual DWORD				GetZoneID(void) = 0;
    
	/** 设置当前的地图ID
     @param
     @param
     @return
     */
	virtual void				SetMapID(DWORD dwMapID) = 0;
    
	/** 取得当前的地图ID
     @param
     @param
     @return
     */
	virtual DWORD				GetMapID(void) = 0;
    
	/** 取得客户端主角
     @param
     @param
     @return
     */
	virtual IPerson *			GetHero(void) = 0;
    
	/** 取得客户端宠物
     @param
     @param
     @return
     */
	virtual IPet *              GetPet(void) = 0;
	/** 通过UID取得实体
     @param
     @param   dwGOClass ：是为了防止外部直接用IEntity转成相应的实体接口，对服务器稳定的威胁
     @                    （tEntity_Class_Person， tEntity_Class_Monster，tEntity_Class_Equipment，tEntity_Class_NonequipableGoods）
     @return
     */
	virtual IEntity *			Get(LONGLONG uid, DWORD dwGOClass) = 0;
    
	/** 通过PDBID取得人物实体
     @param
     @param
     @return
     */
	virtual IEntity *			Get(DWORD dwPDBID) = 0;
    
	/** 通过UID取得实体,尽量不要使用这个方法
     @param   
     @param	
     @return  
     */
	virtual IEntity *			Get(LONGLONG uid) = 0;
    
	/** 取得配置中心
     @param   
     @param   
     @return  
     */
	virtual ISchemeCenter *		GetSchemeCenter(void) = 0;
    
	/** 通过UID中的序列号取得实体
     @param   
     @param   
     @return  
     */
	virtual IEntity *			GetBySNO(DWORD dwSNO) = 0;
    
	/** 取得客户端主角的装备篮SKEPID
     @param   
     @param   
     @return  
     */
	virtual DWORD				GetEquipSkepID(void) = 0;
    
	/** 取得客户端主角的包裹篮SKEPID
     @param   
     @param   
     @return  
     */
	virtual	DWORD				GetPacketSkepID(void) = 0;
    
	/** 取得客户端主角的宠物列表篮SKEPID
     @param   
     @param   
     @return  
     */
	virtual DWORD               GetPetlistSkepID(void) = 0;
    
	/** 取得客户端主角的摊位篮SKEPID
     @param   
     @param   
     @return  
     */
	virtual DWORD               GetStallSkepID(void) = 0;
    
    
	/** 设置客户端主角的装备篮SKEPID
     @param   
     @param   
     @return  
     */
	virtual void				SetEquipSkepID(DWORD dwSkepID) = 0;
    
	/** 设置客户端主角的包裹篮SKEPID
     @param   
     @param   
     @return  
     */
	virtual void				SetPacketSkepID(DWORD dwSkepID) = 0;
    
	/** 设置客户端主角的宠物列表SKEPID
     @param   
     @param   
     @return  
     */
	virtual void                SetPetlistSkepID(DWORD dwSkepID) = 0;
    
	/** 设置客户端主角的摊位篮SKEPID
     @param   
     @param   
     @return  
     */
	virtual void                SetStallSkepID(DWORD dwSkepID) = 0;
    
    
	/** 通过默认数据构造实体
     @param   
     @param   
     @return  
     */
	virtual IEntity *					BuildEntity(DWORD dwEntityClass, LONGLONG uid, LPCSTR pszContext, int nLen, int nIsHero) = 0;
    
	//预加载场景怪物模型
	//add by yhc
	virtual void  LoadMapMonster(DWORD nMapID) = 0;
	virtual void  AddEffectToList(DWORD nMapID,long lResID) = 0;
    
	// add by zjp
	/** 请求他人装备信息
     @param   
     @param   
     @return  
     */
	virtual void RequestEquipInfo(ulong ulPersonPID) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(ENTITYCLIENT_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(xs_cec))
extern "C" IEntityClient * LoadCEC(void);
#	define	CreateEntityClientProc	LoadCEC
#else														/// 动态库版本
typedef IEntityClient * (RKT_CDECL *procCreateEntityClient)(void);
#	define	CreateEntityClientProc	DllApi<procCreateEntityClient>::load(MAKE_DLL_NAME(xs_cec), "LoadCEC")
#endif#endif