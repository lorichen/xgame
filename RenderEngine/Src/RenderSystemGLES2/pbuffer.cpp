#include "StdAfx.h"
#include "pbuffer.h"
#include "Texture.h"

#define MAX_ATTRIBS     256
#define MAX_PFORMATS    256
#define TEX_SIZE        256

void wglGetLastError()
{
	DWORD err = GetLastError();
	switch(err)
	{
	case ERROR_INVALID_PIXEL_FORMAT:
		Error("Win32 Error:  ERROR_INVALID_PIXEL_FORMAT\n");
		break;
	case ERROR_NO_SYSTEM_RESOURCES:
		Error("Win32 Error:  ERROR_NO_SYSTEM_RESOURCES\n");
		break;
	case ERROR_INVALID_DATA:
		Error("Win32 Error:  ERROR_INVALID_DATA\n");
		break;
	case ERROR_INVALID_WINDOW_HANDLE:
		Error("Win32 Error:  ERROR_INVALID_WINDOW_HANDLE\n");
		break;
	case ERROR_RESOURCE_TYPE_NOT_FOUND:
		Error("Win32 Error:  ERROR_RESOURCE_TYPE_NOT_FOUND\n");
		break;
	case ERROR_SUCCESS:
		// no error
		break;
	default:
		Error("Win32 Error:  " << err << endl);
		break;
	}
	SetLastError(0);
}

bool pbuffer::initialize(int width,int height,int colorBits,int depthBits,
		int stencilBits,HDC hdc,HGLRC hglrc,IRenderSystem* pRenderSystem,
		FilterOptions min, FilterOptions mag, FilterOptions mip,TextureAddressingMode s,TextureAddressingMode t)
{
	m_pRenderSystem = pRenderSystem;

	nv_pbuffer * pbuffer = &m_pbuffer;
	memset(pbuffer,0,sizeof(nv_pbuffer));

	// Query for a suitable pixel format based on the specified mode.
	int     format;
	int     pformat[MAX_PFORMATS];
	unsigned int nformats;    int     iattributes[2*MAX_ATTRIBS];
	float   fattributes[2*MAX_ATTRIBS];
	int     nfattribs = 0;
	int     niattribs = 0;

	// Attribute arrays must be "0" terminated - for simplicity, first
	// just zero-out the array entire, then fill from left to right.
	memset(iattributes,0,sizeof(int)*2*MAX_ATTRIBS);
	memset(fattributes,0,sizeof(float)*2*MAX_ATTRIBS);
	// Since we are trying to create a pbuffer, the pixel format we
	// request (and subsequently use) must be "p-buffer capable".
	iattributes[niattribs  ] = WGL_DRAW_TO_PBUFFER_ARB;
	iattributes[++niattribs] = GL_TRUE;

	iattributes[++niattribs  ] = WGL_COLOR_BITS_ARB;
	iattributes[++niattribs] = colorBits;

	iattributes[++niattribs  ] = WGL_ALPHA_BITS_ARB;
	iattributes[++niattribs] = colorBits == 32 ? 8 : 0;

	iattributes[++niattribs  ] = WGL_DEPTH_BITS_ARB;
	iattributes[++niattribs] = depthBits;

	iattributes[++niattribs  ] = WGL_STENCIL_BITS_ARB;
	iattributes[++niattribs] = stencilBits;

	// we are asking for a pbuffer that is meant to be bound
	// as an RGBA texture - therefore we need a color plane
	iattributes[++niattribs] = WGL_BIND_TO_TEXTURE_RGBA_ARB;
	iattributes[++niattribs] = GL_TRUE;

	if ( !wglChoosePixelFormatARB( hdc, iattributes, fattributes, MAX_PFORMATS, pformat, &nformats ) )
	{
		Error("pbuffer creation error:  wglChoosePixelFormatARB() failed.\n");
		finalize();
		return false;
	}
	wglGetLastError();
	if ( nformats <= 0 )
	{
		Error("pbuffer creation error:  Couldn't find a suitable pixel format.\n");
		finalize();
		return false;
	}
	format = pformat[0];

	// Set up the pbuffer attributes
	memset(iattributes,0,sizeof(int)*2*MAX_ATTRIBS);
	niattribs = 0;
	// the render texture format is RGBA
	iattributes[niattribs] = WGL_TEXTURE_FORMAT_ARB;
	if(colorBits == 32)
		iattributes[++niattribs] = WGL_TEXTURE_RGBA_ARB;
	else
		iattributes[++niattribs] = WGL_TEXTURE_RGB_ARB;
	// the render texture target is GL_TEXTURE_2D
	iattributes[++niattribs] = WGL_TEXTURE_TARGET_ARB;
	iattributes[++niattribs] = WGL_TEXTURE_2D_ARB;
	// ask to allocate room for the mipmaps
	iattributes[++niattribs] = WGL_MIPMAP_TEXTURE_ARB;
	iattributes[++niattribs] = TRUE;
	// ask to allocate the largest pbuffer it can, if it is
	// unable to allocate for the width and height
	iattributes[++niattribs] = WGL_PBUFFER_LARGEST_ARB;
	iattributes[++niattribs] = FALSE;
	// Create the p-buffer.
	pbuffer->hpbuffer = wglCreatePbufferARB( hdc, format, width, height, iattributes );
	if ( pbuffer->hpbuffer == 0)
	{
		Error("pbuffer creation error:  wglCreatePbufferARB() failed\n");
		wglGetLastError();
		finalize();
		return false;
	}
	wglGetLastError();

	// Get the device context.
	pbuffer->hdc = wglGetPbufferDCARB( pbuffer->hpbuffer );
	if ( pbuffer->hdc == 0)
	{
		Error("pbuffer creation error:  wglGetPbufferDCARB() failed\n");
		wglGetLastError();
		finalize();
		return false;
	}
	wglGetLastError();

	// Create a gl context for the p-buffer.
	pbuffer->hglrc = wglCreateContext( pbuffer->hdc );
	if ( pbuffer->hglrc == 0)
	{
		Error("pbuffer creation error:  wglCreateContext() failed\n");
		wglGetLastError();
		finalize();
		return false;
	}
	wglGetLastError();

	// Determine the actual width and height we were able to create.
	wglQueryPbufferARB( pbuffer->hpbuffer, WGL_PBUFFER_WIDTH_ARB, &pbuffer->width );
	wglQueryPbufferARB( pbuffer->hpbuffer, WGL_PBUFFER_HEIGHT_ARB, &pbuffer->height );

	this->dc = pbuffer->hdc;
	this->rc = pbuffer->hglrc;

	m_previousDC = hdc;
	m_previousRC = hglrc;

	ITextureManager *pTextureManager = m_pRenderSystem->getTextureManager();
	//m_pTexture = pTextureManager->createEmptyTexture(FO_LINEAR,FO_LINEAR,FO_LINEAR);//>createEmptyTexture(width,height,PF_R8G8B8A8);//
	//m_pTexture = pTextureManager->createEmptyTexture(width,height,PF_R8G8B8A8);//
	if( colorBits == 32)
		m_pTexture = pTextureManager->createEmptyTexture(width,height,PF_R8G8B8A8,min,mag, mip, s, t);
	else
		m_pTexture = pTextureManager->createEmptyTexture(width,height,PF_R8G8B8,min, mag, mip, s,t);

	static uint id = 100;
	m_id = id++;

	return true;
}

