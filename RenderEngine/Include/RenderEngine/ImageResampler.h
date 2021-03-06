
#ifndef __ImageResampler_H__
#define __ImageResampler_H__

#include <algorithm>

// this file is inlined into Image.cpp!
// do not include anywhere else.
namespace xs 
{
	/*! \addtogroup RenderEngine
	*  ��Ⱦ����
	*  @{
	*/


// variable name hints:
// sx_48 = 16/48-bit fixed-point x-position in source
// stepx = difference between adjacent sx_48 values
// sx1 = lower-bound integer x-position in source
// sx2 = upper-bound integer x-position in source
// sxf = fractional weight beween sx1 and sx2
// x,y,z = location of output pixel in destination

// nearest-neighbor resampler, does not convert formats.
// templated on bytes-per-pixel to allow compiler optimizations, such
// as simplifying memcpy() and replacing multiplies with bitshifts
template<uint elemsize> struct NearestResampler {
	static void  scale(const PixelBox& src, const PixelBox& dst) {
		// ASSERT(src.format == dst.format);

		// srcdata stays at beginning, pdst is a moving pointer
		uchar* srcdata = (uchar*)src.data;
		uchar* pdst = (uchar*)dst.data;

		// sx_48,sy_48,sz_48 represent current position in source
		// using 16/48-bit fixed precision, incremented by steps
		uint64 stepx = ((uint64)src.getWidth() << 48) / dst.getWidth();
		uint64 stepy = ((uint64)src.getHeight() << 48) / dst.getHeight();
		uint64 stepz = ((uint64)src.getDepth() << 48) / dst.getDepth();

		// note: ((stepz>>1) - 1) is an extra half-step increment to adjust
		// for the center of the destination pixel, not the top-left corner
		uint64 sz_48 = (stepz >> 1) - 1;
		for (size_t z = dst.front; z < dst.back; z++, sz_48 += stepz) {
			size_t srczoff = (size_t)(sz_48 >> 48) * src.slicePitch;
			
			uint64 sy_48 = (stepy >> 1) - 1;
			for (size_t y = dst.top; y < dst.bottom; y++, sy_48 += stepy) {
				size_t srcyoff = (size_t)(sy_48 >> 48) * src.rowPitch;
			
				uint64 sx_48 = (stepx >> 1) - 1;
				for (size_t x = dst.left; x < dst.right; x++, sx_48 += stepx) {
					uchar* psrc = srcdata +
						elemsize*((size_t)(sx_48 >> 48) + srcyoff + srczoff);
                    memcpy(pdst, psrc, elemsize);
					pdst += elemsize;
				}
				pdst += elemsize*dst.getRowSkip();
			}
			pdst += elemsize*dst.getSliceSkip();
		}
	}
};


// default Floating-point linear resampler, does format conversion
struct LinearResampler {
	static void  scale(const PixelBox& src, const PixelBox& dst) {
		size_t srcelemsize = PixelUtil::getNumElemBytes(src.format);
		size_t dstelemsize = PixelUtil::getNumElemBytes(dst.format);

		// srcdata stays at beginning, pdst is a moving pointer
		uchar* srcdata = (uchar*)src.data;
		uchar* pdst = (uchar*)dst.data;
		
		// sx_48,sy_48,sz_48 represent current position in source
		// using 16/48-bit fixed precision, incremented by steps
		uint64 stepx = ((uint64)src.getWidth() << 48) / dst.getWidth();
		uint64 stepy = ((uint64)src.getHeight() << 48) / dst.getHeight();
		uint64 stepz = ((uint64)src.getDepth() << 48) / dst.getDepth();
		
		// temp is 16/16 bit fixed precision, used to adjust a source
		// coordinate (x, y, or z) backwards by half a pixel so that the
		// integer bits represent the first sample (eg, sx1) and the
		// fractional bits are the blend weight of the second sample
		uint temp;

		// note: ((stepz>>1) - 1) is an extra half-step increment to adjust
		// for the center of the destination pixel, not the top-left corner
		uint64 sz_48 = (stepz >> 1) - 1;
		for (size_t z = dst.front; z < dst.back; z++, sz_48+=stepz) {
			temp = sz_48 >> 32;
			temp = (temp > 0x8000)? temp - 0x8000 : 0;
			size_t sz1 = temp >> 16;				 // src z, sample #1
			size_t sz2 = std::min((uint)sz1+1,src.getDepth()-1);// src z, sample #2
			float szf = (temp & 0xFFFF) / 65536.f; // weight of sample #2

			uint64 sy_48 = (stepy >> 1) - 1;
			for (size_t y = dst.top; y < dst.bottom; y++, sy_48+=stepy) {
				temp = sy_48 >> 32;
				temp = (temp > 0x8000)? temp - 0x8000 : 0;
				size_t sy1 = temp >> 16;					// src y #1
				size_t sy2 = std::min((uint)sy1+1,src.getHeight()-1);// src y #2
				float syf = (temp & 0xFFFF) / 65536.f; // weight of #2
				
				uint64 sx_48 = (stepx >> 1) - 1;
				for (size_t x = dst.left; x < dst.right; x++, sx_48+=stepx) {
					temp = sx_48 >> 32;
					temp = (temp > 0x8000)? temp - 0x8000 : 0;
					size_t sx1 = temp >> 16;					// src x #1
					size_t sx2 = std::min((uint)sx1+1,src.getWidth()-1);// src x #2
					float sxf = (temp & 0xFFFF) / 65536.f; // weight of #2
				
					ColorValue x1y1z1, x2y1z1, x1y2z1, x2y2z1;
					ColorValue x1y1z2, x2y1z2, x1y2z2, x2y2z2;

#define UNPACK(dst,x,y,z) PixelUtil::unpackColor(&dst, src.format, \
	srcdata + srcelemsize*((x)+(y)*src.rowPitch+(z)*src.slicePitch))

					UNPACK(x1y1z1,sx1,sy1,sz1); UNPACK(x2y1z1,sx2,sy1,sz1);
					UNPACK(x1y2z1,sx1,sy2,sz1); UNPACK(x2y2z1,sx2,sy2,sz1);
					UNPACK(x1y1z2,sx1,sy1,sz2); UNPACK(x2y1z2,sx2,sy1,sz2);
					UNPACK(x1y2z2,sx1,sy2,sz2); UNPACK(x2y2z2,sx2,sy2,sz2);
#undef UNPACK

					ColorValue accum =
						x1y1z1 * ((1.0f - sxf)*(1.0f - syf)*(1.0f - szf)) +
						x2y1z1 * (        sxf *(1.0f - syf)*(1.0f - szf)) +
						x1y2z1 * ((1.0f - sxf)*        syf *(1.0f - szf)) +
						x2y2z1 * (        sxf *        syf *(1.0f - szf)) +
						x1y1z2 * ((1.0f - sxf)*(1.0f - syf)*        szf ) +
						x2y1z2 * (        sxf *(1.0f - syf)*        szf ) +
						x1y2z2 * ((1.0f - sxf)*        syf *        szf ) +
						x2y2z2 * (        sxf *        syf *        szf );

					PixelUtil::packColor(accum, dst.format, pdst);

					pdst += dstelemsize;
				}
				pdst += dstelemsize*dst.getRowSkip();
			}
			pdst += dstelemsize*dst.getSliceSkip();
		}
	}
};


