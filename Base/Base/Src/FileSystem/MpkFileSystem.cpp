#include "stdafx.h"
#include "MpkFileSystem.h"
#include "MpkStream.h"
#include "listStream.h"

#define new RKT_NEW

namespace xs{

MpkFileSystem::MpkFileSystem() : m_pMpkManip(0)
{
}

MpkFileSystem::~MpkFileSystem()
{
}

bool MpkFileSystem::open(const char* streamSysPath)
{
	if(!m_pMpkManip)
	{
		m_pMpkManip = createMpkManip();
	}

	m_strMpkFileName = streamSysPath;
	return m_pMpkManip && m_pMpkManip->open(streamSysPath,false);
}

bool MpkFileSystem::find(const char* streamName)
{
	return m_pMpkManip->hasFile(streamName);
}

Stream* MpkFileSystem::get(const char* streamName)
{
	if(!find(streamName))return 0;

	IMpkFileManip *pMpkFileManip = m_pMpkManip->openFile(streamName);
	if(!pMpkFileManip)return 0;

	return new MpkStream(pMpkFileManip);
}

void MpkFileSystem::list(IListFileCallback* pCallback,void *pPara)
{
	if(!pCallback)return;
	const char* pFilename = m_pMpkManip->findFirstFile("*");
	while(pFilename)
	{
		pCallback->onGetStreamName(pFilename,pPara);
		pFilename = m_pMpkManip->findNextFile();
	}
	m_pMpkManip->findClose();
}

bool MpkFileSystem::close()
{
	m_strMpkFileName = "";
	return m_pMpkManip->close();
}

void MpkFileSystem::release()
{
	close();
	safeRelease(m_pMpkManip);
	delete this;
}

const char * MpkFileSystem::getFileSystemName(void)
{
	return m_strMpkFileName.c_str();
}

}
