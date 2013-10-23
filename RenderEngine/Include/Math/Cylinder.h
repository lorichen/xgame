#ifndef __Cylinder_H__
#define __Cylinder_H__

#include "ReVector3.h"
#include "ReRay.h"

namespace xs
{
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/
	
	/**
	Right cylinder
	*/
	class _ReMathExport Cylinder
	{
	private:
		Vector3			p1;
		Vector3			p2;

		float			mRadius;

	public:

		/** Uninitialized */
		Cylinder();
		Cylinder(const Vector3& _p1, const Vector3& _p2, float _r);

		/** The line down the center of the Cylinder */
		Ray getAxis() const;

		const Vector3& getPoint1() const;

		const Vector3& getPoint2() const;

		/**
		Returns true if the point is inside the Cylinder or on its surface.
		*/
		bool contains(const Vector3& p) const;

		float area() const;

		float volume() const;

		float radius() const; 

		/** Center of mass */
		inline Vector3 center() const {
			return (p1 + p2) / 2.0f;
		}

		inline float height() const {
			return (p1 - p2).length();
		}

		/**
		Get close axis aligned bounding box.
		With vertical world orientation, the top and bottom might not be very tight. */
		void getBounds(AABB& out) const;

	};
	/** @} */

} // namespace

#endif
