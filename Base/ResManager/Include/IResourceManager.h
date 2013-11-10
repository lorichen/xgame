/*******************************************************************
** 文件名:	IResourceManager.h
** 版  权:	(C) 
** 
** 日  期:	2008/1/9  10:07
** 版  本:	1.0
** 描  述:	实体资源加载器
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#ifndef __IRESOURCEMANAGER_H_
#define __IRESOURCEMANAGER_H_

#include "LoadLib.h"

namespace xs
{
	/** 资源类型
	*/
	enum ResourceType
	{
		typeResourceModel = 0,	/// 模型资源
		typeResourcePic,        /// 图片类资源
		typeResourceGroundTile, //地图资源
		typeResourceMinimap,	//小地图
		typeResourceLogic,		//这是放在后台线程执行的一段逻辑
		ResourceTypeMaxCount,
	};

	/** 加载模式
	*/
	enum ResLoadModel
	{
		modeSynch,//同步
		modeAsynch,//异步
	};

	/** 资源加载的状态	   
	*/
	enum ResourceState
	{
		stateUninit,	/// 未初始化
		stateAdded,	/// 已经添加到加载队列
		stateLoading,	/// 正在加载中
		stateLoaded,	/// 加载完毕
		stateRelease,   /// 待释放
		stateError,	/// 加载过程中出错
	};


	/** 加载数据接口，mwp，mz等需要实现该接口
	*/
	struct IResource
	{
		virtual size_t size() const = 0;
		virtual void release() = 0;
		virtual void* getInnerData() = 0;
	};


	/** 资源加载器接口
	*/
	struct IResourceLoader
	{
		/** 加载资源
		@param name 资源的名称
		@return void* 加载资源内存
		@note 如果失败返回值为NULL
		*/
		virtual IResource* loadResource(const ulong uResID/*const std::string& name*/) = 0 ;

		/** 释放资源加载对象
		*/
		virtual void  release() = 0 ;
	};


	/** 资源节点接口
	*/
	struct IResourceNode
	{
		virtual IResource* getResource() const = 0;
	};


	/** 资源管理器接口
	*/
	struct IResourceManager
	{
		/** 注册资源处理器
		@param type 加载器类型，参考 ResourceType 枚举定义
		@param resLoader 注册加载器的指针
		*/
		virtual void registerResourceLoader(ResourceType type, IResourceLoader* resLoader) = 0 ;


		/** 卸载资源加载器
		@param type 资源加载器类型
		*/
		virtual void unregisterResourceLoader(ResourceType type) = 0 ;



		/** 请求加载资源
		@param resName 资源的名称,就是资源完整路径名称
		@param type 资源类型，参考 ResourceType 枚举定义
		@param bImmediately 立即加载或者是异步加载
		*/
		virtual handle requestResource(const ulong uResID/*const std::string& resName*/, ResourceType type, bool bImmediately = false, int priority = 0) = 0 ;


		/** 释放指定的资源
		@param resHandle 指定的资源句柄
		@param bDelayRecycle 是否延迟回收
		 (延迟2分钟才回收，避免频繁使用的资源被释放；但是过渡的使用此特性会加重内存负担，所以只有在很必要的地方才使用)
		*/
		virtual bool releaseResource(handle resHandle, bool bDelayRecycle = false) = 0 ;


		/** 设置资源加载模式,
		@param mode modeSynch 同步 modeAsynch 异步
		*/
		virtual void setLoadMode(ResLoadModel mode) = 0; 


		/** 获取资源加载模式,
		@return modeSynch 同步 modeAsynch 异步
		*/
		virtual const ResLoadModel  getLoadMode() const = 0; 

		/** 停止加载线程
		*/
		virtual void stop() = 0;

		/** 查找指定资源
		*/
		//virtual IResource* findResource(ulong resId) = 0 ;

		/** 卸载资源管理器
		*/
		virtual void release() = 0 ;
	};



	#if defined(_LIB) || defined(RESOURCEMANAGER_STATIC_LIB)/// 静态库版本
	#	pragma comment(lib, MAKE_LIB_NAME(xs_rmm))
    extern "C" API_EXPORT IResourceManager* LoadRMM(uint threads);
	#	define	CreateResourceManagerProc	LoadRMM
    
	#else /// 动态库版本
		typedef IResourceManager* (RKT_STDCALL *procCreateResourceManager)(uint threads);
	#	define	CreateResourceManagerProc	DllApi<procCreateResourceManager>::load(MAKE_DLL_NAME(xs_rmm), "LoadRMM")
	#endif

}

#endif