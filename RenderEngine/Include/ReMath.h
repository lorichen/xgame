#include "Math/ReMathPrerequisite.h"
#include "Math/ReMathInc.h"
#include "Math/ReAngle.h"
#include "Math/ReMatrix3.h"
#include "Math/ReMatrix4.h"
#include "Math/RePlane.h"
#include "Math/ReVector2.h"
#include "Math/ReVector3.h"
#include "Math/ReAxisAlignedBox.h"
#include "Math/ReSphere.h"
#include "Math/ReRay.h"
#include "Math/ReQuaternion.h"
#include "Math/ReSimpleSpline.h"
#include "Math/ReRotationalSpline.h"
#include "Math/ReColor4.h"
#include "Math/ReColor3.h"
#include "Math/ReBitwise.h"
#include "Math/CPUMathInfo.h"

#ifdef _DEBUG
	#pragma comment(lib, "xs_mat_d.lib")
#elif RELEASEDEBUG
	#pragma comment(lib, "xs_mat_rd.lib")
#else
	#pragma comment(lib, "xs_mat.lib")
#endif