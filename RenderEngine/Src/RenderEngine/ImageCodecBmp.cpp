#include "StdAfx.h"
#include "PixelFormat.h"
#include "ImageCodecBmp.h" 

namespace xs
{
	const char*	ImageCodecBmp::getType() const
	{
		static std::string strType = "bmp";
		return strType.c_str();
	}

	bool	ImageCodecBmp::decode1(xs::Stream *input,ImageData& data,BITMAPFILEHEADER& fh,BITMAPINFOHEADER& ih) const
	{
		data.format = PF_R8G8B8;
		data.size = data.width * data.height * 3;
		data.pData = new uchar[data.size];

		RGBQUAD rgb[2];
		input->read(rgb,sizeof(rgb));

		//Align(4)
		int ui32Width = ((((ih.biWidth * ih.biBitCount) + 31) & ~31) / 8);

		uint Mask1[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

		for(int y = 0;y < ih.biHeight;++y)
		{
			uchar *pData = data.pData + (ih.biHeight - y - 1) * ih.biWidth * 3;
			int width = 0;
			for(int x = 0;x < ui32Width;++x)
			{
				uchar c;
				input->read(&c,1);
				for(int i = 7;i >= 0 && width < ih.biWidth;--i,++width)
				{
					uint index  = ((c & Mask1[i]) >> i);

					*pData++ = rgb[index].rgbRed;
					*pData++ = rgb[index].rgbGreen;
					*pData++ = rgb[index].rgbBlue;
				}
			}
		}

		return true;
	}

	bool	ImageCodecBmp::decode4(xs::Stream *input,ImageData& data,BITMAPFILEHEADER& fh,BITMAPINFOHEADER& ih) const
	{
		data.format = PF_R8G8B8;
		data.size = data.width * data.height * 3;
		data.pData = new uchar[data.size];

		RGBQUAD rgb[16];
		input->read(rgb,sizeof(rgb));

		//Align(4)
		int ui32Width = ((((ih.biWidth * ih.biBitCount) + 31) & ~31) / 8);

		uint Mask4[2] = {0x0F,0xF0};

		for(int y = 0;y < ih.biHeight;++y)
		{
			uchar *pData = data.pData + (ih.biHeight - y - 1) * ih.biWidth * 3;
			int width = 0;
			for(int x = 0;x < ui32Width;++x)
			{
				uchar c;
				input->read(&c,1);
				for(int i = 1;i >= 0 && width < ih.biWidth;--i,++width)
				{
					uint index  = ((c & Mask4[i]) >> (i * 4));

					*pData++ = rgb[index].rgbRed;
					*pData++ = rgb[index].rgbGreen;
					*pData++ = rgb[index].rgbBlue;
				}
			}
		}

		return true;
	}

	bool	ImageCodecBmp::decode8(xs::Stream *input,ImageData& data,BITMAPFILEHEADER& fh,BITMAPINFOHEADER& ih) const
	{
		data.format = PF_R8G8B8;
		data.size = data.width * data.height * 3;
		data.pData = new uchar[data.size];

		RGBQUAD rgb[256];
		input->read(rgb,sizeof(rgb));

		//Align(4)
		int ui32Width = ((((ih.biWidth * ih.biBitCount) + 31) & ~31) / 8);

		for(int y = 0;y < ih.biHeight;++y)
		{
			uchar *pData = data.pData + (ih.biHeight - y - 1) * ih.biWidth * 3;
			int width = 0;
			for(int x = 0;x < ui32Width;++x,++width)
			{
				uchar c;
				input->read(&c,1);
				
				if(width < ih.biWidth)
				{
					*pData++ = rgb[c].rgbRed;
					*pData++ = rgb[c].rgbGreen;
					*pData++ = rgb[c].rgbBlue;
				}
			}
		}

		return true;
	}

	uint ImageCodecBmp::countBits32(uint data) const
	{
		uint bitCount = 0;
		while(data)
		{
			if(data & 1)bitCount++;
			data >>= 1;
		}

		return bitCount;
	}

#define MAKE556WORD(r,g,b) ((((WORD)(r) >> 3) << 11) | (((WORD)(g) >> 3) << 6)  | ((WORD)(b) >> 2));
#define MAKE565WORD(r,g,b) ((((WORD)(r) >> 3) << 11) | (((WORD)(g) >> 2) << 5)  | ((WORD)(b) >> 3));
#define MAKE555WORD(r,g,b) ((((WORD)(r) >> 3) << 10) | (((WORD)(g) >> 3) << 5)  | ((WORD)(b) >> 3));
#define MAKE1555WORD(a,r,g,b) ((((WORD)(r) >> 3) << 10) | (((WORD)(g) >> 3) << 5)  | ((WORD)(b) >> 3) | ((WORD)(a) << 15));
#define MAKE565RGB(w,r,g,b) (r = (w & 0xF800) >> 8, g = (w & 0x07E0) >> 3, b = (w & 0x001F) << 3); 
#define MAKE555RGB(w,r,g,b) (r = (w & 0x7C00) >> 7, g = (w & 0x03E0) >> 2, b = (w & 0x001F) << 3); 

	bool	ImageCodecBmp::decode16(xs::Stream *input,ImageData& data,BITMAPFILEHEADER& fh,BITMAPINFOHEADER& ih,uchar *pBits) const
	{
		ushort mask = 0x0000;
		uint rBitCount = 0;
		uint gBitCount = 0;
		uint bBitCount = 0;
		uint aBitCount = 0;
		switch(ih.biCompression)
		{
		case BI_RGB:
			{
				data.format = PF_A1R5G5B5;
				mask = 0x8000;
			}
			break;
		case BI_BITFIELDS:
			{
				uint rMask = *(uint*)pBits;
				uint gMask = *((uint*)pBits + 1);
				uint bMask = *((uint*)pBits + 2);
				uint aMask = *((uint*)pBits + 3);
				rBitCount = countBits32(rMask);
				gBitCount = countBits32(gMask);
				bBitCount = countBits32(bMask);
				aBitCount = countBits32(aMask);
				if(aBitCount == 0 && rBitCount == 5 && gBitCount == 6 && bBitCount == 5)
				{
					data.format = PF_R5G6B5;
				}
				else if(aBitCount == 1 && rBitCount == 5 && gBitCount == 5 && bBitCount == 5)
				{
					data.format = PF_A1R5G5B5;
				}
				else if(aBitCount == 0 && rBitCount == 4 && gBitCount == 4 && bBitCount == 4)
				{
					data.format = PF_A4R4G4B4;
					/*
					因为没有PF_X4R4G4B4
					*/
					mask = 0xF000;
				}
				else if(aBitCount == 4 && rBitCount == 4 && gBitCount == 4 && bBitCount == 4)
				{
					data.format = PF_A4R4G4B4;
				}
				else 
				{
					return false;
				}
			}
			break;
		}

		data.size = data.width * data.height * 2;
		data.pData = new uchar[data.size];

		//Align(4)
		uint ui32Width = ((ih.biWidth * 2 + 3) & ~3);

		for(uint y = 0;y < data.height;++y)
		{
			ushort *pData = (ushort*)(data.pData + (ih.biHeight - y - 1) * ih.biWidth * 2);
			for(uint x = 0;x < data.width;++x)
			{
				ushort c;
				input->read(&c,2);

				*pData++ = (c | mask);
			}
			if(ui32Width > data.width * 2)input->seek(ui32Width - data.width * 2,SEEK_CUR);
		}

		return true;
	}

	bool	ImageCodecBmp::decode24(xs::Stream *input,ImageData& data,BITMAPFILEHEADER& fh,BITMAPINFOHEADER& ih) const
	{
		data.format = PF_B8G8R8;
		data.size = data.width * data.height * 3;
		data.pData = new uchar[data.size];

		//Align(4)
		uint ui32Width = ((ih.biWidth * 3 + 3) & ~3);

		for(uint y = 0;y < data.height;++y)
		{
			uchar *pData = data.pData + (ih.biHeight - y - 1) * ih.biWidth * 3;
			for(uint x = 0;x < data.width;++x)
			{
				uchar r,g,b;
				input->read(&r,1);
				input->read(&g,1);
				input->read(&b,1);
				
				*pData++ = r;
				*pData++ = g;
				*pData++ = b;
			}
			if(ui32Width > data.width * 3)input->seek(ui32Width - data.width * 3,SEEK_CUR);
		}

		return true;
	}

	bool	ImageCodecBmp::decode32(xs::Stream *input,ImageData& data,BITMAPFILEHEADER& fh,BITMAPINFOHEADER& ih) const
	{
		data.format = PF_A8R8G8B8;
		data.size = data.width * data.height * 4;
		data.pData = new uchar[data.size];

		//Align(4) - 32Bit is always Align(4)
		int ui32Width = ih.biWidth;

		for(int y = 0;y < ih.biHeight;++y)
		{
			uchar *pData = data.pData + (ih.biHeight - y - 1) * ih.biWidth * 4;
			for(int x = 0;x < ui32Width;++x)
			{
				uchar r,g,b,a;
				input->read(&r,1);
				input->read(&g,1);
				input->read(&b,1);
				input->read(&a,1);
				*pData++ = r;
				*pData++ = g;
				*pData++ = b;
				*pData++ = a;
			}
		}

		return true;
	}

	bool ImageCodecBmp::getDimension(xs::Stream* input,uint& width,uint& height,uint& depth)
	{
		uint fileLength = input->getLength();
		if(fileLength < sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER))
		{
			Error("Invalid BMP File - invalid size\n");
			return false;
		}
		BITMAPFILEHEADER fh;
		input->read(&fh,sizeof(fh));
		if(fh.bfType != (ushort)'MB')
		{
			Error("Invalid BMP File - Not BM\n");
			return false;
		}

		BITMAPINFOHEADER ih;
		input->read(&ih,sizeof(ih));

		width = ih.biWidth;
		height = ih.biHeight;
		depth = 1;

		return true;
	}

