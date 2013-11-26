#include "StdAfx.h"
#include "RenderTargetD3D9Window.h"
#include "RenderSystemD3D9.h"
#include "D3D9Device.h"
#include "MappingsD3D9.h"
#include "D3D9ResourceManager.h"
namespace xs
{

	RenderTargetD3D9Window::RenderTargetD3D9Window(RenderSystemD3D9 * pRenderSystem, bool b2d, RenderTargetManagerD3D9 * pRenderTargetMgr):
	RenderTargetD3D9(pRenderSystem, b2d,pRenderTargetMgr),
	m_pSwapChain(0),
	m_pDepthStencilBuffer(0),
	m_hWnd(0),
	m_bIsDefault(false)
	{
		ZeroMemory(&m_PresentParam, sizeof(m_PresentParam));
	}


	RenderTargetD3D9Window::~RenderTargetD3D9Window()
	{

	}
	

	void RenderTargetD3D9Window::release()
	{
		COM_SAFE_RELEASE(m_pSwapChain);
		COM_SAFE_RELEASE(m_pDepthStencilBuffer);
		delete this;
	}

	const ITexture*	RenderTargetD3D9Window::getTexture()
	{
		return 0;
	}

	bool RenderTargetD3D9Window::getTextureData(void * pData)
	{
		return false;
	}

	void RenderTargetD3D9Window::onAttach(RenderTargetD3D9 * pPrevious)
	{
		IDirect3DDevice9 * pD3D9Device =m_pRenderSystem->getD3D9Device()->getDevice();
		Assert(pD3D9Device);

		//设置当前的渲染目标和深度模版缓存
		IDirect3DSurface9 * pSurf = 0;
		m_pSwapChain->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO, &pSurf);
		pD3D9Device->SetRenderTarget(0, pSurf);
		pSurf->Release();
		pD3D9Device->SetDepthStencilSurface( m_pDepthStencilBuffer);
		
