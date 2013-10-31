#ifndef     __FT__
#define     __FT__

#include "StdAfx.h"

namespace xs{


typedef double   FT_DOUBLE;
typedef float	 FT_FLOAT;

// Fixes for deprecated identifiers in 2.1.5
#ifndef FT_OPEN_MEMORY
    #define FT_OPEN_MEMORY (FT_Open_Flags)1
#endif

#ifndef FT_RENDER_MODE_MONO
    #define FT_RENDER_MODE_MONO ft_render_mode_mono
#endif

#ifndef FT_RENDER_MODE_NORMAL
    #define FT_RENDER_MODE_NORMAL ft_render_mode_normal
#endif

  
#ifdef WIN32

    // Under windows avoid including <windows.h> is overrated. 
    // Sure, it can be avoided and "name space pollution" can be
    // avoided, but why? It really doesn't make that much difference
    // these days.
    #define  WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else

#endif

// Compiler-specific conditional compilation
#ifdef _MSC_VER // MS Visual C++ 

    // Disable various warning.
    // 4786: template name too long
    #pragma warning( disable : 4251 )
    #pragma warning( disable : 4275 )
    #pragma warning( disable : 4786 )
    #pragma warning( disable : 4244 )

#else
#endif  


}
#endif  //  __FT__
