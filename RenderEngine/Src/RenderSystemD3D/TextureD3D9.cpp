#include "StdAfx.h"
#include "TextureD3D9.h"
#include "RenderSystemD3D9.h"
#include "D3D9Device.h"
#include "TextureManagerD3D9.h"
#include "MappingsD3D9.h"
#include "D3D9Driver.h"


namespace xs
{

	
	TextureD3D9::TextureD3D9
		(
		const std::string& name
		,TextureManagerD3D9* pTextureManager
		,FilterOptions min
		,FilterOptions mag
		,FilterOptions mip
		,TextureAddressingMode s
		,TextureAddressingMode t
		)
		:ManagedItem(name)
		,m_pTextureManager(pTextureManager)
		,m_ui32Width(1)
		,m_ui32Height(1)
		,m_ui32Depth(1)
		,m_ui32NumMipmaps(1)
		,m_i32Flags(0)
		,m_pf(PF_UNKNOWN)
		,m_memorySize(0)
		,m_pTex(0)
		,m_poolType(D3DPOOL_MANAGED)
		,m_usage(0)
		,m_minFilter(min)
		,m_magFilter(mag)
		,m_mipmapFilter(mip)
		,m_addressS(s)
		,m_addressT(t)
		,m_bLocked(false)
		,m_uiLockedLevel(-1)
	{
		Assert(m_pTextureManager);
		RenderSystemD3D9 * pRenderSystem = m_pTextureManager->getRenderSystem();
		D3D9ResourceManager * pMgr = pRenderSystem->getD3D9ResourceMgr();
		pMgr->notifyResourceCreated( this);
	}

	/*真正的释放纹理
	*/
	TextureD3D9::~TextureD3D9()
	{
		//释放纹理
		RenderSystemD3D9 * pRenderSystem = m_pTextureManager->getRenderSystem();
		D3D9ResourceManager * pMgr = pRenderSystem->getD3D9ResourceMgr();
		pMgr->lockResourceAccess();
		COM_SAFE_RELEASE( m_pTex);
		pMgr->unlockResourceAccess();

		//释放资源管理器对纹理的引用
		pMgr->notifyResourceRelease(this);
	}
	/*释放贴图
	*/
	void  TextureD3D9::release()
	{
		m_pTextureManager->releaseTexture(this);
	}

	/** 获取贴图宽度
		@return 贴图宽度值
	*/
	uint TextureD3D9::width()
	{
		return m_ui32Width;
	}

	/** 获取贴图高度
	@return 贴图高度值
	*/
	uint TextureD3D9::height()
	{
		return m_ui32Height;
	}

	/** 获取贴图深度值
	@return 贴图深度值
	*/
	uint TextureD3D9::depth()
	{
		return m_ui32Depth;
	}

	/** 获取贴图Mipmaps数目，最少为1
	@return Mipmaps数目
	*/
	uint TextureD3D9::getNumMipmaps()
	{
		return m_ui32NumMipmaps;
	}

	/** 获取贴图面数，cubemap为6，其他情况是1
	@return 贴图面数
	*/
	uint TextureD3D9::getNumFaces()
	{
		return m_i32Flags & IF_CUBEMAP ? 6 : 1;
	}

	/** 是否有Alpha通道
	@return true有通道，false则没有
	*/
	bool TextureD3D9::hasAlpha()
	{
		return PixelUtil::getFlags(m_pf) & PFF_HASALPHA;
	}

	/** 获取贴图像素格式
	@return 像素格式
	*/
	PixelFormat TextureD3D9::getPixelFormat()
	{
		return m_pf;
	}

	/** 贴图是否被压缩
	@return true是压缩格式，false未压缩
	*/
	bool TextureD3D9::isCompressed()
	{
		return m_i32Flags & IF_COMPRESSED ? true : false;
	}

	/** 获取贴图名称，可能是文件名或者是系统生成的唯一名称
	@return 贴图名称
	*/
	const std::string&	TextureD3D9::getName()
	{
		return m_strName;
	}

