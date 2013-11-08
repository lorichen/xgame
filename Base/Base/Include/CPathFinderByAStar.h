/*******************************************************************
** 文件名:	CPathFinderByAStar.h
** 版  权:	(C)  2010 - All Rights Reserved
** 
** 日  期:	2010/07/28
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 万志辉
** 日  期: 2010/07/28
** 描  述: 添加一个修改过的A* 算法；替换掉引擎自带的寻路算法；让人物的
           移动不至于看上去不自然
********************************************************************/
#ifndef __CPATH_FINDERBYASTAR_H__
#define __CPATH_FINDERBYASTAR_H__

#include <assert.h>
#include <functional>
#include <vector>
#include <set>
#include <map> 
#include <iostream> 
using namespace std;



template
< 
	    class    _Map,                                                  // 地图 
		typename _BlockTestFunc = std::mem_fun1_t<bool,_Map,xs::Point> ,    // 阻挡判断函数对象,函数型如bool IsBlock(xs::Point pt)
		int      MAX_PATH_LEN   = 1024 ,                                // 最大路径长度
		int      MAX_SEARCH_AREA= 256                                   // 最大搜索面积
>

class CPathFinderByAStar
{
	//struct stPathNode;
public:
    
	struct stPathNode 		  //结点,也就是每个格子
	{
		xs::Point m_PointPos;
		bool bIsObs;
		int m_nGvalue;            // G值
		int m_nPointvalue;        // 估算代价值 
		stPathNode *pParent;	  // 父结点指针
	} ;	
    
	typedef std::multimap<int, int>  OpenTempList;   //open list 	第二个int是ID号 第一个是估算代价值
	typedef std::set<int>            OpenList;      // //open list 	    第一个int是ID号 第二个是估算代价值
	typedef std::set<int>            CloseList;      //close list 

	OpenTempList::iterator itTempOpen1;
	OpenTempList::iterator itTempOpen2;	
public:
	//iFindPathCallback*	m_pCallback;
	_Map*                m_pMap;                                      // 地图指针
	_BlockTestFunc       m_BlockFunc;                                 // 阻挡判断函数
	unsigned long        m_nMapWidth;                                 // 地图宽度
	unsigned long        m_nMapHeight;                                // 地图高度
	stPathNode*          stAStarPoint;
	xs::Point				 m_PathTemp[MAX_PATH_LEN];                    // 用来存储返回的路径（反序）
	xs::Point				 m_PathResult[MAX_PATH_LEN];                  // 用来存储返回的路径(正序路径)

	//cSize			m_szMap;	
	xs::Point mapNearPoint;		
public:
	CPathFinderByAStar( ) : m_pMap(0),m_BlockFunc(0),stAStarPoint(0),m_nPathCount(0)
	{
		m_mOpenTempList.clear(); //clean tempopen   list 
		m_mOpenList.clear(); 	 //clean open   list 
		m_mCloseList.clear();    //clean close  list 
	}

	~CPathFinderByAStar( )
	{  
		delete[] stAStarPoint;  
	}

	/**
	@name           : 设置地图信息 
	@brief          : 调用FindPath之前请设置地图信息,该函数可以重复调用,所以可以在多个地图上搜索路径
	@param nMapWidth: 地图宽度
	@param nMapHeigh: 地图高度
	@param pMap     : 地图对象指针
	@param isBlock  : 判断地表是否阻挡的函数,如果地图对象判断阻挡的函数是IsBlock则该参数不用传
	@return         : 
	*/
	bool SetMapInfo(unsigned long nMapWidth,unsigned long nMapHeight,_Map * pMap,_BlockTestFunc isBlock=std::mem_fun(&_Map::IsBlock));

