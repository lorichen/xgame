 /*******************************************************************
** 文件名:	e:\CoGame\SchemeEngine\src\SchemeEngine\CSVReader.cpp
** 版  权:	(C) 
** 
** 日  期:	2007/6/23  15:17
** 版  本:	1.0
** 描  述:	CSV读取类,不支持保存
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "stdafx.h"
#include <string.h>
#include "CSVReader.h"


/** 构造函数
@param   
@param   
@return  
*/
CCsvReader::CCsvReader(char szSeparator)
{
	m_szSeparator = szSeparator;
	m_nCurRow = -1;
	m_nCurCol = -1;
	m_nCurPos = -1;
}

/** 析构函数
@param   
@param   
@return  
*/
CCsvReader::~CCsvReader(void)
{

}

/** 得到某一行(已经去掉了末尾可能的回车换行符)
@param   dwRow： 行索引，从0开始，包括各种无意义的行在内  
@param   
@param   
@return  成功返回对应字符串，当索引越界返回NULL
@note     
@warning 
@retval buffer 
*/
const char* CCsvReader::GetLine(DWORD dwRow)
{
	if(dwRow < m_vectorLines.size())
	{
		return m_vectorLines[dwRow].c_str();
	}

	return NULL;
}

/** 得到bool值
@param dwRow： 有效行索引，从0开始
@param dwCol： 列索引，从0开始
@param bDef ： 默认参数，当发生错误或者定位的值未设定时，直接返回默认值  
@return  返回相应的bool值
@note     
@warning 
@retval buffer 
*/
bool CCsvReader::GetBool(DWORD dwRow, DWORD dwCol, bool bDef)
{
	char szBuffer[512] = { 0 };
	if(!GetColString(dwRow, dwCol, szBuffer, 512))
	{
		return bDef;
	}

	if(stricmp(szBuffer, "false") == 0 || stricmp(szBuffer, "0") == 0)
	{
		return false;
	}

	return true;
}

/** 得到int值
@param dwRow： 有效行索引，从0开始
@param dwCol： 列索引，从0开始
@param nDef ： 默认参数，当发生错误或者定位的值未设定时，直接返回默认值
@retval 返回相应的int值
@note     
@warning 
@retval buffer 
*/
int	CCsvReader::GetInt(DWORD dwRow, DWORD dwCol, int nDef)
{
	char szBuffer[512] = { 0 };
	if(!GetColString(dwRow, dwCol, szBuffer, 512))
	{
		return nDef;
	}

	return atoi(szBuffer);
}

/** 得到LONGLONG值
@param dwRow： 有效行索引，从0开始
@param dwCol： 列索引，从0开始
@param nDef ： 默认参数，当发生错误或者定位的值未设定时，直接返回默认值
@retval 返回相应的LONGLONG值
@note     
@warning 
@retval buffer 
*/
LONGLONG CCsvReader::GetInt64(DWORD dwRow, DWORD dwCol, LONGLONG nDef)
{
	char szBuffer[512] = { 0 };
	if(!GetColString(dwRow, dwCol, szBuffer, 512))
	{
		return nDef;
	}

	LONGLONG value;
	sscanf(szBuffer,"%llu",&value);
	return value;
}

