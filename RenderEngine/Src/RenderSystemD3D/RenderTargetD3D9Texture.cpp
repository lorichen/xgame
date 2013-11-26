#include "StdAfx.h"
#include "RenderTargetD3D9Texture.h"
#include "TextureD3D9.h"
#include "TextureManagerD3D9.h"
#include "RenderSystemD3D9.h"
#include "D3D9Device.h"
#include "RenderTargetManagerD3D9.h"

namespace xs
{

	RenderTargetD3D9Texture::RenderTargetD3D9Texture(RenderSystemD3D9 * pRenderSystem, bool b2d, RenderTargetManagerD3D9 * pRenderTargetMgr)
		:RenderTargetD3D9(pRenderSystem, b2d, pRenderTargetMgr)
		,m_pTexture(0)
		,m_pDepthStencilBuffer(0)
	{
	
	}

	RenderTargetD3D9Texture::~RenderTargetD3D9Texture()
	{

	}

	bool RenderTargetD3D9Texture::create(int width, int height,bool alpha,
			FilterOptions min , 
			FilterOptions mag, 
			FilterOptions mip,
			TextureAddressingMode s,											
			TextureAddressingMode t)
	{
		//创建纹理
		TextureManagerD3D9 * pTextureMgr  =  m_pRenderSystem->getTextureMgr();
		if( NULL == pTextureMgr)
			return false;
		PixelFormat pf = alpha ? PF_R8G8B8A8 : PF_B8G8R8;
		m_pTexture = pTextureMgr->createEmptyeTexture(D3DPOOL_DEFAULT, D3DUSAGE_RENDERTARGET, 
			width,height,pf, min, mag, mip,s,t);
		if( NULL == m_pTexture)
			return false;

		//创建深度模板缓存
		IDirect3DDevice9 * pD3DDevice = m_pRenderSystem->getD3D9Device()->getDevice();
		if( NULL == pD3DDevice)
			return  false;
		D3DSURFACE_DESC surfacedesc;
		m_pRenderTargetMgr->getCurrentRenderTarget()->getDepthStencilBuffer()->GetDesc( &surfacedesc);
		HRESULT hr = pD3DDevice->CreateDepthStencilSurface(
			width, 
			height,
			surfacedesc.Format,
			D3DMULTISAMPLE_NONE,
			0,
			TRUE,
			&m_pDepthStencilBuffer,
			NULL);
		if( FAILED(hr) )
		{
			return false;
		}

		//设置视口参数
		m_viewport.X = 0;
		m_viewport.Y = 0;
		m_viewport.Width = width ;
		m_viewport.Height = height;

		return true;
	}

	void RenderTargetD3D9Texture::release()
	{
		if( NULL != m_pTexture)
			m_pTexture->release();

		COM_SAFE_RELEASE(m_pDepthStencilBuffer);
		delete this;
	}

	const ITexture*	RenderTargetD3D9Texture::getTexture()
	{
		return m_pTexture;
	}

