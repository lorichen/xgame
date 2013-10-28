
#include "ReMathPch.h"
#include "CPUMathInfo.h"

#if RE_COMPILER == RE_COMPILER_MSVC
#include <windows.h>
#endif

namespace xs
{
	bool CPUMathInfo::s_initialize =false;
	uint CPUMathInfo::s_capability =0x00000000;


	uint CPUMathInfo::isOsSupport(CPUMathCapability cap)
	{
#if RE_COMPILER == RE_COMPILER_MSVC
		__try 
		{
			switch ( cap) 
			{
			case CPUMC_SSE:
				__asm 
				{
					xorps xmm0, xmm0        // executing SSE instruction
				}
				break;
			case CPUMC_SSE2:
				__asm 
				{
					xorpd xmm0, xmm0        // executing SSE2 instruction
				}
				break;
			case CPUMC_3DNOW:
				__asm 
				{
					pfrcp mm0, mm0          // executing 3DNow! instruction
						emms
				}
				break;
			case CPUMC_MMX:
				__asm 
				{
					pxor mm0, mm0           // executing MMX instruction
						emms
				}
				break;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			if (_exception_code() == STATUS_ILLEGAL_INSTRUCTION) 
			{
				return 0;
			}
			return 0;
		}
		return 1;
#else
		return 0
#endif

	}

// These are the bit flags that get set on calling CPUID
// with register eax set to 1
#define _MMX_FEATURE_BIT        0x00800000
#define _SSE_FEATURE_BIT        0x02000000
#define _SSE2_FEATURE_BIT       0x04000000

// This bit is set when CPUID is called with
// register set to 80000001h (only applicable to AMD)
#define _3DNOW_FEATURE_BIT      0x80000000

	void CPUMathInfo::initializeCPUCapability()
	{
		uint uiFeature = 0;//标准功能
		uint uiExt = 0;//扩展功能
		_asm {
				push ebx
				push ecx
				push edx

				// get the Standard bits
				mov eax, 1
				CPUID
				mov uiFeature, edx

				// get AMD-specials
				mov eax, 80000000h
				CPUID
				cmp eax, 80000000h
				jc notamd
				mov eax, 80000001h
				CPUID
				mov uiExt, edx

notamd:
				pop ecx
				pop ebx
				pop edx
		}

		if (uiFeature & _MMX_FEATURE_BIT) 
		{
			if( isOsSupport(CPUMC_MMX) )
			{
				s_capability |= CPUMC_MMX;
			}
		}

		if( uiFeature & _SSE_FEATURE_BIT )
		{
			if( isOsSupport(CPUMC_SSE) )
			{
				s_capability |= CPUMC_SSE;
			}
		}

		if( uiFeature & _SSE2_FEATURE_BIT )
		{
			if( isOsSupport(CPUMC_SSE2) )
			{
				s_capability |= CPUMC_SSE2;
			}
		}

		if (uiExt & _3DNOW_FEATURE_BIT) 
		{
			if( isOsSupport(CPUMC_3DNOW) )
			{
				s_capability |= CPUMC_3DNOW;
			}
		}
	}

	uint CPUMathInfo::getCapability()
	{
		if(!s_initialize)
		{
			initializeCPUCapability();
			s_initialize = true;
		}

		return s_capability;
	}
}