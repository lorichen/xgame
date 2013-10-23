#ifndef __ImageCodecJpg_H__
#define __ImageCodecJpg_H__

#include "ImageCodec.h"

namespace xs
{
	class ImageCodecJpg : public ImageCodec
	{
	protected:
		ImageCodecJpg(){}
	public:
		static ImageCodec*	Instance()
		{
			static ImageCodecJpg codec;
			return &codec;
		}
	public:
		ImageCodecInterface(;)
	};

}
#endif