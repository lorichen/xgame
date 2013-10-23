#ifndef __Color4_H__
#define __Color4_H__
#include "ReMathPrerequisite.h"

namespace xs
{
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/
	

    typedef uint RGBA;
    typedef uint ARGB;
    typedef uint ABGR;

    /** Class representing colour.
	    @remarks
		    Colour is represented as 4 components, each of which is a
		    floating-point value from 0.0 to 1.0.
	    @par
		    The 3 'normal' colour components are red, green and blue, a higher
		    number indicating greater amounts of that component in the colour.
		    The forth component is the 'alpha' value, which represents
		    transparency. In this case, 0.0 is completely transparent and 1.0 is
		    fully opaque.
    */
    class _ReMathExport Color4
    {
    public:
	    static Color4 Black;
	    static Color4 White;
	    static Color4 Red;
	    static Color4 Green;
	    static Color4 Blue;

	    explicit Color4( float red = 1.0f,
				    float green = 1.0f,
				    float blue = 1.0f,
				    float alpha = 1.0f ) : r(red), g(green), b(blue), a(alpha)
        { }
		explicit Color4( Vector4 v) : r(v.x), g(v.y), b(v.z), a(v.w)
		{ }
		explicit Color4( Vector3 v) : r(v.x), g(v.y), b(v.z), a(1)
		{ }

		const Color4& operator=( const Vector4& v)
		{
			memcpy(val,&v,sizeof(Vector4));

			return *this;
		}

		const Color4& operator=( const Vector3& v)
		{
			memcpy(val,&v,sizeof(Vector3));
			a = 1;

			return *this;
		}

		const Color4& operator=( const Color3& v);

	    bool operator==(const Color4& rhs) const;
	    bool operator!=(const Color4& rhs) const;

        union {
            struct {
	            float r,g,b,a;
            };
            float val[4];
        };

	    /** Retrieves colour as RGBA.
	    */
	    RGBA getAsRGBA(void) const;

	    /** Retrieves colour as ARGB.
	    */
	    ARGB getAsARGB(void) const;

	    /** Retrieves colours as ABGR */
	    ABGR getAsABGR(void) const;

	    /** Sets colour as RGBA.
	    */
        void setAsRGBA(const RGBA val);

	    /** Sets colour as ARGB.
	    */
        void setAsARGB(const ARGB val);

	    /** Sets colour as ABGR.
	    */
        void setAsABGR(const ABGR val);

        // arithmetic operations
        inline Color4 operator + ( const Color4& rkVector ) const
        {
            Color4 kSum;

            kSum.r = r + rkVector.r;
            kSum.g = g + rkVector.g;
            kSum.b = b + rkVector.b;
            kSum.a = a + rkVector.a;

            return kSum;
        }

        inline Color4 operator - ( const Color4& rkVector ) const
        {
            Color4 kDiff;

            kDiff.r = r - rkVector.r;
            kDiff.g = g - rkVector.g;
            kDiff.b = b - rkVector.b;
            kDiff.a = a - rkVector.a;

            return kDiff;
        }

        inline Color4 operator * (const float fScalar ) const
        {
            Color4 kProd;

            kProd.r = fScalar*r;
            kProd.g = fScalar*g;
            kProd.b = fScalar*b;
            kProd.a = fScalar*a;

            return kProd;
        }

        inline Color4 operator * ( const Color4& rhs) const
        {
            Color4 kProd;

            kProd.r = rhs.r * r;
            kProd.g = rhs.g * g;
            kProd.b = rhs.b * b;
            kProd.a = rhs.a * a;

            return kProd;
        }

        inline Color4 operator / ( const Color4& rhs) const
        {
            Color4 kProd;

            kProd.r = rhs.r / r;
            kProd.g = rhs.g / g;
            kProd.b = rhs.b / b;
            kProd.a = rhs.a / a;

            return kProd;
        }

        inline Color4 operator / (const float fScalar ) const
        {
            assert( fScalar != 0.0 );

            Color4 kDiv;

            float fInv = 1.0 / fScalar;
            kDiv.r = r * fInv;
            kDiv.g = g * fInv;
            kDiv.b = b * fInv;
            kDiv.a = a * fInv;

            return kDiv;
        }

        inline friend Color4 operator * (const float fScalar, const Color4& rkVector )
        {
            Color4 kProd;

            kProd.r = fScalar * rkVector.r;
            kProd.g = fScalar * rkVector.g;
            kProd.b = fScalar * rkVector.b;
            kProd.a = fScalar * rkVector.a;

            return kProd;
        }

        // arithmetic updates
        inline Color4& operator += ( const Color4& rkVector )
        {
            r += rkVector.r;
            g += rkVector.g;
            b += rkVector.b;
            a += rkVector.a;

            return *this;
        }

        inline Color4& operator -= ( const Color4& rkVector )
        {
            r -= rkVector.r;
            g -= rkVector.g;
            b -= rkVector.b;
            a -= rkVector.a;

            return *this;
        }

        inline Color4& operator *= (const float fScalar )
        {
            r *= fScalar;
            g *= fScalar;
            b *= fScalar;
            a *= fScalar;
            return *this;
        }

        inline Color4& operator /= (const float fScalar )
        {
            assert( fScalar != 0.0 );

            float fInv = 1.0 / fScalar;

            r *= fInv;
            g *= fInv;
            b *= fInv;
            a *= fInv;

            return *this;
        }
    };

	typedef Color4 ColorValue;
	/** @} */

} // namespace

#endif
