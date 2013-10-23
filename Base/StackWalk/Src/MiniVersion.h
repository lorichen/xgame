// MiniVersion.h  Version 1.1
//
// Author:  Hans Dietrich
//          hdietrich2@hotmail.com
//
// This software is released into the public domain.
// You are free to use it in any way you like, except
// that you may not sell this source code.
//
// This software is provided "as is" with no expressed
// or implied warranty.  I accept no liability for any
// damage or loss of business that this software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MINIVERSION_H
#define MINIVERSION_H

#include <windows.h>
#include <tchar.h>

class CMiniVersion
{
// constructors
public:
	CMiniVersion(const char* lpszPath = NULL);
	bool Init();
	void Release();

// operations
public:

// attributes
public:
	// fixed info
	bool GetFileVersion(WORD *pwVersion);
	bool GetProductVersion(WORD* pwVersion);
	bool GetFileFlags(DWORD& rdwFlags);
	bool GetFileOS(DWORD& rdwOS);
	bool GetFileType(DWORD& rdwType);
	bool GetFileSubtype(DWORD& rdwType);	

	// string info
	bool GetCompanyName(LPTSTR lpszCompanyName, int nSize);
	bool GetFileDescription(LPTSTR lpszFileDescription, int nSize);
	bool GetProductName(LPTSTR lpszProductName, int nSize);

// implementation
protected:
	bool GetFixedInfo(VS_FIXEDFILEINFO& rFixedInfo);
	bool GetStringInfo(const char* lpszKey, LPTSTR lpszValue);

	BYTE*		m_pData;
	DWORD		m_dwHandle;
	WORD		m_wFileVersion[4];
	WORD		m_wProductVersion[4];
	DWORD		m_dwFileFlags;
	DWORD		m_dwFileOS;
	DWORD		m_dwFileType;
	DWORD		m_dwFileSubtype;

	TCHAR		m_szPath[MAX_PATH*2];
	TCHAR		m_szCompanyName[MAX_PATH*2];
	TCHAR		m_szProductName[MAX_PATH*2];
	TCHAR		m_szFileDescription[MAX_PATH*2];
};

#endif
///////////////////////////////////////////////////////////////////////////////
