//==========================================================================
/**
* @file	  : MapTable.cpp
* @author : 
* created : 2008-3-19   19:17
*/
//==========================================================================

#include "stdafx.h"
#include "MapTable.h"
#include "StateDef.h"
#include "ISchemeCenter.h"

/*
人物模型分为3个子mesh，即头发或者头盔对应的子mesh, 脸对应的子mesh和身体体（除脸外）对应的子mesh，头发或者头盔对应的子mesh命名为：”toukui”脸对应的子mesh命名叫做：”lian”，身体对应的子mesh命名叫做：”shenti”
4) 长发由一个子mesh构成，命名为：”toukui”
*/

static char * sModelMeshNames[MSFCP_MAX+1] =
{
	"shenti",
	"lian",
	"toufa",
	"",
};

const char * getModelMeshName(uint index)
{
	if( index >= MSFCP_MAX) return sModelMeshNames[MSFCP_MAX];
	else return sModelMeshNames[index];
}

//移动到schemecenter
/*
const char * getBindPoint(ulong bp)
{
	static const char * map[EBindPoint_Max] = 
	{
		"",
		"guadian_zuoshou",
		"guadian_youshou",
		"guadian_zuobei",
		"guadian_youbei",
		"guadian_toubu",
		"guangxiao1",
		"guangxiao2",
		"guangxiao3",
		"guangxiao4",
		"guangxiao5",
		"lizi1",
		"lizi2",
		"lizi3",
		"lizi4",
	};
	if( bp >= EBindPoint_Max ) return map[EBindPoint_Min];
	return map[bp];
}
*/