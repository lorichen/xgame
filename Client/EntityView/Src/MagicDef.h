//==========================================================================
/**
* @file	  : MagicDef.h
* @author : 
* created : 2008-3-13   17:35
* purpose : 光效属性定义
*/
//==========================================================================

#ifndef __MAGICDEF_H__
#define __MAGICDEF_H__

enum
{ 
	MAX_SUBMAGIC_COUNT	= 1000, 
	MAX_MAGIC_COUNT		= 1000,
};

enum EMagicProperty
{
	MagicProperty_Depth,
	MagicProperty_Name,
	MagicProperty_Id,
	MagicProperty_ResId,
	MagicProperty_ResType,
	MagicProperty_AniType,
	MagicProperty_AniSpeed,
	MagicProperty_AniScale,
	MagicProperty_AniDir,
	MagicProperty_AniColor,
	MagicProperty_AniTrans,
	MagicProperty_AniOffset,

	MaxMagicPropertyCount
};

#endif // __MAGICDEF_H__