//==========================================================================
/**
* @file	  : MwdInfo.h
* @author : 
* created : 2008-6-11   12:39
* purpose : MWD信息聚合文件
*/
//==========================================================================

#ifndef __MWDINFO_H__
#define __MWDINFO_H__

#pragma pack(push)
#pragma pack(1)

/* file format:
------------------
|  MwdInfoHeader |
------------------
|  MwdInfo1      |
|  MwdInfo2      |
|  ......        |
------------------
|  map table     |
------------------
*/

/// 信息文件头
struct MwdInfoHeader // 32 bytes
{
	ulong	mask;			/// 文件头标识字符(INFO)
	ulong	version;		/// 版本号
	ulong	tableOff;		/// 映射表偏移（相对文件开始处）
	ulong	itemCount;		/// 条目数
};

/// 用于小的Tile偏移的数据结构
struct TileOff // 2 bytes
{
	char x;
	char y;
};

/// Mwd信息
struct MwdInfo // 24 bytes
{
	ulong	w;				/// 图片最大宽度(像素)
	ulong	h;				/// 图片最大高度(像素)
	POINT	anchorOffset;	/// 锚点相对于图片左上角最大矩形的偏移量(像素)
	TileOff	sortOffset1;	/// 第一排序点（相对锚点偏移）
	TileOff	sortOffset2;	/// 第二排序点（相对锚点偏移）
	TileOff	tileOffset;		/// 占位块矩形区域左上角相对锚点的偏移
	uchar	tileWidth;		/// 占位块矩形区域的宽度（x方向）
	uchar	tileHeight;		/// 占位块矩形区域的宽度（y方向）

	//uchar	tileInfo[];		/// 占位块信息
};

/// 映射表条目
struct TableItem // 8 bytes
{
	ulong	offset;			/// 在文件中的偏移（相对于文件头，不是文件开始处）
	size_t	size;			/// 数据块大小（字节数）
};

#pragma pack(pop)


#endif // __MWDINFO_H__