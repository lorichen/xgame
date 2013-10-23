
#include "ReMathPch.h"
#include "ReMatrix4.h"

#include "ReVector3.h"
#include "ReMatrix3.h"

namespace xs
{
	uint Matrix4::s_CPUAcceFlag = -1;

    const Matrix4 Matrix4::ZERO(
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0 );

    const Matrix4 Matrix4::IDENTITY(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1 );

    const Matrix4 Matrix4::CLIPSPACE2DTOIMAGESPACE(
        0.5,    0,  0, 0.5, 
          0, -0.5,  0, 0.5, 
          0,    0,  1,   0,
          0,    0,  0,   1);

    inline float
        MINOR(const Matrix4& m, const size_t r0, const size_t r1, const size_t r2, 
								const size_t c0, const size_t c1, const size_t c2)
    {
        return m[r0][c0] * (m[r1][c1] * m[r2][c2] - m[r2][c1] * m[r1][c2]) -
            m[r0][c1] * (m[r1][c0] * m[r2][c2] - m[r2][c0] * m[r1][c2]) +
            m[r0][c2] * (m[r1][c0] * m[r2][c1] - m[r2][c0] * m[r1][c1]);
    }


    Matrix4 Matrix4::adjoint() const
    {//这个的计算过程是反过来的 - 最下面的一行最先计算
        return Matrix4( MINOR(*this, 1, 2, 3, 1, 2, 3),
            -MINOR(*this, 0, 2, 3, 1, 2, 3),
            MINOR(*this, 0, 1, 3, 1, 2, 3),
            -MINOR(*this, 0, 1, 2, 1, 2, 3),

            -MINOR(*this, 1, 2, 3, 0, 2, 3),
            MINOR(*this, 0, 2, 3, 0, 2, 3),
            -MINOR(*this, 0, 1, 3, 0, 2, 3),
            MINOR(*this, 0, 1, 2, 0, 2, 3),

            MINOR(*this, 1, 2, 3, 0, 1, 3),
            -MINOR(*this, 0, 2, 3, 0, 1, 3),
            MINOR(*this, 0, 1, 3, 0, 1, 3),
            -MINOR(*this, 0, 1, 2, 0, 1, 3),

            -MINOR(*this, 1, 2, 3, 0, 1, 2),
            MINOR(*this, 0, 2, 3, 0, 1, 2),
            -MINOR(*this, 0, 1, 3, 0, 1, 2),
            MINOR(*this, 0, 1, 2, 0, 1, 2));
    }


    float Matrix4::determinant() const
    {
        return m[0][0] * MINOR(*this, 1, 2, 3, 1, 2, 3) -
            m[0][1] * MINOR(*this, 1, 2, 3, 0, 2, 3) +
            m[0][2] * MINOR(*this, 1, 2, 3, 0, 1, 3) -
            m[0][3] * MINOR(*this, 1, 2, 3, 0, 1, 2);
    }

    Matrix4 Matrix4::inverse() const
    {
        return adjoint() * (1.0f / determinant());
    }

	Matrix4 Matrix4::perspectiveProjection(
		Radian			radFOVy,
		float			aspect,
		float            nearval,
		float            farval)
	{
		float yy = nearval * Math::Tan(radFOVy / 2.0);
		float xx = yy * aspect;
		float left = -xx;
		float right = xx;
		float bottom = -yy;
		float top = yy;

		float x, y, a, b, c, d;

		x = (2.0f*nearval) / (right-left);
		y = (2.0f*nearval) / (top-bottom);
		a = (right+left) / (right-left);
		b = (top+bottom) / (top-bottom);

		if ((float)farval >= (float)Math::inf()) {
			// Infinite view frustum
			c = -1.0f;
			d = -2.0f * nearval;
		} else {
			c = -(farval+nearval) / (farval-nearval);
			d = -(2.0f*farval*nearval) / (farval-nearval);
		}

		return Matrix4(
			x,  0,  a,  0,
			0,  y,  b,  0,
			0,  0,  c,  d,
			0,  0, -1,  0);
	}
/*	{
		Radian fovy = radFOVy;
		float farPlane = farval;
		float nearPlane = nearval;
        Radian thetaY ( fovy / 2.0f );
        float tanThetaY = Math::Tan(thetaY);
        //float thetaX = thetaY * aspect;
        //float tanThetaX = Math::Tan(thetaX);

        // Calc matrix elements
        float w = (1.0f / tanThetaY) / aspect;
        float h = 1.0f / tanThetaY;
        float q, qn;
        if (farPlane == 0)
        {
            // Infinite far plane
            q = - 1;
            qn = nearPlane * (- 2);
        }
        else
        {
            q = -(farPlane + nearPlane) / (farPlane - nearPlane);
            qn = -2 * (farPlane * nearPlane) / (farPlane - nearPlane);
        }

        // NB This creates Z in range [-1,1]
        //
        // [ w   0   0   0  ]
        // [ 0   h   0   0  ]
        // [ 0   0   q   qn ]
        // [ 0   0   -1  0  ]

		Matrix4 dest;
        dest = Matrix4::ZERO;
        dest[0][0] = w;
        dest[1][1] = h;
        dest[2][2] = q;
        dest[2][3] = qn;
        dest[3][2] = -1;

		return dest;
    }*/
	Matrix4 Matrix4::orthogonalProjection(
		float            left,
		float            right,
		float            bottom,
		float            top,
		float            nearval,
		float            farval) 
	{
		// Adapted from Mesa
		float x, y, z;
		float tx,  ty, tz;

		x = 2.0f / (right - left);
		y = 2.0f / (top - bottom);
		z = -2.0f / (farval - nearval);
		tx = -(right + left) / (right - left);
		ty = -(top + bottom) / (top - bottom);
		tz = -(farval + nearval) / (farval - nearval);

		// NB: This creates 'uniform' orthographic projection matrix,
		// which depth range [-1,1], right-handed rules
		//
		// [ A   0   0   C  ]
		// [ 0   B   0   D  ]
		// [ 0   0   q   qn ]
		// [ 0   0   0   1  ]

		return Matrix4( 
			x ,   0.0f, 0.0f,   tx,
			0.0f,   y , 0.0f,   ty,
			0.0f, 0.0f,   z ,   tz,
			0.0f, 0.0f, 0.0f, 1.0f
			);
	}
}
