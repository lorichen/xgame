#ifdef MATH_EXPORTS
	#define _ReMathExport __declspec(dllexport)
#else
	#define _ReMathExport __declspec(dllimport)
#endif

#pragma warning (disable : 4267)
#pragma warning (disable : 4244)
#pragma warning (disable : 4305)

#include <vector>
#include <list>
//#include <hash_map>
#include <map>
#include <limits>
#include <iostream>
#include "assert.h"