// NOTE THAT THIS FILE IS BASED ON MATERIAL FROM:

// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000, All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// FREE SOURCE CODE
// http://www.magic-software.com/License/free.pdf

#ifndef __Quaternion_H__
#define __Quaternion_H__
#include "ReMathPrerequisite.h"

#include "ReAngle.h"

namespace xs {
	/*! \addtogroup RenderEngine
	*  äÖÈ¾ÒýÇæ
	*  @{
	*/

    /** Implementation of a Quaternion, i.e. a rotation around an axis.
    */
    class _ReMathExport Quaternion
    {
    public:
        inline Quaternion (
            float fX = 0.0, float fY = 0.0, float fZ = 0.0,float fW = 1.0)
		{
			w = fW;
			x = fX;
			y = fY;
			z = fZ;
		}
        inline Quaternion (const Quaternion& rkQ)
		{
			w = rkQ.w;
			x = rkQ.x;
			y = rkQ.y;
			z = rkQ.z;
		}
        /// Construct a quaternion from a rotation matrix
        inline Quaternion(const Matrix3& rot)
        {
            this->FromRotationMatrix(rot);
        }
        /// Construct a quaternion from an angle/axis
        inline Quaternion(const Radian& rfAngle, const Vector3& rkAxis)
        {
            this->FromAngleAxis(rfAngle, rkAxis);
        }
#ifndef RE_FORCE_ANGLE_TYPES
        inline Quaternion(const float& rfAngle, const Vector3& rkAxis)
		{
			this->FromAngleAxis(rfAngle, rkAxis);
		}
#endif//RE_FORCE_ANGLE_TYPES
        /// Construct a quaternion from 3 orthonormal local axes
        inline Quaternion(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
        {
            this->FromAxes(xaxis, yaxis, zaxis);
        }
        /// Construct a quaternion from 3 orthonormal local axes
        inline Quaternion(const Vector3* akAxis)
        {
            this->FromAxes(akAxis);
        }

        void FromRotationMatrix (const Matrix3& kRot);
        void ToRotationMatrix (Matrix3& kRot) const;
        void FromAngleAxis (const Radian& rfAngle, const Vector3& rkAxis);
        void ToAngleAxis (Radian& rfAngle, Vector3& rkAxis) const;
        inline void ToAngleAxis (Degree& dAngle, Vector3& rkAxis) const {
            Radian rAngle;
            ToAngleAxis ( rAngle, rkAxis );
            dAngle = rAngle;
        }
#ifndef RE_FORCE_ANGLE_TYPES
        inline void FromAngleAxis (const float& rfAngle, const Vector3& rkAxis) {
			FromAngleAxis ( Angle(rfAngle), rkAxis );
		}
        inline void ToAngleAxis (float& rfAngle, Vector3& rkAxis) const {
			Radian r;
			ToAngleAxis ( r, rkAxis );
			rfAngle = r.valueAngleUnits();
		}
#endif//RE_FORCE_ANGLE_TYPES
        void FromAxes (const Vector3* akAxis);
        void FromAxes (const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);
        void ToAxes (Vector3* akAxis) const;
        void ToAxes (Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const;
        /// Get the local x-axis
        Vector3 xAxis(void) const;
        /// Get the local y-axis
        Vector3 yAxis(void) const;
        /// Get the local z-axis
        Vector3 zAxis(void) const;

        inline Quaternion& operator= (const Quaternion& rkQ)
		{
			w = rkQ.w;
			x = rkQ.x;
			y = rkQ.y;
			z = rkQ.z;
			return *this;
		}
        Quaternion operator+ (const Quaternion& rkQ) const;
        Quaternion operator- (const Quaternion& rkQ) const;
        Quaternion operator* (const Quaternion& rkQ) const;
        Quaternion operator* (float fScalar) const;
        _ReMathExport friend Quaternion operator* (float fScalar,
            const Quaternion& rkQ);
        Quaternion operator- () const;
        inline bool operator== (const Quaternion& rhs) const
		{
			return (rhs.x == x) && (rhs.y == y) &&
				(rhs.z == z) && (rhs.w == w);
		}
        inline bool operator!= (const Quaternion& rhs) const
		{
			return !operator==(rhs);
		}
        // functions of a quaternion
        float Dot (const Quaternion& rkQ) const;  // dot product
        float Norm () const;  // squared-length
        /// Normalises this quaternion, and returns the previous length
        float normalize(void); 
        Quaternion Inverse () const;  // apply to non-zero quaternion
        Quaternion UnitInverse () const;  // apply to unit-length quaternion
        Quaternion Exp () const;
        Quaternion Log () const;

		float LengthSq() const;
        // rotation of a vector by a quaternion
        Vector3 operator* (const Vector3& rkVector) const;

   		/// Calculate the local roll element of this quaternion
		Radian getRoll(void) const;
   		/// Calculate the local pitch element of this quaternion
		Radian getPitch(void) const;
   		/// Calculate the local yaw element of this quaternion
		Radian getYaw(void) const;		
		/// Equality with tolerance (tolerance is max angle difference)
		bool equals(const Quaternion& rhs, const Radian& tolerance) const;
		
	    // spherical linear interpolation
        static Quaternion Slerp (float fT, const Quaternion& rkP,
            const Quaternion& rkQ, bool shortestPath = false);

        static Quaternion SlerpExtraSpins (float fT,
            const Quaternion& rkP, const Quaternion& rkQ,
            int iExtraSpins);

        // setup for spherical quadratic interpolation
        static void Intermediate (const Quaternion& rkQ0,
            const Quaternion& rkQ1, const Quaternion& rkQ2,
            Quaternion& rka, Quaternion& rkB);
		static Quaternion Intermediate(const Quaternion& q0, const Quaternion& q1, const Quaternion& q2);
        // spherical quadratic interpolation
        static Quaternion Squad (float fT, const Quaternion& rkP,
            const Quaternion& rkA, const Quaternion& rkB,
            const Quaternion& rkQ, bool shortestPath = false);
		
		static void SquadSetup(Quaternion& outA,Quaternion& outB,Quaternion& outC,const Quaternion& q0,const Quaternion& q1,const Quaternion& q2,const Quaternion& q3);

        // normalised linear interpolation - faster but less accurate (non-constant rotation velocity)
        static Quaternion nlerp(float fT, const Quaternion& rkP, 
            const Quaternion& rkQ, bool shortestPath = false);

        // cutoff for sine near zero
        static const float ms_fEpsilon;

        // special values
        static const Quaternion ZERO;
        static const Quaternion IDENTITY;

        float x, y, z, w;

        /** Function for writing to a stream.
        */
        inline _ReMathExport friend std::ostream& operator <<
            ( std::ostream& o, const Quaternion& q )
        {
            o << "Quaternion(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
            return o;
        }

    };
	/** @} */

}




#endif 
