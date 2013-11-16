/*******************************************************************
** 文件名:	ILoginServerProtocol.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 创建人:	
** 日  期:	03/08/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __INTERFACE_LOGIN_SERVER_PROTOCOL_H__
#define __INTERFACE_LOGIN_SERVER_PROTOCOL_H__

#pragma pack(1)
#pragma warning(disable:4200)

namespace xs
{
#	define  MAX_USERNAME_LEN      32
#	define  MAX_PASSWORD_LEN      32
#	define  MAX_ACTORNAME_LEN     32
#	define  APPERANCE_DATA_LEN    65
#	define	MAX_CREATE_ACTOR_NUM  10 //最多能创建10个角色
#	define  MAX_ACTOR_NUM         30 //服务器和客户端能够记录的角色的最大数量，由于会合服，所以最大数量超过10个 

/**
@name : 登陆服务器协议定义
@brief: 登陆服务器功能:
		1.帐号管理: 注册/验证/删除 (注册和删除可以在WEB上做)
		2.角色管理: 创建/选择/删除
		3.登陆排队
		4.软键盘/安全提交密码

		安全登陆方案:
		1.玩家在多服务器之间切换时的身份认证问题 
		(玩家迁移消息只能在服务器之间发,因为客户端连接保持在网关上不会断开,所以不会产生没有什么问题,
		另外数据库需要对角色记录进行加解锁,防止多服务器对角色数据进行存取时出现回档现象)

		2.密码安全传输-防止网络层监听
		(防止监听有几种办法,例如客户端使用 RSA Public-key加密密码,服务器使用private-key解开.
		但Public-key使用过于频繁(不是一次一密),可能会影响加密算法的健壮性
		另外的方案就是使用Diffie Hellman安全交换密码,可以做到一次一密
		但这个方案容易受到中间人攻击,既A-B进行通讯,C伪装成A跟B通讯,同时又伪装成B跟A通讯,从而套取密码,
		我们的做法是通讯层统一进行流加密,流加密时使用Diffie Hellman做到一次一密,另外登陆服务器在接收登陆消息时使用RSA私钥解密密码字符串)

		3.密码安全传输-防止客户端输入窗口被监听
		这个可以采用软键盘机制,但是要注意软键盘的图象和布局必须由服务器发过来,客户端永远不出现明文字符串

		4.由于使用软键盘机制是绝对安全的,所以删除角色,高等级的装备转移强制要求使用软键盘输入2级密码则可以将盗号的危害减到很小
*/

	//////////////////////////////////////////////////////////////////////////////////////////////////
	/**************************** 登陆服务器和客户端之间的通讯协议 **********************************/
	//////////////////////////////////////////////////////////////////////////////////////////////////

	enum
	{
		NoError     = 0 , // 没有错误,纯粹的提示信息
		LoginError      , // 登陆错误
		CreateActorError, // 创建角色错误
		DeleteActorError, // 删除角色错误
		SelectActorError, // 选择角色错误
		RecoverActorError,	//恢复角色报错
	};

	/**
	@name         : 登陆提示
	@brief        : 登陆服务器通用的提示信息,可以用来提示登陆失败,或者在登陆时发给客户端登陆公告等
	*/
#	define   MSG_LOGINSERVER_MESSAGE        1

	/**
	@name         : 登陆提示
	@brief        : 登陆服务器防沉迷保护的提示信息，消息体与MSG_LOGINSERVER_MESSAGE 同用SMsgLoginServer_Message
	*/
#	define   MSG_LOGINSERVER_ENTHRALLPROTECT_MESSAGE        2

	struct SMsgLoginServer_Message
	{
		DWORD  dwErrorCode;                   // 错误码
		WORD   wMsgLen;                       // 提示信息长度
        char   szMessage[];                   // 提示信息内容
	};


	/**
	@name         : 请求登录
	@brief        : 从服务器获得令牌
	*/
#	define    MSG_REGUESTLOGINSERVER_LOGIN         16
	struct SMsgReguestLoginServer_Login
	{
		DWORD          dwReserve;                     // 保留字段,暂时不用,起干扰作用
		unsigned short nLength;
		unsigned char  nMsgId;
		unsigned char  token[32];

	};

