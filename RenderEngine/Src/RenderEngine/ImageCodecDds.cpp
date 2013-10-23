#include "StdAfx.h"
#include "PixelFormat.h"
#include "ImageCodecDds.h"

namespace xs
{
	/**
	the following structs is copied from DDRAW.H,and changed to avoid OS independencies.
	*/

	#define DDSD_HEIGHT             0x00000002l
	#define DDSD_WIDTH              0x00000004l
	#define DDSD_MIPMAPCOUNT        0x00020000l
	#define DDPF_FOURCC             0x00000004l

	#define DUMMYUNIONNAMEN(n)
	typedef struct _DDPIXELFORMAT
	{
		uint       dwSize;                 // size of structure
		uint       dwFlags;                // pixel format flags
		uint       dwFourCC;               // (FOURCC code)
		union
		{
			uint   dwRGBBitCount;          // how many bits per pixel
			uint   dwYUVBitCount;          // how many bits per pixel
			uint   dwZBufferBitDepth;      // how many total bits/pixel in z buffer (including any stencil bits)
			uint   dwAlphaBitDepth;        // how many bits for alpha channels
			uint   dwLuminanceBitCount;    // how many bits per pixel
			uint   dwBumpBitCount;         // how many bits per "buxel", total
			uint   dwPrivateFormatBitCount;// Bits per pixel of private driver formats. Only valid in texture
			// format list and if DDPF_D3DFORMAT is set
		} DUMMYUNIONNAMEN(1);
		union
		{
			uint   dwRBitMask;             // mask for red bit
			uint   dwYBitMask;             // mask for Y bits
			uint   dwStencilBitDepth;      // how many stencil bits (note: dwZBufferBitDepth-dwStencilBitDepth is total Z-only bits)
			uint   dwLuminanceBitMask;     // mask for luminance bits
			uint   dwBumpDuBitMask;        // mask for bump map U delta bits
			uint   dwOperations;           // DDPF_D3DFORMAT Operations
		} DUMMYUNIONNAMEN(2);
		union
		{
			uint   dwGBitMask;             // mask for green bits
			uint   dwUBitMask;             // mask for U bits
			uint   dwZBitMask;             // mask for Z bits
			uint   dwBumpDvBitMask;        // mask for bump map V delta bits
			struct
			{
				ushort    wFlipMSTypes;       // Multisample methods supported via flip for this D3DFORMAT
				ushort    wBltMSTypes;        // Multisample methods supported via blt for this D3DFORMAT
			} MultiSampleCaps;

		} DUMMYUNIONNAMEN(3);
		union
		{
			uint   dwBBitMask;             // mask for blue bits
			uint   dwVBitMask;             // mask for V bits
			uint   dwStencilBitMask;       // mask for stencil bits
			uint   dwBumpLuminanceBitMask; // mask for luminance in bump map
		} DUMMYUNIONNAMEN(4);
		union
		{
			uint   dwRGBAlphaBitMask;      // mask for alpha channel
			uint   dwYUVAlphaBitMask;      // mask for alpha channel
			uint   dwLuminanceAlphaBitMask;// mask for alpha channel
			uint   dwRGBZBitMask;          // mask for Z channel
			uint   dwYUVZBitMask;          // mask for Z channel
		} DUMMYUNIONNAMEN(5);
	} DDPIXELFORMAT;

	typedef struct _DDCOLORKEY
	{
		uint       dwColorSpaceLowValue;   // low boundary of color space that is to
		// be treated as Color Key, inclusive
		uint       dwColorSpaceHighValue;  // high boundary of color space that is
		// to be treated as Color Key, inclusive
	} DDCOLORKEY;

	typedef struct _DDSCAPS2
	{
		uint       dwCaps;         // capabilities of surface wanted
		uint       dwCaps2;
		uint       dwCaps3;
		union
		{
			uint       dwCaps4;
			uint       dwVolumeDepth;
		} DUMMYUNIONNAMEN(1);
	} DDSCAPS2;

