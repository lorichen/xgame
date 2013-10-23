#ifndef __ImageCodecPcx_H__
#define __ImageCodecPcx_H__

#include "ImageCodec.h"

namespace xs
{
	class ImageCodecPcx : public ImageCodec
	{
	protected:
		ImageCodecPcx(){}
	public:
		static ImageCodec*	Instance()
		{
			static ImageCodecPcx codec;
			return &codec;
		}
	public:
		ImageCodecInterface(;)
	};

}
#endif