#ifndef __ImageCodecDds_H__
#define __ImageCodecDds_H__

#include "ImageCodec.h"

namespace xs
{
	class ImageCodecDds : public ImageCodec
	{
	protected:
        bool m_trans2rgba;
		ImageCodecDds();
	public:
		static ImageCodec*	Instance()
		{
			static ImageCodecDds codec;
			return &codec;
		}
	public:
		ImageCodecImplement
	};

}
#endif