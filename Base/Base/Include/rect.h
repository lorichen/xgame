
#ifndef __RECT_H__
#define __RECT_H__

#include "point.h"

namespace xs
{

struct Rect
{
    long left, top, right, bottom;
    
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
    bool ptInRect(const Point& pt) const
    {
        return pt.x >= left && pt.x <= right && pt.y >= top && pt.y <= bottom;
    }
};
    
}


//--------------------------

inline bool PtInRect(const xs::Rect* rc,xs::Point pt)
{
    return rc->ptInRect(pt);
}

inline bool IntersectRect(xs::Rect* rcout,const xs::Rect* rc1,const xs::Rect* rc2)
{
    return rc1->intersect(*rc2,*rcout);
}

inline bool OffsetRect( xs::Rect* rc,int dx,int dy)
{
    rc->left += dx;
    rc->right += dx;
    rc->top += dy;
    rc->bottom += dy;
    return true;
}

inline bool EqualRect(const xs::Rect* rc1,const xs::Rect* rc2)
{
    return *rc1 == *rc2;
}

inline bool UnionRect(xs::Rect* rcout,const xs::Rect *rc1,const xs::Rect *rc2)
{
    if(rc1->left < rc2->left)
        rcout->left = rc1->left;
    else
        rcout->left = rc2->left;
    
    if(rc1->right > rc2->right)
        rcout->right = rc1->right;
    else
        rcout->right = rc2->right;
    
    if(rc1->top < rc2->top)
        rcout->top = rc1->top;
    else
        rcout->top = rc2->top;
    
    if(rc1->bottom > rc2->bottom)
        rcout->bottom = rc1->bottom;
    else
        rcout->bottom = rc2->bottom;
    
    return true;
}

inline bool SetRect(xs::Rect* rc,int left,int top,int right,int bottom)
{
    rc->left = left;
    rc->top = top;
    rc->right = right;
    rc->bottom = bottom;
    return true;
}

inline bool InflateRect( xs::Rect*  rc,int dx,int dy)
{
    rc->left    -= dx;
    rc->right   += dx;
    rc->top     -= dy;
    rc->bottom  += dy;
    return true;
}

#endif //