#include "ReMathPch.h"
#include "Cylinder.h"
#include "RePlane.h"

namespace xs
{

	Cylinder::Cylinder() {
	}


	Cylinder::Cylinder(const Vector3& _p1, const Vector3& _p2, float _r) 
		: p1(_p1), p2(_p2), mRadius(_r)
	{
	}


	Ray Cylinder::getAxis() const
	{
		Ray ray(p1,p2);
		return ray;
	}


	const Vector3& Cylinder::getPoint1() const {
		return p1;
	}


	const Vector3& Cylinder::getPoint2() const {
		return p2;
	}


	float Cylinder::radius() const {
		return mRadius;
	}


	float Cylinder::volume() const {
		return
			Math::PI * Math::Sqr(mRadius) * (p1 - p2).length();
	}


	float Cylinder::area() const {
		return
			// Sides
			(2.0f * Math::PI * mRadius) * height() +

			// Caps
			2.0f * Math::PI * Math::Sqr(mRadius);
	}

	void Cylinder::getBounds(AABB& out) const
	{
		Vector3 vMin = p1.Min(p2) - (Vector3(1, 1, 1) * mRadius);
		Vector3 vMax = p1.Max(p2) + (Vector3(1, 1, 1) * mRadius);
		out.setExtents(vMin,vMax);
	}

	bool Cylinder::contains(const Vector3& p) const { 
		bool b = getAxis().distanceSquared(p) <= Math::Sqr(mRadius);
		Plane plane1(p2 - p1,p1);
		Plane plane2(p1 - p2,p2);
		return b && plane1.getSide(p) != Plane::NEGATIVE_SIDE && plane2.getSide(p) != Plane::NEGATIVE_SIDE;
	}

} // namespace