#    define   MSG_SESSIONKEY                        17
	struct SMsgSessionKey
	{
		DWORD          dwReserve;                     // 保留字段,暂时不用,起干扰作用
		unsigned short nLength;
		unsigned char  nMsgId;
		unsigned char  keyId;
		unsigned char  keyLen;
		unsigned char  sessionkey[64];

	};



	/**
	@name         : 请求登陆
	@brief        : 故意不从1开头,以免被猜测,消息内容为RSA公钥加密过后的数据,该数据解密后为SMsgLoginServer_Login结构
	@return       : 登陆成功则返回角色信息(MSG_LOGINSERVER_ACTOR_INFO),否则返回错误提示(MSG_LOGINSERVER_MESSAGE)
	*/
#	define    MSG_LOGINSERVER_LOGIN         25

	struct SMsgLoginServer_Login
	{
		DWORD          dwReserve;                     // 保留字段,暂时不用,起干扰作用
		unsigned short nLength;
		unsigned char  nMsgId;
		//unsigned char  g_srandKey[8];
		unsigned short userLength;
		char           szUserName[MAX_USERNAME_LEN];  // 用户帐号
		unsigned short pwLength;
		char           szPassword[MAX_PASSWORD_LEN];  // 用户密码

	};

	/**
	@name         : 角色信息
	@brief        : 登陆成功或者增删角色后更新角色信息
	*/
#	define   MSG_LOGINSERVER_ACTOR_INFO     27

	struct SMsgLoginServer_ActorInfo
	{
		struct ActorInfo
		{
			DWORD dwNakedResId;						//人物裸体资源id
			DWORD dwFaceID;                        // 头像
			char  szActorName[MAX_ACTORNAME_LEN];  // 角色名			
			BYTE  nLevel;                          // 等级
			BYTE  nSex;                            // 性别
			BYTE  nNation;                         // 国家
			BYTE  nProfession;                     // 职业	
			char  pAppearance[APPERANCE_DATA_LEN]; // 装备列表	
			int   nCreateTime;						//角色创建时间
			BYTE  nDeleteFlag;						//删除标志，如果非0则表示角色已经删
			int   nDeleteTime;				//删除时间,相对于1970年
			int	  nAccessTmie;				//角色访问时间，相对于1970年，与删除时间之差得到删除间隔
		};
		time_t tSeverTimer;                 // 服务器时间
		WORD   wActorNum;						// 角色个数
        ActorInfo  m_ActorInfo[MAX_ACTOR_NUM]; // 角色信息
	};

	/**
	@name         : 创建角色
	@brief        : 消息结构见SMsgLoginServer_CreateActor
	@return       : 创建成功则返回角色信息(MSG_LOGINSERVER_ACTOR_INFO),否则发送错误提示(MSG_LOGINSERVER_MESSAGE)
	*/
#	define    MSG_LOGINSERVER_CREATE_ACTOR  28

	struct SMsgLoginServer_CreateActor
	{
		char  szActorName[MAX_ACTORNAME_LEN];  // 角色名
		DWORD dwFaceID;                        // 头像ID
		BYTE  nSex;                            // 性别
		BYTE  nNation;                         // 国家
		BYTE  nProfession;                     // 职业
		DWORD	dwNakedResId;					//人物裸体资源id，对应creature配置表
	};
 
	/**
	@name         : 删除角色 
	@brief        : 消息结构见SMsgLoginServer_DeleteActor
	@return       : 删除成功则返回角色信息(MSG_LOGINSERVER_ACTOR_INFO),否则发送错误提示(MSG_LOGINSERVER_MESSAGE)
	*/
#	define   MSG_LOGINSERVER_DELETE_ACTOR  29

	struct SMsgLoginServer_DeleteActor
	{
		char szActorName[MAX_ACTORNAME_LEN];  // 要删除的角色名
	};

	/**
	@name         : 选择角色
	@brief        : 消息结构见SMsgLoginServer_SelectActor
	@return       : 选角色成功,则等候服务器发送载入地图的消息,否则服务器会发送错误提示(MSG_LOGINSERVER_MESSAGE)
	*/
