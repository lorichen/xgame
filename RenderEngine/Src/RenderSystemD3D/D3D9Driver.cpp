#include "StdAfx.h"
#include "D3D9Driver.h"

#include "RenderSystemD3D9.h"

namespace xs
{
	D3D9Driver * D3D9DriverCreater::create(RenderSystemD3D9 * pRenderSystem, uint uiAdapterOrdinal)
	{
		if( NULL == pRenderSystem )
			return NULL;

		D3D9Driver * pD3D9Driver = new D3D9Driver;
		if( !pD3D9Driver->create(pRenderSystem, uiAdapterOrdinal) )
		{
			pD3D9Driver->release();
			return NULL;
		}
		else
		{
			return pD3D9Driver;
		}
	}

	bool D3D9Driver::create(RenderSystemD3D9 * pRenderSystem,uint uiAdapterOrdinal)
	{
		if( NULL == pRenderSystem)
			return false;
		m_pRenderSystem = pRenderSystem;
		m_uiAdapterOrdinal = uiAdapterOrdinal;

		IDirect3D9 * pD3D9 = m_pRenderSystem->getDirect3D9();
		if( NULL == pD3D9)
			return false;

		HRESULT hr = pD3D9->GetAdapterIdentifier( m_uiAdapterOrdinal, 0, &m_AdapterIdentifier );
		if(FAILED(hr))
		{
			TraceLn("GetAdapterIdentifier failed!");
			return false;
		}

		hr = pD3D9->GetAdapterDisplayMode( m_uiAdapterOrdinal, &m_DesktopDisplayMode );
		if(FAILED(hr))
		{
			TraceLn("GetAdapterDisplayMode failed!");
			return false;
		}

		enumerateDisplayMode();

		return true;
	}


	D3D9Driver::D3D9Driver():
	m_uiAdapterOrdinal(0)
	{
		ZeroMemory(&m_AdapterIdentifier, sizeof(m_AdapterIdentifier));
		ZeroMemory(&m_DesktopDisplayMode, sizeof(m_DesktopDisplayMode));
	}
	
	void  D3D9Driver::release()
	{
		//需要释放某些资源

		delete this;
	}

	void D3D9Driver::onDeviceLost()
	{
		//重新获取当前分辨率
		m_pRenderSystem->getDirect3D9()->GetAdapterDisplayMode(m_uiAdapterOrdinal, &m_DesktopDisplayMode);	
	}

	void D3D9Driver::onDeviceReset()
	{
		//nothing to do
	}

	bool D3D9Driver::getDisplayMode(UINT width, UINT height, D3DFORMAT format, D3DDISPLAYMODE & mode)
	{
		std::vector<D3DDISPLAYMODE>::iterator it = m_displayModeList.begin();
		for( ; it != m_displayModeList.end(); ++it)
		{
			if( it->Format == format && it->Width == width && it->Height == height )
			{
				mode = *it;
				return true;
			}
		}
		return false;
	}

	void  D3D9Driver::enumerateDisplayMode()
	{
		IDirect3D9 * pD3D9 = m_pRenderSystem->getDirect3D9();
		D3DFORMAT formats[3] = { D3DFMT_X8R8G8B8, D3DFMT_X1R5G5B5, D3DFMT_R5G6B5, };

		for( uint i = 0; i < 3; ++i)
		{
			UINT count =  pD3D9->GetAdapterModeCount( m_uiAdapterOrdinal, formats[i]);
			for( UINT j =0; j < count; ++j )
			{
				D3DDISPLAYMODE mode;
				pD3D9->EnumAdapterModes(m_uiAdapterOrdinal, formats[i], j, &mode);
				if( mode.Width < 640 || mode.Height < 480 ) continue;
				bool found = false;
				for( uint k = 0; k < m_displayModeList.size(); ++k )
				{
					if( m_displayModeList[k].Format == mode.Format 
						&& m_displayModeList[k].Width == mode.Width
						&& m_displayModeList[k].Height == mode.Height)
					{
						found = true;
						if( m_displayModeList[k].RefreshRate < mode.RefreshRate ) 
							m_displayModeList[k].RefreshRate = mode.RefreshRate;
						break;
					}

				}
				if( !found )
				{
					m_displayModeList.push_back( mode);
				}
			}
		}	
	}

}
