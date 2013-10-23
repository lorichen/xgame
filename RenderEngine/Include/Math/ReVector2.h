#ifndef __Vector2_H__
#define __Vector2_H__
#include "ReMathPrerequisite.h"

#include "ReAngle.h"

namespace xs
{
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/

    /** Standard 2-dimensional vector.
        @remarks
            A direction in 2D space represented as distances along the 2
            orthoganal axes (x, y). Note that positions, directions and
            scaling factors can be represented by a vector, depending on how
            you interpret the values.
    */
    class _ReMathExport Vector2
    {
    public:
        union {
            struct {
                float x, y;        
            };
            float val[2];
        };

    public:
        inline Vector2()
        {
        }

        inline Vector2( float fX, float fY ) 
            : x( fX ), y( fY )
        {
        }

        inline Vector2( float afCoordinate[2] )
            : x( afCoordinate[0] ),
              y( afCoordinate[1] )
        {
        }

        inline Vector2( int afCoordinate[2] )
        {
            x = (float)afCoordinate[0];
            y = (float)afCoordinate[1];
        }

        inline Vector2( const float* const r )
            : x( r[0] ), y( r[1] )
        {
        }

        inline Vector2( const Vector2& rkVector )
            : x( rkVector.x ), y( rkVector.y )
        {
        }

		inline float operator [] ( size_t i ) const
        {
            assert( i < 2 );

            return *(&x+i);
        }

		inline float& operator [] ( size_t i )
        {
            assert( i < 2 );

            return *(&x+i);
        }

        /** Assigns the value of the other vector.
            @param
                rkVector The other vector
        */
        inline Vector2& operator = ( const Vector2& rkVector )
        {
            x = rkVector.x;
            y = rkVector.y;        

            return *this;
        }

        inline bool operator == ( const Vector2& rkVector ) const
        {
            return ( x == rkVector.x && y == rkVector.y );
        }

        inline bool operator != ( const Vector2& rkVector ) const
        {
            return ( x != rkVector.x || y != rkVector.y  );
        }

        // arithmetic operations
        inline Vector2 operator + ( const Vector2& rkVector ) const
        {
            Vector2 kSum;

            kSum.x = x + rkVector.x;
            kSum.y = y + rkVector.y;

            return kSum;
        }

        inline Vector2 operator - ( const Vector2& rkVector ) const
        {
            Vector2 kDiff;

            kDiff.x = x - rkVector.x;
            kDiff.y = y - rkVector.y;

            return kDiff;
        }

        inline Vector2 operator * ( float fScalar ) const
        {
            Vector2 kProd;

            kProd.x = fScalar*x;
            kProd.y = fScalar*y;

            return kProd;
        }

        inline Vector2 operator * ( const Vector2& rhs) const
        {
            Vector2 kProd;

            kProd.x = rhs.x * x;
            kProd.y = rhs.y * y;

            return kProd;
        }

        inline Vector2 operator / ( float fScalar ) const
        {
            assert( fScalar != 0.0 );

            Vector2 kDiv;

            float fInv = 1.0 / fScalar;
            kDiv.x = x * fInv;
            kDiv.y = y * fInv;

            return kDiv;
        }

        inline Vector2 operator - () const
        {
            Vector2 kNeg;

            kNeg.x = -x;
            kNeg.y = -y;

            return kNeg;
        }

        inline friend Vector2 operator * ( float fScalar, const Vector2& rkVector )
        {
            Vector2 kProd;

            kProd.x = fScalar * rkVector.x;
            kProd.y = fScalar * rkVector.y;

            return kProd;
        }

        // arithmetic updates
        inline Vector2& operator += ( const Vector2& rkVector )
        {
            x += rkVector.x;
            y += rkVector.y;

            return *this;
        }

        inline Vector2& operator -= ( const Vector2& rkVector )
        {
            x -= rkVector.x;
            y -= rkVector.y;

            return *this;
        }

        inline Vector2& operator *= ( float fScalar )
        {
            x *= fScalar;
            y *= fScalar;

            return *this;
        }

