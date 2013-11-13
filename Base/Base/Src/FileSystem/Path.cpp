#include "stdafx.h"
#include "path.h"
#include "Api.h"

#define new RKT_NEW

namespace xs{

#ifdef WIN32
#include <windows.h>
#include <direct.h>
#else
//#include <direct.h>
    #  include <unistd.h>
#endif

RKT_API const char* getWorkDir()
{
	static char g_workDir[MAX_PATH] = {0};
	if (g_workDir[0] != (char)0)
		return g_workDir;

#ifdef WIN32
	if (GetModuleFileName(NULL, g_workDir, MAX_PATH - 1))
	{
		CPath filename ;
		filename = g_workDir;
		lstrcpyn(g_workDir, filename.getParentDir().c_str(), MAX_PATH - 1);
	}
#else
	//getcwd(g_workDir, MAX_PATH - 1);  //current path
    //const char* p = getAppPath();
    //memcpy(g_workDir,getAppPath(),strlen(p));
    //memcpy(g_workDir,"./",4);
#endif

	return g_workDir;
}

}