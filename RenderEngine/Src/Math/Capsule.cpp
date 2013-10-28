
#include "ReMathPch.h"
#include "Capsule.h"

namespace xs
{

	Capsule::Capsule() {
	}


	Capsule::Capsule(const Vector3& _p1, const Vector3& _p2, float _r) 
		: p1(_p1), p2(_p2), radius(_r)
	{
	}


	Ray Capsule::getAxis() const
	{
		return Ray(p1,p2);
	}


	Vector3 Capsule::getPoint1() const {
		return p1;
	}


	Vector3 Capsule::getPoint2() const {
		return p2;
	}


	float Capsule::getRadius() const {
		return (float)radius;
	}


	float Capsule::getVolume() const {
		return 
			// Sphere volume
			Math::Pow(radius, 3) * Math::PI * 4 / 3 +

			// Cylinder volume
			Math::Sqr(radius) * (p1 - p2).length();
	}


	float Capsule::getSurfaceArea() const {

		return
			// Sphere area
			Math::Sqr(radius) * 4 * Math::PI +

			// Cylinder area
			2 * Math::PI * radius * (p1 - p2).length();
	}


	void Capsule::getBounds(AABB& out) const {
		Vector3 vMin = p1.Min(p2) - (Vector3(1, 1, 1) * radius);
		Vector3 vMax = p1.Max(p2) + (Vector3(1, 1, 1) * radius);

		out.setExtents(vMin,vMax);
	}

	bool Capsule::contains(const Vector3& p) const { 
		return getAxis().distanceSquared(p) <= Math::Sqr(radius);
	}
} // namespace
