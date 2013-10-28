#include "StdAfx.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "BufferManager.h"

namespace xs
{

	IVertexBuffer*	BufferManager::createVertexBuffer(uint vertexSize,uint numVertices,BufferUsage usage, CreateBufferFlag flag )
	{
		m_mutex.Lock();
		IVertexBuffer *pVertexBuffer = new VertexBuffer(this,vertexSize,numVertices,usage);
		m_mutex.Unlock();

		return pVertexBuffer;
	}

	IIndexBuffer*	BufferManager::createIndexBuffer(IndexType itype,uint numIndexes,BufferUsage usage, CreateBufferFlag flag)
	{
		m_mutex.Lock();
		IIndexBuffer *pIndexBuffer = new IndexBuffer(itype,numIndexes,usage);
		m_mutex.Unlock();

		return pIndexBuffer;
	}
	//---------------------------------------------------------------------
	GLenum BufferManager::getGLType(VertexElementType type)
	{
		switch(type)
		{
		case VET_FLOAT1:
		case VET_FLOAT2:
		case VET_FLOAT3:
		case VET_FLOAT4:
			return GL_FLOAT;
		case VET_SHORT1:
		case VET_SHORT2:
		case VET_SHORT3:
		case VET_SHORT4:
			return GL_SHORT;
		case VET_COLOR:
		case VET_UBYTE4:
			return GL_UNSIGNED_BYTE;
		default:
			return 0;
		};
	}
	GLenum BufferManager::getGLUsage(uint usage)
	{
		switch(usage)
		{
		case BU_STATIC:
		case BU_STATIC_WRITE_ONLY:
			return GL_STATIC_DRAW;
		case BU_DYNAMIC:
		case BU_DYNAMIC_WRITE_ONLY:
			return GL_DYNAMIC_DRAW_ARB;
		case BU_DYNAMIC_WRITE_ONLY_DISCARDABLE:
			return GL_STREAM_DRAW_ARB;
		default:
			return GL_DYNAMIC_DRAW_ARB;
		};
	}

	/// Creates a vertex declaration, may be overridden by certain rendering APIs
	VertexDeclaration* BufferManager::createVertexDeclaration()
	{
		return new VertexDeclaration;
	}

	/// Destroys a vertex declaration, may be overridden by certain rendering APIs
	void BufferManager::destroyVertexDeclaration(VertexDeclaration* decl)
	{
		delete decl;
	}

	/** Creates a new VertexBufferBinding. */
	VertexBufferBinding* BufferManager::createVertexBufferBinding()
	{
		return new VertexBufferBinding;
	}
	/** Destroys a VertexBufferBinding. */
	void BufferManager::destroyVertexBufferBinding(VertexBufferBinding* binding)
	{
		delete binding;
	}
}