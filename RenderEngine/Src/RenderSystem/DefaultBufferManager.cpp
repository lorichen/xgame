#include "StdAfx.h"
#include "DefaultBufferManager.h"

namespace xs 
{

	DefaultVertexBuffer::DefaultVertexBuffer(IBufferManager *pBufferManager,uint vertexSize,uint numVertices,BufferUsage usage)
	{
		m_pBufferManager = pBufferManager;
		m_ui32VertexSize = vertexSize;
		m_ui32NumVertices = numVertices;
		m_ui32SizeInBytes = vertexSize * numVertices;
		mpData = new uchar[m_ui32SizeInBytes];
	}
	//-----------------------------------------------------------------------
	DefaultVertexBuffer::~DefaultVertexBuffer()
	{
		delete[] mpData;
	}
	//-----------------------------------------------------------------------
	void* DefaultVertexBuffer::lock(uint offset, uint length, LockOptions options)
	{
		return mpData + offset;
	}
	//-----------------------------------------------------------------------
	void DefaultVertexBuffer::unlock(void)
	{
	}
	//-----------------------------------------------------------------------
	bool	DefaultVertexBuffer::isLocked()
	{
		return false;
	}
	//-----------------------------------------------------------------------
	void DefaultVertexBuffer::readData(size_t offset, size_t length, void* pDest)
	{
		assert((offset + length) <= m_ui32SizeInBytes);
		memcpy(pDest, mpData + offset, length);
	}
	//-----------------------------------------------------------------------
	void DefaultVertexBuffer::writeData(size_t offset, size_t length, const void* pSource,
		bool discardWholeBuffer)
	{
		assert((offset + length) <= m_ui32SizeInBytes);
		// ignore discard, memory is not guaranteed to be zeroised
		memcpy(mpData + offset, pSource, length);

	}

	IVertexBuffer*	DefaultVertexBuffer::clone()
	{
		IVertexBuffer *pVertexBuffer = m_pBufferManager->createVertexBuffer(m_ui32VertexSize,m_ui32NumVertices,BU_STATIC_WRITE_ONLY);
		if(pVertexBuffer)
		{
			void  *pDest = pVertexBuffer->lock(0,0,BL_NORMAL);
			void  *pSrc = this->lock(0,0,BL_NORMAL);

			if(pSrc && pDest)
			{
				memcpy(pDest,pSrc,m_ui32VertexSize * m_ui32NumVertices);
			}
			if(pDest)pVertexBuffer->unlock();
			if(pSrc)this->unlock();
		}

		return pVertexBuffer;
	}

	uint DefaultVertexBuffer::getNumVertices() const { return m_ui32NumVertices; }
	uint DefaultVertexBuffer::getVertexSize() const { return m_ui32VertexSize; }
	BufferUsage	DefaultVertexBuffer::getUsage() const
	{
		return BU_STATIC_WRITE_ONLY;
	}
	void DefaultVertexBuffer::release(){delete this;}
	//-----------------------------------------------------------------------

	DefaultIndexBuffer::DefaultIndexBuffer(IndexType idxType, uint numIndexes,BufferUsage usage) 
	{
		switch (idxType)
		{
		case IT_16BIT:
			m_ui32IndexSize = sizeof(ushort);
			break;
		case IT_32BIT:
			m_ui32IndexSize = sizeof(uint);
			break;
		}

		m_ui32NumIndexes = numIndexes;
		m_ui32SizeInBytes = m_ui32NumIndexes * numIndexes;
		mpData = new uchar[m_ui32SizeInBytes];
	}
	//-----------------------------------------------------------------------
	DefaultIndexBuffer::~DefaultIndexBuffer()
	{
		delete[] mpData;
	}
	//-----------------------------------------------------------------------
	void* DefaultIndexBuffer::lock(size_t offset, size_t length, LockOptions options)
	{
		return mpData + offset;
	}
	//-----------------------------------------------------------------------
	void DefaultIndexBuffer::unlock(void)
	{
	}
	//-----------------------------------------------------------------------
	bool	DefaultIndexBuffer::isLocked()
	{
		return false;
	}
	//-----------------------------------------------------------------------
	void DefaultIndexBuffer::readData(size_t offset, size_t length, void* pDest)
	{
		assert((offset + length) <= m_ui32SizeInBytes);
		memcpy(pDest, mpData + offset, length);
	}
	//-----------------------------------------------------------------------
	void DefaultIndexBuffer::writeData(size_t offset, size_t length, const void* pSource,
		bool discardWholeBuffer)
	{
		assert((offset + length) <= m_ui32SizeInBytes);
		// ignore discard, memory is not guaranteed to be zeroised
		memcpy(mpData + offset, pSource, length);

	}
	//-----------------------------------------------------------------------

	IndexType DefaultIndexBuffer::getType(void) const { assert("should never get here");return IT_16BIT; }
	uint DefaultIndexBuffer::getNumIndices(void) const { return m_ui32NumIndexes; }
	uint DefaultIndexBuffer::getIndexSize(void) const { return m_ui32IndexSize; }
	BufferUsage	DefaultIndexBuffer::getUsage() const
	{
		return BU_STATIC_WRITE_ONLY;
	}

	void DefaultIndexBuffer::release(){delete this;}

	IVertexBuffer*	DefaultBufferManager::createVertexBuffer(uint vertexSize,uint numVertices,BufferUsage usage, CreateBufferFlag flag)
	{
		return new DefaultVertexBuffer(this,vertexSize,numVertices,usage);
	}

	IIndexBuffer*	DefaultBufferManager::createIndexBuffer(IndexType itype,uint numIndexes,BufferUsage usage, CreateBufferFlag flag)
	{
		return new DefaultIndexBuffer(itype,numIndexes,usage);
	}

	/// Creates a vertex declaration, may be overridden by certain rendering APIs
	VertexDeclaration* DefaultBufferManager::createVertexDeclaration()
	{
		return new VertexDeclaration;
	}

	/// Destroys a vertex declaration, may be overridden by certain rendering APIs
	void DefaultBufferManager::destroyVertexDeclaration(VertexDeclaration* decl)
	{
		delete decl;
	}

	/** Creates a new VertexBufferBinding. */
	VertexBufferBinding* DefaultBufferManager::createVertexBufferBinding()
	{
		return new VertexBufferBinding;
	}
	/** Destroys a VertexBufferBinding. */
	void DefaultBufferManager::destroyVertexBufferBinding(VertexBufferBinding* binding)
	{
		delete binding;
	}
}
