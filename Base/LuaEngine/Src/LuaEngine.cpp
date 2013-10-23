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
#include "stdafx.h"
#include "LuaEngine.h"

CLuaEngine * g_pLuaEngine = NULL;
/** 运行错误追踪(信息）
@param   
@param   
@return  
*/
static int luaA_Trace(lua_State *L)
{
	int n = lua_gettop(L);  /* 参数个数 */
	int i;
	lua_getglobal(L, "tostring");
	for (i=1; i<=n; i++) {
		const char *s;
		lua_pushvalue(L, -1);  /* function to be called */
		lua_pushvalue(L, i);   /* value to print */
		lua_call(L, 1, 1);
		s = lua_tostring(L, -1);  /* get result */
		if (s == NULL)
			return luaL_error(L, "`tostring' must return a string to `print'");
		if (i>1) Info("\t");
		Info(s);
		lua_pop(L, 1);  /* pop result */
	}
	Info(endl);
	return 0;
}

/** 运行错误追踪(错误）
@param   
@param   
@return  
*/
static int luaA_Error (lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	int i;
	lua_getglobal(L, "tostring");
	for (i=1; i<=n; i++) {
		const char *s;
		lua_pushvalue(L, -1);  /* function to be called */
		lua_pushvalue(L, i);   /* value to print */
		lua_call(L, 1, 1);
		s = lua_tostring(L, -1);  /* get result */
		if (s == NULL)
			return luaL_error(L, "`tostring' must return a string to `print'");
		if (i>1) Error("\t");
		Error(s);
		lua_pop(L, 1);  /* pop result */
	}
	Error(endl);
	return 0;
}

/** 运行错误追踪(警告）
@param   
@param   
@return  
*/
static int luaA_Warning(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	int i;
	lua_getglobal(L, "tostring");
	for (i=1; i<=n; i++) {
		const char *s;
		lua_pushvalue(L, -1);  /* function to be called */
		lua_pushvalue(L, i);   /* value to print */
		lua_call(L, 1, 1);
		s = lua_tostring(L, -1);  /* get result */
		if (s == NULL)
			return luaL_error(L, "`tostring' must return a string to `print'");
		if (i>1) Warning("\t");
		Warning(s);
		lua_pop(L, 1);  /* pop result */
	}
	Warning(endl);
	return 0;
}

/** 载入lua
@param   
@param   
@return  
*/
static int luaA_LoadLua(lua_State * L)
{
	size_t l;
	const char * sFileName = luaL_checklstring(L, 1, &l);
	if(sFileName != NULL && g_pLuaEngine != NULL)
	{
		return g_pLuaEngine->LoadLuaFile(sFileName);
	}

	return false;
}

/* function: getWorkDir */
static int luaA_getWorkDir(lua_State* L)
{
	tolua_pushstring(L, getWorkDir());
	return 1;
}


/** 构造
@param   
@param   
@return  
*/
CLuaEngine::CLuaEngine(void)
{
	m_pLuaState = NULL;

	m_bFromPackLoadLua = true;
}

/** 析构
@param   
@param   
@return  
*/
CLuaEngine::~CLuaEngine(void)
{

}

/** 释放
@param   
@param   
@param   
@return  
@note     释放所有资源，并且销毁此对像
@warning 
@retval buffer 
*/
void CLuaEngine::Release(void)
{
	if(m_pLuaState != NULL)
	{
		lua_close(m_pLuaState);
	}

	//全局变量是魔鬼!
	assert (this == g_pLuaEngine);
	g_pLuaEngine = NULL;

	delete this;
}

