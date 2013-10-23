#ifndef __Vector3_H__
#define __Vector3_H__
#include "ReMathPrerequisite.h"

#include "ReAngle.h"
#include "ReQuaternion.h"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace xs
{
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/

    /** Standard 3-dimensional vector.
        @remarks
            A direction in 3D space represented as distances along the 3
            orthoganal axes (x, y, z). Note that positions, directions and
            scaling factors can be represented by a vector, depending on how
            you interpret the values.
    */
    class _ReMathExport Vector3
    {
    public:
        union {
            struct {
                float x, y, z;        
            };
            float val[3];
        };

    public:
        inline Vector3()
        {
        }

        inline Vector3( float fX, float fY, float fZ ) 
            : x( fX ), y( fY ), z( fZ )
        {
        }

        inline Vector3( float afCoordinate[3] )
            : x( afCoordinate[0] ),
              y( afCoordinate[1] ),
              z( afCoordinate[2] )
        {
        }

        inline Vector3( int afCoordinate[3] )
        {
            x = (float)afCoordinate[0];
            y = (float)afCoordinate[1];
            z = (float)afCoordinate[2];
        }

        inline Vector3( const float* const r )
            : x( r[0] ), y( r[1] ), z( r[2] )
        {
        }

        inline Vector3( const Vector3& rkVector )
            : x( rkVector.x ), y( rkVector.y ), z( rkVector.z )
        {
        }

		inline float operator [] ( size_t i ) const
        {
            assert( i < 3 );

            return *(&x+i);
        }

		inline float& operator [] ( size_t i )
        {
            assert( i < 3 );

            return *(&x+i);
        }

        /** Assigns the value of the other vector.
            @param
                rkVector The other vector
        */
        inline Vector3& operator = ( const Vector3& rkVector )
        {
            x = rkVector.x;
            y = rkVector.y;
            z = rkVector.z;            

            return *this;
        }

        inline bool operator == ( const Vector3& rkVector ) const
        {
            return ( x == rkVector.x && y == rkVector.y && z == rkVector.z );
        }

        inline bool operator != ( const Vector3& rkVector ) const
        {
            return ( x != rkVector.x || y != rkVector.y || z != rkVector.z );
        }

        // arithmetic operations
        inline Vector3 operator + ( const Vector3& rkVector ) const
        {
            Vector3 kSum;

            kSum.x = x + rkVector.x;
            kSum.y = y + rkVector.y;
            kSum.z = z + rkVector.z;

            return kSum;
        }

        inline Vector3 operator - ( const Vector3& rkVector ) const
        {
            Vector3 kDiff;

            kDiff.x = x - rkVector.x;
            kDiff.y = y - rkVector.y;
            kDiff.z = z - rkVector.z;

            return kDiff;
        }

        inline Vector3 operator * ( float fScalar ) const
        {
            Vector3 kProd;

            kProd.x = fScalar*x;
            kProd.y = fScalar*y;
            kProd.z = fScalar*z;

            return kProd;
        }

        inline Vector3 operator * ( const Vector3& rhs) const
        {
            Vector3 kProd;

            kProd.x = rhs.x * x;
            kProd.y = rhs.y * y;
            kProd.z = rhs.z * z;

            return kProd;
        }

        inline Vector3 operator / ( float fScalar ) const
        {
            assert( fScalar != 0.0 );

            Vector3 kDiv;

            float fInv = 1.0 / fScalar;
            kDiv.x = x * fInv;
            kDiv.y = y * fInv;
            kDiv.z = z * fInv;

            return kDiv;
        }

        inline Vector3 operator / ( const Vector3& rhs) const
        {
            Vector3 kDiv;

            kDiv.x = x / rhs.x;
            kDiv.y = y / rhs.y;
            kDiv.z = z / rhs.z;

            return kDiv;
        }


        inline Vector3 operator - () const
        {
            Vector3 kNeg;

            kNeg.x = -x;
            kNeg.y = -y;
            kNeg.z = -z;

            return kNeg;
        }

        inline friend Vector3 operator * ( float fScalar, const Vector3& rkVector )
        {
            Vector3 kProd;

            kProd.x = fScalar * rkVector.x;
            kProd.y = fScalar * rkVector.y;
            kProd.z = fScalar * rkVector.z;

            return kProd;
        }

        // arithmetic updates
        inline Vector3& operator += ( const Vector3& rkVector )
        {
            x += rkVector.x;
            y += rkVector.y;
            z += rkVector.z;

            return *this;
        }

        inline Vector3& operator -= ( const Vector3& rkVector )
        {
            x -= rkVector.x;
            y -= rkVector.y;
            z -= rkVector.z;

            return *this;
        }

        inline Vector3& operator *= ( float fScalar )
        {
            x *= fScalar;
            y *= fScalar;
            z *= fScalar;
            return *this;
        }

        inline Vector3& operator *= ( const Vector3& rkVector )
        {
            x *= rkVector.x;
            y *= rkVector.y;
            z *= rkVector.z;

            return *this;
        }

        inline Vector3& operator /= ( float fScalar )
        {
            assert( fScalar != 0.0 );

            float fInv = 1.0 / fScalar;

            x *= fInv;
            y *= fInv;
            z *= fInv;

            return *this;
        }

        inline Vector3& operator /= ( const Vector3& rkVector )
        {
            x /= rkVector.x;
            y /= rkVector.y;
            z /= rkVector.z;

            return *this;
        }


        /** Returns the length (magnitude) of the vector.
            @warning
                This operation requires a square root and is expensive in
                terms of CPU operations. If you don't need to know the exact
                length (e.g. for just comparing lengths) use squaredLength()
                instead.
        */
        inline float length () const
        {
            return Math::Sqrt( x * x + y * y + z * z );
        }

		inline float lengthF () const
		{
			return Math::SqrtF( x * x + y * y + z * z );
		}

        /** Returns the square of the length(magnitude) of the vector.
            @remarks
                This  method is for efficiency - calculating the actual
                length of a vector requires a square root, which is expensive
                in terms of the operations required. This method returns the
                square of the length of the vector, i.e. the same as the
                length but before the square root is taken. Use this if you
                want to find the longest / shortest vector without incurring
                the square root.
        */
        inline float squaredLength () const
        {
            return x * x + y * y + z * z;
        }

        /** Calculates the dot (scalar) product of this vector with another.
            @remarks
                The dot product can be used to calculate the angle between 2
                vectors. If both are unit vectors, the dot product is the
                cosine of the angle; otherwise the dot product must be
                divided by the product of the lengths of both vectors to get
                the cosine of the angle. This result can further be used to
                calculate the distance of a point from a plane.
            @param
                vec Vector with which to calculate the dot product (together
                with this one).
            @returns
                A float representing the dot product value.
        */
        inline float dotProduct(const Vector3& vec) const
        {
            return x * vec.x + y * vec.y + z * vec.z;
        }

        /** Normalises the vector.
            @remarks
                This method normalises the vector such that it's
                length / magnitude is 1. The result is called a unit vector.
            @note
                This function will not crash for zero-sized vectors, but there
                will be no changes made to their components.
            @returns The previous length of the vector.
        */
        inline float normalize()
        {
            float fLength = Math::Sqrt( x * x + y * y + z * z );

            // Will also work for zero-sized vectors, but will change nothing
            if ( fLength > 1e-08 )
            {
                float fInvLength = 1.0 / fLength;
                x *= fInvLength;
                y *= fInvLength;
                z *= fInvLength;
            }

            return fLength;
        }

        /** Calculates the cross-product of 2 vectors, i.e. the vector that
            lies perpendicular to them both.
            @remarks
                The cross-product is normally used to calculate the normal
                vector of a plane, by calculating the cross-product of 2
                non-equivalent vectors which lie on the plane (e.g. 2 edges
                of a triangle).
            @param
                vec Vector which, together with this one, will be used to
                calculate the cross-product.
            @returns
                A vector which is the result of the cross-product. This
                vector will <b>NOT</b> be normalised, to maximise efficiency
                - call Vector3::normalize on the result if you wish this to
                be done. As for which side the resultant vector will be on, the
                returned vector will be on the side from which the arc from 'this'
                to rkVector is anticlockwise, e.g. UNIT_Y.crossProduct(UNIT_Z) 
                = UNIT_X, whilst UNIT_Z.crossProduct(UNIT_Y) = -UNIT_X.
            @par
                For a clearer explanation, look a the left and the bottom edges
                of your monitor's screen. Assume that the first vector is the
                left edge and the second vector is the bottom edge, both of
                them starting from the lower-left corner of the screen. The
                resulting vector is going to be perpendicular to both of them
                and will go <i>inside</i> the screen, towards the cathode tube
                (assuming you're using a CRT monitor, of course).
        */
        inline Vector3 crossProduct( const Vector3& rkVector ) const
        {
            Vector3 kCross;

            kCross.x = y * rkVector.z - z * rkVector.y;
            kCross.y = z * rkVector.x - x * rkVector.z;
            kCross.z = x * rkVector.y - y * rkVector.x;

            return kCross;
        }

        /** Returns a vector at a point half way between this and the passed
            in vector.
        */
        inline Vector3 midPoint( const Vector3& vec ) const
        {
            return Vector3( 
                ( x + vec.x ) * 0.5, 
                ( y + vec.y ) * 0.5, 
                ( z + vec.z ) * 0.5 );
        }

        /** Returns true if the vector's scalar components are all greater
            that the ones of the vector it is compared against.
        */
        inline bool operator < ( const Vector3& rhs ) const
        {
            if( x < rhs.x && y < rhs.y && z < rhs.z )
                return true;
            return false;
        }

        /** Returns true if the vector's scalar components are all smaller
            that the ones of the vector it is compared against.
        */
        inline bool operator > ( const Vector3& rhs ) const
        {
            if( x > rhs.x && y > rhs.y && z > rhs.z )
                return true;
            return false;
        }

        /** Sets this vector's components to the minimum of its own and the 
            ones of the passed in vector.
            @remarks
                'Minimum' in this case means the combination of the lowest
                value of x, y and z from both vectors. Lowest is taken just
                numerically, not magnitude, so -1 < 0.
        */
        inline void makeFloor( const Vector3& cmp )
        {
            if( cmp.x < x ) x = cmp.x;
            if( cmp.y < y ) y = cmp.y;
            if( cmp.z < z ) z = cmp.z;
        }

        /** Sets this vector's components to the maximum of its own and the 
            ones of the passed in vector.
            @remarks
                'Maximum' in this case means the combination of the highest
                value of x, y and z from both vectors. Highest is taken just
                numerically, not magnitude, so 1 > -3.
        */
        inline void makeCeil( const Vector3& cmp )
        {
            if( cmp.x > x ) x = cmp.x;
            if( cmp.y > y ) y = cmp.y;
            if( cmp.z > z ) z = cmp.z;
        }

        /** Generates a vector perpendicular to this vector (eg an 'up' vector).
            @remarks
                This method will return a vector which is perpendicular to this
                vector. There are an infinite number of possibilities but this 
                method will guarantee to generate one of them. If you need more 
                control you should use the Quaternion class.
        */
        inline Vector3 perpendicular(void) const
        {
            static const float fSquareZero = 1e-06 * 1e-06;

            Vector3 perp = this->crossProduct( Vector3::UNIT_X );

            // Check length
            if( perp.squaredLength() < fSquareZero )
            {
                /* This vector is the Y axis multiplied by a scalar, so we have 
                   to use another axis.
                */
                perp = this->crossProduct( Vector3::UNIT_Y );
            }

            return perp;
        }
        /** Generates a new random vector which deviates from this vector by a
            given angle in a random direction.
            @remarks
                This method assumes that the random number generator has already 
                been seeded appropriately.
            @param 
                angle The angle at which to deviate
            @param 
                up Any vector perpendicular to this one (which could generated 
                by cross-product of this vector and any other non-colinear 
                vector). If you choose not to provide this the function will 
                derive one on it's own, however if you provide one yourself the 
                function will be faster (this allows you to reuse up vectors if 
                you call this method more than once) 
            @returns 
                A random vector which deviates from this vector by angle. This 
                vector will not be normalised, normalize it if you wish 
                afterwards.
        */
        inline Vector3 randomDeviant(
            const Radian& angle,
            const Vector3& up = Vector3::ZERO ) const
        {
            Vector3 newUp;

            if (up == Vector3::ZERO)
            {
                // Generate an up vector
                newUp = this->perpendicular();
            }
            else
            {
                newUp = up;
            }

            // Rotate up vector by random amount around this
            Quaternion q;
            q.FromAngleAxis( Radian(Math::UnitRandom() * Math::TWO_PI), *this );
            newUp = q * newUp;

            // Finally rotate this by given angle around randomised up
            q.FromAngleAxis( angle, newUp );
            return q * (*this);
        }
#ifndef RE_FORCE_ANGLE_TYPES
        inline Vector3 randomDeviant(
            float angle,
            const Vector3& up = Vector3::ZERO ) const
        {
            return randomDeviant ( Radian(angle), up );
        }
#endif//RE_FORCE_ANGLE_TYPES

        /** Gets the shortest arc quaternion to rotate this vector to the destination
            vector. 
        @remarks
            Don't call this if you think the dest vector can be close to the inverse
            of this vector, since then ANY axis of rotation is ok. 
        */
        Quaternion getRotationTo(const Vector3& dest) const
        {
            // Based on Stan Melax's article in Game Programming Gems
            Quaternion q;
            // Copy, since cannot modify local
            Vector3 v0 = *this;
            Vector3 v1 = dest;
            v0.normalize();
            v1.normalize();

            Vector3 c = v0.crossProduct(v1);

            // NB if the crossProduct approaches zero, we get unstable because ANY axis will do
            // when v0 == -v1
            float d = v0.dotProduct(v1);
            // If dot == 1, vectors are the same
            if (d >= 1.0f)
            {
                return Quaternion::IDENTITY;
            }
            float s = Math::Sqrt( (1+d)*2 );
            assert (s != 0 && "Divide by zero!");
            float invs = 1 / s;


            q.x = c.x * invs;
            q.y = c.y * invs;
            q.z = c.z * invs;
            q.w = s * 0.5;
            return q;
        }

        /** Returns true if this vector is zero length. */
        inline bool isZeroLength(void) const
        {
            float sqlen = (x * x) + (y * y) + (z * z);
            return (sqlen < (1e-06 * 1e-06));

        }

        /** As normalize, except that this vector is unaffected and the
            normalised vector is returned as a copy. */
        inline Vector3 direction(void) const
        {
            Vector3 ret = *this;
            ret.normalize();
            return ret;
        }

        /** Calculates a reflection vector to the plane with the given normal . 
        @remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
        */
        inline Vector3 reflect(const Vector3& normal) const
        {
            return Vector3( *this - ( 2 * this->dotProduct(normal) * normal ) );
        }

		/** Returns whether this vector is within a positional tolerance
			of another vector.
		@param rhs The vector to compare with
		@param tolerance The amount that each element of the vector may vary by 
			and still be considered equal
		*/
		inline bool positionEquals(const Vector3& rhs, float tolerance = 1e-03) const
		{
			return Math::RealEqual(x, rhs.x, tolerance) &&
				Math::RealEqual(y, rhs.y, tolerance) &&
				Math::RealEqual(z, rhs.z, tolerance);
			
		}
		/** Returns whether this vector is within a directional tolerance
			of another vector.
		@param rhs The vector to compare with
		@param tolerance The maximum angle by which the vectors may vary and
			still be considered equal
		*/
		inline bool directionEquals(const Vector3& rhs, 
			const Radian& tolerance) const
		{
			float dot = dotProduct(rhs);
			Radian angle = Math::ACos(dot);

			return Math::Abs(angle.valueRadians()) <= tolerance.valueRadians();
			
		}

        // special points
        static const Vector3 ZERO;
        static const Vector3 UNIT_X;
        static const Vector3 UNIT_Y;
        static const Vector3 UNIT_Z;
        static const Vector3 NEGATIVE_UNIT_X;
        static const Vector3 NEGATIVE_UNIT_Y;
        static const Vector3 NEGATIVE_UNIT_Z;
        static const Vector3 UNIT_SCALE;

        /** Function for writing to a stream.
        */
        inline _ReMathExport friend std::ostream& operator <<
            ( std::ostream& o, const Vector3& v )
        {
            o << "Vector3(" << v.x << ", " << v.y << ", " << v.z << ")";
            return o;
        }
		//----------------------------------------------------------------------------
		inline Vector3 Vector3::Min(const Vector3 &v) const {
			return Vector3(std::min(v.x, x), std::min(v.y, y), std::min(v.z, z));
		}

		//----------------------------------------------------------------------------
		inline Vector3 Vector3::Max(const Vector3 &v) const {
			return Vector3(std::max(v.x, x), std::max(v.y, y), std::max(v.z, z));
		}
    };
	/** @} */

}
#endif
