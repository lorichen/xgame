
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/
	
/*****************************************************************************
File:  fastmath.cpp

This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/

/************************************************************************/
/* Nvidia提供的FastMath，不过没有传说中的那么快                         */
/************************************************************************/

#ifndef __Nvidia_fastmath_H__
#define __Nvidia_fastmath_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <wtypes.h>


#define FP_BITS(fp) (*(DWORD *)&(fp))
#define FP_ABS_BITS(fp) (FP_BITS(fp)&0x7FFFFFFF)
#define FP_SIGN_BIT(fp) (FP_BITS(fp)&0x80000000)
#define FP_ONE_BITS 0x3F800000


// r = 1/p
#define FP_INV(r,p)                                                          \
{                                                                            \
    int _i = 2 * FP_ONE_BITS - *(int *)&(p);                                 \
    r = *(float *)&_i;                                                       \
    r = r * (2.0f - (p) * r);                                                \
}

/////////////////////////////////////////////////
// The following comes from Vincent Van Eeckhout
// Thanks for sending us the code!
// It's the same thing in assembly but without this C-needed line:
//    r = *(float *)&_i;

static float TWO = 2.0f;

#define FP_INV2(r,p)                     \
{                                        \
    __asm { mov     eax,0x7F000000    }; \
    __asm { sub     eax,dword ptr [p] }; \
    __asm { mov     dword ptr [r],eax }; \
    __asm { fld     dword ptr [p]     }; \
    __asm { fmul    dword ptr [r]     }; \
    __asm { fsubr   [TWO]             }; \
    __asm { fmul    dword ptr [r]     }; \
    __asm { fstp    dword ptr [r]     }; \
}

/////////////////////////////////////////////////


#define FP_EXP(e,p)                                                          \
{                                                                            \
    int _i;                                                                  \
    e = -1.44269504f * (float)0x00800000 * (p);                              \
    _i = (int)e + 0x3F800000;                                                \
    e = *(float *)&_i;                                                       \
}

#define FP_NORM_TO_BYTE(i,p)                                                 \
{                                                                            \
    float _n = (p) + 1.0f;                                                   \
    i = *(int *)&_n;                                                         \
    if (i >= 0x40000000)     i = 0xFF;                                       \
    else if (i <=0x3F800000) i = 0;                                          \
    else i = ((i) >> 15) & 0xFF;                                             \
}



inline unsigned long FP_NORM_TO_BYTE2(float p)                                                 
{                                                                            
  float fpTmp = p + 1.0f;                                                      
  return ((*(unsigned *)&fpTmp) >> 15) & 0xFF;  
}


inline unsigned long FP_NORM_TO_BYTE3(float p)     
{
  float ftmp = p + 12582912.0f;                                                      
  return ((*(unsigned long *)&ftmp) & 0xFF);
}

static unsigned int fast_sqrt_table[0x10000];  // declare table of square roots 

typedef union FastSqrtUnion
{
  float f;
  unsigned int i;
} FastSqrtUnion;

extern void  build_sqrt_table();

inline float fastsqrt(float n)
{
  
  if (FP_BITS(n) == 0)
    return 0.0;                 // check for square root of 0
  
  FP_BITS(n) = fast_sqrt_table[(FP_BITS(n) >> 8) & 0xFFFF] | ((((FP_BITS(n) - 0x3F800000) >> 1) + 0x3F800000) & 0x7F800000);
  
  return n;
}


// At the assembly level the recommended workaround for the second FIST bug is the same for the first; 
// inserting the FRNDINT instruction immediately preceding the FIST instruction. 

__forceinline void FloatToInt(int *int_pointer, float f) 
{
	__asm  fld  f
  __asm  mov  edx,int_pointer
  __asm  FRNDINT
  __asm  fistp dword ptr [edx];

}

#endif
	/** @} */