	/** 从文件中创建贴图
	@param name 文件名
	@return true表示创建成功，false表示失败
	*/
	bool TextureD3D9::loadFromFile(const std::string& name)
	{
		//释放旧的纹理
		RenderSystemD3D9 * pRenderSystem = m_pTextureManager->getRenderSystem();
		D3D9ResourceManager * pMgr = pRenderSystem->getD3D9ResourceMgr();
		pMgr->lockResourceAccess();
		COM_SAFE_RELEASE( m_pTex);
		pMgr->unlockResourceAccess();

		Image image;
		if(!image.load(name))return false;
		return _loadFromImage(image);
	}

	/** 从Raw中创建贴图，Raw的每个单元都应该是pf格式
	@param pBuffer 内存指针
	@param width 宽度
	@param height 高度
	@param pf 像素格式PF_R8G8B8等等
	@return true表示创建成功，false表示失败
	*/
	bool TextureD3D9::loadFromRawData(uchar *pBuffer,uint width,uint height,PixelFormat pf)
	{
		if( width <= 0
			|| height <= 0 )
			return false;

		//0,释放旧的纹理
		RenderSystemD3D9 * pRenderSystem = m_pTextureManager->getRenderSystem();
		D3D9ResourceManager * pResMgr = pRenderSystem->getD3D9ResourceMgr();
		IDirect3DDevice9 * pD3DDevice = pRenderSystem->getD3D9Device()->getDevice();
		pResMgr->lockResourceAccess();
		COM_SAFE_RELEASE( m_pTex);
		pResMgr->unlockResourceAccess();

		//1，设置基本属性
		setWidth(width);
		setHeight(height);
		setDepth(1);
		setNumMipmaps(1);
		setFlags(0);
		setPixelFormat(pf);
		m_poolType = D3DPOOL_MANAGED;
		m_usage = 0;
		m_memorySize = PixelUtil::getMemorySize( width, height, 1, pf);

		//2, 创建纹理对象		
		HRESULT hr;
		D3DFORMAT texfmt = pRenderSystem->getD3D9Device()->getClosetSupportedTextureFormat( m_pf);	
		pRenderSystem->getD3D9ResourceMgr()->waitForEndFrame();	//等待可以创建资源的通知
		pResMgr->lockDeviceAccess();//锁定设备
		hr = pD3DDevice->CreateTexture(
			m_ui32Width,
			m_ui32Height, 
			1,
			0,
			texfmt,
			D3DPOOL_MANAGED,
			& m_pTex,
			NULL);
		pResMgr->unlockDeviceAccess();
		if( FAILED(hr) )
		{
			return false;
		}

		//3，上传纹理
		if( NULL == pBuffer) //没有数据，则直接返回
			return true;

		pResMgr->lockResourceAccess();
		size_t srcPitch = getRowPitch( pf,width);
		RECT srcrc;
		srcrc.left = srcrc.top = 0;
		srcrc.right = width;
		srcrc.bottom = height;
		IDirect3DSurface9 * pSurf = 0;
		m_pTex->GetSurfaceLevel(0, &pSurf);
		hr = D3DXLoadSurfaceFromMemory(pSurf,
			NULL,
			NULL,
			pBuffer,
			MappingsD3D9::getFormat(pf),
			srcPitch,
			NULL,
			&srcrc,
			D3DX_FILTER_POINT,
			0);
		pSurf->Release();
		pResMgr->unlockResourceAccess();
		return SUCCEEDED(hr);
	}



	/** 从Image中创建贴图
	@param image 图像数据
	@param pRect 子图像区域,有时候不需要图像的全部
	@return true表示创建成功，false表示失败
	*/
	bool TextureD3D9::loadFromImage(const Image& image,const Rect* pRect)
	{
		uint width = image.getWidth();
		uint height = image.getHeight();
		if( width <= 0 || height <= 0 )
		{
			Trace("Trace:TextureD3D9::loadFromImage	image.width == 0 or image.width == 0 \n");
			return false;
		}

		//释放旧的纹理
		RenderSystemD3D9 * pRenderSystem = m_pTextureManager->getRenderSystem();
		D3D9ResourceManager * pMgr = pRenderSystem->getD3D9ResourceMgr();
		pMgr->lockResourceAccess();
		COM_SAFE_RELEASE( m_pTex);
		pMgr->unlockResourceAccess();	

		Rect rcImage(0,0,width - 1,height - 1);
		if(NULL == pRect || rcImage == *pRect)
			return _loadFromImage(image);

		Image sub;
		if(sub.subImage(image,*pRect))
		{
			return _loadFromImage(sub);
		}

		return false;
	}


