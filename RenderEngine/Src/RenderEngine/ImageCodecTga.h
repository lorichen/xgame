#ifndef __ImageCodecTga_H__
#define __ImageCodecTga_H__

#include "ImageCodec.h"

namespace xs
{
	class ImageCodecTga : public ImageCodec
	{
	protected:
		ImageCodecTga(){}
	public:
		static ImageCodec*	Instance()
		{
			static ImageCodecTga codec;
			return &codec;
		}
	public:
		ImageCodecInterface(;)
	};

}
#endif