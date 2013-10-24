#include "stdafx.h"
#include <vector>
#include "stream.h"
#include "MpkManip.h"
#include "MpkFileManip.h"

#define new RKT_NEW

namespace xs{

	RKT_API bool copyFile(const char* existingFileName,const char* newFileName,bool bFailIfExists)
	{
#ifdef WIN32
		return CopyFile(existingFileName,newFileName,bFailIfExists) != FALSE;
#else
		return false;
#endif
	}

	RKT_API void getTempFileName(const char* path,char* szFilename)
	{
#ifdef WIN32
		GetTempFileName(path,"unl",0,szFilename);
#else
#endif
	}
	RKT_API void getTempPath(uint len,char* szPath)
	{
#ifdef WIN32
		GetTempPath(len,szPath);
#else
#endif
	}

	/// 创建Mpk文件操作对象
	IMpkManip* createMpkManip()
	{
		return new MpkManip;
	}

	void MpkManip::release()
	{
		close();
		delete this;
	}

	bool MpkManip::open(const char* mpkFilename,bool bCreateNew)
	{
		close();
		return SFileCreateArchiveEx(mpkFilename,bCreateNew ? CREATE_ALWAYS : OPEN_EXISTING,0x8000,&m_hMpk) != FALSE;
	}

	bool MpkManip::close()
	{
		if(!m_hMpk)return true;

		bool bOk = SFileCloseArchive(m_hMpk) != FALSE;
		if(bOk)
		{
			m_hMpk = 0;
		}

		return bOk;
	}

	bool MpkManip::hasFile(const char* filename)
	{
		return SFileHasFile(m_hMpk,(char*)filename) != FALSE;
	}

	IMpkFileManip* MpkManip::openFile(const char* filename)
	{
		if(!hasFile(filename))return 0;

		HANDLE hFile = 0;
		if(!SFileOpenFileEx(m_hMpk,filename,0,&hFile))return 0;

		return new MpkFileManip(hFile);
	}

	bool MpkManip::extractDir(const char* source,const char* target,bool replaceExist,IMpkManipCallback *pCallback,void *p)
	{
		bool bOk = true;

		std::string strSource = source;
		if(!strSource.empty())
			strSource = strSource + "\\*"; 
		else
			strSource = "*";

		SFILE_FIND_DATA fileFindData;
		HANDLE hFind = SFileFindFirstFile(m_hMpk,strSource.c_str(),&fileFindData,0);
		if(hFind)
		{
			do
			{
				std::string targetFilename = std::string(target) + "\\" + fileFindData.cFileName;
				if(pCallback)
				{
					pCallback->onProcessFile(fileFindData.cFileName,p);
				}

				if(!extractFile(fileFindData.cFileName,targetFilename.c_str(),replaceExist))
				{
					bOk = false;
				}
			}
			while(SFileFindNextFile(hFind,&fileFindData));
			SFileFindClose(hFind);
		}

		return bOk;
	}

	//source和target都是文件名
	bool MpkManip::extractFile(const char* source,const char* targetFilename,bool replaceExist)
	{
		bool bOk = false;
		CPathA sourceFilename(source);
		CPathA path(targetFilename);
		CPathA parent(path.getParentDir().c_str());
		if (!parent.empty())
			createDirectoryRecursive(parent.c_str());

		if(isFile(targetFilename) && !replaceExist)return true;

		return SFileExtractFile(m_hMpk,source,targetFilename) != FALSE;
	}

	bool MpkManip::removeDir(const char* dir)
	{
		bool bOk = true;

		std::string strDir = dir;
		if(!strDir.empty())
			strDir = strDir + "\\*"; 
		else
			strDir = "*";

		SFILE_FIND_DATA fileFindData;
		HANDLE hFind = SFileFindFirstFile(m_hMpk,strDir.c_str(),&fileFindData,0);
		if(hFind)
		{
			do
			{
				if(!removeFile(fileFindData.cFileName))
				{
					bOk = false;
				}
			}
			while(SFileFindNextFile(hFind,&fileFindData));
			SFileFindClose(hFind);
		}

		return bOk;
	}

	bool MpkManip::removeFile(const char* filename)
	{
		return (SFileRemoveFile(m_hMpk,filename) != FALSE);
	}

	bool MpkManip::renameFile(const char* source,const char* dest)
	{
		return (SFileRenameFile(m_hMpk,source,dest) != FALSE);
	}

	bool MpkManip::addFile(const char* sourceFilename,const char* targetFilename,bool replaceExist,bool compress,bool encrypt)
	{
		DWORD dwFlags = 0;
		if(replaceExist)dwFlags |= MPK_FILE_REPLACEEXISTING;
		if(compress)	dwFlags |= MPK_FILE_COMPRESS_PKWARE;

		//TODO:加密文件会导致Rename出问题，先关闭这个功能
		//crr  还原支持加密 2010-08-04
		if(encrypt)		dwFlags |= MPK_FILE_ENCRYPTED;
		return SFileAddFile(m_hMpk,sourceFilename,targetFilename,dwFlags) != FALSE;
	}


