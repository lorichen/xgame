/******************************************************************
** 文件名:	actor.h
** 版  权:	(C)  
** 
** 日  期:	2008-3-18
** 版  本:	1.0
** 描  述:	包装角色信息,方便存取			
** 应  用:  
******************************************************************/
#pragma  once

class Actor
{
	static DWORD s_equipShowFlag[PERSON_EQUIPPLACE_MAX];//角色显示控制
	std::string m_szActorNameBuff;//缓存角色名
	unsigned short m_currentSelectActor;//当前选择角色
	SMsgLoginServer_ActorInfo m_actorInfo;//角色列表
	EntityView * m_entity; //待显示的角色
public:

	Actor(SMsgLoginServer_ActorInfo* actorInfo);
	~Actor();

	//函数返回的值不能够保存，因为会变
	const std::string &  getActorName(ulong idx);
	const unsigned char & getLevel(ulong idx) const;
	const unsigned char & getSex(ulong idx) const ;
	const unsigned char & getNation(ulong idx) const ;
	const unsigned char & getProfession(ulong idx)  const;
	const unsigned long & getFaceID(ulong idx) const ;
	const unsigned short & getActorNum() const;
	const unsigned char & getActorState(ulong idx) const;
	const unsigned short& getCurrentSelected() const;
	bool					isActorExpired(ulong idx) const;//角色是否过期,7天后过期
	int					  getDeleteTime(ulong idx) const;//获取角色删除时间
	

	EntityView*	getEntityView() const;
	
	void fillActorInfo(SMsgLoginServer_ActorInfo*ssa);
	SMsgLoginServer_ActorInfo* getActorInfo(){ return &m_actorInfo ;} 
	void udpateActorInfo(SMsgLoginServer_ActorInfo* actorInfo);
	bool setCurrentActorIdx(unsigned long idx); 
	const unsigned long getCurrentActorIdx() const { return m_currentSelectActor;}
	
	void releaseRes();
private:
	void OnEquipShow(DWORD equipId, EntityView * pEntityView, int sex, int  nSmeltSoulLevel);//by cjl,添加了当前强化等级
};