/** 创建
@param   
@param   
@return  
*/
bool CLuaEngine::Create(bool bFromPackLoadLua)
{
	// 是否从包里载入lua
	m_bFromPackLoadLua = bFromPackLoadLua;

	// 初始化LUA库
	m_pLuaState = lua_open();
	if(m_pLuaState == NULL)
	{
		return false;
	}

	// 初始化所有的标准库
	luaopen_base(m_pLuaState);
	luaopen_io(m_pLuaState);
	luaopen_string(m_pLuaState);
	luaopen_table(m_pLuaState);	
	luaopen_math(m_pLuaState);
	luaopen_loadlib(m_pLuaState);
	//luaL_openlibs(m_pLuaState);
	//luaopen_debug(m_pLuaState);

	// 华夏II曾经有过此问题，堆栈太小，出现了频繁的当机
	lua_checkstack(m_pLuaState, 256);

	// 初始化一些基本的api
	lua_register(m_pLuaState, "dofile", luaA_LoadLua);	
	lua_register(m_pLuaState, "print", luaA_Trace);
	lua_register(m_pLuaState, "trace", luaA_Trace);
	lua_register(m_pLuaState, "error", luaA_Error);
	lua_register(m_pLuaState, "warning", luaA_Warning);
	lua_register(m_pLuaState, "getWorkDir", luaA_getWorkDir);

	return true;
}

/** 取得lua_State *
@param   
@param   
@param   
@return  
@note     
@warning 
@retval buffer 
*/
lua_State *	CLuaEngine::GetLuaState(void)
{
	return m_pLuaState;
}

/** 执行一个lua脚本
@param   
@param   
@param   
@return  
@note     
@warning 
@retval buffer 
*/
bool CLuaEngine::LoadLuaFile(LPCSTR szLuaFileName)
{
	if(szLuaFileName == NULL)
	{
		return false;
	}

	if(m_bFromPackLoadLua)
	{
		FileSystem * pFileSystem = getFileSystem();
		if(pFileSystem == NULL)
		{
			return false;
		}

		Stream * pStream = pFileSystem->open(szLuaFileName);
		if(pStream == NULL)
		{
			return false;
		}
		
		uint nLength = pStream->getLength();
		if(nLength > 0)
		{
			// 把文件读进内存
			char * pData = new char[nLength + 1]; pData[nLength] = 0;
			pStream->read(pData, nLength);
			pStream->close();
			pStream->release();

			// 能过内存加载文件			
			int top = lua_gettop(m_pLuaState);
			int nResult = luaL_loadbuffer(m_pLuaState, pData, nLength, pData);
			if(nResult == 0)
			{
				nResult = lua_pcall(m_pLuaState, 0, 0, 0);
				if(nResult == 0)
				{
					lua_settop(m_pLuaState, top);

					delete []pData;

					return true;
				}
			}

			delete []pData;

			LPCSTR pszErrInfor = lua_tostring(m_pLuaState, -1);
			Error("[LuaEngine] 载入" << szLuaFileName << "文件失败，失败原因：" << pszErrInfor << endl);

			lua_settop(m_pLuaState, top);

			return false;			
		}

		return true;
	}
	else
	{
		int top = lua_gettop(m_pLuaState);
		try
		{		
			int nResult = luaL_loadfile(m_pLuaState, szLuaFileName);
			if(nResult == 0)
			{
				nResult = lua_pcall(m_pLuaState, 0, 0, 0);
				if(nResult == 0)
				{
					lua_settop(m_pLuaState, top);

					return true;
				}
			}
		}
		catch (...)
		{

		}

		LPCSTR pszErrInfor = lua_tostring(m_pLuaState, -1);
		Error("[LuaEngine] 载入" << szLuaFileName << "文件失败，失败原因：" << pszErrInfor << endl);

		lua_settop(m_pLuaState, top);

		return false;
	}

	return false;
}

