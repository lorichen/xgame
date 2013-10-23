#ifndef __ImageCodecBlp_H__
#define __ImageCodecBlp_H__

#include "ImageCodec.h"

namespace xs
{
	//+-----------------------------------------------------------------------------
	//| Constants
	//+-----------------------------------------------------------------------------
	const int MAX_NR_OF_BLP_MIP_MAPS = 16;


	//+-----------------------------------------------------------------------------
	//| Blp header structure
	//+-----------------------------------------------------------------------------
	struct BLP_HEADER
	{
		BLP_HEADER()
		{
			Compression = 0;
			Flags = 0;
			Width = 0;
			Height = 0;
			PictureType = 0;
			PictureSubType = 0;
			ZeroMemory(Offset, MAX_NR_OF_BLP_MIP_MAPS * sizeof(uint));
			ZeroMemory(Size, MAX_NR_OF_BLP_MIP_MAPS * sizeof(uint));
		}

		uint Compression;
		uint Flags;
		uint Width;
		uint Height;
		uint PictureType;
		uint PictureSubType;
		uint Offset[MAX_NR_OF_BLP_MIP_MAPS];
		uint Size[MAX_NR_OF_BLP_MIP_MAPS];
	};


	//+-----------------------------------------------------------------------------
	//| Blp RGBA structure
	//+-----------------------------------------------------------------------------
	struct BLP_RGBA
	{
		uchar Red;
		uchar Green;
		uchar Blue;
		uchar Alpha;
	};


	//+-----------------------------------------------------------------------------
	//| Blp pixel structure
	//+-----------------------------------------------------------------------------
	struct BLP_PIXEL
	{
		uchar Index;
	};

	class ImageCodecBlp : public ImageCodec
	{
	protected:
		ImageCodecBlp(){}
	public:
		static ImageCodec*	Instance()
		{
			static ImageCodecBlp codec;
			return &codec;
		}
	public:
		ImageCodecInterface(;)
	private:
		bool decodeBLP1(xs::Stream* input,ImageData& data) const;
		bool decodeBLP2(xs::Stream* input,ImageData& data) const;

		bool loadCompressedBLP1(const BLP_HEADER& header,xs::Stream* input,ImageData& data) const;
		bool loadUnCompressedBLP1(const BLP_HEADER& header,xs::Stream* input,ImageData& data) const;
	};

}
#endif