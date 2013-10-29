#ifndef __pbuffer_h__
#define __pbuffer_h__

#include "RenderTarget.h"

class pbuffer : public RenderTarget
{
public:
	bool initialize(int width,int height,int colorBits,int depthBits,
		int stencilBits,HDC hdc,HGLRC hglrc,IRenderSystem* pRenderSystem,
		FilterOptions min, FilterOptions mag, FilterOptions mip,TextureAddressingMode s,TextureAddressingMode t);
	void  finalize();
private:
	// Define a PBuffer object.
	typedef struct _pbuffer
	{
		HPBUFFERARB  hpbuffer;      // void* to a pbuffer.
		HDC          hdc;           // void* to a device context.
		HGLRC        hglrc;         // void* to a GL rendering context.
		int          width;         // Width of the pbuffer
		int          height;        // Height of the pbuffer
	} nv_pbuffer;
private:
	nv_pbuffer		m_pbuffer;
	IRenderSystem*	m_pRenderSystem;
	ITexture*		m_pTexture;
	HDC				m_previousDC;
	HGLRC			m_previousRC;
public:
	uint		m_id;
public:
	pbuffer() : m_id(0),m_pTexture(0),m_pRenderSystem(0),m_previousDC(0),m_previousRC(0){}
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