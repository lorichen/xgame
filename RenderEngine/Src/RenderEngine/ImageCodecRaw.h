#ifndef __ImageCodecRaw_H__
#define __ImageCodecRaw_H__

#include "ImageCodec.h"

namespace xs
{
	class ImageCodecRaw : public ImageCodec
	{
	protected:
		ImageCodecRaw(){}
	public:
		static ImageCodec*	Instance()
		{
			static ImageCodecRaw codec;
			return &codec;
		}
	public:
		ImageCodecImplement
	};

}
#endif