	/**
	@name			: 搜索路径
	@brief			: 
	@param ptStart  : 起点坐标
	@param ptEnd    : 终点坐标
	@param pPathRet : 返回的路径点
	@param nPathLen : 返回路径长度
	@param nOption  : 搜路选项,是否优化路径
	@param nMaxStep : 最多搜索的步数,如果太远就放弃 (这个参数有两个意义 1.有时候只需要搜出最近几步的路,并不一定非要到达目标点 2.默认设为1024,防止出现死循环)
	@return         : 返回搜路是否成功 
	*/
	bool FindPathByASatr(const xs::Point & ptStart,const xs::Point & ptEnd,xs::Point *& pPath,int & nPathLen,int nOption=SEARCH_OPTION_OPTIMIZE,int nMaxStep=MAX_PATH_LEN);
	/**
	@name			: 是否可以走直线
	@brief			: 
	@param ptStart  : 起点坐标
	@param ptEnd    : 终点坐标
	@return         : 返回搜路是否成功 
	*/
	bool CanWalkDirect(const xs::Point& ptFrom, const xs::Point& ptTo);
	/**
	@name			: 寻找最短路径(核心寻路过程)
	@brief			: 
	@param ptStart  : 起点坐标
	@param ptEnd    : 终点坐标
	@param bChangeEnd : 是否寻求最近点
	@return         : 返回搜路是否成功 
	*/
	bool FindPathAStar(const xs::Point& ptFrom, const xs::Point& ptTo ,bool bChangeEnd );
	/**
	@name			: 获取H值
	@brief			: 
	@param m_nH  : 起点坐标
	@return         : 返回H值
	*/
	int GetHValue( int nH);

	/**
	@name			: m_bDir用于判断是否为对角线,当m_bDir==true时，为斜向，否则为正向
	@brief			: 
	@param m_nH  : 起点坐标
	@return         : 返回H值
	*/
	int GetGGValue( int m_nG,bool m_bDir);
	/**
	@name			: 
	@brief			: 
	*/
	inline  void SearchPos(int m_nIdPos ,int m_nIdPrePos,bool m_b);
	/**
	@name			: 
	@brief			: 
	*/
	void DoInsert(int nfromPre,int ntoNow,bool b);
	/**
	@name			: 
	@brief			: 
	*/
	bool FindResult(int nEndId,int nEndPreId);

	/**
	@name			: 
	@brief			: 
	*/
	bool QueryResult(xs::Point*& lstPoint, int& count);
public:
	OpenTempList m_mOpenTempList;
	OpenList     m_mOpenList;       
	CloseList    m_mCloseList;
	int m_nwidth;      //地图列数
	int m_nlength;     //地图行数
	int m_nPathCount;   //走过的格子数
	int	m_nMapCount;
	int m_nStart;    //起点结点编号
	int m_nEnd;      //终点结点编号	
	int m_nxEnd;//
	int m_nyEnd;//

};
// 沿直路靠近目标点
#define  TEMPLATEMAP_DECLARE		template<class _Map,typename _BlockTestFunc,int MAX_PATH_LEN,int MAX_SEARCH_AREA>
#define  PATH_FINDERASTAR_DECLARE	CPathFinderByAStar<_Map,_BlockTestFunc,MAX_PATH_LEN,MAX_SEARCH_AREA>
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////*A寻路算法*/////////////////////////////////////////////////////
/**
@name           : 设置地图信息 
@brief          : 调用FindPath之前请设置地图信息,该函数可以重复调用,所以可以在多个地图上搜索路径
@param nMapWidth: 地图宽度
@param nMapHeigh: 地图高度
@param pMap     : 地图对象指针
@param isBlock  : 判断地表是否阻挡的函数,如果地图对象判断阻挡的函数是IsBlock则该参数不用传
@return         : 
*/
TEMPLATEMAP_DECLARE
bool PATH_FINDERASTAR_DECLARE::SetMapInfo(unsigned long nMapWidth,unsigned long nMapHeight,_Map * pMap,_BlockTestFunc isBlock)
{

	delete[] stAStarPoint;
	m_nMapWidth = nMapWidth;
	m_nMapHeight= nMapHeight;

	m_pMap = pMap;
	m_BlockFunc = isBlock;
	m_mOpenTempList.clear(); //clean tempopen   list 
	m_mOpenList.clear(); 	//clean open   list 
	m_mCloseList.clear();   //clean close  list
	m_nwidth = nMapWidth;    //地图列数  
	m_nlength = nMapHeight;   //地图行数  
	m_nMapCount = m_nwidth * m_nlength;	

	m_nPathCount = 0;//
	stAStarPoint = new stPathNode[m_nMapCount];
	memset(stAStarPoint, 0, sizeof(stPathNode) * m_nMapCount);
	for (int y = 0; y < m_nlength; y++) 
	{	 
		int numtemp=y * m_nwidth;
		for (int x = 0; x < m_nwidth; x++)	 				
		{
			stAStarPoint[numtemp + x ].m_PointPos.x = x;
			stAStarPoint[numtemp + x ].m_PointPos.y = y;	
		}	
	}
	return true;
}
/**
@name			: 搜索路径
@brief			: 
@param ptStart  : 起点坐标
@param ptEnd    : 终点坐标
@param pPathRet : 返回的路径点
@param nPathLen : 返回路径长度
@param nOption  : 搜路选项,是否优化路径
@param nMaxStep : 最多搜索的步数,如果太远就放弃 (这个参数有两个意义 1.有时候只需要搜出最近几步的路,并不一定非要到达目标点 2.默认设为1024,防止出现死循环)
@return         : 返回搜路是否成功 
*/
TEMPLATEMAP_DECLARE
bool PATH_FINDERASTAR_DECLARE::FindPathByASatr(const xs::Point & ptStart,const xs::Point & ptEnd,xs::Point *& pPath,int & nPathLen,int nOption,int nMaxStep)
{
	if(FindPathAStar(ptStart, ptEnd ,true))
	{
		QueryResult(pPath,nPathLen);
		return true;
	}
	return false;
}

