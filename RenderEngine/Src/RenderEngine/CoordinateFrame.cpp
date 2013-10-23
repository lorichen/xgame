#include "StdAfx.h"
#include "CoordinateFrame.h"

namespace xs
{

const float CoordinateFrame::zLookDirection = -1;

Ray CoordinateFrame::lookRay() const {
    return Ray(translation,lookVector());
}


Matrix4 CoordinateFrame::toMatrix4() const 
{
	Matrix4 m;
	for (uint r = 0; r < 3; ++r) 
	{
		for (uint c = 0; c < 3; ++c) 
		{
			m[r][c] = rotation[r][c];
		}
		m[r][3] = translation[r];
	}
	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;

    return m;
}

void  CoordinateFrame::lookAt(const Vector3 &target) {
    lookAt(target, Vector3::UNIT_Y);
}


void  CoordinateFrame::lookAt(
    const Vector3&  target,
    Vector3         up) {

    up = up.direction();

    Vector3 look = (target - translation).direction();
    if (look.dotProduct(up) > .99) {
        up = Vector3::UNIT_X;
        if (look.dotProduct(up) > .99) {
            up = Vector3::UNIT_Y;
        }
    }

    up -= look * look.dotProduct(up);
    up.normalize();

    Vector3 z = -look;
    Vector3 x = -z.crossProduct(up);
    x.normalize();

    Vector3 y = z.crossProduct(x);

    rotation.SetColumn(0, x);
    rotation.SetColumn(1, y);
    rotation.SetColumn(2, z);
}


CoordinateFrame CoordinateFrame::lerp(
    const CoordinateFrame&  other,
    float                   alpha) const 
{

    if (alpha == 1.0) {
        return other;
    } else if (alpha == 0.0) {
        return *this;
    } else {
        Quaternion q1 = Quaternion(this->rotation);
        Quaternion q2 = Quaternion(other.rotation);

		Matrix3 kRot;
		Quaternion q = Quaternion::Slerp(alpha,q1,q2);
		q.ToRotationMatrix(kRot);
        return CoordinateFrame(kRot,
            this->translation * (1 - alpha) + other.translation * alpha);
    }
} 

Ray CoordinateFrame::toWorldSpace(const Ray& r) const 
{
	Ray ray;
	ray.setOrigin(pointToWorldSpace(r.getOrigin()));
	ray.setDirection(vectorToWorldSpace(r.getDirection()));
	return ray;
}


Ray CoordinateFrame::toObjectSpace(const Ray& r) const
{
	Ray ray;
	ray.setOrigin(pointToObjectSpace(r.getOrigin()));
	ray.setDirection(vectorToObjectSpace(r.getDirection()));
	return ray;
}

} // namespace
