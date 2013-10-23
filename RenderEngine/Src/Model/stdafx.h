#ifndef __Pch_H__
#define __Pch_H__

#if defined(__WIN32__) || defined(_WIN32)
#include "windows.h"
#endif
#include <vector>
#include <list>
#include <algorithm>
#include "Base.h"
#define new RKT_NEW
#include "ReMath.h"

#ifdef SUPPORT_COLLISION_DETECTION
#pragma warning( disable : 4312 )
#include "Opcode.h"
using namespace Opcode;
#endif

#include "Re.h"
#include "IPerfProf.h"

#endif