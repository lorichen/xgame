#include "stdafx.h"
#include "WayPoint.h"
#include "math.h"

WayPoint::WayPoint()
{
	m_nID = 0;
	m_bIsGate = FALSE;
}

WayPoint::~WayPoint()
{

}


void WayPoint::setPointID(int nID)
{
	m_nID = nID;
}

int WayPoint::distance(int x, int y)
{	
	int DX = abs(x - m_Pos.x);
	int DY = abs(y - m_Pos.y);
	
	return sqrt((float)(DX * DX) + DY * DY);
}

size_t WayPoint::getID(void)
{	
	return  idFromPos(m_Pos.x, m_Pos.y);
}

const POINT& WayPoint::getPos(void)
{
	return m_Pos;
}


const std::list<std::pair<size_t, int> >& WayPoint::getAdjoingList(void) const
{
	return m_lsAdjoining;
}


void WayPoint::addEdge(size_t VertaxID, int nPower)
{
	m_lsAdjoining.push_back(std::make_pair(VertaxID, nPower));
}

void WayPoint::setPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

size_t WayPoint::idFromPos(int x, int y)
{
	return  x << 16 | y;
}

void WayPoint::setGate(BOOL bFlag)
{
	m_bIsGate = bFlag;
}

BOOL WayPoint::isGate(void)
{
	return m_bIsGate;
}
