#ifndef __AVIVIDEOOBJECT_H__
#define __AVIVIDEOOBJECT_H__

#include "IVideoObject.h"
#include "vfw.h"	// Header File For Video For Windows

namespace xs
{
	class AVIVideoObject_Win32: public IVideoObject
	{
		//<<interface>>
	public:
		virtual ITexture *	getTexture( uint frame);
		virtual uint		getFrameCount();
		virtual void		release();
		virtual float		getFrameInterval();

	private:
		AVISTREAMINFO		m_AVIStreamInfo;					// Pointer To A Structure Containing Stream Info
		PAVISTREAM			m_pAVIStream;						// Handle To An Open Stream
		PGETFRAME			m_pAVIPGetFrame;					// Pointer To A GetFrame Object
		uint				m_uiFrameCount;						// 帧数
		float				m_fFrameInterval;					// 帧间隔
		uint				m_uiCurrentFrame;					// 当前帧

		HDRAWDIB m_hDD;												// Handle For Our Dib
		HBITMAP m_hBmp;												// Handle To A Device Dependant Bitmap
		HDC m_hDC;													// Memory DC
		unsigned char	* m_imageDataPtr;							//image data pointer

		ITexture *			m_pTexture;

	public:
		AVIVideoObject_Win32();
		bool create(IRenderSystem * pRenderSystem, const std::string filename, uint imgWidth, uint imgHeight);
		~AVIVideoObject_Win32();
	private:
		void flipIt(void* buffer);									// Flips The Red And Blue Bytes (256x256)
	};
}
#endif