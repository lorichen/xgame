#ifndef __Math_H__
#define __Math_H__

#include "ReMathPrerequisite.h"
#include "ReFPU.H"
#include "Nvidia_fastmath.h"

namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/
	
    /** Wrapper class which indicates a given angle value is in Radians.
    @remarks
        Radian values are interchangeable with Degree values, and conversions
        will be done automatically between them.
    */
	class _ReMathExport Radian
	{
		float mRad;

	public:
		explicit Radian ( float r=0 ) : mRad(r) {}
		Radian ( const Degree& d );
		const Radian& operator = ( const float& f ) { mRad = f; return *this; }
		const Radian& operator = ( const Radian& r ) { mRad = r.mRad; return *this; }
		const Radian& operator = ( const Degree& d );

		float valueDegrees() const; // see bottom of this file
		float valueRadians() const { return mRad; }
		float valueAngleUnits() const;

		Radian operator + ( const Radian& r ) const { return Radian ( mRad + r.mRad ); }
		Radian operator + ( const Degree& d ) const;
		Radian& operator += ( const Radian& r ) { mRad += r.mRad; return *this; }
		Radian& operator += ( const Degree& d );
		Radian operator - () { return Radian(-mRad); }
		Radian operator - ( const Radian& r ) const { return Radian ( mRad - r.mRad ); }
		Radian operator - ( const Degree& d ) const;
		Radian& operator -= ( const Radian& r ) { mRad -= r.mRad; return *this; }
		Radian& operator -= ( const Degree& d );
		Radian operator * ( float f ) const { return Radian ( mRad * f ); }
        Radian operator * ( const Radian& f ) const { return Radian ( mRad * f.mRad ); }
		Radian& operator *= ( float f ) { mRad *= f; return *this; }
		Radian operator / ( float f ) const { return Radian ( mRad / f ); }
		Radian& operator /= ( float f ) { mRad /= f; return *this; }

		bool operator <  ( const Radian& r ) const { return mRad <  r.mRad; }
		bool operator <= ( const Radian& r ) const { return mRad <= r.mRad; }
		bool operator == ( const Radian& r ) const { return mRad == r.mRad; }
		bool operator != ( const Radian& r ) const { return mRad != r.mRad; }
		bool operator >= ( const Radian& r ) const { return mRad >= r.mRad; }
		bool operator >  ( const Radian& r ) const { return mRad >  r.mRad; }
	};

    /** Wrapper class which indicates a given angle value is in Degrees.
    @remarks
        Degree values are interchangeable with Radian values, and conversions
        will be done automatically between them.
    */
	class _ReMathExport Degree
	{
		float mDeg; // if you get an error here - make sure to define/typedef 'float' first

	public:
		explicit Degree ( float d=0 ) : mDeg(d) {}
		Degree ( const Radian& r ) : mDeg(r.valueDegrees()) {}
		const Degree& operator = ( const float& f ) { mDeg = f; return *this; }
		const Degree& operator = ( const Degree& d ) { mDeg = d.mDeg; return *this; }
		const Degree& operator = ( const Radian& r ) { mDeg = r.valueDegrees(); return *this; }

		float valueDegrees() const { return mDeg; }
		float valueRadians() const; // see bottom of this file
		float valueAngleUnits() const;

		Degree operator + ( const Degree& d ) const { return Degree ( mDeg + d.mDeg ); }
		Degree operator + ( const Radian& r ) const { return Degree ( mDeg + r.valueDegrees() ); }
		Degree& operator += ( const Degree& d ) { mDeg += d.mDeg; return *this; }
		Degree& operator += ( const Radian& r ) { mDeg += r.valueDegrees(); return *this; }
		Degree operator - () { return Degree(-mDeg); }
		Degree operator - ( const Degree& d ) const { return Degree ( mDeg - d.mDeg ); }
		Degree operator - ( const Radian& r ) const { return Degree ( mDeg - r.valueDegrees() ); }
		Degree& operator -= ( const Degree& d ) { mDeg -= d.mDeg; return *this; }
		Degree& operator -= ( const Radian& r ) { mDeg -= r.valueDegrees(); return *this; }
		Degree operator * ( float f ) const { return Degree ( mDeg * f ); }
        Degree operator * ( const Degree& f ) const { return Degree ( mDeg * f.mDeg ); }
		Degree& operator *= ( float f ) { mDeg *= f; return *this; }
		Degree operator / ( float f ) const { return Degree ( mDeg / f ); }
		Degree& operator /= ( float f ) { mDeg /= f; return *this; }

		bool operator <  ( const Degree& d ) const { return mDeg <  d.mDeg; }
		bool operator <= ( const Degree& d ) const { return mDeg <= d.mDeg; }
		bool operator == ( const Degree& d ) const { return mDeg == d.mDeg; }
		bool operator != ( const Degree& d ) const { return mDeg != d.mDeg; }
		bool operator >= ( const Degree& d ) const { return mDeg >= d.mDeg; }
		bool operator >  ( const Degree& d ) const { return mDeg >  d.mDeg; }
	};

    /** Wrapper class which identifies a value as the currently default angle 
        type, as defined by Math::setAngleUnit.
    @remarks
        Angle values will be automatically converted between radians and degrees,
        as appropriate.
    */
	class _ReMathExport Angle
	{
		float mAngle;
	public:
		explicit Angle ( float angle ) : mAngle(angle) {}
		operator Radian();
		operator Degree();
	};

	// these functions could not be defined within the class definition of class
	// Radian because they required class Degree to be defined
	inline Radian::Radian ( const Degree& d ) : mRad(d.valueRadians()) {
	}
	inline const Radian& Radian::operator = ( const Degree& d ) {
		mRad = d.valueRadians(); return *this;
	}
	inline Radian Radian::operator + ( const Degree& d ) const {
		return Radian ( mRad + d.valueRadians() );
	}
	inline Radian& Radian::operator += ( const Degree& d ) {
		mRad += d.valueRadians();
		return *this;
	}
	inline Radian Radian::operator - ( const Degree& d ) const {
		return Radian ( mRad - d.valueRadians() );
	}
	inline Radian& Radian::operator -= ( const Degree& d ) {
		mRad -= d.valueRadians();
		return *this;
	}

    /** Class to provide access to common mathematical functions.
        @remarks
            Most of the maths functions are aliased versions of the C runtime
            library functions. They are aliased here to provide future
            optimisation opportunities, either from faster RTLs or custom
            math approximations.
        @note
            <br>This is based on MgcMath.h from
            <a href="http://www.magic-software.com">Wild Magic</a>.
    */
    class _ReMathExport Math 
    {
   public:
       /** The angular units used by the API. This functionality is now deprecated in favor
	       of discreet angular unit types ( see Degree and Radian above ). The only place
		   this functionality is actually still used is when parsing files. Search for
		   usage of the Angle class for those instances
       */
       enum AngleUnit
       {
           AU_DEGREE,
           AU_RADIAN
       };

    protected:
       // angle units used by the api
       static AngleUnit msAngleUnit;

        /// Size of the trig tables as determined by constructor.
        static int mTrigTableSize;

        /// Radian -> index factor value ( mTrigTableSize / 2 * PI )
        static float mTrigTableFactor;
        static float* mSinTable;
        static float* mTanTable;

        /** Private function to build trig tables.
        */
        void buildTrigTables();

		static float SinTable (float fValue);
		static float TanTable (float fValue);
    public:
        /** Default constructor.
            @param
                trigTableSize Optional parameter to set the size of the
                tables used to implement Sin, Cos, Tan
        */
        Math(unsigned int trigTableSize = 4096);

        /** Default destructor.
        */
        ~Math();

		static inline int IAbs (int iValue) { return ( iValue >= 0 ? iValue : -iValue ); }
		static inline int ICeil (float fValue) { return int(ceil(fValue)); }
		static inline int IFloor (float fValue) { return int(floor(fValue)); }
        static int ISign (int iValue);

		static inline float Abs (float fValue) { return float(fabs(fValue)); }
		static inline Degree Abs (const Degree& dValue) { return Degree(fabs(dValue.valueDegrees())); }
		static inline Radian Abs (const Radian& rValue) { return Radian(fabs(rValue.valueRadians())); }
		static Radian ACos (float fValue);
		static Radian ASin (float fValue);
		static inline Radian ATan (float fValue) { return Radian(atan(fValue)); }
		static inline Radian ATan2 (float fY, float fX) { return Radian(atan2(fY,fX)); }
		static inline float Ceil (float fValue) { return float(ceil(fValue)); }

        /** Cosine function.
            @param
                fValue Angle in radians
            @param
                useTables If true, uses lookup tables rather than
                calculation - faster but less accurate.
        */
        static inline float Cos (const Radian& fValue, bool useTables = false) {
			return (!useTables) ? float(cos(fValue.valueRadians())) : SinTable(fValue.valueRadians() + HALF_PI);
		}
        /** Cosine function.
            @param
                fValue Angle in radians
            @param
                useTables If true, uses lookup tables rather than
                calculation - faster but less accurate.
        */
        static inline float Cos (float fValue, bool useTables = false) {
			return (!useTables) ? float(cos(fValue)) : SinTable(fValue + HALF_PI);
		}

		static inline float Exp (float fValue) { return float(exp(fValue)); }

		static inline float Floor (float fValue) { return float(floor(fValue)); }

		static inline int Round (float fValue) { if(fValue > 0)return IFloor(fValue + 0.5f);if(fValue < 0)return -IFloor(-fValue + 0.5f);return 0;}

		static inline float Log (float fValue) { return float(log(fValue)); }

		static inline float Pow (float fBase, float fExponent) { return float(pow(fBase,fExponent)); }

        static float Sign (float fValue);
		static inline Radian Sign ( const Radian& rValue )
		{
			return Radian(Sign(rValue.valueRadians()));
		}
		static inline Degree Sign ( const Degree& dValue )
		{
			return Degree(Sign(dValue.valueDegrees()));
		}

        /** Sine function.
            @param
                fValue Angle in radians
            @param
                useTables If true, uses lookup tables rather than
                calculation - faster but less accurate.
        */
        static inline float Sin (const Radian& fValue, bool useTables = false) {
			return (!useTables) ? float(sin(fValue.valueRadians())) : SinTable(fValue.valueRadians());
		}
        /** Sine function.
            @param
                fValue Angle in radians
            @param
                useTables If true, uses lookup tables rather than
                calculation - faster but less accurate.
        */
        static inline float Sin (float fValue, bool useTables = false) {
			return (!useTables) ? float(sin(fValue)) : SinTable(fValue);
		}

		static inline float Sqr (float fValue) { return fValue*fValue; }
//函数调用的开销非常大，直接调用sqrt会省很多时间，考虑去掉函数调用，但代码就有点乱
//#define Sqrt(f) sqrt(f)
		static inline float Sqrt (float fValue) { return float(sqrt(fValue)); }

        static inline Radian Sqrt (const Radian& fValue) { return Radian(sqrt(fValue.valueRadians())); }

        static inline Degree Sqrt (const Degree& fValue) { return Degree(sqrt(fValue.valueDegrees())); }
//#define SqrtF(f) __fsqrt(f)
//#define SqrtF_Table(f) fastsqrt(f)
		static inline float SqrtF (float fValue) { return float(__fsqrt(fValue)); }

		static inline Radian SqrtF (const Radian& fValue) { return Radian(__fsqrt(fValue.valueRadians())); }

		static inline Degree SqrtF (const Degree& fValue) { return Degree(__fsqrt(fValue.valueDegrees())); }

		static inline float SqrtF_Table (float fValue) { return float(fastsqrt(fValue)); }

		static inline Radian SqrtF_Table (const Radian& fValue) { return Radian(fastsqrt(fValue.valueRadians())); }

		static inline Degree SqrtF_Table (const Degree& fValue) { return Degree(fastsqrt(fValue.valueDegrees())); }

        /** Inverse square root i.e. 1 / Sqrt(x), good for vector
            normalisation.
        */
		static float InvSqrt(float fValue);

        static float UnitRandom ();  // in [0,1]

        static float RangeRandom (float fLow, float fHigh);  // in [fLow,fHigh]

        static float SymmetricRandom ();  // in [-1,1]

        /** Tangent function.
            @param
                fValue Angle in radians
            @param
                useTables If true, uses lookup tables rather than
                calculation - faster but less accurate.
        */
		static inline float Tan (const Radian& fValue, bool useTables = false) {
			return (!useTables) ? float(tan(fValue.valueRadians())) : TanTable(fValue.valueRadians());
		}
        /** Tangent function.
            @param
                fValue Angle in radians
            @param
                useTables If true, uses lookup tables rather than
                calculation - faster but less accurate.
        */
		static inline float Tan (float fValue, bool useTables = false) {
			return (!useTables) ? float(tan(fValue)) : TanTable(fValue);
		}

		static inline float DegreesToRadians(float degrees) { return degrees * fDeg2Rad; }
        static inline float RadiansToDegrees(float radians) { return radians * fRad2Deg; }

       /** These functions used to set the assumed angle units (radians or degrees) 
            expected when using the Angle type.
       @par
            You can set this directly after creating a new Root, and also before/after resource creation,
            depending on whether you want the change to affect resource files.
       */
       static void setAngleUnit(AngleUnit unit);
       /** Get the unit being used for angles. */
       static AngleUnit getAngleUnit(void);

       /** Convert from the current AngleUnit to radians. */
       static float AngleUnitsToRadians(float units);
       /** Convert from radians to the current AngleUnit . */
       static float RadiansToAngleUnits(float radians);
       /** Convert from the current AngleUnit to degrees. */
       static float AngleUnitsToDegrees(float units);
       /** Convert from degrees to the current AngleUnit. */
       static float DegreesToAngleUnits(float degrees);

       /** Checks wether a given point is inside a triangle, in a
            2-dimensional (Cartesian) space.
            @remarks
                The vertices of the triangle must be given in either
                trigonometrical (anticlockwise) or inverse trigonometrical
                (clockwise) order.
            @param
                px The X-coordinate of the point.
            @param
                py The Y-coordinate of the point.
            @param
                ax The X-coordinate of the triangle's first vertex.
            @param
                ay The Y-coordinate of the triangle's first vertex.
            @param
                bx The X-coordinate of the triangle's second vertex.
            @param
                by The Y-coordinate of the triangle's second vertex.
            @param
                cx The X-coordinate of the triangle's third vertex.
            @param
                cy The Y-coordinate of the triangle's third vertex.
            @returns
                If the point resides in the triangle, <b>true</b> is
                returned.
            @par
                If the point is outside the triangle, <b>false</b> is
                returned.
        */
        static bool pointInTri2D( float px, float pz, float ax, float az, float bx, float bz, float cx, float cz );

        /** Ray / plane intersection, returns boolean result and distance. */
        static std::pair<bool, float> intersects(const Ray& ray, const Plane& plane);

        /** Ray / sphere intersection, returns boolean result and distance. */
        static std::pair<bool, float> intersects(const Ray& ray, const Sphere& sphere, 
            bool discardInside = true);
        
        /** Ray / box intersection, returns boolean result and distance. */
        static std::pair<bool, float> intersects(const Ray& ray, const AABB& sphere);

        /** Sphere / box intersection test. */
        static bool intersects(const Sphere& sphere, const AABB& box);

        /** Plane / box intersection test. */
        static bool intersects(const Plane& plane, const AABB& box);

        /** Ray / convex plane list intersection test. 
        @param ray The ray to test with
        @param plaeList List of planes which form a convex volume
        @param normalIsOutside Does the normal point outside the volume
        */
        static std::pair<bool, float> intersects(
            const Ray& ray, const std::vector<Plane>& planeList, 
            bool normalIsOutside);
        /** Ray / convex plane list intersection test. 
        @param ray The ray to test with
        @param plaeList List of planes which form a convex volume
        @param normalIsOutside Does the normal point outside the volume
        */
        static std::pair<bool, float> intersects(
            const Ray& ray, const std::list<Plane>& planeList, 
            bool normalIsOutside);

        /** Sphere / plane intersection test. 
        @remarks NB just do a plane.getDistance(sphere.getCenter()) for more detail!
        */
        static bool intersects(const Sphere& sphere, const Plane& plane);

        /** Compare 2 reals, using tolerance for inaccuracies.
        */
        static bool RealEqual(float a, float b,
            float tolerance = std::numeric_limits<float>::epsilon());

        /** Calculates the tangent space vector for a given set of positions / texture coords. */
        static Vector3 calculateTangentSpaceVector(
            const Vector3& position1, const Vector3& position2, const Vector3& position3,
            float u1, float v1, float u2, float v2, float u3, float v3);

        /** Build a reflection matrix for the passed in plane. */
        static Matrix4 buildReflectionMatrix(const Plane& p);
        /** Calculate a face normal, including the w component which is the offset from the origin. */
        static Vector4 calculateFaceNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3);
        /** Calculate a face normal, no w-information. */
        static Vector3 calculateBasicFaceNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3);
        /** Calculate a face normal without normalize, including the w component which is the offset from the origin. */
        static Vector4 calculateFaceNormalWithoutNormalize(const Vector3& v1, const Vector3& v2, const Vector3& v3);
        /** Calculate a face normal without normalize, no w-information. */
        static Vector3 calculateBasicFaceNormalWithoutNormalize(const Vector3& v1, const Vector3& v2, const Vector3& v3);

        static const float POS_INFINITY;
        static const float NEG_INFINITY;
        static const float PI;
        static const float TWO_PI;
        static const float HALF_PI;
		static const float fDeg2Rad;
		static const float fRad2Deg;
		static const float fSqrt2;
		static const float fSqrt3;
		static const float fSqrt5;

		///This function gets the first power of 2 >= the
		///int that we pass it.
		static uint NextP2(uint n)
		{
			uint rval=1;
			while(rval < n) rval<<=1;
			return rval;
		}
		
		///This function gets the last power of 2 <= the
		///int that we pass it.
		///n must >= 1
		static inline uint LastP2(uint n)
		{
			uint rval=1;
			while(rval < n) rval<<=1;
			return rval == n ? rval : rval >> 1;
		}

		/** Returns a reference to a static double.
		This value should not be tested against directly, instead
		G3D::isNan() and G3D::isFinite() will return reliable results. */
		inline static const float& inf() {

			// We already have <limits> included but
			// not using it in older gcc for safe compilations
#if (__GNUC__ == 2)    
			static const float i = 1.0/sin(0.0);
#else
			// double is a standard type and should have infinity
			static const float i = std::numeric_limits<float>::infinity();
#endif
			return i;
		}

		inline static void clamp(int& x,int mini,int maxi)
		{
			if(x > maxi) {x = maxi;return;}
			if(x < mini) {x = mini;return;}
		}
		inline static void clamp(float& x,float mini,float maxi)
		{
			if(x > maxi) {x = maxi;return;}
			if(x < mini) {x = mini;return;}
		}
    };

	// these functions must be defined down here, because they rely on the
	// angle unit conversion functions in class Math:

	inline float Radian::valueDegrees() const
	{
		return Math::RadiansToDegrees ( mRad );
	}

	inline float Radian::valueAngleUnits() const
	{
		return Math::RadiansToAngleUnits ( mRad );
	}

	inline float Degree::valueRadians() const
	{
		return Math::DegreesToRadians ( mDeg );
	}

	inline float Degree::valueAngleUnits() const
	{
		return Math::DegreesToAngleUnits ( mDeg );
	}

	inline Angle::operator Radian()
	{
		return Radian(Math::AngleUnitsToRadians(mAngle));
	}

	inline Angle::operator Degree()
	{
		return Degree(Math::AngleUnitsToDegrees(mAngle));
	}

	inline Radian operator * ( float a, const Radian& b )
	{
		return Radian ( a * b.valueRadians() );
	}

	inline Radian operator / ( float a, const Radian& b )
	{
		return Radian ( a / b.valueRadians() );
	}

	inline Degree operator * ( float a, const Degree& b )
	{
		return Degree ( a * b.valueDegrees() );
	}

	inline Degree operator / ( float a, const Degree& b )
	{
		return Degree ( a / b.valueDegrees() );
	}
	/** @} */

}
#endif