/** 得到float值
@param dwRow： 有效行索引，从0开始
@param dwCol： 列索引，从0开始
@param fDef ： 默认参数，当发生错误或者定位的值未设定时，直接返回默认值
@retval 返回相应的float值 
@note     
@warning 
@retval buffer 
*/
float CCsvReader::GetFloat(DWORD dwRow, DWORD dwCol, float fDef)
{
	char szBuffer[512] = { 0 };
	if(!GetColString(dwRow, dwCol, szBuffer, 512))
	{
		return fDef;
	}

	return (float)atof(szBuffer);
}

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
bool CCsvReader::GetString(DWORD dwRow, DWORD dwCol, char* pszOutBuffer, int &nLen)
{
	if(dwRow >= m_vectorLines.size() || dwCol >= m_vectorFieldDesc.size())
	{
		nLen = 0;
		return false;
	}

	int nStartPos = 0, nEndPos = 0, nFrom = -1;
	if(m_nCurRow == dwRow && m_nCurCol + 1 == (int)dwCol)
	{
		nFrom = m_nCurPos;
	}
	
	GetColPos(m_vectorLines[dwRow], dwCol, nStartPos, nEndPos, nFrom);
	if(nStartPos == -1 && nEndPos == -1)
	{
		nLen = 0;
		return false;
	}

	const char* pszDebugTest = m_vectorLines[dwRow].c_str();
	
	m_nCurRow = dwRow;
	m_nCurCol = dwCol;
	m_nCurPos = nEndPos;
	std::string strout = m_vectorLines[dwRow].substr(nStartPos + 1, nEndPos - nStartPos - 1);

	pszDebugTest = strout.c_str();

	// 是否为宏?
	THASH_MACROMAP::iterator it = m_hashMacroMap.find(strout);
	if(it != m_hashMacroMap.end())
	{
		if(pszOutBuffer == NULL)
		{
			nLen = ((*it).second).size() + 1;
		}
		else
		{
			if(nLen > ((*it).second).size())
			{
				nLen = ((*it).second).size();				
			}			
			
			strncpy(pszOutBuffer, ((*it).second).c_str(), nLen);
		}	
		
		return true;
	}

	if(pszOutBuffer == NULL)
	{
		nLen = strout.size() + 1;
	}
	else
	{
		if(nLen > strout.size())
		{
			nLen = strout.size() + 1;			
		}

		strncpy(pszOutBuffer, strout.c_str(), nLen);
	}	

	return true;
}

/** 获得总行数
@param   
@param   
@param   
@return  返回总行数
@note     
@warning 
@retval buffer 
*/ 
DWORD CCsvReader::GetRecordCount(void) const
{
	return (DWORD)m_vectorLines.size();
}

/** 获得字段的数目
@param   
@param   
@param   
@return  返回总字段数
@note     
@warning 
@retval buffer 
*/
DWORD CCsvReader::GetFieldCount(void) const
{
	return (DWORD)m_vectorFieldDesc.size();
}

/** 通过字段描述获得对应的列索引
@param   pszFieldDesc： 字段描述字符串
@param   
@param   
@return  返回相应的列索引，如果失败，则返回-1；
@note     
@warning 
@retval buffer 
*/
int	CCsvReader::GetFieldIndex(const char* pszFieldDesc) const
{
	if(pszFieldDesc == NULL)
	{
		return -1;
	}

	for(int i = 0; i < (int)m_vectorFieldDesc.size(); i++)
	{
		if(stricmp(m_vectorFieldDesc[i].c_str(), pszFieldDesc) == 0)
		{
			return i;
		}
	}

	return -1;
}

/** 通过列索引获取对应的字段描述
@param   dwCol： 列索引
@param   
@param   
@return  返回相应的字段描述字符串，如果列索引越界，则返回NULL；
@note     
@warning 
@retval buffer 
*/
const char* CCsvReader::GetFieldDesc(DWORD dwCol) const
{
	if(dwCol >= m_vectorFieldDesc.size())
	{
		return NULL;
	}

	if(dwCol < m_vectorFieldDesc.size())
	{
		return m_vectorFieldDesc[dwCol].c_str();
	}

	return NULL;
}

/** 通过列索引获得对应的字段类型(emUnKnow,emBool, emInt...)
@param   dwCol：列索引
@param   
@param   
@return  返回相应的列索引，如果失败，则返回emCSVUnKnow；
@note     
@warning 
@retval buffer 
*/
int CCsvReader::GetFieldType(DWORD dwCol) const
{
	if(dwCol < m_vectorFieldType.size())
	{
		return m_vectorFieldType[dwCol];
	}

	return (int)emCSVUnKnow;
}

/** 获取指定的宏的值
@param  pszMacroName 要获取的宏的名称 
@param   
@param   
@return  对应宏的值，如果不存在返回NULL
@note     
@warning 
@retval buffer 
*/
const char* CCsvReader::GetMacro(const char* pszMacroName) const
{
	if(pszMacroName == NULL)
	{
		return NULL;
	}

	THASH_MACROMAP::const_iterator it = m_hashMacroMap.find(pszMacroName);
	if(it != m_hashMacroMap.end())
	{
		return ((*it).second).c_str();
	}

	return NULL;
}

