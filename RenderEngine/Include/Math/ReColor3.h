#ifndef __Color3_H__
#define __Color3_H__
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
	class _ReMathExport Color3
	{
	public:
		static Color3 Black;
		static Color3 White;
		static Color3 Red;
		static Color3 Green;
		static Color3 Blue;

		explicit Color3( float red = 1.0f,
			float green = 1.0f,
			float blue = 1.0f ) : r(red), g(green), b(blue)
		{ }
		explicit Color3( Vector4 v) : r(v.x), g(v.y), b(v.z)
		{ }
		explicit Color3( Vector3 v) : r(v.x), g(v.y), b(v.z)
		{ }

		const Color3& operator=( const Vector4& v)
		{
			memcpy(val,&v,sizeof(Vector3));

			return *this;
		}

		const Color3& operator=( const Vector3& v)
		{
			memcpy(val,&v,sizeof(Vector3));

			return *this;
		}

		const Color3& operator=( const Color4& v)
		{
			memcpy(val,&v,sizeof(Color3));

			return *this;
		}

		bool operator==(const Color3& rhs) const;
		bool operator!=(const Color3& rhs) const;

		union {
			struct {
				float r,g,b;
			};
			float val[3];
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
		inline Color3 operator + ( const Color3& rkVector ) const
		{
			Color3 kSum;

			kSum.r = r + rkVector.r;
			kSum.g = g + rkVector.g;
			kSum.b = b + rkVector.b;

			return kSum;
		}

		inline Color3 operator - ( const Color3& rkVector ) const
		{
			Color3 kDiff;

			kDiff.r = r - rkVector.r;
			kDiff.g = g - rkVector.g;
			kDiff.b = b - rkVector.b;

			return kDiff;
		}

		inline Color3 operator * (const float fScalar ) const
		{
			Color3 kProd;

			kProd.r = fScalar*r;
			kProd.g = fScalar*g;
			kProd.b = fScalar*b;

			return kProd;
		}

		inline Color3 operator * ( const Color3& rhs) const
		{
			Color3 kProd;

			kProd.r = rhs.r * r;
			kProd.g = rhs.g * g;
			kProd.b = rhs.b * b;

			return kProd;
		}

		inline Color3 operator / ( const Color3& rhs) const
		{
			Color3 kProd;

			kProd.r = rhs.r / r;
			kProd.g = rhs.g / g;
			kProd.b = rhs.b / b;

			return kProd;
		}

		inline Color3 operator / (const float fScalar ) const
		{
			assert( fScalar != 0.0 );

			Color3 kDiv;

			float fInv = 1.0 / fScalar;
			kDiv.r = r * fInv;
			kDiv.g = g * fInv;
			kDiv.b = b * fInv;

			return kDiv;
		}

		inline friend Color3 operator * (const float fScalar, const Color3& rkVector )
		{
			Color3 kProd;

			kProd.r = fScalar * rkVector.r;
			kProd.g = fScalar * rkVector.g;
			kProd.b = fScalar * rkVector.b;

			return kProd;
		}

		// arithmetic updates
		inline Color3& operator += ( const Color3& rkVector )
		{
			r += rkVector.r;
			g += rkVector.g;
			b += rkVector.b;

			return *this;
		}

		inline Color3& operator -= ( const Color3& rkVector )
		{
			r -= rkVector.r;
			g -= rkVector.g;
			b -= rkVector.b;

			return *this;
		}

		inline Color3& operator *= (const float fScalar )
		{
			r *= fScalar;
			g *= fScalar;
			b *= fScalar;
			return *this;
		}

		inline Color3& operator /= (const float fScalar )
		{
			assert( fScalar != 0.0 );

			float fInv = 1.0 / fScalar;

			r *= fInv;
			g *= fInv;
			b *= fInv;

			return *this;
		}
	};
	/** @} */

} // namespace

#endif
