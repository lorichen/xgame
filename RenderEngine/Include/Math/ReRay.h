#ifndef __Ray_H_
#define __Ray_H_
#include "ReMathPrerequisite.h"

#include "ReVector3.h"
#include "RePlaneBoundedVolume.h"

namespace xs {
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/

    /** Representation of a ray in space, ie a line with an origin and mDirection. */
    class _ReMathExport Ray
    {
    protected:
        Vector3 mOrigin;
        Vector3 mDirection;
    public:
        Ray():mOrigin(Vector3::ZERO), mDirection(Vector3::UNIT_Z) {}
        Ray(const Vector3& origin, const Vector3& mDirection)
            :mOrigin(origin), mDirection(mDirection) {}
        virtual ~Ray() {}

        /** Sets the origin of the ray. */
        void setOrigin(const Vector3& origin) {mOrigin = origin;} 
        /** Gets the origin of the ray. */
        const Vector3& getOrigin(void) const {return mOrigin;} 

        /** Sets the mDirection of the ray. */
        void setDirection(const Vector3& dir) {mDirection = dir;} 
        /** Gets the mDirection of the ray. */
        const Vector3& getDirection(void) const {return mDirection;} 

		/** Gets the position of a point t units along the ray. */
		Vector3 getPoint(float t) const { 
			return Vector3(mOrigin + (mDirection * t));
		}
		
		/** Gets the position of a point t units along the ray. */
		Vector3 operator*(float t) const { 
			return getPoint(t);
		};

		/** Tests whether this ray intersects the given plane. 
		@returns A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it intersects. 
			This can be converted to a point in space by calling getPoint().
		*/
		std::pair<bool, float> intersects(const Plane& p) const
		{
			return Math::intersects(*this, p);
		}
        /** Tests whether this ray intersects the given plane bounded volume. 
        @returns A pair structure where the first element indicates whether
        an intersection occurs, and if true, the second element will
        indicate the distance along the ray at which it intersects. 
        This can be converted to a point in space by calling getPoint().
        */
        std::pair<bool, float> intersects(const PlaneBoundedVolume& p) const
        {
            return Math::intersects(*this, p.planes, p.outside == Plane::POSITIVE_SIDE);
        }
		/** Tests whether this ray intersects the given sphere. 
		@returns A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it intersects. 
			This can be converted to a point in space by calling getPoint().
		*/
		std::pair<bool, float> intersects(const Sphere& s) const
		{
			return Math::intersects(*this, s);
		}
		/** Tests whether this ray intersects the given box. 
		@returns A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it intersects. 
			This can be converted to a point in space by calling getPoint().
		*/
		std::pair<bool, float> intersects(const AABB& box) const
		{
			return Math::intersects(*this, box);
		}

		/**
		Returns the closest point on the line to point.
		*/
		Vector3 closestPoint(const Vector3& p) const
		{

			// The vector from the end of the capsule to the point in question.
			Vector3 v(p - mOrigin);

			// Projection of v onto the line segment scaled by 
			// the length of mDirection.
			float t = mDirection.dotProduct(v);

			// Avoid some square roots.  Derivation:
			//    t/mDirection.length() <= mDirection.length()
			//      t <= mDirection.squaredLength()

			if ((t >= 0) && (t <= mDirection.squaredLength())) {

				// The point falls within the segment.  Normalize mDirection,
				// divide t by the length of mDirection.
				return mOrigin + mDirection * t / mDirection.squaredLength();

			} else {

				// The point does not fall within the segment; see which end is closer.

				// Distance from 0, squared
				float d0Squared = v.squaredLength();

				// Distance from 1, squared
				float d1Squared = (v - mDirection).squaredLength();

				if (d0Squared < d1Squared) {

					// Point 0 is closer
					return mOrigin;

				} else {

					// Point 1 is closer
					return mOrigin + mDirection;

				}
			}

		}

		/**
		Returns the distance between point and the line
		*/
		float distance(const Vector3& p) const {
			return (closestPoint(p) - p).length();
		}

		float distanceSquared(const Vector3& p) const {
			return (closestPoint(p) - p).squaredLength();
		}
    };
	/** @} */

}
#endif
