#ifndef __Stdafx_H__
#define __Stdafx_H__

#define _Info(x)

#include "Base.h"
#define new RKT_NEW

#include "Re.h"
#include "IResourceManager.h"
#include "IPerfProf.h"
using namespace xs;

#include "IEntityFactory.h"
//#include "IFormManager.h"
//#include "IAudioEngine.h"
//#include "DEntityProp.h"

#define Ceil(PixelPos, GridLength)	((PixelPos + GridLength - 1) / GridLength)
#define Floor(PixelPos, GridLength)	(PixelPos / GridLength)


#endif