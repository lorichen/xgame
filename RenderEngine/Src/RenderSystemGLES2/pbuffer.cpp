#include "StdAfx.h"
#include "pbuffer.h"
#include "Texture.h"
#include "RenderSystem.h"

#define MAX_ATTRIBS     256
#define MAX_PFORMATS    256
#define TEX_SIZE        256


bool pbuffer::initialize(int width,int height,int colorBits,int depthBits,
		int stencilBits,HDC hdc,EGLContext hglrc,IRenderSystem* pRenderSystem,
		FilterOptions min, FilterOptions mag, FilterOptions mip,TextureAddressingMode s,TextureAddressingMode t)
{
	m_desc.width = width;
	m_desc.height = height;
	m_desc.colorBits = colorBits;
	m_desc.depthBits = depthBits;
	m_desc.stencilBits = stencilBits;
	m_desc.min = min;
	m_desc.mag = mag;
	m_desc.mip = mip;
	m_desc.s = s;
	m_desc.t = t;

	GLint nOriFbo = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &nOriFbo);
	ITextureManager *pTextureManager = m_pRenderSystem->getTextureManager();
	if( m_desc.colorBits == 32)
		m_pTexture = pTextureManager->createEmptyTexture(m_desc.width,m_desc.height,PF_R8G8B8A8,m_desc.min,m_desc.mag, m_desc.mip, m_desc.s, m_desc.t);
	else
		m_pTexture = pTextureManager->createEmptyTexture(m_desc.width,m_desc.height,PF_R8G8B8,m_desc.min, m_desc.mag, m_desc.mip, m_desc.s,m_desc.t);

	if(!m_pTexture)
	{
		return false;
	}

	// Create the object that will allow us to render to the aforementioned texture
	glGenFramebuffers(1, &m_FrameBufferObj);
	TestGLError("pbuffer::initialize | glGenFramebuffers");
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferObj);
	TestGLError("pbuffer::initialize  | glBindFramebuffer");

	// Attach the texture to the FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER
		, GL_COLOR_ATTACHMENT0
		, GL_TEXTURE_2D
		, (static_cast<Texture*>(m_pTexture))->getGLTextureID()
		, 0);

	TestGLError("pbuffer::initialize  | glBindFramebuffer");

	if (m_desc.depthBits > 0)
	{
		glGenRenderbuffers(1, &m_pDepthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_pDepthBuffer);

		TestGLError("pbuffer::initialize  | glBindRenderbuffer");

		glRenderbufferStorage(GL_RENDERBUFFER
			, GL_DEPTH_COMPONENT16
			, m_desc.width, m_desc.height);
	
		TestGLError("pbuffer::initialize  | glRenderbufferStorage");

		glGenRenderbuffers(1, &m_pStencilBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_pStencilBuffer);

		TestGLError("pbuffer::initialize  | glBindRenderbuffer");

		glRenderbufferStorage(GL_RENDERBUFFER
			, GL_STENCIL_INDEX8
			, m_desc.width, m_desc.height);

		TestGLError("pbuffer::initialize  | glRenderbufferStorage");

		// Attach the depth buffer we created earlier to our FBO.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER
			, GL_DEPTH_ATTACHMENT
			, GL_RENDERBUFFER
			, m_pDepthBuffer);

		TestGLError("pbuffer::initialize  | glFramebufferRenderbuffer");

		/*
		// Attach the stencil buffer we created earlier to our FBO.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER
		    , GL_STENCIL_ATTACHMENT
		    , GL_RENDERBUFFER
		    , m_pStencilBuffer);

		TestGLError("pbuffer::initialize  | glFramebufferRenderbuffer");
		*/
	}

	// Check that our FBO creation was successful
	GLuint uStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(uStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		finalize();

		glBindFramebuffer(GL_FRAMEBUFFER, nOriFbo);
		return false;
	}

	glClearColor(1.0,0.0,0.0,1.0);
	// Clear the colour and depth buffers for the FBO / PBuffer surface
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT /*| GL_STENCIL_BUFFER_BIT*/);

	// Unbind the frame buffer object so rendering returns back to the backbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, nOriFbo);


	static uint id = 100;
	m_id = id++;

	return true;
}

void  pbuffer::finalize()
{
	if(m_pTexture)
	{
		ITextureManager *pTextureManager = m_pRenderSystem->getTextureManager();
		pTextureManager->releaseTexture(m_pTexture);
		m_pTexture = 0;
	}

	glDeleteFramebuffers(1, &m_FrameBufferObj);
	glDeleteRenderbuffers(1, &m_pDepthBuffer);
	glDeleteRenderbuffers(1, &m_pStencilBuffer);
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
	glBindFramebuffer(GL_FRAMEBUFFER,m_FrameBufferObj);
	TestGLError("pbuffer::onAttach | glBindFramebuffer");
}

void 		pbuffer::onDetach()
{
	
}

void 		pbuffer::onEndFrame()
{
}

bool		pbuffer::getTextureData(void  *pData)
{
	assert(0);
	return false;
}

void		pbuffer::getRect(RECT *rc)
{
	rc->left = 0;
	rc->top = 0;
	rc->right = m_desc.width;
	rc->bottom = m_desc.height;
}