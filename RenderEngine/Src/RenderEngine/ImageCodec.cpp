#include "StdAfx.h"
#include "PixelFormat.h"
#include "ImageCodec.h"

namespace xs
{
	ImageCodec::CodecList	ImageCodec::m_codecs;

	ImageCodec* ImageCodec::getCodec(const char* extension)
	{
		std::string lwrcase = extension;
		CodecList::const_iterator i = m_codecs.find(lwrcase);
		if (i == m_codecs.end())
		{
			return 0;
		}

		return i->second;
	}
}