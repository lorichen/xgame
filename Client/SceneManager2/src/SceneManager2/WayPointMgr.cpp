#include "stdafx.h"
#include "WayPointMgr.h"
#include "SceneBlock.h"
#include "ISceneManager2.h"
#include "ISchemeEngine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int nPARENTID;
bool ID_EQUIP(const WayPointFinding& FindingStruct) 
{
	return nPARENTID == WayPoint::idFromPos(FindingStruct.x, FindingStruct.y);
}

WayPointMgr::WayPointMgr() : m_pMapBlockBuffer(0),m_pSceneManager(0)
{

}

WayPointMgr::~WayPointMgr()
{

}

void WayPointMgr::close()
{
	m_WayPointMap.clear();
}

bool WayPointMgr::OnSchemeLoad(ICSVReader * pCSVReader,LPCSTR szFileName)
{
	return false;
}

bool WayPointMgr::OnSchemeLoad(TiXmlDocument * pTiXmlDocument,LPCSTR szFileName)
{
	const TiXmlElement* root = pTiXmlDocument->RootElement();
	if(!root)return true;
	const TiXmlElement *pWp = root->FirstChildElement();
	if(!pWp)return true;
	const TiXmlElement *pEd = pWp->NextSiblingElement();
	if(!pEd)return true;

	std::vector<POINT> vPt;
	for(const TiXmlElement* childchild = pWp->FirstChildElement();childchild;childchild = childchild->NextSiblingElement())
	{
		int index = 1;
		int x;
		childchild->Attribute("x",&x);
		int y;
		childchild->Attribute("y",&y);
		int gateway;
		childchild->Attribute("d",&gateway);
		addWayPoint(index++,x,y,gateway);

		POINT pt = {x,y};
		vPt.push_back(pt);
	}

	for(const TiXmlElement* childchild = pEd->FirstChildElement();childchild;childchild = childchild->NextSiblingElement())
	{
		int x,y;
		childchild->Attribute("x",&x);
		childchild->Attribute("y",&y);
		int length;
		childchild->Attribute("l",&length);
		addEdge(vPt[x].x,vPt[x].y,vPt[y].x,vPt[y].y,length);
	}

	return true;
}

bool WayPointMgr::OnSchemeUpdate(ICSVReader * pCSVReader, LPCSTR szFileName)
{
	return false;
}

bool WayPointMgr::OnSchemeUpdate(TiXmlDocument * pTiXmlDocument, LPCSTR szFileName)
{
	return true;
}

bool WayPointMgr::loadConfig(const char* szConfigFileName,SceneBlock *pMapBlockBuffer,ISceneManager2* pSceneManager)
{
	m_pSceneManager = pSceneManager;
	m_pMapBlockBuffer = pMapBlockBuffer;

	ISchemeEngine *pSchemeEngine = CreateSchemeEngineProc();
	if(pSchemeEngine)
	{
		pSchemeEngine->LoadScheme(szConfigFileName,this);
		//pSchemeEngine->Release();
	}

	return m_pathFinder.SetMapInfo(pMapBlockBuffer->m_MapWidth, pMapBlockBuffer->m_MapHeight,pMapBlockBuffer);
}

void WayPointMgr::addWayPoint(int id, int x, int y, int isGate)
{
	WayPoint waypoint;
	waypoint.setPos(x, y);
	waypoint.setPointID(id);
	waypoint.setGate(isGate);
	m_WayPointMap[waypoint.getID()] = waypoint;
}

BOOL WayPointMgr::addEdge(int x1, int y1, int x2, int y2, int power)
{
	WAYPOINT_ITERATOR iter1 = m_WayPointMap.find(WayPoint::idFromPos(x1, y1));
	WAYPOINT_ITERATOR iter2 = m_WayPointMap.find(WayPoint::idFromPos(x2, y2));

	if (iter1 == m_WayPointMap.end()
		|| iter2 == m_WayPointMap.end()
		|| iter1 == iter2)
	{
		return FALSE;
	}

	WayPoint & WayPoint1 = (*iter1).second;
	WayPoint & WayPoint2 = (*iter2).second;

	WayPoint1.addEdge(WayPoint2.getID(), power);
	WayPoint2.addEdge(WayPoint1.getID(), power);
	return TRUE;
}

bool WayPointMgr::isAccessible(int sx,int sy,int dx,int dy)
{
	if (sx == dx && sy == dy)
	{
		return true;
	}

	POINT ptFrom = {sx,sy};
	POINT ptTo = {dx,dy}; 
	POINT * pPath = NULL;
	int nPathLen = 0;

	BOOL bResult = m_pathFinder.FindPath(ptFrom, ptTo, pPath, nPathLen);
	if (nPathLen == 0 
		|| nPathLen > 128
		|| pPath[nPathLen - 1].x != ptTo.x 
		|| pPath[nPathLen - 1].y != ptTo.y )
	{
		return false;
	}

	return true;
}

