#ifndef __Capsule_H__
#define __Capsule_H__

#include "ReVector3.h"
#include "ReRay.h"

namespace xs
{
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/
	
	/**
	A shape formed by extruding a sphere along a line segment.
	*/
	class _ReMathExport Capsule
	{
	private:
		Vector3			p1;
		Vector3			p2;

	public:

		/** @deprecated To be replaced with a (float) radius accessor of the same name.*/
		float			radius;

		/** Uninitialized */
		Capsule();
		Capsule(const Vector3& _p1, const Vector3& _p2, float _r);

		/** The line down the center of the capsule */
		Ray getAxis() const;

		Vector3 getPoint1() const;

		Vector3 getPoint2() const;

		/** Distance between the sphere centers.  The total extent of the cylinder is 
		2r + h. */
		inline float height() const {
			return (p1 - p2).length();
		}

		inline Vector3 center() const {
			return (p1 + p2) / 2.0;
		}

		/**
		Returns true if the point is inside the capsule or on its surface.
		*/
		bool contains(const Vector3& p) const;

		/** @deprecated */
		float getRadius() const;

		/** @deprecated Use volume() */
		float getVolume() const;

		inline float volume() const {
			return getVolume();
		}

		/** @deprecated */
		float getSurfaceArea() const;

		inline float area() const {
			return getSurfaceArea();
		}

		/** Get axis aligned bounding box */
		void getBounds(AABB& out) const;
	};
	/** @} */

} // namespace

#endif
