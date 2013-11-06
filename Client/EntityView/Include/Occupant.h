//==========================================================================
/**
* @file	  : Occupant.h
* @author : 
* created : 2008-1-21   10:32
* purpose : 占位块相关
*/
//==========================================================================

#ifndef __OCCUPANT_H__
#define __OCCUPANT_H__

#include "VariableLengthArray.h"


/**物件站位块列表的头信息
*/
struct COccupantTileListHeader
{
	/// 锚点X
	short nOffAnchorX;

	/// 锚点Y
	short nOffAnchorY;

	/// 占位块列表的宽度
	ushort wWidth;

	/// 占位块列表的高度
	ushort wHeight;
};


typedef VariableLengthArray<COccupantTileListHeader, uchar>			OccupantTileList;


/**占位块类
*/
class Occupant
{
protected:
	OccupantTileList*			m_pOccupantTileList;

public:
	Occupant(void) : m_pOccupantTileList(0)
	{ }

	virtual ~Occupant(void)
	{
		setOccupantTileList(0);
	}


	/**获得占位块列表
	@return 占位块列表
	*/
	virtual OccupantTileList* getOccupantTileList(void) const 
	{ return m_pOccupantTileList;  }

	/**设置占位块列表
	@param pList 占位块列表指针
	@return 是否成功
	*/
	virtual bool setOccupantTileList(const OccupantTileList* pList)
	{
		if (pList && !pList->isValid())
			return false;

		safeRelease(m_pOccupantTileList);

		if (pList == 0)
			return true;

		if (0 == (m_pOccupantTileList = OccupantTileList::create(pList->getArraySize())))
			return false;

		int nSize = pList->getMemSize();
		memcpy(m_pOccupantTileList, pList, nSize);

		return true;
	}
};


#endif // __OCCUPANT_H__