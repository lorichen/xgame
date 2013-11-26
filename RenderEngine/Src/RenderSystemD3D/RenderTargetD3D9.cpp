#include "StdAfx.h"
#include "RenderTargetD3D9.h"
#include "RenderSystemD3D9.h"
#include "D3D9Device.h"
#include "MappingsD3D9.h"


namespace xs
{
	RenderTargetD3D9::RenderState::RenderState():
	m_mtxWorld(xs::Matrix4::IDENTITY),
	m_mtxView(xs::Matrix4::IDENTITY),
	m_mtxProjection(xs::Matrix4::IDENTITY),
	m_mtxModelView(xs::Matrix4::IDENTITY),
	m_cullingMode(CULL_NONE),
	m_bLightingEnabled(false),
	m_bDepthWriteEnabled(true),
	m_bDepthCheckEnabled(false),
	m_DepthBufferFunction(CMPF_LESS)
	{
	}

	RenderTargetD3D9::RenderTargetD3D9 (RenderSystemD3D9 * pRenderSystem, bool b2d, RenderTargetManagerD3D9 * pRenderTargetMgr):
	m_clearColor(xs::ColorValue::Black),					//帧缓存清除状态
	m_clearDepth(1.0f),
	m_clearStencil(0),
	m_redChannel(true), m_greenChannel(true), m_blueChannel(true), m_alphaChannel(true) //颜色缓存mask
	{	
		//
		Assert(pRenderSystem);
		Assert(pRenderTargetMgr);
		m_pRenderSystem = pRenderSystem;
		m_pRenderTargetMgr = pRenderTargetMgr;

		//视口参数	
		m_viewport.X = 0;
		m_viewport.Y = 0;
		m_viewport.Height = 0;
		m_viewport.Width = 0;
		m_viewport.MinZ = 0;
		m_viewport.MaxZ = 1;

		//2d 3d状态
		m_b2D = b2d;
		m_RenderState = m_RenderState3D;
	}

	RenderTargetD3D9::~RenderTargetD3D9()
	{
	}

	void RenderTargetD3D9::onAttach(RenderTargetD3D9 * pPrevious)
	{
		//在这里设置渲染状态
		if( pPrevious )
		{
			//此时比较再设置渲染状态
			IDirect3DDevice9 * pD3D9Device = m_pRenderSystem->getD3D9Device()->getDevice();
			Assert( pD3D9Device);

			//帧缓存清除状态
			//不需要

			//视口参数
			if( m_viewport.X != pPrevious->m_viewport.X
				|| m_viewport.Y != pPrevious->m_viewport.Y
				|| m_viewport.Width != pPrevious->m_viewport.Width
				|| m_viewport.Height != pPrevious->m_viewport.Height)
			{
				pD3D9Device->SetViewport( &m_viewport);
			}

			//颜色缓存Mask
			if( m_redChannel != pPrevious->m_redChannel
				|| m_greenChannel != pPrevious->m_greenChannel
				|| m_blueChannel != pPrevious->m_blueChannel
				|| m_alphaChannel != pPrevious->m_alphaChannel )
			{
				DWORD colormask = 0;
				m_redChannel ? ( colormask |= D3DCOLORWRITEENABLE_RED): 0;
				m_greenChannel ? ( colormask |=D3DCOLORWRITEENABLE_GREEN ):0;
				m_blueChannel ?	(colormask |= D3DCOLORWRITEENABLE_BLUE) : 0;
				m_alphaChannel ? (colormask |= D3DCOLORWRITEENABLE_ALPHA) : 0;
				pD3D9Device->SetRenderState(D3DRS_COLORWRITEENABLE, colormask);			
			}

			//与2d和3d相关的状态

			////矩阵状态
			//world matrix
			if( m_RenderState.m_mtxWorld != pPrevious->m_RenderState.m_mtxWorld )
			{
				pD3D9Device->SetTransform( D3DTS_WORLD, &MappingsD3D9::getMatrix(m_RenderState.m_mtxWorld));
			}

			//view matrix
			if( m_RenderState.m_mtxView != pPrevious->m_RenderState.m_mtxView)
			{
				xs::Matrix4 m_tmpView(m_RenderState.m_mtxView);
				m_tmpView [2][0] = - m_tmpView[2][0];
				m_tmpView [2][1] = - m_tmpView[2][1];
				m_tmpView [2][2] = - m_tmpView[2][2];
				m_tmpView [2][3] = - m_tmpView[2][3];
				pD3D9Device->SetTransform( D3DTS_VIEW, &MappingsD3D9::getMatrix(m_tmpView) );
			}

			//projection matrix
			if( m_RenderState.m_mtxProjection != pPrevious->m_RenderState.m_mtxProjection )
			{
				xs::Matrix4 tmpProj;
				convertProjectionMatrix(m_RenderState.m_mtxProjection, tmpProj, false);
				pD3D9Device->SetTransform(D3DTS_PROJECTION, &MappingsD3D9::getMatrix( tmpProj) );
			}

			//背面拣选
			if( m_RenderState.m_cullingMode != pPrevious->m_RenderState.m_cullingMode )
			{
				pD3D9Device->SetRenderState(D3DRS_CULLMODE, MappingsD3D9::getCullingMode(m_RenderState.m_cullingMode) );
			}

			//光照参数
			if( m_RenderState.m_bLightingEnabled != pPrevious->m_RenderState.m_bLightingEnabled )
			{
				pD3D9Device->SetRenderState(D3DRS_LIGHTING, m_RenderState.m_bLightingEnabled ? TRUE:FALSE);
			}
			
			
			//深度检测
			if( m_RenderState.m_bDepthWriteEnabled != pPrevious->m_RenderState.m_bDepthWriteEnabled )
			{
				pD3D9Device->SetRenderState(D3DRS_ZWRITEENABLE, m_RenderState.m_bDepthWriteEnabled ? TRUE:FALSE);
			}
			if( m_RenderState.m_DepthBufferFunction != pPrevious->m_RenderState.m_DepthBufferFunction )
			{
				pD3D9Device->SetRenderState(D3DRS_ZFUNC, MappingsD3D9::getDepthFunc( m_RenderState.m_DepthBufferFunction) );
			}
			if( m_RenderState.m_bDepthCheckEnabled != pPrevious->m_RenderState.m_bDepthCheckEnabled )
			{
				pD3D9Device->SetRenderState(D3DRS_ZENABLE, m_RenderState.m_bDepthCheckEnabled?D3DZB_TRUE:D3DZB_FALSE);
			}
		}
		else
		{
			resetRenderState();
		}
	}
	void RenderTargetD3D9::onDetach()
	{
		//nothing to do
	}

