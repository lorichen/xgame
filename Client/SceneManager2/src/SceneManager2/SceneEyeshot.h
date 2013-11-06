#ifndef __SceneEyeshot_H__
#define __SceneEyeshot_H__

#include "SceneMgr.h"
#include "Eyeshot.h"
#include "SceneGrid.h"
#include "EntityList.h"

struct IEyeshotCallback;
class Tile;
class SceneManager;

class SceneEyeshot  : public IEyeshotCallback
{
	SceneManager*	m_pSceneManager;
	SceneGrid	m_GridMgr;
	EntityList m_EntityList;

public:
	SceneEyeshot();
	virtual ~SceneEyeshot();

	MapItemInfoList::iterator	m_it;
	EntityView*	getFirstEntity();
	EntityView*	getNextEntity();

	bool create(int nMapWidth, int nMapHeight, POINT& ptPlayerPos, 
		SceneManager* pSceneManager,bool bDynamic);

	bool viewportSizeChanged(int nWidth,int nHeight);
	bool scroll2Center(POINT& ptPlayerPos);
	void close();

	// IEyeshotCallback
	virtual void onChanged(int nMoveDirection, int nDx, int nDy);
	virtual void onFound(int nGridX, int nGridY);
	virtual void onLost(int nGridX, int nGridY);

	void GetPreReadCoveredGridRect(RECT& rcCoveredRect)
	{
		m_GridMgr.getGridRectBelowPreRead(rcCoveredRect);
	}
	
	void GetViewCoveredGridRect(RECT& rcCoveredRect)
	{
		m_GridMgr.getGridRectBelowViewport(rcCoveredRect);
	}
	void AddItemToList(EntityView* pEntity, const POINT& ptTile);
	void RemoveItemFromList(EntityView* pEntity, const POINT& ptTile);
	void MoveItemInList(EntityView* pEntity, const POINT& ptTileFrom, const POINT& ptTileTo);
	EntityList* GetObjectList()		{return &m_EntityList;}
	RECT& getPreReadGrid()				{return m_GridMgr.getPreReadGrid();}
	RECT& getViewGrid()					{return m_GridMgr.getViewGrid();}
};

#endif
