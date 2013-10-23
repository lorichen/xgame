#ifndef __Interpolate_H__
#define __Interpolate_H__

namespace xs
{
	/** 线性插值
	*/
	template<class T>
	inline T interpolate(const float r,const T &v1,const T &v2)
	{
		T v = v1 * (1.0f - r) + v2 * r;
		return v;
	}

	/** Hermite插值
	*/
	template<class T>
	inline T interpolateHermite(const float Factor,const T &v1,const T &v2,const T &in,const T &out)
	{
		float Factor1;
		float Factor2;
		float Factor3;
		float Factor4;
		float FactorTimesTwo;

		FactorTimesTwo = Factor * Factor;

		Factor1 = FactorTimesTwo * (2.0f * Factor - 3.0f) + 1;
		Factor2 = FactorTimesTwo * (Factor - 2.0f) + Factor;
		Factor3 = FactorTimesTwo * (Factor - 1.0f);
		Factor4 = FactorTimesTwo * (3.0f - 2.0f * Factor);

		return (Factor1 * v1) + (Factor2 * out) + (Factor3 * in) + (Factor4 * v2);

		float r = Factor;
		float r2 = r * r;
		float r3 = r2 * r;
		float r2_3 = r2 * 3.0;
		float r3_2 = 2.0 * r3;

		float h1 = r3_2 - r2_3 + 1.0f;
		float h2 = -r3_2 + r2_3;
		float h3 = r3 - 2.0 * r2 + r;
		float h4 = r3 - r2;

		// interpolation
		T v = v1 * h1 + v2 * h2 + in * h3 + out * h4;
		return v;
	}

	/** Bezier插值
	*/
	template<class T>
	inline T interpolateBezier(const float r,const T &v1,const T &v2,const T &in,const T &out)
	{
		float r1;
		float r2;
		float r3;
		float r4;
		float r_2;
		float rr_2;

		float rr = 1 - r;

		r_2 = r * r;
		rr_2 = rr * rr;

		r1 = rr_2 * rr;
		r2 = 3.0f * r * rr_2;
		r3 = 3.0f * r_2 * rr;
		r4 = r_2 * r;

		return (r1 * v1) + (r2 * out) + (r3 * in) + (r4 * v2);
	}

	/** 四元数Sphere插值
	*/
	template<>
	inline Quaternion interpolate<Quaternion>(const float r,const Quaternion &v1,const Quaternion &v2)
	{
		return Quaternion::Slerp(r,v1,v2,true);
	}

	extern Quaternion QuaternionSquad ( float r, const Quaternion&, const Quaternion&, const Quaternion&, const Quaternion& );

	/** 四元数Bezier插值
	*/
	template<>
	inline Quaternion interpolateBezier<Quaternion>(const float r,const Quaternion &v1,const Quaternion &in,const Quaternion &v2,const Quaternion &out)
	{
		return QuaternionSquad(r,v1,in,v2,out);
	}

	/** 四元数Hermite插值
	*/
	template<>
	inline Quaternion interpolateHermite<Quaternion>(const float r,const Quaternion &v1,const Quaternion &in,const Quaternion &v2,const Quaternion &out)
	{
		return interpolateBezier<Quaternion>(r,v1,in,v2,out);
	}
}

#endif