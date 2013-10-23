#ifndef __Vector4_H__
#define __Vector4_H__
#include "ReMathPrerequisite.h"

#include "ReVector3.h"

namespace xs
{
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/

    /** 4-dimensional homogenous vector.
    */
    class _ReMathExport Vector4
    {
    public:
        union {
            struct {
                float x, y, z, w;        
            };
            float val[4];
        };

    public:
        inline Vector4()
        {
        }

        inline Vector4( float fX, float fY, float fZ, float fW ) 
            : x( fX ), y( fY ), z( fZ ), w( fW)
        {
        }

        inline Vector4( float afCoordinate[4] )
            : x( afCoordinate[0] ),
              y( afCoordinate[1] ),
              z( afCoordinate[2] ), 
              w (afCoordinate[3] )
        {
        }

        inline Vector4( int afCoordinate[4] )
        {
            x = (float)afCoordinate[0];
            y = (float)afCoordinate[1];
            z = (float)afCoordinate[2];
            w = (float)afCoordinate[3];
        }

        inline Vector4( const float* const r )
            : x( r[0] ), y( r[1] ), z( r[2] ), w( r[3] )
        {
        }

		inline Vector4( const Vector3& rkVector )
			: x( rkVector.x ), y( rkVector.y ), z( rkVector.z ), w (1)
		{
		}

		inline Vector4( const Vector3& rkVector,float w)
			: x( rkVector.x ), y( rkVector.y ), z( rkVector.z ),w(w)
		{
		}

        inline Vector4( const Vector4& rkVector )
            : x( rkVector.x ), y( rkVector.y ), z( rkVector.z ), w (rkVector.w)
        {
        }

		inline float operator [] ( size_t i ) const
        {
            assert( i < 4 );

            return *(&x+i);
        }

		inline float& operator [] ( size_t i )
        {
            assert( i < 4 );

            return *(&x+i);
        }

        /** Assigns the value of the other vector.
            @param
                rkVector The other vector
        */
        inline Vector4& operator = ( const Vector4& rkVector )
        {
            x = rkVector.x;
            y = rkVector.y;
            z = rkVector.z;            
            w = rkVector.w;            

            return *this;
        }

        inline bool operator == ( const Vector4& rkVector ) const
        {
            return ( x == rkVector.x && 
                y == rkVector.y && 
                z == rkVector.z &&
                w == rkVector.w );
        }

        inline bool operator != ( const Vector4& rkVector ) const
        {
            return ( x != rkVector.x || 
                y != rkVector.y || 
                z != rkVector.z ||
                w != rkVector.w );
        }

        inline Vector4& operator = (const Vector3& rhs)
        {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
            w = 1.0f;
            return *this;
        }

        // arithmetic operations
        inline Vector4 operator + ( const Vector4& rkVector ) const
        {
            Vector4 kSum;

            kSum.x = x + rkVector.x;
            kSum.y = y + rkVector.y;
            kSum.z = z + rkVector.z;
            kSum.w = w + rkVector.w;

            return kSum;
        }

        inline Vector4 operator - ( const Vector4& rkVector ) const
        {
            Vector4 kDiff;

            kDiff.x = x - rkVector.x;
            kDiff.y = y - rkVector.y;
            kDiff.z = z - rkVector.z;
            kDiff.w = w - rkVector.w;

            return kDiff;
        }

        inline Vector4 operator * ( float fScalar ) const
        {
            Vector4 kProd;

            kProd.x = fScalar*x;
            kProd.y = fScalar*y;
            kProd.z = fScalar*z;
            kProd.w = fScalar*w;

            return kProd;
        }

        inline Vector4 operator * ( const Vector4& rhs) const
        {
            Vector4 kProd;

            kProd.x = rhs.x * x;
            kProd.y = rhs.y * y;
            kProd.z = rhs.z * z;
            kProd.w = rhs.w * w;

            return kProd;
        }

        inline Vector4 operator / ( float fScalar ) const
        {
            assert( fScalar != 0.0 );

            Vector4 kDiv;

            float fInv = 1.0 / fScalar;
            kDiv.x = x * fInv;
            kDiv.y = y * fInv;
            kDiv.z = z * fInv;
            kDiv.w = w * fInv;

            return kDiv;
        }

        inline Vector4 operator / ( const Vector4& rhs) const
        {
            Vector4 kDiv;

            kDiv.x = x / rhs.x;
            kDiv.y = y / rhs.y;
            kDiv.z = z / rhs.z;
            kDiv.w = w / rhs.w;

            return kDiv;
        }


        inline Vector4 operator - () const
        {
            Vector4 kNeg;

            kNeg.x = -x;
            kNeg.y = -y;
            kNeg.z = -z;
            kNeg.w = -w;

            return kNeg;
        }

        inline friend Vector4 operator * ( float fScalar, const Vector4& rkVector )
        {
            Vector4 kProd;

            kProd.x = fScalar * rkVector.x;
            kProd.y = fScalar * rkVector.y;
            kProd.z = fScalar * rkVector.z;
            kProd.w = fScalar * rkVector.w;

            return kProd;
        }

        // arithmetic updates
        inline Vector4& operator += ( const Vector4& rkVector )
        {
            x += rkVector.x;
            y += rkVector.y;
            z += rkVector.z;
            w += rkVector.w;

            return *this;
        }

        inline Vector4& operator -= ( const Vector4& rkVector )
        {
            x -= rkVector.x;
            y -= rkVector.y;
            z -= rkVector.z;
            w -= rkVector.w;

            return *this;
        }

        inline Vector4& operator *= ( float fScalar )
        {
            x *= fScalar;
            y *= fScalar;
            z *= fScalar;
            w *= fScalar;
            return *this;
        }

        inline Vector4& operator *= ( const Vector4& rkVector )
        {
            x *= rkVector.x;
            y *= rkVector.y;
            z *= rkVector.z;
            w *= rkVector.w;

            return *this;
        }

        inline Vector4& operator /= ( float fScalar )
        {
            assert( fScalar != 0.0 );

            float fInv = 1.0 / fScalar;

            x *= fInv;
            y *= fInv;
            z *= fInv;
            w *= fInv;

            return *this;
        }

        inline Vector4& operator /= ( const Vector4& rkVector )
        {
            x /= rkVector.x;
            y /= rkVector.y;
            z /= rkVector.z;
            w /= rkVector.w;

            return *this;
        }

        /** Calculates the dot (scalar) product of this vector with another.
            @param
                vec Vector with which to calculate the dot product (together
                with this one).
            @returns
                A float representing the dot product value.
        */
        inline float dotProduct(const Vector4& vec) const
        {
            return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
        }
        /** Function for writing to a stream.
        */
        inline _ReMathExport friend std::ostream& operator <<
            ( std::ostream& o, const Vector4& v )
        {
            o << "Vector4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
            return o;
		}
    };
	/** @} */

}
#endif
