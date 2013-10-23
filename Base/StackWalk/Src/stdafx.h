#define _WIN32_WINDOWS 0x0500

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <dbghelp.h>
#include <tlhelp32.h>
#include "GetWinVer.h"
#include "MiniVersion.h"

#include <string>
#include <list>
using namespace std;
#include "base.h"
#pragma comment(lib, MAKE_DLL_LIB_NAME(xs_com))
#define new RKT_NEW