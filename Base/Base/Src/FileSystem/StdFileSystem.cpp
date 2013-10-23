#include "stdafx.h"
#include "StdFileSystem.h"
#include "FileStream.h"
#include "Trace.h"

#define new RKT_NEW

namespace xs{


StdFileSystem::StdFileSystem()
{
	m_path.clear();
}

StdFileSystem::~StdFileSystem()
{
}

bool StdFileSystem::open(const char* streamSysPath)
{
	Assert(stringIsValid(streamSysPath));
	Assert(isAbsolutePath(streamSysPath));
	Assert(isDirectory(streamSysPath));
	m_path = streamSysPath;
	return true;
}

bool StdFileSystem::find(const char* streamName)
{
	Assert(stringIsValid(streamName));
	// 这里只支持相对路径，但是上层可能传绝对路径 [4/7/2011 zgz]
	//Assert(!isAbsolutePath(streamName));

	if(isFile(streamName))return true;

	CPathA path = m_path;
	path.addTailSlash();
	path += streamName;
	Assert(isAbsolutePath(path.c_str()));

	return isFile(path.c_str());
}

Stream* StdFileSystem::get(const char* streamName)
{
	Assert(stringIsValid(streamName));
	//Assert(!isAbsolutePath(streamName));

	FileStream* stream = new FileStream;
	Assert(stream != NULL);

	CPathA path = m_path;
	//if(find(streamName))
	if(isAbsolutePath(streamName))
	{
		path = streamName;
	}
	else
	{
		path.addTailSlash();
		path += streamName;
	}

	Assert(isAbsolutePath(path.c_str()));

	stream->setPath(path.c_str());

	return static_cast<Stream*>(stream);
}

void StdFileSystem::list(IListFileCallback* pCallback,void *p)
{
	//TODO:
}

bool StdFileSystem::close()
{
	m_path.clear();
	return true;
}

void StdFileSystem::release()
{
	close();
	delete this;
}


const char* StdFileSystem::getFileSystemName(void)
{
	static const char * STD_FILE_SYSTEM_NAME = "标准文件系统";
	return STD_FILE_SYSTEM_NAME;
}

}
