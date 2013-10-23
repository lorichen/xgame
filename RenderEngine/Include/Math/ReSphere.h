#ifndef __Sphere_H_
#define __Sphere_H_
#include "ReMathPrerequisite.h"

#include "ReVector3.h"

namespace xs {
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/


    /** A sphere primitive, mostly used for bounds checking. 
    @remarks
        A sphere in math texts is normally represented by the function
        x^2 + y^2 + z^2 = r^2 (for sphere's centered on the origin). rkt stores spheres
        simply as a center point and a radius.
    */
    class _ReMathExport Sphere
    {
    protected:
        float mRadius;
        Vector3 mCenter;
    public:
        /** Standard constructor - creates a unit sphere around the origin.*/
        Sphere() : mRadius(1.0), mCenter(Vector3::ZERO) {}
        /** Constructor allowing arbitrary spheres. 
            @param center The center point of the sphere.
            @param radius The radius of the sphere.
        */
        Sphere(const Vector3& center, float radius)
            : mRadius(radius), mCenter(center) {}

        /** Returns the radius of the sphere. */
        float getRadius(void) const { return mRadius; }

        /** Sets the radius of the sphere. */
        void setRadius(float radius) { mRadius = radius; }

        /** Returns the center point of the sphere. */
        const Vector3& getCenter(void) const { return mCenter; }

        /** Sets the center point of the sphere. */
        void setCenter(const Vector3& center) { mCenter = center; }

		/** from AABB*/
		void fromAABB(const AABB& aabb)
		{
			const Vector3& vMin = aabb.getMinimum();
			const Vector3& vMax = aabb.getMaximum();
			setCenter(vMin.midPoint(vMax));
			setRadius((vMax - vMin).length() * 0.5f);
		}

		/** Returns whether or not this sphere interects another sphere. */
		bool intersects(const Sphere& s) const
		{
			return (s.mCenter - mCenter).length() <=
				(s.mRadius + mRadius);
		}
		/** Returns whether or not this sphere interects a box. */
		bool intersects(const AABB& box) const
		{
			return Math::intersects(*this, box);
		}
		/** Returns whether or not this sphere interects a plane. */
		bool intersects(const Plane& plane) const
		{
			return Math::intersects(*this, plane);
		}
		/** Returns whether or not this sphere interects a point. */
		bool intersects(const Vector3& v) const
		{
			return ((v - mCenter).length() <= mRadius);
		}
        

    };
	/** @} */

}

#endif