BOOL WayPointMgr::findNearestWayPoint(int nx, int ny, int dx,int dy,POINT &pt, BOOL TestAccessible)
{
	BOOL bResult = FALSE;
	int nNearestDistance = 99999999;
	float dotProduct = -1.0f;

	for (WAYPOINT_ITERATOR itor = m_WayPointMap.begin(); itor != m_WayPointMap.end(); ++itor)
	{
		WayPoint &WayPoint = (*itor).second;
		int nDistance = WayPoint.distance(nx, ny);
		POINT ptPos = WayPoint.getPos();
		Vector2 v1 = Vector2(dx - nx,dy - ny);
		Vector2 v2 = Vector2(ptPos.x - nx,ptPos.y - ny);
		v1.normalize();
		v2.normalize();
		float dp = v1.dotProduct(v2);
		if (/*dp > dotProduct && */nDistance < nNearestDistance)
		{
			if (TestAccessible 
				&& !isAccessible(nx, ny, WayPoint.getPos().x, WayPoint.getPos().y))
			{
				continue;
			}
			dotProduct = dp;
			nNearestDistance = nDistance;
			pt = WayPoint.getPos();
			bResult = TRUE;
		}
	} 
	return bResult;
}

BOOL WayPointMgr::findPath(int Src_X, int Src_Y, int Dst_X, int Dst_Y, std::list<POINT> &lsPath)
{
	if (!m_pMapBlockBuffer->IsOK())
	{
		return FALSE;
	}

	POINT ptFrom;
	if (!findNearestWayPoint(Src_X, Src_Y,Dst_X,Dst_Y, ptFrom, TRUE))
	{
		return FALSE;
	}

	POINT ptTo;
	if (!findNearestWayPoint(Dst_X, Dst_Y,Src_X,Src_Y, ptTo, TRUE/*FALSE*/))
	{
		return FALSE;
	}
	
	//start to find
	std::multiset<WayPointFinding> OpenSet;
	std::list<WayPointFinding> CloseSet;

	WayPointFinding start;
	start.Setpos(ptFrom.x, ptFrom.y);
	start.parent = -1;
	OpenSet.insert(start);

	while(!OpenSet.empty())
	{
		WayPointFinding BestNode = *(OpenSet.begin());
		OpenSet.erase(OpenSet.begin());	
		if (BestNode.x == ptTo.x && BestNode.y == ptTo.y)
		{			
			POINT ptStart = {Src_X, Src_Y};
			POINT ptGoal = {Dst_X, Dst_Y};
			CloseSet.push_back(BestNode);
			getSkeletonPath(ptStart, ptGoal, CloseSet, lsPath);
			
			return beginFind(Src_X,Src_Y,lsPath);
		}

		int nWayPointID = WayPoint::idFromPos(BestNode.x, BestNode.y);
		if (m_WayPointMap.find(nWayPointID) == m_WayPointMap.end())
		{
			return FALSE;
		}

		const WayPoint &CurrentWayPoint = m_WayPointMap[nWayPointID];
		WayPoint::EdgeList edgeset = CurrentWayPoint.getAdjoingList();
	
		for (WayPoint::EdgeList::iterator itor = edgeset.begin();
				itor != edgeset.end(); ++itor)
		{
			WayPointFinding SubNode;
			SubNode.Setpos(m_WayPointMap[(*itor).first].getPos());
			SubNode.g = BestNode.g + (*itor).second;
			SubNode.f = SubNode.g;
			SubNode.parent = nWayPointID;
			findWay(OpenSet, CloseSet, BestNode, SubNode);
		}
	
		CloseSet.push_back(BestNode);
	}

	return FALSE;
}


BOOL WayPointMgr::findPathInSceneBlock(int Src_X, int Src_Y, int Dst_X, int Dst_Y, std::list<POINT> *plsPath)
{
	POINT ptFrom = {Src_X,Src_Y};	
	POINT ptTo = {Dst_X,Dst_Y};
	POINT ptFrom1 = {Src_X,Src_Y};	
	POINT ptTo1 = {Dst_X,Dst_Y};
	POINT *pBuffer;
	int nPathLen;
	if(m_pathFinder.FindPath(ptFrom1,ptTo1,pBuffer,nPathLen))
	{
		for(int i = 0;i < nPathLen;i++)
		{
			POINT pt = {pBuffer[i].x,pBuffer[i].y};
			plsPath->push_back(pt);
		}
		return TRUE;
	}

	return FALSE;
}


