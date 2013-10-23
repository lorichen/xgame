/**
	Created 2007.6.7 by pk
	Email: pk163@163.com
*/

#ifndef RKT_OSTRBUF_H
#define RKT_OSTRBUF_H

#include <stdlib.h>
#include <utility>
#include <string>

namespace xs {

/** 
	示例
	
	@code

	int dosomething(char* dest_buf, int max_size)
	{
		char buf[] = "some thing some thing some thing some thing ";
		int actualCpySize = max_size / 2 + 1;
		memcpy(dest_buf, buf, actualCpySize);
		return actualCpySize;
	}

	void test_ostrbuf()
	{
		ostrbuf osb;

		// 如果怕缓冲区不够，先直接设置一下
		//osb.reserve(1024);

		// 这些操作类似ostream的功能
		osb<<(uint)0xffffffff<<","<<(int)0x80000001<<endl;// endl即换行符
		osb<<99<<","<<(short)14<<","<<-15<<endl;
		osb<<(int64)0x8000000000000001<<","<<(uint64)0xffffffffffffffff<<endl;
		osb<<15.1367891024f<<","<<9999.123456789<<endl;

		 // 将一个字符串的子串写入
		osb<<ostrbuf::substr("我们你们他们abcdefg", 12)<<endl;

		// 定制的格式化
		osb<<ostrbuf::formatf("%02.6f", 3.1415926f)<<endl;
		osb<<_fi("%04d", 25)<<endl; // 这种写法更简洁
		osb<<_fi("%X", 255)<<endl;


		// 直接操作缓冲区示例（在不知情的情况下不推荐这样操作，知情的情况下，也不错，很方便，至少少了字符串拷贝操作）
		osb.grow(64);	// 可能需要128字节空间，这个数值必须正确才能防止缓存越界
		int off = dosomething(osb.cur(), 64); // dosomething 已经完全抛开了 ostrbuf 类，不受任何拘束，危险！
		osb.seek(off);	// 手动将位置seek到正确位置
		osb<<ends;		// 因上面的 dosomething 没有在缓冲区最后加入'\0'，这里不能忘记，除了像 dosomething 这样直接对缓冲区进行写外，其他的操作都会自动加'\0'

		std::cout<<osb.c_str();
	}

	//结果:
	4294967295,-2147483647
	99,14,-15
	-9223372036854775807,18446744073709551615
	15.136789,9999.123457
	我们你们他们
	3.141593
	0025
	FF
	some thing some thing some thing 

	@endcode
*/

/**
	字符串输出缓冲，功能类似ostream，但更加简洁

	@param _GrowBytes 缓冲区的递增大小
	@param _ArrSize 内部的字符数组的大小，当该字符数组容量不够时，才new出真正的缓冲区
*/
template <int _GrowBytes, int _ArrSize = 64>
class basic_ostrbuf
{
private:
	enum {_MyGrowBytes = _GrowBytes > 0 ? ((_GrowBytes + 7) & ~7) : 256};
	enum {_MyArrSize = _ArrSize > 0 ? ((_ArrSize + 7) & ~7) : 64};
	enum {_LongMaxLength = 16-1, _Int64MaxLength = 24-1, _DoubleMaxLength = 48-1}; // 8*n-1

	char*	_buf;				/// 缓冲区
	size_t	_pos;				/// 当前写入位置
	size_t	_cap;				/// 缓冲区容量
	char	_arr[_MyArrSize];	/// 字符数组，当操作小缓冲区时，可以利用他，这样就没必要new内存


	/// 用于用户自定义的格式化，基于sprintf函数的格式化功能
public:
	template<class T>
	struct format 
	{
		const char* _f;
		T _v;
		format(const char* f, const T& v) : _f(f), _v(v) { }
	};

	typedef format<float>					formatf;
	typedef format<double>					formatd;
	typedef format<int>						formati;
	typedef format<__int64>					formati64;
	typedef std::pair<const char*, size_t>	substr;

	// 构造（析构）
public:
	basic_ostrbuf() : _buf(_arr), _pos(0), _cap(_MyArrSize)
	{
		_arr[0] = 0;
	}
	~basic_ostrbuf()
	{
		if (_buf != _arr) delete[] _buf;
	}

	// 基本方法
public:
	void clear()
	{
		grow(1);
		_pos = 0;
		_buf[_pos] = 0;
	}

	char* str() const				{ return _buf; }
	const char* c_str() const		{ return _buf; }
	size_t length() const			{ return _pos; }
	size_t capacity() const			{ return _cap; }
	char* cur() const				{ return _buf + _pos;}
	basic_ostrbuf& seek(int offset)
	{
		int new_pos = (int)_pos + offset;
		if (new_pos >= (int)_cap) new_pos = (int)_cap - 1;
		if (new_pos < 0) new_pos = 0;
		_pos = (size_t)new_pos;
		return (*this);
	}

	/// 预定一个用户设定的缓冲大小（对于大缓冲或已知大小的缓冲，用这种方法可以减少new/delete的次数）
	void reserve(size_t length)
	{
		if (length >= _cap)
			grow(length - _pos);
	}

	// operator<<
public:
	/// 用于endl和ends
	basic_ostrbuf& operator<<(basic_ostrbuf& (__cdecl * fn)(basic_ostrbuf&)) 
	{
		return ((*fn)(*this));
	}

	/// 输入另一个对象
	basic_ostrbuf& operator<<(const basic_ostrbuf& other)
	{
		return push(other.c_str(), other.length());
	}