/** 找开文件
@param   pszFileName：文件名
@param   
@return  
*/
bool CCsvReader::Open(const char* pszFileName, bool bEncrypt)
{
	if(pszFileName == NULL)
	{
		return false;
	}

	m_curStrName = pszFileName;

	// 取得文件系统
	FileSystem * pFileSystem = getFileSystem();
	if(pFileSystem == NULL)
	{
		return false;
	}

	// 读取文件
	Stream * pStream = pFileSystem->open(pszFileName);
	if(pStream == NULL)
	{
		return false;
	}

	// 解密文件
	char * pFileBuffer = NULL;
	MemoryStream memorystream;
	if(bEncrypt)
	{
		int nFileLength = pStream->getLength();
		pFileBuffer = new char [nFileLength + 1];
		if(!pStream->read(pFileBuffer, nFileLength))
		{
			return false;
		}
		pStream->close();
		pStream->release();

		if(!makeMap((uchar *)pFileBuffer, nFileLength, 'LAND'))
		{
			return false;
		}

		memorystream.attach((uchar *)pFileBuffer, nFileLength);
		pStream = &memorystream;
	}
	
	// 解析
	if(!_Open(pStream))
	{
		if(pFileBuffer != NULL)
		{
			delete [] pFileBuffer;
			pFileBuffer = NULL;
		}
		
		// 关闭文件
		pStream->close();
		pStream->release();
		return false;
	}
	// 关闭文件
	pStream->close();
	
	pStream->release();
	if(pFileBuffer != NULL)
	{
		delete [] pFileBuffer;
		pFileBuffer = NULL;
	}
	return true;
}

/** 关闭
@param   
@param   
@return  
*/
void CCsvReader::Close(void)
{
	m_vectorLines.clear();
	m_vectorFieldDesc.clear();
	m_vectorFieldType.clear();
	m_nCurRow = m_nCurCol = m_nCurPos = -1;
}

/** 打开脚本
@param   
@param   
@return  
*/
bool CCsvReader::_Open(Stream *pStream)
{
	if(pStream == NULL)
	{
		return false;
	}	

	DWORD dwValidLineCount = 0;
	char  szBuffer[2048] = { 0 };
	uint dwLength = sizeof(szBuffer);
	bool  bRet = pStream->readString(szBuffer, dwLength);
	while(bRet)
	{
		// 检查注释行或者空行
		DWORD dwLen = strlen(szBuffer);
		if(dwLen > 0)
		{
			if(szBuffer[dwLen - 1] == '\r' || szBuffer[dwLen - 1] == '\n')
			{
				szBuffer[dwLen - 1] = 0;
				dwLen--;
			}
			if(dwLen > 1 && (szBuffer[dwLen - 2] == '\r' || szBuffer[dwLen - 2] == '\n'))
			{
				szBuffer[dwLen - 2] = 0;
				dwLen--;
			}
		}

		bool bMustContinue = false;
		if(dwLen > 0)
		{
			// 忽略空行
			int __nPos = 0;
			bool __bFlag = false;
			while(__nPos < dwLen)
			{
				if(szBuffer[__nPos] != m_szSeparator)
				{
					__bFlag = true;
				}
				__nPos++;
			}
			if(!__bFlag)
			{
				bMustContinue = true;
			}
			else
			{
				// 空格或制表符跳过
				int nPos = 0;
				while(szBuffer[nPos] && (szBuffer[nPos] == ' ' || szBuffer[nPos] == '\t')) 
				{
					nPos++;
				}

				if(nPos >= (int)dwLen)
				{
					bMustContinue = true;
				}
				else if(szBuffer[nPos] == '#')
				{
					// #macro
					if(nPos + 5 < (int)dwLen && memcmp(&szBuffer[nPos + 1], "macro", 5) == 0)
					{
						ParseMacro(&szBuffer[nPos + 6]);
					}

					bMustContinue = true;
				}
			}
		}
		else
		{
			bMustContinue = true;
		}

		if(!bMustContinue)
		{
			// 添加有意义的行
			if(dwValidLineCount == 0) // 添加字段描述行
			{
				ParseFieldDesc(szBuffer);
				dwValidLineCount++;
			}
			else if(dwValidLineCount == 1) // 添加字段类型行
			{
				ParseFieldType(szBuffer);
				dwValidLineCount++;
			}
			else // 添加实际的数据记录
			{
				// add by zjp
				// 中英文的标点符号可能会导致配置表读取失败，添加读配置表的时的一个报错机制
				// 判断机制：每一个数据行解析出来的字段数目与类型行的字段数目相同
				int nSize = m_vectorFieldType.size();
				size_t start_pos = std::string::npos;
				const std::string stringLine = szBuffer;
				size_t end_pos = stringLine.find_first_of(m_szSeparator);
				int nCount = 0;
				while(end_pos != std::string::npos) // find
				{
					nCount++;
					start_pos = end_pos;
					end_pos = stringLine.find_first_of(m_szSeparator, start_pos + 1);
					if(end_pos == std::string::npos && start_pos < stringLine.length())
					{
						end_pos = stringLine.length();
					}
				}

				if (nSize!=nCount)
				{
					ErrorLn("配置表可能中英文逗号有问题,文件名:"<<m_curStrName.c_str()<<"出问题的有效行的行号为:"<<m_vectorLines.size()+3);
				}

				m_vectorLines.push_back(szBuffer);
			}
		}

		// 读下一行
		dwLength = sizeof(szBuffer);
		bRet = pStream->readString(szBuffer, dwLength);
	}

	return true;
}

