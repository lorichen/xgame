/*******************************************************************
** 文件名:	
** 版  权:	
** 创建人:	
** 日  期:
** 版  本:	
** 描  述:	
** 应  用:	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

/*! \addtogroup TeamClient
*  组队客户端
*  @{
*/
// 组队客户端
struct ITeamClient
{
	/** 创建队伍
	@param   
	@param   
	@return  
	*/
	virtual bool           CreateTeam(const std::string& strName,DWORD dwTeamMode,DWORD dwGoodsAllotMode,DWORD dwPetAllotMode,bool bOpen) = 0;
     
	/** 搜索队伍
	@param   
	@param   
	@return  
	*/
	virtual bool           FindTeam() = 0;

	/** 申请队伍
	@param   
	@param   
	@return  
	*/
	virtual bool           ApplyTeam(DWORD dwCaptain) = 0;

	/** 查看本队伍信息
	@param   
	@param   
	@return  
	*/
	virtual bool          ViewSelfTeamMessage(bool bClick) =0;

	/** 查看其他队伍信息
	@param   
	@param   
	@return  
	*/
	virtual bool          ViewOtherTeamMessage(DWORD dwCaptain) = 0;

	/** 查看队员属性装备等详细信息
	@param   所需要查询的人员的PDBID
	@param   
	@return  
	*/
	virtual bool         ViewTeamMemberMessage(DWORD dwPDBID)= 0;

	/** 跟随队长
	@param   
	@param   
	@return  
	*/
	virtual void         FollowCaptain(void) = 0;

	/** 释放
	@param   
	@param   
	@param   
	@return  
	@note     释放所有资源，并且销毁此对像
	@warning 
	@retval buffer 
	*/
	virtual void			Release(void) = 0;

	/** 清空数据
	@param   
	@param   
	@param   
	@return  
	@note    
	@warning 
	@retval buffer 
	*/
	virtual void			Clean(void) = 0;

	/** 队伍ID
	@param   
	@param   
	@return  
	*/
	virtual DWORD			GetTeamID(void) = 0;

	/** 邀请/申请
	@param
	@param
	@return
	*/
	virtual bool			Invite(DWORD dwPDBID,bool bInvite) = 0;

	/** 退出队伍
	@param 是否为下线退队 默认为false
	@param 
	@return
	*/
	virtual void            QuitTeam(bool blogOut = false) = 0;

	/** 开除成员
	@param
	@param
	@return
	*/
	virtual void			DismissMember(DWORD dwPDBID) = 0;

	/** 更换队长
	@param   
	@param   
	@return  
	*/
	virtual void			ChangeTeamCaptain(DWORD dwPDBID) = 0;

	/** 分配方式修改
	@param   
	@param   
	@return  
	*/
	virtual void			ChangeAllotMode(long lAllotMode) = 0;

	/** 当前分配方式
	@param   
	@param   
	@return  
	*/
	virtual long			GetCurAllotMode(void) = 0;

	/** 是否为队友
	@param   
	@param   
	@return  
	*/
	virtual bool			IsTeammate(DWORD dwPDBID) = 0;

	/** 得到队长UID
	@param   
	@param   
	@return  
	*/
	virtual LONGLONG        GetTeamCaptainUID(void) = 0;

	/** 得到队伍人数
	@param   
	@param   
	@return  
	*/
	virtual int            GetTeamMemberNumber(void) = 0;

	/** 得到是否为队长
	@param   
	@param   
	@return  
	*/
	virtual bool            IsTeamCaptain(void) = 0;

	/** 得到队伍名字
	@param   
	@param   
	@return  
	*/
	virtual std::string     GetTeamName()= 0;

	/** 得队伍类型
	@param   
	@param   
	@return  
	*/
	virtual int             GetTeamMode()= 0;

	/** 得到魂将分配模式
	@param   
	@param   
	@return  
	*/
	virtual int             GetPetAllotMode()= 0;

	/** 得到物品分配
	@param   
	@param   
	@return  
	*/
	virtual int				GetGoodsAllotMode()= 0;

	/** 得到私有类型
	@param   
	@param   
	@return  
	*/
	virtual bool            GetTeamPrivate()= 0;

	/** 得到目标的队伍信息
	@param   
	@param   
	@return  
	*/
	virtual void			GetTargetTeamMessage(int nDBid,const std::string& strName,int faceid) = 0;     
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(TEAMCLIENT_STATIC_LIB)		/// 静态库版本
	#	pragma comment(lib, MAKE_LIB_NAME(xs_ctm))
	extern "C" ITeamClient * LoadCTM(void);
	#	define	CreateTeamClientProc	LoadCTM
#else													/// 动态库版本
	typedef ITeamClient * (RKT_CDECL *procCreateTeamClient)(void);
	#	define	CreateTeamClientProc	DllApi<procCreateTeamClient>::load(MAKE_DLL_NAME(xs_ctm), "LoadCTM")
#endif