	typedef struct _DDSURFACEDESC2
	{
		uint               dwSize;                 // size of the DDSURFACEDESC structure
		uint               dwFlags;                // determines what fields are valid
		uint               dwHeight;               // height of surface to be created
		uint               dwWidth;                // width of input surface
		union
		{
			long            lPitch;                 // distance to start of next line (return value only)
			uint           dwLinearSize;           // Formless late-allocated optimized surface size
		} DUMMYUNIONNAMEN(1);
		union
		{
			uint           dwBackBufferCount;      // number of back buffers requested
			uint           dwDepth;                // the depth if this is a volume texture 
		} DUMMYUNIONNAMEN(5);
		union
		{
			uint           dwMipMapCount;          // number of mip-map levels requestde
			// dwZBufferBitDepth removed, use ddpfPixelFormat one instead
			uint           dwRefreshRate;          // refresh rate (used when display mode is described)
			uint           dwSrcVBHandle;          // The source used in VB::Optimize
		} DUMMYUNIONNAMEN(2);
		uint               dwAlphaBitDepth;        // depth of alpha buffer requested
		uint               dwReserved;             // reserved
		void *              lpSurface;              // pointer to the associated surface memory
		union
		{
			DDCOLORKEY      ddckCKDestOverlay;      // color key for destination overlay use
			uint           dwEmptyFaceColor;       // Physical color for empty cubemap faces
		} DUMMYUNIONNAMEN(3);
		DDCOLORKEY          ddckCKDestBlt;          // color key for destination blt use
		DDCOLORKEY          ddckCKSrcOverlay;       // color key for source overlay use
		DDCOLORKEY          ddckCKSrcBlt;           // color key for source blt use
		union
		{
			DDPIXELFORMAT   ddpfPixelFormat;        // pixel format description of the surface
			uint           dwFVF;                  // vertex format description of vertex buffers
		} DUMMYUNIONNAMEN(4);
		DDSCAPS2            ddsCaps;                // direct draw surface capabilities
		uint               dwTextureStage;         // stage in multitexture cascade
	} DDSURFACEDESC2;

	const char*	ImageCodecDds::getType() const
	{
		static std::string strType = "dds";
		return strType.c_str();
	}

	bool ImageCodecDds::getDimension(xs::Stream* input,uint& width,uint& height,uint& depth)
	{
		uchar fourcc[4];
		DDSURFACEDESC2 ddsd2;
		input->read(fourcc,4);
		if(fourcc[0] != 'D' || fourcc[1] != 'D' || fourcc[2] != 'S')
		{
			Error("Invalid DDS format");
			return false;
		}
		input->read(&ddsd2,sizeof(ddsd2));
		if(!(ddsd2.dwFlags & DDSD_WIDTH) || !(ddsd2.dwFlags & DDSD_HEIGHT))
		{
			Error("Invalid DDS format,need DDSD_WIDTH & DDSD_HEIGHT");
			return false;
		}

		height = ddsd2.dwHeight;
		width = ddsd2.dwWidth;
		depth = 1;

		return true;
	}


