/******************************************************************
** 文件名:	CreateActorForm.h
** 版  权:	(C)  
** 
** 日  期:	2008-3-15
** 版  本:	1.0
** 描  述:	创建角色			
** 应  用:  
******************************************************************/
#pragma  once
#include "deelx.h"

class CreateActorForm :
	public FormBase	
{
	/************************************************************************/
	/* 事件定义                                                              */
	/************************************************************************/
public:

	static const std::string EventCreateActorEnter; //通知打开登陆界面
	static const std::string EventCreateActorLeave; //通知关闭选择界面
	static const std::string EventBack; //返回到登陆界面
	static const std::string EventCreateActorFail; //创建角色错误
	static const std::string EventCreateActorErrorAndQuit;//必须退出的错误消息

	static const std::string EventSelectGenderAnimationFinished;//旋转角色动画播放完毕
	static const std::string EventSelectGenderAnimationStart;//选择角色动画播放开始
	static const std::string  EventSelectProfessionSatrt;
	static const std::string EventSelectProfessionEnd;       //

	//导出的用于设置属性的属性名字
	static const std::string PropCurRenderPage;//当前渲染页面
	static const std::string PropCurProfession;//当前选择的职业
	static const std::string PropProfessionVideoWindow;//职业视频窗口

	static const std::string PropCurGender;//当前的性别
	static const std::string PropInitFaceId;//初始头像
	static const std::string PropInitFaceIdNum;//初始可选的头像数量
	static const std::string PropAvailableFaceImg;//初始可选的人物头像
	static const std::string PropInitClothesId;//初始衣着
	static const std::string PropInitClothesIdNum;//初始可选的衣着数量
	static const std::string PropAvailableClothesImg;//初始可选的衣着

	static const std::string PropShowSelectedHeroWindow;//显示选中英雄的窗口
	static const std::string PropShowSelectedPetWindow;//显示选中的宠物窗口
	static const std::string PropSelectedHeroRotation;//旋转选中英雄和宠物

	static const std::string PropHeroName;//设置英雄的名字

	/************************************************************************/
	/* constructor and deconstructor                                       */
	/************************************************************************/
public:
	CreateActorForm();
	virtual ~CreateActorForm();

	/************************************************************************/
	/* base class virtual function                                          */
	/************************************************************************/
	virtual const std::string& GetErrorAndQuitEventForUI(void) {return EventCreateActorErrorAndQuit;}
public:
	virtual FormType getFormType();

	/** 设置为当前
	*/
	virtual void onEnter();

	/** 设置为无效,为切换作准备
	*/
	virtual void onLeave();

	virtual void update();

	virtual void render(IRenderSystem *pRenderSystem);


	/************************************************************************/
	/* UI事件处理                                                           */
	/************************************************************************/
	
	/** 创建角色
	@param name 角色名称
	*/
	int OnCreate(std::string & actorname);


	/** 当角色界面上点cancel的时候或者ESC
	*/
	void onCancel();

	/**设置字符串属性
	*/
	bool SetStringProp(const std::string & propName, const std::string & propValue);

	/**设置数值属性
	*/
	bool SetNumProp(const std::string & propName, int propValue);

	/**设置数值属性(创建面板的门派选择与性别选择)
	*/
	bool SetNumPropDetail(const std::string & propName, int propValue);
	/**获取数值属性
	*/
	int GetNumProp(const std::string & propName);
	/**获取字符串属性
	*/
	std::string GetStringProp(const std::string & propName, const std::string  & param);

	bool  GetProfessionCanClicked();

	/************************************************************************/
	/* message and event                                                     */
	/************************************************************************/
public:
	virtual void onMessage(ulong actionId, SGameMsgHead* head, void* data, size_t len);

	void renderOnSelectProfession(IRenderSystem * pRenderSystem);//选择职业时渲染
	void renderOnSelectGender(IRenderSystem * pRenderSystem,uint profession);//选择性别时渲染
	void renderSelectProfessionActionList(IRenderSystem * pRenderSystem);  //绘制当前选中的模型动作；
	void renderOnSelectDetails(IRenderSystem * pRenderSystem);//选择其它具体信息时渲染
	void renderBackGround(IRenderSystem * pRenderSystem);//渲染背景
	// 绘制3门派的角色
	void renderAllProfessionGender(IRenderSystem * pRenderSystem);
private:
	/** 加载所有的模型数据；只加载一次；
	*/
	void InitLoadAllProfession();

	bool validate();//验证创建角色的数据的有效性

	// 左移动背景框
	void LeftMoveBackGround(IRenderSystem * pRenderSystem);

	// 右移动背景框
	void RightMoveBackGround(IRenderSystem * pRenderSystem);

	// 左移动模型
	void LeftMoveProfessionModel(IRenderSystem * pRenderSystem,float fAngle);

	// 右移动模型
	void RightMoveProfessionModel(IRenderSystem * pRenderSystem,float fAngle);

private:
	ImageRect*	m_pBg; 
	Camera*		m_pCamera;
	float       m_fCamera_x;
	float       m_fCamera_z;
	float       m_fCameraAngle;
	Viewport*	m_pViewport;
	EntityView*		m_pEntity;

	//关于创建的人物的信息
	uchar	m_profession;   // 当前职业
	uchar   m_pOldprofession; // 上一选择的职业；
	int     m_nRestImage;      // 需要重置位置的图片；
	float   m_fProfessionImagePos[3];  // 3个图片当前的位子；
	float   m_fCameraProfessionAngle[3];     // 摄像机旋转的角度；
	bool    m_bProfessionCanClick ;      // 在滑动时是否能点击；
	//性别
	uchar	m_gender;
	std::string m_name;
	uint	m_faceid;
	uint	m_creatureid;

	//不同的渲染页面
	uint	m_curRenderPage;
	float	m_lasttickcount;
	ImageRect * m_pPreLoadImage[4];
	//1,选择职业的渲染状态
	//IVideoObject * m_pAVI;// 视频 播放功能暂时屏蔽；WZH
	std::string m_strAVIWindow;
	bool m_bVideoRestartFlag;


	
	//2,选择性别的渲染状态
	// 加载3个职业的 所有性别角色；保存在一个数组内 WZH
	struct SelectGender
	{
		xs::ModelNode * m_pSelectGender[2];
		std::list<int> m_selectGenderActionList;
		int	m_selectGenderModelRot;//摄像机选择的角度
		EntityView * m_selectGenderPreLoaded[10];//选择性别时预先加载的资源，防止点击下一步卡
		bool m_bNeedShowPersonEffect[2];//是否显示附着在人物身上的特效
		bool m_bNeedShowPlatFormEffet[2];//是否显示附着在平台上的特效
		SelectGender()
		{
			memset( m_pSelectGender,0, sizeof(m_pSelectGender));
			m_selectGenderModelRot = 0;
			memset(m_selectGenderPreLoaded, 0 , sizeof(m_selectGenderPreLoaded));
			for( uint i=0; i<2; ++i)
			{
				m_bNeedShowPersonEffect[i]= false;
				m_bNeedShowPlatFormEffet[i] =false;
			}
		}
	};

	// 所有职业的模型数据；
	SelectGender  m_stAllProfessionGender[3];

	//3,选择角色具体信息的渲染状态
	xs::ModelNode * m_pSelectBgEffect;
	EntityView * m_pSelectedHero;
	std::string m_strShowSelectedHero;
	EntityView * m_pSelectedPet;
	std::string m_strShowSelectedPet;

private:
	CRegexpT<wchar_t>	m_regexp;
	CRandGenerator m_randomgenerator;
};
