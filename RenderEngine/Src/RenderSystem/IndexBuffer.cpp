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

		glGenBuffersARB(1,&m_ui32BufferID);

		if (!m_ui32BufferID)
		{
			Error("Can't Create IndexBuffer\n");
			return;
		}

		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,m_ui32BufferID);

		// Initialise buffer and set usage
		glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER,ui32SizeInBytes,NULL,BufferManager::getGLUsage(usage));

		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,0);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffersARB(1,&m_ui32BufferID);
	}

	void* IndexBuffer::lock(uint offset,uint length,LockOptions options)
	{
		GLenum access = 0;

		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,m_ui32BufferID);

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

		void* pBuffer = glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER,access);

		if(pBuffer == 0)
		{
			Error("can't lock buffer");
		}

		m_bLocked = true;
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,0);
		return static_cast<void*>(static_cast<unsigned char*>(pBuffer) + offset);
	}

	void IndexBuffer::unlock()
	{
		glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER,m_ui32BufferID);

		if(!glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER))
		{
			Error("Can't UnmapBuffer\n");
		}
		m_bLocked = false;
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,0);
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
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,m_ui32BufferID);
		glGetBufferSubDataARB(GL_ELEMENT_ARRAY_BUFFER, offset, length, pDest);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,0);
	}
	//---------------------------------------------------------------------
	void IndexBuffer::writeData(uint offset,uint length,const void* pSource,bool discardWholeBuffer)
	{
		glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER,m_ui32BufferID);

		if(discardWholeBuffer)
		{
			glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER,m_ui32IndexSize * m_ui32NumIndexes,0,BufferManager::getGLUsage(m_Usage));
		}

		// Now update the real buffer
		glBufferSubDataARB(GL_ELEMENT_ARRAY_BUFFER,offset,length,pSource);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER,0);
	}
	GLint		IndexBuffer::getGLBufferId()
	{
		return m_ui32BufferID;
	}
}