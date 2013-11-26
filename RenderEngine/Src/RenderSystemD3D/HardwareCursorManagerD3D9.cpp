#include "StdAfx.h"
#include "HardwareCursorManagerD3D9.h"
#include "RenderSystemD3D9.h"
#include "D3D9Device.h"
#include "TextureD3D9.h"
#include "TextureManagerD3D9.h"

namespace xs
{
	HardwareCursorManagerD3D9 * HardwareCursorManagerD3D9Creater::create( RenderSystemD3D9 * pRenderSystem)
	{
		HardwareCursorManagerD3D9 * pMgr = new HardwareCursorManagerD3D9();
		if( pMgr->create( pRenderSystem ) )
		{
			return pMgr;
		}
		else
		{
			pMgr->release();
			return 0;
		}
	}

	HardwareCursorManagerD3D9::HardwareCursorManagerD3D9():
		m_pRenderSystem(0),
		m_pD3D9device(0),
		m_pCurTexture(0),
		m_uiXHotspot(0),
		m_uiYHotspot(0),
		m_pDefaultTexture(0)
	{
	}

	bool HardwareCursorManagerD3D9::create(RenderSystemD3D9 * pRenderSystem)
	{
		if( 0 == pRenderSystem)
			return false;

		m_pRenderSystem = pRenderSystem;
		m_pD3D9device = m_pRenderSystem->getD3D9Device()->getDevice();

		//构建默认纹理
		uchar buff[32][32][4];
		memset(&buff[0][0][0], 0, sizeof(buff));
		m_pDefaultTexture = m_pRenderSystem->getTextureManager()->createTextureFromRawData(
			&buff[0][0][0],32, 32, PF_B8G8R8A8, FO_POINT, FO_POINT, FO_NONE, TAM_CLAMP, TAM_CLAMP);
		if(  0 == m_pDefaultTexture )
			return false;

		return true;
	}

	void HardwareCursorManagerD3D9::release()
	{
		safeRelease(m_pDefaultTexture);

		delete this;
	}

	void HardwareCursorManagerD3D9::showCursor( bool show)
	{
		HRESULT hr = m_pD3D9device->ShowCursor( show ? TRUE : FALSE);
	}

	void HardwareCursorManagerD3D9::setCursorPosition( int x, int y, bool updateImmediate)
	{
		m_pD3D9device->SetCursorPosition( x, y, updateImmediate ? D3DCURSOR_IMMEDIATE_UPDATE: 0 );
	}

	void HardwareCursorManagerD3D9::setDefaultCursor()
	{
		IDirect3DTexture9* pTex = static_cast<TextureD3D9*>(m_pDefaultTexture)->getD3DTexture();
		IDirect3DSurface9* pSurf = 0;
		HRESULT hr = pTex->GetSurfaceLevel(0, &pSurf);
		if( FAILED(hr) )
			return;

		hr = m_pD3D9device->SetCursorProperties(m_uiXHotspot, m_uiYHotspot, pSurf);

		COM_SAFE_RELEASE(pSurf);
		return;
	}

	void HardwareCursorManagerD3D9::setCursorProperties()
	{
		if( 0 == m_pCurTexture )
		{
			setDefaultCursor();
			return;
		}

		IDirect3DTexture9* pTex = static_cast<TextureD3D9*>(m_pCurTexture)->getD3DTexture();
		if( 0 == pTex )
		{
			setDefaultCursor();
			return;
		}

		IDirect3DSurface9* pSurf = 0;
		HRESULT hr = pTex->GetSurfaceLevel(0, &pSurf);
		if( SUCCEEDED(hr))
		{
			HRESULT hr = m_pD3D9device->SetCursorProperties(m_uiXHotspot, m_uiYHotspot, pSurf);
			COM_SAFE_RELEASE(pSurf);
			return;
		}
		else
		{
			setDefaultCursor();
			return;
		}
	}

	void HardwareCursorManagerD3D9::setCursorHotspot(uint x, uint y)
	{
		m_uiXHotspot = x;
		m_uiYHotspot = y;
		setCursorProperties();
	}


	void	HardwareCursorManagerD3D9::setCursorImage(ITexture* pTexture)
	{
		m_pCurTexture = pTexture;
		setCursorProperties();
	}

	bool	HardwareCursorManagerD3D9::isAvailable()
	{
		return true;
	}


	void HardwareCursorManagerD3D9::notifyOnDeviceLost()
	{
		// nothing to do
	}
	void HardwareCursorManagerD3D9::notifyOnDeviceReset()
	{
		setCursorProperties();
	}

}