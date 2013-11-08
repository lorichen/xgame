#ifndef __TYPE__H__
#define __TYPE__H__

typedef unsigned int			uint;
typedef unsigned char			uchar;
typedef unsigned short			ushort;
typedef unsigned long			ulong;


//----------------non win32-------------------------------
#if (TARGET_PLATFORM != PLATFORM_WIN32)

typedef unsigned long size_t;
typedef unsigned long size_type;

typedef unsigned int  DWORD;
typedef unsigned short WORD;
typedef void*         HWND;
typedef void*         HTREEITEM;
typedef unsigned long ULONG;
typedef void*         HANDLE;
typedef char*         LPCSTR;
typedef unsigned char BYTE;
typedef long          LONG;
typedef unsigned int  UINT;
typedef unsigned int* WPARAM;
typedef unsigned long* LPARAM;

typedef unsigned int            uint32;
typedef unsigned short          uint16;
typedef float                   Real;

typedef long long               __int64;
typedef long long       LONGLONG;

//#define min			std::min

typedef void* HINSTANCE;

#define IN
#define OUT

#define BOOL bool
#define TRUE true
#define FALSE false

#define __cdecl 
#define _cdecl

#define MAX_PATH 256
#define OutputDebugStringA(msg) printf(msg)

#define _fileno fileno
#define _stat stat

#define _chsize chsize
#define _access access

#define sprintf_s snprintf
#define _vsnprintf vsnprintf

#define FORCEINLINE inline

#define _stricmp strcmp

#define WM_USER       0x0400


inline char *_itoa(int num,char *str,int radix)
{
    char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    unsigned unum;
    int i=0,j,k;
    
    if(radix==10&&num<0)
    {
        unum=(unsigned)-num;
        str[i++]='-';
    }
    else unum=(unsigned)num;
    
    do{
        str[i++]=index[unum%(unsigned)radix];
        unum/=radix;
    }while(unum);
    str[i]='\0';
    
    if(str[0]=='-') k=1;
    else k=0;
    char temp;
    for(j=k;j<=(i-1)/2;j++)
    {
        temp=str[j];
        str[j] = str[i-1+k-j];
        str[i-1+k-j] = temp;
    }
    return str;
}

#define itoa _itoa

#include <ctype.h>
inline char *strupr(char *str)
{
    char *ptr = str;
    
    while (*ptr != '\0')
    {
        if (islower(*ptr))
            *ptr = toupper(*ptr);
        ptr++;
    }
    return str;
}

#include <iostream>
#include <string>
#include <ext/hash_map>
using namespace __gnu_cxx;
#define HASH_MAP_NAMESPACE __gnu_cxx
namespace __gnu_cxx
{
    template<> struct hash< std::string >
    {
        size_t operator()( const std::string& x ) const
        {
            //return hash< const char* >()( x.c_str() );
            
            const char *key = x.c_str();
            size_t len = x.length();
            unsigned int hash, i;
            for(hash = i = 0; i < len; ++i)
            {
                hash += key[i];
                hash += (hash << 10);
                hash ^= (hash >> 6);
            }
            hash += (hash << 3);
            hash ^= (hash >> 11);
            hash += (hash << 15);
            return hash;
        }
    };
    
    template<> struct hash<long long>
    {
        size_t operator()(long long x) const
        {
            return x;
        }
    };
    
    template<> struct hash<void*>
    {
        size_t operator()(void* x) const
        {
            return (size_t)x;
        }
    };
}


extern unsigned int GetCurrentThreadId();

#include <unistd.h>
#include <fcntl.h>

#include <math.h>
#define FLT_MAX 3.40282347e+38F


inline void ZeroMemory(void* p,unsigned int size)
{
    memset(p,0x00,size);
}

inline char* strlwr( char* str )
{
    char* orig = str;
    // process the string
    for ( ; *str != '\0'; str++ )
        *str = tolower(*str);
    return orig;
}


struct RECT
{
    long left;
    long top;
    long right;
    long bottom;
};

typedef RECT* LPRECT;

struct POINT
{
    long x;
    long y;
    
    POINT()
    :x(0),y(0)
    {
        
    }
    
    POINT(long _x,long _y)
    :x(_x),y(_y)
    {
    }
    
    POINT(int _x,int _y)
    :x(_x),y(_y)
    {
    }
    
    POINT& operator=(const POINT& p)
    {
        x = p.x;
        y = p.y;
        return *this;
    }
    
    POINT& operator+=(const POINT& p2)
    {
        x += p2.x;
        y += p2.y;
        return *this;
    }
    
