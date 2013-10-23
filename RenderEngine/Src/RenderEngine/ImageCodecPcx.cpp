#include "StdAfx.h"
#include "PixelFormat.h"
#include "ImageCodecPcx.h" 

namespace xs
{
	const char*	ImageCodecPcx::getType() const
	{
		static std::string strType = "pcx";
		return strType.c_str();
	}

#define Read(x)					input->read(&x,sizeof(x));
#define Define_And_Read_8(x)	uchar x;input->read(&x,sizeof(x));
#define Define_And_Read_16(x)	ushort x;input->read(&x,sizeof(x));

	bool ImageCodecPcx::getDimension(xs::Stream* input,uint& width,uint& height,uint& depth)
	{
		Define_And_Read_8(manufacturer);
		Define_And_Read_8(version);
		Define_And_Read_8(encoding);
		Define_And_Read_8(bitsPerPixel);

		if((manufacturer != 0x0A) || (encoding != 0x01))
		{
			Error("PCX file is corrupted");
			return false;
		}

		Define_And_Read_16(xmin);
		Define_And_Read_16(ymin);
		Define_And_Read_16(xmax);
		Define_And_Read_16(ymax);

		width = xmax - xmin + 1;
		height = ymax - ymin + 1;

		return true;
	}


	bool ImageCodecPcx::decode(xs::Stream* input,ImageData& data,const void  *p1,const void  *p2) const
	{
		Define_And_Read_8(manufacturer);
		Define_And_Read_8(version);
		Define_And_Read_8(encoding);
		Define_And_Read_8(bitsPerPixel);

		if((manufacturer != 0x0A) || (encoding != 0x01))
		{
			Error("PCX file is corrupted");
			return false;
		}

		Define_And_Read_16(xmin);
		Define_And_Read_16(ymin);
		Define_And_Read_16(xmax);
		Define_And_Read_16(ymax);

		Define_And_Read_16(horizDPI);
		Define_And_Read_16(vertDPI);

		uchar colorMap[48];
		Read(colorMap);

		input->seek(1,SEEK_CUR);

		Define_And_Read_8(planes);
		Define_And_Read_16(bytesPerLine);
		Define_And_Read_16(paletteType);
		input->seek(4 + 54,SEEK_CUR);

		(void )bytesPerLine;
		(void )version;
		(void )vertDPI;
		(void )horizDPI;

		if((bitsPerPixel != 8) || ((planes != 1) && (planes != 3)))
		{
			Error("Only 8-bit paletted and 24-bit PCX files supported");
			return false;
		}
		if(!(((paletteType == 1) && (planes == 3)) || (planes == 1)))
		{
			Error("Only 8-bit paletted and 24-bit PCX files supported");
			return false;
		}

		data.width = xmax - xmin + 1;
		data.height = ymax - ymin + 1;
		data.depth = 1;
		data.num_mipmaps = 1;
		data.flags = 0;
		data.format = PF_R8G8B8;
		data.size = data.width * data.height * 3;
		data.pData = new uchar[data.size];

		if((paletteType == 1) && (planes == 3))
		{
			uchar* pixel = data.pData;

			// Iterate over each scan line
			for(uint row = 0;row < data.height;++row)
			{
				// Read each scan line once per plane
				for(int plane = 0;plane < planes;++plane)
				{
					int p = row * data.width;
					int p1 = p + data.width;
					while(p < p1)
					{
						Define_And_Read_8(value);
						int length = 1;

						if(value >= 192)
						{
							// This is the length, not the value.  Mask off
							// the two high bits and read the true index.
							length = value & 0x3F;
							Read(value);
						}

						// Set the whole run
						for(int i = length - 1;i >= 0;--i,++p)
						{
							ASSERT((uint)p < data.width * data.height);
							pixel[p * 3 + plane] = value;
						}
					}
				}
			}

		}
		else if(planes == 1)
		{
			uchar palette[768];

			int imarkteginning   = input->getPosition();
			int paletteBeginning = input->getLength() - 769;

			input->seek(paletteBeginning,SEEK_SET);

			Define_And_Read_8(dummy);

			ASSERT(dummy == 12);

			Read(palette);
			input->seek(imarkteginning,SEEK_SET);

			uchar* pixel = data.pData;

			// The palette indices are run length encoded.
			uint p = 0;
			while(p < data.width * data.height)
			{
				Define_And_Read_8(index);
				uchar length = 1;

				if(index >= 192)
				{
					// This is the length, not the index.  Mask off
					// the two high bits and read the true index.
					length = index & 0x3F;
					Read(index);
				}

				uchar *color = &palette[index * 3];

				// Set the whole run
				for(int i = length - 1;i >= 0;--i,++p)
				{
					ASSERT(p < data.width * data.height);
					uint p3 = p * 3;
					pixel[p3 + 0] = color[0];
					pixel[p3 + 1] = color[1];
					pixel[p3 + 2] = color[2];
				}
			}
		}

		return true;
	}

	bool ImageCodecPcx::code(void ) const
	{
		Error("Code to PCX Not Implemented Yet!");
		return false;
	}

	bool ImageCodecPcx::codeToFile(xs::FileStream&,const ImageData& data) const
	{
		Error("Code to PCX Not Implemented Yet!");
		return false;
	}

}