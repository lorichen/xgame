#ifndef __ImageCodecSfp_H__
#define __ImageCodecSfp_H__

#include "ImageCodec.h"

namespace xs
{
	class ImageCodecSfp : public ImageCodec
	{
	protected:
		ImageCodecSfp();
	public:
		static ImageCodec*	Instance()
		{
			static ImageCodecSfp codec;
			return &codec;
		}
	public:
		ImageCodecImplement
	private:
		uchar m_defaultPal[512];
	};

}
#endif