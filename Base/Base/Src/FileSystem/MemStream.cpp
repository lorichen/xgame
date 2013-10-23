#include "stdafx.h"
#include "MemStream.h"
#include "FileStream.h"
#include "Trace.h"

#define new RKT_NEW

namespace xs{

/// 释放流对象
void MemoryStream::release()
{
	delete this;
}

MemoryStream::MemoryStream(uint growBytes)
{
	Assert(growBytes <= 0xffffffff);

	m_growBytes = growBytes;
	m_pos = 0;
	m_bufSize = 0;
	m_fileSize = 0;
	m_buffer = NULL;
	m_autoDelete = true;
}

MemoryStream::MemoryStream(uint	size,uint growBytes)
{
	Assert(size != 0);

	m_growBytes = growBytes;
	m_pos = 0;
	m_bufSize = size;
	m_fileSize = 0;
	m_buffer = new uchar[size];
	m_autoDelete = true;
}

MemoryStream::MemoryStream(uchar* buffer, uint bufSize, uint growBytes)
{
	Assert(buffer != NULL && bufSize != 0);
	Assert(growBytes <= 0xffffffff);

	m_growBytes = growBytes;
	m_pos = 0;
	m_bufSize = bufSize;
	m_fileSize = growBytes == 0 ? bufSize : 0;
	m_buffer = buffer;
	m_autoDelete = false;
}

MemoryStream::~MemoryStream()
{
	if (m_buffer)
		close();
	Assert(m_buffer == NULL);

	m_growBytes = 0;
	m_pos = 0;
	m_bufSize = 0;
	m_fileSize = 0;
	m_autoDelete = false;
}

void MemoryStream::attach(uchar* buffer, uint bufSize, uint growBytes)
{
	Assert(buffer != NULL && bufSize != 0);
	if (m_buffer)
		close();

	m_growBytes = growBytes;
	m_pos = 0;
	m_bufSize = bufSize;
	m_fileSize = growBytes == 0 ? bufSize : 0;
	m_buffer = buffer;
	m_autoDelete = false;
}

uchar* MemoryStream::detach()
{
	uchar* buffer = m_buffer;
	close();
	return buffer;
}

bool MemoryStream::open(const char* mode)
{
	return true;
}

bool MemoryStream::close()
{
	Assert(m_fileSize <= m_bufSize);

	//m_growBytes = 0; // close不等于析构，不能将这个变量清空
	m_pos = 0;
	m_bufSize = 0;
	m_fileSize = 0;
	if (m_buffer && m_autoDelete)
		_free(m_buffer);
	m_buffer = NULL;
	return true;
}

bool MemoryStream::readString(IN OUT char* str,IN OUT uint& length)
{
	if (str == NULL || length == 0)
		return true;
	if(m_pos >= m_fileSize)return false;

	char c;
	uint index = 0;
	while(read(&c,sizeof(c)))
	{
		if(c == '\r')
		{
			char c;
			if(read(&c,sizeof(c)))
			{
				if(c != '\n')
				{
					seek(-1,SEEK_CUR);
				}

				break;
			}
			else
			{
				break;
			}
		}

		str[index++] = c;
		if(index >= length - 1)break;
	}
	str[index] = 0;
	length = index;
	return true;
}

bool MemoryStream::read(IN OUT void* buffer, uint toRead)
{
	Assert(buffer != NULL);
	if (buffer == NULL || toRead == 0) 
		return false;

	if (m_pos >= m_fileSize) // eof
		return false;

	uint bytesOfRead = toRead;
	if (m_pos + toRead > m_fileSize)
		bytesOfRead = (uint)(m_fileSize - m_pos);

	__memcpy((uchar*)buffer, (uchar*)m_buffer + m_pos, bytesOfRead);
	m_pos += bytesOfRead;

	return true;
}

bool MemoryStream::write(const void* buffer, uint toWrite)
{
	Assert(buffer != NULL);
	if (buffer == NULL || toWrite == 0) 
		return false;

	if (m_pos + toWrite > m_bufSize)
	{
		if (m_growBytes == 0)
			return false;

		growFile(m_pos + toWrite);
	}

	Assert(m_pos + toWrite <= m_bufSize); // growFile success ?

	__memcpy((uchar*)m_buffer + m_pos, (uchar*)buffer, toWrite);

	m_pos += toWrite;

	if (m_pos > m_fileSize)
		m_fileSize = m_pos;

	return true;
}

bool MemoryStream::seek(int offset, uint from)
{
	Assert(from == SEEK_SET || from == SEEK_CUR || from == SEEK_END);

	int64 newPos = m_pos;

	if (from == SEEK_SET)
		newPos = offset;
	else if (from == SEEK_CUR)
		newPos += offset;
	else if (from == SEEK_END)
		newPos = m_fileSize + offset;	
	else
		return false;

	if (newPos < 0) // bad seek
		return false;

	if (static_cast<uint>(newPos) > m_fileSize) // 超过文件大小
	{
		if (m_growBytes == 0)
			return false;

		growFile((uint)newPos);
	}

	m_pos = (uint)newPos;

	return true;
}

bool MemoryStream::seekToBegin()
{
	return seek(0, SEEK_SET);
}

bool MemoryStream::seekToEnd()
{
	return seek(0, SEEK_END);
}

bool MemoryStream::flush() const
{
	return true;
}

uint MemoryStream::getLength() const
{
   return m_fileSize;
}

bool MemoryStream::setLength(uint newLen)
{
	Assert(newLen <= 0xffffffffUL);

	if (newLen > m_bufSize)
	{
		if (m_growBytes == 0)
			return false;

		growFile(newLen);
	}

	if (newLen < m_pos)
		m_pos = newLen;

	m_fileSize = newLen;
	return true;
}

int MemoryStream::getPosition() const
{
	return m_pos;
}


//////////////////////////// other /////////////////////////////////////
bool MemoryStream::isExist() const
{
	return true;
}

bool MemoryStream::isOpen() const
{
	return true;
}

bool MemoryStream::setPath(const char* path)
{
	Assert(stringIsValid(path));

	if (path)
		m_path = path;

	return true;
}

bool MemoryStream::remove()
{
	return false;
}

bool MemoryStream::rename(const char* newName)
{
	return true;
}

bool MemoryStream::save(const char* newName)
{
	if (m_buffer)
	{
		CPathA path = newName;
		CPathA parent = path.getParentDir();
		if (!parent.empty())
			createDirectoryRecursive(parent.c_str());

		FileStream fs(newName);
		bool ret = (fs.open("wb") && fs.write(m_buffer, m_fileSize) && fs.close());
		fs.close();
		if (ret)
			return true;
	}
	return false;
}

uchar* MemoryStream::getBuffer() const
{
	return m_buffer;
}

uint MemoryStream::getGrowBytes() const
{
	return m_growBytes;
}

uint MemoryStream::getBufferSize() const
{
	return m_bufSize;
}

uchar* MemoryStream::_alloc(uint bytes)
{
	return (uchar*)new uchar[bytes];
}

uchar* MemoryStream::_realloc(uchar* ptr, uint bytes)
{
	Assert(bytes > 0);	// bytes == 0 means free
	delete[] ptr;
	return (uchar*)new uchar[bytes];
}

#pragma intrinsic(memcpy) // 创建内存代码，无函数调用开销，加快程序速度
uchar* MemoryStream::__memcpy(uchar* dest, const uchar* src, uint bytes)
{
	Assert(dest != NULL);
	Assert(src != NULL);

	return (uchar*)memcpy(dest, src, bytes);
}
#pragma function(memcpy)

void MemoryStream::_free(uchar* ptr)
{
	Assert(ptr != NULL);

	delete[] ptr;
}

bool MemoryStream::growFile(uint newLen)
{
	if (newLen > m_bufSize)
	{
		// grow the buffer
		uint newBufSize = m_bufSize;

		// watch out for buffers which cannot be grown!
		Assert(m_growBytes != 0);

		// determine new buffer size
		while (newBufSize < newLen)
			newBufSize += m_growBytes;

		// allocate new buffer
		uchar* newBuf;
		if (m_buffer == NULL)
			newBuf = _alloc(newBufSize);
		else
		{
			newBuf = new uchar[newBufSize];
			memcpy(newBuf,m_buffer,m_bufSize);
			delete[] m_buffer;
			//newBuf = _realloc(m_buffer, newBufSize);
		}

		if (newBuf == NULL)
			return false;

		m_buffer = newBuf;
		m_bufSize = newBufSize;
	}
	return true;
}


}