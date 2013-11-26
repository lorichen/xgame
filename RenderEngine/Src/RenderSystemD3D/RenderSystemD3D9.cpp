#include "StdAfx.h"
#include "RenderSystemD3D9.h"
#include "D3D9Driver.h"
#include "D3D9Device.h"
#include "D3D9ResourceManager.h"
#include "RenderTargetD3D9.h"
#include "RenderTargetManagerD3D9.h"
#include "StreamSourceMappings.h"
#include "BufferManagerD3D9.h"
#include "IndexBufferD3D9.h"
#include "VertexBufferD3D9.h"
#include "MappingsD3D9.h"
#include "BeginEndPrimitiveProxy.h"
#include "TextureManagerD3D9.h"
#include "TextureD3D9.h"
#include "ShaderProgramManagerD3D9.h"
//#include "VFWAVIObjectD3D9.h"
#include "HardwareCursorManagerD3D9.h"

namespace xs
{
	std::list<RenderSystemD3D9 * > RenderSystemD3D9::m_vRenderSystem;//渲染系统

	void RenderSystemD3D9::release()
	{
		close();

		//从渲染系统队列中删除该渲染系统
		RenderSystemContainerIterator it = m_vRenderSystem.begin();
		for( ; it != m_vRenderSystem.end(); ++it)
		{
			if( (*it) == this)
			{
				m_vRenderSystem.erase( it);
				break;
			}
		}

		delete this;
	}

	RenderSystemD3D9State::RenderSystemD3D9State()
	{
		//图元状态
		m_lineWidth = 1.0f;
		m_pointSize = 1.0f;
		//填充模式
		m_fillMode = FM_SOLID;
		//beginPrimitive 和 endPrimitive 相关状态
		m_color = xs::ColorValue::White;
		m_edgeFlag = true;
		m_normal = xs::Vector3::UNIT_Z;
		for( uint i=0; i< s_maxTexureUnit; ++i)
		{
			m_texcoord[i] = xs::Vector2::ZERO;
		}
		//当前材质
		m_ambient = xs::ColorValue(0.2f,0.2f,0.2f,1.0f);
		m_diffuse = xs::ColorValue(0.8f,0.8f,0.8f,1.0f);
		m_specular = xs::ColorValue::Black;
		m_emissive = xs::ColorValue::Black;
		m_shininess = 0.0f;
		m_material.Ambient = D3DXCOLOR(m_ambient.r, m_ambient.g, m_ambient.b,m_ambient.a);
		m_material.Diffuse = D3DXCOLOR(m_diffuse.r, m_diffuse.g, m_diffuse.b,m_diffuse.a);
		m_material.Specular = D3DXCOLOR(m_specular.r, m_specular.g, m_specular.b,m_specular.a);
		m_material.Emissive = D3DXCOLOR(m_emissive.r, m_emissive.g, m_emissive.b,m_emissive.a);
		m_material.Power = m_shininess;
		m_trackColor = TVC_NONE;//ogl默认关闭颜色跟踪
		//雾化
		m_bEnableFog = false;
		m_fogMode = FOG_EXP;
		m_fogColor = xs::ColorValue(0.0f,0.0f,0.0f,0.0f);
		m_fogExpDensity = 1.0f;
		m_fogLinearStart = 0.0f;
		m_fogLinearEnd = 1.0f;	
		//光照
		for( uint i =0; i < s_maxLight; ++i)
		{
			m_light[i].enable(false);
			MappingsD3D9::getLight(m_light[i], m_d3d9light[i]);
		}
		m_ambientLight = xs::ColorValue( 0.2f, 0.2f, 0.2f, 1.0f);
		//alpha检测
		m_bAlphaCheckEnabled = false;
		m_AlphaCompareFunction = CMPF_ALWAYS_PASS;
		m_AlphaCompareValue = 0;

		//模板检测
		m_bEnableStencilTest = false;
		CompareFunction m_stencilFunc = CMPF_ALWAYS_PASS;
		m_stencilRefValue = 0;
		m_stencilMask = 0xffffffff;
		m_stencilFailOp = SOP_KEEP;
		m_depthFailOp = SOP_KEEP;
		m_passOp = SOP_KEEP;

		//剪切测试
		m_bScissor = false;

		//alpha混合
		m_alphaBlendSrc = SBF_ONE;
		m_alphaBlendDst = SBF_ZERO;

		//采样器状态
		for( uint i =0; i<s_maxTexureUnit; ++i)
		{
			m_addressU[i] = TAM_WRAP;
			m_addressV[i] = TAM_WRAP;
			m_minFilter[i] = FO_POINT;
			m_magFilter[i] = FO_POINT;
			m_mipFilter[i] = FO_NONE;
		}

		//纹理Stage的初始状态
		m_TSSEnable[0] = true;
		for( uint i = 1; i < s_maxTexureUnit; ++i)
		{
			m_TSSEnable[i]= false;
		}
		for( uint i =0; i < s_maxTexureUnit; ++i)
		{
			m_TSSColorOP[i] = TSO_MODULATE;
			m_TSSColorArg[i][0] = TSA_TEXTURE;
			m_TSSColorArg[i][1] = TSA_PREVIOUS;

			m_TSSAlphaOP[i] = TSO_MODULATE;
			m_TSSAlphaArg[i][0] = TSA_TEXTURE;		
			m_TSSAlphaArg[i][1] = TSA_PREVIOUS;	
		}
		
		// 反锯齿
		m_bMutiSampleAntialias = true;

	}

	RenderSystemD3D9::RenderSystemD3D9():
	m_pcsDirect3D9(0),
	m_pcsD3D9DDriver(0),
	m_pcsD3D9Device(0),
	m_pcsD3D9ResourceMgr(0),
	m_pcsRenderTargetMgr(0),
	m_pcsBufferMgr(0),
	m_pcsStreamSourceMappings(0),
	m_pcsBeginEndPrimitiveProxy(0),
	m_pcsTextureMgr(0),
	m_pcsShaderProgramMgr(0),
	m_pcsHardwareCursorMgr(0),
	m_pTeapot(0)
	{

	}

	void	RenderSystemD3D9::close()
	{
		//释放茶壶mesh
		COM_SAFE_RELEASE( m_pTeapot );

		//释放硬件鼠标管理器
		if( NULL != m_pcsHardwareCursorMgr )
		{
			safeRelease(m_pcsHardwareCursorMgr);
		}

		//释放Shader程序管理器	
		if( NULL != m_pcsShaderProgramMgr)
		{
			safeRelease(m_pcsShaderProgramMgr);
		}

		//释放代理beginPrimitive和endPrimitive绘制方式
		if( NULL != m_pcsBeginEndPrimitiveProxy)
		{
			safeRelease(m_pcsBeginEndPrimitiveProxy);
		}

		//创建数据流映射
		if( NULL != m_pcsStreamSourceMappings)
		{
			safeRelease(m_pcsStreamSourceMappings);	
		}

		//释放Buffer管理器
		if( NULL != m_pcsBufferMgr)
		{
			safeRelease( m_pcsBufferMgr	);
		}

		//创建渲染目标管理器
		if(NULL != m_pcsRenderTargetMgr )
		{
			safeRelease( m_pcsRenderTargetMgr );			
		}

		//释放纹理管理器
		if( NULL != m_pcsTextureMgr)
		{
			safeRelease(m_pcsTextureMgr);
		}

		//释放资源管理器
		if( NULL != m_pcsD3D9ResourceMgr )
		{
			safeRelease( m_pcsD3D9ResourceMgr);	
		}

		//释放D3D9设备对象
		if( NULL != m_pcsD3D9Device)
		{
			safeRelease( m_pcsD3D9Device);
		}

		//释放D3D9设备驱动程序
		if( NULL != m_pcsD3D9DDriver)
		{
			safeRelease( m_pcsD3D9DDriver );
		}

		//释放D3D9对象
		COM_SAFE_RELEASE(m_pcsDirect3D9);
	}

	const char* RenderSystemD3D9::getName()
	{
		static char szName[256] = "D3D9";
		return szName;
	}

	RenderSystemCapabilities*	RenderSystemD3D9::getCapabilities()
	{
		return m_pcsD3D9Device->getCapabilities();
	}

	bool RenderSystemD3D9::setCurrentRenderTarget(uint hwnd)
	{
		return m_pcsRenderTargetMgr->setCurrentRenderTarget( hwnd);
	}

	const ITexture*	RenderSystemD3D9::getRenderTargetTexture(uint hwnd)
	{
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getRenderTarget(hwnd);
		if( NULL != pRT) return pRT->getTexture();
		else return 0;
	}

	bool RenderSystemD3D9::isScissorEnabled()
	{
		return m_csRenderState.m_bScissor;
	}

	void RenderSystemD3D9::setScissorEnabled(bool enabled)
	{
		if( m_csRenderState.m_bScissor != enabled)
		{
			m_csRenderState.m_bScissor = enabled;
			m_pcsD3D9Device->getDevice()->SetRenderState(D3DRS_SCISSORTESTENABLE, enabled?TRUE:FALSE);
 		}
	}

	uint RenderSystemD3D9::addRTT(int width,int height, bool alpha ,
		FilterOptions min , 
		FilterOptions mag, 
		FilterOptions mip,
		TextureAddressingMode s ,											
		TextureAddressingMode t)
	{
		return m_pcsRenderTargetMgr->addRTT( width, height, alpha, min, mag, mip, s, t);
	}

	bool RenderSystemD3D9::addRenderTarget(uint hwnd)
	{
		return m_pcsRenderTargetMgr->addRenderTarget( hwnd);
	}

