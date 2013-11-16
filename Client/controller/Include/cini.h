#pragma	once

//#include "stringvalue.h"

#include <map>
#include <hash_map>
#include <string>
#include <fstream>

#define use_hash_map

#ifdef use_hash_map
#include <hash_map>
#else
#include <map>
#endif

//#include <strstream>
#include <sstream>
//#include "afxInclude.h"

//using namespace std;
/*
#ifdef use_hash_map
class hash_compare_string
{
public:
	const static size_t bucket_size = 4;
	const static size_t min_buckets = 8;
	size_t operator()(const std::string& skey) const
	{
		const char* p = skey.c_str();
		size_t value = 0;
		while(*p)
		{
			value *= 5;
			value += *p++;
		}
		return value;
	}
	bool operator() (const std::string& s1,const std::string& s2) const
	{
		return s1 < s2;
	}
};
*/
#ifdef use_hash_map
//typedef hash_map<std::string,std::string,hash_compare_string> iniMap;
typedef stdext::hash_map<std::string,std::string> iniMap;
#else
typedef std::map<std::string,std::string> iniMap;
#endif

class cIni
{
public:
	iniMap m_map;
	std::string m_strFile;
	BOOL Load(LPCSTR szFile)
	{
		m_strFile = szFile;
		std::ifstream f(szFile);
		if (!f.is_open())
			return FALSE;
		try
		{
			std::string skey,svalue,sequel;
			while(f.good())
			{
				//like 
				//file = c:\temp
				//fullscreen = 1
				f >> skey >> sequel >> svalue;
				m_map[skey] = svalue;
			}
		}
		catch (...) 
		{
		}
		return TRUE;
	}
	BOOL Load(LPCSTR szFile,bool bMsg)
	{
		m_strFile = szFile;
		std::ifstream f(szFile);
		if (!f.is_open())
		{
// 			if (bMsg)
// 			{
// 				std::string str = "没有找到文件:";
// 				str += szFile;
// 				AfxMessageBox((LPCTSTR)str.c_str());
// 			}
			return FALSE;
		}
		try
		{
			std::string skey,svalue,sequel;
			while(f.good())
			{
				//like 
				//file = c:\temp
				//fullscreen = 1
				f >> skey >> sequel >> svalue;
				m_map[skey] = svalue;
			}
		}
		catch (...) 
		{
		}
		return TRUE;
	}
	BOOL Create(LPCSTR szFile)
	{
		m_map.clear();
		m_strFile = szFile;
		return TRUE;
	}
	BOOL Save()
	{
		if (m_strFile.empty())
			return FALSE;
		std::ofstream f(m_strFile.c_str());
		if (!f.is_open())
			return FALSE;
		for (iniMap::iterator it = m_map.begin(); it != m_map.end(); ++it)
			f << (*it).first << " = " << (*it).second << "\n";
		return TRUE;
	}
	BOOL SaveAs(LPCSTR szNew)
	{
		m_strFile = szNew;
		return Save();
	}
	
	BOOL GetValue(LPCSTR szKey,std::string& value) const
	{
		iniMap::const_iterator it = m_map.find(szKey);
		if (it == m_map.end())
			return FALSE;
		value = (*it).second;
		return TRUE;
	}

	BOOL SetValue(LPCSTR szKey,const std::string& value)
	{
		if (value.empty())
			return FALSE;
		m_map[szKey] = value;
		return TRUE;
	}

	template <class T>
	BOOL GetValue(LPCSTR szKey, T& value) const
	{
		std::string sValue;
		if (GetValue(szKey,sValue))
		{
//			istrstream myString( sValue.c_str() );
			std::istringstream myString( sValue );
			myString >> value;
			return TRUE;
		}
		return FALSE;
	}

	template <class T>
	BOOL SetValue(LPCSTR szKey, const T& value)
	{
		std::string sValue;
//		ostrstream myString;
		std::ostringstream myString;
		myString << value;
		sValue = myString.str();
		SetValue(szKey,sValue);
		return FALSE;
	}
#ifdef _AFX
	template <class T>
		BOOL GetValue(LPCSTR szKey, T& value, int i) const
	{
		CString s;
		s.Format("%s%d",szKey,i);
		return GetValue(s,value);
	}

	template <class T>
		BOOL SetValue(LPCSTR szKey, const T& value, int i)
	{
		CString s;
		s.Format("%s%d",szKey,i);
		return SetValue(s,value);
	}

	template <class T>
		BOOL GetValue(LPCSTR szKey, T& value, int i, int j) const
	{
		CString s;
		s.Format("%s%d_%d",szKey,i,j);
		return GetValue(s,value);
	}

	template <class T>
		BOOL SetValue(LPCSTR szKey, const T& value, int i, int j)
	{
		CString s;
		s.Format("%s%d_%d",szKey,i,j);
		return SetValue(s,value);
	}
#endif
};
