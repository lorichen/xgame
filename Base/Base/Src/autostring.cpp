/**
* Created by pk 2008.01.04
*/

#include "stdafx.h"
#include "autostring.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>

#define new RKT_NEW

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable:4267)
#endif

namespace xs {

	autostring::autostring()
		: std::string(), wstr(0) 
	{}
	autostring::autostring(const autostring& _Right)
		: std::string(_Right.c_str()), wstr(0)
	{}
	autostring::autostring(const autostring& _Right, size_type _Roff, size_type _Count)
		: std::string(_Right.c_str(), _Roff, _Count), wstr(0)
	{}
	autostring::autostring(const std::string& _Right)
		: std::string(_Right), wstr(0)
	{}
	autostring::autostring(const std::string& _Right, size_type _Roff, size_type _Count)
		: std::string(_Right, _Roff, _Count), wstr(0)
	{}
	autostring::autostring(const char* _Ptr)
		: std::string(_Ptr), wstr(0)
	{}
	autostring::autostring(const char* _Ptr, size_type _Count)
		: std::string(_Ptr, _Count), wstr(0)
	{}
	autostring::autostring(const char* _Ptr, size_type _Roff, size_type _Count)
		: std::string(_Ptr, _Roff, _Count), wstr(0)
	{}
	autostring::autostring(char _Ch)
		: std::string(1, _Ch), wstr(0)
	{}
	autostring::autostring(size_type _Count, char _Ch)
		: std::string(_Count, _Ch), wstr(0)
	{}

	autostring::~autostring()
	{
		clear_wstr();
	}

/*
	autostring& autostring::operator=(const autostring& _Right)
	{	// assign _Right
		assign(_Right);
		return *this;
	}

	autostring& autostring::operator=(const std::string& _Right)
	{	// assign _Right
		assign(_Right);
		return *this;
	}

	autostring& autostring::operator=(const char* _Ptr)
	{	// assign [_Ptr, <null>)
		assign(_Ptr);
		return *this;
	}

	autostring& autostring::operator=(char _Ch)
	{	// assign 1 * _Ch
		assign(1, _Ch);
		return *this;
	}

	autostring& autostring::operator+=(const autostring& _Right)
	{	// append _Right
		append(_Right.c_str());
		return *this;
	}

	autostring& autostring::operator+=(const std::string& _Right)
	{	// append _Right
		append(_Right.c_str());
		return *this;
	}

	autostring& autostring::operator+=(const char *_Ptr)
	{	// append [_Ptr, <null>)
		append(_Ptr);
		return *this;
	}

	autostring& autostring::operator+=(char _Ch)
	{	// append 1 * _Ch
		append((size_type)1, _Ch);
		return *this;
	}
*/
	// ◊™ªª
	const wchar_t* autostring::c_wstr()
	{
		if (!wstr) convert();
		return wstr;
	}

	// «Â≥˝wstrª∫¥Ê
	void autostring::clear_wstr()
	{
		if (wstr)
		{
			delete[] wstr;
			wstr = 0;
		}
	}

    
    size_t get_wchar_size(const char *str)
    {
        size_t len = strlen(str);
        size_t size=0;
        size_t i;
        for(i=0; i < len; i++)
        {
            if( str[size] >= 0 && str[size] <= 127 )
                size+=sizeof(wchar_t);
            else  // is chinanese char
            {
                size+=sizeof(wchar_t);
                i+=2;
            }
        }
        return size;
    }
    
    
	const wchar_t* autostring::to_wchar(const char* str)
	{
#ifdef RKT_WIN32
		if (str == 0)
			return 0;
        
		UINT const cp = GetACP();
		int const len = ::MultiByteToWideChar(cp, 0, str, -1, 0, 0);
        
		wchar_t* wstr = new wchar_t[len];
		::MultiByteToWideChar(cp, 0, str, -1, wstr, len);
        
		return wstr;
#else
        if(!str)
            return 0;
        
        size_t size_of_ch = strlen(str)*sizeof(char);
        size_t size_of_wc = get_wchar_size(str);
        wchar_t *pw = 0;
        if(!(pw =  new wchar_t[size_of_wc]))
        {
            //printf("malloc fail");
            return NULL;
        }
        mbstowcs(pw,str,size_of_wc);
        return pw;
#endif
	}
    
	void autostring::convert()
	{
		wstr = autostring::to_wchar(this->c_str());
	}
    
} // namespace xs

#ifdef _MSC_VER
#	pragma warning(pop)
#endif