	//清除帧缓存函数
	void	RenderTargetD3D9::setClearColor(const ColorValue& color)
	{
		m_clearColor = color;
	}
	const ColorValue&	RenderTargetD3D9::getClearColor()
	{
		return m_clearColor;
	}
	void	RenderTargetD3D9::setClearDepth(float depth)
	{
		m_clearDepth = depth;
	}
	void	RenderTargetD3D9::setClearStencil(int stencil)
	{
		m_clearStencil = stencil;
	}
	float RenderTargetD3D9::getClearDepth()
	{
		return m_clearDepth;
	}
	int RenderTargetD3D9::getClearStencil()
	{
		return m_clearStencil;
	}

	void RenderTargetD3D9::setRenderState( const RenderState & rs)
	{
		//矩阵状态	
		setWorldMatrix( rs.m_mtxWorld);		
		setViewMatrix(rs.m_mtxView);
		setProjectionMatrix( rs.m_mtxProjection);

		//背面拣选
		setCullingMode(rs.m_cullingMode);

		//光照参数
		setLightingEnabled(rs.m_bLightingEnabled);

		//深度检测
		setDepthBufferCheckEnabled(rs.m_bDepthCheckEnabled);
		setDepthBufferFunction(rs.m_DepthBufferFunction);
		setDepthBufferWriteEnabled(rs.m_bDepthWriteEnabled);
	}

	//2d 3d切换
	void	RenderTargetD3D9::switchTo2D()
	{
		if( m_b2D ) return;
		m_b2D = true;

		m_RenderState2D.m_mtxProjection = Matrix4::orthogonalProjection(
			m_viewport.X, m_viewport.X+m_viewport.Width, 
			m_viewport.Y + m_viewport.Height, m_viewport.Y,
			-1.0f, 1.0f);

		m_RenderState3D = m_RenderState;
		setRenderState(m_RenderState2D);
	}

	void RenderTargetD3D9::switchTo2D(int left,int top,int width,int height)
	{
		setViewport(left, top, width, height);
		switchTo2D();
	}

