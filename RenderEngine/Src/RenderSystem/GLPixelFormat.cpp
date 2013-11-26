#include "StdAfx.h"
#include "GLPixelFormat.h"

namespace xs 
{
	//-----------------------------------------------------------------------------
    GLenum GLPixelUtil::getGLOriginFormat(PixelFormat mFormat)
    {
        switch(mFormat)
        {
			case PF_A8:
				return GL_ALPHA;
            case PF_L8:
                return GL_LUMINANCE;
            case PF_L16:
                return GL_LUMINANCE;
			case PF_BYTE_LA:
				return GL_LUMINANCE_ALPHA;
			case PF_R3G3B2:
				return GL_RGB;
			case PF_A1R5G5B5:
				return GL_BGRA;
			case PF_R5G6B5:
				return GL_RGB;
			case PF_B5G6R5:
				return GL_BGR;
			case PF_A4R4G4B4:
				return GL_BGRA;
#if RE_ENDIAN == RE_ENDIAN_BIG
            // Formats are in native endian, so R8G8B8 on little endian is
            // BGR, on big endian it is RGB.
            case PF_R8G8B8:
                return GL_RGB;
            case PF_B8G8R8:
                return GL_BGR;
			case PF_R8G8B8A8:
				return GL_RGBA;
			case PF_B8G8R8A8:
				return GL_BGRA;
#else
            case PF_R8G8B8:
                return GL_BGR;
            case PF_B8G8R8:
                return GL_RGB;
			//case PF_R8G8B8A8: //此时没有对应的
			//case PF_B8G8R8A8://没有对于那个的
#endif
			case PF_X8R8G8B8:
			case PF_A8R8G8B8:
				return GL_BGRA;
			case PF_X8B8G8R8:
            case PF_A8B8G8R8:
                return GL_RGBA;
            case PF_A2R10G10B10:
                return GL_BGRA;
            case PF_A2B10G10R10:
                return GL_RGBA;
			case PF_FLOAT16_R:
                return GL_LUMINANCE;
            case PF_FLOAT16_RGB:
                return GL_RGB;
            case PF_FLOAT16_RGBA:
                return GL_RGBA;
			case PF_FLOAT32_R:
                return GL_LUMINANCE;
            case PF_FLOAT32_RGB:
                return GL_RGB;
            case PF_FLOAT32_RGBA:
                return GL_RGBA;
			case PF_SHORT_RGBA:
				return GL_RGBA;
			case PF_RGB_DXT1:
				return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
            case PF_DXT1:
                return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            case PF_DXT3:
                 return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            case PF_DXT5:
                 return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            default:
                return 0;
        }
    }
	//----------------------------------------------------------------------------- 
    GLenum GLPixelUtil::getGLOriginDataType(PixelFormat mFormat)
    {
        switch(mFormat)
        {
			case PF_A8:
            case PF_L8:
            case PF_R8G8B8:
            case PF_B8G8R8:
			case PF_BYTE_LA:
                return GL_UNSIGNED_BYTE;
			case PF_R3G3B2:
				return GL_UNSIGNED_BYTE_3_3_2;
			case PF_A1R5G5B5:
				return GL_UNSIGNED_SHORT_1_5_5_5_REV;
			case PF_R5G6B5:
			case PF_B5G6R5:
				return GL_UNSIGNED_SHORT_5_6_5;
			case PF_A4R4G4B4:
				return GL_UNSIGNED_SHORT_4_4_4_4_REV;
            case PF_L16:
                return GL_UNSIGNED_SHORT;
#if RE_ENDIAN == RE_ENDIAN_BIG
			case PF_X8B8G8R8:
			case PF_A8B8G8R8:
                return GL_UNSIGNED_INT_8_8_8_8_REV;
			case PF_X8R8G8B8:
            case PF_A8R8G8B8:
				return GL_UNSIGNED_INT_8_8_8_8_REV;
            case PF_B8G8R8A8:
                return GL_UNSIGNED_BYTE;
			case PF_R8G8B8A8:
				return GL_UNSIGNED_BYTE;
#else
			case PF_X8B8G8R8:
			case PF_A8B8G8R8:
                return GL_UNSIGNED_BYTE;
			case PF_X8R8G8B8:
            case PF_A8R8G8B8:
				return GL_UNSIGNED_BYTE;
            case PF_B8G8R8A8:
                return GL_UNSIGNED_INT_8_8_8_8;
			case PF_R8G8B8A8:
				return GL_UNSIGNED_INT_8_8_8_8;
#endif
            case PF_A2R10G10B10:
                return GL_UNSIGNED_INT_2_10_10_10_REV;
            case PF_A2B10G10R10:
                return GL_UNSIGNED_INT_2_10_10_10_REV;
			case PF_FLOAT16_R:
            case PF_FLOAT16_RGB:
            case PF_FLOAT16_RGBA:
                return 0; // GL_HALF_FLOAT -- nyi
			case PF_FLOAT32_R:
            case PF_FLOAT32_RGB:
            case PF_FLOAT32_RGBA:
                return GL_FLOAT;
			case PF_SHORT_RGBA:
				return GL_UNSIGNED_SHORT;
            default:
                return 0;
        }
    }

