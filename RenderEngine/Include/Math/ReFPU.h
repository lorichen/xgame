
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/
	#ifndef __FPU_H__
#define __FPU_H__

#define __IEEE_1_0				0x3f800000					//!< integer representation of 1.0

#define	__SIGN_BITMASK			0x80000000

//! Integer representation of a floating-point value.
#define __IR(x)					((unsigned int&)(x))

//! Signed integer representation of a floating-point value.
#define __SIR(x)					((int&)(x))

//! Absolute integer representation of a floating-point value
#define __AIR(x)					(__IR(x)&0x7fffffff)

//! Floating-point representation of an integer value.
#define __FR(x)					((float&)(x))

//! Integer-based comparison of a floating point value.
//! Don't use it blindly, it can be faster or slower than the FPU comparison, depends on the context.
#define __IS_NEGATIVE_FLOAT(x)	(__IR(x)&0x80000000)

//! Fast fabs for floating-point values. It just clears the sign bit.
//! Don't use it blindy, it can be faster or slower than the FPU comparison, depends on the context.
inline float __FastFabs(float x)
{
	unsigned int FloatBits = __IR(x)&0x7fffffff;
	return __FR(FloatBits);
}

//! Fast square root for floating-point values.
inline float __FastSqrt(float square)
{
	
#if (TARGET_PLATFORM == PLATFORM_WIN32)
	float retval;
    __asm {
		mov             eax, square
			sub             eax, 0x3F800000
			sar             eax, 1
			add             eax, 0x3F800000
			mov             [retval], eax
	}
    return retval;
#else
    return sqrtf(square);
#endif
	
}

//! Saturates positive to zero.
inline float __fsat(float f)
{
	unsigned int y = (unsigned int&)f & ~((int&)f >>31);
	return (float&)y;
}

//! Computes 1.0f / sqrtf(x).
inline float __frsqrt(float f)
{
	float x = f * 0.5f;
	unsigned int y = 0x5f3759df - ((unsigned int&)f >> 1);
	// Iteration...
	(float&)y  = (float&)y * ( 1.5f - ( x * (float&)y * (float&)y ) );
	// Result
	return (float&)y;
}

//! Computes 1.0f / sqrtf(x). Comes from NVIDIA.
inline float __InvSqrt(const float& x)
{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
	unsigned int tmp = (unsigned int(__IEEE_1_0 << 1) + __IEEE_1_0 - *(unsigned int*)&x) >> 1;   
	float y = *(float*)&tmp;                                             
	return y * (1.47f - 0.47f * x * y * y);
#else
    
    //from  by kevin.chen
    float res = x;
    float xhalf = 0.5f * res;
    int i = *(int*)&res;          // get bits for floating value
    i =  0x5f375a86 - (i>>1);    // gives initial guess
    res = *(float*)&i;            // convert bits back to float
    res = res * (1.5f - xhalf*res*res); // Newton step
    return res;
    
#endif
}

//! Computes 1.0f / sqrtf(x). Comes from Quake3. Looks like the first one I had above.
//! See http://www.magic-software.com/3DGEDInvSqrt.html
inline float __RSqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5f;

	x2 = number * 0.5f;
	y  = number;
	i  = * (long *) &y;
	i  = 0x5f3759df - (i >> 1);
	y  = * (float *) &i;
	y  = y * (threehalfs - (x2 * y * y));

	return y;
}

//! TO BE DOCUMENTED
inline float __fsqrt(float f)
{
	unsigned int y = ( ( (int&)f - 0x3f800000 ) >> 1 ) + 0x3f800000;
	// Iteration...?
	// (float&)y = (3.0f - ((float&)y * (float&)y) / f) * (float&)y * 0.5f;
	// Result
	return (float&)y;
}

//! Returns the float ranged espilon value.
inline float __fepsilon(float f)
{
	unsigned int b = (unsigned int&)f & 0xff800000;
	unsigned int a = b | 0x00000001;
	(float&)a -= (float&)b;
	// Result
	return (float&)a;
}

//! Is the float valid ?
inline bool __IsNAN(float value)				{ return (__IR(value)&0x7f800000) == 0x7f800000;	}
inline bool __IsIndeterminate(float value)	{ return __IR(value) == 0xffc00000;				}
inline bool __IsPlusInf(float value)			{ return __IR(value) == 0x7f800000;				}
inline bool __IsMinusInf(float value)		{ return __IR(value) == 0xff800000;				}

inline	bool __IsValidFloat(float value)
{
	if(__IsNAN(value))			return false;
	if(__IsIndeterminate(value))	return false;
	if(__IsPlusInf(value))		return false;
	if(__IsMinusInf(value))		return false;
	return true;
}

#define __CHECK_VALID_FLOAT(x)	ASSERT(__IsValidFloat(x));

/*
//! FPU precision setting function.
inline void SetFPU()
{
// This function evaluates whether the floating-point
// control word is set to single precision/round to nearest/
// exceptions disabled. If these conditions don't hold, the
// function changes the control word to set them and returns
// TRUE, putting the old control word value in the passback
// location pointed to by pwOldCW.
{
uword wTemp, wSave;

__asm fstcw wSave
if (wSave & 0x300 ||            // Not single mode
0x3f != (wSave & 0x3f) ||   // Exceptions enabled
wSave & 0xC00)              // Not round to nearest mode
{
__asm
{
mov ax, wSave
and ax, not 300h    ;; single mode
or  ax, 3fh         ;; disable all exceptions
and ax, not 0xC00   ;; round to nearest mode
mov wTemp, ax
fldcw   wTemp
}
}
}
}
*/
//! This function computes the slowest possible floating-point value (you can also directly use FLT_EPSILON)
inline float __ComputeFloatEpsilon()
{
	float f = 1.0f;
	((unsigned int&)f)^=1;
	return f - 1.0f;	// You can check it's the same as FLT_EPSILON
}