	bool RenderSystemD3D9::removeRenderTarget(uint hwnd)
	{
		return m_pcsRenderTargetMgr->removeRenderTarget( hwnd);
	}

	uint RenderSystemD3D9::MTaddRenderTarget(int layer)
	{
		return m_pcsRenderTargetMgr->MTaddRenderTarget();
	}

	bool RenderSystemD3D9::MTremoveRenderTarget(uint key)
	{
		return m_pcsRenderTargetMgr->MTremoveRenderTarget(key);
	}

	bool RenderSystemD3D9::addOverlayRenderTarget()
	{
		return m_pcsRenderTargetMgr->addOverlayRenderTarget();
	}

	void RenderSystemD3D9::removeOverlayRenderTarget()
	{
		return m_pcsRenderTargetMgr->removeOverlayRenderTarget();
	}

	bool RenderSystemD3D9::setOverlayRenderTarget()
	{
		return m_pcsRenderTargetMgr->setOverlayRenderTarget();
	}

	uint RenderSystemD3D9::getCurrentRenderTarget()
	{
		return m_pcsRenderTargetMgr->getCurrentRenderTargetID();
	}

	bool RenderSystemD3D9::getRenderTargetTextureData(uint hwnd,void  *pData)
	{
		RenderTargetD3D9* pRT = m_pcsRenderTargetMgr->getRenderTarget(hwnd);
		if( 0 == pRT )
			return false;
		else
		{
			return pRT->getTextureData(pData);
		}
	}

	bool	RenderSystemD3D9::create(RenderEngineCreationParameters *param)
	{
		//参数检测
		if( param->rst != RS_D3D9)
		{
			Trace("Trace:RenderSystem::create	param->rst != RS_D3D9");
			return false;
		}

		HWND hWnd = (HWND)param->hwnd;
		if( NULL == hWnd)
		{
			Trace("Trace:RenderSystem::create	m_hWnd == 0\n");
			return false;
		}

		//创建d3d9对象

		m_pcsDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
		if( NULL == m_pcsDirect3D9)
		{
			Trace("Trace:RenderSystem::create	s_pcsDirect3D9 == 0\n");
			release();
			return false;
		}


		//创建D3D9设备驱动程序
		m_pcsD3D9DDriver = D3D9DriverCreater::create(this, D3DADAPTER_DEFAULT);
		if( NULL == m_pcsD3D9DDriver)
		{
			Trace("Trace:RenderSystem::create	m_pcsD3D9DDriver == 0\n");
			release();
			return false;
		}

		//创建D3D9设备对象
		m_pcsD3D9Device = D3D9DeviceCreater::create(
			this,
			m_pcsD3D9DDriver->getAdapterOrdinal(),
			m_pcsD3D9DDriver->getCurrentDisplayMode(),
			*param,
			0);
		if( NULL == m_pcsD3D9Device)
		{
			Trace("Trace:RenderSystem::create	m_pcsD3D9Device == 0\n");
			release();
			return false;		
		}

		//初始化设备对象状态
		if( !resetRenderState() )
		{
			Trace("Trace:RenderSystem::create	initializeRenderState FAIL\n");
			release();
			return false;		
		}

		//创建资源管理器
		m_pcsD3D9ResourceMgr =  D3D9ResourceManagerCreater::create();
		if( NULL == m_pcsD3D9ResourceMgr )
		{
			Trace("Trace:RenderSystem::create	m_pcsD3D9ResourceMgr == 0\n");
			release();
			return false;		
		}

		//创建纹理管理器
		m_pcsTextureMgr = TextureManagerD3D9Creater::create( this);
		if( NULL == m_pcsTextureMgr)
		{
			Trace("Trace:RenderSystem::create	m_pcsTextureMgr == 0\n");
			release();
			return false;
		}

		//创建渲染目标管理器
		m_pcsRenderTargetMgr = RenderTargetManagerD3D9Creater::create(this);
		if(NULL == m_pcsRenderTargetMgr )
		{
			Trace("Trace:RenderSystem::create	m_pcsRenderTargetMgr == 0\n");
			release();
			return false;			
		}

		//创建Buffer管理器
		m_pcsBufferMgr  = BufferManagerD3D9Creater::create(this);
		if( NULL == m_pcsBufferMgr)
		{
			Trace("Trace:RenderSystem::create	m_pcsBufferMgr == 0\n");
			release();
			return false;				
		}

		//创建数据流映射
		m_pcsStreamSourceMappings = StreamSourceMappingsCreater::create(this);
		if( NULL == m_pcsStreamSourceMappings)
		{
			Trace("Trace:RenderSystem::create	m_pcsStreamSourceMappings == 0\n");
			release();
			return false;		
		}

		//创建代理beginPrimitive和endPrimitive绘制方式
		m_pcsBeginEndPrimitiveProxy = BeginEndPrimitiveProxyCreater::create(this);
		if( NULL == m_pcsBeginEndPrimitiveProxy)
		{
			Trace("Trace:RenderSystem::create	m_pcsBeginEndPrimitiveProxy == 0\n");
			release();
			return false;	
		}

	
		//创建Shader管理器	
		m_pcsShaderProgramMgr = ShaderProgramManagerD3D9Creater::create(this);
		if( NULL == m_pcsShaderProgramMgr)
		{
			Trace("Trace:RenderSystem::create	m_pcsShaderProgramMgr == 0\n");
			release();
			return false;	
		}

		//硬件鼠标管理器
		m_pcsHardwareCursorMgr = HardwareCursorManagerD3D9Creater::create( this);
		if(NULL == m_pcsHardwareCursorMgr)
		{	
			//不支持硬件鼠标
			Trace("Trace:RenderSystem::create	m_pcsHardwareCursorMgr == 0\n");			
		}

		//设置当前渲染目标参数和渲染系统参数
		m_pcsRenderTargetMgr->getCurrentRenderTarget()->setCullingMode( xs::CULL_CLOCKWISE);


		//保存当前渲染系统
		m_vRenderSystem.push_back(this);

		//m_pcsD3D9Device->getDevice()->SetDialogBoxMode(TRUE);

		return true;
	}

	bool RenderSystemD3D9::switchDisplayMode(RenderEngineCreationParameters* param)
	{		
		if( NULL == param)
			return false;

		if (param->fullscreen)
		{
			// 全屏时可能导致客户端无响应，暂时没有找到原因，先屏蔽掉 [4/21/2011 zgz]
			//return false;
		}

		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getRenderTarget( (uint)( param->hwnd) );
		if(  0 == pRT )
			return false;

		//如果是默认的窗口，则重置设备
		if( pRT->isDefaultRenderTarget() )
		{
			//加锁
			m_pcsD3D9ResourceMgr->lockDeviceAccess();
			m_pcsD3D9ResourceMgr->lockResourceMgrAccess();
			m_pcsD3D9ResourceMgr->lockAllResourceAccess();

			//释放资源
			m_pcsD3D9DDriver->onDeviceLost();	
			m_pcsRenderTargetMgr->onDeviceLost();
			m_pcsHardwareCursorMgr->notifyOnDeviceLost();
			m_pcsD3D9ResourceMgr->notifyOnDeviceLost();

			//重置设备
			m_pcsD3D9Device->reset( param->fullscreen, param->w, param->h);

			//初始化渲染状态
			resetRenderState();

			//重建资源
			m_pcsD3D9ResourceMgr->notifyOnDeviceReset();
			m_pcsHardwareCursorMgr->notifyOnDeviceReset();
			m_pcsRenderTargetMgr->onDeviceReset();	
			m_pcsD3D9DDriver->onDeviceReset();

			//解锁
			m_pcsD3D9ResourceMgr->unlockAllResourceAccess();
			m_pcsD3D9ResourceMgr->unlockResourceMgrAccess();
			m_pcsD3D9ResourceMgr->unlockDeviceAccess();
			return true;
		}
		else
		{	
			return pRT->switchDisplayMode(param);
		}
	}

