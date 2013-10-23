#ifndef _Bitwise_H__
#define _Bitwise_H__

#include "ReMathPrerequisite.h"

namespace xs {
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/
	

    /** Class for manipulating bit patterns.
    */
    class _ReMathExport Bitwise {
    public:
        /** Returns the number of bits a pattern must be shifted right by to
            remove right-hand zeroes.
        */
		template<typename T>
        static FORCEINLINE unsigned int getBitShift(T mask)
		{
			if (mask == 0)
				return 0;

			unsigned int result = 0;
			while ((mask & 1) == 0) {
				++result;
				mask >>= 1;
			}
			return result;
		}

        /** Takes a value with a given src bit mask, and produces another
            value with a desired bit mask.
            @remarks
                This routine is useful for colour conversion.
        */
		template<typename SrcT, typename DestT>
        static inline DestT convertBitPattern(SrcT srcValue, SrcT srcBitMask, DestT destBitMask)
		{
			// Mask off irrelevant source value bits (if any)
			srcValue = srcValue & srcBitMask;

			// Shift source down to bottom of DWORD
			const unsigned int srcBitShift = getBitShift(srcBitMask);
			srcValue >>= srcBitShift;

			// Get max value possible in source from srcMask
			const SrcT srcMax = srcBitMask >> srcBitShift;

			// Get max avaiable in dest
			const unsigned int destBitShift = getBitShift(destBitMask);
			const DestT destMax = destBitMask >> destBitShift;

			// Scale source value into destination, and shift back
			DestT destValue = (srcValue * destMax) / srcMax;
			return (destValue << destBitShift);
		}

        /**
         * Convert N bit colour channel value to P bits. It fills P bits with the
         * bit pattern repeated. (this is /((1<<n)-1) in fixed point)
         */
        static inline unsigned int fixedToFixed(uint value, unsigned int n, unsigned int p) 
        {
            if(n > p) 
            {
                // Less bits required than available; this is easy
                value >>= n-p;
            } 
            else if(n < p)
            {
                // More bits required than are there, do the fill
                // Use old fashioned division, probably better than a loop
                if(value == 0)
                        value = 0;
                else if(value == (static_cast<unsigned int>(1)<<n)-1)
                        value = (1<<p)-1;
                else    value = value*(1<<p)/((1<<n)-1);
            }
            return value;    
        }

        /**
         * Convert floating point colour channel value between 0.0 and 1.0 (otherwise clamped) 
         * to integer of a certain number of bits. Works for any value of bits between 0 and 31.
         */
        static inline unsigned int floatToFixed(const float value, const unsigned int bits)
        {
            if(value <= 0.0f) return 0;
            else if (value >= 1.0f) return (1<<bits)-1;
            else return (unsigned int)(value * (1<<bits));     
        }

        /**
         * Fixed point to float
         */
        static inline float fixedToFloat(unsigned value, unsigned int bits)
        {
            return (float)value/(float)((1<<bits)-1);
        }

        /**
         * Write a n*8 bits integer value to memory in native endian.
         */
        static inline void intWrite(const void *dest, const int n, const unsigned int value)
        {
            switch(n) {
                case 1:
                    ((uchar*)dest)[0] = (uchar)value;
                    break;
                case 2:
                    ((ushort*)dest)[0] = (ushort)value;
                    break;
                case 3:
#if RE_ENDIAN == RE_ENDIAN_BIG      
                    ((uchar*)dest)[0] = (uchar)((value >> 16) & 0xFF);
                    ((uchar*)dest)[1] = (uchar)((value >> 8) & 0xFF);
                    ((uchar*)dest)[2] = (uchar)(value & 0xFF);
#else
                    ((uchar*)dest)[2] = (uchar)((value >> 16) & 0xFF);
                    ((uchar*)dest)[1] = (uchar)((value >> 8) & 0xFF);
                    ((uchar*)dest)[0] = (uchar)(value & 0xFF);
#endif
                    break;
                case 4:
                    ((uint*)dest)[0] = (uint)value;                
                    break;                
            }        
        }
        /**
         * Read a n*8 bits integer value to memory in native endian.
         */
        static inline unsigned int intRead(const void *src, int n) {
            switch(n) {
                case 1:
                    return ((uchar*)src)[0];
                case 2:
                    return ((ushort*)src)[0];
                case 3:
#if RE_ENDIAN == RE_ENDIAN_BIG      
                    return ((uint)((uchar*)src)[0]<<16)|
                            ((uint)((uchar*)src)[1]<<8)|
                            ((uint)((uchar*)src)[2]);
#else
                    return ((uint)((uchar*)src)[0])|
                            ((uint)((uchar*)src)[1]<<8)|
                            ((uint)((uchar*)src)[2]<<16);
#endif
                case 4:
                    return ((uint*)src)[0];
            } 
            return 0; // ?
        }

        /** Convert a float32 to a float16 (NV_half_float)
         	Courtesy of OpenEXR
        */
        static inline ushort floatToHalf(float i)
        {
            return floatToHalfI(*reinterpret_cast<uint*>(&i));
        }
		/** Converts float in uint format to a a half in ushort format
		*/
        static inline ushort floatToHalfI(uint i)
        {
            register int s =  (i >> 16) & 0x00008000;
            register int e = ((i >> 23) & 0x000000ff) - (127 - 15);
            register int m =   i        & 0x007fffff;
        
            if (e <= 0)
            {
                if (e < -10)
                {
                    return 0;
                }
                m = (m | 0x00800000) >> (1 - e);
        
                return s | (m >> 13);
            }
            else if (e == 0xff - (127 - 15))
            {
                if (m == 0) // Inf
                {
                    return s | 0x7c00;
                } 
                else    // NAN
                {
                    m >>= 13;
                    return s | 0x7c00 | m | (m == 0);
                }
            }
            else
            {
                if (e > 30) // Overflow
                {
                    return s | 0x7c00;
                }
        
                return s | (e << 10) | (m >> 13);
            }
        }
        
        /**
         * Convert a float16 (NV_half_float) to a float32
         * Courtesy of OpenEXR
         */
        static inline float halfToFloat(ushort y)
        {
            uint r = halfToFloatI(y);
            return *reinterpret_cast<float*>(&r);
        }
		/** Converts a half in ushort format to a float
		 	in uint format
		 */
        static inline uint halfToFloatI(ushort y)
        {
            register int s = (y >> 15) & 0x00000001;
            register int e = (y >> 10) & 0x0000001f;
            register int m =  y        & 0x000003ff;
        
            if (e == 0)
            {
                if (m == 0) // Plus or minus zero
                {
                    return s << 31;
                }
                else // Denormalized number -- renormalize it
                {
                    while (!(m & 0x00000400))
                    {
                        m <<= 1;
                        e -=  1;
                    }
        
                    e += 1;
                    m &= ~0x00000400;
                }
            }
            else if (e == 31)
            {
                if (m == 0) // Inf
                {
                    return (s << 31) | 0x7f800000;
                }
                else // NaN
                {
                    return (s << 31) | 0x7f800000 | (m << 13);
                }
            }
        
            e = e + (127 - 15);
            m = m << 13;
        
            return (s << 31) | (e << 23) | m;
        }
         

    };
	/** @} */
}

#endif
