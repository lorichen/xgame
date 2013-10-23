
#include "ReMathPch.h"
#include "RePlane.h"

namespace xs {
    //-----------------------------------------------------------------------
    Plane::Plane ()
    {
        normal = Vector3::ZERO;
        d = 0.0;
    }
    //-----------------------------------------------------------------------
    Plane::Plane (const Plane& rhs)
    {
        normal = rhs.normal;
        d = rhs.d;
    }
    //-----------------------------------------------------------------------
    Plane::Plane (const Vector3& rkNormal, float fConstant)
    {
        normal = rkNormal;
        d = -fConstant;
    }
    //-----------------------------------------------------------------------
    Plane::Plane (const Vector3& rkNormal, const Vector3& rkPoint)
    {
        normal = rkNormal;
        d = -rkNormal.dotProduct(rkPoint);
    }
    //-----------------------------------------------------------------------
    Plane::Plane (const Vector3& rkPoint0, const Vector3& rkPoint1,
        const Vector3& rkPoint2)
    {
        redefine(rkPoint0, rkPoint1, rkPoint2);
    }
    //-----------------------------------------------------------------------
    void Plane::redefine(const Vector3& rkPoint0, const Vector3& rkPoint1,
        const Vector3& rkPoint2)
    {
        Vector3 kEdge1 = rkPoint1 - rkPoint0;
        Vector3 kEdge2 = rkPoint2 - rkPoint0;
        normal = kEdge1.crossProduct(kEdge2);
        normal.normalize();
        d = -normal.dotProduct(rkPoint0);
    }
    //-----------------------------------------------------------------------
    std::ostream& operator<< (std::ostream& o, Plane& p)
    {
        o << "Plane(normal=" << p.normal << ", d=" << p.d << ")";
        return o;
    }
} // namespace xs