/**
@name			: 是否可以走直线
@brief			: 
@param ptStart  : 起点坐标
@param ptEnd    : 终点坐标
@return         : 返回搜路是否成功 
*/
TEMPLATEMAP_DECLARE
bool PATH_FINDERASTAR_DECLARE::CanWalkDirect(const xs::Point& ptFrom, const xs::Point& ptTo)
{
	int dx = ptTo.x - ptFrom.x;
	int dy = ptTo.y - ptFrom.y;

	if (dx == 0 || dy == 0 || abs(dx) == abs(dy))
	{
		xs::Point ptStep = ptTo - ptFrom;
		if (ptStep.x != 0)
			ptStep.x /= abs(ptStep.x);
		if (ptStep.y != 0)
			ptStep.y /= abs(ptStep.y);
		for (xs::Point temp = ptFrom; temp != ptTo; temp += ptStep)
		{
			bool bBlock =  m_BlockFunc(m_pMap,temp);
			if (bBlock)
			{
				return false;
			}
		}
		return true;
	}

	xs::Point ptReFr, ptReTo;   //ptReFr跟起点关联的对角点，ptReTo跟终点关联的对角点
	unsigned int uiadx = abs(dx), uiady = abs(dy);
	int signedDx = (dx > 0) ? 1 : -1;
	int signedDy = (dy > 0) ? 1 : -1;
	if (uiadx > uiady)
	{
		ptReFr.y = ptTo.y;
		ptReFr.x = ptFrom.x + uiady*signedDx;
		ptReTo.y = ptFrom.y;
		ptReTo.x = ptTo.x - uiady*signedDx;
	}
	else
	{
		ptReFr.x = ptTo.x;
		ptReFr.y = ptFrom.y + uiadx*signedDy;
		ptReTo.x = ptFrom.x;
		ptReTo.y = ptTo.y - uiadx*signedDy;
	}

	xs::Point ptStepCro = ptReFr - ptFrom;    //对角距离
	xs::Point ptStepVer = ptReTo - ptFrom;    //垂直距离
	ptStepCro /= abs(ptStepCro.x);
	if (ptStepVer.x != 0)
		ptStepVer /= abs(ptStepVer.x);
	else
		ptStepVer /= abs(ptStepVer.y);

	xs::Point ptStart = ptFrom, ptEnd = ptReTo, ptCur;
	for (; ptStart != ptReFr + ptStepCro; ptStart += ptStepCro, ptEnd += ptStepCro)
	{
		for (ptCur = ptStart; ptCur != ptEnd + ptStepVer; ptCur += ptStepVer)
		{
			bool bBlock =  m_BlockFunc(m_pMap,ptCur);
			if (bBlock)
			{
				return false;
			}
		}
	}
	return true;
}
/**
@name			: 寻找最短路径(核心寻路过程)
@brief			: 
@param ptStart  : 起点坐标
@param ptEnd    : 终点坐标
@param bChangeEnd : 是否寻求最近点
@return         : 返回搜路是否成功 
*/
TEMPLATEMAP_DECLARE
bool PATH_FINDERASTAR_DECLARE::FindPathAStar(const xs::Point& ptFrom, const xs::Point& ptTo ,bool bChangeEnd )
{
	m_mOpenList.clear(); 	//clean open   list 
	m_mCloseList.clear();   //clean close  list 
	m_mOpenTempList.clear();		

	m_nStart = ptFrom.y*m_nwidth+ptFrom.x;
	m_nEnd = ptTo.y*m_nwidth+ptTo.x;
	m_nPathCount = 0;			

	if(!bChangeEnd)
	{
		if(!m_BlockFunc(m_pMap,ptTo))  //说明终点是不可走的 找个就近点 可能是障碍物没打好，有时会有陷入死点
		{			
			int findrang=1;	
			bool bfind=false;						
			while(findrang<=36) //搜索周围6点内的值	
			{
				for(int i=-findrang;abs(i)<=findrang;i++)
				{					
					for(int j=-findrang;abs(j)<=findrang;j++)
					{
						if((abs(i)<findrang)&&(abs(j)<findrang))
						{
							continue;
						}
						else
						{
							mapNearPoint.x=ptTo.x+i;
							mapNearPoint.y=ptTo.y+j;
							
							// 超出地图范围
							if(mapNearPoint.x > m_nwidth || mapNearPoint.y > m_nlength || mapNearPoint.x < 0 || mapNearPoint.y < 0)
							{
								continue;
							}
							m_nEnd=mapNearPoint.y*m_nwidth+mapNearPoint.x;	
							if(!m_BlockFunc(m_pMap,ptTo)) //找到
							{
								bfind=true; 
								break;
							}
						}
					}
					if(bfind) 
					{
						break;
					}
				}
				if(bfind)
				{
					break;
				}
				findrang++;
			}
			if(!bfind)
			{
				return false;	
			}
		}
	}
	else 	
	{
		if(m_BlockFunc(m_pMap,ptTo)) 
		{
			return false; //终点是不可走的
		}
	}

	if(m_nStart == m_nEnd) 
	{
		return false;
	}
	m_nxEnd = m_nEnd%m_nwidth;//
	m_nyEnd = m_nEnd/m_nwidth;//

	//for(int i=0;i<m_nMapCount;i++) 
	//{
	//	stAStarPoint[i].pParent=NULL;
	//}

	if(m_mCloseList.empty())//add the first nod to the open list 
	{			
		stAStarPoint[m_nStart].pParent=NULL;
		stAStarPoint[m_nStart].m_nGvalue=0;			
		stAStarPoint[m_nStart].m_nPointvalue=GetHValue(m_nStart);			
		m_mOpenList.insert(m_nStart); 	//
		m_mOpenTempList.insert(OpenTempList::value_type(stAStarPoint[m_nStart].m_nPointvalue,m_nStart)); 
	}

	////////////////////////////start:处理是否可以直线到目标//////////////////////////////////////////
	//if (CanWalkDirect(ptFrom, ptTo))
	//{
	//	m_nPathCount = 1;
	//	stAStarPoint[m_nEnd].pParent = &stAStarPoint[m_nStart];
	//	return true;
	//}
	//////////////////////////end:处理是否可以直线到目标//////////////////////////////////////////

	//下面是A*算法的具体实现过程	
	while(m_mOpenList.size())
	{
		int CurrentMinValuePosId;	

		itTempOpen1 = m_mOpenTempList.begin(); 
		CurrentMinValuePosId = itTempOpen1->second; 

		itTempOpen2 = itTempOpen1;
		itTempOpen2++;
		m_mOpenTempList.erase(itTempOpen1,itTempOpen2);

		int MinX,MinY;
		MinX=stAStarPoint[CurrentMinValuePosId].m_PointPos.x;
		MinY=stAStarPoint[CurrentMinValuePosId].m_PointPos.y;

		//扩大搜索区域，若被搜索的点为目标点，则结束。
		//向下半部分扩散
		if(MinY != m_nlength-1) 
		{					
			if(CurrentMinValuePosId+m_nwidth == m_nEnd)
				return FindResult(m_nEnd, CurrentMinValuePosId);					
			SearchPos(CurrentMinValuePosId+m_nwidth,CurrentMinValuePosId,false);
			//左下方
			if(MinX != 0)               
			{
				if(CurrentMinValuePosId+m_nwidth-1 == m_nEnd) 
					return FindResult(m_nEnd, CurrentMinValuePosId); 				
				SearchPos(CurrentMinValuePosId+m_nwidth-1,CurrentMinValuePosId,true);		
			}
			//右下方
			if(MinX != (m_nwidth-1))    
			{
				if(CurrentMinValuePosId+m_nwidth+1 == m_nEnd) 
					return FindResult(m_nEnd, CurrentMinValuePosId);//在右下方找到				
				SearchPos(CurrentMinValuePosId+m_nwidth+1,CurrentMinValuePosId,true);	
			}
		}

		//向上扩散	
		if(MinY > 0 )
		{
			//正上方
			if(CurrentMinValuePosId-m_nwidth == m_nEnd)  
				return FindResult(m_nEnd, CurrentMinValuePosId); 			
			SearchPos(CurrentMinValuePosId-m_nwidth,CurrentMinValuePosId,false);
			//左上方
			if(MinX!=0)              
			{		
				if(CurrentMinValuePosId-m_nwidth-1 == m_nEnd) 
					return FindResult(m_nEnd, CurrentMinValuePosId);  			
				SearchPos(CurrentMinValuePosId-m_nwidth-1,CurrentMinValuePosId,true);							
			}
			//右上方
			if(MinX != (m_nwidth-1))   
			{
				if(CurrentMinValuePosId-m_nwidth+1 == m_nEnd)  
					return FindResult(m_nEnd, CurrentMinValuePosId); //右上方找到				
				SearchPos(CurrentMinValuePosId-m_nwidth+1,CurrentMinValuePosId,true);	
			}
		}
		//向右扩散
		if(MinX != (m_nwidth-1))	
		{
			if(CurrentMinValuePosId+1 == m_nEnd)
				return FindResult(m_nEnd, CurrentMinValuePosId);			
			SearchPos(CurrentMinValuePosId+1,CurrentMinValuePosId,false);						
		}
		//向左扩散
		if(MinX > 0)   
		{
			if(CurrentMinValuePosId-1 == m_nEnd) 
				return FindResult(m_nEnd, CurrentMinValuePosId);			
			SearchPos(CurrentMinValuePosId-1,CurrentMinValuePosId,false);						
		}	
		//将此代价值为最低的项点移到close表中 并从open表中删去
		m_mCloseList.insert(CurrentMinValuePosId); //
		m_mOpenList.erase(CurrentMinValuePosId);//itOpenTemp			
	}
	m_mOpenList.clear(); 	//clean open   list 
	m_mCloseList.clear();   //clean close  list 
	m_mOpenTempList.clear();
	return false; //open表为空，表明不存在最短路径
}
/**
@name			: 获取H值
@brief			: 
@param m_nH  : 起点坐标
@return         : 返回H值
*/
TEMPLATEMAP_DECLARE
int  PATH_FINDERASTAR_DECLARE::GetHValue( int nH)
{
	int dx,dy;		
	dx=m_nxEnd - nH%m_nwidth;	//
	dy=m_nyEnd - nH/m_nwidth;	
	return dx*dx+dy*dy;	
}