	UINT TextureD3D9::getRowPitch(PixelFormat pf, uint rowWidth)
	{
		if (PixelUtil::isCompressed(pf))
		{
			// D3D wants the width of one row of cells in bytes
			if (pf == PF_DXT1)
			{				
				return (rowWidth / 4) * 8;// 64 bits (8 bytes) per 4x4 block
			}
			else
			{	
				return  ( rowWidth / 4) * 16;// 128 bits (16 bytes) per 4x4 block
			}
		}
		else
		{
			return  rowWidth * PixelUtil::getNumElemBytes(pf);
		}
	}

	
	/** 设置贴图区域数据,像素格式需要用户自己保证符合，不会缩放图像
	@param level 要设置数据的图像Mipmaps等级,从0开始
	@param left 左边开始下标
	@param top 上边开始下标
	@param width 宽度
	@param height 高度
	@param pData 设置的贴图数据
	@return true表示创建成功，false表示失败
	*/
	bool TextureD3D9::setSubData(uint level,uint left,uint top,uint width,uint height,void  *pData)
	{
		if( NULL == m_pTex )
			return false;

		if( NULL == pData)
			return false;

		if( level >= m_ui32NumMipmaps
			|| width <= 0
			|| ( left + width) > m_ui32Width
			|| height <= 0
			|| (top + height) > m_ui32Height
			)
			return false;

		RenderSystemD3D9 * pRenderSystem = m_pTextureManager->getRenderSystem();
		D3D9ResourceManager * pResMgr = pRenderSystem->getD3D9ResourceMgr();
		IDirect3DDevice9 * pD3DDevice = pRenderSystem->getD3D9Device()->getDevice();

		pResMgr->lockResourceAccess();
		RECT destrc = { left, top, (left+width), (top + height)};
		RECT srcrc = { 0, 0, width, height};
		UINT srcPitch = getRowPitch(m_pf, width);
		IDirect3DSurface9 * pSurf = 0;
		HRESULT hr = m_pTex->GetSurfaceLevel( level, &pSurf);
		hr = D3DXLoadSurfaceFromMemory(
			pSurf,
			NULL,
			&destrc,
			pData,
			MappingsD3D9::getFormat(m_pf),
			srcPitch,
			NULL,
			&srcrc,
			D3DX_FILTER_POINT,
			0);
		pSurf->Release();
		pResMgr->unlockResourceAccess();

		return SUCCEEDED( hr );
	}

	/** 设置贴图区域数据,从图像获取数据,从image的pRect获取数据blt到贴图的(0,0)，不会缩放图像
	@param level 要设置数据的图像Mipmaps等级,从0开始
	@param image 图像数据
	@param pRect 图像的子区域
	@return true表示创建成功，false表示失败
	*/
	bool TextureD3D9::setSubData(uint level,const Image& image,const Rect* pRect)
	{
		//检查
		if( NULL == m_pTex )
			return false;
		if( level >= m_ui32NumMipmaps)
			return false;

		uint imageWidth = image.getWidth();
		uint imageHeight = image.getHeight();
		Rect rcImage = Rect(0,0,imageWidth,imageHeight); //好像不需要减1，D3D9把right和bottom作为岗哨?
		Rect rcIntersect;
		Image sub;
		const Image * pImage = 0;
		if(pRect)
		{
			if(!rcImage.intersect(*pRect,rcIntersect))
				return false;	
			if( !sub.subImage( image, rcIntersect)  )
				return false;
			pImage  = &sub;
		}
		else
		{
			rcIntersect = rcImage;
			pImage = &image;
		}


		RenderSystemD3D9 * pRenderSystem = m_pTextureManager->getRenderSystem();
		D3D9ResourceManager * pResMgr = pRenderSystem->getD3D9ResourceMgr();
		IDirect3DDevice9 * pD3DDevice = pRenderSystem->getD3D9Device()->getDevice();

		pResMgr->lockResourceAccess();
		RECT destrc = { 0, 0, width(), height() };
		RECT srcrc =  { 0, 0, pImage->getWidth(), pImage->getHeight() };
		UINT srcPitch = getRowPitch(pImage->getFormat(), pImage->getWidth() );
		IDirect3DSurface9 * pSurf = 0;
		HRESULT hr = m_pTex->GetSurfaceLevel( level, &pSurf);
		hr = D3DXLoadSurfaceFromMemory(
			pSurf,
			NULL,
			&destrc,
			pImage->getData(),
			MappingsD3D9::getFormat( pImage->getFormat() ),
			srcPitch,
			NULL,
			&srcrc,
			D3DX_FILTER_POINT,
			0);
		pSurf->Release();
		pResMgr->unlockResourceAccess();
		return SUCCEEDED(hr);	
	}

