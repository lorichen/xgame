#if defined(_MSC_VER)
	#define RE_COMPILER RE_COMPILER_MSVC
	#define RE_COMP_VER _MSC_VER
#elif defined(__GNUC__)
	#define RE_COMPILER RE_COMPILER_GNUC
	#define RE_COMP_VER (((__GNUC__) * 100) + (__GNUC_MINOR__ * 10) + __GNUC_PATCHLEVEL__)
#elif defined(__BORLANDC__)
	#define RE_COMPILER RE_COMPILER_BORL
	#define RE_COMP_VER __BCPLUSPLUS__
#else
	#pragma error "Unknown Compiler!"
#endif

#if RE_COMPILER == RE_COMPILER_MSVC 
	#if RE_COMP_VER >= 1200
		#define FORCEINLINE __forceinline
	#endif
#else
	#define FORCEINLINE __inline
#endif

namespace xs
{
	class Math;
	class Degree;
	class Radian;
	class Ray;
	class Bitwise;
	class Matrix3;
	class Matrix4;
	class Vector2;
	class Vector3;
	class Vector4;
	class Sphere;
	class SimpleSpline;
	class RotationalSpline;
	class AABB;
	class Quaternion;
	class Plane;
	class Color4;
	class Color3;

};