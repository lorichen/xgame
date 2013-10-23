//==========================================================================
/**
* @file	  : XlsValue.h
* @author : 
* created : 2008-3-13   15:53
* purpose : 用于记录Excel数据脚本中的一个值
*/
//==========================================================================

#ifndef __XLSVALUE_H__
#define __XLSVALUE_H__

#include "Common.h"
#include "autostring.h"
#include <fstream>
#include "Trace.h"

namespace xs {

	/// 整型数组
	struct IntArray
	{
		int count;
		int* data;

		std::string intArrayToString(const char* compart = ";"){
			std::string str = "";
			for (int i=0; i<count; ++i)
			{
				char buf[16];
				sprintf_s(buf, sizeof(buf), "%d", data[i]);
				if (i < count - 1)
					str = str + buf + compart;
				else
					str += buf;
			}
			return str;
		}
	};

	inline void stringToIntArray(std::string str, IntArray& arr, char compart = ';')
	{
		int count = 0;
		int size = str.size();
		int begin = 0;
		for (int i=0; i<size; ++i)
		{
			if (str[i] == compart || i == size - 1)
			{
				std::string temp = str.substr(begin, i - begin + 1);
				arr.data[count] = atoi(temp.c_str());
				begin = i + 1;
				count++;
			}
		}
		arr.count = count;
	}

	inline void CinCsvFiledTypeString(std::ofstream& of, int type)
	{
		switch (type)
		{
		case 0:
			of << "bool" << ",";
			break;
		case 1:
			of << "int" << ",";
			break;
		case 2:
			of << "int64" << ",";
			break;
		case 3:
			of << "float" << ",";
			break;
		case 4:
			of << "string" << ",";
			break;
		case 5:
			of << "macro" << ",";
			break;
		case 6:
			of << "intArray" << ",";
			break;
		default:
			of << "" << ",";
			break;
		}
	}

	inline void CinCsvFillFiled(std::ofstream& of, int strCount, const char* fillStr = "")
	{
		for (int i=0; i<strCount; ++i)
		{
			of << fillStr << ",";
		}
	}

	/// Excel表格中的一个值对象
	struct XlsValue
	{
		enum {_unknow=-1, _int, _float, _string, _int_array};
		int	type;
		union{
			int		i;
			float	f;
			const autostring* as;
			IntArray* ia;
		};
		static RKT_EXPORT autostring sEmptyAutoString;
		static RKT_EXPORT IntArray sEmptyIntArray;

		XlsValue() : type(_unknow), i(0)			{}
		XlsValue(int val) : type(_int), i(val)		{}
		XlsValue(float val) : type(_float), f(val)	{}
		XlsValue(const char* s) : type(_string) { as = (s && s[0]) ? new autostring(s) : 0; }
		XlsValue(int* vals, int count) : type(_int_array)
		{
			if (count > 0 && vals)
			{
				ia = new IntArray;
				ia->data = new int[count];
				ia->count = count;
				memcpy(ia->data, vals, count*sizeof(int));
			}
			else
				ia = 0;
		}
		XlsValue(const XlsValue& rhs)
		{
			(*this) = rhs;
		}
		~XlsValue()
		{
			if (type == _string && as != 0) delete as;
			else if (type == _int_array && ia != 0)
			{
				delete ia->data;
				delete ia;
			}
		}

		XlsValue& operator = (const XlsValue& rhs)
		{
			type = rhs.type;
			if (type == _int) i = rhs.i;
			else if (type == _float) f = rhs.f;
			else if (type == _string) as = rhs.as ? new autostring(*rhs.as) : 0;
			else if (type == _int_array)
			{
				if (rhs.ia)
				{
					ia = new IntArray;
					ia->data = new int[rhs.ia->count];
					ia->count = rhs.ia->count;
					memcpy(ia->data, rhs.ia->data, ia->count*sizeof(int));
				}
				else ia = rhs.ia;
			}
			return (*this);
		}

