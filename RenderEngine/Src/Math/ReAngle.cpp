#include "ReMathPch.h"

#include "ReAngle.h"
#include "ReAsmMath.h"
#include "ReFPU.h"
#include "ReVector3.h"
#include "ReVector4.h"
#include "ReRay.h"
#include "ReSphere.h"
#include "ReAxisAlignedBox.h"
#include "RePlane.h"


namespace xs
{

    const float Math::POS_INFINITY = std::numeric_limits<float>::infinity();
    const float Math::NEG_INFINITY = -std::numeric_limits<float>::infinity();
    const float Math::PI = float( 4.0 * atan( 1.0 ) );
    const float Math::TWO_PI = float( 2.0 * PI );
    const float Math::HALF_PI = float( 0.5 * PI );
	const float Math::fDeg2Rad = PI / float(180.0);
	const float Math::fRad2Deg = float(180.0) / PI;
	const float Math::fSqrt2 = 1.4142136f;
	const float Math::fSqrt3 = 1.7320508f;
	const float Math::fSqrt5 = 2.2360680f;

    int Math::mTrigTableSize;
	Math::AngleUnit Math::msAngleUnit;

    float  Math::mTrigTableFactor;
    float *Math::mSinTable = NULL;
    float *Math::mTanTable = NULL;
	Math	g_Math;

    //-----------------------------------------------------------------------
    Math::Math( unsigned int trigTableSize )
    {
        msAngleUnit = AU_DEGREE;

        mTrigTableSize = trigTableSize;
        mTrigTableFactor = mTrigTableSize / Math::TWO_PI;

        mSinTable = new float[mTrigTableSize];
        mTanTable = new float[mTrigTableSize];

        buildTrigTables();
		build_sqrt_table();
    }

    //-----------------------------------------------------------------------
    Math::~Math()
    {
        delete [] mSinTable;
        delete [] mTanTable;
    }

    //-----------------------------------------------------------------------
    void Math::buildTrigTables(void)
    {
        // Build trig lookup tables
        // Could get away with building only PI sized Sin table but simpler this 
        // way. Who cares, it'll ony use an extra 8k of memory anyway and I like 
        // simplicity.
        float angle;
        for (int i = 0; i < mTrigTableSize; ++i)
        {
            angle = Math::TWO_PI * i / mTrigTableSize;
            mSinTable[i] = sin(angle);
            mTanTable[i] = tan(angle);
        }
    }
	//-----------------------------------------------------------------------	
	float Math::SinTable (float fValue)
    {
        // Convert range to index values, wrap if required
        int idx;
        if (fValue >= 0)
        {
            idx = int(fValue * mTrigTableFactor) % mTrigTableSize;
        }
        else
        {
            idx = mTrigTableSize - (int(-fValue * mTrigTableFactor) % mTrigTableSize) - 1;
        }

        return mSinTable[idx];
    }
	//-----------------------------------------------------------------------
	float Math::TanTable (float fValue)
    {
        // Convert range to index values, wrap if required
		int idx = int(fValue *= mTrigTableFactor) % mTrigTableSize;
		return mTanTable[idx];
    }
    //-----------------------------------------------------------------------
    int Math::ISign (int iValue)
    {
        return ( iValue > 0 ? +1 : ( iValue < 0 ? -1 : 0 ) );
    }
    //-----------------------------------------------------------------------
    Radian Math::ACos (float fValue)
    {
        if ( -1.0 < fValue )
        {
            if ( fValue < 1.0 )
                return Radian(acos(fValue));
            else
                return Radian(0.0);
        }
        else
        {
            return Radian(PI);
        }
    }
    //-----------------------------------------------------------------------
    Radian Math::ASin (float fValue)
    {
        if ( -1.0 < fValue )
        {
            if ( fValue < 1.0 )
                return Radian(asin(fValue));
            else
                return Radian(-HALF_PI);
        }
        else
        {
            return Radian(HALF_PI);
        }
    }
    //-----------------------------------------------------------------------
    float Math::Sign (float fValue)
    {
        if ( fValue > 0.0 )
            return 1.0;

        if ( fValue < 0.0 )
            return -1.0;

        return 0.0;
    }
	//-----------------------------------------------------------------------
	float Math::InvSqrt(float fValue)
	{
		return float(asm_rsq(fValue));
	}
    //-----------------------------------------------------------------------
    float Math::UnitRandom ()
    {
        return asm_rand() / asm_rand_max();
    }
    
