/*******************************************************************
** 文件名:	e:\Rocket\Client\EntityClient\Include\ISystemService.h
** 版  权:	(C) 
** 创建人:	
** 日  期:	2008/6/17  20:36
** 版  本:	1.0
** 描  述:	系统服务者	
** 应  用:	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

/*! \addtogroup SystemService
*  系统服务者
*  @{
*/

// 玩家远程访问者
struct IPersonVisitor
{
	/** 
	@param   
	@param   
	@return  
	*/
	virtual void			OnVisit(DWORD dwZoneID, DWORD dwMapID, IPerson * pVirtual) = NULL;
};

// 系统服务者
struct ISystemService
{
	/** 释放
	@param   
	@param   
	@param   
	@return  
	@note     释放所有资源，并且销毁此对像
	@warning 
	@retval buffer 
	*/
	virtual void			Release(void) = NULL;

	/** 增加远程玩家访问者
	@param   
	@param   
	@return  
	*/
	virtual	bool			VisitPerson(DWORD dwPDBID, IPersonVisitor * pVisitor) = NULL;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(ENTITYCLIENT_STATIC_LIB)		/// 静态库版本
	#	pragma comment(lib, MAKE_LIB_NAME(EntityClient))
	extern "C" ISystemService * CreateSystemService(void);
	#	define	CreateSystemServiceProc	CreateSystemService
#else														/// 动态库版本
	typedef ISystemService * (RKT_CDECL *procCreateSystemService)(void);
	#	define	CreateSystemServiceProc	DllApi<procCreateSystemService>::load(MAKE_DLL_NAME(EntityClient), "CreateSystemService")
#endif

