/*******************************************************************
** 文件名:	e:\CoGame\SchemeEngine\src\SchemeEngine\CSVReader.h
** 版  权:	(C) 
** 
** 日  期:	2007/6/23  18:54
** 版  本:	1.0
** 描  述:	CSV读取类,不支持保存
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include <string>
#include <vector>
#include <hash_map>
#include "ISchemeEngine.h"
using namespace std;
using namespace stdext;

/* csv文件格式：
#version = 1.0.0.2
#macro camp1 = 10
#macro camp2 = 40
#macro camp3 = 60
怪物名字, 	是否攻击,	血量,		经验,			攻击力,		阵营
string		bool		int			int64			float		macro
半人马		true		1000		854202			189.2		camp1
布拉克		false		25200		4545555			89625		camp2
辛斯雷尔	0			5482122		212154587		184722.8	camp1
鸦爪摄政者	1			45454545	235454545445	8788888.9	camp3

格式要点：
1、无效行：
	a、以#号开头的行(#号前可以包括任意的空白符和制表符)，用于注释，解析时会忽略掉,不能使用中文“#”字符.
	b、空行或者只有空白符和制表符组合成的行，也会忽略掉；

2、有效行：
	a、字段描述行，有意义，但不属于通过行索引能够访问的内容，本格式强制要求有字段描述行，它总是第一个有效行；
	b、字段类型行，有意义，但不属于通过行索引能够访问的内容，本格式强制要求有字段类型行，它总是第二个有效行
		bool	布尔类型，“0”或者“false"(忽略大小写)表示失败，其他表示成功
		int		整型
		int64	64位整型
		float	浮点型
		string	字符型		
	c、记录行，有意义，保存了每条记录，能通过行索引进行访问，从第三个有效行开始的有效行都是记录行；但在程序中这算0行。

3、宏定义
	a、以#macro(注意，是英文#并且macro必须为小写)开头的为宏定义，宏定义的值默认都为字符串，不要加引号
	b、如果采用了宏定义，应该将需要使用宏的整列全部用宏，并且将字段的类型置为macro

4、其他：
	a、bool类型支持true,false表达也支持1,0来表达；
	b、一条记录占一行；
	c、每个字段之间用分隔符进行分隔，
 */
class CCsvReader : public ICSVReader
{
	// 数据类型声明
	typedef vector< string >				TVECTOR_STRING;
	typedef hash_map< string, string >		THASH_MACROMAP;
	typedef vector< int >					TVECTOR_FIELDTYPE;
public:
	//////////////////////////// ICSVReader ///////////////////////////
	///////////////////////////////////////////////////////////////////
	/** 得到某一行(已经去掉了末尾可能的回车换行符)
	@param   dwRow： 行索引，从0开始，包括各种无意义的行在内  
	@param   
	@param   
	@return  成功返回对应字符串，当索引越界返回NULL
	@note     
	@warning 
	@retval buffer 
	*/
	virtual	const char*			GetLine(DWORD dwRow);

	/** 得到bool值
	@param dwRow： 有效行索引，从0开始
	@param dwCol： 列索引，从0开始
	@param bDef ： 默认参数，当发生错误或者定位的值未设定时，直接返回默认值  
	@return  返回相应的bool值
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool			GetBool(DWORD dwRow, DWORD dwCol, bool bDef = false);

	/** 得到int值
	@param dwRow： 有效行索引，从0开始
	@param dwCol： 列索引，从0开始
	@param nDef ： 默认参数，当发生错误或者定位的值未设定时，直接返回默认值
	@retval 返回相应的int值
	@note     
	@warning 
	@retval buffer 
	*/
	virtual	int				GetInt(DWORD dwRow, DWORD dwCol, int nDef = 0);

	/** 得到LONGLONG值
	@param dwRow： 有效行索引，从0开始
	@param dwCol： 列索引，从0开始
	@param nDef ： 默认参数，当发生错误或者定位的值未设定时，直接返回默认值
	@retval 返回相应的LONGLONG值
	@note     
	@warning 
	@retval buffer 
	*/
	virtual	LONGLONG			GetInt64(DWORD dwRow, DWORD dwCol, LONGLONG nDef = 0);

	/** 得到float值
	@param dwRow： 有效行索引，从0开始
	@param dwCol： 列索引，从0开始
	@param fDef ： 默认参数，当发生错误或者定位的值未设定时，直接返回默认值
	@retval 返回相应的float值 
	@note     
	@warning 
	@retval buffer 
	*/
	virtual float			GetFloat(DWORD dwRow, DWORD dwCol, float fDef = 0.0f);

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
	virtual bool			GetString(DWORD dwRow, DWORD dwCol, char* pszOutBuffer, int &nLen);