	void	RenderTargetD3D9::switchTo3D()
	{
		if( !m_b2D ) return;
		m_b2D = false;

		m_RenderState2D = m_RenderState;
		setRenderState(m_RenderState3D);
	}

	bool RenderTargetD3D9::is2dMode()
	{
		return m_b2D;
	}

	//背面拣选
	void RenderTargetD3D9::setCullingMode(const CullingMode& cm)
	{
		if( cm == m_RenderState.m_cullingMode) return;
		m_RenderState.m_cullingMode = cm;
		m_pRenderSystem->getD3D9Device()->getDevice()->SetRenderState(D3DRS_CULLMODE, MappingsD3D9::getCullingMode(cm) );
	}

	const CullingMode& RenderTargetD3D9::getCullingMode()
	{
		return m_RenderState.m_cullingMode;
	}

	//光照参数
	void	RenderTargetD3D9::setLightingEnabled(bool enabled)
	{
		if(	m_RenderState.m_bLightingEnabled == enabled)
			return;

		m_RenderState.m_bLightingEnabled = enabled;
		m_pRenderSystem->getD3D9Device()->getDevice()->SetRenderState(D3DRS_LIGHTING, enabled?TRUE:FALSE);
	}

	bool RenderTargetD3D9::isLightingEnabled()
	{
		return m_RenderState.m_bLightingEnabled;
	}

	//矩阵变换
	void	RenderTargetD3D9::setWorldMatrix(const Matrix4 & mtx)
	{
		//if( m_RenderState.m_mtxWorld == mtx) return; //这种情况很少出现
		m_RenderState.m_mtxWorld = mtx;
		m_RenderState.m_mtxModelView = m_RenderState.m_mtxView * mtx;
		D3DXMATRIX mtxD3D;
		getWorldMatrix(mtxD3D);
		m_pRenderSystem->getD3D9Device()->getDevice()->SetTransform( D3DTS_WORLD, &mtxD3D);
	}

	const Matrix4& RenderTargetD3D9::getWorldMatrix()
	{
		return  m_RenderState.m_mtxWorld;
	}

	const Matrix4& RenderTargetD3D9::getModelViewMatrix()
	{
		return m_RenderState.m_mtxModelView;
	}

	void RenderTargetD3D9::setViewMatrix(const Matrix4& mtx)
	{
		//if( m_RenderState.m_mtxView == mtx) return; //这种情况很少出现
		m_RenderState.m_mtxView = mtx;
		m_RenderState.m_mtxModelView = m_RenderState.m_mtxView * m_RenderState.m_mtxWorld;
		D3DXMATRIX mtxD3D;
		getViewMatrix(mtxD3D);
		m_pRenderSystem->getD3D9Device()->getDevice()->SetTransform( D3DTS_VIEW, &mtxD3D );
	}	

	const Matrix4&  RenderTargetD3D9::getViewMatrix()
	{
		return m_RenderState.m_mtxView; 
	}

	void RenderTargetD3D9::setProjectionMatrix(const Matrix4& mtx)
	{
		//if( m_RenderState.m_mtxProjection == mtx) return;//不会经常出现这种情况
		m_RenderState.m_mtxProjection = mtx;
		D3DXMATRIX mtxD3D;
		getProjectionMatrix(mtxD3D);
		m_pRenderSystem->getD3D9Device()->getDevice()->SetTransform(D3DTS_PROJECTION, &mtxD3D );
	}

	void RenderTargetD3D9::convertProjectionMatrix(const Matrix4& matrix,
							Matrix4& dest, bool forGpuProgram)
	{
		dest = matrix;
		// Convert depth range from [-1,+1] to [0,1]
		dest[2][0] = (dest[2][0] + dest[3][0]) / 2;
		dest[2][1] = (dest[2][1] + dest[3][1]) / 2;
		dest[2][2] = (dest[2][2] + dest[3][2]) / 2;
		dest[2][3] = (dest[2][3] + dest[3][3]) / 2;

		if (!forGpuProgram)
		{
			// Convert right-handed to left-handed
			dest[0][2] = -dest[0][2];
			dest[1][2] = -dest[1][2];
			dest[2][2] = -dest[2][2];
			dest[3][2] = -dest[3][2];
		}
	}

