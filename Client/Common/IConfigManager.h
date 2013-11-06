#ifndef __I_ConfigManager_H__
#define __I_ConfigManager_H__
#include "UserInfoDef.h"
struct IConfigManager
{

	/** 最近登录的服务器数量n
	@param  	@param  
	*/
	virtual int GetLastLoginServerCount() = 0;

	/** 根据最近登录的服务器数量n，使用下标0至n-1获取其对应的分区名字
	因为同时返回地区名和服务器名有点麻烦，故采用下标参数读取 而不采用GetNext的实现方式
	@param  iIndex： 下标
	@param  
	*/
	virtual const std::string&  GetLastLoginAreaName(int iIndex) = 0;


	/** 根据最近登录的服务器数量n，使用下标0至n-1获取其对应的服务器的名字
	@param  iIndex： 下标
	@param  
	*/
	virtual const std::string&  GetLastLoginServerName(int iIndex) = 0;

	//保存分区名及服务器名
	virtual void saveLastLoginAreaServer(const std::string& strArea, const std::string& strServer) = 0;

	///是否保存用户名
	virtual bool isSaveUserName() = 0;

	///获得保存的用户名
	virtual const std::string& getUserName() = 0;

	///设置保存用户名
	virtual void setSaveUserName(bool save) = 0;

	///设置保存的用户名
	virtual void setUserName(const std::string& username) = 0;

	///获取渲染系统类型
	virtual RenderSystemType getRenderSystemType() = 0;

	//设置渲染系统类型
	virtual void setRenderSystemType(RenderSystemType type) = 0;

	//设置保存全屏模式
	virtual void setSystemSetFullScreen(bool bSet) = 0;

	//设置保存窗口模式
	virtual void setSystemSetWindowType(bool bSet) = 0;

	//设置保存窗口模式类型值
	virtual void setSystemSetWindowTypeValue(int nTypeValue_x,int nTypeValue_y) = 0;

	//设置保存阴影
	virtual void setSystemSetShadow(bool bSet) = 0;

	//设置保存垂直同步
	virtual void setSystemSetSynchronization(bool bSet) = 0;

	//设置保存天气效果
	virtual void setSystemSetWeather(bool bSet) = 0;

	//设置保存显示周边玩家
	virtual void setSystemSetShowAllPerson(bool bSet) = 0;

	//设置保存背景音乐
	virtual void setSystemSetBackGMusic(bool bSet) = 0;

	//设置保存游戏音效
	virtual void setSystemSetGameSound(bool bSet) = 0;

	// 设置静音
	virtual void setSystemSetMulte(bool bSet) = 0;

	//设置保存背景音乐值
	virtual void setSystemSetBackGMusicValue(int nValue) = 0;

	//设置保存游戏音效值
	virtual void setSystemSetGameSoundValue(int nValue) = 0;

	//获取保存全屏模式
	virtual bool getSystemSetFullScreen() = 0;

	//获取保存窗口模式
	virtual bool getSystemSetWindowType() = 0;

	//获取保存窗口模式类型值x
	virtual int  getSystemSetWindowTypeValue_x() = 0;

	//获取保存窗口模式类型值y
	virtual int  getSystemSetWindowTypeValue_y() = 0;

	//获取保存阴影
	virtual bool getSystemSetShadow() = 0;

	//获取保存垂直同步
	virtual bool getSystemSetSynchronization() = 0;

	//获取保存天气
	virtual bool getSystemSetWeather() = 0;

	//获取保存显示周边玩家
	virtual bool getSystemSetShowAllPerson() = 0;

	//获取保存背景音乐
	virtual bool getSystemSetBackGMusic() = 0;

	//获取保存游戏音效
	virtual bool getSystemSetGameSound() = 0;

	// 获取静音
	virtual bool getSystemSetMulte() = 0;

	//获取保存背景音乐值
	virtual int getSystemSetBackGMusicValue() = 0;

	//获取保存游戏音效值
	virtual int getSystemSetGameSoundValue() = 0;

	//设置当前骑乘项
	virtual void setUserRidePlace(dbid dbidPerson, int nPlace) = 0;

	//获取当前骑乘项
	virtual int  getUserRidePlace(dbid dbidPerson) = 0;

};

struct IServerListConfig
{
	/** 获取分区数量
	@param   
	@param  
	*/
	virtual int GetAreaCount(void) = NULL;

