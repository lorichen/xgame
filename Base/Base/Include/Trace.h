/**
* Created by pk 2008.01.02
*/

#ifndef RKT_TRACE_H
#define RKT_TRACE_H

#include "Common.h"
#include "ostrbuf.h"
#include <tchar.h>

/**
	Trace设计要点：
	1、方便，仅几个宏直接使用；
	2、基于类型安全，取消诸如 printf 的不安全格式；
	3、输出信息可重定向；
	4、线程安全；
	5、支持输出级别；
*/

namespace xs {

	/// 消息级别
	enum TraceLevel
	{
		levelInfo		= 0x1,	/// 输出详细信息
		levelTrace		= 0x2,	/// 输出Trace信息
		levelWarning	= 0x4,	/// 输出警告信息
		levelError		= 0x8,	/// 输出错误信息

		levelAll = levelInfo|levelTrace|levelWarning|levelError,
	};

	/// 消息接收器
	class RKT_EXPORT TraceListener
	{
		int		level;
		bool	has_time;
	public:
		TraceListener() : level(levelAll), has_time(false) {}

		void setTraceLevel(int lvl)		{ level = lvl; }
		int getTraceLevel() const		{ return level; }
		void setHasTime(bool hasTime)	{ has_time = hasTime; }
		bool hasTime() const			{ return has_time; }
		virtual void onTrace(const char* msg, const char* time, TraceLevel level) {}
	};

	RKT_API void createTrace(int level = levelAll, bool hasTime = true, bool threadSafe = true);
	RKT_API int setTraceLevel(int level);
	RKT_API bool setTraceHasTime(bool hasTime);
	RKT_API void registerTrace(TraceListener* sink);
	RKT_API void unregisterTrace(TraceListener* sink);
	RKT_API void closeTrace();

	RKT_API TraceListener* output2Console(int level = levelAll, bool hasTime = false);
	RKT_API TraceListener* output2File(const char* filename = NULL, int level = levelAll, bool hasTime = false);
	RKT_API TraceListener* output2RichEdit(void* hwnd, int level = levelAll, bool hasTime = false);
	RKT_API TraceListener* output2Socket(const char* host = "127.0.0.1", ushort port = 1234, bool hasTime = false);

	RKT_API void addTraceToRichEdit(void* hWndRichEdit, const char* msg, TraceLevel level);
	RKT_API void dispatch2RichEdit(TraceListener* tl);

	RKT_API void output(const char* msg, TraceLevel level = levelTrace, const char* file = 0, int line = 0);


#define Trace(x)	{ostrbuf osb;osb<<x;output(osb.c_str(), levelTrace, __FILE__, __LINE__);}
#define Warning(x)	{ostrbuf osb;osb<<"WARNING: "<<x;output(osb.c_str(), levelWarning, __FILE__, __LINE__);}
#define Error(x)	{ostrbuf osb;osb<<"ERROR: "<<x;output(osb.c_str(), levelError, __FILE__, __LINE__);}

#define TraceLn(x)	{ostrbuf osb;osb<<x<<endl;output(osb.c_str(), levelTrace, __FILE__, __LINE__);}
#define WarningLn(x){ostrbuf osb;osb<<"WARNING: "<<x<<endl;output(osb.c_str(), levelWarning, __FILE__, __LINE__);}
#define ErrorLn(x)	{ostrbuf osb;osb<<"ERROR: "<<x<<endl;output(osb.c_str(), levelError, __FILE__, __LINE__);}

#ifdef RKT_DEBUG
#	define Info(x)		{ostrbuf osb;osb<<x;output(osb.c_str(), levelInfo, __FILE__, __LINE__);}
#	define InfoLn(x)	{ostrbuf osb;osb<<x<<endl;output(osb.c_str(), levelInfo, __FILE__, __LINE__);}
#	define Assert(x)	{if(!(x)){ostrbuf osb;osb<<"ASSERT: "<<#x<<endl;output(osb.c_str(), levelError, __FILE__, __LINE__);}}
#	define Verify(x)	{if(!(x)){ostrbuf osb;osb<<"VERIFY: "<<#x<<endl;output(osb.c_str(), levelError, __FILE__, __LINE__);}}
#else
#	define Info(x)
#	define InfoLn(x)
#	define Assert(x)
#	define Verify(x)	(x)
#endif

#define TraceOnce(x)		{static bool f=true;if(f){f=!f;Trace(x);}}
#define WarningOnce(x)		{static bool f=true;if(f){f=!f;Warning(x);}}
#define ErrorOnce(x)		{static bool f=true;if(f){f=!f;Error(x);}}

}

#ifdef  ASSERT
#undef ASSERT
#endif

#ifdef ASSERT_DEBUG
#define ASSERT(x)  {if(!(x)){::MessageBox(NULL, _TEXT("断言失效!"), _TEXT("ASSERT"), MB_OK|MB_ICONERROR); __asm{int 3};}}
#ifdef Assert
#undef Assert
#define Assert ASSERT
#endif
#else
#define ASSERT Assert
#endif


#ifdef VERIFY
#undef VERIFY
#endif
#define VERIFY Verify


#endif // RKT_TRACE_H