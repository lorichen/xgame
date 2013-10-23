/*******************************************************************
** 文件名:	e:\CoGame\LuaEngine\src\LuaEngine\LuaEngine.h
** 版  权:	(C) 
** 
** 日  期:	2007/8/22  14:26
** 版  本:	1.0
** 描  述:	lua引擎
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "ILuaEngine.h"

class CLuaEngine : public ILuaEngine
{
public:
	///////////////////////////////////////////////////////////////////
	/** 释放
	@param   
	@param   
	@param   
	@return  
	@note     释放所有资源，并且销毁此对像
	@warning 
	@retval buffer 
	*/
	virtual void			Release(void);

	/** 取得lua_State *
	@param   
	@param   
	@param   
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual lua_State *		GetLuaState(void);

	/** 执行一个lua脚本
	@param   
	@param   
	@param   
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			LoadLuaFile(LPCSTR szLuaFileName);

	/** 执行一段内存里lua
	@param   pLuaData : 载入的lua 数据
	@param   nDataLen ：数据长度
	@param   
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			RunMemoryLua(LPCSTR pLuaData, int nDataLen);

	/** 执行一个lua函数
	@param   szFuncName : lua函数名
	@param   pInParam : 输入参数数组指针
	@param   nInNum : 输入参数个数
	@param   pOutParam : 输出参数数组指针
	@param   nOutNum : 输出参数个数
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			RunLuaFunction(LPCSTR szFunName, CLuaParam * pInParam = NULL, int nInNum = 0, CLuaParam * pOutParam = NULL, int nOutNum = 0);

	/** 取得最近发送的lua错误
	@param
	@param
	@param 
	@return  错误描述
	@note     
	@warning 
	@retval buffer 
	*/
	virtual LPCSTR			GetLastError(void);

	/** 重置Lua引擎，会重新加载所有脚本
	@param
	@param
	@param 
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			Reset(void);

	//////////////////////CLuaEngine///////////////////////////////////
	/** 创建
	@param   bFromPackLoadLua : 是否要从文件包里载入lua文件,如果服务器不将lua打包，就可以设标志为flase
	@param   
	@return  
	*/
	bool					Create(bool bFromPackLoadLua);

	/** 构造
	@param   
	@param   
	@return  
	*/
	CLuaEngine(void);

	/** 析构
	@param   
	@param   
	@return  
	*/
	virtual ~CLuaEngine(void);

private:
	// lua标准
	lua_State *				m_pLuaState;

	// 是否从包里载入lua
	bool					m_bFromPackLoadLua;
};
extern CLuaEngine *			g_pLuaEngine;