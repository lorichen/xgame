#ifndef __VFWAVIOBJECTD3D9_H__
#define __VFWAVIOBJECTD3D9_H__
#include "IVideoObject.h"
#include "vfw.h"	// Header File For Video For Windows

namespace xs
{

	class VFWAVIObjectD3D9 : public IVideoObject
	{
		//<<interface>>
	public:	
		virtual ITexture *	getTexture( uint frame);
		virtual uint		getFrameCount();
		virtual void		release();
		virtual float		getFrameInterval();

	public:
		VFWAVIObjectD3D9();
		~VFWAVIObjectD3D9();
		bool create(IRenderSystem * pRenderSystem, const std::string filename, uint imgWidth, uint imgHeight);
	private:
		AVISTREAMINFO		m_AVIStreamInfo;					// Pointer To A Structure Containing Stream Info
		PAVISTREAM			m_pAVIStream;						// Handle To An Open Stream
		PGETFRAME			m_pAVIPGetFrame;				  // Pointer To A GetFrame Object
		uint				m_uiCurrentFrame;
		uint				m_uiFrameCount;
		float				m_fFrameInterval;	
		ITexture *			m_pTexture;	

		//dib工具，用于缩放
		HDRAWDIB m_hDD;												// Handle For Our Dib
		HBITMAP m_hBmp;												// Handle To A Device Dependant Bitmap
		HDC m_hDC;													// Memory DC
		uchar	* m_imageDataPtr;							//image data pointer
	};
}

#endif