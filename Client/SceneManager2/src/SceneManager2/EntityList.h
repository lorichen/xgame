#ifndef __EntityList_H__
#define __EntityList_H__

#include "IEntityView.h"
#include <list>
using namespace std;

enum EntityListErrType
{
	EE_INVALID = -1,
	EE_FAILED = 0,
	EE_SUCCESS = 1
};

struct EntityInfo
{
	EntityView*	pEntity;
	xs::Point		ptTile;
	xs::Point		ptGrid;
	EntityInfo(EntityView* p, const xs::Point& tile, const xs::Point& grid)
		: pEntity(p), ptTile(tile), ptGrid(grid) {}
	bool operator == (const EntityInfo& info)
	{
		return (info.pEntity == pEntity && 
			info.ptTile.x == ptTile.x && info.ptTile.y == ptTile.y);
	}
};
typedef list<EntityInfo>				MapItemInfoList;
typedef list<EntityInfo>::iterator	MapItemInfoListPtr;

//add by yhc
//管理本场景内的实体列表
//添加,删除,实体移动等操作
//findFirstItem,findNextItem对实体的遍历效率比枚举来得高

class EntityList  
{
protected:
	MapItemInfoList		m_EntityList;
	MapItemInfoListPtr	m_pCurObj;

public:
	EntityList()
	{
		clear();
	}
	virtual ~EntityList()
	{
		clear();
	}
	bool isEmpty()
	{
		return m_EntityList.empty();
	}
	void clear()
	{
		m_EntityList.clear();
		m_pCurObj = m_EntityList.end();
	}
	int getCount()
	{
		return m_EntityList.size();
	}
	MapItemInfoList* getListPtr()
	{
		return &m_EntityList;
	}
	void add(EntityView* pEntity, const xs::Point& ptTile, const xs::Point& ptGrid)
	{
		if (pEntity == 0) return;
		m_EntityList.push_back(EntityInfo(pEntity, ptTile, ptGrid));
	}
	int remove(EntityView* pEntity, const xs::Point& ptTile)
	{
		if (pEntity == 0) return EE_INVALID;
		MapItemInfoListPtr first = m_EntityList.begin();
		MapItemInfoListPtr last = m_EntityList.end();
		while (first != last)
		{
			MapItemInfoListPtr next = first;
			++next;
			if ((*first).pEntity == pEntity)
			{
				m_EntityList.erase(first);
				return EE_SUCCESS;
			}
			first = next;
		}
		return EE_FAILED;
	}
	int move(EntityView* pEntity, const xs::Point& ptTileFrom, const xs::Point& ptTileTo, const xs::Point& ptGridTo)
	{
		if (pEntity == 0) return EE_INVALID;
		MapItemInfoListPtr first = m_EntityList.begin();
		MapItemInfoListPtr last = m_EntityList.end();
		while (first != last)
		{
			MapItemInfoListPtr next = first;
			++next;
			if ((*first).pEntity == pEntity)
			{
				(*first).ptTile = ptTileTo;
				(*first).ptGrid = ptGridTo;
				return EE_SUCCESS;
			}
			first = next;
		}
		add(pEntity, ptTileTo, ptGridTo);
		return EE_FAILED;
	}

	EntityInfo* findFirstItem(int nGridX, int nGridY)
	{
		m_pCurObj = m_EntityList.begin();
		MapItemInfoListPtr last = m_EntityList.end();
		while (m_pCurObj != last)
		{
			MapItemInfoListPtr next = m_pCurObj;
			++next;
			EntityInfo& info = (*m_pCurObj);
			if (info.ptGrid.x == nGridX && info.ptGrid.y == nGridY)
				return &(*m_pCurObj);
			m_pCurObj = next;
		}
		return 0;
	}
	EntityInfo* findNextItem(int nGridX, int nGridY)
	{
		++m_pCurObj;
		MapItemInfoListPtr last = m_EntityList.end();
		while (m_pCurObj != last)
		{
			MapItemInfoListPtr next = m_pCurObj;
			++next;
			EntityInfo& info = (*m_pCurObj);
			if (info.ptGrid.x == nGridX && info.ptGrid.y == nGridY)
				return &(*m_pCurObj);
			m_pCurObj = next;
		}
		return 0;
	}
};

#endif
