#include "StdAfx.h"
#include "TextureManager.h"
#include "Texture.h"
#include "RenderTarget.h"


void 		RenderTarget::release()
{
	delete this;
}

const ITexture*	RenderTarget::getTexture()
{
	return 0;
}

void 		RenderTarget::onAttach()
{
}

void 		RenderTarget::onDetach()
{
}

void 		RenderTarget::onEndFrame(int layer)
{
	PP_BY_NAME_START("glFinish");
	::glFinish();
	PP_BY_NAME_STOP();
	PP_BY_NAME_START("SwapBuffers");
	if(layer == 0)
		wglSwapLayerBuffers(dc,WGL_SWAP_MAIN_PLANE);
	else
		//#define WGL_SWAP_OVERLAY1       0x00000002
		//#define WGL_SWAP_OVERLAY2       0x00000004
		//#define WGL_SWAP_OVERLAY3       0x00000008
		//#define WGL_SWAP_OVERLAY4       0x00000010
		//#define WGL_SWAP_OVERLAY5       0x00000020
		//#define WGL_SWAP_OVERLAY6       0x00000040
		//#define WGL_SWAP_OVERLAY7       0x00000080
		wglSwapLayerBuffers(dc,(1 << layer));

	PP_BY_NAME_STOP();
}

bool		RenderTarget::getTextureData(void  *pData)
{
	return false;
}

void		RenderTarget::getRect(RECT *rc)
{
	GetClientRect(m_hWnd,rc);
}