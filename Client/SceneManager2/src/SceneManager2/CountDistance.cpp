/*******************************************************************
** 文件名:	e:\Rocket\Server\ZoneManager\Src\CountDistance.h
** 版  权:	(C) 
** 
** 日  期:	2008/1/29  10:34
** 版  本:	1.0
** 描  述:	计算两点tile的距离
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include <math.h>
#include "DGlobalGame.h"
#include "CountDistance.h"

/** 
@param   
@param   
@return  
*/
CCountDistance::CCountDistance(void)
{
	// 等于 |x1-x2|的平方＋|y1-y2|的平方，再将这个和开方，就等于AB的距离
	for(int nW = 0; nW < MAX_DISTANCE_WIDTH; nW++)
	{
		for(int nH = 0; nH < MAX_DISTANCE_HIGH; nH++)
		{
			m_nDistance[nW][nH]= (ushort)(sqrt(pow((double)nW, 2) + pow((double)nH, 2)) + 0.5);	
		}
	}
}

/** 
@param   
@param   
@return  
*/
CCountDistance::~CCountDistance(void)
{

}

/** 计算
@param   
@param   
@return  
*/
int	CCountDistance::Distance(const POINT& ptTile1, const POINT& ptTile2)
{
	int nW = abs(ptTile1.x - ptTile2.x);
	int nH = abs(ptTile1.y - ptTile2.y);	

	if((nW >= MAX_DISTANCE_WIDTH) || ( nH >= MAX_DISTANCE_HIGH))
	{
		return INVALID_DISTANCE;
	}

	return m_nDistance[nW][nH];
}

CCountDistance	g_countDistance;