	size_t TextureD3D9::getMemorySize()
	{
		return m_memorySize;
	}


	void TextureD3D9::setCustomInfo(const std::string & key, const std::string & val)
	{
		m_customInfo[key] = val;
	}


	bool TextureD3D9::getCustomInfo(const std::string & key, std::string & val)
	{
		CustomInfoMap::iterator it = m_customInfo.find(key);
		if( it == m_customInfo.end() )
			return false;

		val = it->second;
		return true;
	}

	bool TextureD3D9::_loadFromImage(const Image& image)
	{
		//1,设置纹理基本属性
		setWidth(image.getWidth());
		if( m_ui32Width <= 0 ) return false;
		setHeight(image.getHeight());
		if( m_ui32Height <= 0 ) return false;
		setDepth(image.getDepth());
		if( m_ui32Depth <= 0 ) return false;
		setNumMipmaps(image.getNumMipmaps());
		setFlags(image.getFlags());
		setPixelFormat(image.getFormat());
		m_poolType = D3DPOOL_MANAGED;
		m_usage = 0;

		RenderSystemD3D9 * pRenderSystem = m_pTextureManager->getRenderSystem();
		D3D9ResourceManager * pResMgr = pRenderSystem->getD3D9ResourceMgr();
		IDirect3DDevice9 * pD3DDevice = pRenderSystem->getD3D9Device()->getDevice();
	
		//2, 构建纹理
		HRESULT hr;
		D3DFORMAT texfmt = pRenderSystem->getD3D9Device()->getClosetSupportedTextureFormat( m_pf);
		pRenderSystem->getD3D9ResourceMgr()->waitForEndFrame();//等待可以创建资源的通知
		pResMgr->lockDeviceAccess();//锁定设备
		hr = pD3DDevice->CreateTexture(
			m_ui32Width,
			m_ui32Height, 
			m_ui32NumMipmaps,
			0,
			texfmt,
			D3DPOOL_MANAGED,
			& m_pTex,
			NULL);
		pResMgr->unlockDeviceAccess();
		if( FAILED(hr) )
			return false;


		//3， 上传纹理
		pResMgr->lockResourceAccess();
		for( uint i=0; i< m_ui32NumMipmaps; ++i)
		{
			xs::PixelBox box = image.getPixelBox(0, i);
			size_t srcPitch = getRowPitch(box.format, box.rowPitch);
			RECT srcrc = getD3DRect( box);
			IDirect3DSurface9 * pSurf = 0;
			m_pTex->GetSurfaceLevel(i, &pSurf);
			hr = D3DXLoadSurfaceFromMemory(pSurf,
				NULL,
				NULL,
				box.data,
				MappingsD3D9::getFormat(box.format),
				srcPitch,
				NULL,
				&srcrc,
				D3DX_FILTER_POINT,
				0);
			pSurf->Release();
			if( FAILED(hr) )
			{
				pResMgr->unlockResourceAccess();
				return false;
			}
			m_memorySize += PixelUtil::getMemorySize( box.getWidth(), box.getHeight(), box.getDepth(), box.format);
		}
		pResMgr->unlockResourceAccess();

		return true;
	}

