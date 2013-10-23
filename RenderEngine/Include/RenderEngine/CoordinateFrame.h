#ifndef __CoordinateFrame_H__
#define __CoordinateFrame_H__

#include "Math/ReVector3.h"
#include "Math/ReVector4.h"
#include "Math/ReMatrix3.h"

namespace xs
{
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/


/**
 A rigid body RT (rotation-translation) transformation.
    
CoordinateFrame abstracts a 4x4 matrix that maps object space to world space:
  
  v_world = C * v_object

CoordinateFrame::rotation is the upper 3x3 submatrix, CoordinateFrame::translation
is the right 3x1 column.  The 4th row is always [0 0 0 1], so it isn't stored.  
So you don't have to remember which way the multiplication and transformation work,
it provides explicit toWorldSpace and toObjectSpace methods.  Also, points, vectors 
(directions), and surface normals transform differently, so they have separate methods.
 
Some helper functions transform whole primitives like boxes in and out of object space.

Convert to Matrix4 using CoordinateFrame::toMatrix4.  You <I>can</I> construct a CoordinateFrame
from a Matrix4 using Matrix4::approxCoordinateFrame, however, because a Matrix4 is more 
general than a CoordinateFrame, some information may be lost.

See also: xs::Matrix4, xs::Quat
*/
class CoordinateFrame 
{

public:

    /**
     The direction an object "looks" relative to its own axes.
     @deprecated This is always -1 and will be fixed at that value in future releases.
     */
    static const float				zLookDirection;

    /**
     Takes object space points to world space.
     */
    Matrix3							rotation;

    /**
     Takes object space points to world space.
     */
    Vector3							translation;

    inline bool operator==(const CoordinateFrame& other) const {
        return (translation == other.translation) && (rotation == other.rotation);
    }

    inline bool operator!=(const CoordinateFrame& other) const {
        return !(*this == other);
    }

    /**
     Initializes to the identity coordinate frame.
     */
    inline CoordinateFrame() : 
        rotation(Matrix3::IDENTITY), translation(Vector3::ZERO) {
    }

	CoordinateFrame(const Vector3& _translation) :
        rotation(Matrix3::IDENTITY), translation(_translation) {
	}

    CoordinateFrame(const Matrix3 &rotation, const Vector3 &translation) :
        rotation(rotation), translation(translation) {
    }

    CoordinateFrame(const Matrix3 &rotation) :
        rotation(rotation), translation(Vector3::ZERO) {
    }

    CoordinateFrame(const CoordinateFrame &other) :
        rotation(other.rotation), translation(other.translation) {}

    /**
      Computes the inverse of this coordinate frame.
     */
    inline CoordinateFrame inverse() const {
        CoordinateFrame out;
        // Use left multiply to avoid data dependence with the
        // Transpose operation.
//        out.translation = -translation * out.rotation;
        out.rotation = rotation.Transpose();
        out.translation = -out.rotation * translation;
        return out;
    }

    inline ~CoordinateFrame() {}

    /** See also Matrix4::approxCoordinateFrame */
    class Matrix4 toMatrix4() const;

    /**
     Returns the heading as an angle in radians, where
    north is 0 and west is PI/2
     */
    inline float getHeading() const {
        Vector3 look = rotation.GetColumn(2);
        float angle = (float) atan2( -look.z, look.x);
        return angle;
    }

    /**
     Takes the coordinate frame into object space.
     this->inverse() * c
     */
    inline CoordinateFrame toObjectSpace(const CoordinateFrame& c) const {
        return this->inverse() * c;
    }

    inline Vector4 toObjectSpace(const Vector4& v) const {
        return this->inverse().toWorldSpace(v);
    }

    inline Vector4 toWorldSpace(const Vector4& v) const {
        return Vector4(rotation * Vector3(v.x, v.y, v.z) + translation * v.w, v.w);
    }