	bool RenderTargetD3D9Texture::getTextureData(void * pData)
	{
		if( 0 == m_pTexture || 0 == pData )
			return false;

		IDirect3DTexture9* pSrcTexture = m_pTexture->getD3DTexture();
		if( 0 == pSrcTexture )
			return false;
		D3DSURFACE_DESC srcTexDesc;
		pSrcTexture->GetLevelDesc(0, &srcTexDesc);

		IDirect3DTexture9* pDestTex =0;
		IDirect3DDevice9* pD3D9Device = m_pRenderSystem->getD3D9Device()->getDevice();
		HRESULT hr = pD3D9Device->CreateTexture( srcTexDesc.Width,
			srcTexDesc.Height,
			0,
			0,
			srcTexDesc.Format,
			D3DPOOL_SYSTEMMEM,
			&pDestTex,
			0);
		if( FAILED(hr))
		{
			return false;
		}

		IDirect3DSurface9* pSrcSurf  =0;
		pSrcTexture->GetSurfaceLevel(0, &pSrcSurf);
		IDirect3DSurface9* pDestSurf = 0;
		pDestTex->GetSurfaceLevel(0, &pDestSurf);
		hr = pD3D9Device->GetRenderTargetData( pSrcSurf, pDestSurf);
		if( FAILED(hr))
		{
			COM_SAFE_RELEASE(pSrcSurf);
			COM_SAFE_RELEASE(pDestSurf);
			COM_SAFE_RELEASE(pDestTex);
			return false;
		}
		else
		{
			COM_SAFE_RELEASE(pSrcSurf);
		}

		D3DSURFACE_DESC destSurfDesc;
		pDestSurf->GetDesc(&destSurfDesc);
		D3DLOCKED_RECT lockrect;
		pDestSurf->LockRect(&lockrect, NULL, D3DLOCK_READONLY);

		if( m_pTexture->hasAlpha() )
		{
			if( destSurfDesc.Format == D3DFMT_A8B8G8R8 )
			{
				uchar* pDesData = (uchar*)pData;
				for( UINT i=0; i < destSurfDesc.Height; ++i)
				{
					uchar* pSrcData = (uchar*)lockrect.pBits + i * lockrect.Pitch;
					for( UINT j =0; j<destSurfDesc.Width; ++j)
					{
						uint startAddr = j * 4;
						pDesData[0] = pSrcData[0];//red
						pDesData[1] = pSrcData[1];//green;
						pDesData[2] = pSrcData[2];//blue;
						pDesData[3] = pSrcData[3];//alpha
						pDesData +=4;
						pSrcData +=4;
					}
				}
			}
			else
			{
				pDestSurf->UnlockRect();
				COM_SAFE_RELEASE(pDestSurf);
				COM_SAFE_RELEASE(pDestTex);
				return false;
			}
		}
		else
		{
			if( destSurfDesc.Format == D3DFMT_R8G8B8)
			{
				uchar* pDesData = (uchar*)pData;
				for( UINT i=0; i < destSurfDesc.Height; ++i)
				{
					uchar* pSrcData = (uchar*)lockrect.pBits + i * lockrect.Pitch;
					for( UINT j =0; j<destSurfDesc.Width; ++j)
					{
						pDesData[0] = pSrcData[2];//red
						pDesData[1] = pSrcData[1];//green;
						pDesData[2] = pSrcData[0];//blue;
						pDesData +=3;
						pSrcData +=3;
					}
				}
			}
			else if( destSurfDesc.Format == D3DFMT_X8R8G8B8)
			{
				uchar* pDesData = (uchar*)pData;
				for( int i= destSurfDesc.Height-1; i >=0  ;  --i)
				{
					uchar* pSrcData = (uchar*)lockrect.pBits + i * lockrect.Pitch;
					for( UINT j =0; j<destSurfDesc.Width; ++j)
					{
						pDesData[0] = pSrcData[2];//red
						pDesData[1] = pSrcData[1];//green;
						pDesData[2] = pSrcData[0];//blue;
						pDesData +=3;
						pSrcData +=4;
					}
				}
			}
			else
			{
				pDestSurf->UnlockRect();
				COM_SAFE_RELEASE(pDestSurf);
				COM_SAFE_RELEASE(pDestTex);
				return false;
			}
		}

		pDestSurf->UnlockRect();
		COM_SAFE_RELEASE(pDestSurf);
		COM_SAFE_RELEASE(pDestTex);
		return true;
	}

	void RenderTargetD3D9Texture::onAttach(RenderTargetD3D9 * pPrevious)
	{
		IDirect3DDevice9 * pD3D9Device =m_pRenderSystem->getD3D9Device()->getDevice();
		Assert(pD3D9Device);

		//设置当前的渲染目标和深度模版缓存
		IDirect3DTexture9 * pD3DTexture = m_pTexture->getD3DTexture();
		if( pD3DTexture )
		{
			IDirect3DSurface9 * pSurf = 0;
			pD3DTexture->GetSurfaceLevel(0,&pSurf);
			pD3D9Device->SetRenderTarget(0, pSurf);
			pSurf->Release();
		}
		pD3D9Device->SetDepthStencilSurface(m_pDepthStencilBuffer);
		
		//设置当前渲染目标的渲染状态
		RenderTargetD3D9::onAttach(pPrevious);

	}

