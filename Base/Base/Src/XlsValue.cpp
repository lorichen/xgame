//==========================================================================
/**
* @file	  : XlsValue.cpp
* @author : 
* created : 2008-3-13   15:54
*/
//==========================================================================

#include "stdafx.h"
#include "XlsValue.h"

#define new RKT_NEW

namespace xs {

	RKT_EXPORT autostring XlsValue::sEmptyAutoString;
	RKT_EXPORT IntArray XlsValue::sEmptyIntArray = {0, 0};

	RKT_EXPORT XlsValue	XlsRecord::sEmptyValue;

} // namespace