    GLenum GLPixelUtil::getClosestGLInternalFormat(PixelFormat mFormat)
    {
        switch(mFormat) {
            case PF_L8:
                return GL_LUMINANCE8;
            case PF_L16:
                return GL_LUMINANCE16;
            case PF_A8:
                return GL_ALPHA8;
            case PF_A4L4:
                return GL_LUMINANCE4_ALPHA4;
			case PF_BYTE_LA:
				return GL_LUMINANCE8_ALPHA8;
			case PF_R3G3B2:
				return GL_R3_G3_B2;
			case PF_A1R5G5B5:
				return GL_RGB5_A1;
            case PF_R5G6B5:
			case PF_B5G6R5:
                return GL_RGB5;
            case PF_A4R4G4B4:
                return GL_RGBA4;
            case PF_R8G8B8:
            case PF_B8G8R8:
			case PF_X8B8G8R8:
			case PF_X8R8G8B8:
                return GL_RGB8;
            case PF_A8R8G8B8:
            case PF_B8G8R8A8:
                return GL_RGBA8;
            case PF_A2R10G10B10:
            case PF_A2B10G10R10:
                return GL_RGB10_A2;
			case PF_FLOAT16_R:
				return GL_LUMINANCE_FLOAT16_ATI;
            case PF_FLOAT16_RGB:
                return GL_RGB_FLOAT16_ATI;
                //    return GL_RGB16F;
            case PF_FLOAT16_RGBA:
                return GL_RGBA_FLOAT16_ATI;
                //    return GL_RGBA16F;
			case PF_FLOAT32_R:
				return GL_LUMINANCE_FLOAT32_ATI;
            case PF_FLOAT32_RGB:
                return GL_RGB_FLOAT32_ATI;
                //    return GL_RGB32F;
            case PF_FLOAT32_RGBA:
                return GL_RGBA_FLOAT32_ATI;
                //    return GL_RGBA32F;
			case PF_SHORT_RGBA:
				return GL_RGBA16;
			case PF_RGB_DXT1:
				return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
            case PF_DXT1:
                return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            case PF_DXT3:
                return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            case PF_DXT5:
                return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            default:
                return GL_RGBA8;
        }
    }
	
	//----------------------------------------------------------------------------- 	
	PixelFormat GLPixelUtil::getClosestREFormat(GLenum fmt)
	{
		switch(fmt) 
		{
		case GL_LUMINANCE8:
			return PF_L8;
		case GL_LUMINANCE16:
			return PF_L16;
		case GL_ALPHA8:
			return PF_A8;
		case GL_LUMINANCE4_ALPHA4:
			// Unsupported by GL as input format, use the uchar packed format
			return PF_BYTE_LA;
		case GL_LUMINANCE8_ALPHA8:
			return PF_BYTE_LA;
		case GL_R3_G3_B2:
			return PF_R3G3B2;
		case GL_RGB5_A1:
			return PF_A1R5G5B5;
		case GL_RGB5:
			return PF_R5G6B5;
		case GL_RGBA4:
			return PF_A4R4G4B4;
		case GL_RGB8:
			return PF_X8R8G8B8;
		case GL_RGBA8:
			return PF_A8R8G8B8;
		case GL_RGB10_A2:
			return PF_A2R10G10B10;
		case GL_RGBA16:
			return PF_SHORT_RGBA;
		case GL_LUMINANCE_FLOAT16_ATI:
			return PF_FLOAT16_R;
		case GL_LUMINANCE_FLOAT32_ATI:
			return PF_FLOAT32_R;
		case GL_RGB_FLOAT16_ATI: // GL_RGB16F
		case GL_RGBA_FLOAT16_ATI:
		case GL_RGB_FLOAT32_ATI:
		case GL_RGBA_FLOAT32_ATI:
			return PF_FLOAT32_RGBA;
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
			return PF_RGB_DXT1;
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			return PF_DXT1;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
			return PF_DXT3;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			return PF_DXT5;
		default:
			return PF_A8R8G8B8;
		};
	}
	//----------------------------------------------------------------------------- 

	size_t GLPixelUtil::getMaxMipmaps(size_t width, size_t height, size_t depth, PixelFormat format)
	{
		size_t count = 0;
		do {
			if(width>1)		width = width/2;
			if(height>1)	height = height/2;
			if(depth>1)		depth = depth/2;
			/*
			NOT needed, compressed formats will have mipmaps up to 1x1
			if(PixelUtil::isValidExtent(width, height, depth, format))
				count ++;
			else
				break;
			*/
				
			count ++;
		} while(!(width == 1 && height == 1 && depth == 1));
		
		return count;
	}
	
};
