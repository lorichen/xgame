/*****************************************************************************
File:  fastmath.cpp

This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/
#include "ReMathPch.h"
#include "Nvidia_fastmath.h"
#include "ReVector3.h"

void  build_sqrt_table()
{
  unsigned int i;
  FastSqrtUnion s;
  
  for (i = 0; i <= 0x7FFF; i++)
  {
    
    // Build a float with the bit pattern i as mantissa
    //  and an exponent of 0, stored as 127
    
    s.i = (i << 8) | (0x7F << 23);
    s.f = (float)sqrt(s.f);
    
    // Take the square root then strip the first 7 bits of
    //  the mantissa into the table
    
    fast_sqrt_table[i + 0x8000] = (s.i & 0x7FFFFF);
    
    // Repeat the process, this time with an exponent of 1, 
    //  stored as 128
    
    s.i = (i << 8) | (0x80 << 23);
    s.f = (float)sqrt(s.f);
    
    fast_sqrt_table[i] = (s.i & 0x7FFFFF);
  }
}

/*
int main(int argc, char* argv[])
{

  float t, it, test_sqrt;
  int i = 0;

  build_sqrt_table();

  t = 1234.121234f;
  
  DWORD t1 = GetTickCount();
  for(float fa = 0;fa < 2345.4323f;fa += 0.123f)
  for(float fb = 0;fb < 222.2f;fb += 0.0333f)
  {
	test_sqrt = fastsqrt(fa + fb);
  }
  DWORD t2 = GetTickCount();

  DWORD t3 = GetTickCount();
  for(float fa = 0;fa < 2345.4323f;fa += 0.123f)
  for(float fb = 0;fb < 222.2f;fb += 0.0333f)
  {
	test_sqrt = sqrt(fa + fb);
  }
  DWORD t4 = GetTickCount();


  printf("sqrt expected %20.10f  approx %20.10f\n", sqrt(t), test_sqrt);

  FP_INV(it,t);
  printf("inv  expected %20.10f  approx %20.10f\n", 1/t, it);


  i = 0xdeafbabe;

  FloatToInt(&i, t);
  printf("ftol expected %d  actual %d %08X\n", (int)t, i, i);

  return 0;
}*/


/////////////////////////////////////////////////////////////////////
//  3D and geometry ops /////////////////////////////////////////////



//-----------------------------------------------------------------------------
// Name: CylTest_CapsFirst
// Orig: Greg James - gjames@NVIDIA.com
// Lisc: Free code - no warranty & no money back.  Use it all you want
// Desc: 
//    This function tests if the 3D point 'testpt' lies within an arbitrarily
// oriented cylinder.  The cylinder is defined by an axis from 'pt1' to 'pt2',
// the axis having a length squared of 'lengthsq' (pre-compute for each cylinder
// to avoid repeated work!), and radius squared of 'radius_sq'.
//    The function tests against the end caps first, which is cheap -> only 
// a single dot product to test against the parallel cylinder caps.  If the
// point is within these, more work is done to find the distance of the point
// from the cylinder axis.
//    Fancy Math (TM) makes the whole test possible with only two dot-products
// a subtract, and two multiplies.  For clarity, the 2nd mult is kept as a
// divide.  It might be faster to change this to a mult by also passing in
// 1/lengthsq and using that instead.
//    Elminiate the first 3 subtracts by specifying the cylinder as a base
// point on one end cap and a vector to the other end cap (pass in {dx,dy,dz}
// instead of 'pt2' ).
//
//    The dot product is constant along a plane perpendicular to a vector.
//    The magnitude of the cross product divided by one vector length is
// constant along a cylinder surface defined by the other vector as axis.
//
// Return:  -1.0 if point is outside the cylinder
// Return:  distance squared from cylinder axis if point is inside.
//
//-----------------------------------------------------------------------------

float CylTest_CapsFirst( const xs::Vector3 & pt1, const xs::Vector3 & pt2, float lengthsq, float radius_sq, const xs::Vector3 & testpt )
{
	float dx, dy, dz;	// vector d  from line segment point 1 to point 2
	float pdx, pdy, pdz;	// vector pd from point 1 to test point
	float dot, dsq;

	dx = pt2.x - pt1.x;	// translate so pt1 is origin.  Make vector from
	dy = pt2.y - pt1.y;     // pt1 to pt2.  Need for this is easily eliminated
	dz = pt2.z - pt1.z;

	pdx = testpt.x - pt1.x;		// vector from pt1 to test point.
	pdy = testpt.y - pt1.y;
	pdz = testpt.z - pt1.z;

	// Dot the d and pd vectors to see if point lies behind the 
	// cylinder cap at pt1.x, pt1.y, pt1.z

	dot = pdx * dx + pdy * dy + pdz * dz;

	// If dot is less than zero the point is behind the pt1 cap.
	// If greater than the cylinder axis line segment length squared
	// then the point is outside the other end cap at pt2.

	if( dot < 0.0f || dot > lengthsq )
	{
		return( -1.0f );
	}
	else 
	{
		// Point lies within the parallel caps, so find
		// distance squared from point to line, using the fact that sin^2 + cos^2 = 1
		// the dot = cos() * |d||pd|, and cross*cross = sin^2 * |d|^2 * |pd|^2
		// Carefull: '*' means mult for scalars and dotproduct for vectors
		// In short, where dist is pt distance to cyl axis: 
		// dist = sin( pd to d ) * |pd|
		// distsq = dsq = (1 - cos^2( pd to d)) * |pd|^2
		// dsq = ( 1 - (pd * d)^2 / (|pd|^2 * |d|^2) ) * |pd|^2
		// dsq = pd * pd - dot * dot / lengthsq
		//  where lengthsq is d*d or |d|^2 that is passed into this function 

		// distance squared to the cylinder axis:

		dsq = (pdx*pdx + pdy*pdy + pdz*pdz) - dot*dot/lengthsq;

		if( dsq > radius_sq )
		{
			return( -1.0f );
		}
		else
		{
			return( dsq );		// return distance squared to axis
		}
	}
}