	bool ImageCodecBmp::decode(xs::Stream* input,ImageData& data,const void  *p1,const void  *p2) const
	{
		uint fileLength = input->getLength();
		if(fileLength < sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER))
		{
			Error("Invalid BMP File");
			return false;
		}
		BITMAPFILEHEADER fh;
		input->read(&fh,sizeof(fh));
		if(fh.bfType != (ushort)'MB')
		{
			Error("Invalid BMP File");
			return false;
		}
		//Only BI_RGB format BMP fh.bfSize == fileLength
		//if(fh.bfSize != fileLength)
		//{
		//	Error("Invalid BMP File");
		//	return false;
		//}
		BITMAPINFOHEADER ih;
		input->read(&ih,sizeof(ih));
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#define BI_JPEG       4L
#define BI_PNG        5L
		if(ih.biCompression != BI_RGB && ih.biBitCount != 16)
		{
			Error("BmpDecode not support Non-BI_RGB format now");
			return false;
		}

		data.width = ih.biWidth;
		data.height = ih.biHeight;
		data.depth = 1;
		data.num_mipmaps = 1;
		data.flags = 0;
		//data.format = PF_UNKNOWN;
		//data.size = 0;
		//data.pData = 0;

		uchar *p16Bits = 0;
		//Palatte is not necessarily packed after BITMAPINFOHEADER
		if(ih.biSize > sizeof(ih))
		{
			/*
			这个地方可能会多余一些值，对于16位色Bmp，MSDN中说:
			The bitmap has a maximum of 2^16 colors. If the biCompression member of 
			the BITMAPINFOHEADER is BI_RGB, the bmiColors member of BITMAPINFO is NULL. 
			Each WORD in the bitmap array represents a single pixel. 
			The relative intensities of red, green, and blue are represented with five 
			bits for each color component. The value for blue is in the least significant 
			five bits, followed by five bits each for green and red. The most significant bit 
			is not used. The bmiColors color table is used for optimizing colors used on 
			palette-based devices, and must contain the number of entries specified by the 
			biClrUsed member of the BITMAPINFOHEADER. 
			If the biCompression member of the BITMAPINFOHEADER is BI_BITFIELDS, 
			the bmiColors member contains three DWORD color masks that specify the red, green, 
			and blue components, respectively, of each pixel. 
			Each WORD in the bitmap array represents a single pixel.
			*/
			uint length = ih.biSize - sizeof(ih);
			if(ih.biCompression != BI_RGB && ih.biBitCount == 16)
			{
				p16Bits = new uchar[length];
				input->read(p16Bits,length);
			}
			else
			{
				input->seek(length,SEEK_CUR);
			}
		}

