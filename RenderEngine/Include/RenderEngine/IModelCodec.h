#ifndef __I_ModelCodec_H__
#define __I_ModelCodec_H__

namespace xs
{
	struct IModelCodec
	{
		virtual const char*	getType() const = 0;
		virtual bool decode(xs::Stream *pStream) = 0;
	};
};

#endif