		//设置当前渲染目标的渲染状态
		RenderTargetD3D9::onAttach(pPrevious);
	}

	void 		RenderTargetD3D9Window::onDetach()
	{
		//需要做detach的事情
		//目前将深度模板缓存取消
		IDirect3DDevice9 * pD3D9Device = m_pRenderSystem->getD3D9Device()->getDevice();
		Assert(pD3D9Device);

		//取消深度模板缓存
		pD3D9Device->SetDepthStencilSurface(NULL);

	}

	void		RenderTargetD3D9Window::clearFrameBuffer(bool clearColorBuffer,bool clearDepthBuffer ,bool clearStencilBuffer)
	{
		IDirect3DDevice9 * pD3D9Device = m_pRenderSystem->getD3D9Device()->getDevice();
		Assert(pD3D9Device);

		DWORD clearFlags = 0;
		clearFlags |= clearColorBuffer ? D3DCLEAR_TARGET : 0;
		clearFlags |= clearDepthBuffer ? D3DCLEAR_ZBUFFER : 0;
		clearFlags |= clearStencilBuffer ? D3DCLEAR_STENCIL : 0;
		pD3D9Device->Clear(
			0,
			0,
			clearFlags,
			m_clearColor.getAsARGB(),
			m_clearDepth,
			(DWORD)m_clearStencil );
	}

	void		RenderTargetD3D9Window::onBeginFrame(bool clearColorBuffer,bool clearDepthBuffer,bool clearStencilBuffer)
	{
		IDirect3DDevice9 * pD3D9Device = m_pRenderSystem->getD3D9Device()->getDevice();
		Assert(pD3D9Device);

		m_pSwapChain->Present(NULL, NULL, NULL, NULL, 0);

		DWORD clearFlags = 0;
		clearFlags |= clearColorBuffer ? D3DCLEAR_TARGET : 0;
		clearFlags |= clearDepthBuffer ? D3DCLEAR_ZBUFFER : 0;
		clearFlags |= clearStencilBuffer ? D3DCLEAR_STENCIL : 0;
		pD3D9Device->Clear(
			0,
			0,
			clearFlags,
			m_clearColor.getAsARGB(),
			m_clearDepth,
			(DWORD)m_clearStencil );

		m_pRenderSystem->getD3D9ResourceMgr()->beginFrame();
		pD3D9Device->BeginScene();

	}


	void 		RenderTargetD3D9Window::onEndFrame(int layer)
	{
		//暂时是交换缓存
		IDirect3DDevice9 * pD3D9Device = m_pRenderSystem->getD3D9Device()->getDevice();
		Assert(pD3D9Device);

		pD3D9Device->EndScene();
		m_pRenderSystem->getD3D9ResourceMgr()->endFrame();
	}

	IDirect3DSurface9 * RenderTargetD3D9Window::getDepthStencilBuffer()
	{
		return m_pDepthStencilBuffer;
	}

	bool RenderTargetD3D9Window::isDefaultRenderTarget()
	{
		return m_bIsDefault;
	}

	void  RenderTargetD3D9Window::onDeviceLost()
	{
		COM_SAFE_RELEASE(m_pDepthStencilBuffer);
		COM_SAFE_RELEASE(m_pSwapChain);
	}

	RECT RenderTargetD3D9Window::getFrameBufferRect()
	{
		RECT rc;
		rc.left = rc.top  = 0;
		rc.right = m_PresentParam.BackBufferWidth;
		rc.bottom = m_PresentParam.BackBufferHeight;
		return rc;
	}

	bool RenderTargetD3D9Window::getDisplayMode(RenderEngineCreationParameters & createParam)
	{
		if( m_hWnd != (HWND)createParam.hwnd)
			return false;

		createParam.w = m_PresentParam.BackBufferWidth;
		createParam.h = m_PresentParam.BackBufferHeight;
		createParam.fullscreen = m_PresentParam.Windowed?false:true;
		createParam.colorDepth = 32;//待实现
		createParam.refreshRate = m_PresentParam.FullScreen_RefreshRateInHz;
		return true;
	}

	bool RenderTargetD3D9Window::switchDisplayMode(RenderEngineCreationParameters* param)
	{
		if( 0 == param )
			return false;

		if( m_hWnd != (HWND)param->hwnd)
			return false;


		if( m_bIsDefault )//默认的设备窗口
		{
			//nothing to do
			return true;
		}
		else//其它窗口
		{
			//释放资源
			COM_SAFE_RELEASE(m_pDepthStencilBuffer);
			COM_SAFE_RELEASE(m_pSwapChain);

			//创建资源
			return create(m_hWnd);
		}
	}


	void RenderTargetD3D9Window::onDeviceReset()
	{
		IDirect3DDevice9 * pD3D9Device =m_pRenderSystem->getD3D9Device()->getDevice();
		Assert( pD3D9Device );
		if( m_bIsDefault )//默认渲染目标
		{
			pD3D9Device->GetSwapChain(0,&m_pSwapChain);
			m_pSwapChain->GetPresentParameters( &m_PresentParam);
			m_hWnd =m_PresentParam.hDeviceWindow;
			pD3D9Device->GetDepthStencilSurface( &m_pDepthStencilBuffer);	
			//pD3D9Device->SetDialogBoxMode(TRUE);
		}
		else
		{
			create(m_hWnd);
		}

		//设置视口参数
		m_viewport.X = 0;
		m_viewport.Y = 0;
		m_viewport.Width = m_PresentParam.BackBufferWidth;
		m_viewport.Height = m_PresentParam.BackBufferHeight;
	}

	bool RenderTargetD3D9Window::createAsDefaultRenderTarget()
	{		
		IDirect3DDevice9 * pD3D9Device =m_pRenderSystem->getD3D9Device()->getDevice();
		if( NULL == pD3D9Device)
			return false;

		pD3D9Device->GetSwapChain(0, &m_pSwapChain);
		if( NULL == m_pSwapChain )
		{
			return false;
		}
		m_pSwapChain->GetPresentParameters( &m_PresentParam);
		pD3D9Device->GetDepthStencilSurface( &m_pDepthStencilBuffer);	
		m_hWnd =m_PresentParam.hDeviceWindow;

		//设置为默认渲染目标
		m_bIsDefault = true;

		//设置视口参数
		RECT rc;
		GetClientRect(m_hWnd, &rc);
		setViewport(rc.left,  rc.top, rc.right-rc.left, rc.bottom-rc.top);

		resetRenderState();//同步渲染状态

		return true;
	}

	bool RenderTargetD3D9Window::create(HWND hwnd)
	{
		IDirect3DDevice9 * pD3DDevice = m_pRenderSystem->getD3D9Device()->getDevice();
		Assert( pD3DDevice);

		::RECT rc;
		GetClientRect(hwnd, &rc);
		UINT width = rc.right - rc.left;
		if( 0 == width)
			width = 1;
		UINT height =rc.bottom - rc.top;
		if( 0 == height )
			height = 1;

		//创建交换链
		D3DPRESENT_PARAMETERS params;
		ZeroMemory( &params, sizeof( params) );
		params.Windowed = TRUE;
		params.BackBufferCount = 1;
		params.BackBufferWidth = width;
		params.BackBufferHeight = height;
		params.BackBufferFormat = D3DFMT_UNKNOWN;
		params.SwapEffect = D3DSWAPEFFECT_DISCARD;
		params.EnableAutoDepthStencil = false;
		params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
		params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		params.hDeviceWindow = hwnd;
		HRESULT hr = pD3DDevice->CreateAdditionalSwapChain( &params, &m_pSwapChain);
		if( FAILED( hr) )
			return false;
		m_hWnd = hwnd;
		m_pSwapChain->GetPresentParameters( &m_PresentParam);

		//创建深度模板缓存
		hr = pD3DDevice->CreateDepthStencilSurface(width, 
			height, 
			D3DFMT_D24S8, 
			D3DMULTISAMPLE_NONE,
			0, 
			TRUE,
			&m_pDepthStencilBuffer,
			NULL);

		if( FAILED(hr) )//尝试下D3DFMT_D15S1?
		{
			hr = pD3DDevice->CreateDepthStencilSurface(width, 
			height, 
			D3DFMT_D15S1, 
			D3DMULTISAMPLE_NONE,
			0, 
			TRUE,
			&m_pDepthStencilBuffer,
			NULL);		
		}

		if( FAILED(hr) )
			return false;

		//设置视口参数
		m_viewport.X = 0;
		m_viewport.Y = 0;
		m_viewport.Width = width ;
		m_viewport.Height = height;

		return true;
	}
}
