#include "StdAfx.h"
#include "VFWAVIObjectD3D9.h"
#include "TextureD3D9.h"
#include "TexturemanagerD3D9.h"


namespace xs
{
	VFWAVIObjectD3D9::VFWAVIObjectD3D9()
	{
		memset(&m_AVIStreamInfo, 0, sizeof( m_AVIStreamInfo) );
		m_pAVIStream = 0;
		m_pAVIPGetFrame = 0;

		m_pTexture = 0;
		m_uiCurrentFrame = -1;
	    m_uiFrameCount = 0;
		m_fFrameInterval = 0;

		m_hDD = NULL;
		m_hBmp = NULL;
		m_hDC = NULL;
		m_imageDataPtr = 0;
	}

	VFWAVIObjectD3D9::~VFWAVIObjectD3D9()
	{
		//nothing to do
	}

	void VFWAVIObjectD3D9::release()
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


		if( NULL !=  m_pAVIStream)
		{
			if( NULL != m_pAVIPGetFrame ) // Deallocates The GetFrame Resources
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
	}

	uint VFWAVIObjectD3D9::getFrameCount() 
	{
		return m_uiFrameCount;
	}

	float VFWAVIObjectD3D9::getFrameInterval()
	{
		return m_fFrameInterval;
	}


	ITexture * VFWAVIObjectD3D9::getTexture( uint frame)
	{
		if( frame > m_uiFrameCount) return 0;
		if( frame == m_uiCurrentFrame) return m_pTexture;
		if( NULL == m_pTexture) return 0;
		m_uiCurrentFrame = frame;

		//更新纹理
		if( NULL == m_pTexture) return 0;
		LPBITMAPINFOHEADER lpbi;									// Holds The Bitmap Header Information
		lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(m_pAVIPGetFrame, frame);	// Grab Data From The AVI Stream
		uchar *pdata=(uchar *)lpbi+lpbi->biSize+lpbi->biClrUsed * sizeof(RGBQUAD);	// Pointer To Data Returned By AVIStreamGetFrame

		uchar * pimgdata = 0;
		if( ( m_pTexture->width() != m_AVIStreamInfo.rcFrame.right - m_AVIStreamInfo.rcFrame.left )
			&& ( m_pTexture->height() != m_AVIStreamInfo.rcFrame.bottom - m_AVIStreamInfo.rcFrame.top ) )
		{
			//缩放
			DrawDibDraw (m_hDD, m_hDC, 0, 0,
				m_pTexture->width(), m_pTexture->height(), 
				lpbi, pdata, 0, 0,
				m_AVIStreamInfo.rcFrame.right - m_AVIStreamInfo.rcFrame.left,
				m_AVIStreamInfo.rcFrame.bottom - m_AVIStreamInfo.rcFrame.top,
				0);
			pimgdata = m_imageDataPtr;
		}
		else
		{
			pimgdata = pdata;
		}

		TextureD3D9 * pD3DTex = static_cast<TextureD3D9 *>(m_pTexture);	
		if( NULL == pD3DTex->getD3DTexture()) return 0;
		D3DSURFACE_DESC surfdesc;
		pD3DTex->getD3DTexture()->GetLevelDesc(0, &surfdesc);
		D3DLOCKED_RECT lockedrect;
		if( !pD3DTex->lock(0, lockedrect, D3DLOCK_DISCARD) ) return 0;
		if( surfdesc.Format == D3DFMT_R8G8B8)
		{
			memcpy( lockedrect.pBits, m_imageDataPtr, pD3DTex->width()* pD3DTex->height() * 3);
		}
		else if( surfdesc.Format == D3DFMT_X8R8G8B8)
		{		
			//汇编写效率更高??
			uchar * ptexdata = (uchar*)(lockedrect.pBits);
			uint width = pD3DTex->width();
			uint height = pD3DTex->height();
			for( uint i = 0; i < height; ++i)
			{
				for( uint j = 0; j< width; ++j)
				{
					ptexdata[0] =  pimgdata[0];
					ptexdata[1] =  pimgdata[1];
					ptexdata[2] =  pimgdata[2];
					ptexdata += 4;
					pimgdata += 3;
				}
				ptexdata += lockedrect.Pitch - width * 4;
			}
		}
		else
		{
			//还有其它的格式?
		}
		pD3DTex->unlock();
		
		return m_pTexture;
	}

	bool VFWAVIObjectD3D9::create(IRenderSystem * pRenderSystem, const std::string filename, uint imgWidth, uint imgHeight)
	{
		//检测
		if( imgWidth == 0 || imgWidth > 1024
			|| imgHeight == 0 || imgHeight > 768 )
			return false;

		if( NULL == pRenderSystem)
			return false;

		//初始化dib库
		if( !( m_hDD = DrawDibOpen() ) )
			return false;

		if( !( m_hDC = ::CreateCompatibleDC(0) ) )
			return false;

		//初始化AVI库
		AVIFileInit();
		if (AVIStreamOpenFromFileA(&m_pAVIStream, (LPCTSTR)(filename.c_str()), streamtypeVIDEO, 0, OF_READ, NULL) !=0) 
			return false;
		AVIStreamInfo(m_pAVIStream, &m_AVIStreamInfo, sizeof(m_AVIStreamInfo));			// Reads Information About The Stream Into psi									
		m_uiFrameCount = AVIStreamLength(m_pAVIStream);									// The Last Frame Of The Stream
		m_fFrameInterval = static_cast<float>( AVIStreamSampleToTime(m_pAVIStream, m_uiFrameCount) ) / static_cast<float>(m_uiFrameCount);
		m_pAVIPGetFrame=AVIStreamGetFrameOpen(m_pAVIStream, NULL);						// Create The PGETFRAME	Using Our Request Mode
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

		//创建空的纹理
		ITextureManager * pTexMgr = pRenderSystem->getTextureManager();
		TextureManagerD3D9 * pD3DTexMgr = static_cast<TextureManagerD3D9*>(pTexMgr);
		m_pTexture = pD3DTexMgr->createEmptyeTexture(D3DPOOL_DEFAULT, D3DUSAGE_DYNAMIC, imgWidth, imgHeight, PF_R8G8B8);
		if(NULL == m_pTexture)
			return false;

		return true;
	}
}