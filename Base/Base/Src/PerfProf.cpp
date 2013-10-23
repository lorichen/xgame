#include "stdafx.h"
#include "Common.h"
#include "IPerfProf.h"
#include "profile.h"

#define new RKT_NEW

namespace xs
{
#ifndef _NO_PERF_PROF
	ppSample::ppSample( const char * name )
	{
		CProfiler::Instance()->Start(name);
	}

	ppSample::ppSample(const char * name,int id)
	{
		char szMsg[256];
		sprintf(szMsg,"%s%d",name,id);
		CProfiler::Instance()->Start(szMsg);
	}
		
	ppSample::~ppSample( void )
	{
		CProfiler::Instance()->Stop();
	}

	RKT_API IProfile*	getProfile()
	{
		return CProfiler::Instance();
	}
#else
	RKT_API IProfile*	getProfile()
	{
		return 0;
	}
#endif
}