    //-----------------------------------------------------------------------
    float Math::RangeRandom (float fLow, float fHigh)
    {
        return (fHigh-fLow)*UnitRandom() + fLow;
    }

    //-----------------------------------------------------------------------
    float Math::SymmetricRandom ()
    {
		return 2.0f * UnitRandom() - 1.0f;
    }

   //-----------------------------------------------------------------------
    void Math::setAngleUnit(Math::AngleUnit unit)
   {
       msAngleUnit = unit;
   }
   //-----------------------------------------------------------------------
   Math::AngleUnit Math::getAngleUnit(void)
   {
       return msAngleUnit;
   }
    //-----------------------------------------------------------------------
    float Math::AngleUnitsToRadians(float angleunits)
    {
       if (msAngleUnit == AU_DEGREE)
           return angleunits * fDeg2Rad;
       else
           return angleunits;
    }

    //-----------------------------------------------------------------------
    float Math::RadiansToAngleUnits(float radians)
    {
       if (msAngleUnit == AU_DEGREE)
           return radians * fRad2Deg;
       else
           return radians;
    }

    //-----------------------------------------------------------------------
    float Math::AngleUnitsToDegrees(float angleunits)
    {
       if (msAngleUnit == AU_RADIAN)
           return angleunits * fRad2Deg;
       else
           return angleunits;
    }

    //-----------------------------------------------------------------------
    float Math::DegreesToAngleUnits(float degrees)
    {
       if (msAngleUnit == AU_RADIAN)
           return degrees * fDeg2Rad;
       else
           return degrees;
    }

    //-----------------------------------------------------------------------
    bool Math::pointInTri2D( float px, float py, float ax, float ay, float bx, float by, float cx, float cy )
    {
        float v1x, v2x, v1y, v2y;
        bool bClockwise;

        v1x = bx - ax;
        v1y = by - ay;

        v2x = px - bx;
        v2y = py - by;

        // For the sake of readability
        #define Clockwise ( v1x * v2y - v1y * v2x >= 0.0 )

        bClockwise = Clockwise;

        v1x = cx - bx;
        v1y = cy - by;

        v2x = px - cx;
        v2y = py - cy;

        if( Clockwise != bClockwise )
            return false;

        v1x = ax - cx;
        v1y = ay - cy;

        v2x = px - ax;
        v2y = py - ay;

        if( Clockwise != bClockwise )
            return false;

        return true;

        // Clean up the #defines
        #undef Clockwise
    }

    //-----------------------------------------------------------------------
    bool Math::RealEqual( float a, float b, float tolerance )
    {
        if (fabs(b-a) <= tolerance)
            return true;
        else
            return false;
    }

