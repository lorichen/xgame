
#include "ReMathPch.h"
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

#include "ReQuaternion.h"

#include "ReAngle.h"
#include "ReMatrix3.h"
#include "ReVector3.h"

namespace xs {

    const float Quaternion::ms_fEpsilon = 1e-03;
    const Quaternion Quaternion::ZERO(0.0,0.0,0.0,0.0);
    const Quaternion Quaternion::IDENTITY(0.0,0.0,0.0,1.0);

    //-----------------------------------------------------------------------
    void Quaternion::FromRotationMatrix (const Matrix3& kRot)
    {
        // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
        // article "Quaternion Calculus and Fast Animation".

        float fTrace = kRot[0][0]+kRot[1][1]+kRot[2][2];
        float fRoot;

        if ( fTrace > 0.0 )
        {
            // |w| > 1/2, may as well choose w > 1/2
            fRoot = Math::Sqrt(fTrace + 1.0);  // 2w
            w = 0.5*fRoot;
            fRoot = 0.5/fRoot;  // 1/(4w)
            x = (kRot[2][1]-kRot[1][2])*fRoot;
            y = (kRot[0][2]-kRot[2][0])*fRoot;
            z = (kRot[1][0]-kRot[0][1])*fRoot;
        }
        else
        {
            // |w| <= 1/2
            static size_t s_iNext[3] = { 1, 2, 0 };
            size_t i = 0;
            if ( kRot[1][1] > kRot[0][0] )
                i = 1;
            if ( kRot[2][2] > kRot[i][i] )
                i = 2;
            size_t j = s_iNext[i];
            size_t k = s_iNext[j];

            fRoot = Math::Sqrt(kRot[i][i]-kRot[j][j]-kRot[k][k] + 1.0);
            float* apkQuat[3] = { &x, &y, &z };
            *apkQuat[i] = 0.5*fRoot;
            fRoot = 0.5/fRoot;
            w = (kRot[k][j]-kRot[j][k])*fRoot;
            *apkQuat[j] = (kRot[j][i]+kRot[i][j])*fRoot;
            *apkQuat[k] = (kRot[k][i]+kRot[i][k])*fRoot;
        }
    }
    //-----------------------------------------------------------------------
    void Quaternion::ToRotationMatrix (Matrix3& kRot) const
    {
        float fTx  = 2.0*x;
        float fTy  = 2.0*y;
        float fTz  = 2.0*z;
        float fTwx = fTx*w;
        float fTwy = fTy*w;
        float fTwz = fTz*w;
        float fTxx = fTx*x;
        float fTxy = fTy*x;
        float fTxz = fTz*x;
        float fTyy = fTy*y;
        float fTyz = fTz*y;
        float fTzz = fTz*z;

        kRot[0][0] = 1.0-(fTyy+fTzz);
        kRot[0][1] = fTxy-fTwz;
        kRot[0][2] = fTxz+fTwy;
        kRot[1][0] = fTxy+fTwz;
        kRot[1][1] = 1.0-(fTxx+fTzz);
        kRot[1][2] = fTyz-fTwx;
        kRot[2][0] = fTxz-fTwy;
        kRot[2][1] = fTyz+fTwx;
        kRot[2][2] = 1.0-(fTxx+fTyy);
    }
    //-----------------------------------------------------------------------
    void Quaternion::FromAngleAxis (const Radian& rfAngle,
        const Vector3& rkAxis)
    {
        // assert:  axis[] is unit length
        //
        // The quaternion representing the rotation is
        //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

        Radian fHalfAngle ( 0.5*rfAngle );
        float fSin = Math::Sin(fHalfAngle);
        w = Math::Cos(fHalfAngle);
        x = fSin*rkAxis.x;
        y = fSin*rkAxis.y;
        z = fSin*rkAxis.z;
    }
    //-----------------------------------------------------------------------
    void Quaternion::ToAngleAxis (Radian& rfAngle, Vector3& rkAxis) const
    {
        // The quaternion representing the rotation is
        //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

        float fSqrLength = x*x+y*y+z*z;
        if ( fSqrLength > 0.0 )
        {
            rfAngle = 2.0*Math::ACos(w);
            float fInvLength = Math::InvSqrt(fSqrLength);
            rkAxis.x = x*fInvLength;
            rkAxis.y = y*fInvLength;
            rkAxis.z = z*fInvLength;
        }
        else
        {
            // angle is 0 (mod 2*pi), so any axis will do
            rfAngle = Radian(0.0);
            rkAxis.x = 1.0;
            rkAxis.y = 0.0;
            rkAxis.z = 0.0;
        }
    }
    //-----------------------------------------------------------------------
    void Quaternion::FromAxes (const Vector3* akAxis)
    {
        Matrix3 kRot;

        for (size_t iCol = 0; iCol < 3; iCol++)
        {
            kRot[0][iCol] = akAxis[iCol].x;
            kRot[1][iCol] = akAxis[iCol].y;
            kRot[2][iCol] = akAxis[iCol].z;
        }

        FromRotationMatrix(kRot);
    }
    //-----------------------------------------------------------------------
    void Quaternion::FromAxes (const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
    {
        Matrix3 kRot;

        kRot[0][0] = xaxis.x;
        kRot[1][0] = xaxis.y;
        kRot[2][0] = xaxis.z;

        kRot[0][1] = yaxis.x;
        kRot[1][1] = yaxis.y;
        kRot[2][1] = yaxis.z;

        kRot[0][2] = zaxis.x;
        kRot[1][2] = zaxis.y;
        kRot[2][2] = zaxis.z;

        FromRotationMatrix(kRot);

    }
    //-----------------------------------------------------------------------
    void Quaternion::ToAxes (Vector3* akAxis) const
    {
        Matrix3 kRot;

        ToRotationMatrix(kRot);

        for (size_t iCol = 0; iCol < 3; iCol++)
        {
            akAxis[iCol].x = kRot[0][iCol];
            akAxis[iCol].y = kRot[1][iCol];
            akAxis[iCol].z = kRot[2][iCol];
        }
    }
    //-----------------------------------------------------------------------
    Vector3 Quaternion::xAxis(void) const
    {
        float fTx  = 2.0*x;
        float fTy  = 2.0*y;
        float fTz  = 2.0*z;
        float fTwy = fTy*w;
        float fTwz = fTz*w;
        float fTxy = fTy*x;
        float fTxz = fTz*x;
        float fTyy = fTy*y;
        float fTzz = fTz*z;

        return Vector3(1.0-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);
    }
    //-----------------------------------------------------------------------
    Vector3 Quaternion::yAxis(void) const
    {
        float fTx  = 2.0*x;
        float fTy  = 2.0*y;
        float fTz  = 2.0*z;
        float fTwx = fTx*w;
        float fTwz = fTz*w;
        float fTxx = fTx*x;
        float fTxy = fTy*x;
        float fTyz = fTz*y;
        float fTzz = fTz*z;

        return Vector3(fTxy-fTwz, 1.0-(fTxx+fTzz), fTyz+fTwx);
    }
    //-----------------------------------------------------------------------
    Vector3 Quaternion::zAxis(void) const
    {
        float fTx  = 2.0*x;
        float fTy  = 2.0*y;
        float fTz  = 2.0*z;
        float fTwx = fTx*w;
        float fTwy = fTy*w;
        float fTxx = fTx*x;
        float fTxz = fTz*x;
        float fTyy = fTy*y;
        float fTyz = fTz*y;

        return Vector3(fTxz+fTwy, fTyz-fTwx, 1.0-(fTxx+fTyy));
    }
    //-----------------------------------------------------------------------
    void Quaternion::ToAxes (Vector3& xaxis, Vector3& yaxis, Vector3& zaxis) const
    {
        Matrix3 kRot;

        ToRotationMatrix(kRot);

        xaxis.x = kRot[0][0];
        xaxis.y = kRot[1][0];
        xaxis.z = kRot[2][0];

        yaxis.x = kRot[0][1];
        yaxis.y = kRot[1][1];
        yaxis.z = kRot[2][1];

        zaxis.x = kRot[0][2];
        zaxis.y = kRot[1][2];
        zaxis.z = kRot[2][2];
    }

    //-----------------------------------------------------------------------
    Quaternion Quaternion::operator+ (const Quaternion& rkQ) const
    {
        return Quaternion(x+rkQ.x,y+rkQ.y,z+rkQ.z,w+rkQ.w);
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::operator- (const Quaternion& rkQ) const
    {
        return Quaternion(x-rkQ.x,y-rkQ.y,z-rkQ.z,w-rkQ.w);
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::operator* (const Quaternion& rkQ) const
    {
        // NOTE:  Multiplication is not generally commutative, so in most
        // cases p*q != q*p.

        return Quaternion
        (
            w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
            w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
            w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x,
			w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z
        );
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::operator* (float fScalar) const
    {
        return Quaternion(fScalar*x,fScalar*y,fScalar*z,fScalar*w);
    }
    //-----------------------------------------------------------------------
    Quaternion operator* (float fScalar, const Quaternion& rkQ)
    {
        return Quaternion(fScalar*rkQ.x,fScalar*rkQ.y,
            fScalar*rkQ.z,fScalar*rkQ.w);
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::operator- () const
    {
        return Quaternion(-x,-y,-z,-w);
    }
    //-----------------------------------------------------------------------
    float Quaternion::Dot (const Quaternion& rkQ) const
    {
        return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;
    }
    //-----------------------------------------------------------------------
    float Quaternion::Norm () const
    {
        return w*w+x*x+y*y+z*z;
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::Inverse () const
    {
        float fNorm = w*w+x*x+y*y+z*z;
        if ( fNorm > 0.0 )
        {
            float fInvNorm = 1.0/fNorm;
            return Quaternion(-x*fInvNorm,-y*fInvNorm,-z*fInvNorm,w*fInvNorm);
        }
        else
        {
            // return an invalid result to flag the error
            return ZERO;
        }
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::UnitInverse () const
    {
        // assert:  'this' is unit length
        return Quaternion(-x,-y,-z,w);
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::Exp () const
    {
        // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
        // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
        // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

        Radian fAngle ( Math::Sqrt(x*x+y*y+z*z) );
        float fSin = Math::Sin(fAngle);

        Quaternion kResult;
        kResult.w = Math::Cos(fAngle);

        if ( Math::Abs(fSin) >= ms_fEpsilon )
        {
            float fCoeff = fSin/(fAngle.valueRadians());
            kResult.x = fCoeff*x;
            kResult.y = fCoeff*y;
            kResult.z = fCoeff*z;
        }
        else
        {
            kResult.x = x;
            kResult.y = y;
            kResult.z = z;
        }

        return kResult;
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::Log () const
    {
        // If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
        // log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
        // sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

        Quaternion kResult;
        kResult.w = 0.0;

        if ( Math::Abs(w) < 1.0 )
        {
            Radian fAngle ( Math::ACos(w) );
            float fSin = Math::Sin(fAngle);
            if ( Math::Abs(fSin) >= ms_fEpsilon )
            {
                float fCoeff = fAngle.valueRadians()/fSin;
                kResult.x = fCoeff*x;
                kResult.y = fCoeff*y;
                kResult.z = fCoeff*z;
                return kResult;
            }
        }

        kResult.x = x;
        kResult.y = y;
        kResult.z = z;

        return kResult;
	}
	//-----------------------------------------------------------------------
	float Quaternion::LengthSq() const
	{
		return x * x + y * y + z * z + w * w;
	}
    //-----------------------------------------------------------------------
    Vector3 Quaternion::operator* (const Vector3& v) const
    {
		// nVidia SDK implementation
		Vector3 uv, uuv; 
		Vector3 qvec(x, y, z);
		uv = qvec.crossProduct(v); 
		uuv = qvec.crossProduct(uv); 
		uv *= (2.0f * w); 
		uuv *= 2.0f; 
		
		return v + uv + uuv;

    }
    //-----------------------------------------------------------------------
	bool Quaternion::equals(const Quaternion& rhs, const Radian& tolerance) const
	{
        float fCos = Dot(rhs);
        Radian angle = Math::ACos(fCos);

		return (Math::Abs(angle.valueRadians()) <= tolerance.valueRadians())
            || Math::RealEqual(angle.valueRadians(), Math::PI, tolerance.valueRadians());

		
	}
    //-----------------------------------------------------------------------
    Quaternion Quaternion::Slerp (float fT, const Quaternion& rkP,
        const Quaternion& rkQ, bool shortestPath)
    {
        float fCos = rkP.Dot(rkQ);
        Radian fAngle ( Math::ACos(fCos) );

		float rad = fAngle.valueRadians();
        if ( Math::Abs(rad) < ms_fEpsilon )
            return rkP;

        float fSin = Math::Sin(fAngle);
        float fInvSin = 1.0/fSin;
        float fCoeff0 = Math::Sin((1.0-fT)*fAngle)*fInvSin;
        float fCoeff1 = Math::Sin(fT*fAngle)*fInvSin;
        // Do we need to invert rotation?
        if (fCos < 0.0f && shortestPath)
        {
            fCoeff0 = -fCoeff0;
            // taking the complement requires renormalisation
            Quaternion t(fCoeff0*rkP + fCoeff1*rkQ);
            t.normalize();
            return t;
        }
        else
        {
            return fCoeff0*rkP + fCoeff1*rkQ;
        }
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::SlerpExtraSpins (float fT,
        const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins)
    {
        float fCos = rkP.Dot(rkQ);
        Radian fAngle ( Math::ACos(fCos) );

        if ( Math::Abs(fAngle.valueRadians()) < ms_fEpsilon )
            return rkP;

        float fSin = Math::Sin(fAngle);
        Radian fPhase ( Math::PI*iExtraSpins*fT );
        float fInvSin = 1.0/fSin;
        float fCoeff0 = Math::Sin((1.0-fT)*fAngle - fPhase)*fInvSin;
        float fCoeff1 = Math::Sin(fT*fAngle + fPhase)*fInvSin;
        return fCoeff0*rkP + fCoeff1*rkQ;
    }
    //-----------------------------------------------------------------------
    void Quaternion::Intermediate (const Quaternion& rkQ0,
        const Quaternion& rkQ1, const Quaternion& rkQ2,
        Quaternion& rkA, Quaternion& rkB)
    {
        // assert:  q0, q1, q2 are unit quaternions

        Quaternion kQ0inv = rkQ0.UnitInverse();
        Quaternion kQ1inv = rkQ1.UnitInverse();
        Quaternion rkP0 = kQ0inv*rkQ1;
        Quaternion rkP1 = kQ1inv*rkQ2;
        Quaternion kArg = 0.25*(rkP0.Log()-rkP1.Log());
        Quaternion kMinusArg = -kArg;

        rkA = rkQ1*kArg.Exp();
        rkB = rkQ1*kMinusArg.Exp();
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::Intermediate(const Quaternion& q0, const Quaternion& q1, const Quaternion& q2)
	{
		// From advanced Animation and Rendering
		// Techniques by Watt and Watt, Page 366:
		// computing the inner quadrangle 
		// points (qa and qb) to guarantee tangent
		// continuity.
		// 
		Quaternion q1inv = q1.Inverse();
		Quaternion c1 = q1inv * q2;
		Quaternion c2 = q1inv * q0;
		Quaternion c3 = (-0.25) * (c2.Log() + c1.Log());
		Quaternion qa = q1 * c3.Exp();
		qa.normalize();
		return qa;
	}
    //-----------------------------------------------------------------------
    Quaternion Quaternion::Squad (float fT,
        const Quaternion& rkP, const Quaternion& rkA,
        const Quaternion& rkB, const Quaternion& rkQ, bool shortestPath)
    {
        float fSlerpT = 2.0*fT*(1.0-fT);
        Quaternion kSlerpP = Slerp(fT, rkP, rkQ, shortestPath);
        Quaternion kSlerpQ = Slerp(fT, rkA, rkB);
        return Slerp(fSlerpT, kSlerpP ,kSlerpQ);
    }
	//----------------------------------------------------------------------
	void Quaternion::SquadSetup(Quaternion& outA,Quaternion& outB,Quaternion& outC,const Quaternion& q0,const Quaternion& q1,const Quaternion& q2,const Quaternion& q3)
	{
		Quaternion m1 = q0 + q1;
		Quaternion m2 = q0 - q1;
		Quaternion qt0 = ((m1.LengthSq() < m2.LengthSq()) ? -q0 : q0);
		m1 = q1 + q2;
		m2 = q1 - q2;
		Quaternion qt2 = ((m1.LengthSq() < m2.LengthSq()) ? -q2 : q2);
		m1 = q2 + q3;
		m2 = q2 - q3;
		Quaternion qt3 = ((m1.LengthSq() < m2.LengthSq()) ? -q3 : q3);

		//Quaternion qt0 = q0 + q1;
		//qt0.normalize();

		//Quaternion qt2 = q2 + q1;
		//qt2.normalize();

		//Quaternion qt3 = q2 + q3;
		//qt3.normalize();

		Quaternion qt1 = q1;

		//qt1.normalize();
		//qt2.normalize();
		//qt3.normalize();
		//qt0.normalize();

		Quaternion expQt1 = qt1.UnitInverse();					//expQt1.normalize();
		Quaternion expQt2 = qt2.UnitInverse();					//expQt2.normalize();
		Quaternion expQt1Qt2 = expQt1 * qt2;					//expQt1Qt2.normalize();
		Quaternion expQt1Qt0 = expQt1 * qt0;					//expQt1Qt0.normalize();
		Quaternion expQt2Qt3 = expQt2 * qt3;					//expQt2Qt3.normalize();
		Quaternion expQt2Qt1 = expQt2 * qt1;					//expQt2Qt1.normalize();
		Quaternion logExpQt1Qt2 = expQt1Qt2.Log();				//logExpQt1Qt2.normalize();
		Quaternion logExpQt1Qt0 = expQt1Qt0.Log();				//logExpQt1Qt0.normalize();
		Quaternion logExpQt2Qt3 = expQt2Qt3.Log();				//logExpQt2Qt3.normalize();
		Quaternion logExpQt2Qt1 = expQt2Qt1.Log();				//logExpQt2Qt1.normalize();
		Quaternion t1 = -0.25 * (logExpQt1Qt0 + logExpQt1Qt2);	//t1.normalize();
		Quaternion t2 = -0.25 * (logExpQt2Qt1 + logExpQt2Qt3);	//t2.normalize();
		Quaternion t1Exp = t1.Exp();							//t1Exp.normalize();
		Quaternion t2Exp = t2.Exp();							//t2Exp.normalize();

		outA = qt1 * t1Exp;										//outA.normalize();
		outB = qt2 * t2Exp;										//outB.normalize();
		outC = qt2;
	}
    //-----------------------------------------------------------------------
    float Quaternion::normalize(void)
    {
        float len = Norm();
        float factor = 1.0f / Math::Sqrt(len);
        *this = *this * factor;
        return len;
    }
    //-----------------------------------------------------------------------
	Radian Quaternion::getRoll(void) const
	{
		return Radian(Math::ATan2(2*(x*y + w*z), w*w + x*x - y*y - z*z));
	}
    //-----------------------------------------------------------------------
	Radian Quaternion::getPitch(void) const
	{
		return Radian(Math::ATan2(2*(y*z + w*x), w*w - x*x - y*y + z*z));
	}
    //-----------------------------------------------------------------------
	Radian Quaternion::getYaw(void) const
	{
		return Radian(Math::ASin(-2*(x*z - w*y)));
	}
    //-----------------------------------------------------------------------
    Quaternion Quaternion::nlerp(float fT, const Quaternion& rkP, 
        const Quaternion& rkQ, bool shortestPath)
    {
		Quaternion result;
        float fCos = rkP.Dot(rkQ);
		if (fCos < 0.0f && shortestPath)
		{
			result = rkP + fT * ((-rkQ) - rkP);
		}
		else
		{
			result = rkP + fT * (rkQ - rkP);
		}
        result.normalize();
        return result;
    }
    
#if (TARGET_PLATFORM != PLATFORM_WIN32)
    void QuaternionSquad(float, Quaternion const&, Quaternion const&, Quaternion const&, Quaternion const&)
    {
        
    }
#endif
    
    
}