// Float32 linear resampler, converts FLOAT32_RGB/FLOAT32_RGBA only.
// avoids overhead of pixel unpack/repack function calls
struct LinearResampler_Float32 {
	static void  scale(const PixelBox& src, const PixelBox& dst) {
		size_t srcchannels = PixelUtil::getNumElemBytes(src.format) / sizeof(float);
		size_t dstchannels = PixelUtil::getNumElemBytes(dst.format) / sizeof(float);
		// ASSERT(srcchannels == 3 || srcchannels == 4);
		// ASSERT(dstchannels == 3 || dstchannels == 4);

		// srcdata stays at beginning, pdst is a moving pointer
		float* srcdata = (float*)src.data;
		float* pdst = (float*)dst.data;
		
		// sx_48,sy_48,sz_48 represent current position in source
		// using 16/48-bit fixed precision, incremented by steps
		uint64 stepx = ((uint64)src.getWidth() << 48) / dst.getWidth();
		uint64 stepy = ((uint64)src.getHeight() << 48) / dst.getHeight();
		uint64 stepz = ((uint64)src.getDepth() << 48) / dst.getDepth();
		
		// temp is 16/16 bit fixed precision, used to adjust a source
		// coordinate (x, y, or z) backwards by half a pixel so that the
		// integer bits represent the first sample (eg, sx1) and the
		// fractional bits are the blend weight of the second sample
		uint temp;

		// note: ((stepz>>1) - 1) is an extra half-step increment to adjust
		// for the center of the destination pixel, not the top-left corner
		uint64 sz_48 = (stepz >> 1) - 1;
		for (size_t z = dst.front; z < dst.back; z++, sz_48+=stepz) {
			temp = sz_48 >> 32;
			temp = (temp > 0x8000)? temp - 0x8000 : 0;
			size_t sz1 = temp >> 16;				 // src z, sample #1
			size_t sz2 = std::min((uint)sz1+1,src.getDepth()-1);// src z, sample #2
			float szf = (temp & 0xFFFF) / 65536.f; // weight of sample #2

			uint64 sy_48 = (stepy >> 1) - 1;
			for (size_t y = dst.top; y < dst.bottom; y++, sy_48+=stepy) {
				temp = sy_48 >> 32;
				temp = (temp > 0x8000)? temp - 0x8000 : 0;
				size_t sy1 = temp >> 16;					// src y #1
				size_t sy2 = std::min((uint)sy1+1,src.getHeight()-1);// src y #2
				float syf = (temp & 0xFFFF) / 65536.f; // weight of #2
				
				uint64 sx_48 = (stepx >> 1) - 1;
				for (size_t x = dst.left; x < dst.right; x++, sx_48+=stepx) {
					temp = sx_48 >> 32;
					temp = (temp > 0x8000)? temp - 0x8000 : 0;
					size_t sx1 = temp >> 16;					// src x #1
					size_t sx2 = std::min((uint)sx1+1,src.getWidth()-1);// src x #2
					float sxf = (temp & 0xFFFF) / 65536.f; // weight of #2
					
					// process R,G,B,A simultaneously for cache coherence?
					float accum[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

#define ACCUM3(x,y,z,factor) \
	{ float f = factor; \
	size_t off = (x+y*src.rowPitch+z*src.slicePitch)*srcchannels; \
    accum[0]+=srcdata[off+0]*f; accum[1]+=srcdata[off+1]*f; \
	accum[2]+=srcdata[off+2]*f; }

#define ACCUM4(x,y,z,factor) \
	{ float f = factor; \
	size_t off = (x+y*src.rowPitch+z*src.slicePitch)*srcchannels; \
    accum[0]+=srcdata[off+0]*f; accum[1]+=srcdata[off+1]*f; \
	accum[2]+=srcdata[off+2]*f; accum[3]+=srcdata[off+3]*f; }

					if (srcchannels == 3 || dstchannels == 3) {
						// RGB, no alpha
						ACCUM3(sx1,sy1,sz1,(1.0f-sxf)*(1.0f-syf)*(1.0f-szf));
						ACCUM3(sx2,sy1,sz1,      sxf *(1.0f-syf)*(1.0f-szf));
						ACCUM3(sx1,sy2,sz1,(1.0f-sxf)*      syf *(1.0f-szf));
						ACCUM3(sx2,sy2,sz1,      sxf *      syf *(1.0f-szf));
						ACCUM3(sx1,sy1,sz2,(1.0f-sxf)*(1.0f-syf)*      szf );
						ACCUM3(sx2,sy1,sz2,      sxf *(1.0f-syf)*      szf );
						ACCUM3(sx1,sy2,sz2,(1.0f-sxf)*      syf *      szf );
						ACCUM3(sx2,sy2,sz2,      sxf *      syf *      szf );
						accum[3] = 1.0f;
					} else {
						// RGBA
						ACCUM4(sx1,sy1,sz1,(1.0f-sxf)*(1.0f-syf)*(1.0f-szf));
						ACCUM4(sx2,sy1,sz1,      sxf *(1.0f-syf)*(1.0f-szf));
						ACCUM4(sx1,sy2,sz1,(1.0f-sxf)*      syf *(1.0f-szf));
						ACCUM4(sx2,sy2,sz1,      sxf *      syf *(1.0f-szf));
						ACCUM4(sx1,sy1,sz2,(1.0f-sxf)*(1.0f-syf)*      szf );
						ACCUM4(sx2,sy1,sz2,      sxf *(1.0f-syf)*      szf );
						ACCUM4(sx1,sy2,sz2,(1.0f-sxf)*      syf *      szf );
						ACCUM4(sx2,sy2,sz2,      sxf *      syf *      szf );
					}

					memcpy(pdst, accum, sizeof(float)*dstchannels);

#undef ACCUM3
#undef ACCUM4

					pdst += dstchannels;
				}
				pdst += dstchannels*dst.getRowSkip();
			}
			pdst += dstchannels*dst.getSliceSkip();
		}
	}
};



// uchar linear resampler, does not do any format conversions.
// only handles pixel formats that use 1 uchar per color channel.
// 2D only; punts 3D pixelboxes to default LinearResampler (slow).
// templated on bytes-per-pixel to allow compiler optimizations, such
// as unrolling loops and replacing multiplies with bitshifts
template<uint channels> struct LinearResampler_Byte {
	static void  scale(const PixelBox& src, const PixelBox& dst) {
		// ASSERT(src.format == dst.format);

		// only optimized for 2D
		if (src.getDepth() > 1 || dst.getDepth() > 1) {
			LinearResampler::scale(src, dst);
			return;
		}

		// srcdata stays at beginning of slice, pdst is a moving pointer
		uchar* srcdata = (uchar*)src.data;
		uchar* pdst = (uchar*)dst.data;

		// sx_48,sy_48 represent current position in source
		// using 16/48-bit fixed precision, incremented by steps
		uint64 stepx = ((uint64)src.getWidth() << 48) / dst.getWidth();
		uint64 stepy = ((uint64)src.getHeight() << 48) / dst.getHeight();
		
		// bottom 28 bits of temp are 16/12 bit fixed precision, used to
		// adjust a source coordinate backwards by half a pixel so that the
		// integer bits represent the first sample (eg, sx1) and the
		// fractional bits are the blend weight of the second sample
		uint temp;
		
		uint64 sy_48 = (stepy >> 1) - 1;
		for (size_t y = dst.top; y < dst.bottom; y++, sy_48+=stepy) {
			temp = sy_48 >> 36;
			temp = (temp > 0x800)? temp - 0x800: 0;
			uint syf = temp & 0xFFF;
			size_t sy1 = temp >> 12;
			size_t sy2 = std::min((uint)sy1+1, src.bottom-src.top-1);
			size_t syoff1 = sy1 * src.rowPitch;
			size_t syoff2 = sy2 * src.rowPitch;

			uint64 sx_48 = (stepx >> 1) - 1;
			for (size_t x = dst.left; x < dst.right; x++, sx_48+=stepx) {
				temp = sx_48 >> 36;
				temp = (temp > 0x800)? temp - 0x800 : 0;
				uint sxf = temp & 0xFFF;
				size_t sx1 = temp >> 12;
				size_t sx2 = std::min((uint)sx1+1, src.right-src.left-1);

				uint sxfsyf = sxf*syf;
				for (uint k = 0; k < channels; k++) {
					uint accum =
						srcdata[(sx1 + syoff1)*channels+k]*(0x1000000-(sxf<<12)-(syf<<12)+sxfsyf) +
						srcdata[(sx2 + syoff1)*channels+k]*((sxf<<12)-sxfsyf) +
						srcdata[(sx1 + syoff2)*channels+k]*((syf<<12)-sxfsyf) +
						srcdata[(sx2 + syoff2)*channels+k]*sxfsyf;
					// accum is computed using 8/24-bit fixed-point math
					// (maximum is 0xFF000000; rounding will not cause overflow)
					*pdst++ = (accum + 0x800000) >> 24;
				}
			}
			pdst += channels*dst.getRowSkip();
		}
	}
};
	/** @} */

}

#endif