#	define  MSG_LOGINSERVER_SELECT_ACTOR   30

	struct SMsgLoginServer_SelectActor
	{
		char szActorName[MAX_ACTORNAME_LEN];  // 要删除的角色名
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
	/**************************** 登陆服务器和场景服务器之间的通讯协议 ******************************/
	//////////////////////////////////////////////////////////////////////////////////////////////////

	/**
	@name         : 登陆服务器通知场景服务器创建角色
	@brief        : 注意: 这条消息是登陆服务器发给场景服务器的,数据结构见SMsgLoginServer_EnterGameWorld
	*/
#	define MSG_LOGINSERVER_ENTER_GAMEWORLD      31

	struct SMsgLoginServer_EnterGameWorld
	{
		DWORD    dwActorDBID;             // 角色数据库ID
		DWORD    dwClientID;              // 客户端ID(网关)
		char	 szClientIP[16];		  // 客户端IP
	};

	/**
	@name         : 网关通知某个用户重连
	@brief        : 数据包结构SMsgLoginServer_Reconnect
	*/
#	define MSG_LOGINSERVER_RECONNECT            32

	struct SMsgLoginServer_Reconnect
	{
		DWORD    dwClientID;              // 客户端ID(网关)
		DWORD    dwServerID;              // 服务器ID
	};
};

//////////////////////////////////////////////////////////////////////////////////////////////////
/**************************** 登陆服务器和客户端之间的通讯协议 **********************************/
//////////////////////////////////////////////////////////////////////////////////////////////////

/**
@name         : 客户端恢复角色
@brief        : 消息结构见SMsgLoginServer_ClientRecoverActor
@return       : 恢复成功，更新角色列表，只用于客户端恢复，否则服务器会发送错误提示(MSG_LOGINSERVER_MESSAGE)
*/
#	define  MSG_LOGINSERVER_RECOVER_ACTOR   33

struct SMsgLoginServer_RecoverActor
{
	char szActorName[MAX_ACTORNAME_LEN];  // 要恢复的角色名
};


// 此消息定义移到DGlobalMessage.h ，后续如果产生新的宏，记得不要和MSG_CLIENT_LOGOUT等的值相同
// /**				客户端主动发起退出登录，			
// @name         : 退出登录
// 注意，根据当前服务器的架构，该消息其实是发给网关MSG_ENDPOINT_GATEWAY，网关需解析该条消息，以便断开与客户端的连接
// @brief        : 
// @return       : 
// **/
// #	define    MSG_CLIENT_LOGOUT           34
// 
// 
// /* 客户端主动从游戏返回角色选择，现有的角色游戏逻辑需中断，重新返回角色信息
// */
// #	define   MSG_RE_SELECT_ACTOR         35
// struct SMsgReSelectActor_SS
// {
//	DWORD dwClientID; //客户端网关处ID
// };


/**
@name         : 登录排队通知
@brief        : 消息结构见SMsgLoginServer_QueueUpdate
*/
#	define  MSG_LOGINSERVER_QUEUEUPDATE      36

struct SMsgLoginServer_QueueUpdate
{
	int nPlace; //您队列位置
	int nRestTime; //预计排队时间
};

// /* 客户端加载地图成功，场景服创建角色Person完毕，转入到PLAY状态
// */
 #	define   MSG_ACTOR_GO_TO_PLAY_STATE   37
 struct SMsgActorGotoPlayState_SS
 {
 	DWORD dwClientID; //客户端网关处ID
 };

/*	发送在线人数给gm客户端
*/
#define	MSG_ACTOR_SHOW_CLIENT_COUNT		40
struct SMsgActorShowClientCount_SC
{
	DWORD dwClientCount;
};

#define MSG_ACTOR_RENAME_RESULT			41
struct SMsgActorReNameResult_SC			
{
	WORD wMsgLen;						//提示消息长度
	char  szResult[];					//提示消息
};

#pragma pack()

#endif//__INTERFACE_LOGIN_SERVER_PROTOCOL_H__