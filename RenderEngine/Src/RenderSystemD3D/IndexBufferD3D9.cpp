#include "StdAfx.h"
#include "IndexBufferD3D9.h"
#include "BufferManagerD3D9.h"
#include "RenderSystemD3D9.h"
#include "D3D9ResourceManager.h"
#include "D3D9Device.h"
#include "MappingsD3D9.h"


namespace xs
{

	IndexBufferD3D9 * IndexBufferD3D9Creater::create(BufferManagerD3D9 * pMgr,IndexType itype,uint numIndices,BufferUsage usage, CreateBufferFlag flag )
	{	
		IndexBufferD3D9 * pIB = new IndexBufferD3D9();
		if( !pIB->create(pMgr, itype, numIndices, usage, flag) )
		{
			pIB->release();
			return 0;
		}

		return pIB;
	}
	
	IndexBufferD3D9::IndexBufferD3D9():
	m_pBufferMgr(0),
	m_pIB(0),
	m_eIndexType(IT_16BIT),
	m_uiNumIndices(0),
	m_eUsage(BU_STATIC),
	m_uiIndexSize(0),
	m_bIsLocked(false)
	{
		ZeroMemory(&m_bufferDesc, sizeof(m_bufferDesc) );
	}

	IndexBufferD3D9::~IndexBufferD3D9()
	{
		
	}

	bool IndexBufferD3D9::create(BufferManagerD3D9 * pMgr,IndexType itype,uint numIndices,BufferUsage usage, CreateBufferFlag flag )
	{
		if( NULL == pMgr) return false;

		RenderSystemD3D9 * pRenderSystem = pMgr->getRenderSystem();
		Assert(pRenderSystem);
		D3D9Device * pDevice = pRenderSystem->getD3D9Device();
		Assert(pDevice);
		IDirect3DDevice9 * pD3D9Device = pDevice->getDevice();
		Assert(pD3D9Device);

		//初始化
		m_pBufferMgr = pMgr;
		m_eIndexType = itype;
		m_uiIndexSize = MappingsD3D9::getIndexTypeSize(m_eIndexType);
		m_uiNumIndices = numIndices;
		m_eUsage = usage;

		//等待可以创建资源的通知
		if( !( flag & CBF_IMMEDIATE) )
			pRenderSystem->getD3D9ResourceMgr()->waitForEndFrame();
		
		//锁定设备访问
		pRenderSystem->getD3D9ResourceMgr()->lockDeviceAccess();

		//创建buffer
		HRESULT hr = pD3D9Device->CreateIndexBuffer(  (m_uiIndexSize * m_uiNumIndices),
			MappingsD3D9::getBufferUsage(m_eUsage),
			MappingsD3D9::getIndexTypeFormat(m_eIndexType),
			MappingsD3D9::getPoolTypeByBufferUsage(m_eUsage),
			&m_pIB,
			NULL);
		
		//解锁设备访问
		pRenderSystem->getD3D9ResourceMgr()->unlockDeviceAccess();

		if( FAILED(hr) )
		{
			return false;
		}

		//获取描述
		m_pIB->GetDesc(&m_bufferDesc);

		//将该资源添加到资源管理器
		pRenderSystem->getD3D9ResourceMgr()->notifyResourceCreated(this);

		//添加到buffer 管理器中
		m_pBufferMgr->addIndexBuffer(this);

		return true;
	}

	void IndexBufferD3D9::release()
	{
		//从buffer 管理器中清除
		m_pBufferMgr->removeIndexBuffer( this);

		//清除自身
		m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->lockResourceAccess();	
		COM_SAFE_RELEASE( m_pIB);
		m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();

		//将该资源从资源管理器清除
		m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->notifyResourceRelease(this);

		//释放该对象
		delete this;
	}


	IndexType IndexBufferD3D9::getType(void ) const
	{
		return m_eIndexType;
	}

	uint IndexBufferD3D9::getNumIndices(void ) const
	{
		return m_uiNumIndices;
	}


