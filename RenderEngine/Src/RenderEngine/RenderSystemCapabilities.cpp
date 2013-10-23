#include "StdAfx.h"
#include "RenderSystemCapabilities.h"

namespace xs 
{

	//-----------------------------------------------------------------------
	RenderSystemCapabilities::RenderSystemCapabilities() 
		: mNumWorldMatrices(0), mNumTextureUnits(0), mStencilBufferBitDepth(0),
		mNumVertexBlendMatrices(0), mCapabilities(0)
	{
	}
	//-----------------------------------------------------------------------
	RenderSystemCapabilities::~RenderSystemCapabilities()
	{
	}
};