inline bool __IsFloatZero(float x, float epsilon=1e-6f)
{
	return x*x < epsilon;
}

#define __FCOMI_ST0	_asm	_emit	0xdb	_asm	_emit	0xf0
#define __FCOMIP_ST0	_asm	_emit	0xdf	_asm	_emit	0xf0
#define __FCMOVB_ST0	_asm	_emit	0xda	_asm	_emit	0xc0
#define __FCMOVNB_ST0	_asm	_emit	0xdb	_asm	_emit	0xc0

#define __FCOMI_ST1	_asm	_emit	0xdb	_asm	_emit	0xf1
#define __FCOMIP_ST1	_asm	_emit	0xdf	_asm	_emit	0xf1
#define __FCMOVB_ST1	_asm	_emit	0xda	_asm	_emit	0xc1
#define __FCMOVNB_ST1	_asm	_emit	0xdb	_asm	_emit	0xc1

#define __FCOMI_ST2	_asm	_emit	0xdb	_asm	_emit	0xf2
#define __FCOMIP_ST2	_asm	_emit	0xdf	_asm	_emit	0xf2
#define __FCMOVB_ST2	_asm	_emit	0xda	_asm	_emit	0xc2
#define __FCMOVNB_ST2	_asm	_emit	0xdb	_asm	_emit	0xc2

#define __FCOMI_ST3	_asm	_emit	0xdb	_asm	_emit	0xf3
#define __FCOMIP_ST3	_asm	_emit	0xdf	_asm	_emit	0xf3
#define __FCMOVB_ST3	_asm	_emit	0xda	_asm	_emit	0xc3
#define __FCMOVNB_ST3	_asm	_emit	0xdb	_asm	_emit	0xc3

#define __FCOMI_ST4	_asm	_emit	0xdb	_asm	_emit	0xf4
#define __FCOMIP_ST4	_asm	_emit	0xdf	_asm	_emit	0xf4
#define __FCMOVB_ST4	_asm	_emit	0xda	_asm	_emit	0xc4
#define __FCMOVNB_ST4	_asm	_emit	0xdb	_asm	_emit	0xc4

#define __FCOMI_ST5	_asm	_emit	0xdb	_asm	_emit	0xf5
#define __FCOMIP_ST5	_asm	_emit	0xdf	_asm	_emit	0xf5
#define __FCMOVB_ST5	_asm	_emit	0xda	_asm	_emit	0xc5
#define __FCMOVNB_ST5	_asm	_emit	0xdb	_asm	_emit	0xc5

#define __FCOMI_ST6	_asm	_emit	0xdb	_asm	_emit	0xf6
#define __FCOMIP_ST6	_asm	_emit	0xdf	_asm	_emit	0xf6
#define __FCMOVB_ST6	_asm	_emit	0xda	_asm	_emit	0xc6
#define __FCMOVNB_ST6	_asm	_emit	0xdb	_asm	_emit	0xc6

#define __FCOMI_ST7	_asm	_emit	0xdb	_asm	_emit	0xf7
#define __FCOMIP_ST7	_asm	_emit	0xdf	_asm	_emit	0xf7
#define __FCMOVB_ST7	_asm	_emit	0xda	_asm	_emit	0xc7
#define __FCMOVNB_ST7	_asm	_emit	0xdb	_asm	_emit	0xc7

//! A global function to find MAX(a,b) using FCOMI/FCMOV
inline float __FCMax2(float a, float b)
{
    
#if (TARGET_PLATFORM == PLATFORM_WIN32)
	float Res;
	_asm	fld		[a]
	_asm	fld		[b]
	__FCOMI_ST1
		__FCMOVB_ST1
		_asm	fstp	[Res]
		_asm	fcomp
			return Res;
#else
    return (a > b)?a:b;
#endif
}

//! A global function to find MIN(a,b) using FCOMI/FCMOV
inline float __FCMin2(float a, float b)
{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
    
	float Res;
	_asm	fld		[a]
	_asm	fld		[b]
	__FCOMI_ST1
		__FCMOVNB_ST1
		_asm	fstp	[Res]
		_asm	fcomp
			return Res;
#else
    return (a < b)?a:b;
#endif
}

//! A global function to find MAX(a,b,c) using FCOMI/FCMOV
inline float __FCMax3(float a, float b, float c)
{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
	float Res;
	_asm	fld		[a]
	_asm	fld		[b]
	_asm	fld		[c]
	__FCOMI_ST1
		__FCMOVB_ST1
		__FCOMI_ST2
		__FCMOVB_ST2
		_asm	fstp	[Res]
		_asm	fcompp
			return Res;
#else
    float x = __FCMax2(a,b);
    return __FCMax2(x,c);
#endif
}

//! A global function to find MIN(a,b,c) using FCOMI/FCMOV
inline float __FCMin3(float a, float b, float c)
{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
	float Res;
	_asm	fld		[a]
	_asm	fld		[b]
	_asm	fld		[c]
	__FCOMI_ST1
		__FCMOVNB_ST1
		__FCOMI_ST2
		__FCMOVNB_ST2
		_asm	fstp	[Res]
		_asm	fcompp
			return Res;
#else
    float x = __FCMin2(a,b);
    return __FCMin2(x,c);
#endif
}

inline int __ConvertToSortable(float f)
{
	int& Fi = (int&)f;
	int Fmask = (Fi>>31);
	Fi ^= Fmask;
	Fmask &= ~(1<<31);
	Fi -= Fmask;
	return Fi;
}

#endif // __ICEFPU_H__

	/** @} */