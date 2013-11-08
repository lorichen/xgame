

#ifndef __POINT_H__
#define __POINT_H__

namespace xs
{
    
struct Point
{
    int x,y;
    
    Point(){}
    Point(int x,int y) : x(x),y(y){}
    Point& operator = (const Point& pt)
    {
        x = pt.x;
        y = pt.y;
        return *this;
    }
    bool operator==(const Point& pt)
    {
        return x == pt.x && y == pt.y;
    }
    bool operator!=(const Point& pt)
    {
        return x != pt.x || y != pt.y;
    }
};
    
}


#endif //