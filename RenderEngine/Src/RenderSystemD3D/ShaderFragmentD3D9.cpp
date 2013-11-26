#include "StdAfx.h"
#include "ShaderFragmentD3D9.h"
#include "RenderSystemD3D9.h"
#include "D3D9Device.h"


namespace xs
{

	ShaderFragmentD3D9::ShaderFragmentD3D9( const std::string & name)
		:ManagedItem(name)
		,m_pRenderSystem(0)
		,m_pShader(0)
	{
		
	}

	ShaderFragmentD3D9::~ShaderFragmentD3D9()
	{
	
	}

	bool ShaderFragmentD3D9::create(RenderSystemD3D9 * pRenderSystem, ShaderType st,const uchar *pBuffer,uint len)
	{
		if( NULL == pRenderSystem )
			return false;
		if( NULL == pBuffer)
			return false;
		if( len <= 0 )
			return false;

		m_type = st;
		m_pRenderSystem = pRenderSystem;
		IDirect3DDevice9 * pD3DDevice = m_pRenderSystem->getD3D9Device()->getDevice();
		if( NULL == pD3DDevice)
			return false;
		xs::D3D9ResourceManager * pD3DResMgr = m_pRenderSystem->getD3D9ResourceMgr();
		if( NULL == pD3DResMgr)
			return false;

		//vertex shader
		if( m_type == ST_VERTEX_PROGRAM)
		{
			//compile shader
			ID3DXBuffer * pShaderBuf = 0;
			HRESULT hr = D3DXAssembleShader(
				(LPCSTR)pBuffer,
				(UINT)len,
				NULL,
				NULL,
				0,
				&pShaderBuf,
				NULL);	
			if( FAILED(hr) )
				return false;

			//create shader object
			pD3DResMgr->waitForEndFrame();//等待创建资源的通知
			pD3DResMgr->lockDeviceAccess();//锁定设备
			hr = pD3DDevice->CreateVertexShader( 
				(const DWORD *)(pShaderBuf->GetBufferPointer()),
				(IDirect3DVertexShader9 **)(&m_pShader) );
			pD3DResMgr->unlockDeviceAccess();
			pShaderBuf->Release();

			if( FAILED(hr) )
			{
				return false;
			}
			else
			{
				pD3DResMgr->notifyResourceCreated(this);
				return true;			
			}		
		}
		else // pixel shader
		{
			//compile shader
			ID3DXBuffer * pShaderBuf = 0;
			HRESULT hr = D3DXAssembleShader(
				(LPCSTR)pBuffer,
				(UINT)len,
				NULL,
				NULL,
				0,
				&pShaderBuf,
				NULL);	
			if( FAILED(hr) )
				return false;

			//create shader object
			pD3DResMgr->waitForEndFrame();//等待创建资源的通知
			pD3DResMgr->lockDeviceAccess();//锁定设备
			hr = pD3DDevice->CreatePixelShader( 
				(const DWORD *)(pShaderBuf->GetBufferPointer()),
				(IDirect3DPixelShader9 **)(&m_pShader) );
			pD3DResMgr->unlockDeviceAccess();
			pShaderBuf->Release();

			if( FAILED(hr) )
			{
				return false;
			}
			else
			{
				pD3DResMgr->notifyResourceCreated(this);
				return true;			
			}		
		}
	}

	void ShaderFragmentD3D9::release()		
	{
		if( m_pShader)
		{
			xs::D3D9ResourceManager * pD3DResMgr = m_pRenderSystem->getD3D9ResourceMgr();
			Assert( NULL != pD3DResMgr);
			switch( m_type)
			{
			case ST_VERTEX_PROGRAM:
				{
					pD3DResMgr->lockResourceAccess();
					((IDirect3DVertexShader9 *)m_pShader )->Release();
					m_pShader = 0;
					pD3DResMgr->unlockResourceAccess();
					pD3DResMgr->notifyResourceRelease(this);
				}
				break;
			case ST_FRAGMENT_PROGRAM:
				{
					pD3DResMgr->lockResourceAccess();
					((IDirect3DPixelShader9 *)m_pShader)->Release();
					m_pShader = 0;
					pD3DResMgr->unlockResourceAccess();
					pD3DResMgr->notifyResourceRelease(this);
				}
				break;
			default: // 绝不会到这里
				break;
			}
		}
		delete this;
	}

