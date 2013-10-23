#include "StdAfx.h"
#include "PixelFormat.h"
#include "ImageCodecRaw.h" 

namespace xs
{

	const char*	ImageCodecRaw::getType() const
	{
		static std::string strType = "raw";
		return strType.c_str();
	}

	bool ImageCodecRaw::getDimension(xs::Stream* input,uint& width,uint& height,uint& depth)
	{
		uint ui32RawLength = input->getLength();
		uint ui32RawLengthSQRT = Math::Sqrt(ui32RawLength);
		if(ui32RawLengthSQRT * ui32RawLengthSQRT != ui32RawLength)
		{
			Error("RAW data length must be N * N");
			return false;
		}

		height = ui32RawLengthSQRT;
		width = ui32RawLengthSQRT;
		depth = 1;

		return true;
	}


	bool ImageCodecRaw::decode(xs::Stream* input,ImageData& data,const void  *p1,const void  *p2) const
	{
		uint ui32RawLength = input->getLength();
		uint ui32RawLengthSQRT = Math::Sqrt(ui32RawLength);
		if(ui32RawLengthSQRT * ui32RawLengthSQRT != ui32RawLength)
		{
			Error("RAW data length must be N * N");
			return false;
		}

		data.height = ui32RawLengthSQRT;
		data.width = ui32RawLengthSQRT;
		data.depth = 1;
		data.size = ui32RawLength;
		data.num_mipmaps = 1;
		data.flags = 0;
		data.format = PF_A8;
		data.pData = new uchar[ui32RawLength];
		input->read(data.pData,ui32RawLength);

		return true;
	}

	bool ImageCodecRaw::code(void ) const
	{
		Error("Code to RAW Not Implemented Yet!");
		return false;
	}

	bool ImageCodecRaw::codeToFile(xs::FileStream&,const ImageData& data) const
	{
		Error("Code to RAW Not Implemented Yet!");
		return false;
	}

}