	/** 获得总行数
	@param   
	@param   
	@param   
	@return  返回总行数
	@note     
	@warning 
	@retval buffer 
	*/ 
	virtual DWORD			GetRecordCount(void) const;

	/** 获得字段的数目
	@param   
	@param   
	@param   
	@return  返回总字段数
	@note     
	@warning 
	@retval buffer 
	*/
	virtual DWORD			GetFieldCount(void) const;

	/** 通过字段描述获得对应的列索引
	@param   pszFieldDesc： 字段描述字符串
	@param   
	@param   
	@return  返回相应的列索引，如果失败，则返回-1；
	@note     
	@warning 
	@retval buffer 
	*/
	virtual int				GetFieldIndex(const char* pszFieldDesc) const;

	/** 通过列索引获取对应的字段描述
	@param   dwCol： 列索引
	@param   
	@param   
	@return  返回相应的字段描述字符串，如果列索引越界，则返回NULL；
	@note     
	@warning 
	@retval buffer 
	*/
	virtual const char*			GetFieldDesc(DWORD dwCol) const;

	/** 通过列索引获得对应的字段类型(emUnKnow,emBool, emInt...)
	@param   dwCol：列索引
	@param   
	@param   
	@return  返回相应的列索引，如果失败，则返回emCSVUnKnow；
	@note     
	@warning 
	@retval buffer 
	*/
	virtual int				GetFieldType(DWORD dwCol) const;

	/** 获取指定的宏的值
	@param  pszMacroName 要获取的宏的名称 
	@param   
	@param   
	@return  对应宏的值，如果不存在返回NULL
	@note     
	@warning 
	@retval buffer 
	*/
	virtual const char*			GetMacro(const char* pszMacroName) const;

	//////////////////////////// CCsvReader ///////////////////////////
	///////////////////////////////////////////////////////////////////
	/** 找开文件
	@param   pszFileName：文件名
	@param   
	@return  
	*/
	bool					Open(const char* pszFileName, bool bEncrypt);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void					Close(void);

	/** 构造函数
	@param   
	@param   
	@return  
	*/
	CCsvReader(char szSeparator = ',');

	/** 析构函数
	@param   
	@param   
	@return  
	*/
	virtual ~CCsvReader(void);

private:
	/** 打开脚本
	@param   
	@param   
	@return  
	*/
	bool					_Open(Stream * pStream);

	/** 取得何行何列字符
	@param   
	@param   
	@return  
	*/
	bool					GetColString(DWORD dwRow, DWORD dwCol, char* pBuffer, DWORD dwBytes);

	/** 
	@param   
	@param   
	@return  
	*/
	void					GetColPos(const std::string &stringLine, DWORD dwCol, int &nStartPos, int &nEndPos, int nFrom = -1);

	/** 解析字段描述
	@param   
	@param   
	@return  
	*/
	void					ParseFieldDesc(const std::string &stringLine);

	/** 解析字段类型 
	@param   
	@param   
	@return  
	*/
	void					ParseFieldType(const std::string &stringLine);

	/** 解析宏
	@param   
	@param   
	@return  
	*/
	bool					ParseMacro(const std::string &stringExp);

	/** 去掉空格和回车符
	@param   
	@param   
	@return  
	*/
	void					FilterSpace(const char* pszStr, int nFrom, int nTo, int &nPos, int &nlen);

	//技能编辑使用。add wangtao
	vector<string>&         GetFiledDescVector();
	vector<int>&			GetFiledTypeVector();

private:
	char					m_szSeparator;		// 字段间的分隔符
	int						m_nCurRow;			// 当前操作的行(用于加速访问下一个字段)	
	int						m_nCurCol;			// 当前操作的列(用于加速访问下一个字段)	
	int						m_nCurPos;			// 当前字段的结束位置(用于加速访问下一个字段)
	TVECTOR_STRING			m_vectorLines;		// 行字符串(不保存注释行)	
	TVECTOR_STRING			m_vectorFieldDesc;	// 字段描述	
	TVECTOR_FIELDTYPE		m_vectorFieldType;	// 字段类型	
	THASH_MACROMAP			m_hashMacroMap;		// 宏映射表		

	// 当前读取的文件名
	std::string				m_curStrName;		// 当前文件名
};