	/** 获得着色器类型顶点着色器还是像素着色器
	@return 着色器类型
	@see ShaderType
	*/
	ShaderType ShaderFragmentD3D9::getType()
	{
		return m_type;
	}

	/** 编译着色器
	@return 是否成功
	*/
	bool ShaderFragmentD3D9::compile()
	{
		return true;
	}

	/** 绑定到此着色器
	@return 是否绑定成功
	*/
	bool ShaderFragmentD3D9::bind()
	{
		IDirect3DDevice9 * pD3DDevice = m_pRenderSystem->getD3D9Device()->getDevice();
		if( NULL == pD3DDevice)
			return false;
		
		HRESULT hr;
		switch( m_type)
		{
		case ST_VERTEX_PROGRAM:
			hr = pD3DDevice->SetVertexShader( (IDirect3DVertexShader9 *)m_pShader);
			break;
		case ST_FRAGMENT_PROGRAM:
			hr = pD3DDevice->SetPixelShader( (IDirect3DPixelShader9 *)m_pShader);
			break;
		default:
			return false;
			break;		
		}

		return SUCCEEDED(hr);
	}

	/** 取消绑定此着色器，和bind配套使用
	*/
	void ShaderFragmentD3D9::unbind()
	{
		IDirect3DDevice9 * pD3DDevice = m_pRenderSystem->getD3D9Device()->getDevice();
		if( NULL == pD3DDevice)
			return;
		
		HRESULT hr;
		switch( m_type)
		{
		case ST_VERTEX_PROGRAM:
			hr = pD3DDevice->SetVertexShader( NULL);
			break;
		case ST_FRAGMENT_PROGRAM:
			hr = pD3DDevice->SetPixelShader( NULL);
			break;
		default:
			return;
			break;		
		}
		return;
	}

	/** 设置统一浮点四维向量
	@param startRegister	起始的浮点常量寄存器序号
	@param pData			浮点数据指针
	@param countVector4		四维浮点向量的个数
	*/
	bool ShaderFragmentD3D9::setUniformFloatVector4(uint startRegister, float * pData, uint countVector4)
	{
		if( 0 == pData || 0 == countVector4 )
			return false;

		IDirect3DDevice9 * pD3DDevice = m_pRenderSystem->getD3D9Device()->getDevice();
		HRESULT hr;
		if( ST_VERTEX_PROGRAM == m_type )
			hr = pD3DDevice->SetVertexShaderConstantF( (UINT)startRegister, pData, countVector4);
		else
			hr = pD3DDevice->SetPixelShaderConstantF( (UINT)startRegister, pData, countVector4);

		return SUCCEEDED(hr);
	}

	/** 设置统一整型四维向量
	@param startRegister	起始的整型常量寄存器序号
	@param pData			整型数据指针
	@param countVector4		四维整形向量的个数
	*/
	bool ShaderFragmentD3D9::setUniformIntVector4(uint startRegister, int * pData, uint countVector4)
	{
		if( 0 == pData || 0 == countVector4 )
			return false;

		IDirect3DDevice9 * pD3DDevice = m_pRenderSystem->getD3D9Device()->getDevice();
		HRESULT hr;
		if( ST_VERTEX_PROGRAM == m_type )
			hr = pD3DDevice->SetVertexShaderConstantI( (UINT)startRegister, pData, countVector4);
		else
			hr = pD3DDevice->SetPixelShaderConstantI( (UINT)startRegister, pData, countVector4);

		return SUCCEEDED(hr);
	}

	/** 设置统一布尔变量
	@param startRegister	起始的布尔常量寄存器序号
	@param pData			布尔数据指针，0表示false，非0表示true
	@param countBool		布尔数据的个数
	*/
	bool ShaderFragmentD3D9::setUniformBool(uint startRegister, int * pData, uint countBool)
	{
		if( 0 == pData || 0 == countBool )
			return false;

		IDirect3DDevice9 * pD3DDevice = m_pRenderSystem->getD3D9Device()->getDevice();
		HRESULT hr;
		if( ST_VERTEX_PROGRAM == m_type )
			hr = pD3DDevice->SetVertexShaderConstantB( (UINT)startRegister, (const BOOL *)pData, countBool);
		else
			hr = pD3DDevice->SetPixelShaderConstantB( (UINT)startRegister, (const BOOL*)pData, countBool);
		return SUCCEEDED(hr);	
	}

