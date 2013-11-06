//==========================================================================
/**
* @file	  : OffsetArray.h
* @author : 
* created : 2008-2-16   12:43
* purpose : 一些TILE偏移点数组的定义
*/
//==========================================================================

#ifndef __OFFSETARRAY_H__
#define __OFFSETARRAY_H__

#include "Direction.h"



enum {Sector3_Size=21, FrontRect_5_6_Size=30, FrontRect_5_9_Size=46};

struct TileOffset
{
	char x,y;
};

/// 半径为3的圆形
//extern const TileOffset arrOffsetCircle3[Cricle3_Size];

/// 半径为4的圆形
//extern const TileOffset arrOffsetCircle4[Cricle4_Size];

/// 半径为3－12的圆形
extern const TileOffset* arrOffsetCircle[13];
extern const ulong arrSizeCircle[13];

/// 8方向上距离为3的扇面
extern const TileOffset arrOffsetSector3[eDirectionCount][Sector3_Size];

/// 8方向上身前5*6的矩形
extern const TileOffset arrOffsetFrontRect_5_6[eDirectionCount][FrontRect_5_6_Size];

/// 8方向上身前5*9的矩形
extern const TileOffset arrOffsetFrontRect_5_9[eDirectionCount][FrontRect_5_9_Size];

/// 8方向上的射线偏移
extern const TileOffset arrOffsetRadial[eDirectionCount];

/// 8方向上宽度为3的射线初始点偏移
extern const TileOffset arrOffsetRadialOrigin_3[eDirectionCount][3];

/// 8方向上宽度为5的射线初始点偏移
extern const TileOffset arrOffsetRadialOrigin_5[eDirectionCount][5];

#endif // __OFFSETARRAY_H__