        inline Vector2& operator /= ( float fScalar )
        {
            assert( fScalar != 0.0 );

            float fInv = 1.0 / fScalar;

            x *= fInv;
            y *= fInv;

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
            return Math::Sqrt( x * x + y * y );
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
            return x * x + y * y;
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
        inline float dotProduct(const Vector2& vec) const
        {
            return x * vec.x + y * vec.y;
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
            float fLength = Math::Sqrt( x * x + y * y);

            // Will also work for zero-sized vectors, but will change nothing
            if ( fLength > 1e-08 )
            {
                float fInvLength = 1.0 / fLength;
                x *= fInvLength;
                y *= fInvLength;
            }

            return fLength;
        }

    
   
        /** Returns a vector at a point half way between this and the passed
            in vector.
        */
        inline Vector2 midPoint( const Vector2& vec ) const
        {
            return Vector2( 
                ( x + vec.x ) * 0.5, 
                ( y + vec.y ) * 0.5 );
        }

        /** Returns true if the vector's scalar components are all greater
            that the ones of the vector it is compared against.
        */
        inline bool operator < ( const Vector2& rhs ) const
        {
            if( x < rhs.x && y < rhs.y )
                return true;
            return false;
        }

        /** Returns true if the vector's scalar components are all smaller
            that the ones of the vector it is compared against.
        */
        inline bool operator > ( const Vector2& rhs ) const
        {
            if( x > rhs.x && y > rhs.y )
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
        inline void makeFloor( const Vector2& cmp )
        {
            if( cmp.x < x ) x = cmp.x;
            if( cmp.y < y ) y = cmp.y;
        }

        /** Sets this vector's components to the maximum of its own and the 
            ones of the passed in vector.
            @remarks
                'Maximum' in this case means the combination of the highest
                value of x, y and z from both vectors. Highest is taken just
                numerically, not magnitude, so 1 > -3.
        */
        inline void makeCeil( const Vector2& cmp )
        {
            if( cmp.x > x ) x = cmp.x;
            if( cmp.y > y ) y = cmp.y;
        }

        /** Generates a vector perpendicular to this vector (eg an 'up' vector).
            @remarks
                This method will return a vector which is perpendicular to this
                vector. There are an infinite number of possibilities but this 
                method will guarantee to generate one of them. If you need more 
                control you should use the Quaternion class.
        */
        inline Vector2 perpendicular(void) const
        {         
            return Vector2 (-y, x);
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
        inline Vector2 crossProduct( const Vector2& rkVector ) const
        {
            return Vector2(-rkVector.y, rkVector.x);
        }
        /** Generates a new random vector which deviates from this vector by a
            given angle in a random direction.
            @remarks
                This method assumes that the random number generator has already 
                been seeded appropriately.
            @param 
                angle The angle at which to deviate in radians
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
        inline Vector2 randomDeviant(
            float angle) const
        {
           
            angle *=  Math::UnitRandom() * Math::TWO_PI;
            float cosa = cos(angle);
            float sina = sin(angle);
            return  Vector2(cosa * x - sina * y,
                            sina * x + cosa * y);
        }
        
        /** Returns true if this vector is zero length. */
        inline bool isZeroLength(void) const
        {
            float sqlen = (x * x) + (y * y);
            return (sqlen < (1e-06 * 1e-06));

        }

        /** As normalize, except that this vector is unaffected and the
            normalised vector is returned as a copy. */
        inline Vector2 normalisedCopy(void) const
        {
            Vector2 ret = *this;
            ret.normalize();
            return ret;
        }

        /** Calculates a reflection vector to the plane with the given normal . 
        @remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
        */
        inline Vector2 reflect(const Vector2& normal) const
        {
            return Vector2( *this - ( 2 * this->dotProduct(normal) * normal ) );
        }

        // special points
        static const Vector2 ZERO;
        static const Vector2 UNIT_X;
        static const Vector2 UNIT_Y;
        static const Vector2 NEGATIVE_UNIT_X;
        static const Vector2 NEGATIVE_UNIT_Y;
        static const Vector2 UNIT_SCALE;

        /** Function for writing to a stream.
        */
        inline _ReMathExport friend std::ostream& operator <<
            ( std::ostream& o, const Vector2& v )
        {
            o << "Vector2(" << v.x << ", " << v.y <<  ")";
            return o;
		}
    };
	/** @} */

}
#endif
