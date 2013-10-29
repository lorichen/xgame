#include "StdAfx.h"
#include "VertexBuffer.h"
#include "BufferManager.h"

namespace xs
{
	VertexBuffer::VertexBuffer(IBufferManager *pBufferManager,uint vertexSize,uint numVertices,BufferUsage usage) : m_ui32VertexSize(vertexSize),m_ui32NumVertices(numVertices),m_Usage(usage),m_bLocked(false)
	{
		m_pBufferManager = pBufferManager;
		m_pbuffer = 0;

		glGenBuffers(1,&m_ui32BufferID);

		if (!m_ui32BufferID)
		{
			Error("Can't Create VertexBuffer\n");
		}

		glBindBuffer(GL_ARRAY_BUFFER,m_ui32BufferID);

		// Initialise mapped buffer and set usage
		glBufferData(GL_ARRAY_BUFFER,vertexSize * numVertices,NULL,BufferManager::getGLUsage(usage));

		glBindBuffer(GL_ARRAY_BUFFER,0);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1,&m_ui32BufferID);

		if(m_pbuffer)
		{
			delete[] m_pbuffer;
		}
	}

	
	void* VertexBuffer::lock(uint offset,uint length,LockOptions options)
	{
		if(m_bLocked)return 0;

		GLenum access = 0;

		glBindBuffer(GL_ARRAY_BUFFER,m_ui32BufferID);

		if(options == BL_DISCARD)
		{
			//TODO: really we should use this to indicate our discard of the buffer
			//However it makes no difference to fps on nVidia, and can crash some ATI
			//glBufferData_ptr(GL_ELEMENT_ARRAY_BUFFER, mSizeInBytes, NULL, 
			//    GLHardwareBufferManager::getGLUsage(mUsage));

			// TODO: we should be using the below implementation, but nVidia cards
			// choke on it and perform terribly - for investigation with nVidia
			//access = (mUsage == BU_DYNAMIC || mUsage == BU_STATIC) ? 
			//    GL_READ_WRITE : GL_WRITE_ONLY;
			access = GL_READ_WRITE;

		}
		else if(options == BL_READ_ONLY)
		{
			if(m_Usage == BU_WRITE_ONLY)
			{
				Warning("GLHardwareVertexBuffer: Locking a write-only vertex buffer for reading, performance TRACE0_WARNING.\n");
			}
			access = GL_READ_ONLY;
		}
		else if(options == BL_NORMAL || options == BL_NO_OVERWRITE)
		{
			// TODO: we should be using the below implementation, but nVidia cards
			// choke on it and perform terribly - for investigation with nVidia
			//access = (mUsage == BU_DYNAMIC || mUsage == BU_STATIC) ? 
			//    GL_READ_WRITE : GL_WRITE_ONLY;
			access = GL_READ_WRITE;
		}
		else
		{
			//default
			access = GL_READ_WRITE;
		}

		void* pBuffer = 0;
#if 0		
		//do not use glMapbuffer!
		pBuffer = glMapBuffer( GL_ARRAY_BUFFER,access);
#else
		if(m_pbuffer)
		{
			delete[] m_pbuffer;
			m_pbuffer = 0;
		}
		if(length == 0)
		{
			length = m_ui32VertexSize * m_ui32NumVertices;
		}
		m_pbuffer = new char[length];
		m_offset = offset;
		m_bufferSize = length;
		pBuffer = m_pbuffer;
#endif
		if(pBuffer == 0)
		{
			return 0;
		}
		m_bLocked = true;
		glBindBuffer(GL_ARRAY_BUFFER,0);
		return static_cast<void*>(static_cast<unsigned char*>(pBuffer) + offset);
	}

	void VertexBuffer::unlock()
	{
		glBindBuffer(GL_ARRAY_BUFFER,m_ui32BufferID);

#if 0
		if(!glUnmapBuffer(GL_ARRAY_BUFFER))
		{
			Error("Vertex Buffer Data Crupted!");
		}
#else
		glBufferSubData(GL_ARRAY_BUFFER,m_offset,m_bufferSize,m_pbuffer);
		if(m_pbuffer)
		{
			delete[] m_pbuffer;
			m_pbuffer = 0;
		}
#endif
		m_bLocked = false;
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}

	bool	VertexBuffer::isLocked()
	{
		return m_bLocked;
	}

	uint VertexBuffer::getVertexSize() const { return m_ui32VertexSize; }

	uint VertexBuffer::getNumVertices() const { return m_ui32NumVertices; }
	BufferUsage	VertexBuffer::getUsage() const
	{
		return m_Usage;
	}

	void VertexBuffer::release(){delete this;}

	void	VertexBuffer::readData(uint offset,uint length,void* pDest)
	{
		assert(0);
		/*
		glBindBuffer(GL_ARRAY_BUFFER,m_ui32BufferID);
		glGetBufferSubData(GL_ARRAY_BUFFER,offset,length,pDest);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		*/
	}

	void	VertexBuffer::writeData(uint offset,uint length,const void* pSource,bool discardWholeBuffer)
	{
        glBindBuffer(GL_ARRAY_BUFFER,m_ui32BufferID);

		if(discardWholeBuffer)
		{
			glBufferData(GL_ARRAY_BUFFER,m_ui32VertexSize * m_ui32NumVertices,0,BufferManager::getGLUsage(m_Usage));
		}

		// Now update the real buffer
		glBufferSubData(GL_ARRAY_BUFFER,offset,length,pSource);
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}

	IVertexBuffer*	VertexBuffer::clone()
	{
		IVertexBuffer *pVertexBuffer = m_pBufferManager->createVertexBuffer(m_ui32VertexSize,m_ui32NumVertices,m_Usage);
		if(pVertexBuffer)
		{
			void  *pDest = pVertexBuffer->lock(0,0,BL_DISCARD);
			void  *pSrc = 0;
			if(m_Usage & BU_WRITE_ONLY)
				pSrc = this->lock(0,0,BL_NORMAL);
			else
				pSrc = this->lock(0,0,BL_READ_ONLY);

			if(pSrc && pDest)
			{
				memcpy(pDest,pSrc,m_ui32VertexSize * m_ui32NumVertices);
			}
			if(pDest)pVertexBuffer->unlock();
			if(pSrc)this->unlock();
		}

		return pVertexBuffer;
	}

	GLint		VertexBuffer::getGLBufferId()
	{
		return m_ui32BufferID;
	}
}