	bool MpkManip::dirProc(const char* path, const char* relativePath, uint userData)
	{
		return true;
	}

	bool MpkManip::fileProc(const char* path, const char* relativePath, uint userData)
	{
		_Data *pData = (_Data*)userData;
		if(pData->pCallback)pData->pCallback->onProcessFile(path,pData->pCallbackData);
		return pData->pThis->addFile(path,relativePath,pData->bReplaceExist,pData->compress,pData->encrypt);
	}

	bool MpkManip::addDir(const char* sourceDir,const char* targetDir,bool replaceExist,bool compress,bool encrypt,IMpkManipCallback *pCallback,void *p)
	{
		_Data data;
		data.pThis = this;
		data.bReplaceExist = replaceExist;
		data.pCallback = pCallback;
		data.pCallbackData = p;
		data.compress = compress;
		data.encrypt = encrypt;
		return browseDirectory(sourceDir,targetDir,dirProc,fileProc,(uint)&data,bdm_file|bdm_dir);
	}

	void MpkManip::dumpListFile()
	{
		SFILE_FIND_DATA fileFindData;
		HANDLE hFind = SFileFindFirstFile(m_hMpk,"*",&fileFindData,0);
		if(hFind)
		{
			do
			{
#ifdef _WIN32
				OutputDebugString(fileFindData.cFileName);
				OutputDebugString("\n");
#endif
			}
			while(SFileFindNextFile(hFind,&fileFindData));
			SFileFindClose(hFind);
		}
	}

	const char* MpkManip::findFirstFile(const char* szMask)
	{
		if(m_hFind)
		{
			SFileFindClose(m_hFind);
			m_hFind = 0;
		}
		if(m_hFind)return 0;
		m_hFind = SFileFindFirstFile(m_hMpk,szMask,&m_FileFindData,0);
		if(m_hFind)
		{
			return m_FileFindData.cFileName;
		}
		return 0;
	}

	const char* MpkManip::findNextFile()
	{
		if(m_hFind)
		{
			if(SFileFindNextFile(m_hFind,&m_FileFindData))
			{
				return m_FileFindData.cFileName;
			}
		}

		return 0;
	}

	void MpkManip::findClose()
	{
		if(m_hFind)
		{
			SFileFindClose(m_hFind);
			m_hFind = 0;
		}
	}

	uint MpkManip::getUseRate(const char* mpkFilename)
	{
		HANDLE hMpk = 0;
		uint nRate = 100;
		if(SFileCreateArchiveEx(mpkFilename,OPEN_EXISTING,8000,&hMpk))
		{
			DWORD dwTotal,dwFree;
			SFileGetBlockNum(hMpk,dwTotal,dwFree);
			if(dwTotal > 0)
				nRate = 100 * (dwTotal - dwFree) / dwTotal;
			SFileCloseArchive(hMpk);
		}

		return nRate;
	}

	bool MpkManip::cleanupFragment(const char* mpkFilename)
	{
		bool bOk = false;
		HANDLE hMpk = 0;
		char tempFilename[MAX_PATH];
		if(SFileCreateArchiveEx(mpkFilename,OPEN_EXISTING,8000,&hMpk))
		{
			uint nTotal = SFileGetFileInfo(hMpk,SFILE_INFO_NUM_FILES);
			uint nFiles = 0;
			char tempPath[MAX_PATH];
			getTempPath(MAX_PATH, tempPath);
			getTempFileName(tempPath,tempFilename);

			HANDLE hTargetMpk = 0;
			if(SFileCreateArchiveEx(tempFilename,CREATE_ALWAYS,8000,&hTargetMpk))
			{
				SFILE_FIND_DATA fileFindData;
				HANDLE hFind = SFileFindFirstFile(hMpk,"*",&fileFindData,0);
				if(hFind)
				{
					do
					{
						const char* pFilename = fileFindData.cFileName;
						char tempFilename[MAX_PATH];
						getTempFileName(tempPath,tempFilename);
						if(SFileExtractFile(hMpk,pFilename,tempFilename))
						{
							if(SFileAddFile(hTargetMpk,tempFilename,pFilename,MPK_FILE_REPLACEEXISTING | MPK_FILE_COMPRESS_PKWARE))
							{
								nFiles++;
							}
						}
					}
					while(SFileFindNextFile(hFind,&fileFindData));

					if(nTotal == nFiles)bOk = true;
				}
				SFileCloseArchive(hTargetMpk);
			}

			SFileCloseArchive(hMpk);
		}

		if(bOk && copyFile(tempFilename,mpkFilename,FALSE))
		{
			bOk = true;
		}

		return bOk;
	}

	const std::string& MpkManip::getListFileName()
	{
		static const std::string strListFilename = LISTFILE_NAME;
		return strListFilename;
	}

}