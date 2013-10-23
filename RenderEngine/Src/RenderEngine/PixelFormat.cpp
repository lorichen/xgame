#include "StdAfx.h"
#include "RenderEngineCommon.h"
#include "PixelFormat.h"

namespace {
#include "PixelConversions.h"
};

namespace xs 
{

    //-----------------------------------------------------------------------
    /**
    * A record that describes a pixel format in detail.
    */
    struct PixelFormatDescription {
        /* Name of the format, as in the enum */
        char *name;
        /* Number of bytes one element (colour value) takes. */
        uint elemBytes;
        /* Pixel format flags, see enum PixelFormatFlags for the bit field
        * definitions 
        */
        uint flags;
        /* Number of bits in one element. */
        // int elemBits; == elemBytes * 8
        /* Number of bits for red(or luminance), green, blue, alpha
        */
        int rbits,gbits,bbits,abits; /*, ibits, dbits, ... */

        /* Masks and shifts as used by packers/unpackers */
        uint rmask, gmask, bmask, amask;
        int rshift, gshift, bshift, ashift;
    };
    //-----------------------------------------------------------------------
    /** Pixel format database */
    PixelFormatDescription _pixelFormats[PF_COUNT] = {
	//-----------------------------------------------------------------------0
        {"PF_UNKNOWN", 
            /* Bytes per element */ 
            0,  
            /* Flags */
            0,  
            /* rbits, gbits, bbits, abits */
            0, 0, 0, 0,
            /* Masks and shifts */
            0, 0, 0, 0, 0, 0, 0, 0 
        },
	//-----------------------------------------------------------------------1
        {"PF_L8", 
        /* Bytes per element */ 
        1,  
        /* Flags */
        PFF_LUMINANCE | PFF_NATIVEENDIAN,
        /* rbits, gbits, bbits, abits */
        8, 0, 0, 0,
        /* Masks and shifts */
        0xFF, 0, 0, 0, 0, 0, 0, 0 
        },
	//-----------------------------------------------------------------------2
        {"PF_L16", 
        /* Bytes per element */ 
        2,  
        /* Flags */
        PFF_LUMINANCE | PFF_NATIVEENDIAN,  
        /* rbits, gbits, bbits, abits */
        16, 0, 0, 0,
        /* Masks and shifts */
        0xFFFF, 0, 0, 0, 0, 0, 0, 0 
        },
	//-----------------------------------------------------------------------3
        {"PF_A8", 
        /* Bytes per element */ 
        1,  
        /* Flags */
        PFF_HASALPHA | PFF_NATIVEENDIAN,
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 8,
        /* Masks and shifts */
        0, 0, 0, 0xFF, 0, 0, 0, 0 
        },
	//-----------------------------------------------------------------------4
        {"PF_A4L4", 
        /* Bytes per element */ 
        1,  
        /* Flags */
        PFF_HASALPHA | PFF_LUMINANCE | PFF_NATIVEENDIAN,
        /* rbits, gbits, bbits, abits */
        4, 0, 0, 4,
        /* Masks and shifts */
        0x0F, 0, 0, 0xF0, 0, 0, 0, 4
        },
	//-----------------------------------------------------------------------5
        {"PF_BYTE_LA", 
        /* Bytes per element */ 
        2,  
        /* Flags */
        PFF_HASALPHA | PFF_LUMINANCE,  
        /* rbits, gbits, bbits, abits */
        8, 0, 0, 8,
        /* Masks and shifts */
        0,0,0,0,0,0,0,0
        },
	//-----------------------------------------------------------------------6
        {"PF_R5G6B5", 
        /* Bytes per element */ 
        2,  
        /* Flags */
        PFF_NATIVEENDIAN,  
        /* rbits, gbits, bbits, abits */
        5, 6, 5, 0,
        /* Masks and shifts */
        0xF800, 0x07E0, 0x001F, 0, 
        11, 5, 0, 0 
        },
	//-----------------------------------------------------------------------7
		{"PF_B5G6R5", 
        /* Bytes per element */ 
        2,  
        /* Flags */
        PFF_NATIVEENDIAN,  
        /* rbits, gbits, bbits, abits */
        5, 6, 5, 0,
        /* Masks and shifts */
        0x001F, 0x07E0, 0xF800, 0, 
        0, 5, 11, 0 
        },
	//-----------------------------------------------------------------------8
        {"PF_A4R4G4B4", 
        /* Bytes per element */ 
        2,  
        /* Flags */
        PFF_HASALPHA | PFF_NATIVEENDIAN,  
        /* rbits, gbits, bbits, abits */
        4, 4, 4, 4,
        /* Masks and shifts */
        0x0F00, 0x00F0, 0x000F, 0xF000, 
        8, 4, 0, 12 
        },
	//-----------------------------------------------------------------------9
        {"PF_A1R5G5B5", 
        /* Bytes per element */ 
        2,  
        /* Flags */
        PFF_HASALPHA | PFF_NATIVEENDIAN,  
        /* rbits, gbits, bbits, abits */
        5, 5, 5, 1,
        /* Masks and shifts */
        0x7C00, 0x03E0, 0x001F, 0x8000, 
        10, 5, 0, 15,
        },
	//-----------------------------------------------------------------------10
        {"PF_R8G8B8", 
        /* Bytes per element */ 
        3,  // 24 bit integer -- special
        /* Flags */
        PFF_NATIVEENDIAN,  
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 0,
        /* Masks and shifts */
        0xFF0000, 0x00FF00, 0x0000FF, 0, 
        16, 8, 0, 0 
        },
	//-----------------------------------------------------------------------11
        {"PF_B8G8R8", 
        /* Bytes per element */ 
        3,  // 24 bit integer -- special
        /* Flags */
        PFF_NATIVEENDIAN,  
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 0,
        /* Masks and shifts */
        0x0000FF, 0x00FF00, 0xFF0000, 0, 
        0, 8, 16, 0 
        },
	//-----------------------------------------------------------------------12
        {"PF_A8R8G8B8", 
        /* Bytes per element */ 
        4,  
        /* Flags */
        PFF_HASALPHA | PFF_NATIVEENDIAN,  
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 8,
        /* Masks and shifts */
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000,
        16, 8, 0, 24
        },
	//-----------------------------------------------------------------------13
        {"PF_A8B8G8R8", 
        /* Bytes per element */ 
        4,  
        /* Flags */
        PFF_HASALPHA | PFF_NATIVEENDIAN,  
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 8,
        /* Masks and shifts */
        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
        0, 8, 16, 24,
        },
	//-----------------------------------------------------------------------14
        {"PF_B8G8R8A8", 
        /* Bytes per element */ 
        4,  
        /* Flags */
        PFF_HASALPHA | PFF_NATIVEENDIAN,  
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 8,
        /* Masks and shifts */
        0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF,
        8, 16, 24, 0
        },
	//-----------------------------------------------------------------------15
        {"PF_A2R10G10B10", 
        /* Bytes per element */ 
        4,  
        /* Flags */
        PFF_HASALPHA | PFF_NATIVEENDIAN,  
        /* rbits, gbits, bbits, abits */
        10, 10, 10, 2,
        /* Masks and shifts */
        0x3FF00000, 0x000FFC00, 0x000003FF, 0xC0000000,
        20, 10, 0, 30
        },
	//-----------------------------------------------------------------------16
        {"PF_A2B10G10R10", 
        /* Bytes per element */ 
        4,  
        /* Flags */
        PFF_HASALPHA | PFF_NATIVEENDIAN,  
        /* rbits, gbits, bbits, abits */
        10, 10, 10, 2,
        /* Masks and shifts */
        0x000003FF, 0x000FFC00, 0x3FF00000, 0xC0000000,
        0, 10, 20, 30
        },
	//-----------------------------------------------------------------------17
		{"PF_RGB_DXT1", 
		/* Bytes per element */ 
		0,  
		/* Flags */
		PFF_COMPRESSED,  
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0 
		},
	//-----------------------------------------------------------------------18
        {"PF_DXT1", 
        /* Bytes per element */ 
        0,  
        /* Flags */
        PFF_COMPRESSED | PFF_HASALPHA,  
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0 
        },
	//-----------------------------------------------------------------------19
        {"PF_DXT2", 
        /* Bytes per element */ 
        0,  
        /* Flags */
        PFF_COMPRESSED | PFF_HASALPHA,  
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0 
        },
	//-----------------------------------------------------------------------20
        {"PF_DXT3", 
        /* Bytes per element */ 
        0,  
        /* Flags */
        PFF_COMPRESSED | PFF_HASALPHA,  
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0 
        },
	//-----------------------------------------------------------------------21
        {"PF_DXT4", 
        /* Bytes per element */ 
        0,  
        /* Flags */
        PFF_COMPRESSED | PFF_HASALPHA,  
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0 
        },
	//-----------------------------------------------------------------------22
        {"PF_DXT5", 
        /* Bytes per element */ 
        0,  
        /* Flags */
        PFF_COMPRESSED | PFF_HASALPHA,  
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0 
        },
	//-----------------------------------------------------------------------23
        {"PF_FLOAT16_RGB", 
        /* Bytes per element */ 
        6,  
        /* Flags */
        PFF_FLOAT,  
        /* rbits, gbits, bbits, abits */
        16, 16, 16, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0 
        },
	//-----------------------------------------------------------------------24
        {"PF_FLOAT16_RGBA", 
        /* Bytes per element */ 
        8,  
        /* Flags */
        PFF_FLOAT,  
        /* rbits, gbits, bbits, abits */
        16, 16, 16, 16,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0 
        },
	//-----------------------------------------------------------------------25
        {"PF_FLOAT32_RGB", 
        /* Bytes per element */ 
        12,  
        /* Flags */
        PFF_FLOAT,  
        /* rbits, gbits, bbits, abits */
        32, 32, 32, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0 
        },
	//-----------------------------------------------------------------------26
        {"PF_FLOAT32_RGBA", 
        /* Bytes per element */ 
        16,  
        /* Flags */
        PFF_FLOAT,  
        /* rbits, gbits, bbits, abits */
        32, 32, 32, 32,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0 
        },
	//-----------------------------------------------------------------------27
        {"PF_X8R8G8B8", 
        /* Bytes per element */ 
        4,  
        /* Flags */
        PFF_NATIVEENDIAN,  
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 0,
        /* Masks and shifts */
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000,
        16, 8, 0, 24
        },
	//-----------------------------------------------------------------------28
        {"PF_X8B8G8R8", 
        /* Bytes per element */ 
        4,  
        /* Flags */
        PFF_NATIVEENDIAN,  
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 0,
        /* Masks and shifts */
        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
        0, 8, 16, 24
        },
	//-----------------------------------------------------------------------29
        {"PF_R8G8B8A8", 
        /* Bytes per element */ 
        4,  
        /* Flags */
        PFF_HASALPHA | PFF_NATIVEENDIAN,  
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 8,
        /* Masks and shifts */
        0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF,
        24, 16, 8, 0
        },
	//-----------------------------------------------------------------------30
		{"PF_DEPTH", 
        /* Bytes per element */ 
        4,  
        /* Flags */
        PFF_DEPTH,  
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------31
		{"PF_SHORT_RGBA",
		/* Bytes per element */ 
        8,  
        /* Flags */
        PFF_HASALPHA,  
        /* rbits, gbits, bbits, abits */
        16, 16, 16, 16,
        /* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------32
        {"PF_R3G3B2", 
        /* Bytes per element */ 
        1,  
        /* Flags */
        PFF_NATIVEENDIAN,  
        /* rbits, gbits, bbits, abits */
        3, 3, 2, 0,
        /* Masks and shifts */
        0xE0, 0x1C, 0x03, 0, 
        5, 2, 0, 0 
        },
	//-----------------------------------------------------------------------33
        {"PF_FLOAT16_R", 
        /* Bytes per element */ 
        2,  
        /* Flags */
        PFF_FLOAT,  
        /* rbits, gbits, bbits, abits */
        16, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0 
        },
	//-----------------------------------------------------------------------34
        {"PF_FLOAT32_R", 
        /* Bytes per element */ 
        4,  
        /* Flags */
        PFF_FLOAT,  
        /* rbits, gbits, bbits, abits */
        32, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0 
        },
    };
    //-----------------------------------------------------------------------
	uint PixelBox::getConsecutiveSize() const 
	{ 
		return PixelUtil::getMemorySize(getWidth(), getHeight(), getDepth(), format); 
	}
	PixelBox PixelBox::getSubVolume(const Box &def) const
	{
		if(PixelUtil::isCompressed(format))
		{
			if(def.left == left && def.top == top && def.front == front &&
			   def.right == right && def.bottom == bottom && def.back == back)
			{
				// Entire buffer is being queried
				return *this;
			}
			throw("Cannot return subvolume of compressed PixelBuffer");
		}
		if(!contains(def))
		{
			throw("Bounds out of range");
		}

		const uint elemSize = PixelUtil::getNumElemBytes(format);
		// Calculate new data origin
		PixelBox rval(def, format, ((uchar*)data) 
			+ ((def.left-left)*elemSize)
			+ ((def.top-top)*rowPitch*elemSize)
			+ ((def.front-front)*slicePitch*elemSize)
		);		

		rval.rowPitch = rowPitch;
		rval.slicePitch = slicePitch;
		rval.format = format;

		return rval;
	}
    //-----------------------------------------------------------------------
    /**
    * Directly get the description record for provided pixel format. For debug builds,
    * this checks the bounds of fmt with an assertion.
    */    
    static inline const PixelFormatDescription &getDescriptionFor(const PixelFormat fmt)
    {
        const int ord = (int)fmt;
        ASSERT(ord>=0 && ord<PF_COUNT);

        return _pixelFormats[ord];
    }
    //-----------------------------------------------------------------------
    uint PixelUtil::getNumElemBytes( PixelFormat format )
    {
        return getDescriptionFor(format).elemBytes;
    }
	//-----------------------------------------------------------------------
	uint PixelUtil::getMemorySize(uint width, uint height, uint depth, PixelFormat format)
	{
		if(isCompressed(format)) 
		{
			switch(format) 
			{
				// DXT formats work by dividing the image into 4x4 blocks, then encoding each
				// 4x4 block with a certain number of bytes. DXT can only be used on 2D images.
				case PF_RGB_DXT1:
				case PF_DXT1:
					ASSERT(depth == 1);
					return ((width+3)/4)*((height+3)/4)*8;
				case PF_DXT2:
				case PF_DXT3:
				case PF_DXT4:
				case PF_DXT5:
					ASSERT(depth == 1);
					return ((width+3)/4)*((height+3)/4)*16;
				default:
				throw("Invalid compressed pixel format");
			}
		} 
		else 
		{
			return width*height*depth*getNumElemBytes(format); 
		}
	}
    //-----------------------------------------------------------------------
    uint PixelUtil::getNumElemBits( PixelFormat format )
    {
        return getDescriptionFor(format).elemBytes * 8;
    }
    //-----------------------------------------------------------------------
    uint PixelUtil::getFlags( PixelFormat format )
    {
        return getDescriptionFor(format).flags;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::hasAlpha(PixelFormat format)
    {
        return (PixelUtil::getFlags(format) & PFF_HASALPHA) > 0;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::isFloatingPoint(PixelFormat format)
    {
        return (PixelUtil::getFlags(format) & PFF_FLOAT) > 0;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::isCompressed(PixelFormat format)
    {
        return (PixelUtil::getFlags(format) & PFF_COMPRESSED) > 0;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::isDepth(PixelFormat format)
    {
        return (PixelUtil::getFlags(format) & PFF_DEPTH) > 0;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::isNativeEndian(PixelFormat format)
    {
        return (PixelUtil::getFlags(format) & PFF_NATIVEENDIAN) > 0;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::isLuminance(PixelFormat format)
    {
        return (PixelUtil::getFlags(format) & PFF_LUMINANCE) > 0;
    }
    //-----------------------------------------------------------------------
	bool PixelUtil::isValidExtent(uint width, uint height, uint depth, PixelFormat format)
	{
		if(isCompressed(format)) 
		{
			switch(format) 
			{
				case PF_RGB_DXT1:
				case PF_DXT1:
				case PF_DXT2:
				case PF_DXT3:
				case PF_DXT4:
				case PF_DXT5:
					return ((width&3)==0 && (height&3)==0 && depth==1);
				default:
					return true;
			}
		} 
		else 
		{
			return true; 
		}
	}
	//-----------------------------------------------------------------------
    void  PixelUtil::getBitDepths(PixelFormat format, int rgba[4])
    {
        const PixelFormatDescription &des = getDescriptionFor(format);
        rgba[0] = des.rbits;
        rgba[1] = des.gbits;
        rgba[2] = des.bbits;
        rgba[3] = des.abits;
    }
	//-----------------------------------------------------------------------
	void  PixelUtil::getBitMasks(PixelFormat format, uint rgba[4])
    {
        const PixelFormatDescription &des = getDescriptionFor(format);
        rgba[0] = des.rmask;
        rgba[1] = des.gmask;
        rgba[2] = des.bmask;
        rgba[3] = des.amask;
    }
    //-----------------------------------------------------------------------
    std::string PixelUtil::getFormatName(PixelFormat srcformat)
    {
        return getDescriptionFor(srcformat).name;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::isAccessible(PixelFormat srcformat) {
        uint flags = getFlags(srcformat);
        return !((flags & PFF_COMPRESSED) || (flags & PFF_DEPTH));
    }
    //-----------------------------------------------------------------------
    /*************************************************************************
    * Pixel packing/unpacking utilities
    */
    void  PixelUtil::packColor(const ColorValue &colour, const PixelFormat pf,  const void * dest)
    {
        packColor(colour.r, colour.g, colour.b, colour.a, pf, dest);
    }
    //-----------------------------------------------------------------------
    void  PixelUtil::packColor(const uchar r, const uchar g, const uchar b, const uchar a, const PixelFormat pf,  const void * dest)
    {
        const PixelFormatDescription &des = getDescriptionFor(pf);
        if(des.flags & PFF_NATIVEENDIAN) {
            // Shortcut for integer formats packing
            uint value = ((Bitwise::fixedToFixed(r, 8, des.rbits)<<des.rshift) & des.rmask) |
                ((Bitwise::fixedToFixed(g, 8, des.gbits)<<des.gshift) & des.gmask) |
                ((Bitwise::fixedToFixed(b, 8, des.bbits)<<des.bshift) & des.bmask) |
                ((Bitwise::fixedToFixed(a, 8, des.abits)<<des.ashift) & des.amask);
            // And write to memory
            Bitwise::intWrite(dest, des.elemBytes, value);
        } else {
            // Convert to float
            packColor((float)r/255.0f,(float)g/255.0f,(float)b/255.0f,(float)a/255.0f, pf, dest);
        }
    }
    //-----------------------------------------------------------------------
    void  PixelUtil::packColor(const float r, const float g, const float b, const float a, const PixelFormat pf,  const void * dest)
    {
        // Catch-it-all here
        const PixelFormatDescription &des = getDescriptionFor(pf);
        if(des.flags & PFF_NATIVEENDIAN) {
            // Do the packing
            //std::cerr << dest << " " << r << " " << g <<  " " << b << " " << a << std::endl;
            const uint value = ((Bitwise::floatToFixed(r, des.rbits)<<des.rshift) & des.rmask) |
                ((Bitwise::floatToFixed(g, des.gbits)<<des.gshift) & des.gmask) |
                ((Bitwise::floatToFixed(b, des.bbits)<<des.bshift) & des.bmask) |
                ((Bitwise::floatToFixed(a, des.abits)<<des.ashift) & des.amask);
            // And write to memory
            Bitwise::intWrite(dest, des.elemBytes, value);                                
        } else {
            switch(pf)
            {
            case PF_FLOAT32_R:
                ((float*)dest)[0] = r;
                break;
            case PF_FLOAT32_RGB:
                ((float*)dest)[0] = r;
                ((float*)dest)[1] = g;
                ((float*)dest)[2] = b;
                break;
            case PF_FLOAT32_RGBA:
                ((float*)dest)[0] = r;
                ((float*)dest)[1] = g;
                ((float*)dest)[2] = b;
                ((float*)dest)[3] = a;
                break;
            case PF_FLOAT16_R:
                ((ushort*)dest)[0] = Bitwise::floatToHalf(r);
                break;
            case PF_FLOAT16_RGB:
                ((ushort*)dest)[0] = Bitwise::floatToHalf(r);
                ((ushort*)dest)[1] = Bitwise::floatToHalf(g);
                ((ushort*)dest)[2] = Bitwise::floatToHalf(b);
                break;
            case PF_FLOAT16_RGBA:
                ((ushort*)dest)[0] = Bitwise::floatToHalf(r);
                ((ushort*)dest)[1] = Bitwise::floatToHalf(g);
                ((ushort*)dest)[2] = Bitwise::floatToHalf(b);
                ((ushort*)dest)[3] = Bitwise::floatToHalf(a);
                break;
			case PF_SHORT_RGBA:
				((ushort*)dest)[0] = Bitwise::floatToFixed(r, 16);
                ((ushort*)dest)[1] = Bitwise::floatToFixed(g, 16);
                ((ushort*)dest)[2] = Bitwise::floatToFixed(b, 16);
                ((ushort*)dest)[3] = Bitwise::floatToFixed(a, 16);
				break;
			case PF_BYTE_LA:
				((uchar*)dest)[0] = Bitwise::floatToFixed(r, 8);
                ((uchar*)dest)[1] = Bitwise::floatToFixed(a, 8);
				break;
            default:
                // Not yet supported
                throw("pack to " + getFormatName(pf) + " not implemented");
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
    void  PixelUtil::unpackColor(ColorValue *colour, PixelFormat pf,  const void * src)
    {
        unpackColor(&colour->r, &colour->g, &colour->b, &colour->a, pf, src);
    }
    //-----------------------------------------------------------------------
    void  PixelUtil::unpackColor(uchar *r, uchar *g, uchar *b, uchar *a, PixelFormat pf,  const void * src)
    {
        const PixelFormatDescription &des = getDescriptionFor(pf);
        if(des.flags & PFF_NATIVEENDIAN) {
            // Shortcut for integer formats unpacking
            const uint value = Bitwise::intRead(src, des.elemBytes);
            if(des.flags & PFF_LUMINANCE) 
            {
                // Luminance format -- only rbits used
                *r = *g = *b = Bitwise::fixedToFixed(
                    (value & des.rmask)>>des.rshift, des.rbits, 8);
            } 
            else 
            {
                *r = Bitwise::fixedToFixed((value & des.rmask)>>des.rshift, des.rbits, 8);
                *g = Bitwise::fixedToFixed((value & des.gmask)>>des.gshift, des.gbits, 8);
                *b = Bitwise::fixedToFixed((value & des.bmask)>>des.bshift, des.bbits, 8);
            }
            if(des.flags & PFF_HASALPHA)
            {
                *a = Bitwise::fixedToFixed((value & des.amask)>>des.ashift, des.abits, 8);
            }
            else
            {
                *a = 255; // No alpha, default a component to full
            }
        } else {
            // Do the operation with the more generic Floating point
            float rr, gg, bb, aa;
            unpackColor(&rr,&gg,&bb,&aa, pf, src);
            *r = Bitwise::floatToFixed(rr, 8);
            *g = Bitwise::floatToFixed(gg, 8);            
            *b = Bitwise::floatToFixed(bb, 8); 
            *a = Bitwise::floatToFixed(aa, 8);                    
        }
    }
    //-----------------------------------------------------------------------
    void  PixelUtil::unpackColor(float *r, float *g, float *b, float *a, 
        PixelFormat pf,  const void * src) 
    {
        const PixelFormatDescription &des = getDescriptionFor(pf);
        if(des.flags & PFF_NATIVEENDIAN) {
            // Shortcut for integer formats unpacking
            const uint value = Bitwise::intRead(src, des.elemBytes);
            if(des.flags & PFF_LUMINANCE) 
            {
                // Luminance format -- only rbits used
                *r = *g = *b = Bitwise::fixedToFloat(
                    (value & des.rmask)>>des.rshift, des.rbits);
            } 
            else
            { 
                *r = Bitwise::fixedToFloat((value & des.rmask)>>des.rshift, des.rbits);
                *g = Bitwise::fixedToFloat((value & des.gmask)>>des.gshift, des.gbits);
                *b = Bitwise::fixedToFloat((value & des.bmask)>>des.bshift, des.bbits);
            }
            if(des.flags & PFF_HASALPHA)
            {
                *a = Bitwise::fixedToFloat((value & des.amask)>>des.ashift, des.abits);
            }
            else
            {
                *a = 1.0f; // No alpha, default a component to full
            }
        } else {
            switch(pf)
            {
            case PF_FLOAT32_R:
                *r = *g = *b = ((float*)src)[0];
                *a = 1.0f;
                break;
            case PF_FLOAT32_RGB:
                *r = ((float*)src)[0];
                *g = ((float*)src)[1];
                *b = ((float*)src)[2];
                *a = 1.0f;
                break;
            case PF_FLOAT32_RGBA:
                *r = ((float*)src)[0];
                *g = ((float*)src)[1];
                *b = ((float*)src)[2];
                *a = ((float*)src)[3];
                break;
            case PF_FLOAT16_R:
                *r = *g = *b = Bitwise::halfToFloat(((ushort*)src)[0]);
                *a = 1.0f;
                break;
            case PF_FLOAT16_RGB:
                *r = Bitwise::halfToFloat(((ushort*)src)[0]);
                *g = Bitwise::halfToFloat(((ushort*)src)[1]);
                *b = Bitwise::halfToFloat(((ushort*)src)[2]);
                *a = 1.0f;
                break;
            case PF_FLOAT16_RGBA:
                *r = Bitwise::halfToFloat(((ushort*)src)[0]);
                *g = Bitwise::halfToFloat(((ushort*)src)[1]);
                *b = Bitwise::halfToFloat(((ushort*)src)[2]);
                *a = Bitwise::halfToFloat(((ushort*)src)[3]);
                break;
			case PF_SHORT_RGBA:
				*r = Bitwise::fixedToFloat(((ushort*)src)[0], 16);
                *g = Bitwise::fixedToFloat(((ushort*)src)[1], 16);
				*b = Bitwise::fixedToFloat(((ushort*)src)[2], 16);
				*a = Bitwise::fixedToFloat(((ushort*)src)[3], 16);
				break;
			case PF_BYTE_LA:
				*r = *g = *b = Bitwise::fixedToFloat(((uchar*)src)[0], 8);
				*a = Bitwise::fixedToFloat(((uchar*)src)[1], 8);
				break;
            default:
                // Not yet supported
                throw("unpack from " + getFormatName(pf) + " not implemented");
                break;
            }
        }    
    }
    //-----------------------------------------------------------------------
    /* Convert pixels from one format to another */
    void  PixelUtil::bulkPixelConversion(void  *srcp, PixelFormat srcFormat, 
        void  *destp, PixelFormat dstFormat, uint count)
    {
        PixelBox src(count, 1, 1, srcFormat, srcp), 
				 dst(count, 1, 1, dstFormat, destp);

        bulkPixelConversion(src, dst);
    }
    //-----------------------------------------------------------------------
    void  PixelUtil::bulkPixelConversion(const PixelBox &src, const PixelBox &dst)
    {
        ASSERT(src.getWidth() == dst.getWidth() && 
			   src.getHeight() == dst.getHeight() && 
			   src.getDepth() == dst.getDepth());

		// Check for compressed formats, we don't support decompression, compression or recoding
		if(PixelUtil::isCompressed(src.format) || PixelUtil::isCompressed(dst.format))
		{
			if(src.format == dst.format)
			{
				memcpy(dst.data, src.data, src.getConsecutiveSize());
				return;
			}
			else
			{
				throw("This method can not be used to compress or decompress images");
			}
		}

        // The easy case
        if(src.format == dst.format) {
            // Everything consecutive?
            if(src.isConsecutive() && dst.isConsecutive()) 
            {
				memcpy(dst.data, src.data, src.getConsecutiveSize());
                return;
            }

            uchar *srcptr = static_cast<uchar*>(src.data);
            uchar *dstptr = static_cast<uchar*>(dst.data);
            const uint srcPixelSize = PixelUtil::getNumElemBytes(src.format);
            const uint dstPixelSize = PixelUtil::getNumElemBytes(dst.format);

            // Calculate pitches+skips in bytes
            const uint srcRowPitchBytes = src.rowPitch*srcPixelSize;
            //const uint srcRowSkipBytes = src.getRowSkip()*srcPixelSize;
            const uint srcSliceSkipBytes = src.getSliceSkip()*srcPixelSize;

            const uint dstRowPitchBytes = dst.rowPitch*dstPixelSize;
            //const uint dstRowSkipBytes = dst.getRowSkip()*dstPixelSize;
            const uint dstSliceSkipBytes = dst.getSliceSkip()*dstPixelSize;

            // Otherwise, copy per row
            const uint rowSize = src.getWidth()*srcPixelSize;
            for(uint z=src.front; z<src.back; z++) 
            {
                for(uint y=src.top; y<src.bottom; y++)
                {
					memcpy(dstptr, srcptr, rowSize);
                    srcptr += srcRowPitchBytes;
                    dstptr += dstRowPitchBytes;
                }
                srcptr += srcSliceSkipBytes;
                dstptr += dstSliceSkipBytes;
            }
            return;
        }
		// Converting to PF_X8R8G8B8 is exactly the same as converting to
		// PF_A8R8G8B8. (same with PF_X8B8G8R8 and PF_A8B8G8R8)
		if(dst.format == PF_X8R8G8B8 || dst.format == PF_X8B8G8R8)
		{
			// Do the same conversion, with PF_A8R8G8B8, which has a lot of 
			// optimized conversions
			PixelBox tempdst = dst;
			tempdst.format = dst.format==PF_X8R8G8B8?PF_A8R8G8B8:PF_A8B8G8R8;
			bulkPixelConversion(src, tempdst);
			return;
		}
		// Converting from PF_X8R8G8B8 is exactly the same as converting from
		// PF_A8R8G8B8, given that the destination format does not have alpha.
		if((src.format == PF_X8R8G8B8||src.format == PF_X8B8G8R8) && !hasAlpha(dst.format))
		{
			// Do the same conversion, with PF_A8R8G8B8, which has a lot of 
			// optimized conversions
			PixelBox tempsrc = src;
			tempsrc.format = src.format==PF_X8R8G8B8?PF_A8R8G8B8:PF_A8B8G8R8;
			bulkPixelConversion(tempsrc, dst);
			return;
		}

// NB VC6 can't handle the templates required for optimised conversion, tough
#if RE_COMPILER != RE_COMPILER_MSVC || RE_COMP_VER >= 1300
        // Is there a specialized, inlined, conversion?
        if(doOptimizedConversion(src, dst))
        {
            // If so, good
            return;
        }
#endif 

        uchar *srcptr = static_cast<uchar*>(src.data);
        uchar *dstptr = static_cast<uchar*>(dst.data);
        const uint srcPixelSize = PixelUtil::getNumElemBytes(src.format);
        const uint dstPixelSize = PixelUtil::getNumElemBytes(dst.format);

        // Calculate pitches+skips in bytes
        const uint srcRowSkipBytes = src.getRowSkip()*srcPixelSize;
        const uint srcSliceSkipBytes = src.getSliceSkip()*srcPixelSize;
        const uint dstRowSkipBytes = dst.getRowSkip()*dstPixelSize;
        const uint dstSliceSkipBytes = dst.getSliceSkip()*dstPixelSize;

        // The brute force fallback
        float r,g,b,a;
        for(uint z=src.front; z<src.back; z++) 
        {
            for(uint y=src.top; y<src.bottom; y++)
            {
                for(uint x=src.left; x<src.right; x++)
                {
                    unpackColor(&r, &g, &b, &a, src.format, srcptr);
                    packColor(r, g, b, a, dst.format, dstptr);
                    srcptr += srcPixelSize; 
                    dstptr += dstPixelSize;
                }
                srcptr += srcRowSkipBytes;
                dstptr += dstRowSkipBytes;
            }
            srcptr += srcSliceSkipBytes;
            dstptr += dstSliceSkipBytes;
        }
    }

}
