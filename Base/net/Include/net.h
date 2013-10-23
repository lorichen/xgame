/*******************************************************************
** 文件名:	net.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	01/26/2008
** 版  本:	1.0
** 描  述:	网络通讯接口
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


#ifndef __NET_H__
#define __NET_H__

// 导出宏定义
#ifndef NET_API
#	ifdef NET_EXPORTS
#		define NET_API		extern "C" __declspec(dllexport)
#	else
#		define NET_API		extern "C" __declspec(dllimport)
#	endif
#endif//NET_API

namespace xs
{
#	define MAX_NET_PACKAGE_SIZE  (1024*63)// 最大数据块大小
#	define CONN_ERROR_PACKER  -1         // 解包失败
#	define CONN_ERROR_DNS     -2         // 域名解释失败

	struct IConnection;                  // TCP连接
	struct IUDPSocket;                   // UDP套接字
    struct IPacker;                      // 组包接口

	/**
	@name : 网络事件通知接口
	@brief: 网络事件触发时回调
	*/
	struct IConnectionEventHandler
	{
		/**
		@brief              : 收到被动连接
		@param pIncomingConn: 客户端连入的新连接
		@param ppHandler    : 处理该连接的Handler
		@warning            : 收到连接后一定要设置Handler
		*/
		virtual void OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler ) = 0;

		/**
		@brief      : 连接成功
		@param conn : 连接对象指针
		*/
		virtual void OnConnected( IConnection * conn ) = 0;

		/**
		@brief      : 连接断开
		@param conn : 连接对象指针
		@param reason:断开原因(保留) 
		*/
		virtual void OnDisconnected( IConnection * conn,DWORD reason ) = 0;

		/**
		@brief        : 收到数据
		@param conn   : 连接对象指针
		@param pData  : 数据地址
		@pram dwDataLen:数据长度
		@return       :
		*/
		virtual void OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen ) = 0;

		/**
		@name             : 网络错误事件
		@brief            :
		@param conn       : 出现错误的套接字
		@param dwErrorCode: 错误码
		*/
		virtual void OnError( IConnection * conn,DWORD dwErrorCode ) = 0;
	};

	/**
	@name : TCP连接对象
	@brief: 内部封装了一个TCP套接字对象
	*/
	struct IConnection
	{
		/**
		@brief        : 监听被动连接
		@return       : 没有错误发生返回true,收到连接后回调OnAccept
		*/
		virtual bool Listen() = 0;

		/**
		@brief        : 主动连接远程主机
		@param  ip    : 远程主机IP
		@param  port  : 远程主机端口
		@return       : 没有错误发生返回true,连接成功后回调OnConnected
		*/
		virtual bool Connect( const char * ip,unsigned short port ) = 0;

		/**
		@brief        : 发送数据
		@param  pData : 数据指向的地址
		@param dwDataLen:数据长度
		@param dwFlag : 发送数据的特殊需求,可以由SEND_DATA_FLAG中的枚举值位或组成
		@return       : 没有错误发生返回true
		*/
		virtual bool SendData( const char * pData,DWORD dwDataLen,DWORD dwFlag=0 ) = 0;

		/**
		@brief        : 关闭连接
		@return       : 没有错误发生返回true,关闭成功后回调OnDisconnected
		*/
		//crr 从编程的角度讲，主动Disconnect就是主动调用closesocket，然后release
		//现有代码中实现方式也确实如此，
		//为了线程安全，disconnect和release必须放在同一个线程中完成，如果暴露两个接口，则需要触发两个事件，效率低
		//virtual bool Disconnect() = 0;

		/**
		@brief        : 设置事件处理接口,用来接收回调事件
		@param pHandler:接口指针
		*/
		virtual void SetEventHandler( IConnectionEventHandler * pHandler ) = 0;

		/**
		@brief        : 取得事件处理接口
		@return       : 接口指针
		*/
		virtual IConnectionEventHandler * GetEventHandler() const = 0;


		/**
		@name              : 取得对方的网络地址
		@param dwRemoteIP  : 返回对方的IP [OUT]
		@param wRemotePort : 返回对方的端口 [OUT]
		@return            : 如果对方的地址还不确定则返回失败
		*/
		virtual bool GetRemoteAddr(DWORD &dwRemoteIP,WORD & wRemotePort) = 0;


		/**
		@name              : 取得对方的网络地址
		@param ppRemoteIP  : 返回对方的IP [OUT]
		@param dwRemotePort: 返回对方的端口 [OUT]
		@return            : 如果对方的地址还不确定则返回失败
		*/
		virtual bool GetRemoteAddr( const char ** ppRemoteIP,DWORD & dwRemotePort) = 0;

		/**
		@name         : 是否是被动连接
		@brief        : 
		*/
		virtual bool  IsPassive() const = 0;

		/**
		@brief        : 销毁该对象,释放内存
		*/
		virtual void Release() = 0;

		virtual void ChangePacker(DWORD CryptId,uchar *key,DWORD keylen) = 0;
	};

	/**
	@name : UDP事件处理接口
	@brief: 
	*/
	struct IUDPHandler
	{
		/**
		@name             : 数据到达事件
		@brief            : UDP套接字收到数据时回调用
		@para  socket     : 收到数据的套接字
		@param szRemoteIP : 对方IP
		@param nRemotePort: 对方端口
		@param pData      : 数据地址
		@pram dwDataLen   : 数据长度
		*/
		virtual void OnRecv( IUDPSocket * socket,LPVOID pData,DWORD dwDataLen,const char * szRemoteIP,WORD nRemotePort ) = 0;

		/**
		@name             : 网络错误事件
		@brief            :
		@param socket     : 出现错误的套接字
		@param dwErrorCode: 错误码
		*/
		virtual void OnError( IUDPSocket * socket,DWORD dwErrorCode ) = 0;
	};

	/**
	@name : UDP套接字
	@brief: 
	*/
	struct IUDPSocket
	{
		/**
		@brief            : 发送数据
		@param szRemoteIP : 对方IP
		@param nRemotePort: 对方端口
		@param  pData     : 数据指向的地址
		@param dwDataLen  : 数据长度
		@param dwFlag     : 发送数据的特殊需求,可以由SEND_DATA_FLAG中的枚举值位或组成
		@return           : 没有错误发生返回true
		*/
		virtual bool SendData( const char * szRemoteIP,WORD nRemotePort,const char * pData,DWORD dwDataLen,DWORD dwFlag=0 ) = 0;


		/**
		@brief        : 设置事件处理接口,用来接收回调事件
		@param pHandler:接口指针
		*/
		virtual void SetEventHandler( IUDPHandler * pHandler ) = 0;

		/**
		@brief        : 取得事件处理接口
		@return       : 接口指针
		*/
		virtual IUDPHandler * GetEventHandler() const = 0;

		/**
		@brief        : 销毁该对象,释放内存
		*/
		virtual void Release() = 0;
	};

	/// 发送数据时的标志位
	enum SEND_DATA_FLAG
	{
		NO_COPY      = 0X01 ,     // 发送数据时不要额外拷贝内存,这个标志主要是为了提高性能
								  // 因为发送数据是一个异步重叠IO,所以传递给操作系统SOCKET
								  // 实现的数据BUFF不能指向栈上,所以通常没发送一次数据内部
								  // 都要重新new一段内存,并把数据拷贝过去再发.如果SendData
								  // 函数中传递的数据地址已经是分配在堆上的,用户调用时可以
								  // 指定就用这块内存发送,这样可以减少一次内存拷贝的开销.
		NO_ENCRYPT   = (0X01<<1), // 发送数据时通常会使用默认的加密算法进行加密.为了取消加密
								  // 或者应用层已经加过密了可以设置该标志取消内部的加密过程。
		NO_PACKAGE   = (0X01<<2), // 取消内部的组包过程.因为TCP是流传输的,所以发数据时默认会组包
		OOB_DATA     = (0X01<<3), // 紧急数据,暂时不支持
	};

	/**
	@name         : 初始化通讯层
	@brief        : 初始化过程:
	              : 1.初始化异步IO框架
    @param dwThreadNum:使用多少个线程处理异步IO
	@return       : 1 ... OK
	                -1... 异步IO框架初始化失败
	*/
	NET_API int  InitializeNetwork(DWORD dwThreadNum=1);

	/**
	@name         : 释放通讯层
	@brief        : 如果在程序退出时应用层还没有调用该函数,通讯层会自己释放
	@brief        : 所以这个函数可以不调用
	*/
	NET_API int  UninitializeNetwork();

	/**
	@name            : 获取回调用内容
	@brief           : 通讯层的各种事件都是在通讯线程触发的,而用户希望收到的通讯事件必须在用户线程
                     : 这样上层代码就不需要额外处理线程同步的操作,所以在用户线程的主循环里,需要持续
                     : 调用DispatchNetworkEvents函数来驱动通讯事件的派发,类似在主循环收取窗口消息一样

	*/
	NET_API void DispatchNetworkEvents();

	/**
	@name            : 取得网络事件的通知句柄
	@brief           : 应用层可以监听这个事件然后调用DispatchNetworkEvents收取网络消息
	@return          : 返回事件句柄
	*/
	NET_API HANDLE GetNetworkEventHandle();

	// 创建连接时的标志为
