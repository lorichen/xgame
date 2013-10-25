/**
* Created by pk 2008.01.07
*/

#ifndef RKT_STRINGHASH_H
#define RKT_STRINGHASH_H

#include "Common.h"
#include <functional>

#ifdef RKT_COMPILER_MSVC
#	pragma warning(push)
#	pragma warning(disable:4996)
#endif


namespace xs {
    struct StrHashKeyType
	{
		ulong name;
		ulong name1;
		ulong name2;
	};
}

#if (TARGET_PLATFORM != PLATFORM_WIN32)
namespace __gnu_cxx {
   /*
    template<> struct hash< xs::StrHashKeyType >
    {
        size_t operator()(const xs::StrHashKeyType& x) const
        {
            size_t s = hash< unsigned long >()( x.name ) +
            hash< unsigned long >()( x.name1 ) +
            hash< unsigned long >()( x.name2 ) ;
            
            return hash<size_t>()(s);
        }
    };
    */
}

#endif

namespace xs {
    
    
	
    /// 字符串映射成整数
	#define STR2ID(str)	hashString((str), 0)

	/// 哈希一个字符串
	RKT_API ulong hashString(const char* str, ulong type = 0);

	/// 加密一段内存
	RKT_API bool makeMap(uchar* buffer, ulong size, ulong key);


	
	struct my_less
	{
		bool operator()(const xs::StrHashKeyType& l, const xs::StrHashKeyType& r) const
		{
			return (l.name1 == r.name1 && l.name2 == r.name2) ? false : true;
		}
	};
    
#if (TARGET_PLATFORM == PLATFORM_WIN32)
	template<class T>
	class StrHashMap: public HashMap<StrHashKeyType, T, hash_compare< xs::StrHashKeyType, my_less>  >
#else
    template<class T>
	class StrHashMap : public HashMap<StrHashKeyType, T  >
#endif
    {
	public:
        // kevin.chen hide , i am not sure!!!
        /*
        T& operator[](const char* str)
		{
			StrHashKeyType _Keyval;
			calcHashValue(_Keyval, str);
			iterator _Where = this->lower_bound(_Keyval);
			if (_Where == this->end())
				_Where = this->insert(value_type(_Keyval, mapped_type())).first;
			return ((*_Where).second);
		}
		T& operator[](StrHashKeyType key)
		{
			iterator _Where = this->lower_bound(key);
			if (_Where == this->end())
				_Where = this->insert(value_type(key, mapped_type())).first;
			return ((*_Where).second);
		}
		iterator find(const char* str)
		{
			StrHashKeyType _Keyval;
			calcHashValue(_Keyval, str);
			return lower_bound(_Keyval);
		}
		const_iterator find(const char* str) const
		{
			StrHashKeyType _Keyval;
			calcHashValue(_Keyval, str);
			return lower_bound(_Keyval);
		}
        */

		StrHashKeyType strToKeyType(const char* str)
		{
			StrHashKeyType _Keyval;
			calcHashValue(_Keyval, str);
			return _Keyval;
		}
	private:
		inline void calcHashValue(StrHashKeyType& key, const char* str)
		{
			key.name = hashString(str, 0);
			key.name1 = hashString(str, 1);
			key.name2 = hashString(str, 2);
		}
	};
} // namespace

#ifdef RKT_COMPILER_MSVC
#	pragma warning(pop)
#endif


namespace std
{
	inline size_t hash_value(xs::StrHashKeyType key)
	{
		return key.name;
	};
}


#endif // RKT_STRINGHASH_H