#ifndef __PlaneBoundedVolume_H_
#define __PlaneBoundedVolume_H_

#include "ReMathPrerequisite.h"
#include "ReAxisAlignedBox.h"
#include "ReSphere.h"
#include "ReAngle.h"
#include "RePlane.h"

namespace xs {
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/

    /** Represents a convex volume bounded by planes.
    */
    class _ReMathExport PlaneBoundedVolume
    {
    public:
        typedef std::vector<Plane> PlaneList;
        /// Publicly accessible plane list, you can modify this direct
        PlaneList planes;
        Plane::Side outside;

        PlaneBoundedVolume() :outside(Plane::NEGATIVE_SIDE) {}
        /** Constructor, determines which side is deemed to be 'outside' */
        PlaneBoundedVolume(Plane::Side theOutside) 
            : outside(theOutside) {}

        /** Intersection test with AABB
        @remarks May return false positives but will never miss an intersection.
        */
        inline bool intersects(const AABB& box) const
        {
            if (box.isNull()) return false;
            // If all points are on outside of any plane, we fail
            const Vector3* points = box.getAllCorners();
            PlaneList::const_iterator i, iend;
            iend = planes.end();
            for (i = planes.begin(); i != iend; ++i)
            {
                const Plane& plane = *i;

                // Test which side of the plane the corners are
                // Intersection fails when at all corners are on the
                // outside of one plane
                bool splittingPlane = true;
                for (int corner = 0; corner < 8; ++corner)
                {
                    if (plane.getSide(points[corner]) != outside)
                    {
                        // this point is on the wrong side
                        splittingPlane = false;
                        break;
                    }
                }
                if (splittingPlane)
                {
                    // Found a splitting plane therefore return not intersecting
                    return false;
                }
            }

            // couldn't find a splitting plane, assume intersecting
            return true;

        }
        /** Intersection test with Sphere
        @remarks May return false positives but will never miss an intersection.
        */
        inline bool intersects(const Sphere& sphere) const
        {
            PlaneList::const_iterator i, iend;
            iend = planes.end();
            for (i = planes.begin(); i != iend; ++i)
            {
                const Plane& plane = *i;

                // Test which side of the plane the sphere is
                float d = plane.getDistance(sphere.getCenter());
                // Negate d if planes point inwards
                if (outside == Plane::NEGATIVE_SIDE) d = -d;

                if ( (d - sphere.getRadius()) > 0)
                    return false;
            }

            return true;

        }

        /** Intersection test with a Ray
        @returns std::pair of hit (bool) and distance
        @remarks May return false positives but will never miss an intersection.
        */
        inline std::pair<bool, float> intersects(const Ray& ray)
        {
            return Math::intersects(ray, planes, outside == Plane::POSITIVE_SIDE);
        }

    };

    typedef std::vector<PlaneBoundedVolume> PlaneBoundedVolumeList;

	/** @} */

}

#endif