	/// 输入字符串
	basic_ostrbuf& operator<<(const char* str)
	{
		return (str ? push(str, strlen(str)) : (*this));
	}
	/// 输入一个子串
	basic_ostrbuf& operator<<(const substr& str)
	{
		return push(str.first, str.second);
	}
	/// 输入std::string对象
	basic_ostrbuf& operator<<(const std::string& str)
	{
		return push(str.c_str(), str.length());
	}
	/// 输入一个字符
	/// 注：这里c当字符处理，不当0-255的数字处理，0-255的数字，需要转换为short,int,long等进行处理
	basic_ostrbuf& operator<<(char c)
	{
		return put(c);
	}

	/// 导入数字
	basic_ostrbuf& operator<<(short int si)
	{
		return operator<<(long(si));
	}
	basic_ostrbuf& operator<<(unsigned short int usi)
	{
		return operator<<(unsigned long(usi));
	}
	basic_ostrbuf& operator<<(int __w64 i)
	{
#ifdef INT_64_BITS
		return operator<<(__int64(i));
#else
		return operator<<(long(i));
#endif
	}
	basic_ostrbuf& operator<<(unsigned int __w64 ui)
	{
#ifdef INT_64_BITS
		return operator<<(unsigned __int64(ui));
#else
		return operator<<(unsigned long(ui));
#endif
	}
	basic_ostrbuf& operator<<(long l)
	{
		grow(_LongMaxLength);
		_ltoa_s(l, cur(), _LongMaxLength, 10);
		_pos += strlen(cur());
		return (*this);
	}
	basic_ostrbuf& operator<<(unsigned long ul)
	{
		grow(_LongMaxLength);
		_ultoa_s(ul, cur(), _LongMaxLength, 10);
		_pos += strlen(cur());
		return (*this);
	}
	basic_ostrbuf& operator<<(__int64 i64)
	{
		grow(_Int64MaxLength);
		_i64toa_s(i64, cur(), _Int64MaxLength, 10);
		_pos += strlen(cur());
		return (*this);
	}
	basic_ostrbuf& operator<<(unsigned __int64 ui64)
	{
		grow(_Int64MaxLength);
		_ui64toa_s(ui64, cur(), _Int64MaxLength, 10);
		_pos += strlen(cur());
		return (*this);
	}
	basic_ostrbuf& operator<<(float f)
	{
		grow(_DoubleMaxLength);
		sprintf_s(cur(), _DoubleMaxLength, "%f", f);
		_pos += strlen(cur());
		return (*this);
	}
	basic_ostrbuf& operator<<(double d)
	{
		grow(_DoubleMaxLength);
		sprintf_s(cur(), _DoubleMaxLength, "%f", d);
		_pos += strlen(cur());
		return (*this);
	}

	/// 定制的格式化
	template<class T>
	basic_ostrbuf& operator<<(const format<T>& val)
	{
		grow(_DoubleMaxLength);
		sprintf_s(cur(), _DoubleMaxLength, val._f, val._v);
		_pos += strlen(cur());
		return (*this);
	}
public:
	void grow(size_t len)
	{
		if (_pos + len >= _cap)
		{
			// 计算新的尺寸，并8字节对齐
			size_t new_size = _pos + len + 1;
			size_t new_cap = _cap ? _cap : _MyGrowBytes;
			while (new_cap < new_size)
				new_cap += _MyGrowBytes;
			new_cap = (new_cap + 7) & ~7;

			char* new_buf = new char[new_cap];
			if (_pos > 0)
			{
				memcpy(new_buf, _buf, _pos + 1);
				if (_buf != _arr)
					delete[] _buf;
			}
			else
				new_buf[0] = 0;

			_buf = new_buf;
			_cap = new_cap;
		}
	}
	basic_ostrbuf& put(char c)
	{
		grow(1);
		_buf[_pos] = c;
		if (c != 0)
			_buf[++_pos] = 0;
		return (*this);
	}
	basic_ostrbuf& push(const char* str, size_t len) // 外部保证 len <= strlen(str)
	{
		if (str && len > 0)
		{
			grow(len);
			memcpy(_buf + _pos, str, len);
			_buf[_pos += len] = 0;
		}
		return (*this);
	}
};



/// 字符串终止
template<int _GrowBytes, int _ArrSize>
inline basic_ostrbuf<_GrowBytes, _ArrSize>& _cdecl ends(basic_ostrbuf<_GrowBytes, _ArrSize>& osb)
{
	return osb.put('\0');
}

/// 换行
template<int _GrowBytes, int _ArrSize>
inline basic_ostrbuf<_GrowBytes, _ArrSize>& _cdecl endl(basic_ostrbuf<_GrowBytes, _ArrSize>& osb)
{
	return osb.put('\n');
}

#if 0
/** 
	示例：用户自定义类型的定制方法，定义后可以这样处理你的自定义类型：
	YourType  yt;
	ostrbuf<<yt;
*/
template<int _GrowBytes, int _ArrSize>
inline basic_ostrbuf<_GrowBytes, _ArrSize>& _cdecl operator<<(basic_ostrbuf<_GrowBytes, _ArrSize>& osb, const YourType& val)
{
	if (val.c_str())
		osb.push(val.c_str(), val.length());
	return osb;
}
#endif


typedef basic_ostrbuf<256, 256>		ostrbuf;


// 为了方便使用，作这样的 typedef
typedef ostrbuf::formatf	_ff;
typedef ostrbuf::formatd	_fd;
typedef ostrbuf::formati	_fi;
typedef ostrbuf::formati64	_fi64;
typedef ostrbuf::substr		_ss;

}

#endif // RKT_OSTRBUF_H