#ifndef __I_StackWalk_H__
#define __I_StackWalk_H__

#include <string>

enum
{
	///是否输出Log
	dumpLog,

	///基本信息
	dumpBasicInfo,

	///模块列表
	dumpModuleList,

	//窗口列表
	dumpWindowList,

	///进程列表
	dumpProcessList,

	///寄存器
	dumpRegister,

	///调用栈
	dumpCallStack,

	///输出dmp文件
	dumpMiniDump,

	dumpCount
};

struct IStackWalk;

struct IStackWalkCallback
{
	///开始dump信息会调用这个函数-
	virtual void onDumpStart(IStackWalk*) = 0;

	///可以用IStackWalk的dumpLn Dump想输出到LogFile的信息
	virtual void onDump(IStackWalk*,PEXCEPTION_POINTERS) = 0;

	///Dump结束会调用此函数，此函数返回值会被当作异常捕获的返回值
	virtual long onDumpFinish(IStackWalk*,PEXCEPTION_POINTERS) = 0;
};

struct IStackWalk
{
	///挂一个IStackWalkCallback的回调
	virtual bool attachStackWalkCallback(IStackWalkCallback *) = 0;

	///删除回调
	virtual bool detachStackWalkCallback(IStackWalkCallback *) = 0;

	///设置Log文件名
	virtual void setLogFileName(const char* szLogFileName) = 0;

	///设置Dmp文件名
	virtual void setDumpFileName(const char* szDumpFileName) = 0;

	///对上面的枚举的开关
	virtual void setDumpOn(int iEnum,bool bOn) = 0;

	///设置Dump类型
	virtual void setDumpType(long lDumpType) = 0;

	///得到异常码
	virtual unsigned long getExceptionCode(PEXCEPTION_POINTERS pExceptionInfo) = 0;

	///得到异常描述
	virtual const char* getExceptionDesc(PEXCEPTION_POINTERS pExceptionInfo) = 0;

	///得到崩溃的模块文件偏移
	virtual unsigned long getCrashFileOffset(PEXCEPTION_POINTERS pExceptionInfo) = 0;

	///得到崩溃的模块文件名
	virtual const char*	getCrashModuleName(PEXCEPTION_POINTERS pExceptionInfo) = 0;

	///输出一些Dump信息到Log文件
	virtual void dumpLn(const char* szMsg) = 0;

	///取得崩溃模块的产品版本号
	virtual void getCrashModuleProductVer(PEXCEPTION_POINTERS pExceptionInfo,unsigned short pwVer[4]) = 0;

	///取得当前调用栈
	virtual std::string getCallStack(CONTEXT& Context,HANDLE hThread) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(CRASHDUMP_STATIC_LIB)	/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(xs_swm))
extern "C" IStackWalk * LoadSWM();
#	define	createStackWalkProc	LoadSWM
#else													/// 动态库版本
typedef IStackWalk * (RKT_CDECL *proccreateStackWalk)(void);
#	define	createStackWalkProc	DllApi<proccreateStackWalk>::load(MAKE_DLL_NAME(xs_swm), "LoadSWM")
#endif


#endif