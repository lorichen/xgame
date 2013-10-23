#ifndef __StackWalk_H__
#define __StackWalk_H__

#include "IStackWalk.h"

enum BasicType
{
    btNoType = 0,
    btVoid = 1,
    btChar = 2,
    btWChar = 3,
    btInt = 6,
    btUInt = 7,
    btFloat = 8,
    btBCD = 9,
    btBool = 10,
    btLong = 13,
    btULong = 14,
    btCurrency = 25,
    btDate = 26,
    btVariant = 27,
    btComplex = 28,
    btBit = 29,
    btBSTR = 30,
    btHresult = 31
};

class StackWalk : public IStackWalk
{
public:
	virtual bool attachStackWalkCallback(IStackWalkCallback *);
	virtual bool detachStackWalkCallback(IStackWalkCallback *);
	virtual void setLogFileName(const char* szLogFileName);
	virtual void setDumpFileName(const char* szDumpFileName);
	virtual void setDumpType(long lDumpType);
	virtual void setDumpOn(int iEnum,bool bOn);
	virtual unsigned long getExceptionCode(PEXCEPTION_POINTERS pExceptionInfo);
	virtual const char* getExceptionDesc(PEXCEPTION_POINTERS pExceptionInfo);
	virtual unsigned long	getCrashFileOffset(PEXCEPTION_POINTERS pExceptionInfo);
	virtual const char*	getCrashModuleName(PEXCEPTION_POINTERS pExceptionInfo);
	virtual void dumpLn(const char* szMsg);
	void getCrashModuleProductVer(PEXCEPTION_POINTERS pExceptionInfo,unsigned short pwVer[4]);
	std::string getCallStack(CONTEXT& Context,HANDLE hThread);

protected:
    StackWalk();

public:
	static StackWalk *Instance()
	{
		static StackWalk cd;
		return &cd;
	}
    ~StackWalk();

protected:
	void DumpModuleList(PEXCEPTION_POINTERS pExceptionInfo);
	void DumpWindowList(PEXCEPTION_POINTERS pExceptionInfo);
	void DumpProcessList(PEXCEPTION_POINTERS pExceptionInfo);
	void DumpRegister(PEXCEPTION_POINTERS pExceptionInfo);
	void DumpCallStack(PCONTEXT,HANDLE);
	void DumpBasicInfo(PEXCEPTION_POINTERS pExceptionInfo);
	void DumpMiniDump(PEXCEPTION_POINTERS pExceptionInfo);
	void DumpLog(PEXCEPTION_POINTERS pExceptionInfo);

private:
	void DumpMiniDump(PEXCEPTION_POINTERS pExceptionInfo,HANDLE hFile);
	const char* GetExceptionDescription(DWORD ExceptionCode);
	LPTSTR GetFilePart(const char*);
	LPTSTR lstrrchr(const char*,int ch);
	void DumpModuleInfo(HINSTANCE ModuleHandle);
	void FormatTime(LPTSTR output, FILETIME TimeToPrint);
	void WriteStackDetails(PCONTEXT pContext,HANDLE);
	static bool CALLBACK EnumerateSymbolsCallback(PSYMBOL_INFO  pSymInfo,ULONG SymbolSize,PVOID UserContext);
	bool GetLogicalAddress(PVOID addr, PTSTR szModule, DWORD len, DWORD& section, DWORD& offset );
	static bool FormatSymbolValue(PSYMBOL_INFO pSym,STACKFRAME * sf,char * pszBuffer,unsigned cbBuffer);
	static char * StackWalk::DumpTypeIndex(char * pszCurrBuffer,DWORD64 modBase,DWORD dwTypeIndex,unsigned nestingLevel,DWORD_PTR offset,bool & bHandled);
	static BasicType GetBasicType( DWORD typeIndex, DWORD64 modBase );
	static char * StackWalk::FormatOutputValue(   char * pszCurrBuffer,BasicType basicType,DWORD64 length,PVOID pAddress);

	static void OnDumpStart();
	static LONG OnDumpFinish(PEXCEPTION_POINTERS pExceptionInfo);
	static FILE *m_fpLog;

private:
	bool	m_pbOn[dumpCount];
	long	m_lStackWalkType;

private:
    static LONG WINAPI UnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);
	bool IsWindows98();

private:
    TCHAR m_szLogFileName[MAX_PATH];
	TCHAR m_szDumpFileName[MAX_PATH];
    LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;
    static string m_szRpt;

private:
	static void Dump(const char* szMsg,...);

public:
	static IStackWalkCallback *	m_pSink;
};

#endif