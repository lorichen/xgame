#ifndef __Matrix4__
#define __Matrix4__
#include "ReMathPrerequisite.h"

#include "ReVector3.h"
#include "ReMatrix3.h"
#include "ReVector4.h"
#include "RePlane.h"
#include "CPUMathInfo.h"
namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/
	
    /** Class encapsulating a standard 4x4 homogenous matrix.
        @remarks
            rkt uses column vectors when applying matrix multiplications,
            This means a vector is represented as a single column, 4-row
            matrix. This has the effect that the tranformations implemented
            by the matrices happens right-to-left e.g. if vector V is to be
            transformed by M1 then M2 then M3, the calculation would be
            M3 * M2 * M1 * V. The order that matrices are concatenated is
            vital since matrix multiplication is not cummatative, i.e. you
            can get a different result if you concatenate in the wrong order.
        @par
            The use of column vectors and right-to-left ordering is the
            standard in most mathematical texts, and id the same as used in
            OpenGL. It is, however, the opposite of Direct3D, which has
            inexplicably chosen to differ from the accepted standard and uses
            row vectors and left-to-right matrix multiplication.
        @par
            rkt deals with the differences between D3D and OpenGL etc.
            internally when operating through different render systems. rkt
            users only need to conform to standard maths conventions, i.e.
            right-to-left matrix multiplication, (rkt transposes matrices it
            passes to D3D to compensate).
        @par
            The generic form M * V which shows the layout of the matrix 
            entries is shown below:
            <pre>
                [ m[0][0]  m[0][1]  m[0][2]  m[0][3] ]   {x}
                | m[1][0]  m[1][1]  m[1][2]  m[1][3] | * {y}
                | m[2][0]  m[2][1]  m[2][2]  m[2][3] |   {z}
                [ m[3][0]  m[3][1]  m[3][2]  m[3][3] ]   {1}
            </pre>
    */
    class _ReMathExport Matrix4
    {
    //protected:
	public:
        /// The matrix entries, indexed by [row][col].
        union {
            float m[4][4];
            float _m[16];
        };
		static uint s_CPUAcceFlag;
    public:
		inline const void rotationY(const float fDegree)
		{
			float fAngle = Math::DegreesToRadians(fDegree);
			operator=(IDENTITY);
			m[0][0] =  Math::Cos(fAngle,true);
			m[0][2] =  Math::Sin(fAngle,true);
			m[2][0] = -Math::Sin(fAngle,true);
			m[2][2] =  Math::Cos(fAngle,true);
		}
		inline const void rotationX(const float fDegree)
		{
			float fAngle = Math::DegreesToRadians(fDegree);
			operator=(IDENTITY);
			m[1][1] =  Math::Cos(fAngle,true);
			m[1][2] =  Math::Sin(fAngle,true);
			m[2][1] = -Math::Sin(fAngle,true);
			m[2][2] =  Math::Cos(fAngle,true);
		}
		inline const void rotationZ(const float fDegree)
		{
			float fAngle = Math::DegreesToRadians(fDegree);
			operator=(IDENTITY);
			m[0][0] =  Math::Cos(fAngle,true);
			m[0][1] =  Math::Sin(fAngle,true);
			m[1][0] = -Math::Sin(fAngle,true);
			m[1][1] =  Math::Cos(fAngle,true);
		}

		//add by yhc 正玄余玄值精确一些不通过查表等到,
		inline const void PrecisionRotationY(const float fDegree)
		{
			float fAngle = Math::DegreesToRadians(fDegree);
			operator=(IDENTITY);
			m[0][0] =  Math::Cos(fAngle,false);
			m[0][2] =  Math::Sin(fAngle,false);
			m[2][0] = -Math::Sin(fAngle,false);
			m[2][2] =  Math::Cos(fAngle,false);
		}
		inline const void PrecisionRotationX(const float fDegree)
		{
			float fAngle = Math::DegreesToRadians(fDegree);
			operator=(IDENTITY);
			m[1][1] =  Math::Cos(fAngle,false);
			m[1][2] =  Math::Sin(fAngle,false);
			m[2][1] = -Math::Sin(fAngle,false);
			m[2][2] =  Math::Cos(fAngle,false);
		}
		inline const void PrecisionRotationZ(const float fDegree)
		{
			float fAngle = Math::DegreesToRadians(fDegree);
			operator=(IDENTITY);
			m[0][0] =  Math::Cos(fAngle,false);
			m[0][1] =  Math::Sin(fAngle,false);
			m[1][0] = -Math::Sin(fAngle,false);
			m[1][1] =  Math::Cos(fAngle,false);
		}
		//end

        /** Default constructor.
            @note
                It does <b>NOT</b> initialize the matrix for efficiency.
        */
        inline Matrix4()
        {
        }

        inline Matrix4(
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33 )
        {
            m[0][0] = m00;
            m[0][1] = m01;
            m[0][2] = m02;
            m[0][3] = m03;
            m[1][0] = m10;
            m[1][1] = m11;
            m[1][2] = m12;
            m[1][3] = m13;
            m[2][0] = m20;
            m[2][1] = m21;
            m[2][2] = m22;
            m[2][3] = m23;
            m[3][0] = m30;
            m[3][1] = m31;
            m[3][2] = m32;
            m[3][3] = m33;
        }

        /** Creates a standard 4x4 transformation matrix with a zero translation part from a rotation/scaling 3x3 matrix.
         */

        inline Matrix4(const Matrix3& m3x3)
        {
          operator=(m3x3);

		  m[0][3] = 0;
		  m[1][3] = 0;
		  m[2][3] = 0;
		  m[3][3] = 1;
		  m[3][0] = 0;
		  m[3][1] = 0;
		  m[3][2] = 0;
        }

        /** Creates a standard 4x4 transformation matrix with a zero translation part from a rotation/scaling Quaternion.
         */
        
        inline Matrix4(const Quaternion& rot)
        {
			Matrix3 m3x3;
			rot.ToRotationMatrix(m3x3);
			operator=(m3x3);

			m[0][3] = 0;
			m[1][3] = 0;
			m[2][3] = 0;
			m[3][3] = 1;
			m[3][0] = 0;
			m[3][1] = 0;
			m[3][2] = 0;
        }
        

        inline float* operator [] ( size_t iRow )
        {
            assert( iRow < 4 );
            return m[iRow];
        }

        inline const float *const operator [] ( size_t iRow ) const
        {
            assert( iRow < 4 );
            return m[iRow];
        }

        inline Matrix4 concatenate(const Matrix4 &m2) const
        {
            Matrix4 r;
#if (TARGET_PLATFORM == PLATFORM_WIN32)
            
			if(s_CPUAcceFlag == -1 )//初始化标志
			{
				s_CPUAcceFlag = CPUMathInfo::getCapability();
			}

			if( s_CPUAcceFlag & CPUMC_SSE )//支持SSE
			{
				const float *pa = &_m[0];
				const float *pb = &m2._m[0];
				float *pret = &r._m[0];
				__asm
				{
					mov		eax,[pb]
					mov		ecx,[pa]

					movups	xmm4,[eax]				// Other.M[0][0-3]
					movups	xmm5,[eax+16]			// Other.M[1][0-3]
					movups	xmm6,[eax+32]			// Other.M[2][0-3]
					movups	xmm7,[eax+48]			// Other.M[3][0-3]

					mov		eax,[pret]

					// Begin first row of result.
					movss	xmm0,[ecx]				// M[0][0] 
					shufps	xmm0,xmm0,0
						mulps	xmm0,xmm4

						movss	xmm1,[ecx+4]			// M[0][1]
					shufps	xmm1,xmm1,0
						mulps	xmm1,xmm5

						movss	xmm2,[ecx+8]			// M[0][2]
					shufps	xmm2,xmm2,0
						mulps	xmm2,xmm6

						addps	xmm1,xmm0				// First row done with xmm0

						movss	xmm3,[ecx+12]			// M[0][3]
					shufps	xmm3,xmm3,0
						mulps	xmm3,xmm7

						// Begin second row of result.
						movss	xmm0,[ecx+16]			// M[1][0] 
					shufps	xmm0,xmm0,0
						mulps	xmm0,xmm4

						addps	xmm3,xmm2				// First row done with xmm2

						movss	xmm2,[ecx+20]			// M[1][1]
					shufps	xmm2,xmm2,0
						mulps	xmm2,xmm5

						addps	xmm3,xmm1				// First row done with xmm1

						movss	xmm1,[ecx+24]			// M[1][2]
					shufps	xmm1,xmm1,0
						mulps	xmm1,xmm6

						movups	[eax],xmm3				// Store Result.M[0][0-3]
						// Done computing first row.

						addps	xmm2,xmm0				// Second row done with xmm0

						movss	xmm3,[ecx+28]			// M[1][3]
					shufps	xmm3,xmm3,0
						mulps	xmm3,xmm7

						// Begin third row of result.
						movss	xmm0,[ecx+32]			// M[2][0] 
					shufps	xmm0,xmm0,0
						mulps	xmm0,xmm4

						addps	xmm3,xmm1				// Second row done with xmm1

						movss	xmm1,[ecx+36]			// M[2][1]
					shufps	xmm1,xmm1,0
						mulps	xmm1,xmm5

						addps	xmm3,xmm2				// Second row done with xmm2

						movss	xmm2,[ecx+40]			// M[2][2]
					shufps	xmm2,xmm2,0
						mulps	xmm2,xmm6

						movups	[eax+16],xmm3			// Store Result.M[1][0-3]
						// Done computing second row.

						addps	xmm1,xmm0				// Third row done with xmm0

						movss	xmm3,[ecx+44]			// M[2][3]
					shufps	xmm3,xmm3,0
						mulps	xmm3,xmm7

						// Begin fourth row of result.
						movss	xmm0,[ecx+48]			// M[3][0]
					shufps	xmm0,xmm0,0
						mulps	xmm0,xmm4

						addps	xmm3,xmm2				// Third row done with xmm2

						movss	xmm2,[ecx+52]			// M[3][1]
					shufps	xmm2,xmm2,0
						mulps	xmm2,xmm5

						addps	xmm3,xmm1				// Third row done with xmm1

						movss	xmm1,[ecx+56]			// M[3][2]
					shufps	xmm1,xmm1,0
						mulps	xmm1,xmm6

						movups	[eax+32],xmm3			// Store Result.M[2][0-3]
						// Done computing third row.

						addps	xmm2,xmm0

						movss	xmm3,[ecx+60]			// M[3][3]
					shufps	xmm3,xmm3,0
						mulps	xmm3,xmm7

						// stall

						addps	xmm3,xmm1

						// stall

						addps	xmm3,xmm2

						movups	[eax+48],xmm3			// Store Result.M[3][0-3]
				} 
			}
			else if( s_CPUAcceFlag & CPUMC_3DNOW)//支持3DNOW
			{
				const float *pa = &_m[0];
				const float *pb = &m2._m[0];
				float *pret = &r._m[0];
				__asm
				{
					femms

					mov         ecx, pa
					mov         edx, pb
					mov         eax, pret

					prefetch    [ecx+32]       ;// These may help -
					prefetch    [edx+32]       ;//    and probably don't hurt

					movq        mm0,[ecx]      ;// a21   | a11
					movq        mm1,[ecx+8]      ;// a41   | a31
					movq        mm4,[edx]      ;// b21   | b11
					punpckhdq   mm2,mm0         ;// a21   |
					movq        mm5,[edx+16]   ;// b22   | b12
					punpckhdq   mm3,mm1         ;// a41   |
					movq        mm6,[edx+32]   ;// b23   | b13
					punpckldq   mm0,mm0         ;// a11   | a11
					punpckldq   mm1,mm1         ;// a31   | a31
					pfmul       mm4,mm0         ;// a11*b21 | a11*b11
					punpckhdq   mm2,mm2         ;// a21   | a21
					pfmul       mm0,[edx+8]      ;// a11*b41 | a11*b31
					movq        mm7,[edx+48]   ;// b24   | b14
					pfmul       mm5,mm2         ;// a21*b22 | a21*b12
					punpckhdq   mm3,mm3         ;// a41   | a41
					pfmul       mm2,[edx+24]   ;// a21*b42 | a21*b32
					pfmul       mm6,mm1         ;// a31*b23 | a31*b13
					pfadd       mm5,mm4         ;// a21*b22 + a11*b21 | a21*b12 + a11*b11
					pfmul       mm1,[edx+40]   ;// a31*b43 | a31*b33
					pfadd       mm2,mm0         ;// a21*b42 + a11*b41 | a21*b32 + a11*b31
					pfmul       mm7,mm3         ;// a41*b24 | a41*b14
					pfadd       mm6,mm5         ;// a21*b22 + a11*b21 + a31*b23 | a21*b12 + a11*b11 + a31*b13
					pfmul       mm3,[edx+56]   ;// a41*b44 | a41*b34
					pfadd       mm2,mm1         ;// a21*b42 + a11*b41 + a31*b43 | a21*b32 + a11*b31 + a31*b33
					pfadd       mm7,mm6         ;// a41*b24 + a21*b22 + a11*b21 + a31*b23 |  a41*b14 + a21*b12 + a11*b11 + a31*b13
					movq        mm0,[ecx+16]   ;// a22   | a12
					pfadd       mm3,mm2         ;// a41*b44 + a21*b42 + a11*b41 + a31*b43 | a41*b34 + a21*b32 + a11*b31 + a31*b33
					movq        mm1,[ecx+24]   ;// a42   | a32
					movq        [eax],mm7      ;// r21   | r11
					movq        mm4,[edx]      ;// b21   | b11
					movq        [eax+8],mm3      ;// r41   | r31

					punpckhdq   mm2,mm0         ;// a22   | XXX
					movq        mm5,[edx+16]   ;// b22   | b12
					punpckhdq   mm3,mm1         ;// a42   | XXX
					movq        mm6,[edx+32]   ;// b23   | b13
					punpckldq   mm0,mm0         ;// a12   | a12
					punpckldq   mm1,mm1         ;// a32   | a32
					pfmul       mm4,mm0         ;// a12*b21 | a12*b11
					punpckhdq   mm2,mm2         ;// a22   | a22
					pfmul       mm0,[edx+8]      ;// a12*b41 | a12*b31
					movq        mm7,[edx+48]   ;// b24   | b14
					pfmul       mm5,mm2         ;// a22*b22 | a22*b12
					punpckhdq   mm3,mm3         ;// a42   | a42
					pfmul       mm2,[edx+24]   ;// a22*b42 | a22*b32
					pfmul       mm6,mm1         ;// a32*b23 | a32*b13
					pfadd       mm5,mm4         ;// a12*b21 + a22*b22 | a12*b11 + a22*b12
					pfmul       mm1,[edx+40]   ;// a32*b43 | a32*b33
					pfadd       mm2,mm0         ;// a12*b41 + a22*b42 | a12*b11 + a22*b32
					pfmul       mm7,mm3         ;// a42*b24 | a42*b14
					pfadd       mm6,mm5         ;// a32*b23 + a12*b21 + a22*b22 | a32*b13 + a12*b11 + a22*b12
					pfmul       mm3,[edx+56]   ;// a42*b44 | a42*b34
					pfadd       mm2,mm1         ;// a32*b43 + a12*b41 + a22*b42 | a32*b33 + a12*b11 + a22*b32
					pfadd       mm7,mm6         ;// a42*b24 + a32*b23 + a12*b21 + a22*b22 | a42*b14 + a32*b13 + a12*b11 + a22*b12
					movq        mm0,[ecx+32]   ;// a23 | a13
					pfadd       mm3,mm2         ;// a42*b44 + a32*b43 + a12*b41 + a22*b42 | a42*b34 + a32*b33 + a12*b11 + a22*b32
					movq        mm1,[ecx+40]   ;// a43 | a33
					movq        [eax+16],mm7   ;// r22 | r12
					movq        mm4,[edx]      ;// b21   | b11
					movq        [eax+24],mm3   ;// r42 | r32

					punpckhdq   mm2,mm0         ;// a23 | XXX
					movq        mm5,[edx+16]   ;// b22 | b12
					punpckhdq   mm3,mm1         ;// a43 | XXX
					movq        mm6,[edx+32]   ;// b23 | b13
					punpckldq   mm0,mm0         ;// a13 | a13
					punpckldq   mm1,mm1         ;// a33 | a33
					pfmul       mm4,mm0         ;// a13*b21 | a13*b11
					punpckhdq   mm2,mm2         ;// a23 | a23
					pfmul       mm0,[edx+8]      ;// a13*b41 | a13*b31
					movq        mm7,[edx+48]   ;// b24 | b14
					pfmul       mm5,mm2         ;// a23*b22 | a23*b12
					punpckhdq   mm3,mm3         ;// a43 | a43
					pfmul       mm2,[edx+24]   ;// a23*b42 | a23*b32
					pfmul       mm6,mm1         ;// a33*b23 | a33*b13
					pfadd       mm5,mm4         ;// a23*b22 + a13*b21 | a23*b12 + a13*b11
					pfmul       mm1,[edx+40]   ;// a33*b43 | a33*b33
					pfadd       mm2,mm0         ;// a13*b41 + a23*b42 | a13*b31 + a23*b32
					pfmul       mm7,mm3         ;// a43*b24 | a43*b14
					pfadd       mm6,mm5         ;// a33*b23 + a23*b22 + a13*b21 | a33*b13 + a23*b12 + a13*b11
					pfmul       mm3,[edx+56]   ;// a43*b44 | a43*b34
					pfadd       mm2,mm1         ;// a33*b43*a13*b41 + a23*b42 | a33*b33 + a13*b31 + a23*b32
					pfadd       mm7,mm6         ;// a43*b24 + a33*b23 + a23*b22 + a13*b21 | a43*b14 + a33*b13 + a23*b12 + a13*b11
					movq        mm0,[ecx+48]   ;// a24 | a14
					pfadd       mm3,mm2         ;// a43*b44 + a33*b43*a13*b41 + a23*b42 | a43*b34 + a33*b33 + a13*b31 + a23*b32
					movq        mm1,[ecx+56]   ;// a44 | a34
					movq        [eax+32],mm7   ;// r23 | r13
					movq        mm4,[edx]      ;// b21 | b11
					movq        [eax+40],mm3   ;// r43 | r33

					punpckhdq   mm2,mm0         ;// a24 | XXX
					movq        mm5,[edx+16]   ;// b22 | b12
					punpckhdq   mm3,mm1         ;// a44 | XXX
					movq        mm6,[edx+32]   ;// b23 | b13
					punpckldq   mm0,mm0         ;// a14 | a14
					punpckldq   mm1,mm1         ;// a34 | a34
					pfmul       mm4,mm0         ;// a14*b21 | a14*b11
					punpckhdq   mm2,mm2         ;// a24 | a24
					pfmul       mm0,[edx+8]      ;// a14*b41 | a14*b31
					movq        mm7,[edx+48]   ;// b24 | b14
					pfmul       mm5,mm2         ;// a24*b22 | a24*b12
					punpckhdq   mm3,mm3         ;// a44 | a44
					pfmul       mm2,[edx+24]   ;// a24*b 42 | a24*b32
					pfmul       mm6,mm1         ;// a34*b23 | a34*b13
					pfadd       mm5,mm4         ;// a14*b21 + a24*b22 | a14*b11 + a24*b12
					pfmul       mm1,[edx+40]   ;// a34*b43 | a34*b33
					pfadd       mm2,mm0         ;// a14*b41 + a24*b 42 | a14*b31 + a24*b32
					pfmul       mm7,mm3         ;// a44*b24 | a44*b14
					pfadd       mm6,mm5         ;// a34*b23 + a14*b21 + a24*b22 | a34*b13 + a14*b11 + a24*b12
					pfmul       mm3,[edx+56]   ;// a44*b44 | a44*b34
					pfadd       mm2,mm1         ;// a34*b43 + a14*b41 + a24*b 42 | a34*b33 + a14*b31 + a24*b32
					pfadd       mm7,mm6         ;// a44*b24 + a14*b23 + a24*b 42 | a44*b14 + a14*b31 + a24*b32
					pfadd       mm3,mm2         ;// a44*b44 + a34*b43 + a14*b41 + a24*b42 | a44*b34 + a34*b33 + a14*b31 + a24*b32
					movq        [eax+48],mm7   ;// r24 | r14
					movq        [eax+56],mm3   ;// r44 | r34
					femms
				}

			}
			else
			{
				r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
				r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
				r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
				r.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

				r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
				r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
				r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
				r.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

				r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
				r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
				r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
				r.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

				r.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
				r.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
				r.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
				r.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];
			}
#else
            assert(0);
#endif
            return r;
    
        }

        /** Matrix concatenation using '*'.
        */
        inline Matrix4 operator * ( const Matrix4 &m2 ) const
        {

            //return concatenate( m2 );


			Matrix4 r;

    #if (TARGET_PLATFORM == PLATFORM_WIN32)
			if(s_CPUAcceFlag == -1 )//初始化标志
			{
				s_CPUAcceFlag = CPUMathInfo::getCapability();
			}

			if( s_CPUAcceFlag & CPUMC_SSE )//支持SSE
			{
				const float *pa = &_m[0];
				const float *pb = &m2._m[0];
				float *pret = &r._m[0];
				__asm
				{
					mov		eax,[pb]
					mov		ecx,[pa]

					movups	xmm4,[eax]				// Other.M[0][0-3]
					movups	xmm5,[eax+16]			// Other.M[1][0-3]
					movups	xmm6,[eax+32]			// Other.M[2][0-3]
					movups	xmm7,[eax+48]			// Other.M[3][0-3]

					mov		eax,[pret]

					// Begin first row of result.
					movss	xmm0,[ecx]				// M[0][0] 
					shufps	xmm0,xmm0,0
						mulps	xmm0,xmm4

						movss	xmm1,[ecx+4]			// M[0][1]
					shufps	xmm1,xmm1,0
						mulps	xmm1,xmm5

						movss	xmm2,[ecx+8]			// M[0][2]
					shufps	xmm2,xmm2,0
						mulps	xmm2,xmm6

						addps	xmm1,xmm0				// First row done with xmm0

						movss	xmm3,[ecx+12]			// M[0][3]
					shufps	xmm3,xmm3,0
						mulps	xmm3,xmm7

						// Begin second row of result.
						movss	xmm0,[ecx+16]			// M[1][0] 
					shufps	xmm0,xmm0,0
						mulps	xmm0,xmm4

						addps	xmm3,xmm2				// First row done with xmm2

						movss	xmm2,[ecx+20]			// M[1][1]
					shufps	xmm2,xmm2,0
						mulps	xmm2,xmm5

						addps	xmm3,xmm1				// First row done with xmm1

						movss	xmm1,[ecx+24]			// M[1][2]
					shufps	xmm1,xmm1,0
						mulps	xmm1,xmm6

						movups	[eax],xmm3				// Store Result.M[0][0-3]
						// Done computing first row.

						addps	xmm2,xmm0				// Second row done with xmm0

						movss	xmm3,[ecx+28]			// M[1][3]
					shufps	xmm3,xmm3,0
						mulps	xmm3,xmm7

						// Begin third row of result.
						movss	xmm0,[ecx+32]			// M[2][0] 
					shufps	xmm0,xmm0,0
						mulps	xmm0,xmm4

						addps	xmm3,xmm1				// Second row done with xmm1

						movss	xmm1,[ecx+36]			// M[2][1]
					shufps	xmm1,xmm1,0
						mulps	xmm1,xmm5

						addps	xmm3,xmm2				// Second row done with xmm2

						movss	xmm2,[ecx+40]			// M[2][2]
					shufps	xmm2,xmm2,0
						mulps	xmm2,xmm6

						movups	[eax+16],xmm3			// Store Result.M[1][0-3]
						// Done computing second row.

						addps	xmm1,xmm0				// Third row done with xmm0

						movss	xmm3,[ecx+44]			// M[2][3]
					shufps	xmm3,xmm3,0
						mulps	xmm3,xmm7

						// Begin fourth row of result.
						movss	xmm0,[ecx+48]			// M[3][0]
					shufps	xmm0,xmm0,0
						mulps	xmm0,xmm4

						addps	xmm3,xmm2				// Third row done with xmm2

						movss	xmm2,[ecx+52]			// M[3][1]
					shufps	xmm2,xmm2,0
						mulps	xmm2,xmm5

						addps	xmm3,xmm1				// Third row done with xmm1

						movss	xmm1,[ecx+56]			// M[3][2]
					shufps	xmm1,xmm1,0
						mulps	xmm1,xmm6

						movups	[eax+32],xmm3			// Store Result.M[2][0-3]
						// Done computing third row.

						addps	xmm2,xmm0

						movss	xmm3,[ecx+60]			// M[3][3]
					shufps	xmm3,xmm3,0
						mulps	xmm3,xmm7

						// stall

						addps	xmm3,xmm1

						// stall

						addps	xmm3,xmm2

						movups	[eax+48],xmm3			// Store Result.M[3][0-3]
				} 
			}
			else if( s_CPUAcceFlag & CPUMC_3DNOW)//支持3DNOW
			{
				const float *pa = &_m[0];
				const float *pb = &m2._m[0];
				float *pret = &r._m[0];
				__asm
				{
					femms

						mov         ecx, pa
						mov         edx, pb
						mov         eax, pret

						prefetch    [ecx+32]       ;// These may help -
					prefetch    [edx+32]       ;//    and probably don't hurt

					movq        mm0,[ecx]      ;// a21   | a11
					movq        mm1,[ecx+8]      ;// a41   | a31
					movq        mm4,[edx]      ;// b21   | b11
					punpckhdq   mm2,mm0         ;// a21   |
					movq        mm5,[edx+16]   ;// b22   | b12
					punpckhdq   mm3,mm1         ;// a41   |
					movq        mm6,[edx+32]   ;// b23   | b13
					punpckldq   mm0,mm0         ;// a11   | a11
					punpckldq   mm1,mm1         ;// a31   | a31
					pfmul       mm4,mm0         ;// a11*b21 | a11*b11
					punpckhdq   mm2,mm2         ;// a21   | a21
					pfmul       mm0,[edx+8]      ;// a11*b41 | a11*b31
					movq        mm7,[edx+48]   ;// b24   | b14
					pfmul       mm5,mm2         ;// a21*b22 | a21*b12
					punpckhdq   mm3,mm3         ;// a41   | a41
					pfmul       mm2,[edx+24]   ;// a21*b42 | a21*b32
					pfmul       mm6,mm1         ;// a31*b23 | a31*b13
					pfadd       mm5,mm4         ;// a21*b22 + a11*b21 | a21*b12 + a11*b11
					pfmul       mm1,[edx+40]   ;// a31*b43 | a31*b33
					pfadd       mm2,mm0         ;// a21*b42 + a11*b41 | a21*b32 + a11*b31
					pfmul       mm7,mm3         ;// a41*b24 | a41*b14
					pfadd       mm6,mm5         ;// a21*b22 + a11*b21 + a31*b23 | a21*b12 + a11*b11 + a31*b13
					pfmul       mm3,[edx+56]   ;// a41*b44 | a41*b34
					pfadd       mm2,mm1         ;// a21*b42 + a11*b41 + a31*b43 | a21*b32 + a11*b31 + a31*b33
					pfadd       mm7,mm6         ;// a41*b24 + a21*b22 + a11*b21 + a31*b23 |  a41*b14 + a21*b12 + a11*b11 + a31*b13
					movq        mm0,[ecx+16]   ;// a22   | a12
					pfadd       mm3,mm2         ;// a41*b44 + a21*b42 + a11*b41 + a31*b43 | a41*b34 + a21*b32 + a11*b31 + a31*b33
					movq        mm1,[ecx+24]   ;// a42   | a32
					movq        [eax],mm7      ;// r21   | r11
					movq        mm4,[edx]      ;// b21   | b11
					movq        [eax+8],mm3      ;// r41   | r31

					punpckhdq   mm2,mm0         ;// a22   | XXX
					movq        mm5,[edx+16]   ;// b22   | b12
					punpckhdq   mm3,mm1         ;// a42   | XXX
					movq        mm6,[edx+32]   ;// b23   | b13
					punpckldq   mm0,mm0         ;// a12   | a12
					punpckldq   mm1,mm1         ;// a32   | a32
					pfmul       mm4,mm0         ;// a12*b21 | a12*b11
					punpckhdq   mm2,mm2         ;// a22   | a22
					pfmul       mm0,[edx+8]      ;// a12*b41 | a12*b31
					movq        mm7,[edx+48]   ;// b24   | b14
					pfmul       mm5,mm2         ;// a22*b22 | a22*b12
					punpckhdq   mm3,mm3         ;// a42   | a42
					pfmul       mm2,[edx+24]   ;// a22*b42 | a22*b32
					pfmul       mm6,mm1         ;// a32*b23 | a32*b13
					pfadd       mm5,mm4         ;// a12*b21 + a22*b22 | a12*b11 + a22*b12
					pfmul       mm1,[edx+40]   ;// a32*b43 | a32*b33
					pfadd       mm2,mm0         ;// a12*b41 + a22*b42 | a12*b11 + a22*b32
					pfmul       mm7,mm3         ;// a42*b24 | a42*b14
					pfadd       mm6,mm5         ;// a32*b23 + a12*b21 + a22*b22 | a32*b13 + a12*b11 + a22*b12
					pfmul       mm3,[edx+56]   ;// a42*b44 | a42*b34
					pfadd       mm2,mm1         ;// a32*b43 + a12*b41 + a22*b42 | a32*b33 + a12*b11 + a22*b32
					pfadd       mm7,mm6         ;// a42*b24 + a32*b23 + a12*b21 + a22*b22 | a42*b14 + a32*b13 + a12*b11 + a22*b12
					movq        mm0,[ecx+32]   ;// a23 | a13
					pfadd       mm3,mm2         ;// a42*b44 + a32*b43 + a12*b41 + a22*b42 | a42*b34 + a32*b33 + a12*b11 + a22*b32
					movq        mm1,[ecx+40]   ;// a43 | a33
					movq        [eax+16],mm7   ;// r22 | r12
					movq        mm4,[edx]      ;// b21   | b11
					movq        [eax+24],mm3   ;// r42 | r32

					punpckhdq   mm2,mm0         ;// a23 | XXX
					movq        mm5,[edx+16]   ;// b22 | b12
					punpckhdq   mm3,mm1         ;// a43 | XXX
					movq        mm6,[edx+32]   ;// b23 | b13
					punpckldq   mm0,mm0         ;// a13 | a13
					punpckldq   mm1,mm1         ;// a33 | a33
					pfmul       mm4,mm0         ;// a13*b21 | a13*b11
					punpckhdq   mm2,mm2         ;// a23 | a23
					pfmul       mm0,[edx+8]      ;// a13*b41 | a13*b31
					movq        mm7,[edx+48]   ;// b24 | b14
					pfmul       mm5,mm2         ;// a23*b22 | a23*b12
					punpckhdq   mm3,mm3         ;// a43 | a43
					pfmul       mm2,[edx+24]   ;// a23*b42 | a23*b32
					pfmul       mm6,mm1         ;// a33*b23 | a33*b13
					pfadd       mm5,mm4         ;// a23*b22 + a13*b21 | a23*b12 + a13*b11
					pfmul       mm1,[edx+40]   ;// a33*b43 | a33*b33
					pfadd       mm2,mm0         ;// a13*b41 + a23*b42 | a13*b31 + a23*b32
					pfmul       mm7,mm3         ;// a43*b24 | a43*b14
					pfadd       mm6,mm5         ;// a33*b23 + a23*b22 + a13*b21 | a33*b13 + a23*b12 + a13*b11
					pfmul       mm3,[edx+56]   ;// a43*b44 | a43*b34
					pfadd       mm2,mm1         ;// a33*b43*a13*b41 + a23*b42 | a33*b33 + a13*b31 + a23*b32
					pfadd       mm7,mm6         ;// a43*b24 + a33*b23 + a23*b22 + a13*b21 | a43*b14 + a33*b13 + a23*b12 + a13*b11
					movq        mm0,[ecx+48]   ;// a24 | a14
					pfadd       mm3,mm2         ;// a43*b44 + a33*b43*a13*b41 + a23*b42 | a43*b34 + a33*b33 + a13*b31 + a23*b32
					movq        mm1,[ecx+56]   ;// a44 | a34
					movq        [eax+32],mm7   ;// r23 | r13
					movq        mm4,[edx]      ;// b21 | b11
					movq        [eax+40],mm3   ;// r43 | r33

					punpckhdq   mm2,mm0         ;// a24 | XXX
					movq        mm5,[edx+16]   ;// b22 | b12
					punpckhdq   mm3,mm1         ;// a44 | XXX
					movq        mm6,[edx+32]   ;// b23 | b13
					punpckldq   mm0,mm0         ;// a14 | a14
					punpckldq   mm1,mm1         ;// a34 | a34
					pfmul       mm4,mm0         ;// a14*b21 | a14*b11
					punpckhdq   mm2,mm2         ;// a24 | a24
					pfmul       mm0,[edx+8]      ;// a14*b41 | a14*b31
					movq        mm7,[edx+48]   ;// b24 | b14
					pfmul       mm5,mm2         ;// a24*b22 | a24*b12
					punpckhdq   mm3,mm3         ;// a44 | a44
					pfmul       mm2,[edx+24]   ;// a24*b 42 | a24*b32
					pfmul       mm6,mm1         ;// a34*b23 | a34*b13
					pfadd       mm5,mm4         ;// a14*b21 + a24*b22 | a14*b11 + a24*b12
					pfmul       mm1,[edx+40]   ;// a34*b43 | a34*b33
					pfadd       mm2,mm0         ;// a14*b41 + a24*b 42 | a14*b31 + a24*b32
					pfmul       mm7,mm3         ;// a44*b24 | a44*b14
					pfadd       mm6,mm5         ;// a34*b23 + a14*b21 + a24*b22 | a34*b13 + a14*b11 + a24*b12
					pfmul       mm3,[edx+56]   ;// a44*b44 | a44*b34
					pfadd       mm2,mm1         ;// a34*b43 + a14*b41 + a24*b 42 | a34*b33 + a14*b31 + a24*b32
					pfadd       mm7,mm6         ;// a44*b24 + a14*b23 + a24*b 42 | a44*b14 + a14*b31 + a24*b32
					pfadd       mm3,mm2         ;// a44*b44 + a34*b43 + a14*b41 + a24*b42 | a44*b34 + a34*b33 + a14*b31 + a24*b32
					movq        [eax+48],mm7   ;// r24 | r14
					movq        [eax+56],mm3   ;// r44 | r34
					femms
				}

			}
			else
#endif
			{
				r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
				r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
				r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
				r.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

				r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
				r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
				r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
				r.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

				r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
				r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
				r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
				r.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

				r.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
				r.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
				r.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
				r.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];
			}

			return r;


        }

        /** Vector transformation using '*'.
            @remarks
                Transforms the given 3-D vector by the matrix, projecting the 
                result back into <i>w</i> = 1.
            @note
                This means that the initial <i>w</i> is considered to be 1.0,
                and then all the tree elements of the resulting 3-D vector are
                divided by the resulting <i>w</i>.
        */
        inline Vector3 operator * ( const Vector3 &v ) const
        {
            Vector3 r;

            float fInvW = 1.0 / ( m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] );

            r.x = ( m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] ) * fInvW;
            r.y = ( m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] ) * fInvW;
            r.z = ( m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] ) * fInvW;

            return r;
        }
        inline Vector4 operator * (const Vector4& v) const
        {
            return Vector4(
                m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w, 
                m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
                m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
                m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
                );
        }
        inline Plane operator * (const Plane& p) const
        {
            Plane ret;
			Matrix4 invTrans = inverse().transpose();
            ret.normal.x =
                invTrans[0][0] * p.normal.x + invTrans[0][1] * p.normal.y + invTrans[0][2] * p.normal.z;
            ret.normal.y = 
                invTrans[1][0] * p.normal.x + invTrans[1][1] * p.normal.y + invTrans[1][2] * p.normal.z;
            ret.normal.z = 
                invTrans[2][0] * p.normal.x + invTrans[2][1] * p.normal.y + invTrans[2][2] * p.normal.z;
            Vector3 pt = p.normal * -p.d;
            pt = *this * pt;
            ret.d = - pt.dotProduct(ret.normal);
            return ret;
        }


        /** Matrix addition.
        */
        inline Matrix4 operator + ( const Matrix4 &m2 ) const
        {
            Matrix4 r;

            r.m[0][0] = m[0][0] + m2.m[0][0];
            r.m[0][1] = m[0][1] + m2.m[0][1];
            r.m[0][2] = m[0][2] + m2.m[0][2];
            r.m[0][3] = m[0][3] + m2.m[0][3];

            r.m[1][0] = m[1][0] + m2.m[1][0];
            r.m[1][1] = m[1][1] + m2.m[1][1];
            r.m[1][2] = m[1][2] + m2.m[1][2];
            r.m[1][3] = m[1][3] + m2.m[1][3];

            r.m[2][0] = m[2][0] + m2.m[2][0];
            r.m[2][1] = m[2][1] + m2.m[2][1];
            r.m[2][2] = m[2][2] + m2.m[2][2];
            r.m[2][3] = m[2][3] + m2.m[2][3];

            r.m[3][0] = m[3][0] + m2.m[3][0];
            r.m[3][1] = m[3][1] + m2.m[3][1];
            r.m[3][2] = m[3][2] + m2.m[3][2];
            r.m[3][3] = m[3][3] + m2.m[3][3];

            return r;
        }

        /** Matrix subtraction.
        */
        inline Matrix4 operator - ( const Matrix4 &m2 ) const
        {
            Matrix4 r;
            r.m[0][0] = m[0][0] - m2.m[0][0];
            r.m[0][1] = m[0][1] - m2.m[0][1];
            r.m[0][2] = m[0][2] - m2.m[0][2];
            r.m[0][3] = m[0][3] - m2.m[0][3];

            r.m[1][0] = m[1][0] - m2.m[1][0];
            r.m[1][1] = m[1][1] - m2.m[1][1];
            r.m[1][2] = m[1][2] - m2.m[1][2];
            r.m[1][3] = m[1][3] - m2.m[1][3];

            r.m[2][0] = m[2][0] - m2.m[2][0];
            r.m[2][1] = m[2][1] - m2.m[2][1];
            r.m[2][2] = m[2][2] - m2.m[2][2];
            r.m[2][3] = m[2][3] - m2.m[2][3];

            r.m[3][0] = m[3][0] - m2.m[3][0];
            r.m[3][1] = m[3][1] - m2.m[3][1];
            r.m[3][2] = m[3][2] - m2.m[3][2];
            r.m[3][3] = m[3][3] - m2.m[3][3];

            return r;
        }

        /** Tests 2 matrices for equality.
        */
        inline bool operator == ( const Matrix4& m2 ) const
        {
            if( 
                m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
                m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
                m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
                m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3] )
                return false;
            return true;
        }

        /** Tests 2 matrices for inequality.
        */
        inline bool operator != ( const Matrix4& m2 ) const
        {
            if( 
                m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
                m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
                m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
                m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3] )
                return true;
            return false;
        }

        /** Assignment from 3x3 matrix.
        */
        inline void operator = ( const Matrix3& mat3 )
        {
            m[0][0] = mat3.m[0][0]; m[0][1] = mat3.m[0][1]; m[0][2] = mat3.m[0][2];
            m[1][0] = mat3.m[1][0]; m[1][1] = mat3.m[1][1]; m[1][2] = mat3.m[1][2];
            m[2][0] = mat3.m[2][0]; m[2][1] = mat3.m[2][1]; m[2][2] = mat3.m[2][2];
        }

        inline Matrix4 transpose(void) const
        {
            return Matrix4(m[0][0], m[1][0], m[2][0], m[3][0],
                           m[0][1], m[1][1], m[2][1], m[3][1],
                           m[0][2], m[1][2], m[2][2], m[3][2],
                           m[0][3], m[1][3], m[2][3], m[3][3]);
        }

        /*
        -----------------------------------------------------------------------
        Translation Transformation
        -----------------------------------------------------------------------
        */

        /** Sets the translation transformation part of the matrix.
        */
        inline void setTrans( const Vector3& v )
        {
            m[0][3] = v.x;
            m[1][3] = v.y;
            m[2][3] = v.z;
        }

		/** Sets the translation transformation part of the matrix.
		*/
		inline void setTrans( float tx,float ty,float tz)
		{
			m[0][3] = tx;
			m[1][3] = ty;
			m[2][3] = tz;
		}

        /** Extracts the translation transformation part of the matrix.
         */
        inline Vector3 getTrans() const
        {
          return Vector3(m[0][3], m[1][3], m[2][3]);
        }
        

        /** Builds a translation matrix
        */
        inline void makeTrans( const Vector3& v )
        {
            m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = v.x;
            m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0; m[1][3] = v.y;
            m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0; m[2][3] = v.z;
            m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 1.0;
        }

        inline void makeTrans( float tx, float ty, float tz )
        {
            m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = tx;
            m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0; m[1][3] = ty;
            m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0; m[2][3] = tz;
            m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 1.0;
        }

        /** Gets a translation matrix.
        */
        inline static Matrix4 getTrans( const Vector3& v )
        {
            Matrix4 r;

            r.m[0][0] = 1.0; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = v.x;
            r.m[1][0] = 0.0; r.m[1][1] = 1.0; r.m[1][2] = 0.0; r.m[1][3] = v.y;
            r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = 1.0; r.m[2][3] = v.z;
            r.m[3][0] = 0.0; r.m[3][1] = 0.0; r.m[3][2] = 0.0; r.m[3][3] = 1.0;

            return r;
        }

        /** Gets a translation matrix - variation for not using a vector.
        */
        inline static Matrix4 getTrans( float t_x, float t_y, float t_z )
        {
            Matrix4 r;

            r.m[0][0] = 1.0; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = t_x;
            r.m[1][0] = 0.0; r.m[1][1] = 1.0; r.m[1][2] = 0.0; r.m[1][3] = t_y;
            r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = 1.0; r.m[2][3] = t_z;
            r.m[3][0] = 0.0; r.m[3][1] = 0.0; r.m[3][2] = 0.0; r.m[3][3] = 1.0;

            return r;
        }

        /*
        -----------------------------------------------------------------------
        Scale Transformation
        -----------------------------------------------------------------------
        */
        /** Sets the scale part of the matrix.
        */
        inline void setScale( const Vector3& v )
        {
            m[0][0] = v.x;
            m[1][1] = v.y;
            m[2][2] = v.z;
        }

        /** Gets a scale matrix.
        */
        inline static Matrix4 getScale( const Vector3& v )
        {
            Matrix4 r;
            r.m[0][0] = v.x; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = 0.0;
            r.m[1][0] = 0.0; r.m[1][1] = v.y; r.m[1][2] = 0.0; r.m[1][3] = 0.0;
            r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = v.z; r.m[2][3] = 0.0;
            r.m[3][0] = 0.0; r.m[3][1] = 0.0; r.m[3][2] = 0.0; r.m[3][3] = 1.0;

            return r;
        }

        /** Gets a scale matrix - variation for not using a vector.
        */
        inline static Matrix4 getScale( float s_x, float s_y, float s_z )
        {
            Matrix4 r;
            r.m[0][0] = s_x; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = 0.0;
            r.m[1][0] = 0.0; r.m[1][1] = s_y; r.m[1][2] = 0.0; r.m[1][3] = 0.0;
            r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = s_z; r.m[2][3] = 0.0;
            r.m[3][0] = 0.0; r.m[3][1] = 0.0; r.m[3][2] = 0.0; r.m[3][3] = 1.0;

            return r;
        }

        /** Extracts the rotation / scaling part of the Matrix as a 3x3 matrix. 
        @param m3x3 Destination Matrix3
        */
        inline void extract3x3Matrix(Matrix3& m3x3) const
        {
            m3x3.m[0][0] = m[0][0];
            m3x3.m[0][1] = m[0][1];
            m3x3.m[0][2] = m[0][2];
            m3x3.m[1][0] = m[1][0];
            m3x3.m[1][1] = m[1][1];
            m3x3.m[1][2] = m[1][2];
            m3x3.m[2][0] = m[2][0];
            m3x3.m[2][1] = m[2][1];
            m3x3.m[2][2] = m[2][2];

        }

        /** Extracts the rotation / scaling part as a quaternion from the Matrix.
         */
        inline Quaternion extractQuaternion() const
        {
          Matrix3 m3x3;
          extract3x3Matrix(m3x3);
          return Quaternion(m3x3);
        }

        static const Matrix4 ZERO;
        static const Matrix4 IDENTITY;
        /** Useful little matrix which takes 2D clipspace {-1, 1} to {0,1}
            and inverts the Y. */
        static const Matrix4 CLIPSPACE2DTOIMAGESPACE;

        inline Matrix4 operator*(float scalar)
        {
            return Matrix4(
                scalar*m[0][0], scalar*m[0][1], scalar*m[0][2], scalar*m[0][3],
                scalar*m[1][0], scalar*m[1][1], scalar*m[1][2], scalar*m[1][3],
                scalar*m[2][0], scalar*m[2][1], scalar*m[2][2], scalar*m[2][3],
                scalar*m[3][0], scalar*m[3][1], scalar*m[3][2], scalar*m[3][3]);
        }

        /** Function for writing to a stream.
        */
        inline _ReMathExport friend std::ostream& operator <<
            ( std::ostream& o, const Matrix4& m )
        {
            o << "Matrix4(";
			for (size_t i = 0; i < 4; ++i)
            {
                o << " row" << (unsigned)i << "{";
                for(size_t j = 0; j < 4; ++j)
                {
                    o << m[i][j] << " ";
                }
                o << "}";
            }
            o << ")";
            return o;
        }
		
		Matrix4 adjoint() const;
		float determinant() const;
		Matrix4 inverse() const;

		static Matrix4 perspectiveProjection(
			Radian			radFOVy,
			float			aspect,
			float            nearval,
			float            farval);
		static Matrix4       orthogonalProjection(
			float            left,
			float            right,
			float            bottom,
			float            top,
			float            nearval,
			float            farval);

		void resolve(Vector3& translation,Quaternion& rotation,Vector3& scale)
		{
			rotation = extractQuaternion();

			translation.x = m[0][3];
			translation.y = m[1][3];
			translation.z = m[2][3];

			scale.x = Vector3(m[0][0],m[0][1],m[0][2]).length();
			scale.y = Vector3(m[1][0],m[1][1],m[1][2]).length();
			scale.z = Vector3(m[2][0],m[2][1],m[2][2]).length();
		}
		void  transform(const Vector3& position, const Vector3& scale, const Quaternion& orientation)
		{
			Matrix4 destMatrix;
			destMatrix = Matrix4::IDENTITY;
			// Ordering:
			//    1. Scale
			//    2. Rotate
			//    3. Translate

			// Parent scaling is already applied to derived position
			// Own scale is applied before rotation
			Matrix3 rot3x3, scale3x3;
			orientation.ToRotationMatrix(rot3x3);
			scale3x3 = Matrix3::ZERO;
			scale3x3[0][0] = scale.x;
			scale3x3[1][1] = scale.y;
			scale3x3[2][2] = scale.z;

			destMatrix = rot3x3 * scale3x3;
			destMatrix.setTrans(position);

			this->operator=(destMatrix);
		}

	};

    /* Removed from Vector4 and made a non-member here because otherwise
       ReMatrix4.h and ReVector4.h have to try to include and inline each 
       other, which frankly doesn't work ;)
   */
    inline Vector4 operator * (const Vector4& v, const Matrix4& mat)
    {
        return Vector4(
            v.x*mat[0][0] + v.y*mat[1][0] + v.z*mat[2][0] + v.w*mat[3][0],
            v.x*mat[0][1] + v.y*mat[1][1] + v.z*mat[2][1] + v.w*mat[3][1],
            v.x*mat[0][2] + v.y*mat[1][2] + v.z*mat[2][2] + v.w*mat[3][2],
            v.x*mat[0][3] + v.y*mat[1][3] + v.z*mat[2][3] + v.w*mat[3][3]
            );
	}
	/** @} */

}
#endif
