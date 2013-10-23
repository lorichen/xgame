#include "stdafx.h"
#include <sys/stat.h>
#include <direct.h>
#include "Stream.h"
#include "Trace.h"

#define new RKT_NEW

namespace xs{

//////////////////////////////////////////////////////////////////////////
// 文件对象基类
Stream::Stream(const char* path)
{
	m_path.isFile(true);
	m_path = path;
}

Stream::~Stream()
{
	close();
}

void Stream::release()
{
	close();
	delete this;
}

bool Stream::setPath(const char* path)
{
	Assert(stringIsValid(path));

	if (isOpen())
		close();

	if (path)
		m_path = path;

	return true;
}


//////////////////////////////////////////////////////////////////////////
inline static bool _isDirectoryExist(CPathA& absolutePath)
{
	_finddata_t fd;
	intptr_t r = _findfirst(absolutePath.c_str(), &fd); // "c:\"也会返回－1，当目录不存在，因为驱动器不是目录
	_findclose(r);
	return (r != -1 && fd.attrib & 16);
}

inline static bool _isAbsolutePath(CPathA& path)
{
	//	这里的判断条件不安全，因为path.length()为0时，对path的读操作是可能会导致异常的，因此这里修改为
	//	直接使用api ::PathIsRelative()；
#if 0
	return (path.length() >= 3 && path[0] == '\\' || path[1] == ':');
#endif
	return !::PathIsRelativeA(path.c_str());
}

RKT_API void toggleFullPath(CPathA& path)
{
	if (path.empty())
	{
		path = getWorkDir();
		return;
	}

	if (!_isAbsolutePath(path))
	{
		CPathA workPath = getWorkDir();
		workPath.addTailSlash();
		path.insert(0, workPath);
	}
}

RKT_API bool checkPath(const char* path, bool& isAbsolutePath, uint& attrib)
{
	CPathA mypath = path;
	toggleFullPath(mypath);

	_finddata_t fd;
	intptr_t r = _findfirst(mypath.c_str(), &fd); // "c:\"也会返回－1，当目录不存在，因为驱动器不是目录
	attrib = fd.attrib;
	_findclose(r);

	return r != -1;
}

RKT_API bool isAbsolutePath(const char* path)
{
	return _isAbsolutePath(CPathA(path));
}

RKT_API bool isFile(const char* path)
{
	bool isAbsolutePath;
	uint attrib;
	bool ret = checkPath(path, isAbsolutePath, attrib);
	return (ret && !(attrib & 16));
}

RKT_API bool isDirectory(const char* path)
{
	bool isAbsolutePath;
	uint attrib;
	bool ret = checkPath(path, isAbsolutePath, attrib);
	return (ret && (attrib & 16));
}

// 递归创建目录(包括多级,absolutePath需绝对路径名)
// 自动检测目录是否存在
RKT_API bool createDirectoryRecursive(const char* absolutePath)
{
	Assert(stringIsValid(absolutePath));

	CPathA strDir(absolutePath, false);

	if (_isDirectoryExist(strDir))
		return true;

	// 获取父目录
	CPathA strParent(strDir.getParentDir(), false);
	if (strParent.empty()) // 目录名称错误
		return false; 

	bool ret = true;
	if (strParent.length() > 3) // 如果长度小于3，表示为磁盘根目录
		ret = _isDirectoryExist(strParent);// 检查父目录是否存在

	if (!ret) // 父目录不存在,递归调用创建父目录
		ret = createDirectoryRecursive(strParent.c_str()); 

	if (ret) // 父目录存在,直接创建目录
	{
		int r = _mkdir(strDir.c_str());
		//if (r == -1) // error
		//	setLastError(UNLERR_STREAM_CREATE_DIRECTORY);
		ret = r != -1;
	}

	return ret;
}

RKT_API bool removeDirectoryRecursive(const char* absolutePath)
{
	Assert(stringIsValid(absolutePath));

	CPathA strDir(absolutePath, false);

	// 参数长度必须大于3，即不能为磁盘根目录或空白
	if (strDir.empty() || strDir.length() <= 3)
		return false;

	CPathA strFiles(strDir, true);
	strFiles += "\\*.*";

	_finddata_t fd;
	intptr_t hFind = _findfirst(strFiles.c_str(), &fd);
	if (hFind == -1 || !(fd.attrib & 16)) // directory not exist, return true
		return true; 

	bool ret = true;
	if (hFind != -1) // directory exist
	{
		bool bFind = true;
		while (bFind)
		{
			CPathA strTmp = strDir;
			if (fd.attrib & 16) // remove all sub directory
			{ 
				if (fd.name[0] != '.') // "." ".."
				{
					strTmp.addTailSlash();
					strTmp += fd.name;
					bool rb = true;
					ret = (rb = removeDirectoryRecursive(strTmp.c_str())) && ret;
				}
			}
			else // remove all files in this directory
			{ 
				strTmp.addTailSlash();
				strTmp +=  fd.name;
				_chmod(strTmp.c_str(), _S_IWRITE); // disable readonly flags
				bool rb = true;
				ret = (rb = (remove(strTmp.c_str()) == 0)) && ret;
				if (!rb)
				{
					//TRACE1_ERROR("removeFile: %s\n", strTmp.c_str());
				}
			}

			bFind = _findnext(hFind, &fd) != -1;
		}
		_findclose(hFind);

		if (ret)
		{
			if (_rmdir(strDir.c_str()) != 0)
			{
				//TRACE1_ERROR("removeDirectory: %s\n", strDir.c_str());
				return false;
			}
			return true;
		}
	}

	return false;
}

static bool _browseDir(CPathA& absoluteDir, CPathA& relativeDir, bool (*DIRPROC)(const char*, const char*, uint), 
					  bool (*FILEPROC)(const char*, const char*, uint), uint userData, uint mask,bool recursive)
{
	intptr_t hFile;
	_finddata_t fd;

	CPathA strFiles = absoluteDir;
	strFiles += "\\*.*";

	if ((hFile = _findfirst(strFiles.c_str(), &fd)) != -1)
	{
		do
		{
			if (fd.attrib & 16) // is directory!
			{
				if (mask & 1)
				{
					if (fd.name[0] != '.') // for ".", ".."
					{
						if (!absoluteDir.empty())
							absoluteDir.addTailSlash();
						if (!relativeDir.empty())
							relativeDir.addTailSlash();

						absoluteDir += fd.name;
						relativeDir += fd.name;

						if (DIRPROC != NULL &&	false == (DIRPROC)(absoluteDir.c_str(), relativeDir.c_str(), userData))
							return false;

						if(recursive)
						{
							if (!_browseDir(absoluteDir, relativeDir, DIRPROC, FILEPROC, userData, mask,recursive))
								return false;
						}

						absoluteDir = absoluteDir.getParentDir();
						relativeDir = relativeDir.getParentDir();
					}
				}
			}
			else // 是文件
			{
				if (mask & 2)
				{
					CPathA strFileName = absoluteDir;
					CPathA relativeFile = relativeDir;

					if (!strFileName.empty())
						strFileName.addTailSlash();
					if (!relativeFile.empty())
						relativeFile.addTailSlash();

					strFileName += fd.name;
					relativeFile += fd.name;

					if (FILEPROC != NULL &&	false == (FILEPROC)(strFileName.c_str(), relativeFile.c_str(), userData))
						return false;
				}
			}
		} while (_findnext(hFile, &fd) != -1);

		_findclose(hFile);
	}

	return true;
}

RKT_API bool browseDirectory(const char* absoluteDir, const char* relativeDir, bool (*DIRPROC)(const char*, const char*, uint), 
					  bool (*FILEPROC)(const char*, const char*, uint), uint userData, uint mask,bool recursive)
{
	CPathA absolutePath(absoluteDir, false);
	CPathA relativePath(relativeDir, false);
	Assert(!absolutePath.empty());

	return _browseDir(absolutePath, relativePath, DIRPROC, FILEPROC, userData, mask,recursive);
}


}