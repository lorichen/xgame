#include "StdAfx.h"
#include "RenderEngine.h"
#include "Camera.h"
#include "Viewport.h"
#include "ImageCodecBlp.h"
#include "ImageCodecDds.h"
#include "ImageCodecTga.h"
#include "ImageCodecBmp.h"
#include "ImageCodecJpg.h"
#include "ImageCodecPng.h"
#include "ImageCodecPcx.h"
#include "ImageCodecRaw.h"
#include "ImageCodecSfp.h"
#include "CommonIndexData.h"
#include "FontManager.h"
#include "_ParticleSystemEmitter.h"
#include "_ParticleSystemEmitterPlane.h"
#include "_ParticleSystemEmitterSphere.h"
#include "_ParticleSystemEmitterRealSphere.h"

namespace xs
{
	RenderEngine::RenderEngine() : m_pRenderSystem(0),m_pModelManager(0)
	{
	}

	RenderEngine::~RenderEngine()
	{
	}

	Viewport*		RenderEngine::createViewport()
	{
		return new Viewport;
	}

	const RenderEngineCreationParameters& RenderEngine::getCreationParams()
	{
		return m_param;
	}

	Camera*			RenderEngine::createCamera(Viewport* pViewport)
	{
		return new Camera(pViewport);
	}

	IModelManager*	RenderEngine::createModelManager()
	{
		return m_ModelManager.create(m_pRenderSystem);
	}

	IRenderSystem* RenderEngine::getRenderSystem()
	{
		return m_pRenderSystem;
	}

	IRenderSystem*	RenderEngine::createRenderSystem(RenderEngineCreationParameters* param)
	{
		IRenderSystem *pRenderSystem = 0;
		char szRenderSystemDllName[MAX_PATH];

		switch(param->rst)
		{
			case RS_OPENGL:
			{
				#ifdef _DEBUG
					strcpy(szRenderSystemDllName,"RenderSystemGL_d.dll");
				#else
					#ifdef RELEASEDEBUG
						strcpy(szRenderSystemDllName,"RenderSystemGL_rd.dll");
					#else
						strcpy(szRenderSystemDllName,"RenderSystemGL.dll");
					#endif
				#endif

				pRenderSystem = m_RenderSystem.create(szRenderSystemDllName,param);
			}
			break;
	
			case RS_OPENGLES2:
			{
#ifdef _DEBUG
				strcpy(szRenderSystemDllName,"RenderSystemGLES2_d.dll");
#else
#ifdef RELEASEDEBUG
				strcpy(szRenderSystemDllName,"RenderSystemGLES2_rd.dll");
#else
				strcpy(szRenderSystemDllName,"RenderSystemGLES2.dll");
#endif
#endif
				pRenderSystem = m_RenderSystem.create(szRenderSystemDllName,param);
			}
			break;

			case RS_D3D9:
			{
				#ifdef _DEBUG
					strcpy(szRenderSystemDllName,"xs_renderd3d_d.dll");
				#else
					#ifdef RELEASEDEBUG
						strcpy(szRenderSystemDllName,"xs_renderd3d_rd.dll");
					#else
						strcpy(szRenderSystemDllName,"xs_renderd3d.dll");
					#endif
				#endif

				pRenderSystem = m_RenderSystem.create(szRenderSystemDllName,param);
			}
			break;
		};

		return pRenderSystem;
	}

	void  RenderEngine::closeRenderSystem()
	{
		safeRelease(m_pRenderSystem);
	}

	extern IRenderEngine *g_pRenderEngine;
	void  RenderEngine::release()
	{
		g_pRenderEngine = 0;
		safeRelease(m_pModelManager);
		getFontManager()->releaseAll();

		closeRenderSystem();

		delete this;
	}

	IModelManager*		RenderEngine::getModelManager()
	{
		return m_pModelManager;
	}

	IFontManager*		RenderEngine::getFontManager()
	{
		return FontManager::Instance();
	}

	bool			RenderEngine::create(RenderEngineCreationParameters* param)
	{
		if(m_pRenderSystem)return true;

		ASSERT(param);
		m_param = *param;

		PP_BY_NAME_START("createRenderSystem");
		m_pRenderSystem = createRenderSystem(param);
		PP_BY_NAME_STOP();
		if(m_pRenderSystem)
		{
			//Register ImageCodecs
			ImageCodec::registerCodec(ImageCodecBlp::Instance());
			ImageCodec::registerCodec(ImageCodecDds::Instance());
			ImageCodec::registerCodec(ImageCodecTga::Instance());
			ImageCodec::registerCodec(ImageCodecBmp::Instance());
			ImageCodec::registerCodec(ImageCodecJpg::Instance());
			ImageCodec::registerCodec(ImageCodecPng::Instance());
			ImageCodec::registerCodec(ImageCodecPcx::Instance());
			ImageCodec::registerCodec(ImageCodecRaw::Instance());
			ImageCodec::registerCodec(ImageCodecSfp::Instance());

			//Register EmitterFactories
			ParticleSystemEmitterFactory::registerEmitterFactory(PSET_PLANE,ParticleSystemEmitterFactoryPlane::Instance());
			ParticleSystemEmitterFactory::registerEmitterFactory(PSET_SPHERE,ParticleSystemEmitterFactorySphere::Instance());
			ParticleSystemEmitterFactory::registerEmitterFactory(PSET_REAL_SPHERE, ParticleSystemEmitterFactoryRealSphere::Instance() );

			PP_BY_NAME_START("createModelManager");
			m_pModelManager = createModelManager();
			PP_BY_NAME_STOP();
		}

		return m_pRenderSystem != 0;
	}
}