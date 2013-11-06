// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

// 如果必须将位于下面指定平台之前的平台作为目标，请修改下列定义。
// 有关不同平台对应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用特定于 Windows XP 或更高版本的功能。
#define WINVER 0x0501		// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用特定于 Windows 98 或更高版本的功能。
#define _WIN32_WINDOWS 0x0410 // 将此值更改为适当的值，以指定将 Windows Me 或更高版本作为目标。
#endif

#ifndef _WIN32_IE			// 允许使用特定于 IE 6.0 或更高版本的功能。
#define _WIN32_IE 0x0600	// 将此值更改为相应的值，以适用于 IE 的其他版本。
#endif

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>



// TODO: 在此处引用程序需要的其他头文件
#include "Base.h"
#pragma comment(lib, MAKE_DLL_LIB_NAME(xs_com))
#pragma comment(lib, MAKE_DLL_LIB_NAME(xs_gui))
#define new RKT_NEW

#include "Re.h"
#include "IResourceManager.h"
using namespace xs;

#if !defined (SAFE_RELEASE)
#define SAFE_RELEASE(p)				{ if((p) != NULL) { (p)->Release(); (p)=NULL; } }
#endif

#if !defined (SAFE_DELETE)
#define SAFE_DELETE(p)				{ if((p) != NULL) { delete (p);     (p)=NULL; } }
#endif

#if !defined (SAFE_DELETEARRAY)
#define SAFE_DELETEARRAY(p)			{ if((p) != NULL) { delete [](p);    (p)=NULL; } }
#endif


#include "IGlobalClient.h"
#include "ISceneManager2.h"
#include "ISchemeEngine.h"
#include "IEntityClient.h"

#include "Direction.h"
#include "MapTable.h"

#include "DGlobalGame.h"
#include "DEntityProp.h"
#include "DGlobalMisc.h"
#include "net.h"
#include "DGlobalMessage.h"
#include "HelperFunc.h"
#include "DGlobalEvent.h"


enum UpdateOption
{
	updatePosition	=	0x1,	/// 位置
	updateAngle		=	0x2,	/// 角度
	updateAnimation	=	0x4,	/// 动画
	updateAniSpeed	=	0x8,	/// 动画速度
	updateColor		=	0x10,	/// 颜色
	updateScale		=	0x20,	/// 缩放
	updateFadeIn	=	0x40,	/// 淡入
	updateFadeOut	=	0x80,	/// 淡出
	updatePart		=	0x100,	/// 部件
	updateRidePet	=	0x200,	/// 宠物骑乘
	updatePetdie    =   0x400,  /// 宠物消弭(wan)
	updateJump      =   0x800,  /// 跳跃
	//updateDelete	=	0x200,	/// 删除自己
//	updateMoveSpeed	=	0x400,	/// 移动速度
};

#define FADE_ALPHA	0.001f

