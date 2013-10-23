/**
* Created by pk 2008.01.04
*/

#ifndef RKT_BASE_H
#define RKT_BASE_H

#pragma warning(disable : 4996) // strcpy等非安全警告

#include "Common.h"
#include "Mem.h"
#include "Singleton.h"
#include "ostrbuf.h"
#include "buffer.h"
#include "Trace.h"
#include "Api.h"
#include "LoadLib.h"
#include "autostring.h"
#include "StringHelper.h"
#include "StringHash.h"
#include "StlHelper.h"
#include "FileSystem.h"
#include "App.h"
#include "Object.h"
#include "LockFree.h"
#include "Thread.h"
#include "Handle.h"
#include "Timer.h"
#include "tinyxml.h"
#include "TimerAxis.h"
#include "RandGenerator.h"
#include "XlsValue.h"
#include "IPerfProf.h"
#include "MemObj.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "tolua++.h"
}

#endif // RKT_BASE_H