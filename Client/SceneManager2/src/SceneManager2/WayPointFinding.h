#ifndef __WayPointFinding_H__
#define __WayPointFinding_H__

struct WayPointFinding  
{
public:
	
	int x, y;
	
	int f;
	int g;
	int h;
	size_t parent;
	
	WayPointFinding();
	bool operator < (const WayPointFinding& other) const;
	bool operator == (const WayPointFinding& other) const;
	void Setpos(int nx, int ny);
	void Setpos(const xs::Point &pt);
	xs::Point getPos(void) const; 
};

#endif

