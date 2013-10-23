#include "stdafx.h"
#include "DataStream.h"

#define new RKT_NEW

namespace xs{

DataStream::DataStream() : m_impl() {}

DataStream::DataStream(const void* pBuffer, size_t nSize)
: m_impl(pBuffer, nSize, nSize)
{}

DataStream::DataStream(const DataStream& rhs)
: m_impl(rhs.data(), rhs.size(), rhs.size(), rhs.m_impl.m_nCurrent)
{}

DataStream& DataStream::operator=(const DataStream& rhs)
{
	if (this != &rhs)
	{
		DataStream temp(rhs);
		m_impl.Swap(temp.m_impl);
	}
	return *this;
}

DataStream::~DataStream()
{
}

// Get methods
const void* DataStream::data() const { return m_impl.m_pData; }
const void* DataStream::dataEnd() const { return (m_impl.m_pData + m_impl.m_nSize); }
size_t DataStream::size() const { return m_impl.m_nSize; }
size_t DataStream::capacity() const { return m_impl.m_nCapacity; }

bool DataStream::atEnd() const { return (m_impl.m_nCurrent >= m_impl.m_nSize); }

// Set methods
void DataStream::empty() { m_impl.Empty(); }

DataStream& DataStream::operator<<(bool v)				{ return _Append(v); }
DataStream& DataStream::operator<<(short v)				{ return _Append(v); }
DataStream& DataStream::operator<<(ushort v)	{ return _Append(v); }
DataStream& DataStream::operator<<(int v)				{ return _Append(v); }
DataStream& DataStream::operator<<(uint v)		{ return _Append(v); }
DataStream& DataStream::operator<<(long v)				{ return _Append(v); }
DataStream& DataStream::operator<<(ulong v)		{ return _Append(v); }

DataStream& DataStream::operator<<(const char* p)
{
	_AppendWithLen(p, strlen(p));
	return *this;
}
DataStream& DataStream::operator<<(const wchar_t* p)
{
	_AppendWithLen(p, wcslen(p) * sizeof(wchar_t));
	return *this;
}

template<class Traits>
DataStream& DataStream::operator<<(const std::basic_string<Traits>& s)	
{
	_AppendWithLen(s.c_str(), s.length() * sizeof(Traits));
	return *this;
}

DataStream& DataStream::operator<<(const DataStream& rhs)
{
	_AppendWithLen(rhs.data(), rhs.size());
	return *this;
}

DataStream& DataStream::append(const void* pBuffer, size_t nSize)
{
	m_impl.Append(pBuffer, nSize);
	return *this;
}

DataStream& DataStream::write(const void* pBuffer, size_t nSize)
{
	m_impl.Append(pBuffer, nSize);
	return *this;
}

DataStream& DataStream::operator>>(bool& v)				{ return _Read(v); }
DataStream& DataStream::operator>>(short& v)			{ return _Read(v); }
DataStream& DataStream::operator>>(ushort& v)	{ return _Read(v); }
DataStream& DataStream::operator>>(int& v)				{ return _Read(v); }
DataStream& DataStream::operator>>(uint& v)		{ return _Read(v); }
DataStream& DataStream::operator>>(long& v)				{ return _Read(v); }
DataStream& DataStream::operator>>(ulong& v)	{ return _Read(v); }


template<class Traits>
DataStream& DataStream::operator>>(std::basic_string<Traits>& s)
{
	size_t nSize = 0;
	m_impl.ReadData(&nSize, sizeof(size_t));

	s = std::basic_string<Traits>(reinterpret_cast<const Traits*>(m_impl.GetCurrentPtr(nSize)), nSize / sizeof(Traits));
	m_impl.MovePtr(nSize);

	return *this;
}

DataStream& DataStream::operator>>(DataStream& rhs)
{
	size_t nSize = 0;
	m_impl.ReadData(&nSize, sizeof(size_t));

	rhs = DataStream(m_impl.GetCurrentPtr(nSize), nSize);
	m_impl.MovePtr(nSize);

	return *this;
}

}