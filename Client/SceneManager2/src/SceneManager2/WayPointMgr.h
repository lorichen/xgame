#ifndef __WayPointMgr_H__
#define __WayPointMgr_H__

#include "WayPoint.h"
#include "WayPointFinding.h"
#include <map>
#include <list>
#include <set>
#include <algorithm>
#include "ISchemeEngine.h"
#include "PathFinder.h"

struct ISceneManager2;
class SceneBlock;

class WayPointMgr : public ISchemeUpdateSink
{
public:
	// ISchemeUpdateSink
	virtual bool			OnSchemeLoad(ICSVReader * pCSVReader,const char* szFileName);
	virtual bool			OnSchemeLoad(TiXmlDocument * pTiXmlDocument,const char* szFileName);
	virtual bool			OnSchemeUpdate(ICSVReader * pCSVReader, const char* szFileName);
	virtual bool			OnSchemeUpdate(TiXmlDocument * pTiXmlDocument, const char* szFileName);
private:
	ISceneManager2*		m_pSceneManager;
	SceneBlock*	m_pMapBlockBuffer;
private:
	std::map<size_t, WayPoint> m_WayPointMap;
	typedef std::map<size_t, WayPoint>::iterator WAYPOINT_ITERATOR;

	BOOL findNearestWayPoint(int nx, int ny, int dx,int dy,POINT &pt, BOOL TestAccessible);
	
	void findWay(std::multiset<WayPointFinding> &OpenSet
		, std::list<WayPointFinding> &CloseSet, WayPointFinding CurrentNode
		, WayPointFinding SubNode);

	BOOL getSkeletonPath(const POINT& ptStart, const POINT& ptGoal
		,std::list<WayPointFinding> &CloseSet, std::list<POINT> &lsPath);

	BOOL beginFind(int Src_X,int Src_Y,std::list<POINT> &lsPath);

	BOOL findPathInSceneBlock(int Src_X, int Src_Y, int Dst_X, int Dst_Y, std::list<POINT> *plsPath);

	bool isAccessible(int sx,int sy,int dx,int dy);

//////////////////////////////////////////////////////////////////////////

public:
	WayPointMgr();
	virtual ~WayPointMgr();

	void addWayPoint(int id, int x, int y, int isGate);

	BOOL addEdge(int x1, int y1, int x2, int y2, int power);

	BOOL findPath(int Src_X, int Src_Y, int Dst_X, int Dst_Y, std::list<POINT> &lsPath);

	bool loadConfig(const char* szConfigFileName,SceneBlock *pMapBlockBuffer,ISceneManager2* pSceneManager);

	void close();
private:
	PathFinder<SceneBlock> m_pathFinder;
};


#endif
