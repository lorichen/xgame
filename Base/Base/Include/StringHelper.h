/**
* Created by pk 2008.01.04
*/

#ifndef RKT_STRINGHELPER_H
#define RKT_STRINGHELPER_H

#include "Common.h"
#include <string>
#include <algorithm>

namespace xs {
	/**
		字符串辅助类，主要用于对一些常用的字符串的操作
		该类基本都是静态方法
	*/
	class RKT_EXPORT StringHelper
	{
	public:
		/// 替换字符
		static void replace(std::string& str, char _Old, char _New)
		{
			size_t _Pos = std::string::npos;
			while (str[++_Pos])
			{
				if (str[_Pos] == _Old)
					str[_Pos] = _New;
			}
		}

		/// 替换字符串
		static void replace(std::string& str, const std::string& _Old, const std::string& _New)
		{
			size_t _Oldlen = _Old.length();
			if (_Oldlen == 0)
				return;

			size_t _Newlen = _New.length();
			size_t _Pos = std::string::npos;
			_Pos = str.find(_Old.c_str(), _Pos + 1);
			while (_Pos != std::string::npos)
			{
				str.replace(_Pos, _Oldlen, _New.c_str(), _Newlen);
				_Pos += _Newlen - _Oldlen;
				_Pos = str.find(_Old.c_str(), _Pos + 1);
			}
		}

        //  替换字符串：只进行一次替换操作；
        static void ReplaceOnce(std::string& str, const std::string& _Old, const std::string& _New)
        {
            size_t _Oldlen = _Old.length();
            if (_Oldlen == 0)
            {
                ASSERT(false);
                return;
            }

            size_t _Newlen = _New.length();
            size_t _Pos = std::string::npos;
            _Pos = str.find(_Old.c_str(), _Pos + 1);
            if (_Pos != std::string::npos)
            {
                str.replace(_Pos, _Oldlen, _New.c_str(), _Newlen);
            }
        }

		/// 去除头尾的空格
		static void  trim(std::string& str,const std::string& val)
		{
			str.erase(0,str.find_first_not_of(val));
			str.erase(str.find_last_not_of(val)+val.size());
		}

		/// 大小写不敏感的字符串比较
		static int casecmp(const std::string& str1, const std::string& str2)
		{
#if defined(_MSC_VER) && _MSC_VER >= 1400
			return ::_stricmp(str1.c_str(), str2.c_str());
#else
			return ::stricmp(str1.c_str(), str2.c_str());
#endif
		}

		/// 格式化一个字符串
		static std::string& format(std::string& str, const char* _Format, ...)
		{
			va_list _Args;
			va_start(_Args, _Format);
			const size_t _Size = 1024;
			char _Tempbuf[_Size];
#if defined(_MSC_VER) && _MSC_VER >= 1400
			size_t _Ret = ::_vsnprintf_s(_Tempbuf, _Size - sizeof(char), 1024-1, _Format, _Args);
#else
			size_t _Ret = ::_vsnprintf(_Tempbuf, _Size - sizeof(char), _Format, _Args);
#endif
			if (_Ret == -1 || _Ret == _Size - sizeof(char))
			{
				_Tempbuf[_Size - sizeof(char)] = '\0';
			}
			va_end(_Args);
			str.assign(_Tempbuf);
			return str;
		}

		/// 转换成整数
		static int toInt(const std::string& str)
		{
			return ::atoi(str.c_str());
		}

		/// 转换成整数
		static long toLong(const std::string& str)
		{
			return ::atol(str.c_str());
		}

		/// 转换成int64
		static int64 toInt64(const std::string& str)
		{
			return ::_atoi64(str.c_str());
		}

		/// 转换成浮点
		static float toFloat(const std::string& str)
		{
			return (float)::atof(str.c_str());
		}

		/// 从整数转换成字符串
		static std::string& fromInt(std::string& str, int _Val, int _Radix = 10)
		{
			str.reserve(40);
#if defined(_MSC_VER) && _MSC_VER >= 1400
			::_itoa_s(_Val, (char*)str.c_str(), 40, _Radix);
#else
			str = _itoa(_Val, (char*)str.c_str(), _Radix);
#endif
			return str;
		}

		/// 从整数转换成字符串
		static std::string& fromLong(std::string& str, long _Val, int _Radix = 10)
		{
			str.reserve(40);
#if defined(_MSC_VER) && _MSC_VER >= 1400
			::_ltoa_s(_Val, (char*)str.c_str(), 40, _Radix);
#else
			str = _itoa(_Val, (char*)str.c_str(), _Radix);
#endif
			return str;
		}

		/// 从int64转换成字符串
		static std::string& fromInt64(std::string& str, int64 _Val, int _Radix = 10)
		{
			str.reserve(80);
#if defined(_MSC_VER) && _MSC_VER >= 1400
			::_i64toa_s(_Val, (char*)str.c_str(), 80, _Radix);
#else
			str = _i64toa(_Val, (char*)str.c_str(), _Radix);
#endif
			return str;
		}

		/// 从float转换成字符串
		static std::string& fromFloat(std::string& str, float _Val)
		{
			str.reserve(80);
#if defined(_MSC_VER) && _MSC_VER >= 1400
			::sprintf_s((char*)str.c_str(), 80, "%f", _Val);
#else
			::sprintf((char*)str.c_str(), "%f", _Val);
#endif
			return str;
		}

		/// 转换成小写
		static std::string& toLower(std::string& str)
		{
			std::transform(str.begin(), str.end(), str.begin(), ::tolower);
			return str;
		}

		/// 转换成大写
		static std::string& toUpper(std::string& str)
		{
			std::transform(str.begin(), str.end(), str.begin(), ::toupper);
			return str;
		}
	};

/**测试例子
	std::string s1;
	StringHelper::format(s1, "ab中文c%04d", 789);
	StringHelper::toUpper(s1);
	StringHelper::toLower(s1);

	int n = StringHelper::toInt("1234567890");
	int64 n64 = StringHelper::toInt64("1234567890123");
	float ff = StringHelper::toFloat("3.141592600");

	std::string sss;
	StringHelper::fromInt(sss, n);
	StringHelper::fromInt64(sss, n64);
	StringHelper::fromFloat(sss, ff);
	*/
}

#endif // RKT_STRINGHELPER_H