#include "StdAfx.h"
#include "BufferManagerD3D9.h"
#include "RenderSystemD3D9.h"
#include "VertexBufferD3D9.h"
#include "IndexBufferD3D9.h"


namespace xs
{


	BufferManagerD3D9 * BufferManagerD3D9Creater::create( RenderSystemD3D9 * pRenderSystem)
	{
		if( NULL == pRenderSystem)
			return 0;

		BufferManagerD3D9 * pMgr = new BufferManagerD3D9();
		if( !pMgr)
			return 0;

		pMgr->m_pRenderSystem = pRenderSystem;

		return pMgr;
	}



	BufferManagerD3D9::BufferManagerD3D9():
	m_pRenderSystem(0)
	{

	}
	BufferManagerD3D9::~BufferManagerD3D9()
	{

	}

	IVertexBuffer* BufferManagerD3D9::createVertexBuffer(uint vertexSize, uint numVertices, BufferUsage usage, CreateBufferFlag flag)
	{
		IVertexBuffer * pVertexBuffer =  VertexBufferD3D9Creater::create( this, vertexSize, numVertices,usage, flag);
		return pVertexBuffer;
	}

	IIndexBuffer*	BufferManagerD3D9::createIndexBuffer(IndexType itype,uint numIndices,BufferUsage usage, CreateBufferFlag flag )
	{
		IIndexBuffer * pIndexBuffer = IndexBufferD3D9Creater::create(this, itype, numIndices, usage, flag);
		return  pIndexBuffer;
	}

	VertexDeclaration* BufferManagerD3D9::createVertexDeclaration()
	{
		VertexDeclaration * pDecl = new VertexDeclaration;
		if( NULL != pDecl)
		{
			m_mutex.Lock();
			m_vVertexDeclarationContainer.insert(pDecl);
			m_mutex.Unlock();
		}
		return pDecl;
	}

	VertexBufferBinding* BufferManagerD3D9::createVertexBufferBinding()
	{
		VertexBufferBinding * pBind = new VertexBufferBinding;
		if( NULL != pBind)
		{
			m_mutex.Lock();
			m_vVertexBufferBindingContainer.insert(pBind);
			m_mutex.Unlock();
		}
		return pBind;
	}

	void  BufferManagerD3D9::destroyVertexDeclaration(VertexDeclaration* decl)
	{							
		m_mutex.Lock();
		std::set<VertexDeclaration * >::iterator it =  m_vVertexDeclarationContainer.find( decl);
		if( it == m_vVertexDeclarationContainer.end() )
		{
			m_mutex.Unlock();
		}
		else
		{
			m_mutex.Unlock();
			delete (*it);
		}
	}

	void  BufferManagerD3D9::destroyVertexBufferBinding(VertexBufferBinding* binding)
	{
		m_mutex.Lock();
		std::set< VertexBufferBinding * >::iterator it = m_vVertexBufferBindingContainer.find( binding);
		if( it == m_vVertexBufferBindingContainer.end() )
		{
			m_mutex.Unlock();
		}
		else
		{
			m_mutex.Unlock();
			delete (*it);
		}
	}

	void BufferManagerD3D9::release()
	{
		std::set< IIndexBuffer * >::iterator itI = m_vIndexBufferContainer.begin();
		while( itI != m_vIndexBufferContainer.end() )
		{
			(*itI)->release();
			itI = m_vIndexBufferContainer.begin();
		}

		// Õ∑≈vertex buffer
		std::set< IVertexBuffer * >::iterator itV = m_vVertexBufferContainer.begin();
		while( itV != m_vVertexBufferContainer.end() )
		{
			(*itV)->release();
			itV = m_vVertexBufferContainer.begin();
		}

		//VertexDeclaration
		std::set< VertexDeclaration * >::iterator itd = m_vVertexDeclarationContainer.begin();
		for( ; itd != m_vVertexDeclarationContainer.end(); ++itd)
		{
			delete (*itd);
		}
		m_vVertexDeclarationContainer.clear();

		//VertexBufferBinding
		std::set<VertexBufferBinding * >::iterator itb = m_vVertexBufferBindingContainer.begin();
		for( ; itb != m_vVertexBufferBindingContainer.end(); ++itb)
		{
			delete (*itb);
		}
		m_vVertexBufferBindingContainer.clear();

		delete this;
	}

	void BufferManagerD3D9::addIndexBuffer( IIndexBuffer * pIB)
	{	
		if( 0 == pIB) return;

		m_mutex.Lock();
		m_vIndexBufferContainer.insert(pIB);
		m_mutex.Unlock();

	}

	void BufferManagerD3D9::removeIndexBuffer( IIndexBuffer * pIB)
	{
		if( 0 == pIB) return; 

		m_mutex.Lock();
		m_vIndexBufferContainer.erase(pIB);
		m_mutex.Unlock();
	}


	void BufferManagerD3D9::addVertexBuffer( IVertexBuffer * pVB)
	{
		if(0 == pVB ) return;

		m_mutex.Lock();
		m_vVertexBufferContainer.insert(pVB);
		m_mutex.Unlock();

	}

	void BufferManagerD3D9::removeVertexBuffer( IVertexBuffer * pVB)
	{
		if( 0 == pVB) return;

		m_mutex.Lock();
		m_vVertexBufferContainer.erase(pVB);
		m_mutex.Unlock();			
	}

}