    //-----------------------------------------------------------------------
    std::pair<bool, float> Math::intersects(const Ray& ray, const Plane& plane)
    {

        float denom = plane.normal.dotProduct(ray.getDirection());
        if (Math::Abs(denom) < std::numeric_limits<float>::epsilon())
        {
            // Parallel
            return std::pair<bool, float>(false, 0);
        }
        else
        {
            float nom = plane.normal.dotProduct(ray.getOrigin()) + plane.d;
            float t = -(nom/denom);
            return std::pair<bool, float>(t >= 0, t);
        }
        
    }
    //-----------------------------------------------------------------------
    std::pair<bool, float> Math::intersects(const Ray& ray, 
        const std::vector<Plane>& planes, bool normalIsOutside)
    {
        std::vector<Plane>::const_iterator planeit, planeitend;
        planeitend = planes.end();
        bool allInside = true;
        std::pair<bool, float> ret;
        ret.first = false;
        ret.second = 0.0f;

        // derive side
        // NB we don't pass directly since that would require Plane::Side in 
        // interface, which results in recursive includes since Math is so fundamental
        Plane::Side outside = normalIsOutside ? Plane::POSITIVE_SIDE : Plane::NEGATIVE_SIDE;

        for (planeit = planes.begin(); planeit != planeitend; ++planeit)
        {
            const Plane& plane = *planeit;
            // is origin outside?
            if (plane.getSide(ray.getOrigin()) == outside)
            {
                allInside = false;
                // Test single plane
                std::pair<bool, float> planeRes = 
                    ray.intersects(plane);
                if (planeRes.first)
                {
                    // Ok, we intersected
                    ret.first = true;
                    // Use the most distant result since convex volume
					ret.second = std::max(ret.second, planeRes.second);
                }
            }
        }

        if (allInside)
        {
            // Intersecting at 0 distance since inside the volume!
            ret.first = true;
            ret.second = 0.0f;
        }

        return ret;
    }
    //-----------------------------------------------------------------------
    std::pair<bool, float> Math::intersects(const Ray& ray, 
        const std::list<Plane>& planes, bool normalIsOutside)
    {
        std::list<Plane>::const_iterator planeit, planeitend;
        planeitend = planes.end();
        bool allInside = true;
        std::pair<bool, float> ret;
        ret.first = false;
        ret.second = 0.0f;

        // derive side
        // NB we don't pass directly since that would require Plane::Side in 
        // interface, which results in recursive includes since Math is so fundamental
        Plane::Side outside = normalIsOutside ? Plane::POSITIVE_SIDE : Plane::NEGATIVE_SIDE;

        for (planeit = planes.begin(); planeit != planeitend; ++planeit)
        {
            const Plane& plane = *planeit;
            // is origin outside?
            if (plane.getSide(ray.getOrigin()) == outside)
            {
                allInside = false;
                // Test single plane
                std::pair<bool, float> planeRes = 
                    ray.intersects(plane);
                if (planeRes.first)
                {
                    // Ok, we intersected
                    ret.first = true;
                    // Use the most distant result since convex volume
					ret.second = std::max(ret.second, planeRes.second);
                }
            }
        }

        if (allInside)
        {
            // Intersecting at 0 distance since inside the volume!
            ret.first = true;
            ret.second = 0.0f;
        }

        return ret;
    }
    //-----------------------------------------------------------------------
    std::pair<bool, float> Math::intersects(const Ray& ray, const Sphere& sphere, 
        bool discardInside)
    {
        const Vector3& raydir = ray.getDirection();
        // Adjust ray origin relative to sphere center
        const Vector3& rayorig = ray.getOrigin() - sphere.getCenter();
        float radius = sphere.getRadius();

        // Check origin inside first
        if (rayorig.squaredLength() <= radius*radius && discardInside)
        {
            return std::pair<bool, float>(true, 0);
        }

        // Mmm, quadratics
        // Build coeffs which can be used with std quadratic solver
        // ie t = (-b +/- sqrt(b*b + 4ac)) / 2a
        float a = raydir.dotProduct(raydir);
        float b = 2 * rayorig.dotProduct(raydir);
        float c = rayorig.dotProduct(rayorig) - radius*radius;

        // Calc determinant
        float d = (b*b) - (4 * a * c);
        if (d < 0)
        {
            // No intersection
            return std::pair<bool, float>(false, 0);
        }
        else
        {
            // BTW, if d=0 there is one intersection, if d > 0 there are 2
            // But we only want the closest one, so that's ok, just use the 
            // '-' version of the solver
            float t = ( -b - Math::Sqrt(d) ) / (2 * a);
            if (t < 0)
                t = ( -b + Math::Sqrt(d) ) / (2 * a);
            return std::pair<bool, float>(true, t);
        }


    }
    //-----------------------------------------------------------------------
    std::pair<bool, float> Math::intersects(const Ray& ray, const AABB& box)
    {
        if (box.isNull()) return std::pair<bool, float>(false, 0);

        float lowt = 0.0f;
        float t;
        bool hit = false;
        Vector3 hitpoint;
        const Vector3& min = box.getMinimum();
        const Vector3& max = box.getMaximum();
        const Vector3& rayorig = ray.getOrigin();
        const Vector3& raydir = ray.getDirection();

        // Check origin inside first
        if ( rayorig > min && rayorig < max )
        {
            return std::pair<bool, float>(true, 0);
        }

        // Check each face in turn, only check closest 3
        // Min x
        if (rayorig.x < min.x && raydir.x > 0)
        {
            t = (min.x - rayorig.x) / raydir.x;
            if (t > 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
                    hitpoint.z >= min.z && hitpoint.z <= max.z &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Max x
        if (rayorig.x > max.x && raydir.x < 0)
        {
            t = (max.x - rayorig.x) / raydir.x;
            if (t > 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
                    hitpoint.z >= min.z && hitpoint.z <= max.z &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Min y
        if (rayorig.y < min.y && raydir.y > 0)
        {
            t = (min.y - rayorig.y) / raydir.y;
            if (t > 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
                    hitpoint.z >= min.z && hitpoint.z <= max.z &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Max y
        if (rayorig.y > max.y && raydir.y < 0)
        {
            t = (max.y - rayorig.y) / raydir.y;
            if (t > 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
                    hitpoint.z >= min.z && hitpoint.z <= max.z &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Min z
        if (rayorig.z < min.z && raydir.z > 0)
        {
            t = (min.z - rayorig.z) / raydir.z;
            if (t > 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
                    hitpoint.y >= min.y && hitpoint.y <= max.y &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }
        // Max z
        if (rayorig.z > max.z && raydir.z < 0)
        {
            t = (max.z - rayorig.z) / raydir.z;
            if (t > 0)
            {
                // Substitute t back into ray and check bounds and dist
                hitpoint = rayorig + raydir * t;
                if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
                    hitpoint.y >= min.y && hitpoint.y <= max.y &&
                    (!hit || t < lowt))
                {
                    hit = true;
                    lowt = t;
                }
            }
        }

        return std::pair<bool, float>(hit, lowt);

    }
    //-----------------------------------------------------------------------
    bool Math::intersects(const Sphere& sphere, const AABB& box)
    {
        if (box.isNull()) return false;

        // Use splitting planes
        const Vector3& center = sphere.getCenter();
        float radius = sphere.getRadius();
        const Vector3& min = box.getMinimum();
        const Vector3& max = box.getMaximum();

        // just test facing planes, early fail if sphere is totally outside
        if (center.x < min.x && 
            min.x - center.x > radius)
        {
            return false;
        }
        if (center.x > max.x && 
            center.x  - max.x > radius)
        {
            return false;
        }

        if (center.y < min.y && 
            min.y - center.y > radius)
        {
            return false;
        }
        if (center.y > max.y && 
            center.y  - max.y > radius)
        {
            return false;
        }

        if (center.z < min.z && 
            min.z - center.z > radius)
        {
            return false;
        }
        if (center.z > max.z && 
            center.z  - max.z > radius)
        {
            return false;
        }

        // Must intersect
        return true;

    }
    //-----------------------------------------------------------------------
    bool Math::intersects(const Plane& plane, const AABB& box)
    {
        if (box.isNull()) return false;

        // Get corners of the box
        const Vector3* pCorners = box.getAllCorners();


        // Test which side of the plane the corners are
        // Intersection occurs when at least one corner is on the 
        // opposite side to another
        Plane::Side lastSide = plane.getSide(pCorners[0]);
        for (int corner = 1; corner < 8; ++corner)
        {
            if (plane.getSide(pCorners[corner]) != lastSide)
            {
                return true;
            }
        }

        return false;
    }
    //-----------------------------------------------------------------------
    bool Math::intersects(const Sphere& sphere, const Plane& plane)
    {
        return (
            Math::Abs(plane.normal.dotProduct(sphere.getCenter()))
            <= sphere.getRadius() );
    }
    //-----------------------------------------------------------------------
    Vector3 Math::calculateTangentSpaceVector(
        const Vector3& position1, const Vector3& position2, const Vector3& position3,
        float u1, float v1, float u2, float v2, float u3, float v3)
    {
	    //side0 is the vector along one side of the triangle of vertices passed in, 
	    //and side1 is the vector along another side. Taking the cross product of these returns the normal.
	    Vector3 side0 = position1 - position2;
	    Vector3 side1 = position3 - position1;
	    //Calculate face normal
	    Vector3 normal = side1.crossProduct(side0);
	    normal.normalize();
	    //Now we use a formula to calculate the tangent. 
	    float deltaV0 = v1 - v2;
	    float deltaV1 = v3 - v1;
	    Vector3 tangent = deltaV1 * side0 - deltaV0 * side1;
	    tangent.normalize();
	    //Calculate binormal
	    float deltaU0 = u1 - u2;
	    float deltaU1 = u3 - u1;
	    Vector3 binormal = deltaU1 * side0 - deltaU0 * side1;
	    binormal.normalize();
	    //Now, we take the cross product of the tangents to get a vector which 
	    //should point in the same direction as our normal calculated above. 
	    //If it points in the opposite direction (the dot product between the normals is less than zero), 
	    //then we need to reverse the s and t tangents. 
	    //This is because the triangle has been mirrored when going from tangent space to object space.
	    //reverse tangents if necessary
	    Vector3 tangentCross = tangent.crossProduct(binormal);
	    if (tangentCross.dotProduct(normal) < 0.0f)
	    {
		    tangent = -tangent;
		    binormal = -binormal;
	    }

        return tangent;

    }
    //-----------------------------------------------------------------------
    Matrix4 Math::buildReflectionMatrix(const Plane& p)
    {
        return Matrix4(
            -2 * p.normal.x * p.normal.x + 1,   -2 * p.normal.x * p.normal.y,       -2 * p.normal.x * p.normal.z,       -2 * p.normal.x * p.d, 
            -2 * p.normal.y * p.normal.x,       -2 * p.normal.y * p.normal.y + 1,   -2 * p.normal.y * p.normal.z,       -2 * p.normal.y * p.d, 
            -2 * p.normal.z * p.normal.x,       -2 * p.normal.z * p.normal.y,       -2 * p.normal.z * p.normal.z + 1,   -2 * p.normal.z * p.d, 
            0,                                  0,                                  0,                                  1);
    }
    //-----------------------------------------------------------------------
    Vector4 Math::calculateFaceNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3)
    {
        Vector3 normal = calculateBasicFaceNormal(v1, v2, v3);
        // Now set up the w (distance of tri from origin
        return Vector4(normal.x, normal.y, normal.z, -(normal.dotProduct(v1)));
    }
    //-----------------------------------------------------------------------
    Vector3 Math::calculateBasicFaceNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3)
    {
        Vector3 normal = (v2 - v1).crossProduct(v3 - v1);
        normal.normalize();
        return normal;
    }
    //-----------------------------------------------------------------------
    Vector4 Math::calculateFaceNormalWithoutNormalize(const Vector3& v1, const Vector3& v2, const Vector3& v3)
    {
        Vector3 normal = calculateBasicFaceNormalWithoutNormalize(v1, v2, v3);
        // Now set up the w (distance of tri from origin)
        return Vector4(normal.x, normal.y, normal.z, -(normal.dotProduct(v1)));
    }
    //-----------------------------------------------------------------------
    Vector3 Math::calculateBasicFaceNormalWithoutNormalize(const Vector3& v1, const Vector3& v2, const Vector3& v3)
    {
        Vector3 normal = (v2 - v1).crossProduct(v3 - v1);
        return normal;
    }
}