	void RenderTargetD3D9Texture::onDetach()
	{
		//需要做detach的事情
		//目前将深度模板缓存取消
		IDirect3DDevice9 * pD3D9Device = m_pRenderSystem->getD3D9Device()->getDevice();
		Assert(pD3D9Device);

		//取消深度模板缓存
		pD3D9Device->SetDepthStencilSurface(NULL);
	}

	void RenderTargetD3D9Texture::clearFrameBuffer(bool clearColorBuffer,bool clearDepthBuffer ,bool clearStencilBuffer)
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

	void RenderTargetD3D9Texture::onBeginFrame(bool clearColorBuffer,bool clearDepthBuffer,bool clearStencilBuffer)
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

		m_pRenderSystem->getD3D9ResourceMgr()->beginFrame();
		pD3D9Device->BeginScene();
	}

	void RenderTargetD3D9Texture::onEndFrame(int layer)
	{
		IDirect3DDevice9 * pD3D9Device = m_pRenderSystem->getD3D9Device()->getDevice();
		Assert(pD3D9Device);
		pD3D9Device->EndScene();
		m_pRenderSystem->getD3D9ResourceMgr()->endFrame();
	}

	RECT RenderTargetD3D9Texture::getFrameBufferRect()
	{
		RECT rc;
		rc.left = rc.top = 0;
		Assert(m_pTexture);
		rc.right = m_pTexture->width();
		rc.bottom = m_pTexture->height();
		return rc;
	}

	bool RenderTargetD3D9Texture::getDisplayMode(RenderEngineCreationParameters & createParam)
	{
		if( 0 == m_pTexture )
			return false;

		createParam.w = m_pTexture->width();
		createParam.h = m_pTexture->height();
		createParam.colorDepth = m_pTexture->hasAlpha()?32:24;
		createParam.fullscreen = false;
		createParam.refreshRate = 0;

		return true;
	}

	bool RenderTargetD3D9Texture::switchDisplayMode(RenderEngineCreationParameters* param)
	{
		if( 0 == param )
			return false;

		//获取参数
		int width = param->w; 
		int height = param->h;
		bool alpha = (param->colorDepth>24) ? true:false;
		FilterOptions minFO = m_pTexture->getMinFilter();
		FilterOptions magFO = m_pTexture->getMagFilter();
		FilterOptions mipFO = m_pTexture->getMipFilter();
		TextureAddressingMode s = m_pTexture->getAddressS();
		TextureAddressingMode t = m_pTexture->getAddressT();

		//释放资源
		safeRelease(m_pTexture);
		COM_SAFE_RELEASE(m_pDepthStencilBuffer);

		//重新创建
		return create(width, height, alpha, minFO, magFO, mipFO, s,t);

	}


	IDirect3DSurface9 * RenderTargetD3D9Texture::getDepthStencilBuffer()
	{
		return m_pDepthStencilBuffer;
	}

	void RenderTargetD3D9Texture::onDeviceLost()
	{
		COM_SAFE_RELEASE( m_pDepthStencilBuffer);
		//m_pTexture //m_pTexture已经由D3DResourceManager处理了
	}

	void RenderTargetD3D9Texture::onDeviceReset()
	{
		//创建深度模板缓存
		IDirect3DDevice9 * pD3DDevice = m_pRenderSystem->getD3D9Device()->getDevice();
		Assert(pD3DDevice);

		HRESULT hr = pD3DDevice->CreateDepthStencilSurface(
			m_pTexture->width(), 
			m_pTexture->height(),
			D3DFMT_D24S8,
			D3DMULTISAMPLE_NONE,
			0,
			TRUE,
			&m_pDepthStencilBuffer,
			NULL);
		if( FAILED(hr) )
		{
			hr = pD3DDevice->CreateDepthStencilSurface(
			m_pTexture->width(), 
			m_pTexture->height(),
			D3DFMT_D15S1,
			D3DMULTISAMPLE_NONE,
			0,
			TRUE,
			&m_pDepthStencilBuffer,
			NULL);
		}

		//设置视口参数
		m_viewport.X = 0;
		m_viewport.Y = 0;
		m_viewport.Width = m_pTexture->width();
		m_viewport.Height = m_pTexture->height();
	}
}