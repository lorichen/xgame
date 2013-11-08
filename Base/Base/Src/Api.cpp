/**
* Created by pk 2008.01.03
*/

#include "stdafx.h"
#include "Api.h"

#define new RKT_NEW

#if (TARGET_PLATFORM == PLATFORM_IOS)
#include <sys/time.h>
#endif



namespace xs {
#if (TARGET_PLATFORM == PLATFORM_WIN32)
	bool GetClientRect(void* hwnd,Rect* rc)
	{
		RECT rc1;
		GetClientRect((HWND)hwnd,&rc1);
		rc->left = rc1.left;
		rc->right = rc1.right;
		rc->top = rc1.top;
		rc->bottom = rc1.bottom;
		return true;
	}
#endif

	RKT_API	ulong getTickCount()
	{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		return ::GetTickCount();
#elif (TARGET_PLATFORM == PLATFORM_IOS)
        struct timeval tv;
		struct timezone tz;
		gettimeofday(&tv , &tz);
		return (ulong)((tv.tv_sec & 0xfffff) * 1000 + tv.tv_usec / 1000);
#else
		struct timeval tv;
		struct timezone tz;
		gettimeofday(&tv , &tz);
		return (ulong)((tv.tv_sec & 0xfffff) * 1000 + tv.tv_usec / 1000);
#endif
	}


	RKT_API void sleep(ulong milliSeconds)
	{
#ifdef RKT_WIN32
		::Sleep(milliSeconds);
#elif defined(RKT_LINUX)
		usleep(milliSeconds * 1000LL);
#endif
	}


	static double initPerformanceTimer()
	{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		LARGE_INTEGER litmp;
		if (!::QueryPerformanceFrequency(&litmp))
		{
			//Error("the computer not support QueryPerformanceFrequency!\n");
		}
		return (double)litmp.QuadPart;
        
#elif (TARGET_PLATFORM == PLATFORM_IOS)
        //not impl
        return 1;
#else
		char buffer[10];
		getProcValue("/proc/cpuinfo","cpu MHz", buffer);
		if (buffer[0] == '\0')
			return 0.0;

		return atof(buffer);
#endif
	}

	static double gPerformanceFrequency = 1000 / initPerformanceTimer();


	RKT_API int64 getPerformanceCount()
	{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		LARGE_INTEGER litmp;
		::QueryPerformanceCounter(&litmp);
		return (int64)litmp.QuadPart;
#elif (TARGET_PLATFORM == PLATFORM_IOS)
        //not impl
        return getTickCount();
#else
		register int64 ticks;
		__asm__ volatile (".byte 0x0f, 0x31" : "=A" (ticks));
		return ticks;
#endif
	}

	double count2Time(int64 deltaCount)
	{
		return deltaCount * gPerformanceFrequency;
	}

	RKT_API ulong getTickCountEx()
	{
		return (ulong)count2Time(getPerformanceCount());
	}



	RKT_API bool isDebuggerPresent()
	{
#ifdef RKT_WIN32
		return (IsDebuggerPresent() == TRUE);
#elif defined(RKT_LINUX)
		return false;
#endif
	}


	RKT_API void debuggerBreak(const char* exp, const char* file, uint line, int* staticIntVarEqualZero)
	{
#ifdef RKT_COMPILER_MSVC
		if (isDebuggerPresent())
		{
			if (staticIntVarEqualZero)
			{
				if (*staticIntVarEqualZero == 0)
				{
					*staticIntVarEqualZero = 1;
					//Error("DebuggerBreakOnce: %s\n%s(%d)\n", exp, file, line);
					__asm{int 3};			
				}
			}
			else
			{
				//Error("DebuggerBreak: %s\n%s(%d)\n", exp, file, line);
				__asm{int 3};
			}
		}
#else
#endif
	}


	static void* gGlobal = 0;
	RKT_API void* getGlobal()
	{ return gGlobal; }

	RKT_API void setGlobal(void* global)
	{ gGlobal = global; }

}