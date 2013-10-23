#include "stdafx.h"
#include <vector>
#include "stream.h"
#include "MpkFileManip.h"

#define new RKT_NEW

namespace xs{

void MpkFileManip::release()
{
	if(m_hFile)
	{
		SFileCloseFile(m_hFile);
		m_hFile = 0;
	}

	delete this;
}

bool MpkFileManip::read(void *pBuffer,uint size)
{
	if(!SFileReadFile(m_hFile,pBuffer,size,0,0))return false;

	return true;
}

bool MpkFileManip::seek(uint length,uint from)
{
	DWORD seekFrom = FILE_CURRENT;
	switch(from)
	{
	case SEEK_SET:
		seekFrom = FILE_BEGIN;
		break;
	case SEEK_CUR:
		seekFrom = FILE_CURRENT;
		break;
	case SEEK_END:
		seekFrom = FILE_END;
		break;
	}

	return SFileSetFilePointer(m_hFile,length,0,seekFrom) != -1;
}

uint MpkFileManip::getPos()
{
	return SFileGetFilePointer(m_hFile);
}

uint MpkFileManip::size()
{
	DWORD high = 0;
	DWORD low = SFileGetFileSize(m_hFile,&high);
	return low;//(high << 32) | low;
}


MpkFileManip::MpkFileManip(void* hFile)
{
	m_hFile = hFile;
}


uint MpkFileManip::getOriginSize()
{
	return SFileGetFileInfo(m_hFile,SFILE_INFO_FILE_SIZE);
}

uint MpkFileManip::getCompressedSize()
{
	return SFileGetFileInfo(m_hFile,SFILE_INFO_COMPRESSED_SIZE);
}

bool MpkFileManip::isEncrypted()
{
	DWORD dwFlags = SFileGetFileInfo(m_hFile,SFILE_INFO_FLAGS);
	return (dwFlags & MPK_FILE_ENCRYPTED) != 0;
}

}