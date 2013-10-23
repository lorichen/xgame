#include "math\ReMathPrerequisite.h"
#include "math\ReMathInc.h"
#include "math\ReAngle.h"
#include "math\ReMatrix3.h"
#include "math\ReMatrix4.h"
#include "math\RePlane.h"
#include "math\ReVector2.h"
#include "math\ReVector3.h"
#include "math\ReAxisAlignedBox.h"
#include "math\ReSphere.h"
#include "math\ReRay.h"
#include "math\ReQuaternion.h"
#include "math\ReSimpleSpline.h"
#include "math\ReRotationalSpline.h"
#include "math\ReColor4.h"
#include "math\ReColor3.h"
#include "math\ReBitwise.h"
#include "math\CPUMathInfo.h"

#ifdef _DEBUG
	#pragma comment(lib, "xs_mat_d.lib")
#elif RELEASEDEBUG
	#pragma comment(lib, "xs_mat_rd.lib")
#else
	#pragma comment(lib, "xs_mat.lib")
#endif