/**
@name			: m_bDir用于判断是否为对角线,当m_bDir==true时，为斜向，否则为正向
@brief			: 
@param m_nH  : 起点坐标
@return         : 返回H值
*/
TEMPLATEMAP_DECLARE
int  PATH_FINDERASTAR_DECLARE::GetGGValue( int nG,bool bDir)
{
	if(bDir)
		return  stAStarPoint[nG].pParent->m_nGvalue+14;
	else 		
		return  stAStarPoint[nG].pParent->m_nGvalue+10;
}
/**
@name			: 
@brief			: 
*/
TEMPLATEMAP_DECLARE
void  PATH_FINDERASTAR_DECLARE::SearchPos(int m_nIdPos ,int m_nIdPrePos,bool m_b)
{
	bool bBlock =  m_BlockFunc(m_pMap,stAStarPoint[m_nIdPos].m_PointPos);
	if(!bBlock)//判断目标是否可走，可走续继
	{ 
		if(m_mCloseList.find(m_nIdPos)==m_mCloseList.end())   //close中找不到			
		{
			if(m_mOpenList.find(m_nIdPos)!=m_mOpenList.end()) //已在open表中  判断两个点，哪个估价值更低					
			{
				int k=10;
				if(m_b) k=14;
				if(stAStarPoint[m_nIdPrePos].m_nGvalue + k < stAStarPoint[m_nIdPos].m_nGvalue)
				{
					///////////////////////////////////////////////////////////////////////////////
					itTempOpen1 = m_mOpenTempList.find(stAStarPoint[m_nIdPos].m_nPointvalue);
					while(m_nIdPos != itTempOpen1->second)
					{
						itTempOpen1++;
					}
					itTempOpen2 = itTempOpen1;
					itTempOpen2++;
					m_mOpenTempList.erase(itTempOpen1,itTempOpen2);
					///////////////以上代码不要则可以找到接近最近的路径////////////////////////////

					stAStarPoint[m_nIdPos].pParent = &stAStarPoint[m_nIdPrePos];
					stAStarPoint[m_nIdPos].m_nGvalue =stAStarPoint[m_nIdPrePos].m_nGvalue + k;//小心
					stAStarPoint[m_nIdPos].m_nPointvalue=stAStarPoint[m_nIdPos].m_nGvalue+GetHValue(m_nIdPos);

					///////////////////////////////////////////////////////////////////////////////
					m_mOpenTempList.insert(OpenTempList::value_type(stAStarPoint[m_nIdPos].m_nPointvalue,m_nIdPos)); 	
					///////////////以上代码不要则可以找到接近最近的路径////////////////////////////
				}
			}	
			else
			{ 
				DoInsert(m_nIdPrePos,m_nIdPos,m_b);//不在open表中,将其存入
			}
		}
	}
}
/**
@name			: 
@brief			: 
*/
TEMPLATEMAP_DECLARE
void  PATH_FINDERASTAR_DECLARE::DoInsert(int nfromPre,int ntoNow,bool b)
{
	stAStarPoint[ntoNow].pParent = &stAStarPoint[nfromPre];
	stAStarPoint[ntoNow].m_nGvalue = GetGGValue(ntoNow,b);
	stAStarPoint[ntoNow].m_nPointvalue=stAStarPoint[ntoNow].m_nGvalue+GetHValue(ntoNow);
	m_mOpenList.insert(ntoNow);
	m_mOpenTempList.insert(OpenTempList::value_type(stAStarPoint[ntoNow].m_nPointvalue,ntoNow)); 
}
/**
@name			: 
@brief			: 
*/
TEMPLATEMAP_DECLARE
bool  PATH_FINDERASTAR_DECLARE::FindResult(int nEndId,int nEndPreId)
{
	stAStarPoint[m_nEnd].pParent = &stAStarPoint[nEndPreId];
	m_nPathCount=0;
	stPathNode  *pp = &stAStarPoint[m_nEnd];		
	while(pp) 
	{	
		m_PathTemp[m_nPathCount] = pp->m_PointPos;
		m_nPathCount++;
		pp = pp->pParent;	
	}
	//m_nPathCount--;			
	return true;
}

/**
@name			: 
@brief			: 
*/
TEMPLATEMAP_DECLARE
bool  PATH_FINDERASTAR_DECLARE::QueryResult(xs::Point*& lstPoint, int& count)
{
	lstPoint = 0;
	count = 0;
	count = m_nPathCount;
	for (int i = 0;i < m_nPathCount;i++)
	{
		m_PathResult[i] = m_PathTemp[m_nPathCount - i - 1];
	}
     lstPoint = &m_PathResult[0];
	//if (lstPoint)
	//{
	//	int n = m_nPathCount;
	//	stPathNode  *ppp = &stAStarPoint[m_nEnd];			
	//	while(ppp)
	//	{	
	//		n--;
	//		if( n<0 ) 
	//			break;
	//		lstPoint[n]=ppp->m_PointPos;
	//		ppp=ppp->pParent;				
	//	}
	//}
	return true;
}
#endif//__CPATH_FINDERBYASTAR_H__