		bool bDecode = false;
		switch(ih.biBitCount)
		{
		case 1:		bDecode = decode1(input,data,fh,ih);	break;
		case 4:		bDecode = decode4(input,data,fh,ih);	break;
		case 8:		bDecode = decode8(input,data,fh,ih);	break;
		case 16:	bDecode = decode16(input,data,fh,ih,p16Bits);	break;
		case 24:	bDecode = decode24(input,data,fh,ih);	break;
		case 32:	bDecode = decode32(input,data,fh,ih);	break;
		}

		if(p16Bits)delete[] p16Bits;
		return bDecode;
	}

	bool ImageCodecBmp::code(void ) const
	{
		Error("Code to BMP Not Implemented Yet!");
		return false;
	}

	bool ImageCodecBmp::codeToFile(xs::FileStream& output,const ImageData& data) const
	{
		// 构造文件头和信息头
		BITMAPFILEHEADER bfh;
		BITMAPINFOHEADER bih;

		uint headsize = sizeof(bfh) + sizeof(bih);
		uint nFileSize = headsize + data.width * data.height * 4;

		bfh.bfType = 'MB';
		bfh.bfSize = nFileSize;
		bfh.bfReserved1 = 0;
		bfh.bfReserved2 = 0;
		bfh.bfOffBits = headsize;

		bih.biSize			= sizeof(bih);
		bih.biWidth			= data.width;
		bih.biHeight		= data.height;
		bih.biPlanes		= 1;
		bih.biBitCount		= 32;
		bih.biCompression	= 0;
		bih.biSizeImage		= 0;
		bih.biXPelsPerMeter	= 72;
		bih.biYPelsPerMeter	= 72;
		bih.biClrUsed		= 0;
		bih.biClrImportant	= 0;

		output.write(&bfh,sizeof(bfh));
		output.write(&bih,sizeof(bih));
		switch(data.format)
		{
		case PF_A8R8G8B8:
			{
				for(uint i = 0;i < data.height;i++)
				{
					uint *pLine = (uint*)(data.pData + (data.height - i - 1) * data.width * 4);
					output.write(pLine,data.width * 4);
				}
			}
			break;
		}

		return false;
	}

}