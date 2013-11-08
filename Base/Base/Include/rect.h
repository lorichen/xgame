
#ifndef __RECT_H__
#define __RECT_H__

namespace xs
{

struct Rect
{
    int left, top, right, bottom;
    
    Rect()
    {
    }
    Rect( int l, int t, int r, int b )
    {
        left = l;
        top = t;
        right = r;
        bottom = b;
    }
    Rect& operator = ( const Rect& other )
    {
        left = other.left;
        top = other.top;
        right = other.right;
        bottom = other.bottom;
        
        return *this;
    }
    
    void  moveTo00()
    {
        if(left > right){int t = left - right;left = 0;right = left + t;}
        else {int t = right - left;left = 0;right = left + t;}
        if(top > bottom){int t = top - bottom;top = 0;bottom = top + t;}
        else {int t = bottom - top;top = 0;bottom = top + t;}
    }
    
    bool operator==(const Rect& other) const
    {
        return left == other.left && right == other.right && top == other.top && bottom == other.bottom;
    }
    
    bool operator!=(const Rect& other) const
    {
        return !(*this == other);
    }
    
    int width() const
    {
        return right - left + 1;
    }
    int height() const
    {
        return bottom - top + 1;
    }
    bool intersect(const Rect& rcOther,Rect& rcIntersect) const
    {
        Rect rc = *this;
        if(rc.left < rcOther.left)rc.left = rcOther.left;
        if(rc.top < rcOther.top)rc.top = rcOther.top;
        if(rc.right > rcOther.right)rc.right = rcOther.right;
        if(rc.bottom > rcOther.bottom)rc.bottom = rcOther.bottom;
        
        if(rc.right < rc.left || rc.bottom < rc.top)return false;
        
        rcIntersect = rc;
        return true;
    }
    bool ptInRect(const Point& pt)
    {
        return pt.x >= left && pt.x <= right && pt.y >= top && pt.y <= bottom;
    }
};

}
#endif //