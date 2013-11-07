#include "StdAfx.h"
#include "TextureManager.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "RenderSystem.h"

#if (TARGET_PLATFORM == PLATFORM_WIN32)
//for pbuffer
RenderTarget::RenderTarget(RenderSystem* pRenderSystem) :rc(0),dc(0),m_b2D(false),m_hWnd(0)\
,m_pRenderSystem(pRenderSystem),m_FrameBufferObj(0)
{
	m_eglDisplay = m_pRenderSystem->m_eglDisplay;
	m_eglSurface = m_pRenderSystem->m_eglSurface;
}

//for self
RenderTarget::RenderTarget(HDC dc,HWND hWnd,RenderSystem* pRenderSystem,EGLContext shareContext) 
: dc(dc),m_b2D(false),m_hWnd(hWnd)
,m_pRenderSystem(pRenderSystem),m_FrameBufferObj(0)
{
	m_vpLeft = m_vpTop = 0;
	m_vpWidth = m_vpHeight = 0;

	m_eglDisplay = m_pRenderSystem->m_eglDisplay;
	m_eglSurface = m_pRenderSystem->m_eglSurface;

	EGLint ai32ContextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	rc = eglCreateContext(m_eglDisplay, m_pRenderSystem->m_eglConfig, shareContext, ai32ContextAttribs);
	eglMakeCurrent(m_eglDisplay,m_eglSurface,m_eglSurface,rc);
}

GLuint  RenderTarget::GetFrameBufferObj()
{
    return m_FrameBufferObj;
}
#else

RenderTarget::RenderTarget(RenderSystem* pRenderSystem) :m_pContext(NULL),m_b2D(false)
,m_pRenderSystem(pRenderSystem),m_FrameBufferObj(0)
{
	//use by pbuffer
}

RenderTarget::RenderTarget(void* uiView,RenderSystem* pRenderSystem,void* shareContext)
:m_pContext(NULL)
,m_pRenderSystem(pRenderSystem),m_FrameBufferObj(0)
,m_b2D(false)
{
    bool b = _CreateIOS(uiView,shareContext);
    
    if(!b)
    {
        printf("create ios opengles2 error!");
    }
    else
    {
        Reset();
    }
}

#endif

void 		RenderTarget::release()
{
#if (TARGET_PLATFORM == PLATFORM_WIN32)

	eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(m_eglDisplay, rc);
    
#else
    _DestoryIOS();
#endif

	delete this;
}

const ITexture*	RenderTarget::getTexture()
{
	return 0;
}

void 		RenderTarget::onAttach()
{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
	eglMakeCurrent(m_eglDisplay,m_eglSurface,m_eglSurface,rc);
	glBindFramebuffer(GL_FRAMEBUFFER,m_FrameBufferObj);
	TestGLError("RenderTarget::onAttach | glBindFramebuffer");
#else
    
#endif
}

void 		RenderTarget::onDetach()
{
}

void 		RenderTarget::onEndFrame(int layer)
{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
	eglSwapBuffers(m_eglDisplay, m_eglSurface);
	TestGLError("RenderTarget::onEndFrame | eglSwapBuffers");
#else
    Present();
#endif
}

bool		RenderTarget::getTextureData(void  *pData)
{
	assert(0);
	return false;
}

void		RenderTarget::getRect(RECT *rc)
{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
	GetClientRect(m_hWnd,rc);
#else
    GetClientRect(rc);
#endif
}