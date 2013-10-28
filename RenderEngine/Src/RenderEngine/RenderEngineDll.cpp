#include "StdAfx.h"
#include "RenderEngine.h"
#include "ImageCodecBlp.h"
#include "ImageCodecDds.h"
#include "ImageCodecTga.h"
#include "ImageCodecBmp.h"
#include "ImageCodecJpg.h"
#include "ImageCodecPng.h"
#include "ImageCodecPcx.h"
#include "ImageCodecRaw.h"
#include "ImageCodecSfp.h"

namespace xs
{

	IRenderEngine*	g_pRenderEngine = 0;
	struct IResourceManager;

	extern "C" __declspec(dllexport) IRenderEngine* LoadRenderEngine(RenderEngineCreationParameters *param)
	{
		if(g_pRenderEngine)return g_pRenderEngine;

		RenderEngine *pRenderEngine = new RenderEngine;
		if(pRenderEngine)
		{
			if(!pRenderEngine->create(param))
			{
				delete pRenderEngine;
				pRenderEngine = 0;
			}
		}

		g_pRenderEngine = pRenderEngine;
		return g_pRenderEngine;
	}

	extern "C" __declspec(dllexport) IRenderEngine* getRenderEngine()
	{
		return g_pRenderEngine;
	}

	extern "C" __declspec(dllexport) void registerImageCodec()
	{
		//Register ImageCodecs
		ImageCodec::registerCodec(ImageCodecBlp::Instance());
		ImageCodec::registerCodec(ImageCodecDds::Instance());
		ImageCodec::registerCodec(ImageCodecTga::Instance());
		//ImageCodec::registerCodec(ImageCodecBmp::Instance());
		ImageCodec::registerCodec(ImageCodecJpg::Instance());
		ImageCodec::registerCodec(ImageCodecPng::Instance());
		ImageCodec::registerCodec(ImageCodecPcx::Instance());
		ImageCodec::registerCodec(ImageCodecRaw::Instance());
		//ImageCodec::registerCodec(ImageCodecSfp::Instance());
	}
}