	/** 将世界，摄像机，投影等矩阵绑定到shader的常量,适合低shader，只能同时绑定一个矩阵
	*/
	void ShaderFragmentD3D9::bindTransformMatrix(uint index, uint type, uint bindRow)
	{
		if( ST_VERTEX_PROGRAM != m_type) return;
		if( bindRow <= 0 || bindRow > 4) return;

		D3DXMATRIX mtx;
		IDirect3DDevice9 * pD3DDevice = m_pRenderSystem->getD3D9Device()->getDevice();
		
		switch( type)
		{
		case TMT_WORLD:
			{
				m_pRenderSystem->getTransformMatrix( TMT_WORLD, mtx);
				D3DXMatrixTranspose(&mtx, &mtx);
				pD3DDevice->SetVertexShaderConstantF(index, &mtx.m[0][0], bindRow);
				return;
			}
			break;
		case TMT_VIEW:
			{
				m_pRenderSystem->getTransformMatrix( TMT_VIEW, mtx);
				D3DXMatrixTranspose(&mtx, &mtx);
				pD3DDevice->SetVertexShaderConstantF(index, &mtx.m[0][0], bindRow);
				return;		
			}
			break;
		case TMT_WORLD_VIEW:
			{
				m_pRenderSystem->getTransformMatrix( TMT_WORLD_VIEW, mtx);
				D3DXMatrixTranspose(&mtx, &mtx);
				pD3DDevice->SetVertexShaderConstantF(index, &mtx.m[0][0], bindRow);
				return;			
			}
			break;
		case TMT_PROJECTION:
			{
				m_pRenderSystem->getTransformMatrix( TMT_PROJECTION, mtx);
				D3DXMatrixTranspose(&mtx, &mtx);
				pD3DDevice->SetVertexShaderConstantF(index, &mtx.m[0][0], bindRow);
				return;
			}
			break;
		case TMT_VIEW_PROJECTION:
			{
				m_pRenderSystem->getTransformMatrix( TMT_VIEW_PROJECTION, mtx);
				D3DXMatrixTranspose(&mtx, &mtx);
				pD3DDevice->SetVertexShaderConstantF(index, &mtx.m[0][0], bindRow);
				return;	
			}
			break;
		case TMT_WORLD_VIEW_PROJECTION:
			{
				m_pRenderSystem->getTransformMatrix( TMT_WORLD_VIEW_PROJECTION, mtx);
				D3DXMatrixTranspose(&mtx, &mtx);
				pD3DDevice->SetVertexShaderConstantF(index, &mtx.m[0][0], bindRow);
				return;
			}
			break;
		case TMT_WORLD_NORMAL:
			{
				m_pRenderSystem->getTransformMatrix( TMT_WORLD_NORMAL, mtx);
				D3DXMatrixTranspose(&mtx, &mtx);
				pD3DDevice->SetVertexShaderConstantF(index, &mtx.m[0][0], bindRow);
				return;	
			}
			break;
		case TMT_VIEW_NORMAL:
			{
				m_pRenderSystem->getTransformMatrix( TMT_VIEW_NORMAL, mtx);
				D3DXMatrixTranspose(&mtx, &mtx);
				pD3DDevice->SetVertexShaderConstantF(index, &mtx.m[0][0], bindRow);
				return;	
			}
			break;
		case TMT_WORLD_VIEW_NORMAL:
			{
				m_pRenderSystem->getTransformMatrix( TMT_WORLD_VIEW_NORMAL, mtx);
				D3DXMatrixTranspose(&mtx, &mtx);
				pD3DDevice->SetVertexShaderConstantF(index, &mtx.m[0][0], bindRow);
				return;		
			}
			break;
		default:
			break;
		}
		return;
	}
}