	// 设备丢失，在这里释放Default Pool管理的资源
	void TextureD3D9::notifyOnDeviceLost()
	{
		//在这里释放状态块
		if( m_poolType == D3DPOOL_DEFAULT)
		{
			COM_SAFE_RELEASE( m_pTex );
		}
	}

	//设备恢复, 在这里重建Default Pool 管理的资源
	void TextureD3D9::notifyOnDeviceReset()
	{
		if( m_poolType == D3DPOOL_DEFAULT)
		{
			//重构纹理
			Assert(  NULL == m_pTex);
			D3DFORMAT texfmt = m_pTextureManager->getRenderSystem()->getD3D9Device()->getClosetSupportedTextureFormat( m_pf);
			IDirect3DDevice9 * pD3DDevice = m_pTextureManager->getRenderSystem()->getD3D9Device()->getDevice();
			pD3DDevice->CreateTexture(
				m_ui32Width,
				m_ui32Height, 
				m_ui32NumMipmaps,
				m_usage,
				texfmt,
				D3DPOOL_DEFAULT,
				& m_pTex,
				NULL);
		}
	}

	void TextureD3D9::setWidth(uint w)
	{
		m_ui32Width = w;
	}

	void TextureD3D9::setHeight(uint h)
	{
		m_ui32Height =h;
	}

	void TextureD3D9::setDepth(uint d)
	{
		m_ui32Depth = d;
	}

	void TextureD3D9::setNumMipmaps(uint numMipmaps)
	{
		m_ui32NumMipmaps = numMipmaps;
	}

	void TextureD3D9::setFlags(int flags)
	{
		m_i32Flags = flags;
	}

	void TextureD3D9::setPixelFormat(PixelFormat pf)
	{
		m_pf = pf;
	}

	RECT TextureD3D9::getD3DRect(const  xs::Box & box )
	{
		RECT rc;
		rc.left = static_cast<LONG>(box.left);
		rc.top  = static_cast<LONG>(box.top);
		rc.right = static_cast<LONG>(box.right);
		rc.bottom = static_cast<LONG>(box.bottom);
		return rc;
	}

	void TextureD3D9::applyFilterState( DWORD sampler )
	{
		IDirect3DDevice9 * pD3DDevice = m_pTextureManager->getRenderSystem()->getD3D9Device()->getDevice();
		RenderSystemD3D9State * pRenderSystemState = m_pTextureManager->getRenderSystem()->getRenderSystemState();
		Assert( sampler < pRenderSystemState->s_maxTexureUnit);
		if (sampler >= pRenderSystemState->s_maxTexureUnit)
		{
			return;
		}

		//min filter
		if( m_minFilter != pRenderSystemState->m_minFilter[sampler] )
		{
			pRenderSystemState->m_minFilter[sampler] = m_minFilter;
			pD3DDevice->SetSamplerState(sampler, D3DSAMP_MINFILTER, MappingsD3D9::getFilterType(m_minFilter));
		}
		

		//mag filter
		if( m_magFilter != pRenderSystemState->m_magFilter[sampler] )
		{
			pRenderSystemState->m_magFilter[sampler] = m_magFilter;
			pD3DDevice->SetSamplerState( sampler, D3DSAMP_MAGFILTER, MappingsD3D9::getFilterType( m_magFilter) );
		}

		//mip filter
		if( m_mipmapFilter != pRenderSystemState->m_mipFilter[sampler] )
		{
			pRenderSystemState->m_mipFilter[sampler] = m_mipmapFilter;
			pD3DDevice->SetSamplerState(sampler, D3DSAMP_MIPFILTER, MappingsD3D9::getFilterType( m_mipmapFilter));
		}

		//address u 
		if( m_addressS != pRenderSystemState->m_addressU[sampler]
		&& MappingsD3D9::getAddressMode(m_addressS) != MappingsD3D9::getAddressMode(pRenderSystemState->m_addressU[sampler]) 
			)
		{
			pRenderSystemState->m_addressU[sampler] = m_addressS;
			pD3DDevice->SetSamplerState(sampler, D3DSAMP_ADDRESSU, MappingsD3D9::getAddressMode(m_addressS) );
		}

		//address v
		if( m_addressT != pRenderSystemState->m_addressV[sampler] 
		&& MappingsD3D9::getAddressMode(m_addressT) != MappingsD3D9::getAddressMode(pRenderSystemState->m_addressV[sampler]) 
			)
		{
			pRenderSystemState->m_addressV[sampler] = m_addressT;
			pD3DDevice->SetSamplerState(sampler, D3DSAMP_ADDRESSV, MappingsD3D9::getAddressMode(m_addressT) );

		}
	}