    /**
     Transforms the point into world space.
     */
    inline Vector3 pointToWorldSpace(const Vector3& v) const {
        return Vector3(
			rotation[0][0] * v[0] + rotation[0][1] * v[1] + rotation[0][2] * v[2] + translation[0],
			rotation[1][0] * v[0] + rotation[1][1] * v[1] + rotation[1][2] * v[2] + translation[1],
			rotation[2][0] * v[0] + rotation[2][1] * v[1] + rotation[2][2] * v[2] + translation[2]);
    }

    /**
     Transforms the point into object space.
     */
	inline Vector3 pointToObjectSpace(const Vector3& v) const {
		float p[3];
		p[0] = v[0] - translation[0];
		p[1] = v[1] - translation[1];
		p[2] = v[2] - translation[2];
		return Vector3(
			rotation[0][0] * p[0] + rotation[1][0] * p[1] + rotation[2][0] * p[2],
			rotation[0][1] * p[0] + rotation[1][1] * p[1] + rotation[2][1] * p[2],
			rotation[0][2] * p[0] + rotation[1][2] * p[1] + rotation[2][2] * p[2]);
    }

    /**
     Transforms the vector into world space (no translation).
     */
    inline Vector3 vectorToWorldSpace(const Vector3& v) const {
        return rotation * v;
    }

    inline Vector3 normalToWorldSpace(const Vector3& v) const {
        return rotation * v;
    }

    /**
     Transforms the vector into object space (no translation).
     */
    inline Vector3 vectorToObjectSpace(const Vector3 &v) const {
        return rotation.Transpose() * v;
    }

    inline Vector3 normalToObjectSpace(const Vector3 &v) const {
        return rotation.Transpose() * v;
    }

    /** Compose: create the transformation that is <I>other</I> followed by <I>this</I>.*/
    CoordinateFrame operator*(const CoordinateFrame &other) const {
        return CoordinateFrame(rotation * other.rotation,
                               pointToWorldSpace(other.translation));
    }

    CoordinateFrame operator+(const Vector3& v) const {
        return CoordinateFrame(rotation, translation + v);
    }

    CoordinateFrame operator-(const Vector3& v) const {
        return CoordinateFrame(rotation, translation - v);
    }

    void  lookAt(const Vector3& target);

    void  lookAt(
        const Vector3&  target,
        Vector3         up);

    /** @deprecated See lookVector */
	inline Vector3 getLookVector() const {
		return rotation.GetColumn(2) * zLookDirection;
	}

    /** The direction this camera is looking (its negative z axis)*/
	inline Vector3 lookVector() const {
		return rotation.GetColumn(2) * zLookDirection;
	}

    /** Returns the ray starting at the camera origin travelling in direction CoordinateFrame::lookVector. */
    class Ray lookRay() const;

    /** Up direction for this camera (its y axis). */
    inline Vector3 upVector() const {
        return rotation.GetColumn(1);
    }

    /**
     If a viewer looks along the look vector, this is the viewer's "left"
     @deprecated leftVector
     */
    inline Vector3 getLeftVector() const {
		return -rotation.GetColumn(0);
	}

    /** @deprecated See rightVector */
    inline Vector3 getRightVector() const {
		return rotation.GetColumn(0);
	}

    /**
     If a viewer looks along the look vector, this is the viewer's "left".
     Useful for strafing motions and building alternative coordinate frames.
     */
    inline Vector3 leftVector() const {
		return -rotation.GetColumn(0);
    }

    inline Vector3 rightVector() const {
		return rotation.GetColumn(0);
    }

    /**
     Linearly interpolates between two coordinate frames, using
     Quat::slerp for the rotations.
     */
    CoordinateFrame lerp(
        const CoordinateFrame&  other,
        float                   alpha) const;

	Ray CoordinateFrame::toWorldSpace(const Ray& r) const;
	Ray CoordinateFrame::toObjectSpace(const Ray& r) const;
};

	/** @} */
} // namespace

#endif
