#ifndef __pbuffer_h__
#define __pbuffer_h__

#include "RenderTarget.h"

class pbuffer : public RenderTarget
{
public:

	bool initialize(int width,int height,int colorBits,int depthBits,
		int stencilBits,HDC hdc,EGLContext hglrc,IRenderSystem* pRenderSystem,
		FilterOptions min, FilterOptions mag, FilterOptions mip,TextureAddressingMode s,TextureAddressingMode t);
	void  finalize();
private:
	
private:
	RenderTargetDesc m_desc;
	
	ITexture*		 m_pTexture;
	HDC				 m_previousDC;
	EGLContext		 m_previousRC;

	GLuint			 m_pDepthBuffer;
	GLuint			 m_pStencilBuffer;
public:
	uint			 m_id;
public:
	pbuffer(RenderSystem* pRenderSystem) : RenderTarget(pRenderSystem),m_id(0),m_pTexture(0),m_previousDC(0),m_previousRC(0){}
public:
	virtual void 		onDetach();
	virtual void 		onAttach();
	virtual const ITexture*	getTexture();
	virtual void		release();
	virtual void 		onEndFrame();
	virtual bool		getTextureData(void  *pData);
	virtual void		getRect(RECT *rc);
};

#endif