/*******************************************************************
** 文件名:	SchemeCreateActor.h
** 版  权:	(C) 
** 
** 日  期:	2008/2/27  16:57
** 版  本:	1.0
** 描  述:	配置人物创建参数
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include <string>

//  测试代码：修改为直接读取xml文件，以便于观察配置文件修改的效果；
//人物创建参数配置脚本
//#define CREATEACTOR_SCHEME_FILENAME			"Data\\Scp\\CreateActorConfig.xmc"
#define CREATEACTOR_SCHEME_FILENAME			"Data\\Scp\\CreateActorConfig.xml"

//配置人物创建参数
struct SCreateActorCongfig
{
	std::vector<int> actorFaceIds[6];
	std::vector<int> actorClothesIds[6];
	std::vector<double> actorClothesIdScale[6];

	///客户端显示相关
	int	selectGenderPlatForm[PERSON_VOCATION_MAX][2];//选择性别时平台资源id
	std::string selectGenderPlatFormPath[PERSON_VOCATION_MAX][2];;//对应文件名
	int selectGenderPlatFormEffect[PERSON_VOCATION_MAX][2];//选择性别时平台特效资源id
	std::string selectGenderPlatFormEffectPath[PERSON_VOCATION_MAX][2];//对应文件名
	int	selectGenderPerson[PERSON_VOCATION_MAX][2];//选择性别时人物资源id
	std::string selectGenderPersonPath[PERSON_VOCATION_MAX][2];//对应文件名
	int	selectGenderPersonEffect[PERSON_VOCATION_MAX][2];//选择性别时人物特效资源id
	std::string selectGenderPersonEffectPath[PERSON_VOCATION_MAX][2];//对应文件名
	int selectGenderPersonWeapon[PERSON_VOCATION_MAX][2];//人物武器资源id
	std::string selectGenderPersonWeaponPath[PERSON_VOCATION_MAX][2];//人物武器资源id路径
	std::string selectGenderBg[PERSON_VOCATION_MAX];//背景图片
	std::string selectProfessionVideoFile[PERSON_VOCATION_MAX];//选择职业的视频文件地址

	std::string	createActorDetailBackgroundImage;//创建角色的最后一步的背景图片

	void Clear();
};

class CSchemeCreateActor : public SingletonEx<CSchemeCreateActor>, public ISchemeUpdateSink
{	
	SCreateActorCongfig	m_config; //创建人物配置
public:
	///////////////////////////////ISchemeUpdateSink///////////////////	
	/** CSV配置载入时通知
	@param   pCSVReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/ 
	virtual bool			OnSchemeLoad(ICSVReader * pCSVReader,const char* szFileName);

	/** XML配置载入时通知
	@param   pTiXmlDocument：读取XML的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool			OnSchemeLoad(TiXmlDocument * pTiXmlDocument,const char* szFileName);

	/** 配置动态更新时通知
	@param   pCSVReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool			OnSchemeUpdate(ICSVReader * pCSVReader, const char* szFileName);

	/** 配置动态更新时通知
	@param   pTiXmlDocument：读取XML的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool			OnSchemeUpdate(TiXmlDocument * pTiXmlDocument, const char* szFileName);

	///////////////////////////////CSchemeBuff//////////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool					LoadScheme(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void					Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	const SCreateActorCongfig &	GetCreateActorScheme();

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeCreateActor(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeCreateActor(void);
};
