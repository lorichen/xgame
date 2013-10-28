/*******************************************************************
** 文件名:	e:\CoGame\Demo\src\Character\RandGenerator.h
** 版  权:	(C) 
** 
** 日  期:	2007/9/13  16:04
** 版  本:	1.0
** 描  述:	随机数发生器，使用前须播种子CRandGenerator::Seed
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#define GENIUS_NUMBER 0x376EAC5D

#if (TARGET_PLATFORM == PLATFORM_WIN32)

#else
#include "Api.h"
#endif

class CRandGenerator
{
	enum 
	{
		Number = 0x1000, 
	};
public:
	/** 固定种子
	@param   
	@param   
	@return  
	*/
	void Seed(DWORD dwSeed)
	{
		m_Seed = dwSeed^GENIUS_NUMBER;
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		Batch();
#else
        srand(m_Seed);
#endif
	}

	/** 随机种子
	@param   
	@param   
	@return  
	*/
	DWORD Seed(void)
	{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		int s;
		__asm 
		{
			_asm _emit 0x0f _asm _emit 0x31
				mov s,eax
		}
        
#else
        int s = xs::getTickCount();
#endif

		Seed(s);

		return s^GENIUS_NUMBER;
	}

	/** 
	@param   
	@param   
	@return  
	*/
	DWORD GetSeed(void) const
	{
		return m_Seed^GENIUS_NUMBER;
	}
	
	/** 产生一个WORD
	@param   
	@param   
	@return  
	*/
	WORD GetWORD(void)
	{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		if(m_Ptr >= Number * 2)
		{
			Batch();
			m_Ptr = 0;
		}

		return *((WORD *)m_pBuffer + m_Ptr++);
#else
        return rand();
#endif
	};

	/** 产生一个DWord
	@param   
	@param   
	@return  
	*/
	DWORD GetDWORD(void)
	{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		if(m_Ptr >= Number * 2 - 1)
		{
			Batch();
			m_Ptr=0;
		}	

		m_Ptr += 2;
		return *(DWORD *)((WORD *)m_pBuffer + m_Ptr - 2);
#else
        WORD r1 = rand();
        WORD r2 = rand();
        return ((r1 << 16) | r2);
#endif
	}

	/** 
	@param   
	@param   
	@return  
	*/
	CRandGenerator(void)
    :m_pBuffer(0)
	{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		int s;
		m_pBuffer = new DWORD[Number];
		m_Ptr = Number * 2;
		__asm
		{
			_asm _emit 0x0f _asm _emit 0x31
				mov s,eax
		}

		m_Seed = s;
#else
        unsigned long s;
        s = xs::getTickCount();
        srand(s);
        m_Seed = s;
#endif
	}

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CRandGenerator(void)
	{
        if(m_pBuffer)
            delete[] m_pBuffer;
	}

private:
	/** 产生一批
	@param   
	@param   
	@return  
	*/
	void		Batch(void)
	{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		__asm 
		{
			cld
				mov edi,[ecx]CRandGenerator.m_pBuffer
				mov eax,[ecx]CRandGenerator.m_Seed
				mov ecx,0x1000		;//WRandom::Number
			mov esi,0xE7BD2160

				ALIGN 4
_loop_circle1:
			push ecx
				mov ecx,32
_loop_circle2:
			mov ebx,eax
				shl eax,1
				and ebx,esi;	//select the bit for xor
			mov edx,ebx
				bswap ebx
				xor bx,dx
				xor bh,bl;		// because P only judge one byte
			;// so must XOR to judge the p of whole word 
			jp _next;		//jp equals the xor 
			inc eax
_next:
			dec ecx
				jnz _loop_circle2
				pop ecx
				stosd
				dec ecx
				jnz _loop_circle1
				mov ecx,this
				mov [ecx]CRandGenerator.m_Seed,eax
		}
#else
        
#endif
	}

private:
	DWORD *		m_pBuffer;
	DWORD		m_Seed;
	unsigned	m_Ptr;
};

extern CRandGenerator g_RandGenerator;