/** 执行一段内存里lua
@param   pLuaData : 载入的lua 数据
@param   nDataLen ：数据长度
@param   
@return  
@note     
@warning 
@retval buffer 
*/
bool CLuaEngine::RunMemoryLua(LPCSTR pLuaData, int nDataLen)
{
	if(pLuaData == NULL || nDataLen <= 0)
	{
		return false;
	}

	// 能过内存加载文件			
	int top = lua_gettop(m_pLuaState);
	int nResult = luaL_loadbuffer(m_pLuaState, pLuaData, nDataLen, pLuaData);
	if(nResult == 0)
	{
		nResult = lua_pcall(m_pLuaState, 0, 0, 0);
		if(nResult == 0)
		{
			lua_settop(m_pLuaState, top);

			return true;
		}
	}

	LPCSTR pszErrInfor = lua_tostring(m_pLuaState, -1);
	Error("[LuaEngine] 执行内存lua失败，失败原因：" << pszErrInfor << endl);

	lua_settop(m_pLuaState, top);			

	return false;
}

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
bool CLuaEngine::RunLuaFunction(LPCSTR szFunName, CLuaParam * pInParam, int nInNum, CLuaParam * pOutParam, int nOutNum)
{
	int top = lua_gettop(m_pLuaState);

	try
	{
		lua_getglobal(m_pLuaState, szFunName);

		if(!lua_isfunction(m_pLuaState, -1))
		{
			Error("[LuaEngine] 调用" << szFunName << "函数失败，找不到此函数！" << endl);

			lua_settop(m_pLuaState, top);

			return false;
		}

		for(int i = 0; i < nInNum; i++)
		{
			switch(pInParam[i].GetDataType())
			{
			case PARAM_DATATYPE_NUMBER:
				{
					lua_pushnumber(m_pLuaState, (int)pInParam[i]);
				}			
				break;			
			case PARAM_DATATYPE_STRING:
				{
					lua_pushstring(m_pLuaState, (LPCSTR)pInParam[i]);
				}			
				break;
			case PARAM_DATATYPE_LIGHTUSERDATA:
				{
					lua_pushlightuserdata(m_pLuaState, (void *)pInParam[i]);
				}
				break;
			default:
				{
					Error("[LuaEngine] 调用" << szFunName << "函数失败，输入参数类型错误！" << endl);

					lua_settop(m_pLuaState, top);

					return false;
				}
			}
		}

		// 调用执行
		int nResult = lua_pcall(m_pLuaState, nInNum, nOutNum, 0);
		if(nResult == 0)
		{
			for(int n = 0; n < nOutNum; n++)
			{
				int nType = lua_type(m_pLuaState, -1);
				switch(nType)
				{
				case PARAM_DATATYPE_NUMBER:
					{
						pOutParam[n] = (int)lua_tonumber(m_pLuaState, -1);

						lua_pop(m_pLuaState, 1);
					}
					break;
				case PARAM_DATATYPE_STRING:
					{
						pOutParam[n] = (LPCSTR)lua_tostring(m_pLuaState, -1);

						lua_pop(m_pLuaState, 1);
					}
					break;
				case PARAM_DATATYPE_LIGHTUSERDATA:
					{
						pOutParam[n] = (LPCSTR)lua_topointer(m_pLuaState, -1);

						lua_pop(m_pLuaState, 1);
					}
					break;
				default:
					{
						Error("[LuaEngine] 调用" << szFunName << "函数失败，输出参数类型错误！" << endl);

						lua_settop(m_pLuaState, top);

						return false;
					}
				}
			}

			lua_settop(m_pLuaState, top);

			return true;
		}
	}
	catch (...)
	{

	}

	LPCSTR pszErrInfor = lua_tostring(m_pLuaState, -1);
	Error("[LuaEngine] 调用" << szFunName << "函数失败，失败原因：" << pszErrInfor << endl);

	lua_settop(m_pLuaState, top);

	return false;
}

/** 取得最近发送的lua错误
@param
@param
@param 
@return  错误描述
@note     
@warning 
@retval buffer 
*/
LPCSTR CLuaEngine::GetLastError(void)
{
	return lua_tostring(m_pLuaState, -1);
}

/** 重置Lua引擎，会重新加载所有脚本
@param
@param
@param 
@return  
@note     
@warning 
@retval buffer 
*/
bool CLuaEngine::Reset(void)
{
	assert (m_pLuaState);
	if(m_pLuaState == NULL)
	{
		return false;
	}
	lua_close(m_pLuaState);
	return this->Create(m_bFromPackLoadLua);
}

/** 输出函数
@param   
@param   
@return  
*/
API_EXPORT ILuaEngine * LoadLEM(bool bFromPackLoadLua)
{
	// 如果创建了，则直接返回
	if(g_pLuaEngine != NULL)
	{
		return g_pLuaEngine;
	}

	// 实例化
	g_pLuaEngine = new CLuaEngine();
	if(!g_pLuaEngine)
	{
		return NULL;
	}

	// 初始化数据库服务器配置
	if(!g_pLuaEngine->Create(bFromPackLoadLua))
	{
		SAFE_DELETE(g_pLuaEngine);
		return NULL;
	}	

	return g_pLuaEngine;
}