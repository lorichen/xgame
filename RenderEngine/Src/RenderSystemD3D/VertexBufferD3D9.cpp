#include "StdAfx.h"
#include "VertexBufferD3D9.h"
#include "BufferManagerD3D9.h"
#include "D3D9Device.h"
#include "RenderSystemD3D9.h"
#include "MappingsD3D9.h"
#include "D3D9ResourceManager.h"

namespace xs
{


	VertexBufferD3D9 * VertexBufferD3D9Creater::create(BufferManagerD3D9 * pBufferMgr, uint vertexSize, uint numVertices, BufferUsage usage, CreateBufferFlag flag)
	{
		VertexBufferD3D9 * pVB = new VertexBufferD3D9();
		if( NULL == pVB)
			return 0;
		
		if( !pVB->create(pBufferMgr, vertexSize, numVertices,usage, flag) )
		{
			pVB->release();
			return 0;
		}

		return pVB;
	}

	VertexBufferD3D9::VertexBufferD3D9():
	m_pVB(0),
	m_uiVertexSize(0),
	m_uiNumVertices(0),
	m_uiSizeInBytes(0),
	m_eUsage(BU_STATIC),
	m_pBufferMgr(0),
	m_bIsLocked(false)
	{
		ZeroMemory(&m_bufferDesc, sizeof(m_bufferDesc));
	}

	VertexBufferD3D9::~VertexBufferD3D9()
	{
		
	}

	bool VertexBufferD3D9::create(BufferManagerD3D9 * pBufferMgr, uint vertexSize, uint numVertices, BufferUsage usage, CreateBufferFlag flag )
	{
		if( NULL == pBufferMgr)
			return false;
		if( vertexSize == 0 || numVertices == 0 )
			return false;

		RenderSystemD3D9 * pRenderSystem = pBufferMgr->getRenderSystem();
		Assert( pRenderSystem );
		D3D9Device * pDevice =  pRenderSystem->getD3D9Device();
		Assert( pDevice);
		IDirect3DDevice9 * pD3D9Device = pDevice->getDevice();
		Assert( pD3D9Device);

		//初始化
		m_uiVertexSize = vertexSize;
		m_uiNumVertices = numVertices;
		m_uiSizeInBytes = m_uiVertexSize * m_uiNumVertices;
		m_eUsage = usage;
		m_pBufferMgr = pBufferMgr;

		//等待可以创建资源的通知
		if( !(flag & CBF_IMMEDIATE) ) pRenderSystem->getD3D9ResourceMgr()->waitForEndFrame();
					
		//在这里锁定设备的访问
		pRenderSystem->getD3D9ResourceMgr()->lockDeviceAccess();

		HRESULT hr = pD3D9Device->CreateVertexBuffer(
			m_uiVertexSize * m_uiNumVertices,
			MappingsD3D9::getBufferUsage(m_eUsage),
			0,//不指定FVF格式
			MappingsD3D9::getPoolTypeByBufferUsage(m_eUsage),
			&m_pVB,
			NULL);

		//解锁设备的访问
		pRenderSystem->getD3D9ResourceMgr()->unlockDeviceAccess();

		if( FAILED(hr) )
		{
			return false;
		}
		
		m_pVB->GetDesc(&m_bufferDesc);

		//将该资源添加到资源管理器
		pRenderSystem->getD3D9ResourceMgr()->notifyResourceCreated(this);

		//添加到buffer管理器中
		m_pBufferMgr->addVertexBuffer(this);

		return true;
	}

	/** 释放顶点缓冲区对象
	*/
	void 	VertexBufferD3D9::release()
	{
		//从buffer 管理其中清除
		m_pBufferMgr->removeVertexBuffer(this);

		//清除自身
		m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->lockResourceAccess();	
		COM_SAFE_RELEASE( m_pVB);
		m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();

		//资源管理器释放该资源
		m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->notifyResourceRelease( this);
		
		//释放该对象
		delete this;
	}

	uint	VertexBufferD3D9::getVertexSize() const
	{
		return m_uiVertexSize;
	}

	uint	VertexBufferD3D9::getNumVertices() const
	{
		return m_uiNumVertices;
	}

	BufferUsage	VertexBufferD3D9::getUsage() const
	{
		return m_eUsage;
	}

	void *	VertexBufferD3D9::lock(uint offset,uint length,LockOptions options) 
	{
		m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->lockResourceAccess();
		if( NULL == m_pVB  || offset + length > m_bufferDesc.Size )
		{
			m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();
			return 0;
		}	

		//BL_DISCARD 和 BL_NO_OVERWRITE 只对 D3DPOOL_DEFAULT池有效
		if( ( options == BL_DISCARD || options == BL_NO_OVERWRITE )
			&& ( m_bufferDesc.Pool != D3DPOOL_DEFAULT) )
		{
			options = BL_NORMAL;
		}

		DWORD lockflags =0;
		switch( options)
		{
		case BL_NORMAL:
			break;
		case BL_DISCARD:
			lockflags = D3DLOCK_DISCARD;
			break;
		case BL_NO_OVERWRITE:
			lockflags = D3DLOCK_NOOVERWRITE;
			break;
		case BL_READ_ONLY:
			lockflags = D3DLOCK_READONLY;
			break;
		default:
			// 这不可能
			break;
		}

		void * pData = 0;
		HRESULT hr = m_pVB->Lock( offset, length, &pData, lockflags);
		if( FAILED(hr) )
		{
			m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();
			return 0;
		}
		m_bIsLocked = true;
		return pData;
	}

