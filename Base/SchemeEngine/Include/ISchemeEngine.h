/*******************************************************************
** 文件名:	e:\CoGame\SchemeEngine\include\ISchemeEngine.h
** 版  权:	(C) 
** 
** 日  期:	2007/6/23  14:57
** 版  本:	1.0
** 描  述:  配置引擎,CSV只支持读取，XML支持读写
** 应  用:  本配置引擎为了提高效率和避免太多麻烦，这里不能被嵌套调用，也就是在回调中再装载配置.	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include <list>
using namespace std;

// 申明
class TiXmlDocument;

namespace xs {

/**************************** CSV读取器 *****************************
********************************************************************/
// 字段类型
enum
{
	emCSVUnKnow = -1,		// 未知
	emCSVBool,				// bool
	emCSVInt,				// int
	emCSVInt64,				// int64
	emCSVFloat,				// float
	emCSVString,			// string
	emCSVMacro,				// 宏
	emCsvIntArray           //intArray          
};

// CSV读取器
struct ICSVReader
{
	/** 得到某一行(已经去掉了末尾可能的回车换行符)
	@param   dwRow： 行索引，从0开始，包括各种无意义的行在内  
	@param   
	@param   
	@return  成功返回对应字符串，当索引越界返回0
	@note     
	@warning 
	@retval buffer 
	*/
	virtual	const char*		GetLine(DWORD dwRow) = 0;

	/** 得到bool值
	@param dwRow： 有效行索引，从0开始
	@param dwCol： 列索引，从0开始
	@param bDef ： 默认参数，当发生错误或者定位的值未设定时，直接返回默认值  
	@return  返回相应的bool值
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			GetBool(DWORD dwRow, DWORD dwCol, bool bDef = false) = 0;

	/** 得到int值
	@param dwRow： 有效行索引，从0开始
	@param dwCol： 列索引，从0开始
	@param nDef ： 默认参数，当发生错误或者定位的值未设定时，直接返回默认值
	@retval 返回相应的int值
	@note     
	@warning 
	@retval buffer 
	*/
	virtual	int				GetInt(DWORD dwRow, DWORD dwCol, int nDef = 0) = 0;

	/** 得到LONGLONG值
	@param dwRow： 有效行索引，从0开始
	@param dwCol： 列索引，从0开始
	@param nDef ： 默认参数，当发生错误或者定位的值未设定时，直接返回默认值
	@retval 返回相应的LONGLONG值
	@note     
	@warning 
	@retval buffer 
	*/
	virtual	LONGLONG		GetInt64(DWORD dwRow, DWORD dwCol, LONGLONG nDef = 0) = 0;

	/** 得到float值
	@param dwRow： 有效行索引，从0开始
	@param dwCol： 列索引，从0开始
	@param fDef ： 默认参数，当发生错误或者定位的值未设定时，直接返回默认值
	@retval 返回相应的float值 
	@note     
	@warning 
	@retval buffer 
	*/
	virtual float			GetFloat(DWORD dwRow, DWORD dwCol, float fDef = 0.0f) = 0;

	/** 得到string值
	@param dwRow： 有效行索引，从0开始
	@param dwCol： 列索引，从0开始
	@param pszOutBuffer：将结果填充至此buffer,如果pszBuffer为空，nLen则返回实际长度
	@param nLen：缓冲区的最大长度，实际返回的长度也用这个参数带回
	@retval 成功：true, 失败:flase
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			GetString(DWORD dwRow, DWORD dwCol, char* pszOutBuffer, int &nLen) = 0;

	/** 获得总行数
	@param   
	@param   
	@param   
	@return  返回总行数
	@note     
	@warning 
	@retval buffer 
	*/ 
	virtual DWORD			GetRecordCount(void) const = 0;

	/** 获得字段的数目
	@param   
	@param   
	@param   
	@return  返回总字段数
	@note     
	@warning 
	@retval buffer 
	*/
	virtual DWORD			GetFieldCount(void) const = 0;