		inline int getInt() const
		{
			if (type == _int)
				return i;

			Assert(type == _int);
			return 0;
		}
		inline void setInt(int val)
		{
			type = _int;
			i = val;
		}
		inline float getFloat() const
		{
			if (type == _float)
				return f;

			Assert(type == _float);
			return 0;
		}
		inline void setFloat(float val)
		{
			type = _float;
			f = val;
		}
		inline const autostring& getString() const
		{
			if (type == _string)
				return as ? *as : sEmptyAutoString;

			Assert(type == _string);
			return sEmptyAutoString;
		}
		inline void setString(const char* val)
		{
			type = _string;
			as = (val && val[0]) ? new autostring(val) : 0;
		}
		inline const IntArray& getIntArray() const
		{
			if (type == _int_array)
				return ia ? *ia : sEmptyIntArray;

			Assert(type == _int_array);
			return sEmptyIntArray;
		}
		inline void setIntArray(int* vals, int count)
		{
			type = _int_array;
			ia = 0;
			if (count > 0 && vals)
			{
				ia = new IntArray;
				ia->data = new int[count];
				ia->count = count;
				memcpy(ia->data, vals, count*sizeof(int));
			}
		}
	};


	/// Excel记录，基于索引的值数组(类似一个简化的vector)
	class XlsRecord
	{
		XlsValue*		mValues;	/// 值数组
		size_t			mCount;		/// 数组节点数目
		size_t			mCapacity;	/// 数组容量

	public:
		static RKT_EXPORT XlsValue	sEmptyValue;

	public:
		XlsRecord() : mValues(0), mCount(0), mCapacity(0) {}
		~XlsRecord()  { clear(); safeDeleteArray(mValues); }

		XlsValue& operator [](uint index) const
		{
			return (index < mCapacity) ? mValues[index] : sEmptyValue;
		}

		void reserve(size_t count)
		{
			if (count > mCapacity)
			{
				mCapacity = count;
				XlsValue* newValues = new XlsValue[mCapacity];
				if (mValues && mCount)
					memcpy(newValues, mValues, mCount * sizeof(XlsValue));
				safeDeleteArray(mValues);
				mValues = newValues;
			}
		}
		inline void clear()				{ mCount = 0; }
		inline size_t size() const		{ return mCount; }
		inline size_t capacity() const	{ return mCapacity; }

		int getInt(uint index) const					{ return (*this)[index].getInt(); }
		float getFloat(uint index) const				{ return (*this)[index].getFloat(); }
		const autostring& getString(uint index) const	{ return (*this)[index].getString(); }
		const IntArray& getIntArray(uint index) const	{ return (*this)[index].getIntArray(); }

		//用于技能编辑,add wangtao
		void setInt(uint index, uint val){
			(*this)[index].setInt(val);
		}
		void setFloat(uint index, float val){
			(*this)[index].setFloat(val);
		}
		void setString(uint index, const char* val){
			(*this)[index].setString(val);
		}
		void setIntArray(uint index, int* p, uint count){
			(*this)[index].setIntArray(p, count);
		}
	};


	/// 记录集，适合分大类小类的情况下，用于某些读取Excel数据脚本的应用中
	template<int _MaxSubRecordCount>
	class RecordSet
	{
		XlsRecord*	mRecords[_MaxSubRecordCount];	/// 索引为0的记录为大类记录，其他为子类记录

	public:
		RecordSet() { memset(mRecords, 0, sizeof(mRecords)); }
		~RecordSet()
		{
			for (int i=0; i<_MaxSubRecordCount; i++)
				safeDelete(mRecords[i]);
		}

		void setRecord(XlsRecord* record)			{ mRecords[0] = record; }
		const XlsRecord* getRecord() const			{ return mRecords[0]; }

		void setSubRecord(ulong index, XlsRecord* record)
		{
			if (index >= 0 && index < _MaxSubRecordCount)
			{
				mRecords[index] = record;
				return;
			}

			Assert(index >= 0 && index < _MaxSubRecordCount);
		}
		const XlsRecord* getSubRecord(ulong index) const
		{
			if (index >= 0 && index < _MaxSubRecordCount)
				return mRecords[index];

			Assert(index >= 0 && index < _MaxSubRecordCount);
			return NULL;
		}
	};




	/// 将一个字符串分解为整数列表
	template<class T>
	inline int parseIntArray(const char* str, /*in out*/T* arr, int count, const char sep = ';')
	{
		Assert(arr != NULL && count > 0);
		if (!str) return 0;

		char buf[32];
		int i = 0;
		int j = 0;
		const char* p = str;
		while (*p)
		{
			if (*p == sep)
			{
				p++;
				buf[i] = 0;
				i = 0;
				arr[j++] = (T)atoi(buf);
				if (j >= count)
					return j;
			}
			else
				buf[i++] = *p++;
		}

		if (i > 0)
		{
			buf[i] = 0;
			arr[j++] = atoi(buf);
		}

		return j;
	}

} // namespace

#endif // __XLSVALUE_H__