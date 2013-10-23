#ifndef __ImageCodecBmp_H__
#define __ImageCodecBmp_H__

#include "ImageCodec.h"

namespace xs
{
#pragma pack(push,2)
	typedef struct tagBITMAPFILEHEADER
	{
		ushort	bfType;
		uint	bfSize;
		ushort	bfReserved1;
		ushort	bfReserved2;
		uint	bfOffBits;
	}BITMAPFILEHEADER;
	//sizeof(BITMAPFILEHEADER)==14
#pragma pack(pop)
	typedef struct tagBITMAPINFOHEADER
	{
		uint		biSize;
		long		biWidth;
		long		biHeight;
		ushort      biPlanes;
		ushort      biBitCount;
		uint      biCompression;
		uint      biSizeImage;
		long		biXPelsPerMeter;
		long		biYPelsPerMeter;
		uint      biClrUsed;
		uint      biClrImportant;
	}BITMAPINFOHEADER;
	//sizeof(BITMAPINFOHEADER)==40

	typedef struct tagRGBQUAD
	{
		uchar   rgbBlue;
		uchar	rgbGreen;
		uchar	rgbRed;
		uchar	rgbReserved;
	} RGBQUAD;

	/*
	 *	1bit,4bit,8bit,16bit(R5G6B5),24bit,32bit(R8G8B8A8),uncompressed
	 */
	class ImageCodecBmp : public ImageCodec
	{
	protected:
		ImageCodecBmp(){}
	public:
		static ImageCodec*	Instance()
		{
			static ImageCodecBmp codec;
			return &codec;
		}
	public:
		ImageCodecInterface(;)
	private:
		bool	decode1(xs::Stream *input,ImageData& data,BITMAPFILEHEADER& fh,BITMAPINFOHEADER& ih) const;
		bool	decode4(xs::Stream *input,ImageData& data,BITMAPFILEHEADER& fh,BITMAPINFOHEADER& ih) const;
		bool	decode8(xs::Stream *input,ImageData& data,BITMAPFILEHEADER& fh,BITMAPINFOHEADER& ih) const;
		bool	decode16(xs::Stream *input,ImageData& data,BITMAPFILEHEADER& fh,BITMAPINFOHEADER& ih,uchar*) const;
		bool	decode24(xs::Stream *input,ImageData& data,BITMAPFILEHEADER& fh,BITMAPINFOHEADER& ih) const;
		bool	decode32(xs::Stream *input,ImageData& data,BITMAPFILEHEADER& fh,BITMAPINFOHEADER& ih) const;

		uint countBits32(uint data) const;
	};

}
#endif