	uint IndexBufferD3D9::getIndexSize(void ) const
	{
		return m_uiIndexSize;
	}

	BufferUsage	IndexBufferD3D9::getUsage() const
	{
		return m_eUsage;
	}

	void * IndexBufferD3D9::lock(uint offset,uint length,LockOptions options)
	{
		m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->lockResourceAccess();
		if( NULL == m_pIB  || offset + length > m_bufferDesc.Size    )
		{
			m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();
			return 0;
		}	

		if( ( options == BL_DISCARD || options == BL_NO_OVERWRITE )
			&& ( m_bufferDesc.Pool != D3DPOOL_DEFAULT) )
		{
			m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();
			return 0;
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
		HRESULT hr = m_pIB->Lock( offset, length, &pData, lockflags);
		if( FAILED(hr) )
		{
			m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();
			return 0;
		}
		m_bIsLocked = true;
		return pData;
	}

	void IndexBufferD3D9::unlock()
	{
		if( m_bIsLocked )
		{
			Assert(m_pIB);
			m_pIB->Unlock();		
			m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();
			m_bIsLocked = false;
		}
	}

	bool IndexBufferD3D9::isLocked()
	{
		return m_bIsLocked;
	}

	void IndexBufferD3D9::readData(uint offset,uint length,void * pDest)
	{
		if( NULL == pDest)
			return;
		if( length == 0 ) 
			return;
		if( m_eUsage & BU_WRITE_ONLY) //资源是只写的，读取失败
			return;

		//锁定资源的访问
		m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->lockResourceAccess();
		
		if( NULL == m_pIB || offset + length > m_bufferDesc.Size )//读取数据失败
		{
			m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();//解锁
			return;
		}
		void * pData = 0;
		HRESULT hr = m_pIB->Lock( offset, length, &pData, D3DLOCK_READONLY);
		if( FAILED(hr) )//锁定失败
		{
			m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();//解锁
			return;
		}
		m_bIsLocked = true;
		memcpy(pDest, pData, length);		
		m_pIB->Unlock();
		m_bIsLocked = false;
		m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();//解锁
		return;
	}

	void IndexBufferD3D9::writeData(																			
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

		if( NULL == m_pIB || offset + length > m_bufferDesc.Size )//写入数据失败
		{
			m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();//解锁
			return;
		}

		void * pData = 0;
		HRESULT hr;
		if( (m_bufferDesc.Usage & D3DUSAGE_DYNAMIC) && discardWholeBuffer)
		{
			hr = m_pIB->Lock( offset, length, &pData, D3DLOCK_DISCARD); 
		}
		else
		{
			hr = m_pIB->Lock(offset, length, &pData, 0);
		}
		if( FAILED(hr) )//lock失败
		{
			m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();
			return;
		}
		m_bIsLocked = true;
		memcpy( pData,pSource,length);
		m_pIB->Unlock();
		m_bIsLocked = false;
		m_pBufferMgr->getRenderSystem()->getD3D9ResourceMgr()->unlockResourceAccess();
		return;
	}

	void IndexBufferD3D9::notifyOnDeviceLost()
	{
		//如果是Default Pool 管理的IndexBuffer，则释放该buffer
		if(m_bufferDesc.Pool == D3DPOOL_DEFAULT)
		{
			COM_SAFE_RELEASE(m_pIB);
			return;
		}

	}

	void IndexBufferD3D9::notifyOnDeviceReset()
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
			HRESULT hr = pD3D9Device->CreateIndexBuffer(  (m_uiIndexSize * m_uiNumIndices),
				MappingsD3D9::getBufferUsage(m_eUsage),
				MappingsD3D9::getIndexTypeFormat(m_eIndexType),
				MappingsD3D9::getPoolTypeByBufferUsage(m_eUsage),
				&m_pIB,
				NULL);

			if( SUCCEEDED(hr) )
			{
				m_pIB->GetDesc(&m_bufferDesc);
			}
		}
	}
	
}
