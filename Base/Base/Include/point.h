

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
    
    Point& operator+=(const Point& p2)
    {
        x += p2.x;
        y += p2.y;
        return *this;
    }
    
    Point& operator-=(const Point& p2)
    {
        x -= p2.x;
        y -= p2.y;
        return *this;
    }
    
    Point& operator/=(const int n)
    {
        x /= n;
        y /= n;
        return *this;
    }
    
    Point operator + (const Point& p2) const
    {
        Point pt(x,y);
        pt.x += p2.x;
        pt.y += p2.y;
        return pt;
    }
    
    Point operator - (const Point& p2) const
    {
        Point pt(x,y);
        pt.x -= p2.x;
        pt.y -= p2.y;
        return pt;
    }
};
    
}


#endif //