	/**
	*/
	bool TextureD3D9::lock(UINT level, D3DLOCKED_RECT & lockedrect, DWORD flag)
	{
		if( D3DPOOL_DEFAULT == m_poolType 
			&& D3DUSAGE_DYNAMIC != m_usage )
			 return false;

		if( m_bLocked ) return false;

		if( level >= m_ui32NumMipmaps) return false;

		if( NULL == m_pTex) return false;

		RenderSystemD3D9 * pRenderSystem = m_pTextureManager->getRenderSystem();
		D3D9ResourceManager * pResMgr = pRenderSystem->getD3D9ResourceMgr();
		IDirect3DDevice9 * pD3DDevice = pRenderSystem->getD3D9Device()->getDevice();

		pResMgr->lockResourceAccess();
		IDirect3DSurface9 * pSurf = 0;
		HRESULT hr = m_pTex->GetSurfaceLevel( level, & pSurf);
		pSurf->LockRect( &lockedrect, NULL, flag|D3DLOCK_NO_DIRTY_UPDATE);
		pSurf->Release();

		if( FAILED(hr) )
		{
			pResMgr->unlockResourceAccess();
			return false;
		}
		else
		{
			m_bLocked = true;
			m_uiLockedLevel = level;
			return true;
		}
	}

	/**
	*/
	void TextureD3D9::unlock()
	{
		if( m_bLocked)
		{
			IDirect3DSurface9 * pSurf = 0;
			HRESULT hr = m_pTex->GetSurfaceLevel( m_uiLockedLevel, & pSurf);
			pSurf->UnlockRect();
			pSurf->Release();
			m_pTextureManager->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();
			m_bLocked = false;
			m_uiLockedLevel = -1;
		}
	}

	/**
	*/
	bool TextureD3D9::isLocked()
	{
		return m_bLocked;
	}

	/** 创建纹理
	*/
	bool TextureD3D9::_create(D3DPOOL pool, DWORD usage, uint width, uint height,  PixelFormat pf, CreateTextureOptions option)
	{
		if( width <= 0
			|| height <= 0 )
			return false;

		//0,释放旧的纹理
		RenderSystemD3D9 * pRenderSystem = m_pTextureManager->getRenderSystem();
		D3D9ResourceManager * pResMgr = pRenderSystem->getD3D9ResourceMgr();
		IDirect3DDevice9 * pD3DDevice = pRenderSystem->getD3D9Device()->getDevice();
		pResMgr->lockResourceAccess();
		COM_SAFE_RELEASE( m_pTex);
		pResMgr->unlockResourceAccess();

		//1，设置基本属性
		setWidth(width);
		setHeight(height);
		setDepth(1);
		setNumMipmaps(1);
		setFlags(0);
		setPixelFormat(pf);
		m_poolType = pool;
		m_usage = usage;
		m_memorySize = PixelUtil::getMemorySize( width, height, 1, pf);

		//2, 创建纹理对象	
		HRESULT hr;
		D3DFORMAT texfmt = pRenderSystem->getD3D9Device()->getClosetSupportedTextureFormat( m_pf);

		if( !(option & CTO_NOWAIT) ) pResMgr->waitForEndFrame();//等待创建资源的通知

		pResMgr->lockDeviceAccess();//锁定设备
		hr = pD3DDevice->CreateTexture(
			m_ui32Width,
			m_ui32Height, 
			1,
			usage,
			texfmt,
			pool,
			&m_pTex,
			NULL);
		pResMgr->unlockDeviceAccess();
		return SUCCEEDED(hr);	
	}
}