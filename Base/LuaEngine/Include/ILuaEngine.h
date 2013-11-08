/*******************************************************************
** 文件名:	e:\CoGame\LuaEngine\build\ILuaEngine.h
** 版  权:	(C) 
** 
** 日  期:	2007/8/24  19:22
** 版  本:	1.0
** 描  述:	Lua引擎
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

/// 申明一下
struct lua_State;

namespace xs {

/*! \addtogroup LuaEngine
*  lua引擎
*  @{
*/

enum
{
	PARAM_DATATYPE_NONE = -1,			/// LUA_TNONE
	PARAM_DATATYPE_NIL = 0,				/// LUA_TNIL
	PARAM_DATATYPE_BOOLEAN = 1,			/// LUA_TBOOLEAN
	PARAM_DATATYPE_LIGHTUSERDATA = 2,	/// LUA_TLIGHTUSERDATA
	PARAM_DATATYPE_NUMBER = 3,			/// LUA_TNUMBER
	PARAM_DATATYPE_STRING =	4,			/// LUA_TSTRING
	PARAM_DATATYPE_TABLE = 5,			/// LUA_TTABLE
	PARAM_DATATYPE_FUNCTION = 6,		/// LUA_TFUNCTION
	PARAM_DATATYPE_USERDATA	= 7,		/// LUA_TUSERDATA
	PARAM_DATATYPE_THREAD = 8,			/// LUA_TTHREAD
};


/// Lua参数
class CLuaParam
{
public:
	/** 
	@param   
	@param   
	@return  
	*/
	void operator = (int nValue)
	{
		m_nDataType = PARAM_DATATYPE_NUMBER;
		memcpy(m_szValue, &nValue, sizeof(int));
	}

	/** 
	@param   
	@param   
	@return  
	*/
	void operator = (LPCSTR pszValue)
	{
		m_nDataType = PARAM_DATATYPE_STRING;
		strncpy(m_szValue, pszValue, sizeof(m_szValue) - 1);
	}

	/** 
	@param   
	@param   
	@return  
	*/
	void operator = (void * pValue)
	{
		m_nDataType = PARAM_DATATYPE_LIGHTUSERDATA;		
		memcpy(m_szValue, &pValue, sizeof(void *));
	}

	/** 
	@param   
	@param   
	@return  
	*/
	operator LPCSTR(void)
	{
		return m_szValue;
	}

	/** 
	@param   
	@param   
	@return  
	*/
	operator int(void)
	{
		return *((int *)m_szValue);
	}

	/** 
	@param   
	@param   
	@return  
	*/
	operator void *(void)
	{
		return (void *)(* (int *)m_szValue);
	}

	/** 取得数据类型
	@param   
	@param   
	@return  
	*/
	int		GetDataType(void)
	{
		return m_nDataType;
	}

	/** 构造
	@param   
	@param   
	@return  
	*/
	~CLuaParam(void)
	{
		m_nDataType = PARAM_DATATYPE_NUMBER;
		memset(m_szValue, 0, sizeof(m_szValue));
	}

private:
	/// 数据类型
	int				m_nDataType;

	/// 动态生成不好，调用频繁，容易产品内存碎片
	char			m_szValue[1024];	
};

/// lua服务器
struct ILuaEngine
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
	virtual void			Release(void) = 0;

	/** 取得lua_State *
	@param   
	@param   
	@param   
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual lua_State *		GetLuaState(void) = 0;	

	/** 执行一个lua脚本
	@param   szLuaFileName ： lua文件名
	@param   
	@param   
	@return  成功：返回true,失败：false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			LoadLuaFile(LPCSTR szLuaFileName) = 0;

	/** 执行一段内存里lua
	@param   pLuaData : 载入的lua 数据
	@param   nDataLen ：数据长度
	@param   
	@return  成功：返回true,失败：false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			RunMemoryLua(LPCSTR pLuaData, int nDataLen) = 0;

	/** 执行一个lua函数
	@param   szFuncName : lua函数名
	@param   pInParam : 输入参数数组指针
	@param   nInNum : 输入参数个数
	@param   pOutParam : 输出参数数组指针
	@param   nOutNum : 输出参数个数
	@return  成功：返回true,失败：false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			RunLuaFunction(LPCSTR szFunName, CLuaParam * pInParam = 0, int nInNum = 0, CLuaParam * pOutParam = 0, int nOutNum = 0) = 0;

	/** 取得最近发送的lua错误
	@param
	@param
	@param 
	@return  错误描述
	@note     
	@warning 
	@retval buffer 
	*/
	virtual const char*			GetLastError(void) = 0;

	/** 重置Lua引擎，会重新加载所有脚本
	@param
	@param
	@param 
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			Reset(void) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(LUAENGINE_STATIC_LIB)		/// 静态库版本
	#	pragma comment(lib, MAKE_LIB_NAME(xs_lem))
	extern "C" ILuaEngine * CreateLuaEngine(bool bFromPackLoadLua);
	#	define	CreateLuaEngineProc	LoadLEM
#else													/// 动态库版本
	typedef ILuaEngine * (RKT_CDECL *procCreateLuaEngine)(bool bFromPackLoadLua);
	#	define	CreateLuaEngineProc	DllApi<procCreateLuaEngine>::load(MAKE_DLL_NAME(xs_lem), "LoadLEM")
#endif

} //*namespace xs