	void RenderTargetD3D9::resetRenderState()
	{
		IDirect3DDevice9 * pD3D9Device = m_pRenderSystem->getD3D9Device()->getDevice();
		Assert( pD3D9Device);

		//帧缓存清除状态
		//不需要

		//视口参数
		pD3D9Device->SetViewport(&m_viewport);

		//颜色缓存Mask
		DWORD colormask = 0;
		m_redChannel ? ( colormask |= D3DCOLORWRITEENABLE_RED): 0;
		m_greenChannel ? ( colormask |=D3DCOLORWRITEENABLE_GREEN ):0;
		m_blueChannel ?	(colormask |= D3DCOLORWRITEENABLE_BLUE) : 0;
		m_alphaChannel ? (colormask |= D3DCOLORWRITEENABLE_ALPHA) : 0;
		pD3D9Device->SetRenderState(D3DRS_COLORWRITEENABLE, colormask);

		//与2d和3d相关的状态

		////矩阵状态
		D3DXMATRIX mtxD3D;
		//world matrix
		getWorldMatrix(mtxD3D);
		pD3D9Device->SetTransform( D3DTS_WORLD, &mtxD3D);
		//view matrix
		getViewMatrix(mtxD3D);
		pD3D9Device->SetTransform( D3DTS_VIEW, &mtxD3D );
		//projection matrix
		getProjectionMatrix(mtxD3D);
		pD3D9Device->SetTransform(D3DTS_PROJECTION, &mtxD3D );

		//背面拣选
		pD3D9Device->SetRenderState(D3DRS_CULLMODE, MappingsD3D9::getCullingMode(m_RenderState.m_cullingMode) );

		//光照参数
		pD3D9Device->SetRenderState(D3DRS_LIGHTING, m_RenderState.m_bLightingEnabled ? TRUE:FALSE);

		//深度检测
		pD3D9Device->SetRenderState(D3DRS_ZWRITEENABLE, m_RenderState.m_bDepthWriteEnabled ? TRUE:FALSE);
		pD3D9Device->SetRenderState(D3DRS_ZFUNC, MappingsD3D9::getDepthFunc( m_RenderState.m_DepthBufferFunction) );
		pD3D9Device->SetRenderState(D3DRS_ZENABLE, m_RenderState.m_bDepthCheckEnabled?D3DZB_TRUE:D3DZB_FALSE);
		
	}

	const Matrix4& RenderTargetD3D9::getProjectionMatrix()
	{
		return m_RenderState.m_mtxProjection;
	}

	void RenderTargetD3D9::getWorldMatrix(D3DXMATRIX & mtx)
	{
		MappingsD3D9::getMatrix( m_RenderState.m_mtxWorld, mtx);
	}

	void RenderTargetD3D9::getViewMatrix(D3DXMATRIX & mtx)
	{
		xs::Matrix4 tmp( m_RenderState.m_mtxView);
		tmp [2][0] = - tmp[2][0];
		tmp [2][1] = - tmp[2][1];
		tmp [2][2] = - tmp[2][2];
		tmp [2][3] = - tmp[2][3];
		MappingsD3D9::getMatrix(tmp, mtx);
	}

	void RenderTargetD3D9::getProjectionMatrix(D3DXMATRIX & mtx)
	{
		xs::Matrix4 tmp;
		convertProjectionMatrix(m_RenderState.m_mtxProjection, tmp,false);
		MappingsD3D9::getMatrix(tmp, mtx);	
	}

	//深度检测
	bool RenderTargetD3D9::isDepthBufferCheckEnabled()
	{
		return m_RenderState.m_bDepthCheckEnabled;
	}
	void RenderTargetD3D9::setDepthBufferCheckEnabled(bool enabled)
	{
		if( m_RenderState.m_bDepthCheckEnabled == enabled)
			return;

		m_RenderState.m_bDepthCheckEnabled = enabled;
		m_pRenderSystem->getD3D9Device()->getDevice()->SetRenderState(D3DRS_ZENABLE, enabled?D3DZB_TRUE:D3DZB_FALSE);
	}