	/** 根据分区数量n，使用下标0至n-1获取某分区名字
	@param  iAreaIndex： 分区下标
	@param  
	*/
	virtual const std::string&  GetAreaName(int iAreaIndex) = NULL;

	/** 根据分区数量n，使用下标0至n-1获取某分区下面服务器的数量
	@param  iAreaIndex： 分区下标
	@param  
	*/
	virtual int GetServerCount(int iAreaIndex) = NULL;


	/** 使用下标获取 某分区下面 某服务器的名字
	@param  iAreaIndex： 分区下标
	@param  iServerIndex ：服务器下标
	*/
	virtual const std::string&  GetServerName(int iAreaIndex, int iServerIndex) = NULL;


	//////////////////////////////////////////////////////////////////////////////////
	/** 被推荐的服务器数量n
	@param  	@param  
	*/
	virtual int GetRecommendedServerCount() = NULL;

	/** 根据被推荐的服务器数量n，使用下标0至n-1获取其对应的分区名字
	@param  iIndex： 分区下标
	@param  
	*/
	virtual const std::string&  GetRecommendedAreaName(int iIndex) = NULL;


	/** 根据被推荐的服务器数量n，使用下标0至n-1获取其对应的服务器的名字
	@param  iIndex： 下标
	@param  
	*/
	virtual const std::string&  GetRecommendedServerName(int iIndex) = NULL;
	////////////////////////////////////////////////////////////////////////////////////

	/** 根据分区名字，及服务器名字，定位到某具体的服务器，获取其对外所有网关主机的ip及端口中值最小的那个
	@param  strAreaName： 分区名称
	@param  strServerName ：服务器名称
	@param  strIpOut： 输出型参数，服务器主机的IP
	@param  iPortOut ：服务器名称，服务器主机的Port
	*/
	virtual bool FindMinHost(const std::string& strAreaName, const std::string &strServerName, std::string &strIpOut, int &iPortOut) = NULL;


	/** 根据分区名字，及服务器名字，定位到某具体的服务器，获取其对外主机的ip及端口,多网关的时候随机返回一个
	@param  strAreaName： 分区名称
	@param  strServerName ：服务器名称
	@param  strIpOut： 输出型参数，服务器主机的IP
	@param  iPortOut ：服务器名称，服务器主机的Port
	*/
	virtual bool FindHost(const std::string& strAreaName, const std::string &strServerName, std::string &strIpOut, int &iPortOut) = NULL;


	//////////////////////////////////////////////////////////////////////////////
	//获取配置的超级中心ip/port
	virtual void GetSuperCenterHost(std::string &strIpOut, int &iPortOut) = NULL;

	/** 更新某个服务器的在线人数
	@param  dwServerAddr： 服务器地址
	@param  wServerPort ：服务器端口
	@param  dwOnlineNum： 在线人数
	*/
	virtual bool UpdateOnlineNum(DWORD dwServerAddr, WORD wServerPort, DWORD dwOnlineNum) = NULL;


	/** 设置服务器列表在线人数已经更新
	@param  bFlag： 服务器地址
	*/
	virtual void SetServerListStatusUpdated(bool bFlag= true) = NULL;

	/** 使用下标获取 某分区下面 某服务器的名字
	@param  iAreaIndex： 分区下标
	@param  iServerIndex ：服务器下标
	*/
	virtual DWORD   GetServerOnlineNum(int iAreaIndex, int iServerIndex) = NULL;

	/** 通过分区名 及该分区下面服务器的名字 来获取某服务器的在线人数
	@param strArea：	地域分区，比如"电信一区"
	@param strServer：  分区下面的某服务器名称比如"满江红"
	*/
	virtual DWORD   GetServerOnlineNumByName(const std::string& strArea, const std::string & strServer) = NULL;

	/** 通过分区名 及该分区下面服务器的名字 来判断该服务器是否被推荐
	@param strArea：	地域分区，比如"电信一区"
	@param strServer：  分区下面的某服务器名称比如"满江红"
	*/
	virtual int   CheckServerRecommendedByName(const std::string& strArea, const std::string & strServer) = NULL;
	///////////////////////////////////////////////////////////////////////////////

	//删除自己
	virtual void release(void) = NULL;
};
#endif