#	define 	NO_PACK      0            // 不组包
#	define	DEFAULT_PACK (IPacker *)1 // 使用默认方式组包

	/**
	@name            : 创建一个TCP连接对象
	@param dwBindPort: 绑定端口,传0主要用在主动方发起连接时随机选择一个可用端口
	@param pHandler  : 事件处理接口指针
	@param pPacker   : 组包接口,传0表示不进行组包,传1表示使用默认的简单组包方法
	@brief:
	*/
	NET_API IConnection * CreateTCPConnection(USHORT dwBindPort=0,IConnectionEventHandler * pHandler=0,IPacker * pPacker=DEFAULT_PACK);

	/**
	@name            : 创建一个UDP套接字对象
	@param dwBindPort: 绑定端口
	@param pHandler  : 事件处理接口指针
	@brief:
	*/
	NET_API IUDPSocket * CreateUDPSocket(USHORT dwBindPort,IUDPHandler * pHandler=0);



///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(NET_STATIC_LIB)	/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(xs_nm))
extern "C" IConnection * CreateTCPConnection(USHORT dwBindPort,IConnectionEventHandler * pHandler,IPacker * pPacker);
#	define	CreateTCPConnectionProc	CreateTCPConnection
#else													/// 动态库版本
typedef IConnection * (RKT_CDECL *procCreateTCPConnection)(USHORT dwBindPort,IConnectionEventHandler * pHandler,IPacker * pPacker);
#	define	CreateTCPConnectionProc	DllApi<procCreateTCPConnection>::load(MAKE_DLL_NAME(xs_nm), "CreateTCPConnection")
#endif

};

#endif//__NET_H__