/** 取得何行何列字符
@param   
@param   
@return  
*/
bool CCsvReader::GetColString(DWORD dwRow, DWORD dwCol, char* pBuffer, DWORD dwBytes)
{
	if(dwRow >= m_vectorLines.size() || dwCol >= m_vectorFieldDesc.size())
	{
		return false;
	}

	if(pBuffer == NULL || dwBytes <= 0)
	{
		return false;
	}

	int nStartPos = 0, nEndPos = 0, nFrom = -1;	
	if(m_nCurRow == dwRow && m_nCurCol + 1 == (int)dwCol)
	{
		nFrom = m_nCurPos;
	}

	std::string &str = m_vectorLines[dwRow];
	GetColPos(str, dwCol, nStartPos, nEndPos, nFrom);
	if(nStartPos == -1 && nEndPos == -1) // no find
	{
		return false;
	}

	m_nCurRow = dwRow;
	m_nCurCol = dwCol;
	m_nCurPos = nEndPos;
	int nLen  = nEndPos - nStartPos - 1;
	if((int)dwBytes <= nLen)
	{
		nLen = dwBytes - 1;
	}
	else if(nLen == 0)
	{
		return false;
	}

	strncpy(pBuffer, &str[nStartPos + 1], nLen);
	pBuffer[nLen] = 0;

	// 是否是宏?
	THASH_MACROMAP::iterator it = m_hashMacroMap.find(pBuffer);
	if(it != m_hashMacroMap.end())
	{
		strncpy(pBuffer, ((*it).second).c_str(), dwBytes - 1);
		pBuffer[((*it).second).length()] = 0;
	}

	return true;
}

/** 
@param   
@param   
@return  
*/
void CCsvReader::GetColPos(const std::string &stringLine, DWORD dwCol, int &nStartPos, int &nEndPos, int nFrom)
{
	if(dwCol >= m_vectorFieldDesc.size())
	{
		return;
	}

	if(nFrom >= (int)stringLine.length() || dwCol >= m_vectorFieldDesc.size())
	{
		nStartPos = nEndPos = -1;
		return;
	}

	// 定位列的开始位置
	size_t start_pos = nFrom;
	if(start_pos == -1) // 从头开始找
	{
		for(int i = 0; i < (int)dwCol; i++)
		{
			start_pos = stringLine.find_first_of(m_szSeparator, start_pos + 1);
			if(start_pos == std::string::npos)
			{				
				nStartPos = nEndPos = -1;

				return;
			}
		}
	}

	// 定位列的结束位置
	size_t end_pos = stringLine.find_first_of(m_szSeparator, start_pos + 1);
	if(end_pos != std::string::npos)
	{
		nStartPos = start_pos;
		nEndPos = end_pos;
	}
	else
	{		
		nStartPos = start_pos;
		nEndPos = stringLine.length();
	}
}

