#pragma once
#include "SceneMgr.h"
#include <set>
#include <map> 
#include <iostream> 


using namespace std;
class CPathFindAStar
{
public:
	/* 路径点
	*/
	struct stPathPoint
	{
		POINT  pt;
		bool   bPass;
		bool   bIsObs;
		long   value;    // 估价值
		stPathPoint*  pParent;
	};

	/* 估价值比较
	*/
	struct stInPP
	{
		stPathPoint*	ppp;
		stInPP(stPathPoint* p) : ppp(p) {}
		bool operator < (const stInPP& pp) const
		{
			return ppp->value < pp.ppp->value;
		}
	};

	/* 
	*/ 
	typedef std::multiset<stInPP> aSearch;

	/* 
	*/
	typedef aSearch::iterator itSearch;

	/* 
	*/
	typedef std::vector<POINT> aPoint;

	/* 
	*/
	typedef aPoint::iterator itPoint;
public:

	/* 寻路地图大小
	*/
	bool SetMapInfo(SceneMgr& nMap);

	/** 初始化路径值
	*/
	void New();

	/** 得到方向的值
	*/
	void GetPosDirPos(long& rx, long& ry, int x, int y, int dir);

	/** 得到该方向上的路径
	*/
	stPathPoint* GetPathPosDirPathPos(int x, int y, int dir, long& rx, long& ry);

	/** 寻路
	*/
	bool  FindPath(const POINT& ptFrom, const POINT& ptTo ,bool bChangeEnd,SceneMgr & mSceneMgr,bool isNoBlcok = false);

	/** 计算方向
	*/
	int  ComputDir(const POINT& pForm, const POINT& pTo);

	/** 需要优化取得正续坐标
	*/
	bool QueryResult();
	/* 
	*/
	CPathFindAStar(void);

	/* 
	*/
	~CPathFindAStar(void);

	/* 返回结果路径值
	*/
	static aPoint		s_aResult;

	static aPoint		s_aResultAStar;

protected:
	/* 
	*/
	aSearch             m_aSearch;

	//SceneMgr*           m_pSceneMgr;

	/*地图信息
	*/
	Size		        m_szMap;
	/* 
	*/
	ulong				m_ulMapCount;

	/* 路径
	*/
	stPathPoint*		m_pPathPoint;

	/* 返回结果路径值
	*/
	//static aPoint		s_aResult;

	/* 标识结果
	*/
	int					m_iResultCount;

	/* 
	*/
	POINT               m_aPoint[8];
};
