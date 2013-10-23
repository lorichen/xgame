#ifndef __CPUMathInfo_H__
#define __CPUMathInfo_H__
#include "ReMathPrerequisite.h"

namespace xs
{
	/* 获取CPU关于数学计算的功能，包括MMX，SSE，3DNow等
	*/
	enum	CPUMathCapability
	{
		CPUMC_MMX = 0x00000001,//支持mmx
		CPUMC_SSE = 0x00000002,//支持sse
		CPUMC_SSE2 = 0x00000004,//支持sse2
		CPUMC_3DNOW = 0x00000008,//支持3dNow
	};
	class _ReMathExport CPUMathInfo
	{
		static bool s_initialize;//是否初始化cpu数学运算功能
		static uint s_capability;//cpu性能
	public:
		static uint getCapability();//获取性能
	private:
		static void initializeCPUCapability();//初始化cpu数学运算信息
		static uint isOsSupport(CPUMathCapability cap);//操作系统是否支持CPU的这种功能
	};
}
#endif