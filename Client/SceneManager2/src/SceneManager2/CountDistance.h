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
#pragma once

// 最大两点tile距离（宽）
#define MAX_DISTANCE_WIDTH		512

// 最大两点tile距离（高）
#define MAX_DISTANCE_HIGH		512

// 定义
class CCountDistance
{
public:
	/** 计算
	@param   
	@param   
	@return  
	*/
	int			Distance(const xs::Point& ptTile1, const xs::Point& ptTile2);

	/** 
	@param   
	@param   
	@return  
	*/
	CCountDistance(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CCountDistance(void);

private:
	// 已计算好的距离
	ushort			m_nDistance[MAX_DISTANCE_WIDTH][MAX_DISTANCE_HIGH];
};