	void RenderTargetD3D9::setDepthBufferFunction(const CompareFunction& cf)
	{
		if( m_RenderState.m_DepthBufferFunction == cf)
			return;

		m_RenderState.m_DepthBufferFunction = cf;
		m_pRenderSystem->getD3D9Device()->getDevice()->SetRenderState(D3DRS_ZFUNC, MappingsD3D9::getDepthFunc(cf) );
	
	}
	void RenderTargetD3D9::setDepthBufferWriteEnabled(bool enabled)
	{
		if( m_RenderState.m_bDepthWriteEnabled == enabled)
			return;

		m_RenderState.m_bDepthWriteEnabled = enabled;
		m_pRenderSystem->getD3D9Device()->getDevice()->SetRenderState(D3DRS_ZWRITEENABLE, enabled?TRUE:FALSE);

	}
	bool RenderTargetD3D9::isDepthBufferWriteEnabled()
	{
		return m_RenderState.m_bDepthWriteEnabled;
	}
	const CompareFunction& RenderTargetD3D9::getDepthBufferFunction()
	{
		return m_RenderState.m_DepthBufferFunction;
	}

	//颜色缓存mask
	void RenderTargetD3D9::setColorBufferWriteEnabled(bool red, bool green, bool blue, bool alpha)
	{
		DWORD mask = 0;
		(m_redChannel = red)?( mask |= D3DCOLORWRITEENABLE_RED): 0;
		(m_greenChannel = green) ? ( mask |=D3DCOLORWRITEENABLE_GREEN ):0;
		(m_blueChannel = blue) ?(mask |= D3DCOLORWRITEENABLE_BLUE) : 0;
		(m_alphaChannel = alpha) ?(mask |= D3DCOLORWRITEENABLE_ALPHA) : 0;
		m_pRenderSystem->getD3D9Device()->getDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, mask);
		
	}

	//视口
	void RenderTargetD3D9::setViewport(int left,int top,int width,int height)
	{
		m_viewport.X = left >= 0 ? left: 0;
		m_viewport.Y = top >= 0 ? top : 0;
		m_viewport.Width = width >= 0 ? width : 0;
		m_viewport.Height = height >= 0 ? height : 0;
		m_pRenderSystem->getD3D9Device()->getDevice()->SetViewport( &m_viewport);
	}

	void RenderTargetD3D9::getViewport(int &left,int &top,int &width,int &height)
	{
		left = m_viewport.X;
		top = m_viewport.Y;
		width = m_viewport.Width;
		height = m_viewport.Height;
	}

	//投影和逆投影
	Vector3 RenderTargetD3D9::unproject( const Vector3 & v)
	{
		//world matrix
		D3DXMATRIX matWorldD3D ( MappingsD3D9::getMatrix( m_RenderState.m_mtxWorld ) );
		//view matrix
		xs::Matrix4 matView( m_RenderState.m_mtxView );
		matView[2][0] = -matView[2][0];
		matView[2][1] = -matView[2][1];
		matView[2][2] = -matView[2][2];
		matView[2][3] = -matView[2][3];
		D3DXMATRIX matViewD3D( MappingsD3D9::getMatrix( matView ) );
		//projection matrix
		xs::Matrix4 matProj;
		convertProjectionMatrix(m_RenderState.m_mtxProjection, matProj, false);
		D3DXMATRIX matProjD3D( MappingsD3D9::getMatrix( matProj) );
		

		D3DXVECTOR3 vin(v.x, v.y, v.z);
		D3DXVECTOR3 vout;
		D3DXVec3Unproject(&vout, &vin, &m_viewport, &matProjD3D, &matViewD3D, &matWorldD3D);
		return Vector3(vout.x, vout.y, vout.z);
	}

	Vector3 RenderTargetD3D9::project(const Vector3 & v)
	{
		//world matrix
		D3DXMATRIX matWorldD3D ( MappingsD3D9::getMatrix( m_RenderState.m_mtxWorld ) );
		//view matrix
		xs::Matrix4 matView( m_RenderState.m_mtxView );
		matView[2][0] = -matView[2][0];
		matView[2][1] = -matView[2][1];
		matView[2][2] = -matView[2][2];
		matView[2][3] = -matView[2][3];
		D3DXMATRIX matViewD3D( MappingsD3D9::getMatrix( matView ) );
		//projection matrix
		xs::Matrix4 matProj;
		convertProjectionMatrix(m_RenderState.m_mtxProjection, matProj, false);
		D3DXMATRIX matProjD3D( MappingsD3D9::getMatrix( matProj) );
		
		D3DXVECTOR3 vin(v.x, v.y, v.z);
		D3DXVECTOR3 vout;
		D3DXVec3Project(&vout, &vin, &m_viewport, &matProjD3D, &matViewD3D, &matWorldD3D);
		return Vector3(vout.x, vout.y, vout.z);		
	}
}