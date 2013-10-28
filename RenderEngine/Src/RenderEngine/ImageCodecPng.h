#ifndef __ImageCodecPng_H__
#define __ImageCodecPng_H__

#include "ImageCodec.h"

namespace xs
{
	class ImageCodecPng : public ImageCodec
	{
	protected:
		ImageCodecPng(){}
	public:
		static ImageCodec*	Instance()
		{
			static ImageCodecPng codec;
			return &codec;
		}
	public:
		ImageCodecImplement
	};

}
#endif