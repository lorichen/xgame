/**
* Created by pk 2008.01.03
*/

#include "stdafx.h"
#include "LoadLib.h"

#define new RKT_NEW

#ifdef RKT_WIN32
#	include <windows.h>
#elif defined (RKT_LINUX)
#	include <dlfcn.h>
#endif

namespace xs {

#ifdef RKT_WIN32

	RKT_API void* loadlib(const char* filename)
	{
		return LoadLibraryA(filename);
	}

	RKT_API bool freelib(void* moduleHandle)
	{
		return FreeLibrary((HMODULE)moduleHandle) == TRUE;
	}

	RKT_API Proc getProcAddress(void* moduleHandle, const char* procName)
	{
		return reinterpret_cast<Proc>(GetProcAddress((HMODULE)moduleHandle, procName));
	}

#elif defined(RKT_LINUX)

	RKT_API void* loadlib(const char* filename)
	{
		return dlopen(filename, RTLD_NOW);
	}

	RKT_API bool freelib(void* moduleHandle)
	{
		return dlclose(moduleHandle);
	}

	RKT_API Proc getProcAddress(void* moduleHandle, const char* procName)
	{
		return (Proc)dlsym(moduleHandle, procName);
	}

#endif



}