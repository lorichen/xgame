//==========================================================================
/**
* @file	  : Base.cpp
* @author : 
* created : 2008-5-24   9:28
*/
//==========================================================================

#include "stdafx.h"


namespace xs {
extern void destroyHandleTable();
} // namespace


#ifdef RKT_WIN32


BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//if (!FCL::innerFCL_Init())
		//	return FALSE;
		{
			int aaa=0;
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		//FCL::innerFCL_Destroy();
		{
			xs::destroyHandleTable();
			int aaa=0;
		}
		break;
	}
	return TRUE;
}

#else // !RKT_WIN32

#endif // !RKT_WIN32