	void RenderSystemD3D9::getViewport(int &left,int &top,int &width,int &height)
	{
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT) pRT->getViewport(left, top, width, height);
	}

	void RenderSystemD3D9::setViewport(int left,int top,int width,int height)
	{
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT) pRT->setViewport(left, top, width, height);
	}

	void RenderSystemD3D9::setClearColor(const ColorValue& color)
	{
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT) pRT->setClearColor(color);
	}

	const ColorValue& RenderSystemD3D9::getClearColor()
	{
		static xs::ColorValue invalid;
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT)	return pRT->getClearColor();
		else return invalid;
	}

	void RenderSystemD3D9::setClearDepth(float depth)
	{
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT)	pRT->setClearDepth(depth);
	}

	void 		RenderSystemD3D9::setClearStencil(int stencil)
	{
		m_pcsRenderTargetMgr->getCurrentRenderTarget()->setClearStencil(stencil);
	}

	void RenderSystemD3D9::beginFrame(bool clearColorBuffer,bool clearDepthBuffer,bool clearStencilBuffer)
	{
		m_pcsRenderTargetMgr->getCurrentRenderTarget()->onBeginFrame(clearColorBuffer,clearDepthBuffer,clearStencilBuffer);
	}

	void RenderSystemD3D9::endFrame(int layer)
	{
		m_pcsRenderTargetMgr->getCurrentRenderTarget()->onEndFrame(layer);
		IDirect3DDevice9 * pDevice = m_pcsD3D9Device->getDevice();
		HRESULT hr = pDevice->TestCooperativeLevel();
		if( hr ==  D3DERR_DEVICENOTRESET ) //此时恢复设备
		{
			//加锁
			m_pcsD3D9ResourceMgr->lockDeviceAccess();
			m_pcsD3D9ResourceMgr->lockResourceMgrAccess();
			m_pcsD3D9ResourceMgr->lockAllResourceAccess();

			//释放资源
			m_pcsD3D9DDriver->onDeviceLost();			
			m_pcsRenderTargetMgr->onDeviceLost();
			m_pcsHardwareCursorMgr->notifyOnDeviceLost();
			m_pcsD3D9ResourceMgr->notifyOnDeviceLost();

			//重置设备
			if( m_pcsD3D9Device->reset() )
			{
				//初始化渲染状态
				resetRenderState();

				//重建资源
				m_pcsD3D9ResourceMgr->notifyOnDeviceReset();
				m_pcsHardwareCursorMgr->notifyOnDeviceReset();
				m_pcsRenderTargetMgr->onDeviceReset();		
				m_pcsD3D9DDriver->onDeviceReset();
			}	

			//解锁
			m_pcsD3D9ResourceMgr->unlockAllResourceAccess();
			m_pcsD3D9ResourceMgr->unlockResourceMgrAccess();
			m_pcsD3D9ResourceMgr->unlockDeviceAccess();
		}

	}

	void		RenderSystemD3D9::switchTo2D()
	{
		m_pcsRenderTargetMgr->getCurrentRenderTarget()->switchTo2D();
	}

	void 		RenderSystemD3D9::switchTo2D(int left,int top,int width,int height)
	{
		m_pcsRenderTargetMgr->getCurrentRenderTarget()->switchTo2D(left, top, width, height);
	}

	void		RenderSystemD3D9::switchTo3D()
	{
		m_pcsRenderTargetMgr->getCurrentRenderTarget()->switchTo3D();
	}


	bool		RenderSystemD3D9::is2dMode()
	{
		return m_pcsRenderTargetMgr->getCurrentRenderTarget()->is2dMode();
	}


	void		RenderSystemD3D9::setProjectionMatrix(const Matrix4& mtx)
	{
		//一定要有渲染目标
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT) pRT->setProjectionMatrix( mtx);
	}


	void		RenderSystemD3D9::setViewMatrix(const Matrix4& mtx)
	{
		//一定要有渲染目标
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT) pRT->setViewMatrix( mtx);
	}


	const Matrix4&	RenderSystemD3D9::getProjectionMatrix()
	{
		static Matrix4  invalid;
		//一定要有渲染目标
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT) return pRT->getProjectionMatrix();
		else return invalid;
	}

	const Matrix4&	RenderSystemD3D9::getViewMatrix()
	{
		static Matrix4  invalid;
		//一定要有渲染目标
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT) return pRT->getViewMatrix();
		else return invalid;
	}

	const Matrix4&	RenderSystemD3D9::getWorldMatrix()
	{
		static Matrix4  invalid;
		//一定要有渲染目标
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT)	return pRT->getWorldMatrix();
		else return invalid;
	}

	const Matrix4&		RenderSystemD3D9::getModelViewMatrix()
	{
		static Matrix4 invalid; 
		//一定要有渲染目标
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT) return pRT->getModelViewMatrix();
		else return invalid;
	}

	void		RenderSystemD3D9::setWorldMatrix(const Matrix4 &mtx)
	{
		//一定要有渲染目标
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT)	pRT->setWorldMatrix( mtx);
	}

	void		RenderSystemD3D9::setDepthBufferWriteEnabled(bool enabled)
	{
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT) pRT->setDepthBufferWriteEnabled(enabled);
	}

	bool		RenderSystemD3D9::isDepthBufferWriteEnabled()
	{
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT) return pRT->isDepthBufferWriteEnabled();
		else return true;
	}

	void		RenderSystemD3D9::setLightingEnabled(bool enabled)
	{
		//一定要有渲染目标
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT) pRT->setLightingEnabled( enabled);
	}

	bool		RenderSystemD3D9::isLightingEnabled()
	{
		//一定要有渲染目标
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT) return pRT->isLightingEnabled();
		else return false;
	}

	const FillMode&	RenderSystemD3D9::getFillMode()
	{
		return m_csRenderState.m_fillMode;
	}

	void		RenderSystemD3D9::setFillMode(const FillMode& sdl)
	{
		if( m_csRenderState.m_fillMode == sdl)
			return;
		m_csRenderState.m_fillMode = sdl;
		m_pcsD3D9Device->getDevice()->SetRenderState(D3DRS_FILLMODE, MappingsD3D9::getFillMode(sdl) );
	}

	void RenderSystemD3D9::setLineWidth(float width)
	{
		//D3D9不支持线宽
		m_csRenderState.m_lineWidth = width;
	}

	float RenderSystemD3D9::getLineWidth()
	{
		//D3D9不直接支持线宽
		return m_csRenderState.m_lineWidth;
	}

	bool		RenderSystemD3D9::isDepthBufferCheckEnabled()
	{
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT) return pRT->isDepthBufferCheckEnabled();
		else return false;
	}

	void RenderSystemD3D9::setDepthBufferCheckEnabled(bool enabled)
	{
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT) pRT->setDepthBufferCheckEnabled( enabled);
	}

	void RenderSystemD3D9::setAlphaFunction(const CompareFunction& cf,float value)
	{
		if( m_csRenderState.m_AlphaCompareFunction != cf)
		{
			m_csRenderState.m_AlphaCompareFunction = cf;
			m_pcsD3D9Device->getDevice()->SetRenderState(D3DRS_ALPHAFUNC,MappingsD3D9::getAlphaFunc(cf) );
		}

		xs::Math::clamp(value, 0.0f,1.0f);
		if( value != m_csRenderState.m_AlphaCompareValue )
		{
			m_csRenderState.m_AlphaCompareValue = value;
			uchar ucVal =  value * 255.0f;
			m_pcsD3D9Device->getDevice()->SetRenderState( D3DRS_ALPHAREF, (DWORD)ucVal);
		}
	}

	void RenderSystemD3D9::setAlphaFunction(const CompareFunction& cf,uchar value)
	{
		if( m_csRenderState.m_AlphaCompareFunction != cf)
		{
			m_csRenderState.m_AlphaCompareFunction = cf;
			m_pcsD3D9Device->getDevice()->SetRenderState(D3DRS_ALPHAFUNC,MappingsD3D9::getAlphaFunc(cf) );
		}
		float fval = (float)value/255.0f;
		if( fval != m_csRenderState.m_AlphaCompareValue )
		{
			m_csRenderState.m_AlphaCompareValue = fval;
			m_pcsD3D9Device->getDevice()->SetRenderState( D3DRS_ALPHAREF, (DWORD)value);
		}
	
	}

	void		RenderSystemD3D9::getAlphaFunction(CompareFunction& cf,float& value)
	{
		cf = m_csRenderState.m_AlphaCompareFunction;
		value = m_csRenderState.m_AlphaCompareValue;
	}

	bool		RenderSystemD3D9::alphaCheckEnabled()
	{
		return m_csRenderState.m_bAlphaCheckEnabled;
	}

	void RenderSystemD3D9::setAlphaCheckEnabled(bool bEnabled)
	{
		if( m_csRenderState.m_bAlphaCheckEnabled == bEnabled)
			return;

		m_csRenderState.m_bAlphaCheckEnabled = bEnabled;
		m_pcsD3D9Device->getDevice()->SetRenderState(D3DRS_ALPHATESTENABLE,bEnabled?TRUE:FALSE);
	}

	void RenderSystemD3D9::setDepthBufferFunction(const CompareFunction& cf)
	{
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT) pRT->setDepthBufferFunction(cf);
	}

	const CompareFunction& RenderSystemD3D9::getDepthBufferFunction()
	{
		static  CompareFunction cf;
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT) return pRT->getDepthBufferFunction();
		else return cf;
	}

	const CullingMode& RenderSystemD3D9::getCullingMode()
	{
		return m_pcsRenderTargetMgr->getCurrentRenderTarget()->getCullingMode();
	}

	void RenderSystemD3D9::setEdgeFlag(bool bEdgeFlag)
	{
		m_csRenderState.m_edgeFlag = bEdgeFlag;
		m_pcsBeginEndPrimitiveProxy->setEdgeFlag(bEdgeFlag);
	}

	bool RenderSystemD3D9::getEdgeFlag()
	{
		return m_csRenderState.m_edgeFlag;
	}

	void RenderSystemD3D9::setCullingMode(const CullingMode& cm)
	{
		m_pcsRenderTargetMgr->getCurrentRenderTarget()->setCullingMode(cm);
	}

	void RenderSystemD3D9::getSceneBlending(SceneBlendFactor& src,SceneBlendFactor& dst)
	{
		src = m_csRenderState.m_alphaBlendSrc;
		dst = m_csRenderState.m_alphaBlendDst;
	}

	void RenderSystemD3D9::setSceneBlending(const SceneBlendFactor& sourceFactor,const SceneBlendFactor& destFactor)
	{
		if( m_csRenderState.m_alphaBlendSrc == sourceFactor && m_csRenderState.m_alphaBlendDst == destFactor)
			return;

		m_csRenderState.m_alphaBlendSrc = sourceFactor;
		m_csRenderState.m_alphaBlendDst = destFactor;

		m_pcsD3D9Device->getDevice()->SetRenderState(D3DRS_SRCBLEND, MappingsD3D9::getBlendFactor(sourceFactor) );
		m_pcsD3D9Device->getDevice()->SetRenderState(D3DRS_DESTBLEND, MappingsD3D9::getBlendFactor(destFactor) );

		if(sourceFactor == SBF_ONE && destFactor == SBF_ZERO)
		{
			m_pcsD3D9Device->getDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
		else
		{
			m_pcsD3D9Device->getDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		}

	}

	IBufferManager*		RenderSystemD3D9::getBufferManager()
	{
		return m_pcsBufferMgr;
	}

	IShaderProgramManager* RenderSystemD3D9::getShaderProgramManager()
	{
		return m_pcsShaderProgramMgr;
	}

	bool RenderSystemD3D9::getDisplayMode(RenderEngineCreationParameters & createParam)
	{
		RenderTargetD3D9 * pRT  = m_pcsRenderTargetMgr->getRenderTarget((uint)createParam.hwnd);
		if( 0 == pRT )
			return false;

		return pRT->getDisplayMode(createParam);

	}

	void RenderSystemD3D9::bindCurrentShaderProgram(IShaderProgram* pShaderProgram,bool setShaderConst)
	{
		if(pShaderProgram)
		{
			pShaderProgram->bind();
		}
	}


	void RenderSystemD3D9::clearFrameBuffer(bool clearColorBuffer,bool clearDepthBuffer ,bool clearStencilBuffer)
	{
		m_pcsRenderTargetMgr->getCurrentRenderTarget()->clearFrameBuffer(clearColorBuffer, clearDepthBuffer, clearStencilBuffer);
	}

	ITextureManager*		RenderSystemD3D9::getTextureManager()
	{
		return m_pcsTextureMgr;
	}

	TextureAddressingMode	RenderSystemD3D9::getTextureWrapS(ushort unit)
	{
		Assert( unit < m_csRenderState.s_maxTexureUnit );
		return m_csRenderState.m_addressU[unit];
	}

	TextureAddressingMode	RenderSystemD3D9::getTextureWrapT(ushort unit)
	{
		Assert( unit < m_csRenderState.s_maxTexureUnit );
		return m_csRenderState.m_addressV[unit];
	}

	void RenderSystemD3D9::setTextureWrapS(ushort unit,const TextureAddressingMode& tam)
	{
		Assert( unit < m_csRenderState.s_maxTexureUnit);
		if (unit >= m_csRenderState.s_maxTexureUnit)
		{
			return;
		}
		if( m_csRenderState.m_addressU[unit] == tam ) return;
		if( MappingsD3D9::getAddressMode( tam) == MappingsD3D9::getAddressMode( m_csRenderState.m_addressU[unit]) ) return;
		m_csRenderState.m_addressU[unit] = tam;
		IDirect3DDevice9 * pd3dDevice = m_pcsD3D9Device->getDevice();
		pd3dDevice->SetSamplerState(unit, D3DSAMP_ADDRESSU, MappingsD3D9::getAddressMode( tam) );
	}

	void RenderSystemD3D9::setTextureWrapT(ushort unit,const TextureAddressingMode& tam)
	{
		Assert( unit < m_csRenderState.s_maxTexureUnit );
		if (unit >= m_csRenderState.s_maxTexureUnit)
		{
			return;
		}
		if( m_csRenderState.m_addressV[unit] == tam) return;
		if( MappingsD3D9::getAddressMode(tam) == MappingsD3D9::getAddressMode(m_csRenderState.m_addressV[unit]) ) return;
		m_csRenderState.m_addressV[unit] = tam;
		IDirect3DDevice9 * pd3dDevice = m_pcsD3D9Device->getDevice();
		pd3dDevice->SetSamplerState(unit, D3DSAMP_ADDRESSV, MappingsD3D9::getAddressMode( tam) );

	}

	void RenderSystemD3D9::setTextureCoordCalculation(ushort unit,TexCoordCalcMethod m)
	{
		// to be continued
	}

	void RenderSystemD3D9::setTextureWrap(ushort unit,const TextureAddressingMode& tamS,const TextureAddressingMode& tamT)
	{
		setTextureWrapS(unit, tamS);
		setTextureWrapT(unit, tamT);
	}

	void 		RenderSystemD3D9::setMultiTexcoord(ushort unit,const Vector2& texcoord)
	{
		if( unit >= m_csRenderState.s_maxTexureUnit)
			return;

		m_csRenderState.m_texcoord[unit] = texcoord;
		m_pcsBeginEndPrimitiveProxy->setMultiTexcoord( unit, texcoord);
	}

	const Vector2&		RenderSystemD3D9::getMultiTexcoord(ushort unit)
	{
		Assert( unit <= m_csRenderState.s_maxTexureUnit);
		return m_csRenderState.m_texcoord[unit];
	}


	void		RenderSystemD3D9::setTexcoord(const Vector2& texcoord)
	{
		m_csRenderState.m_texcoord[0] = texcoord;
		m_pcsBeginEndPrimitiveProxy->setTexcoord(texcoord);
	}


	void		RenderSystemD3D9::setColor(const ColorValue& color)
	{
		m_csRenderState.m_color = color;
		m_pcsBeginEndPrimitiveProxy->setColor( color);
	}


	const ColorValue&		RenderSystemD3D9::getColor()
	{
		return m_csRenderState.m_color;
	}


	void		RenderSystemD3D9::setNormal(const Vector3& normal)
	{
		m_csRenderState.m_normal = normal;
		m_pcsBeginEndPrimitiveProxy->setNormal( normal);
	}

	const Vector3&		RenderSystemD3D9::getNormal()
	{
		return m_csRenderState.m_normal;
	}


	void		RenderSystemD3D9::sendVertex(const Vector2& vertex)
	{
		m_pcsBeginEndPrimitiveProxy->sendVertex( vertex);
	}


	void		RenderSystemD3D9::sendVertex(const Vector3& vertex)
	{
		m_pcsBeginEndPrimitiveProxy->sendVertex(vertex);
	}


	void		RenderSystemD3D9::sendVertex(const Vector4& vertex)
	{
		m_pcsBeginEndPrimitiveProxy->sendVertex(vertex);
	}

	void		RenderSystemD3D9::beginPrimitive(PrimitiveType pt)
	{
		m_pcsBeginEndPrimitiveProxy->beginPrimitive(pt);
	}

	void		RenderSystemD3D9::endPrimitive()
	{
		m_pcsBeginEndPrimitiveProxy->endPrimitive();
	}

	void		RenderSystemD3D9::render(const RenderOperation& op)
	{
	}

	void		RenderSystemD3D9::render(const RenderDirect& rd)
	{
	}

	void RenderSystemD3D9::setTexture(ushort unit,ITexture* pTexture)
	{	
		if( unit >= m_csRenderState.s_maxTexureUnit)
			return;

		IDirect3DDevice9 * pd3ddevice = m_pcsD3D9Device->getDevice();
		TextureD3D9 * pD3DTex = static_cast<TextureD3D9*>(pTexture);
		IDirect3DTexture9 * pRealTex = NULL;

		//关闭纹理映射
		if(  NULL == pD3DTex || (pRealTex = pD3DTex->getD3DTexture() ) == NULL  )
		{
			if( m_csRenderState.m_TSSEnable[unit] )
			{
				m_csRenderState.m_TSSEnable[unit] = false;
				//dx文档说： Alpha operations cannot be disabled when color operations are enabled
				pd3ddevice->SetTextureStageState(unit, D3DTSS_COLOROP, D3DTOP_DISABLE);
				pd3ddevice->SetTextureStageState(unit, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
				pd3ddevice->SetTexture(unit,0);				
			}
			return;
		}
		else //开启纹理映射
		{
			if(  !m_csRenderState.m_TSSEnable[unit] )
			{
				m_csRenderState.m_TSSEnable[unit] = true;
				//dx文档说： Alpha operations cannot be disabled when color operations are enabled
				pd3ddevice->SetTextureStageState(unit, D3DTSS_ALPHAOP, MappingsD3D9::getTextureStageOperator(m_csRenderState.m_TSSAlphaOP[unit]) );
				pd3ddevice->SetTextureStageState(unit, D3DTSS_COLOROP, MappingsD3D9::getTextureStageOperator(m_csRenderState.m_TSSColorOP[unit]) );
			}
			pd3ddevice->SetTexture( unit, pRealTex);
			pD3DTex->applyFilterState(unit);
			return;
		}	
	}

	const ColorValue& RenderSystemD3D9::getAmbientLight()
	{
		return m_csRenderState.m_ambientLight;
	}

	void		RenderSystemD3D9::setAmbientLight(const ColorValue& color)
	{
		if( m_csRenderState.m_ambientLight == color)
			return;
		
		m_csRenderState.m_ambientLight = color;
		m_pcsD3D9Device->getDevice()->SetRenderState(D3DRS_AMBIENT,(DWORD)color.getAsARGB() );
	}


	void		RenderSystemD3D9::setTexcoordVertexBuffer(ushort unit,IVertexBuffer* vertexBuffer,uint start,uint stride)
	{	
		//用数据流映射管理
		m_pcsStreamSourceMappings->setTexcoordVertexBuffer(unit, vertexBuffer,start);
	}


	void		RenderSystemD3D9::setVertexVertexBuffer(IVertexBuffer* vertexBuffer,uint start,uint stride)
	{
		//用数据流映射管理
		m_pcsStreamSourceMappings->setVertexVertexBuffer(vertexBuffer,start);
	}


	void		RenderSystemD3D9::setDiffuseVertexBuffer(IVertexBuffer* vertexBuffer,uint start,uint stride)
	{
		//用数据流映射管理
		m_pcsStreamSourceMappings->setDiffuseVertexBuffer(vertexBuffer, start);
	}


	void		RenderSystemD3D9::setSpecularVertexBuffer(IVertexBuffer* vertexBuffer,uint start,uint stride)
	{
		//用数据流映射管理
		m_pcsStreamSourceMappings->setSpecularVertexBuffer(vertexBuffer, start);
	}

	void		RenderSystemD3D9::setBlendWeightVertexBuffer(IVertexBuffer* pVB, uint start)
	{
		//用数据流映射管理
		m_pcsStreamSourceMappings->setBlendWeightVertexBuffer( pVB, start);
	
	}

	void		RenderSystemD3D9::setBlendIndicesVertexBuffer(IVertexBuffer* pVB, uint start)
	{
		//用数据流映射管理
		m_pcsStreamSourceMappings->setBlendIndicesVertexBuffer( pVB, start);
	}


	void		RenderSystemD3D9::setNormalVertexBuffer(IVertexBuffer* vertexBuffer,uint start,uint stride)
	{
		//用数据流映射管理
		m_pcsStreamSourceMappings->setNormalVertexBuffer( vertexBuffer, start);
	}


	void		RenderSystemD3D9::setIndexBuffer(IIndexBuffer* indexBuffer)
	{
		//用数据流映射管理
		m_pcsStreamSourceMappings->setIndexBuffer( indexBuffer);
	}

	void RenderSystemD3D9::drawRangeIndexedPrimitive(PrimitiveType pt,IndexType indexType,uint ui32IndexStart,uint ui32IndexCount,uint ui32VertexStart,uint ui32VertexEnd)
	{
		//PT_LINE_LOOP被PT_LINE_STRIP取代
		//PT_QUAD_STRIP和PT_QUADS不支持
		if( pt == PT_QUAD_STRIP
			|| pt == PT_QUADS
			|| pt == PT_LINE_LOOP )
			return;

		IDirect3DDevice9 * pD3DDevice = m_pcsD3D9Device->getDevice();
		//设置顶点声明
		IDirect3DVertexDeclaration9 * pDcl = m_pcsStreamSourceMappings->getVertexDeclaration();
		if( NULL == pDcl)
			return;
		pD3DDevice->SetVertexDeclaration( pDcl);
		//开始绘制
		pD3DDevice->DrawIndexedPrimitive(MappingsD3D9::getPrimptiveType( pt),
			0,
			0,
			ui32VertexEnd,
			(UINT)ui32IndexStart,
			MappingsD3D9::getPrimitiveCount(pt,ui32IndexCount) );
	}

	void RenderSystemD3D9::drawIndexedPrimitive(PrimitiveType primitiveType,uint ui32IndexCount,IndexType indexType,void* pdata)
	{
		//PT_QUAD_STRIP,PT_LINE_LOOP, PT_QUADS不支持
		if( primitiveType == PT_QUAD_STRIP
			|| primitiveType == PT_QUADS
			|| primitiveType == PT_LINE_LOOP )
			return;

		IDirect3DDevice9 * pD3DDevice = m_pcsD3D9Device->getDevice();
		//设置顶点声明
		IDirect3DVertexDeclaration9 * pDcl = m_pcsStreamSourceMappings->getVertexDeclaration();
		if( NULL == pDcl)
			return;
		pD3DDevice->SetVertexDeclaration( pDcl);
		//开始绘制
		pD3DDevice->DrawIndexedPrimitive(MappingsD3D9::getPrimptiveType( primitiveType),
			0,
			0,
			m_pcsStreamSourceMappings->getNumVertices(),//这里有个bug，函数getNumVertices()获得的数据不一定是准确。
			0,
			MappingsD3D9::getPrimitiveCount(primitiveType,ui32IndexCount) );
	}

	void RenderSystemD3D9::drawPrimitive(PrimitiveType primitiveType,uint ui32VertexStart,uint ui32VertexCount)
	{
		//PT_QUAD_STRIP,PT_LINE_LOOP,PT_QUADS不支持
		if( primitiveType == PT_QUAD_STRIP
			|| primitiveType == PT_QUADS)
			return;

		HRESULT hr;

		IDirect3DDevice9 * pD3DDevice = m_pcsD3D9Device->getDevice();
		//设置顶点声明
		IDirect3DVertexDeclaration9 * pDcl = m_pcsStreamSourceMappings->getVertexDeclaration();
		if( NULL == pDcl)
			return;
		hr = pD3DDevice->SetVertexDeclaration( pDcl);
		//开始绘制
		hr = pD3DDevice->DrawPrimitive( MappingsD3D9::getPrimptiveType(primitiveType),
			ui32VertexStart,MappingsD3D9::getPrimitiveCount(primitiveType, ui32VertexCount) );
	}

	void RenderSystemD3D9::setFog(FogMode mode,const ColorValue& color,float expDensity,float linearStart,float linearEnd)
	{
		//暂时只使用顶点雾化
		IDirect3DDevice9 * pD3Ddevice = m_pcsD3D9Device->getDevice();
		if( mode != m_csRenderState.m_fogMode)
		{
			m_csRenderState.m_fogMode = mode;
			pD3Ddevice->SetRenderState(D3DRS_FOGVERTEXMODE, MappingsD3D9::getFogMode( mode) );
		}

		if( m_csRenderState.m_fogColor != color)
		{
			m_csRenderState.m_fogColor = color;
			pD3Ddevice->SetRenderState(D3DRS_FOGCOLOR,(DWORD)color.getAsARGB() );
		}

		if( m_csRenderState.m_fogExpDensity != expDensity)
		{
			m_csRenderState.m_fogExpDensity = expDensity;
			pD3Ddevice->SetRenderState( D3DRS_FOGDENSITY, F2DW( expDensity) );
		}

		if(m_csRenderState.m_fogLinearStart != linearStart)
		{
			m_csRenderState.m_fogLinearStart = linearStart;
			pD3Ddevice->SetRenderState( D3DRS_FOGSTART, F2DW( linearStart) );
		}

		if( m_csRenderState.m_fogLinearEnd != linearEnd)
		{
			m_csRenderState.m_fogLinearEnd = linearEnd;
			pD3Ddevice->SetRenderState( D3DRS_FOGEND, F2DW( linearEnd) );
		}
	}

	void	RenderSystemD3D9::enableFog(bool enable)
	{
		if( m_csRenderState.m_bEnableFog == enable)
			return;
		m_csRenderState.m_bEnableFog = enable;
		m_pcsD3D9Device->getDevice()->SetRenderState(D3DRS_FOGENABLE, enable?TRUE:FALSE);
	}

	bool	RenderSystemD3D9::isFogEnabled()
	{
		return m_csRenderState.m_bEnableFog;
	}

	void		RenderSystemD3D9::setLight(ushort unit,Light* pLight)
	{
		if( unit >= m_csRenderState.s_maxLight )
			return;

		if( NULL == pLight)//禁用光照
		{
			m_csRenderState.m_light[unit].enable(false);
			m_pcsD3D9Device->getDevice()->LightEnable(unit, FALSE);
		}
		else
		{
			//拷贝数据
			MappingsD3D9::getLight( *pLight, m_csRenderState.m_d3d9light[unit]);
			MappingsD3D9::getLight( m_csRenderState.m_d3d9light[unit], m_csRenderState.m_light[unit]);
			m_csRenderState.m_light[unit].enable( pLight->isEnabled() );

			m_pcsD3D9Device->getDevice()->SetLight(unit, &m_csRenderState.m_d3d9light[unit]);
			m_pcsD3D9Device->getDevice()->LightEnable(unit, pLight->isEnabled()?TRUE:FALSE);					
		}
	}

	void		RenderSystemD3D9::setPointSize(float size)
	{
		if( m_csRenderState.m_pointSize == size)
			return;
		m_csRenderState.m_pointSize = size;
		m_pcsD3D9Device->getDevice()->SetRenderState(D3DRS_POINTSIZE, F2DW(size));
	}

	void RenderSystemD3D9::setTrackingColorEnum(TrackVertexColorEnum tracking)
	{
		if( m_csRenderState.m_trackColor == tracking)
			return;

		m_csRenderState.m_trackColor = tracking;
		if( tracking != TVC_NONE	)
		{
			IDirect3DDevice9 * pd3ddevice = m_pcsD3D9Device->getDevice();
			pd3ddevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, (tracking&TVC_AMBIENT)?D3DMCS_COLOR1:D3DMCS_MATERIAL);
			pd3ddevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, (tracking&TVC_DIFFUSE)?D3DMCS_COLOR1:D3DMCS_MATERIAL);
			pd3ddevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, (tracking&TVC_SPECULAR)?D3DMCS_COLOR1:D3DMCS_MATERIAL);
			pd3ddevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, (tracking&TVC_EMISSIVE)?D3DMCS_COLOR1:D3DMCS_MATERIAL);
			pd3ddevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);
		}
		else
		{
			m_pcsD3D9Device->getDevice()->SetRenderState(D3DRS_COLORVERTEX, FALSE);
		}
	}

	const ColorValue&	RenderSystemD3D9::getSurfaceDiffuse()
	{
		return m_csRenderState.m_diffuse;
	}

	void RenderSystemD3D9::setSurfaceDiffuse(const ColorValue &diffuse)
	{
		m_csRenderState.m_diffuse = diffuse;
		m_csRenderState.m_ambient = diffuse;
		m_csRenderState.m_material.Diffuse = D3DXCOLOR(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
		m_csRenderState.m_material.Ambient = D3DXCOLOR(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
		m_pcsD3D9Device->getDevice()->SetMaterial(&m_csRenderState.m_material);	
	}

	//-----------------------------------------------------------------------------
	void RenderSystemD3D9::setSurfaceParams(
		const ColorValue &ambient,
		const ColorValue &diffuse,
		const ColorValue &specular,
		const ColorValue &emissive,
		float shininess)
	{
		bool needUpdate = false;
		if( m_csRenderState.m_ambient != ambient)
		{
			m_csRenderState.m_ambient = ambient;
			m_csRenderState.m_material.Ambient = D3DXCOLOR(ambient.r, ambient.g, ambient.b, ambient.a);
			needUpdate = true;
		}

		if( m_csRenderState.m_diffuse != diffuse)
		{
			m_csRenderState.m_diffuse = diffuse;
			m_csRenderState.m_material.Diffuse = D3DXCOLOR( diffuse.r, diffuse.g, diffuse.b, diffuse.a);
			needUpdate = true;
		}

		if( m_csRenderState.m_specular != specular)
		{
			m_csRenderState.m_specular = specular;
			m_csRenderState.m_material.Specular = D3DXCOLOR( specular.r, specular.g, specular.b, specular.a);
			needUpdate = true;
		}

		if( m_csRenderState.m_emissive != emissive)
		{
			m_csRenderState.m_emissive = emissive;
			m_csRenderState.m_material.Emissive = D3DXCOLOR(emissive.r, emissive.g, emissive.b, emissive.a);
			needUpdate = true;
		}

		if(m_csRenderState. m_shininess != shininess )
		{
			m_csRenderState.m_shininess = shininess;
			m_csRenderState.m_material.Power = shininess;
			needUpdate = true;
		}

		if( needUpdate)
		{
			m_pcsD3D9Device->getDevice()->SetMaterial( &m_csRenderState.m_material);
		}
	}
	void RenderSystemD3D9::setClipPlane(uint index,const Plane*pPlane)
	{
	}

	IVideoObject * RenderSystemD3D9::createVideoObject(const char * pszFilename, uint type, uint imgWidth, uint imgHeight )
	{
		return 0;
		/*
		switch( type)
		{
		case EVOT_AVI:
			{
				VFWAVIObjectD3D9 * pObj = new VFWAVIObjectD3D9();
				if( !pObj->create(this, pszFilename, imgWidth, imgHeight) )
				{
					pObj->release();
					return 0;
				}
				else
					return pObj;
			}
			break;
		default:
			return 0;
		}
		*/
	}

	void RenderSystemD3D9::teapot(float size,const ColorValue& color)
	{
		if( NULL == m_pTeapot) 
		{
			//生成茶壶Mesh
			D3DXCreateTeapot(m_pcsD3D9Device->getDevice(), &m_pTeapot, NULL);
			if( NULL == m_pTeapot )
				return;
		}

		CullingMode cm = getCullingMode();
		setCullingMode(CULL_COUNTERCLOCKWISE);
		const xs::Matrix4 & mtxWorld = getWorldMatrix();
		xs::Matrix4 mtxScale = xs::Matrix4::IDENTITY;
		mtxScale.setScale(xs::Vector3(size, size, size) );
		mtxScale  = mtxWorld * mtxScale;
		setWorldMatrix( mtxScale);
		m_pTeapot->DrawSubset(0);
		setWorldMatrix( mtxScale);
		setCullingMode( cm);
	}

	void RenderSystemD3D9::line(const Point& ptFirst,const Point& ptSecond,const ColorValue& color)
	{
		struct  CustomVertex
		{
			float x,y,z;
			DWORD diffuse;
		};
		xs::ARGB d3dcolor = color.getAsARGB();
		CustomVertex pVertex[2] = 
		{
			{ptFirst.x, ptFirst.y, 0.0f, d3dcolor},
			{ptSecond.x, ptSecond.y, 0.0f, d3dcolor},
		};
		IDirect3DDevice9* pDevice = m_pcsD3D9Device->getDevice();
		pDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
		pDevice->DrawPrimitiveUP(D3DPT_LINELIST,1, (void*)pVertex, sizeof(CustomVertex));
	}

	void RenderSystemD3D9::rectangle(const Rect& rc,const ColorValue& color)
	{
		struct CustomVertex
		{
			float x,y,z;
			DWORD color;
		};

		xs::ARGB d3dcolor = color.getAsARGB();
		CustomVertex pVertex[5] = 
		{
			{rc.left, rc.top, 0.0f, d3dcolor,},
			{rc.right, rc.top, 0.0f, d3dcolor,},
			{rc.right, rc.bottom,0.0f, d3dcolor},
			{rc.left, rc.bottom, 0.0f, d3dcolor},
			{rc.left, rc.top, 0.0f, d3dcolor,},
		};

		IDirect3DDevice9* pDevice = m_pcsD3D9Device->getDevice();
		pDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
		pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, pVertex, sizeof(CustomVertex));
	}

	void RenderSystemD3D9::rectangle(const Rect& rc,const ColorValue& cLeftTop,const ColorValue& cLeftBottom,const ColorValue& cRightTop,const ColorValue& cRightBottom)
	{
		struct CustomVertex
		{
			float x,y,z;
			DWORD color;
		};

		xs::ARGB	cllt = cLeftTop.getAsARGB();
		xs::ARGB	clrt = cRightTop.getAsARGB();
		xs::ARGB	clrb = cRightBottom.getAsARGB();
		xs::ARGB	cllb = cLeftBottom.getAsARGB();
		CustomVertex pVertex[5] = 
		{
			{rc.left, rc.top, 0.0f, cllt,},
			{rc.right, rc.top, 0.0f, clrt,},
			{rc.right, rc.bottom,0.0f, clrb},
			{rc.left, rc.bottom, 0.0f, cllb},
			{rc.left, rc.top, 0.0f, cllt,},
		};

		IDirect3DDevice9* pDevice = m_pcsD3D9Device->getDevice();
		pDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
		pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, pVertex, sizeof(CustomVertex));
	}

	void RenderSystemD3D9::rectangle(const Rect& r,ITexture* pTexture, const Vector2 & vLetfTop, const Vector2 & vLeftBottom, const Vector2 & vRightBottom, const Vector2 & RightTop)
	{
		struct CustomVertex
		{
			float x,y,z;
			float u,v;
		};
		
		Rect rc(r.left-0.5f, r.top-0.5f, r.right-0.5f, r.bottom -0.5f);
		CustomVertex pVertex[4] = 
		{
			{rc.left, rc.top, 0.0f, vLetfTop.x, vLetfTop.y, },
			{rc.right, rc.top, 0.0f, RightTop.x, RightTop.y, },
			{rc.left, rc.bottom, 0.0f, vLeftBottom.x, vLeftBottom.y, },
			{rc.right, rc.bottom, 0.0f, vRightBottom.x, vRightBottom.y,},
		};

		setTexture(0, pTexture);
		IDirect3DDevice9* pDevice = m_pcsD3D9Device->getDevice();
		pDevice->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1);
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2, pVertex, sizeof(CustomVertex));
		setTexture(0,0);	
	}

	void RenderSystemD3D9::point(const Point& pt,const ColorValue& color)
	{
		struct CustomVertex
		{
			float x,y,z;
			DWORD color;
		};

		xs::ARGB d3dcolor = color.getAsARGB();

		CustomVertex pVertex[1] = 
		{
			{pt.x, pt.y, 0.0f,d3dcolor, },
		};

		IDirect3DDevice9* pDevice = m_pcsD3D9Device->getDevice();
		pDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
		pDevice->DrawPrimitiveUP(D3DPT_POINTLIST,1, pVertex, sizeof(CustomVertex));
	}

	void		RenderSystemD3D9::rectangle(const Rect& r,ITexture* pTexture)
	{
		struct CustomVertex
		{
			float x,y,z;
			float u,v;
		};

		Rect rc(r.left-0.5f, r.top-0.5f, r.right-0.5f, r.bottom-0.5f);
		CustomVertex pVertex[4] = 
		{
			{rc.left, rc.top, 0.0f, 0, 0, },
			{rc.right, rc.top, 0.0f, 1, 0, },
			{rc.left, rc.bottom, 0.0f, 0, 1, },
			{rc.right, rc.bottom, 0.0f, 1,1,},
		};

		setTexture(0, pTexture);
		IDirect3DDevice9* pDevice = m_pcsD3D9Device->getDevice();
		pDevice->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1);
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2, pVertex, sizeof(CustomVertex));
		setTexture(0,0);

	}

	void		RenderSystemD3D9::box(const Rect& r,const ColorValue& color)
	{
		struct CustomVertex
		{
			float x,y,z;
			DWORD color;
		};

		Rect rc(r.left, r.top-1, r.right+1, r.bottom);
		xs::ARGB d3dcolor = color.getAsARGB();

		CustomVertex pVertex[4] = 
		{
			{rc.left, rc.top, 0.0f,d3dcolor, },
			{rc.right, rc.top, 0.0f, d3dcolor, },
			{rc.left, rc.bottom, 0.0f, d3dcolor, },
			{rc.right, rc.bottom, 0.0f, d3dcolor,},
		};

		IDirect3DDevice9* pDevice = m_pcsD3D9Device->getDevice();
		pDevice->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2, pVertex, sizeof(CustomVertex));	
	}


	Vector3		RenderSystemD3D9::unproject(const Vector3& v)
	{
		RenderTargetD3D9 * pCurRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( NULL == pCurRT )
			return Vector3(0.0f,0.0f,0.0f);
		else 
			return pCurRT->unproject( v);
	}

	Vector3		RenderSystemD3D9::project(const Vector3& v)
	{
		RenderTargetD3D9 * pCurRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( NULL == pCurRT )
			return Vector3(0.0f,0.0f,0.0f);
		else 
			return pCurRT->project( v);
	}


	void 		RenderSystemD3D9::setVerticalSync(bool vsync)
	{
		//加锁
		m_pcsD3D9ResourceMgr->lockDeviceAccess();
		m_pcsD3D9ResourceMgr->lockResourceMgrAccess();
		m_pcsD3D9ResourceMgr->lockAllResourceAccess();

		//释放资源
		m_pcsD3D9DDriver->onDeviceLost();	
		m_pcsRenderTargetMgr->onDeviceLost();
		m_pcsHardwareCursorMgr->notifyOnDeviceLost();
		m_pcsD3D9ResourceMgr->notifyOnDeviceLost();

		//重置设备
		m_pcsD3D9Device->reset( vsync);

		//初始化渲染状态
		resetRenderState();

		//重建资源
		m_pcsD3D9ResourceMgr->notifyOnDeviceReset();
		m_pcsHardwareCursorMgr->notifyOnDeviceReset();
		m_pcsRenderTargetMgr->onDeviceReset();
		m_pcsD3D9DDriver->onDeviceReset();

		//解锁
		m_pcsD3D9ResourceMgr->unlockAllResourceAccess();
		m_pcsD3D9ResourceMgr->unlockResourceMgrAccess();
		m_pcsD3D9ResourceMgr->unlockDeviceAccess();
	}

	//---------------------------------------------------------------------
	void  RenderSystemD3D9::setStencilCheckEnabled(bool enabled)
	{
		if( m_csRenderState.m_bEnableStencilTest == enabled)
			return;

		m_csRenderState.m_bEnableStencilTest = enabled;
		m_pcsD3D9Device->getDevice()->SetRenderState(D3DRS_STENCILENABLE, enabled?TRUE:FALSE);
	}

	//---------------------------------------------------------------------
	void  RenderSystemD3D9::setStencilBufferParams(
		CompareFunction func, 
		uint refValue,
		uint mask,
		StencilOperation stencilFailOp,
		StencilOperation depthFailOp,
		StencilOperation passOp)
	{
		IDirect3DDevice9 * pd3ddevice= m_pcsD3D9Device->getDevice();
		if( func != m_csRenderState.m_stencilFunc)
		{
			m_csRenderState.m_stencilFunc = func;
			pd3ddevice->SetRenderState(D3DRS_STENCILFUNC, MappingsD3D9::getStencilFunc(func));
		}

		if( refValue != m_csRenderState.m_stencilRefValue)
		{
			m_csRenderState.m_stencilRefValue = refValue;
			pd3ddevice->SetRenderState(D3DRS_STENCILREF, refValue);
		}

		if( mask != m_csRenderState.m_stencilMask)
		{
			m_csRenderState.m_stencilMask = mask;
			pd3ddevice->SetRenderState(D3DRS_STENCILMASK, mask);
			pd3ddevice->SetRenderState(D3DRS_STENCILWRITEMASK,mask);
		}

		if( stencilFailOp !=  m_csRenderState.m_stencilFailOp )
		{
			m_csRenderState.m_stencilFailOp = stencilFailOp;
			pd3ddevice->SetRenderState(D3DRS_STENCILFAIL, MappingsD3D9::getStencilOp(stencilFailOp));
		}

		if( depthFailOp != m_csRenderState.m_depthFailOp)
		{
			m_csRenderState.m_depthFailOp  = depthFailOp;
			pd3ddevice->SetRenderState(D3DRS_STENCILZFAIL, MappingsD3D9::getStencilOp(depthFailOp) );
		}

		if( passOp != m_csRenderState.m_passOp)
		{
			m_csRenderState.m_passOp = passOp;
			pd3ddevice->SetRenderState(D3DRS_STENCILPASS, MappingsD3D9::getStencilOp(passOp) );
		}
	}

	void RenderSystemD3D9::setColorBufferWriteEnabled(bool red, bool green, bool blue, bool alpha)
	{
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( pRT) pRT->setColorBufferWriteEnabled( red, green, blue, alpha);
	}

	void RenderSystemD3D9::beginSelection(int x,int y,uint variance)
	{
	}

	void RenderSystemD3D9::endSelection(int& num,int* pBuffer)
	{
	}

	void RenderSystemD3D9::setSelectionIndex(int index)
	{
	}

	const RenderMode&	RenderSystemD3D9::getRenderMode()
	{
		static RenderMode rm;
		return rm;
	}

	RenderSystemType RenderSystemD3D9::getRenderSystemType()
	{
		return RS_D3D9;
	}

	bool RenderSystemD3D9::resetRenderState()
	{
		if( NULL == m_pcsD3D9Device )
			return false;

		IDirect3DDevice9 * pd3ddevice = m_pcsD3D9Device->getDevice();
		if( NULL == pd3ddevice)
			return false;

		//图元状态
		//d3d9不直接支持线宽
		pd3ddevice->SetRenderState(D3DRS_POINTSIZE, F2DW( m_csRenderState.m_pointSize) );

		//填充模式
		pd3ddevice->SetRenderState(D3DRS_FILLMODE, MappingsD3D9::getFillMode(m_csRenderState.m_fillMode) );

		//beginPrimitive 和 endPrimitive 相关状态
		//不用设置

		//当前材质
		pd3ddevice->SetMaterial(&m_csRenderState.m_material);
		if( m_csRenderState.m_trackColor != TVC_NONE	)
		{
			pd3ddevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);
			pd3ddevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, (m_csRenderState.m_trackColor&TVC_AMBIENT)?D3DMCS_COLOR1:D3DMCS_MATERIAL);
			pd3ddevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, (m_csRenderState.m_trackColor&TVC_DIFFUSE)?D3DMCS_COLOR1:D3DMCS_MATERIAL);
			pd3ddevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, (m_csRenderState.m_trackColor&TVC_SPECULAR)?D3DMCS_COLOR1:D3DMCS_MATERIAL);
			pd3ddevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, (m_csRenderState.m_trackColor&TVC_EMISSIVE)?D3DMCS_COLOR1:D3DMCS_MATERIAL);
		}
		else
		{
			pd3ddevice->SetRenderState(D3DRS_COLORVERTEX, FALSE);
		}

		//雾化
		pd3ddevice->SetRenderState(D3DRS_FOGENABLE, m_csRenderState.m_bEnableFog?TRUE:FALSE);
		pd3ddevice->SetRenderState(D3DRS_FOGVERTEXMODE, MappingsD3D9::getFogMode( m_csRenderState.m_fogMode) );
		pd3ddevice->SetRenderState(D3DRS_FOGCOLOR, (DWORD) m_csRenderState.m_fogColor.getAsARGB() );
		pd3ddevice->SetRenderState(D3DRS_FOGSTART, F2DW( m_csRenderState.m_fogLinearStart) );
		pd3ddevice->SetRenderState(D3DRS_FOGEND, F2DW( m_csRenderState.m_fogLinearEnd) );
		pd3ddevice->SetRenderState(D3DRS_FOGDENSITY, F2DW(m_csRenderState.m_fogExpDensity) );

		//光照
		for( uint i =0; i< m_csRenderState.s_maxLight; ++i)
		{
			pd3ddevice->SetLight(i, &m_csRenderState.m_d3d9light[i]);
			pd3ddevice->LightEnable(i, m_csRenderState.m_light[i].isEnabled() ? TRUE:FALSE);
		}
		pd3ddevice->SetRenderState(D3DRS_AMBIENT, m_csRenderState.m_ambientLight.getAsARGB() );

		//alpha检测
		pd3ddevice->SetRenderState(D3DRS_ALPHAFUNC, MappingsD3D9::getAlphaFunc(m_csRenderState.m_AlphaCompareFunction) );
		float fAlphaValue = m_csRenderState.m_AlphaCompareValue;
		xs::Math::clamp(fAlphaValue, 0.0f,1.0f);
		uchar ucAlphaVal =  fAlphaValue * 255.0f;
		pd3ddevice->SetRenderState( D3DRS_ALPHAREF, (DWORD)ucAlphaVal);
		pd3ddevice->SetRenderState( D3DRS_ALPHATESTENABLE, m_csRenderState.m_bAlphaCheckEnabled?TRUE:FALSE);

		//模板检测
		pd3ddevice->SetRenderState(D3DRS_STENCILFUNC, MappingsD3D9::getStencilFunc(m_csRenderState.m_stencilFunc));
		pd3ddevice->SetRenderState(D3DRS_STENCILMASK, m_csRenderState.m_stencilMask);
		pd3ddevice->SetRenderState(D3DRS_STENCILWRITEMASK, m_csRenderState.m_stencilMask);
		pd3ddevice->SetRenderState(D3DRS_STENCILREF,  m_csRenderState.m_stencilRefValue);
		pd3ddevice->SetRenderState(D3DRS_STENCILFAIL, MappingsD3D9::getStencilOp(m_csRenderState.m_stencilFailOp));
		pd3ddevice->SetRenderState(D3DRS_STENCILZFAIL, MappingsD3D9::getStencilOp( m_csRenderState.m_depthFailOp) );
		pd3ddevice->SetRenderState(D3DRS_STENCILPASS, MappingsD3D9::getStencilOp( m_csRenderState.m_passOp) );
		pd3ddevice->SetRenderState(D3DRS_STENCILENABLE, m_csRenderState.m_bEnableStencilTest ? TRUE:FALSE);

		//剪切测试
		pd3ddevice->SetRenderState(D3DRS_SCISSORTESTENABLE, m_csRenderState.m_bScissor ?TRUE:FALSE);

		//alpha混合
		pd3ddevice->SetRenderState(D3DRS_SRCBLEND, MappingsD3D9::getBlendFactor(m_csRenderState.m_alphaBlendSrc));
		pd3ddevice->SetRenderState(D3DRS_DESTBLEND, MappingsD3D9::getBlendFactor(m_csRenderState.m_alphaBlendDst));
		if( m_csRenderState.m_alphaBlendSrc == SBF_ONE &&  m_csRenderState.m_alphaBlendDst == SBF_ZERO)
		{
			pd3ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
		else
		{
			pd3ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		}

		//采样器状态
		for( uint i =0; i<m_csRenderState.s_maxTexureUnit; ++i)
		{
			pd3ddevice->SetSamplerState(i, D3DSAMP_ADDRESSU, MappingsD3D9::getAddressMode( m_csRenderState.m_addressU[i]) );
			pd3ddevice->SetSamplerState(i, D3DSAMP_ADDRESSV, MappingsD3D9::getAddressMode( m_csRenderState.m_addressV[i]) );
			pd3ddevice->SetSamplerState(i, D3DSAMP_MINFILTER, MappingsD3D9::getFilterType( m_csRenderState.m_minFilter[i]) );
			pd3ddevice->SetSamplerState(i, D3DSAMP_MAGFILTER, MappingsD3D9::getFilterType( m_csRenderState.m_magFilter[i]) );
			pd3ddevice->SetSamplerState(i, D3DSAMP_MIPFILTER, MappingsD3D9::getFilterType( m_csRenderState.m_mipFilter[i]) );
		}

		//纹理阶段状态
		for( uint i =0; i< m_csRenderState.s_maxTexureUnit; ++i)
		{
			if( m_csRenderState.m_TSSEnable[i] )
			{
				pd3ddevice->SetTextureStageState(i, D3DTSS_ALPHAOP, MappingsD3D9::getTextureStageOperator(m_csRenderState.m_TSSAlphaOP[i]) );
				pd3ddevice->SetTextureStageState(i, D3DTSS_COLOROP, MappingsD3D9::getTextureStageOperator(m_csRenderState.m_TSSColorOP[i]) );
			}
			else
			{
				pd3ddevice->SetTextureStageState(i, D3DTSS_COLOROP, D3DTOP_DISABLE );
				pd3ddevice->SetTextureStageState(i, D3DTSS_ALPHAOP, D3DTOP_DISABLE );							
			}
			pd3ddevice->SetTextureStageState(i, D3DTSS_ALPHAARG1, MappingsD3D9::getTextureStageArgument(m_csRenderState.m_TSSAlphaArg[i][0]) );
			pd3ddevice->SetTextureStageState(i, D3DTSS_ALPHAARG2, MappingsD3D9::getTextureStageArgument(m_csRenderState.m_TSSAlphaArg[i][1]) );	
			pd3ddevice->SetTextureStageState(i, D3DTSS_COLORARG1, MappingsD3D9::getTextureStageArgument(m_csRenderState.m_TSSColorArg[i][0]) );
			pd3ddevice->SetTextureStageState(i, D3DTSS_COLORARG2, MappingsD3D9::getTextureStageArgument(m_csRenderState.m_TSSColorArg[i][1]) );
		}

		// 反锯齿
		pd3ddevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, m_csRenderState.m_bMutiSampleAntialias ? TRUE : FALSE);
		return true;
	}

	bool RenderSystemD3D9::setTextureStageStatus(	
		uint stage,
		TextureStageOperator colorOP,
		TextureStageArgument colorArg0,
		TextureStageArgument colorArg1,
		TextureStageOperator alphaOP,
		TextureStageArgument alphaArg0,
		TextureStageArgument alphaArg1)
	{
		if( stage >= m_csRenderState.s_maxTexureUnit) return false;
		IDirect3DDevice9 * pD3DDevice = m_pcsD3D9Device->getDevice();

		if( m_csRenderState.m_TSSAlphaOP[stage] != alphaOP )
		{
			m_csRenderState.m_TSSAlphaOP[stage] = alphaOP;
			if( m_csRenderState.m_TSSEnable[stage] )
				pD3DDevice->SetTextureStageState(stage, D3DTSS_ALPHAOP, MappingsD3D9::getTextureStageOperator(alphaOP) );
		}

		if( alphaArg0 != m_csRenderState.m_TSSAlphaArg[stage][0])
		{
			m_csRenderState.m_TSSAlphaArg[stage][0] = alphaArg0;
			pD3DDevice->SetTextureStageState(stage, D3DTSS_ALPHAARG1, MappingsD3D9::getTextureStageArgument(alphaArg0) );
		}
		
		if( alphaArg1 != m_csRenderState.m_TSSAlphaArg[stage][1])
		{
			m_csRenderState.m_TSSAlphaArg[stage][1] = alphaArg1;
			pD3DDevice->SetTextureStageState(stage, D3DTSS_ALPHAARG2, MappingsD3D9::getTextureStageArgument(alphaArg1) );
		}

		if( m_csRenderState.m_TSSColorOP[stage] != colorOP )
		{
			m_csRenderState.m_TSSColorOP[stage] = colorOP;
			if( m_csRenderState.m_TSSEnable[stage] )
				pD3DDevice->SetTextureStageState(stage, D3DTSS_COLOROP, MappingsD3D9::getTextureStageOperator(colorOP) );
		}

		if( colorArg0 != m_csRenderState.m_TSSColorArg[stage][0])
		{
			m_csRenderState.m_TSSColorArg[stage][0] = colorArg0;
			pD3DDevice->SetTextureStageState(stage, D3DTSS_COLORARG1, MappingsD3D9::getTextureStageArgument(colorArg0) );
		}
		if( colorArg1 != m_csRenderState.m_TSSColorArg[stage][1])
		{
			m_csRenderState.m_TSSColorArg[stage][1] = colorArg1;
			pD3DDevice->SetTextureStageState(stage, D3DTSS_COLORARG2, MappingsD3D9::getTextureStageArgument(colorArg1) );
		}
			
		return true;
	}

	void RenderSystemD3D9::getTextureStageStatus(	
		uint stage,
		TextureStageOperator & colorOP,
		TextureStageArgument & colorArg0,
		TextureStageArgument & colorArg1,
		TextureStageOperator & alphaOP,
		TextureStageArgument & alphaArg0,
		TextureStageArgument & alphaArg1)
	{
		if( stage >= m_csRenderState.s_maxTexureUnit) return;
		colorOP = m_csRenderState.m_TSSColorOP[stage];
		colorArg0 = m_csRenderState.m_TSSColorArg[stage][0];
		colorArg1 = m_csRenderState.m_TSSColorArg[stage][1];
		alphaOP = m_csRenderState.m_TSSAlphaOP[stage];
		alphaArg0 = m_csRenderState.m_TSSAlphaArg[stage][0];
		alphaArg1 = m_csRenderState.m_TSSAlphaArg[stage][1];
		return;
	}

	IHardwareCursorManager * RenderSystemD3D9::getHardwareCursorManager()
	{
		return m_pcsHardwareCursorMgr;
	}

	void RenderSystemD3D9::getTransformMatrix(TransformMatrixType type, D3DXMATRIX & mtx)
	{
		RenderTargetD3D9 * pRT = m_pcsRenderTargetMgr->getCurrentRenderTarget();
		if( NULL == pRT ) 
		{
			D3DXMatrixIdentity( &mtx);
			return;
		}

		switch( type)
		{
		case TMT_WORLD:
			{
				pRT->getWorldMatrix( mtx);
				return;
			}
			break;
		case TMT_VIEW:
			{
				pRT->getViewMatrix( mtx);
				return;
			}
			break;
		case TMT_WORLD_VIEW:
			{
				D3DXMATRIX tmp;
				pRT->getWorldMatrix(mtx);
				pRT->getViewMatrix(tmp);
				D3DXMatrixMultiply( &mtx, &mtx,&tmp);
				return;
			}
			break;
		case TMT_PROJECTION:
			{
				pRT->getProjectionMatrix(mtx);
				return;
			}
			break;
		case TMT_VIEW_PROJECTION:
			{
				D3DXMATRIX tmp;
				pRT->getViewMatrix(mtx);
				pRT->getProjectionMatrix(tmp);
				D3DXMatrixMultiply(&mtx, &mtx, &tmp);
				return;
			}
			break;
		case TMT_WORLD_VIEW_PROJECTION:
			{
				D3DXMATRIX tmp1, tmp2;
				pRT->getWorldMatrix(mtx);
				pRT->getViewMatrix(tmp1);
				pRT->getProjectionMatrix(tmp2);
				D3DXMatrixMultiply(&mtx, &mtx, &tmp1);
				D3DXMatrixMultiply(&mtx, &mtx, &tmp2);
				return;
			}
			break;
		case TMT_WORLD_NORMAL:
			{
				pRT->getWorldMatrix( mtx);
				D3DXMatrixInverse(&mtx, NULL, &mtx);
				D3DXMatrixTranspose(&mtx, &mtx);
				return;	
			}
			break;
		case TMT_VIEW_NORMAL:
			{
				pRT->getViewMatrix(mtx);
				D3DXMatrixInverse( &mtx, NULL, &mtx);
				D3DXMatrixTranspose(&mtx, &mtx);
				return;
			}
			break;
		case TMT_WORLD_VIEW_NORMAL:
			{
				D3DXMATRIX tmp;
				pRT->getWorldMatrix(mtx);
				pRT->getViewMatrix(tmp);
				D3DXMatrixMultiply(&mtx, &mtx, &tmp);
				D3DXMatrixTranspose(&mtx, &mtx);
				return;		
			}
			break;
		default: //绝不会到这里
			break;
		}
	}
}