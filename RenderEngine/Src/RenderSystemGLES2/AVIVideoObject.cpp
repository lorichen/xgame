#include "StdAfx.h"
#include "AVIVideoObject.h"

namespace xs
{
	AVIVideoObject_Win32::AVIVideoObject_Win32()
	{
		memset(&m_AVIStreamInfo, 0, sizeof( m_AVIStreamInfo) );
		m_pAVIStream = 0;
		m_pAVIPGetFrame = 0;
		m_uiFrameCount = 0;
		m_fFrameInterval = 0;
		m_uiCurrentFrame = -1;

		m_hDD = NULL;
		m_hBmp = NULL;
		m_hDC = NULL;
		m_imageDataPtr = 0;

		m_pTexture = 0;
	}

	AVIVideoObject_Win32::~AVIVideoObject_Win32()
	{
		
	}

	void AVIVideoObject_Win32::release()
	{
		if( NULL != m_hDC)
		{
			::DeleteDC(m_hDC); //delete compatitable dc object
			m_hDC = NULL;
		}

		if( NULL != m_hBmp ) // Delete The Device Dependant Bitmap Object
		{
			DeleteObject(m_hBmp);
			m_hBmp = NULL;
		}

		if( NULL != m_hDD) // Closes The DrawDib Device Context
		{
			DrawDibClose(m_hDD);
			m_hDD = NULL;
		}

		if(NULL != m_pAVIStream)
		{
			if( m_pAVIPGetFrame ) // Deallocates The GetFrame Resources
			{
				AVIStreamGetFrameClose(m_pAVIPGetFrame);
				m_pAVIPGetFrame = NULL;
			}
			AVIStreamRelease(m_pAVIStream);// Release The Stream
			m_pAVIStream = NULL;
			AVIFileExit();// Release The File
		}	

		if( NULL != m_pTexture)
		{
			m_pTexture->release();
			m_pTexture = NULL;
		}

		delete this;

		return;
	}

	uint AVIVideoObject_Win32::getFrameCount()
	{
		return m_uiFrameCount;
	}

	float AVIVideoObject_Win32::getFrameInterval()
	{
		return m_fFrameInterval;
	}

	ITexture * AVIVideoObject_Win32::getTexture( uint frame)
	{
		if( frame > m_uiFrameCount) return 0;
		if( frame == m_uiCurrentFrame) return m_pTexture;
		m_uiCurrentFrame = frame;

		//更新纹理
		if( NULL == m_pTexture) return 0;

		LPBITMAPINFOHEADER lpbi;									// Holds The Bitmap Header Information
		lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(m_pAVIPGetFrame, frame);	// Grab Data From The AVI Stream
		char *pdata=(char *)lpbi+lpbi->biSize+lpbi->biClrUsed * sizeof(RGBQUAD);	// Pointer To Data Returned By AVIStreamGetFrame

		// Convert Data To Requested Bitmap Format
		DrawDibDraw (m_hDD, m_hDC, 0, 0,
			m_pTexture->width(), m_pTexture->height(), 
			lpbi, pdata, 0, 0,
			m_AVIStreamInfo.rcFrame.right - m_AVIStreamInfo.rcFrame.left,
			m_AVIStreamInfo.rcFrame.bottom - m_AVIStreamInfo.rcFrame.top,
			0);
		flipIt(m_imageDataPtr);

		m_pTexture->setSubData(0, 0,0, m_pTexture->width(), m_pTexture->height(),m_imageDataPtr);
		return m_pTexture;
	}

	bool AVIVideoObject_Win32::create(IRenderSystem * pRenderSystem, const std::string filename , uint imgWidth, uint imgHeight)
	{
		//检测
		if( NULL == pRenderSystem)
			return false;

		if( imgWidth == 0 || imgWidth > 1024
			|| imgHeight == 0 || imgHeight > 768 )
			return false;

		if( !( m_hDD = DrawDibOpen() ) )
			return false;

		if( !( m_hDC = ::CreateCompatibleDC(0) ) )
			return false;

		AVIFileInit();//初始化库
		// Opens The AVI Stream
		if (AVIStreamOpenFromFileA(&m_pAVIStream, (LPCTSTR)(filename.c_str()), streamtypeVIDEO, 0, OF_READ, NULL) !=0) 
			return false;
		AVIStreamInfo(m_pAVIStream, &m_AVIStreamInfo, sizeof(m_AVIStreamInfo));					// Reads Information About The Stream Into psi									
		m_uiFrameCount = AVIStreamLength(m_pAVIStream);	
		m_fFrameInterval = static_cast<float>( AVIStreamSampleToTime(m_pAVIStream, m_uiFrameCount) ) / static_cast<float>(m_uiFrameCount);
		m_pAVIPGetFrame=AVIStreamGetFrameOpen(m_pAVIStream, NULL);	// Create The PGETFRAME	Using Our Request Mode
		if( NULL == m_pAVIPGetFrame)
			return false;

		//位图
		BITMAPINFOHEADER bmih;
		memset(&bmih,0, sizeof(bmih));
		bmih.biSize = sizeof (BITMAPINFOHEADER);													// Size Of The BitmapInfoHeader
		bmih.biPlanes = 1;																			// Bitplanes	
		bmih.biBitCount = 24;																		// Bits Format
		bmih.biWidth = imgWidth;																	// Width
		bmih.biHeight = imgHeight;																	// Height
		bmih.biCompression = BI_RGB;																// Requested Mode = RGB
		m_hBmp = CreateDIBSection (m_hDC, (BITMAPINFO*)(&bmih), DIB_RGB_COLORS, (void**)(&m_imageDataPtr), NULL, NULL);
		if( m_hBmp == NULL)
			return false;	
		SelectObject (m_hDC, m_hBmp);								// Select hBitmap Into Our Device Context (hdc)

		//创建纹理
		m_pTexture = pRenderSystem->getTextureManager()->createEmptyTexture(imgWidth, imgHeight, PF_R8G8B8);
		if( NULL == m_pTexture)
			return false;

		return true;
	}


	void AVIVideoObject_Win32::flipIt(void* buffer)					// Flips The Red And Blue Bytes
	{
		void* b = buffer;											// Pointer To The Buffer
		int memdim =m_pTexture->width() *m_pTexture->height();
		__asm														// Assembler Code To Follow
		{
			mov ecx, memdim 								// Counter Set To Dimensions Of Our Memory Block
			mov ebx, b												// Points ebx To Our Data (b)
			label:													// Label Used For Looping
				mov al,[ebx+0]										// Loads Value At ebx Into al
				mov ah,[ebx+2]										// Loads Value At ebx+2 Into ah
				mov [ebx+2],al										// Stores Value In al At ebx+2
				mov [ebx+0],ah										// Stores Value In ah At ebx
				
				add ebx,3											// Moves Through The Data By 3 Bytes
				dec ecx												// Decreases Our Loop Counter
				jnz label											// If Not Zero Jump Back To Label
		}
	}
}
