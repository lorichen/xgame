#include "stdafx.h"

#include "Stream.h"
#include "Trace.h"
#include <assert.h>

#if (TARGET_PLATFORM == PLATFORM_WIN32)

#else
#include <dirent.h>
#include <sys/types.h>
#endif

#include <sys/stat.h>

#define new RKT_NEW

namespace xs{

//////////////////////////////////////////////////////////////////////////
// Œƒº˛∂‘œÛª˘¿‡
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
#if (TARGET_PLATFORM == PLATFORM_WIN32)
	_finddata_t fd;
	intptr_t r = _findfirst(absolutePath.c_str(), &fd); // "c:\"“≤ª·∑µªÿ£≠1£¨µ±ƒø¬º≤ª¥Ê‘⁄£¨“ÚŒ™«˝∂Ø∆˜≤ª «ƒø¬º
	_findclose(r);
	return (r != -1 && fd.attrib & 16);
#else
    DIR* dir;
    if(!(dir = opendir(absolutePath.c_str())))
        return false;
    return true;
#endif
}

inline static bool _isAbsolutePath(CPathA& path)
{
	//	’‚¿Ôµƒ≈–∂œÃıº˛≤ª∞≤»´£¨“ÚŒ™path.length()Œ™0 ±£¨∂‘pathµƒ∂¡≤Ÿ◊˜ «ø…ƒ‹ª·µº÷¬“Ï≥£µƒ£¨“Ú¥À’‚¿Ô–ﬁ∏ƒŒ™
	//	÷±Ω” π”√api ::PathIsRelative()£ª
#if (TAGET_PLATFORM == PLATFORM_WIN32)
#if 0
	return (path.length() >= 3 && path[0] == '\\' || path[1] == ':');
#endif
	return !::PathIsRelativeA(path.c_str());
    
#else
    return (path.length() >= 3 && path[0] == '\\') || path[0] == '/' || path[0] == '.' ||  path[0] == '\\'|| path[1] == ':' ;
#endif
}

RKT_API void toggleFullPath(CPathA& path)
{
#if (TAGET_PLATFORM == PLATFORM_WIN32)
	if (path.empty())
	{
		path = getWorkDir();
		return;
	}
	if (!_isAbsolutePath(path))
	{
		CPathA workPath;
        workPath = getWorkDir();
		workPath.addTailSlash();
		path.insert(0, workPath);
	}
#endif
}

RKT_API bool checkPath(const char* path, bool& isAbsolutePath, uint& attrib)
{
	CPathA mypath ;
    mypath = path;
	
	//toggleFullPath(mypath);
	if(mypath.empty())
	{
		attrib |= 16;
		return true;
	}

#if (TARGET_PLATFORM == PLATFORM_WIN32)
	_finddata_t fd;
	intptr_t r = _findfirst(mypath.c_str(), &fd); // "c:\"“≤ª·∑µªÿ£≠1£¨µ±ƒø¬º≤ª¥Ê‘⁄£¨“ÚŒ™«˝∂Ø∆˜≤ª «ƒø¬º
	attrib = fd.attrib;
	_findclose(r);
	return r != -1;
#else
    DIR* dir;
    if((dir = opendir(mypath.c_str())))
    {
        attrib |= 16;
        return true;
    }
    FILE* fp = fopen(mypath.c_str(),"rb");
    if(!fp)
    {
        attrib = 0;
        return false;
    }
    attrib = 0;
    fclose(fp);
    return true;
#endif
}

RKT_API bool isAbsolutePath(const char* path)
{
    CPathA p ;
    p = path;
	return _isAbsolutePath(p);
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

// µ›πÈ¥¥Ω®ƒø¬º(∞¸¿®∂‡º∂,absolutePath–Ëæ¯∂‘¬∑æ∂√˚)
// ◊‘∂ØºÏ≤‚ƒø¬º «∑Ò¥Ê‘⁄
RKT_API bool createDirectoryRecursive(const char* absolutePath)
{
	Assert(stringIsValid(absolutePath));

	CPathA strDir(absolutePath, false);

	if (_isDirectoryExist(strDir))
		return true;

	// ªÒ»°∏∏ƒø¬º
	CPathA strParent(strDir.getParentDir().c_str(), false);
	if (strParent.empty()) // ƒø¬º√˚≥∆¥ÌŒÛ
		return false; 

	bool ret = true;
	if (strParent.length() > 3) // »Áπ˚≥§∂»–°”⁄3£¨±Ì æŒ™¥≈≈Ã∏˘ƒø¬º
		ret = _isDirectoryExist(strParent);// ºÏ≤È∏∏ƒø¬º «∑Ò¥Ê‘⁄

	if (!ret) // ∏∏ƒø¬º≤ª¥Ê‘⁄,µ›πÈµ˜”√¥¥Ω®∏∏ƒø¬º
		ret = createDirectoryRecursive(strParent.c_str()); 

	if (ret) // ∏∏ƒø¬º¥Ê‘⁄,÷±Ω”¥¥Ω®ƒø¬º
	{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		int r = _mkdir(strDir.c_str());
#else
        int r = mkdir(strDir.c_str(),0777);
#endif
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

	// ≤Œ ˝≥§∂»±ÿ–Î¥Û”⁄3£¨º¥≤ªƒ‹Œ™¥≈≈Ã∏˘ƒø¬ºªÚø’∞◊
	if (strDir.empty() || strDir.length() <= 3)
		return false;

#if (TARGET_PLATFORM == PLATFORM_WIN32)
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
    
#else
    assert(0); //not imple yet
    
#endif

	return false;
}

static bool _browseDir(CPathA& absoluteDir, CPathA& relativeDir, bool (*DIRPROC)(const char*, const char*, uint), 
					  bool (*FILEPROC)(const char*, const char*, uint), uint userData, uint mask,bool recursive)
{
    
#if (TARGET_PLATFORM == PLATFORM_WIN32)
	intptr_t hFile;
	_finddata_t fd;

	CPathA strFiles = absoluteDir;
	strFiles += "/*.*";

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
			else //  «Œƒº˛
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
#else
    //assert(0);
    return false;
#endif
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