	/*
		DDS:Image Width&Height must be multiple of 4!
	*/
	bool ImageCodecDds::decode(xs::Stream* input,ImageData& data,const void  *p1,const void  *p2) const
	{
		uchar fourcc[4];
		DDSURFACEDESC2 ddsd2;
		input->read(fourcc,4);
		if(fourcc[0] != 'D' || fourcc[1] != 'D' || fourcc[2] != 'S')
		{
			Error("Invalid DDS format");
			return false;
		}
		input->read(&ddsd2,sizeof(ddsd2));
		if(!(ddsd2.dwFlags & DDSD_WIDTH) || !(ddsd2.dwFlags & DDSD_HEIGHT))
		{
			Error("Invalid DDS format,need DDSD_WIDTH & DDSD_HEIGHT");
			return false;
		}
		if((ddsd2.dwWidth % 4 != 0) || (ddsd2.dwHeight % 4 != 0))
		{
			Error("Invalid dds format,width/height must be mutiple of 4\n");
			return false;
		}
		if(!(ddsd2.ddpfPixelFormat.dwFlags & DDPF_FOURCC) || 
				(ddsd2.ddpfPixelFormat.dwFourCC != (uint)'1TXD' 
				&& ddsd2.ddpfPixelFormat.dwFourCC != (uint)'3TXD'
				&& ddsd2.ddpfPixelFormat.dwFourCC != (uint)'5TXD')
			)
		{
			Error("DdsCodec Only Support DXT1,DXT3,DXT5 Now!");
			return false;
		}

		uint dwMipMapCount = 1;
		if(ddsd2.dwFlags & DDSD_MIPMAPCOUNT)
			dwMipMapCount = ddsd2.dwMipMapCount;

		data.height = ddsd2.dwHeight;
		data.width = ddsd2.dwWidth;
		data.depth = 1;
		data.size = 0;
		data.num_mipmaps = dwMipMapCount;// - 1;
		data.flags = IF_COMPRESSED;
		data.format = PF_UNKNOWN;
		data.pData = 0;

		if(ddsd2.ddpfPixelFormat.dwFourCC == (uint)'1TXD')
		{
			//if(ddsd2.ddpfPixelFormat.dwAlphaBitDepth == 0)
			//	data.format = PF_RGB_DXT1;
			//else
				data.format = PF_DXT1;
		}
		//else if(ddsd2.ddpfPixelFormat.dwFourCC == (uint)'2TXD')
		//{
		//	data.format = PF_DXT2;
		//}
		else if(ddsd2.ddpfPixelFormat.dwFourCC == (uint)'3TXD')
		{
			data.format = PF_DXT3;
		}
		//else if(ddsd2.ddpfPixelFormat.dwFourCC == (uint)'4TXD')
		//{
		//	data.format = PF_DXT4;
		//}
		else
		{
			data.format = PF_DXT5;
		}

		uint ui32ImageDataSize = input->getLength() - sizeof(ddsd2) - 4;
		data.size = ui32ImageDataSize;
		data.pData = new uchar[ui32ImageDataSize];
		input->read(data.pData,ui32ImageDataSize);

		//Alpha Info
		/*data.pTransparenceData = 0;
		switch(data.format)
		{
		case PF_DXT1:
			{
				bool hasAlpha = false;
				data.pTransparenceData = new uchar[data.width * data.height];
				int blockW = (data.width >> 2);
				int blockH = (data.height >> 2);
				for(int i = 0;i < blockW;i++)
				for(int j = 0;j < blockH;j++)
				{
					uchar *c = data.pData + ((blockW * j + i) << 3);
					ushort c0,c1;
					uint bits;
					c0 = c[0] + (c[1] << 8);
					c1 = c[2] + (c[3] << 8);
					bits = c[4] + ((c[5] + ((c[6] + (c[7] << 8)) << 8)) << 8);

					for(uint k = 0;k < 4;k++)
					for(uint l = 0;l < 4;l++)
					{
						uchar code = ((bits >> (((l << 2) + k) << 1)) & 0x3);

						int w = i * 4 + k;
						int h = j * 4 + l;
						if(c0 < c1 && code == 3)
						{
							hasAlpha = true;
							data.pTransparenceData[h * data.width + w] = 0;
						}
						else
						{
							data.pTransparenceData[h * data.width + w] = 255;
						}
					}
				}
				if(!hasAlpha)
				{
					data.format = PF_RGB_DXT1;
					delete[] data.pTransparenceData;
					data.pTransparenceData = 0;
				}
			}
			break;
		case PF_DXT3:
			break;
		case PF_DXT5:
			break;
		}*/

		return true;
	}

	bool ImageCodecDds::code(void ) const
	{
		Error("Code to DDS Not Implement Yet!");
		return false;
	}

	bool ImageCodecDds::codeToFile(xs::FileStream&,const ImageData& data) const
	{
		Error("Code to DDS Not Implement Yet!");
		return false;
	}

}