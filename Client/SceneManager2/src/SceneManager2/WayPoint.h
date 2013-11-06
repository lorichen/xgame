#ifndef __WayPoint_H__
#define __WayPoint_H__

#include <list>

class WayPoint 
{
public:
	WayPoint();
	virtual ~WayPoint();
	int distance(int x, int y);

	size_t getID(void);

	const POINT& getPos(void);

	const std::list<std::pair<size_t, int> >& getAdjoingList(void) const;

	void addEdge(size_t VertaxID, int nPower);

	void setPos(int x, int y);

	void setPointID(int nID);

	void setGate(BOOL bFlag);

	BOOL isGate(void);
public:
	static size_t idFromPos(int x, int y);
	typedef std::list<std::pair<size_t, int> > EdgeList;

private:
	POINT m_Pos;
	EdgeList m_lsAdjoining;
	int m_nID;
	BOOL m_bIsGate;
};

#endif
