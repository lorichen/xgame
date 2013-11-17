#include "stdafx.h"
#include "MpkStream.h"
#include "IMpkManip.h"

#define new RKT_NEW

namespace xs{

MpkStream::MpkStream(IMpkFileManip *pFileManip) : m_pFileManip(pFileManip)
{
}

bool MpkStream::read(IN OUT void* buffer, uint toRead)
{
	return m_pFileManip->read(buffer,toRead);
}

bool MpkStream::readString(IN OUT char* str,IN OUT uint& length)
{
	if (str == NULL || length == 0)
		return true;
	if(getPosition() >= (int)getLength())return false;

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

bool MpkStream::seek(int offset, uint from) 
{
	return m_pFileManip->seek(offset,from);
}

bool MpkStream::seekToBegin() 
{
	return m_pFileManip->seek(0,SEEK_SET);
}

bool MpkStream::seekToEnd() 
{
	return m_pFileManip->seek(0,SEEK_END);
}

int MpkStream::getPosition() const
{
	return m_pFileManip->getPos();
}

uint MpkStream::getLength() 
{
	return m_pFileManip->getOriginSize();
}

void MpkStream::release()
{
	m_pFileManip->release();
	delete this;
}


}