void  pbuffer::finalize()
{
	safeRelease(m_pTexture);

	nv_pbuffer * pbuffer = &m_pbuffer;
	if ( pbuffer->hpbuffer )
	{
		// Check if we are currently rendering in the pbuffer
		if (wglGetCurrentContext() == pbuffer->hglrc )
			wglMakeCurrent(0,0);
		// delete the pbuffer context
		wglDeleteContext( pbuffer->hglrc );
		wglReleasePbufferDCARB( pbuffer->hpbuffer, pbuffer->hdc );
		wglDestroyPbufferARB( pbuffer->hpbuffer );
		pbuffer->hpbuffer = 0;
	}
}

const ITexture*	pbuffer::getTexture()
{
	return m_pTexture;
}

void  pbuffer::release()
{
	finalize();
	delete this;
}

void 		pbuffer::onAttach()
{
	glBindTexture(GL_TEXTURE_2D,static_cast<Texture*>(m_pTexture)->getGLTextureID());
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

	// release the pbuffer from the render texture object
	if (wglReleaseTexImageARB(m_pbuffer.hpbuffer, WGL_FRONT_LEFT_ARB) == FALSE)
		wglGetLastError();
}

void 		pbuffer::onDetach()
{
	// bind the pbuffer to the render texture object
	if (wglBindTexImageARB(m_pbuffer.hpbuffer, WGL_FRONT_LEFT_ARB) == FALSE)
		wglGetLastError();
}

void 		pbuffer::onEndFrame()
{
}

bool		pbuffer::getTextureData(void  *pData)
{
	glBindTexture(GL_TEXTURE_2D,static_cast<Texture*>(m_pTexture)->getGLTextureID());
	GLint width,height;
	glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&height);
	GLint format;
	glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_INTERNAL_FORMAT,&format);
	GLint r,g,b,a;
	glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_RED_SIZE,&r);
	glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_GREEN_SIZE,&g);
	glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_BLUE_SIZE,&b);
	glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_ALPHA_SIZE,&a);
	glGetTexImage( GL_TEXTURE_2D,0,GL_RGB,GL_UNSIGNED_BYTE,pData);
	glBindTexture(GL_TEXTURE_2D,0);

	return true;
}

void		pbuffer::getRect(RECT *rc)
{
	rc->left = 0;
	rc->top = 0;
	rc->right = m_pbuffer.width;
	rc->bottom = m_pbuffer.height;
}