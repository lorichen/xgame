//==========================================================================
/**
* @file	  : EffectDef.h
* @author : 
* created : 2008-3-13   17:35
* purpose : 光效属性定义
*/
//==========================================================================

#ifndef __EFFECTDEF_H__
#define __EFFECTDEF_H__

enum
{ 
	MAX_SUBEFFECT_COUNT	= 1000, 
	MAX_EFFECT_COUNT	= 1000,
};

// EffectView.csv
enum EEffectProperty
{
	EffectProperty_Depth,
	EffectProperty_Name,
	EffectProperty_Id,
	EffectProperty_MagicId,
	EffectProperty_Loops,
	EffectProperty_TailSilence,
	EffectProperty_Color,
	EffectProperty_Scale,
	EffectProperty_NeedStunAction,
	EffectProperty_NeedShadow,
	EffectProperty_ChangeSharp,

	MaxEffectPropertyCount
};

#endif // __EFFECTDEF_H__