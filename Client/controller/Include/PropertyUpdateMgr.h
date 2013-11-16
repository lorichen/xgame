/******************************************************************
** 文件名:	PropertyUpdateMgr.h
** 版  权:	(C)  
** 
** 日  期:	2008-4-2
** 版  本:	1.0
** 描  述:	界面更新属性显示			
** 应  用:  
******************************************************************/
#pragma once

class PropertyUpdateMgr : public SingletonEx<PropertyUpdateMgr>
{
	/************************************************************************/
	/* event define                                                         */
	/************************************************************************/	
public:
	static const std::string	EventOnPlayerHPUpdate;		//actor hp
	static const std::string	EventOnPlayerMPUpdate;		//actor mp
	static const std::string    EventOnPlayerSPUpdate;		//actor sp
	static const std::string	EventOnPlayerMoneyUpdate;   //actor packet money
	static const std::string	EventOnPlayerGoldingotUpdate;   //actor packet goldingot
	static const std::string    EventOnPlayerWareMoneyUpdate; //actor warehouse money
	static const std::string	EventOnPlayerJiaoZiUpdate;	// actor jiao zi 
	static const std::string	EventOnPlayerNameUpdate;    //actor name update
	static const std::string	EventOnPlayerLevelUpdate;    //actor level update

	static const std::string	EventOnPlayerPKModeUpdate;    //actor pkmode update

	static const std::string	EventOnPlayerExpUpdate;    //actor exp update

	static const std::string	EventOnPlayerFightUpdate;           // 更新主角战斗力

	static const std::string	EventOnTargetHPUpdate;		//target hp
	static const std::string	EventOnTargetMPUpdate;		//target mp
	static const std::string	EventOnTargetNameUpdate;    //target name update
	static const std::string	EventOnTargetLevelUpdate;    //target level update
	static const std::string	EventOnTargetTypeUpdate;    //target level update

	

	static const std::string	EventOnMapNameUpdate;    //map name update

	static const std::string	EventOnPetHPUpdate;		//pet hp
	static const std::string	EventOnPetEXPUpdate;	//pet exp
	static const std::string	EventOnPetNameUpdate;   //pet name
	static const std::string	EventOnPetStarsUpdate;	//pet stars
	static const std::string	EventOnPetLevelUpdate;	//pet level
	static const std::string	EventOnPetFormationUpdate; // pet formation
	static const std::string	EventOnPetFormationListUpdate; // pet formation list	

	static const std::string	EventOnFaceIDUpdate;      // FaceID

	/************************************************************************/
	/* constructor and deconstructor                                        */
	/************************************************************************/
public:
	PropertyUpdateMgr();
	~PropertyUpdateMgr();


	void create();

	/************************************************************************/
	/* fire event                                                           */
	/************************************************************************/
public:
	void onPlayerHPUpdate(EventArgs& args);
	void onPlayerMPUpdate(EventArgs& args);
	void onPlayerSPUpdate(EventArgs& args);
	void onPlayerMoneyUpdate(EventArgs& args);
	void onPlayerGoldingotUpdate(EventArgs& args);
	void onPlayerWareMoneyUpdate(EventArgs& args);
	void onPlayerNameUpdate(EventArgs& args);
	void onPlayerJiaoZiUpdate(EventArgs& args);

	void onPlayerLevelUpdate(EventArgs& args);
	void onPlayerExpUpdate(EventArgs& args);
	void OnPlayerFightUpdate(EventArgs& args);

	void onTargetHPUpdate(EventArgs& args);
	void onTargetMPUpdate(EventArgs& args);
	void onTargetNameUpdate(EventArgs& args);

	void onTargetLevelUpdate(EventArgs& args);

	void onMapNameUpdate(EventArgs& args);

	void onTargetTypeUpdate(EventArgs& args);
	void onTargetPKModeUpdate(EventArgs& args);
	// 更新宠物属性
	void onPetHPUpdate(EventArgs& args);
	void onPetEXPUpdate(EventArgs& args);
	void onPetStarsUpdate(EventArgs& args);
	void onPetLevelUpdate(EventArgs& args);
	void onPetNameUpdate(EventArgs& args);
	void onPetFormationUpdate(EventArgs& args);
	void onPetFormationListUpdate(EventArgs& args);

	void onFaceIDUpdate(EventArgs& args);

};