    POINT& operator-=(const POINT& p2)
    {
        x -= p2.x;
        y -= p2.y;
        return *this;
    }
    
    POINT& operator/=(const long& n)
    {
        x /= n;
        y /= n;
        return *this;
    }
  
    
    POINT operator + (const POINT& p2) const
    {
        POINT pt(x,y);
        pt.x += p2.x;
        pt.y += p2.y;
        return pt;
    }
    
    POINT operator - (const POINT& p2) const
    {
        POINT pt(x,y);
        pt.x -= p2.x;
        pt.y -= p2.y;
        return pt;
    }
    
    bool operator != (const POINT& p) const
    {
        return !(*this == p);
    }
    
    bool operator == (const POINT& p) const
    {
        if(x == p.x && y == p.y)
            return true;
        return false;
    }

};

struct SIZE
{
    long cx;
    long cy;
};

inline bool PtInRect(const RECT* rc,POINT& pt)
{
    return (
        rc->left <= pt.x &&
        rc->top <= pt.y  &&
        rc->right >= pt.x &&
        rc->bottom >= pt.y
    );
}

inline bool IntersectRect(RECT* rcout,const RECT* rc1,const RECT* rc2)
{
    if(rc1->bottom > rc2->top &&
       rc1->top < rc2->bottom &&
       rc1->right > rc2->left &&
       rc1->left < rc2->right
       )
    {
        rcout->left = rc1->left;
        rcout->right = rc1->right;
        rcout->top = rc1->top;
        rcout->bottom = rc1->bottom;
        
        if(rc2->right < rc1->left)
            rcout->right = rc2->right;
        
        if(rc2->bottom < rc1->bottom)
            rcout->bottom = rc2->bottom;
        
        if(rc2->left > rc1->left)
            rcout->left = rc2->left;
        
        if(rc2->top > rc1->top)
            rcout->top = rc2->top;
        
        if(rcout->top > rcout->bottom)
            rcout->top = rcout->bottom;
        
        if(rcout->left > rcout->right)
            rcout->left = rcout->right;
        
        return true;
    }
    else
    {
        rcout->left = rcout->top = rcout->right = rcout->bottom = 0;
        return false;
    }
    
}

inline bool OffsetRect( RECT* rc,int dx,int dy)
{
    rc->left += dx;
    rc->right += dx;
    rc->top += dy;
    rc->bottom += dy;
    return true;
}

inline bool EqualRect(const RECT* rc1,const RECT* rc2)
{
    return (
        rc1->left == rc2->left &&
        rc1->right == rc2->right &&
        rc1->top == rc2->top &&
        rc1->bottom == rc2->bottom
    );
}

inline bool UnionRect(RECT* rcout,const RECT *rc1,const RECT *rc2)
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

inline bool SetRect(RECT* rc,int left,int top,int right,int bottom)
{
    rc->left = left;
    rc->top = top;
    rc->right = right;
    rc->bottom = bottom;
    return true;
}

inline bool InflateRect( RECT*  rc,int dx,int dy)
{
    rc->left    -= dx;
    rc->right   += dx;
    rc->top     -= dy;
    rc->bottom  += dy;
    return true;
}


#else   //--------------  win32  -----------------

#include <direct.h>
#include <hash_map>
#include <io.h>
#include <direct.h>
using namespace stdext;
#define HASH_MAP_NAMESPACE stdext

#endif

//-------------------------------------------------------

//just for ios
#if (TARGET_PLATFORM == PLATFORM_IOS)

#include <CoreFoundation/CFBase.h>
#include <mach/mach_time.h>
#include <sys/_types/_timespec.h>
#define ORWL_NANO (+1.0E-9)
#define ORWL_GIGA UINT64_C(1000000000)

static double orwl_timebase = 0.0;
static uint64_t orwl_timestart = 0;

inline  struct timespec clock_gettime(void)
{
    // be more careful in a multithreaded environement
    if (!orwl_timestart) {
        mach_timebase_info_data_t tb = { 0 };
        mach_timebase_info(&tb);
        orwl_timebase = tb.numer;
        orwl_timebase /= tb.denom;
        orwl_timestart = mach_absolute_time();
    }
    struct timespec t;
    double diff = (mach_absolute_time() - orwl_timestart) * orwl_timebase;
    t.tv_sec = diff * ORWL_NANO;
    t.tv_nsec = diff - (t.tv_sec * ORWL_GIGA);
    return t;
}

#endif





#endif