void WayPointMgr::findWay(std::multiset<WayPointFinding> &OpenSet
		, std::list<WayPointFinding> &CloseSet, WayPointFinding CurrentNode
		, WayPointFinding SubNode)
{
	std::multiset<WayPointFinding>::iterator OperItor = std::find(OpenSet.begin(), OpenSet.end(), SubNode);
	std::list<WayPointFinding>::iterator CloseItor =  std::find(CloseSet.begin(), CloseSet.end(), SubNode);
	
	if (OperItor == OpenSet.end() && CloseItor == CloseSet.end())
	{
		OpenSet.insert(SubNode);		
		return;
	}
	else if (OperItor != OpenSet.end())
	{
		if (SubNode.g < (*OperItor).g)
		{
			WayPointFinding temp = (*OperItor);
			temp.g = SubNode.g;
			temp.f = temp.g;
			temp.parent = SubNode.parent;
			OpenSet.erase(OperItor);
			OpenSet.insert(temp);
			return;
		}
	}
	else
	{
		if (SubNode.g < (*CloseItor).g)
		{
			WayPointFinding temp = (*CloseItor);
			temp.g = SubNode.g;
			temp.f = temp.g;
			temp.parent = SubNode.parent;
			CloseSet.erase(CloseItor);
			OpenSet.insert(temp);
			return;			
		}
	}

}

BOOL WayPointMgr::getSkeletonPath(const POINT& ptStart, const POINT& ptGoal
				  , std::list<WayPointFinding> &CloseSet, std::list<POINT> &lsPath)
{	
	if (!CloseSet.empty())
	{		
		WayPointFinding& FindingStruct = (*CloseSet.rbegin());
		lsPath.push_front(FindingStruct.getPos());

		while(FindingStruct.parent != -1)
		{
			for (std::list<WayPointFinding>::iterator itor = CloseSet.begin();
			itor != CloseSet.end(); ++itor)
			{
				WayPointFinding& Next = (*itor);
				if (WayPoint::idFromPos(Next.x, Next.y) == FindingStruct.parent)
				{
					FindingStruct = Next;
					break;
				}				
			}
			lsPath.push_front(FindingStruct.getPos());
		}		
	}

	return TRUE;

	std::list<POINT> lsTemp;
	lsTemp.swap(lsPath);
	for (std::list<POINT>::iterator itor = lsTemp.begin(); itor != lsTemp.end(); ++itor)
	{
		POINT &pt = *itor;
		int nWayPointID = WayPoint::idFromPos(pt.x, pt.y);
		WAYPOINT_ITERATOR pWayPoint = m_WayPointMap.find(nWayPointID);
		if (pWayPoint == m_WayPointMap.end())
		{
			continue;  
		}

		if ((*pWayPoint).second.isGate())
		{
			std::list<POINT>::iterator  nextitor = itor;
			++nextitor;
			if (nextitor != lsTemp.end())
			{
				POINT &ptNext = *itor;
				int nNextWayPointID = WayPoint::idFromPos(ptNext.x, ptNext.y);
				WAYPOINT_ITERATOR pNextWayPoint = m_WayPointMap.find(nNextWayPointID);
				if (pNextWayPoint != m_WayPointMap.end()
					&& (*pNextWayPoint).second.isGate())
				{
					lsPath.push_back(pt);
				}				
			}			
		}
	}

	lsPath.push_back(ptGoal);
	
	return TRUE;
}

BOOL WayPointMgr::beginFind(int Src_X,int Src_Y,std::list<POINT> &lsPath)
{
	if (lsPath.empty())
	{
		return FALSE;
	}

	POINT ptFrom = {Src_X,Src_Y};
	POINT ptTo;
	
	std::list<POINT> lsSegmentPath, lsEntirePath;

	for (std::list<POINT>::iterator itor = lsPath.begin(); itor != lsPath.end(); ++itor)
	{
		lsSegmentPath.clear();
		ptTo = *itor;
		if(ptFrom.x != ptTo.x || ptFrom.y != ptTo.y)
		{
			if (!findPathInSceneBlock(ptFrom.x, ptFrom.y, ptTo.x, ptTo.y, &lsSegmentPath))
			{
				return FALSE;
			}
		}
	
		//lsEntirePath.push_back(ptTo);
		lsEntirePath.insert(lsEntirePath.end(), lsSegmentPath.begin(), lsSegmentPath.end());
		ptFrom = ptTo;
	}
	

	lsPath.swap(lsEntirePath);

	return TRUE;
};
