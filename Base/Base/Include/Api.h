 /**
* Created by pk 2008.01.03
*/

#ifndef RKT_API_H
#define RKT_API_H

#include "Common.h"

namespace xs {
	extern "C" RKT_API bool GetClientRect(void* hwnd,Rect* rc);
	/// 获得系统Tick数
	RKT_API ulong getTickCount();

	/// 采取高性能计时器获取的Tick数
	RKT_API ulong getTickCountEx();

	/// 线程休眠（毫秒）
	RKT_API void sleep(ulong milliSeconds);

	/// 获取系统高性能计数
	RKT_API int64 getPerformanceCount();

	/// 是否是调试器模式
	RKT_API bool isDebuggerPresent();

	/// 调试中断
	RKT_API void debuggerBreak(const char* exp, const char* file, uint line, int* staticIntVarEqualZero = 0);

	/// 获取全局变量（用于IGlobalClient和IGlobalServer，没有想到什么好办法）
	RKT_API void* getGlobal();

	/// 设置全局对象（用于IGlobalClient和IGlobalServer，没有想到什么好办法）
	RKT_API void setGlobal(void* global);

#ifdef RKT_DEBUG
#	define DebuggerBreak(exp)		(void)( (exp) || (debuggerBreak(#exp, __FILE__, __LINE__, 0), 0) )
#	define DebuggerBreakOnce(exp)	{static int __s_once__ = 0; (void)( (exp) || (debuggerBreak(#exp, __FILE__, __LINE__, &__s_once__), 0) );}
#else
#	define DebuggerBreak(exp)		((void)0)
#	define DebuggerBreakOnce(exp)	((void)0)
#endif

}

#endif // RKT_API_H