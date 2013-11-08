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

	bool create(int nMapWidth, int nMapHeight, xs::Point& ptPlayerPos, 
		SceneManager* pSceneManager,bool bDynamic);

	bool viewportSizeChanged(int nWidth,int nHeight);
	bool scroll2Center(xs::Point& ptPlayerPos);
	void close();

	// IEyeshotCallback
	virtual void onChanged(int nMoveDirection, int nDx, int nDy);
	virtual void onFound(int nGridX, int nGridY);
	virtual void onLost(int nGridX, int nGridY);

	void GetPreReadCoveredGridRect(xs::Rect& rcCoveredRect)
	{
		m_GridMgr.getGridRectBelowPreRead(rcCoveredRect);
	}
	
	void GetViewCoveredGridRect(xs::Rect& rcCoveredRect)
	{
		m_GridMgr.getGridRectBelowViewport(rcCoveredRect);
	}
	void AddItemToList(EntityView* pEntity, const xs::Point& ptTile);
	void RemoveItemFromList(EntityView* pEntity, const xs::Point& ptTile);
	void MoveItemInList(EntityView* pEntity, const xs::Point& ptTileFrom, const xs::Point& ptTileTo);
	EntityList* GetObjectList()		{return &m_EntityList;}
	xs::Rect& getPreReadGrid()				{return m_GridMgr.getPreReadGrid();}
	xs::Rect& getViewGrid()					{return m_GridMgr.getViewGrid();}
};

#endif