	/** 通过字段描述获得对应的列索引
	@param   pszFieldDesc： 字段描述字符串
	@param   
	@param   
	@return  返回相应的列索引，如果失败，则返回-1；
	@note     
	@warning 
	@retval buffer 
	*/
	virtual int				GetFieldIndex(const char* pszFieldDesc) const = 0;

	/** 通过列索引获取对应的字段描述
	@param   dwCol： 列索引
	@param   
	@param   
	@return  返回相应的字段描述字符串，如果列索引越界，则返回0；
	@note     
	@warning 
	@retval buffer 
	*/
	virtual const char *	GetFieldDesc(DWORD dwCol) const = 0;

	/** 通过列索引获得对应的字段类型(emCSVUnKnow,emCSVBool, emCSVInt,emCSVInt64...)
	@param   dwCol：列索引
	@param   
	@param   
	@return  返回相应的列索引，如果失败，则返回emCSVUnKnow；
	@note     
	@warning 
	@retval buffer 
	*/
	virtual int				GetFieldType(DWORD dwCol) const = 0;

	/** 获取指定的宏的值
	@param  pszMacroName 要获取的宏的名称 
	@param   
	@param   
	@return  对应宏的值，如果不存在返回0
	@note     
	@warning 
	@retval buffer 
	*/
	virtual const char *	GetMacro(const char* pszMacroName) const = 0;

	//技能编辑使用。add wangtao
	virtual vector<string>&         GetFiledDescVector() = 0;
	virtual vector<int>&			GetFiledTypeVector() = 0;
};


/************************** 配置引擎接口 ****************************
********************************************************************/
// 配置更新回调接口
struct ISchemeUpdateSink
{
	/** CSV配置载入时通知
	@param   pCSVReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/ 
	virtual bool			OnSchemeLoad(ICSVReader * pCSVReader,const char* szFileName) = 0;

	/** XML配置载入时通知
	@param   pTiXmlDocument：读取XML的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool			OnSchemeLoad(TiXmlDocument * pTiXmlDocument,const char* szFileName) = 0;

	
	/** 配置动态更新时通知
	@param   pCSVReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool			OnSchemeUpdate(ICSVReader * pCSVReader, const char* szFileName) = 0;

	/** 配置动态更新时通知
	@param   pTiXmlDocument：读取XML的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool			OnSchemeUpdate(TiXmlDocument * pTiXmlDocument, const char* szFileName) = 0;
};

struct ISchemeEngine
{
	/** 释放配置引擎
	@param   
	@param   
	@param   
	@return  
	@note     
	@warning 
	@retval buffer 
	*/	
	virtual void			Release(void) = 0;

	/** 载入配置,会自动注册sink
	@param   szFileName：配置文件名，必须为绝对路径
	@param   pSink：配置更新回调接口
	@param   bDynamic:是否支持动态更新
	@return  成功返回true，否则为false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			LoadScheme(const char* szFileName, ISchemeUpdateSink * pSink, bool bDynamic = true) = 0;

	/** 更新配置
	@param   szFileName：配置文件名，必须为绝对路径
	@param   
	@param   
	@return  成功返回true，否则为false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			UpdateScheme(const char* szFileName) = 0;

	/** 取消注册配置回调接口
	@param   szFileName：配置文件名，必须为绝对路径
	@param   pSink：配置更新回调接口
	@param   
	@return  成功返回true，否则为false
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			UnregisterSink(const char* szFileName, ISchemeUpdateSink * pSink) = 0;

	/** 取得所有可动态更新的配置
	@param   
	@param   
	@return  
	*/
	virtual list<string> *	GetDynamicSchemeFileName(void) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(SCHEMEENGINE_STATIC_LIB)	/// 静态库版本
	#	pragma comment(lib, MAKE_LIB_NAME(xs_sem))
extern "C" xs::ISchemeEngine * LoadSEM(void);
	#	define	CreateSchemeEngineProc	LoadSEM
#else													/// 动态库版本
typedef xs::ISchemeEngine * (RKT_CDECL *procCreateSchemeEngine)(void);
	#	define	CreateSchemeEngineProc	DllApi<procCreateSchemeEngine>::load(MAKE_DLL_NAME(xs_sem), "LoadSEM")
#endif

} //*namespace xs/
