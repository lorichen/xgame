#include "StdAfx.h"
#include "IndexBuffer.h"
#include "BufferManager.h"

namespace xs
{
	IndexBuffer::IndexBuffer(IndexType idxType, uint numIndexes,BufferUsage usage) : 
		m_Usage(usage),
		m_IndexType(idxType),
		m_ui32NumIndexes(numIndexes),m_bLocked(false)
	{
		m_pbuffer = 0;

		switch (idxType)
		{
		case IT_16BIT:
			m_ui32IndexSize = sizeof(unsigned short);
			break;
		case IT_32BIT:
			m_ui32IndexSize = sizeof(unsigned int);
			break;
		}

		uint ui32SizeInBytes = m_ui32IndexSize * m_ui32NumIndexes;

		glGenBuffers(1,&m_ui32BufferID);

		if (!m_ui32BufferID)
		{
			Error("Can't Create IndexBuffer\n");
			return;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ui32BufferID);

		// Initialise buffer and set usage
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,ui32SizeInBytes,NULL,BufferManager::getGLUsage(usage));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1,&m_ui32BufferID);

		if(m_pbuffer)
		{
			delete[] m_pbuffer;
		}
	}

	void* IndexBuffer::lock(uint offset,uint length,LockOptions options)
	{
		GLenum access = 0;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ui32BufferID);

		if(options == BL_DISCARD)
		{
			//TODO: really we should use this to indicate our discard of the buffer
			//However it makes no difference to fps on nVidia, and can crash some ATI
			//glBufferData_ptr(GL_ELEMENT_ARRAY_BUFFER, mSizeInBytes, NULL, 
			//    GLHardwareBufferManager::getGLUsage(m_Usage));

			// TODO: we should be using the below implementation, but nVidia cards
			// choke on it and perform terribly - for investigation with nVidia
			access = (m_Usage == BU_DYNAMIC || m_Usage == BU_STATIC) ? GL_READ_WRITE : GL_WRITE_ONLY;
			//access = GL_READ_WRITE;
		}
		else if(options == BL_READ_ONLY)
		{
			if(m_Usage == BU_WRITE_ONLY)
			{
				Error("Invalid attempt to lock a write-only index buffer as read-only");
			}
			access = GL_READ_ONLY;
		}
		else if(options == BL_NORMAL || options == BL_NO_OVERWRITE)
		{
			// TODO: we should be using the below implementation, but nVidia cards
			// choke on it and perform terribly - for investigation with nVidia
			access = (m_Usage == BU_DYNAMIC || m_Usage == BU_STATIC) ? GL_READ_WRITE : GL_WRITE_ONLY;
			//access = GL_READ_WRITE;
		}
		else
		{
			//default 
			access = GL_READ_WRITE;
		}

		void* pBuffer = 0;
#if 0
	    pBuffer =glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,access);
#else
		if(m_pbuffer)
		{
			delete[] m_pbuffer;
			m_pbuffer = 0;
		}
		if(length == 0)
		{
			length = m_ui32NumIndexes * ((m_IndexType == IT_16BIT)?sizeof(short):sizeof(int));
		}
		m_pbuffer = new char[length];
		m_offset = offset;
		m_bufferSize = length;
		pBuffer = m_pbuffer;
#endif
		if(pBuffer == 0)
		{
			Error("can't lock buffer");
		}

		m_bLocked = true;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
		return static_cast<void*>(static_cast<unsigned char*>(pBuffer) + offset);
	}

	void IndexBuffer::unlock()
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,m_ui32BufferID);
#if 0
		if(!glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER))
		{
			Error("Can't UnmapBuffer\n");
		}
#else
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,m_offset,m_bufferSize,m_pbuffer);
		if(m_pbuffer)
		{
			delete[] m_pbuffer;
			m_pbuffer = 0;
		}
#endif
		m_bLocked = false;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	}

	bool	IndexBuffer::isLocked()
	{
		return m_bLocked;
	}

	IndexType IndexBuffer::getType(void) const { return m_IndexType; }
	uint IndexBuffer::getNumIndices(void) const { return m_ui32NumIndexes; }
	uint IndexBuffer::getIndexSize(void) const { return m_ui32IndexSize; }
	BufferUsage	IndexBuffer::getUsage() const
	{
		return m_Usage;
	}

	void IndexBuffer::release(){delete this;}

	//---------------------------------------------------------------------
	void IndexBuffer::readData(uint offset,uint length,void* pDest)
	{
		assert(0);
		/*
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ui32BufferID);
		glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, length, pDest);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
		*/
	}
	//---------------------------------------------------------------------
	void IndexBuffer::writeData(uint offset,uint length,const void* pSource,bool discardWholeBuffer)
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER,m_ui32BufferID);

		if(discardWholeBuffer)
		{
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,m_ui32IndexSize * m_ui32NumIndexes,0,BufferManager::getGLUsage(m_Usage));
		}

		// Now update the real buffer
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,offset,length,pSource);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	}
	GLint		IndexBuffer::getGLBufferId()
	{
		return m_ui32BufferID;
	}
}