/** 解析字段描述
@param   
@param   
@return  
*/
void CCsvReader::ParseFieldDesc(const std::string &stringLine)
{
	size_t start_pos = std::string::npos;
	size_t end_pos = stringLine.find_first_of(m_szSeparator);
	while(end_pos != std::string::npos) // find
	{
		std::string str = stringLine.substr(start_pos + 1, end_pos - start_pos - 1);
		m_vectorFieldDesc.push_back(str);
		start_pos = end_pos;
		end_pos = stringLine.find_first_of(m_szSeparator, start_pos + 1);
		if(end_pos == std::string::npos && start_pos < stringLine.length())
		{
			end_pos = stringLine.length();
		}
	}
}

/** 解析字段类型 
@param   
@param   
@return  
*/
void CCsvReader::ParseFieldType(const std::string &stringLine)
{
	size_t start_pos = std::string::npos;
	size_t end_pos = stringLine.find_first_of(m_szSeparator);
	while(end_pos != std::string::npos) // find
	{
		std::string str = stringLine.substr(start_pos + 1, end_pos - start_pos - 1);
		if(stricmp(str.c_str(), "bool") == 0)
		{
			m_vectorFieldType.push_back((int)emCSVBool);
		}
		else if(stricmp(str.c_str(), "int") == 0)
		{
			m_vectorFieldType.push_back((int)emCSVInt);
		}
		else if(stricmp(str.c_str(), "int64") == 0)
		{
			m_vectorFieldType.push_back((int)emCSVInt64);
		}
		else if(stricmp(str.c_str(), "float") == 0)
		{
			m_vectorFieldType.push_back((int)emCSVFloat);
		}
		else if(stricmp(str.c_str(), "string") == 0)
		{
			m_vectorFieldType.push_back((int)emCSVString);
		}
		else if(stricmp(str.c_str(), "intArray") == 0)
		{
			m_vectorFieldType.push_back((int)emCsvIntArray);
		}
		else
		{			
			m_vectorFieldType.push_back((int)emCSVUnKnow);
		}

		start_pos = end_pos;
		end_pos = stringLine.find_first_of(m_szSeparator, start_pos + 1);
		if(end_pos == std::string::npos && start_pos < stringLine.length())
		{
			end_pos = stringLine.length();
		}
	}
}

/** 解析宏
@param   
@param   
@return  
*/
bool CCsvReader::ParseMacro(const std::string &stringExp)
{
	size_t pos = stringExp.find('=');
	size_t len = stringExp.length();
	if(pos > 0 && pos < len)
	{
		int nNamePos;
		int nNameLen = 0;
		FilterSpace(stringExp.c_str(), 0, pos, nNamePos, nNameLen);
		int nValuePos;
		int nValueLen = 0;
		FilterSpace(stringExp.c_str(), pos + 1, len, nValuePos, nValueLen);

		if(nNameLen > 0 && nValueLen > 0)
		{
			std::string name = stringExp.substr(nNamePos, nNameLen);
			std::string value = stringExp.substr(nValuePos, nValueLen);
			m_hashMacroMap[name] = value;
			return true;
		}
	}

	return false;	
}

/** 去掉空格和回车符
@param   
@param   
@return  
*/
void CCsvReader::FilterSpace(const char* pszStr, int nFrom, int nTo, int &nPos, int &nlen)
{
	if(pszStr == NULL)
	{
		return;
	}

	if(nFrom == nTo)
	{
		nPos = -1;
		nlen = 0;
		return;
	}

	if(nFrom > nTo)
	{
		size_t tmp = nFrom;
		nFrom = nTo;
		nTo = tmp;
	}

	while(nFrom < nTo && pszStr[nFrom] && (pszStr[nFrom] == m_szSeparator || pszStr[nFrom] == ' ' || pszStr[nFrom] == '\t')) // 空格或制表符跳过
	{
		nFrom++;
	}

	nTo--;
	while(nTo > nFrom && pszStr[nTo] && (pszStr[nTo] == m_szSeparator || pszStr[nTo] == ' ' || pszStr[nTo] == '\t')) // 空格或制表符跳过
	{
		nTo--;
	}

	nPos = nFrom;
	nlen = nTo - nFrom + 1;
}

vector<string>& CCsvReader::GetFiledDescVector()
{
	return m_vectorFieldDesc;
}

vector<int>& CCsvReader::GetFiledTypeVector()
 {
 	return m_vectorFieldType;
 }