	void 	VertexBufferD3D9::unlock()
	{
		if( m_bIsLocked )
		{
			Assert(m_pVB);
			m_pVB->Unlock();
			m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();
			m_bIsLocked = false;
		}
	}

	bool	VertexBufferD3D9::isLocked()
	{
		return m_bIsLocked;
	}

	void 	VertexBufferD3D9::readData(uint offset,uint length,void * pDest)
	{
		if( NULL == pDest)
			return;
		if( length == 0 ) 
			return;
		if( m_eUsage & BU_WRITE_ONLY) //资源是只写的，读取失败
			return;

		//锁定资源的访问
		m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->lockResourceAccess();
		
		if( NULL == m_pVB || offset + length > m_bufferDesc.Size )//读取数据失败
		{
			m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();//解锁
			return;
		}
		void * pData = 0;
		HRESULT hr = m_pVB->Lock( offset, length, &pData, D3DLOCK_READONLY);
		if( FAILED(hr) )//锁定失败
		{
			m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();//解锁
			return;
		}
		m_bIsLocked = true;
		memcpy(pDest, pData, length);		
		m_pVB->Unlock();
		m_bIsLocked = false;
		m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();//解锁
		return;
	}


	void 	VertexBufferD3D9::writeData(																			
		uint offset,																	
		uint length,																	
		const void * pSource,															
		bool discardWholeBuffer )
	{
		if( NULL == pSource )
			return;
		if( length == 0 )
			return;

		//锁定资源访问
		m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->lockResourceAccess();

		if( NULL == m_pVB || offset + length > m_bufferDesc.Size )//写入数据失败
		{
			m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();//解锁
			return;
		}

		void * pData = 0;
		HRESULT hr;
		//discardWholeBuffer 只有当用途为D3DUSAGE_DYNAMIC时才有用
		if( (m_bufferDesc.Usage & D3DUSAGE_DYNAMIC) && discardWholeBuffer)
		{
			hr = m_pVB->Lock( offset, length, &pData, D3DLOCK_DISCARD); 
		}
		else
		{
			hr = m_pVB->Lock(offset, length, &pData, 0);
		}
		if( FAILED(hr) )//lock失败
		{
			m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();
			return;
		}
		m_bIsLocked = true;
		memcpy( pData,pSource,length);
		m_pVB->Unlock();
		m_bIsLocked = false;
		m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();
		return;
	}

	IVertexBuffer*	VertexBufferD3D9::clone()
	{
		//构建VertexBuffer
		IVertexBuffer * pVB = m_pBufferMgr->createVertexBuffer(
			m_uiVertexSize, m_uiNumVertices, m_eUsage);
		if( NULL == pVB)
			return 0;
		
		//加锁
		m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->lockResourceAccess();
		if( NULL == m_pVB)
		{
			pVB->release();
			m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();
			return 0;
		}
			
		//锁定目标缓存
		void * pDest = pVB->lock(0,0, BL_DISCARD);
		if( NULL == pDest )
		{
			pVB->release();
			m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();
			return 0;
		}

		//锁定源缓存
		void * pSrc = 0;
		if( m_eUsage & BU_WRITE_ONLY )
		{
			pSrc = this->lock(0,0, BL_NORMAL);
		}
		else
		{
			pSrc = this->lock(0,0, BL_READ_ONLY);
		}
		if( NULL == pSrc)
		{
			if( pDest) pVB->unlock();
			pVB->release();
			m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();
			return 0;
		}

		//数据拷贝
		memcpy( pDest, pSrc, m_uiSizeInBytes);


		//解锁
		this->unlock();
		pVB->unlock();
		m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();
		return pVB;
	}

	void VertexBufferD3D9::notifyOnDeviceLost()
	{
		//如果是Default Pool 管理的IndexBuffer，则释放该buffer
		if(m_bufferDesc.Pool == D3DPOOL_DEFAULT)
		{
			COM_SAFE_RELEASE(m_pVB);
			return;
		}
	}

	void VertexBufferD3D9::notifyOnDeviceReset()
	{
		//如果是Default Pool 管理的IndexBuffer，则重建该buffer
		if( m_bufferDesc.Pool == D3DPOOL_DEFAULT)
		{
			RenderSystemD3D9 * pRenderSystem = m_pBufferMgr->getRenderSystem();
			Assert(pRenderSystem);
			D3D9Device * pDevice = pRenderSystem->getD3D9Device();
			Assert( pDevice);
			IDirect3DDevice9 * pD3D9Device = pDevice->getDevice();
			Assert( pD3D9Device);	

			//创建buffer
			HRESULT hr = pD3D9Device->CreateVertexBuffer( 
				m_uiSizeInBytes,
				MappingsD3D9::getBufferUsage(m_eUsage),
				0,
				MappingsD3D9::getPoolTypeByBufferUsage(m_eUsage),
				&m_pVB,
				NULL);

			if( SUCCEEDED(hr) )
			{
				m_pVB->GetDesc(&m_bufferDesc);
			}
		}
	}

}