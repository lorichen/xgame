#ifndef __pbuffer_h__
#define __pbuffer_h__

#include "RenderTarget.h"

class pbuffer : public RenderTarget
{
public:

	
	void  finalize();
private:
	
private:
	RenderTargetDesc m_desc;
	
	ITexture*		 m_pTexture;
    
#if (TARGET_PLATFORM == PLATFORM_WIN32)
	HDC				 m_previousDC;
	EGLContext		 m_previousRC;
#else
    
#endif
    
	GLuint			 m_pDepthBuffer;
	GLuint			 m_pStencilBuffer;
public:
	uint			 m_id;
public:

	pbuffer(RenderSystem* pRenderSystem);
    
    bool initialize(int width,int height,int colorBits,int depthBits,
                    int stencilBits,IRenderSystem* pRenderSystem,
                    FilterOptions min, FilterOptions mag, FilterOptions mip,TextureAddressingMode s,TextureAddressingMode t);

    
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