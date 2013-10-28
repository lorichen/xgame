// Original free version by:
// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000, All Rights Reserved

#ifndef __Plane_H__
#define __Plane_H__
#include "ReMathPrerequisite.h"

#include "ReVector3.h"

namespace xs {
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/

    /** Defines a plane in 3D space.
        @remarks
            A plane is defined in 3D space by the equation
            Ax + By + Cz + D = 0
        @par
            This equates to a vector (the normal of the plane, whose x, y
            and z components equate to the coefficients A, B and C
            respectively), and a constant (D) which is the distance along
            the normal you have to go to move the plane back to the origin.
     */
    class _ReMathExport Plane
    {
    public:
        /** Default constructor - sets everything to 0.
        */
        Plane ();
        Plane (const Plane& rhs);
        /** Construct a plane through a normal, and a distance to move the plane along the normal.*/
        Plane (const Vector3& rkNormal, float fConstant);
        Plane (const Vector3& rkNormal, const Vector3& rkPoint);
        Plane (const Vector3& rkPoint0, const Vector3& rkPoint1,
            const Vector3& rkPoint2);

        /** The "positive side" of the plane is the half space to which the
            plane normal points. The "negative side" is the other half
            space. The flag "no side" indicates the plane itself.
        */
        enum Side
        {
            NO_SIDE,
            POSITIVE_SIDE,
            NEGATIVE_SIDE
        };

		//-----------------------------------------------------------------------
		/** This is a pseudodistance. The sign of the return value is
		positive if the point is on the positive side of the plane,
		negative if the point is on the negative side, and zero if the
		point is on the plane.
		@par
		The absolute value of the return value is the true distance only
		when the plane normal is a unit length vector.
		*/
		inline float getDistance (const Vector3& rkPoint) const
		{
			return normal.dotProduct(rkPoint) + d;
		}
		//-----------------------------------------------------------------------
		inline Plane::Side getSide (const Vector3& rkPoint) const
		{
			float fDistance = getDistance(rkPoint);

			if ( fDistance < 0.0 )
				return Plane::NEGATIVE_SIDE;

			if ( fDistance > 0.0 )
				return Plane::POSITIVE_SIDE;

			return Plane::NO_SIDE;
		}

        /** Redefine this plane based on 3 points. */
        void redefine(const Vector3& rkPoint0, const Vector3& rkPoint1,
            const Vector3& rkPoint2);

        Vector3 normal;
        float d;
        /// Comparison operator
        bool operator==(const Plane& rhs) const
        {
            return (rhs.d == d && rhs.normal == normal);
        }

        _ReMathExport friend std::ostream& operator<< (std::ostream& o, Plane& p);
    };

    typedef std::vector<Plane> PlaneList;
	/** @} */

} // namespace xs

#endif
