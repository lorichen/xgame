#ifndef __VertexBufferD3D9_H__
#define __VertexBufferD3D9_H__

#include "D3D9ResourceManager.h"

namespace xs
{

	class BufferManagerD3D9;
	class VertexBufferD3D9;

	class VertexBufferD3D9Creater
	{
	public:
		static VertexBufferD3D9 * create(BufferManagerD3D9 * pBufferMgr, uint vertexSize, uint numVertices, BufferUsage usage, CreateBufferFlag flag);
	};

	class VertexBufferD3D9: public IVertexBuffer, public D3D9Resource
	{
		//IVertexBuffer接口
	public:
		/** 释放定点缓冲区对象
		*/
		virtual void 	release();

		/** 获得顶点大小，每个顶点占用的字节数
		@return 顶点大小
		*/
		virtual uint	getVertexSize() const;

		/** 获得顶点的数量
		@return 顶点数量
		*/
		virtual uint	getNumVertices() const;

		/** Buffer的使用方式
		@return Buffer的使用方式
		@see BufferUsage
		*/
		virtual BufferUsage	getUsage() const;

		/** 加锁缓冲区，获得数据的指针，从而可以操作数据
		@param offset 从数据开始的偏移值
		@param length 加锁的区域长度
		@param options 加锁的方式
		@see LockOptions
		@return 数据区指针
		*/
		virtual void *	lock(uint offset,uint length,LockOptions options) ;

		/** 解锁缓冲区，和lock配套使用
		*/
		virtual void 	unlock();

		/** 获取缓冲区是否处于被加锁状态
		@return true缓冲区被加锁，false未锁定
		*/
		virtual bool	isLocked() ;

		/** 从缓冲区读取数据，放到指定的缓存
		@param offset 从数据开始的偏移值
		@param length 要读取的数据长度
		@param pDest 目标缓冲区地址
		*/
		virtual void 	readData(uint offset,uint length,void * pDest) ;	

		/** 把数据写入缓冲区
		@param offset 从数据开始的偏移值
		@param length 要写入的的数据长度
		@param pSource 要写入的数据地址
		@param discardWholeBuffer 是否要抛弃整个buffer的数据(会提高速度)
		*/
		virtual void 	writeData(																			
			uint offset,																	
			uint length,																	
			const void * pSource,															
			bool discardWholeBuffer = false) ;

		/** 克隆顶点缓冲区的数据，用户负责返回的顶点缓冲区对象的生命周期
		@return 克隆出来的顶点缓冲区
		*/
		virtual IVertexBuffer*	clone();

		//D3D9Resource 接口
	public:

		// 设备丢失，在这里释放Default Pool管理的资源
		virtual void notifyOnDeviceLost();

		//设备恢复, 在这里重建Default Pool 管理的资源
		virtual void notifyOnDeviceReset();

	public:
		inline IDirect3DVertexBuffer9 * getD3DVertexBuffer9() { return m_pVB; }

	private:
		friend class VertexBufferD3D9Creater;
		bool create(BufferManagerD3D9 * pBufferMgr, uint vertexSize, uint numVertices, BufferUsage usage, CreateBufferFlag flag );
		VertexBufferD3D9();
		~VertexBufferD3D9();
		
	private:
		IDirect3DVertexBuffer9 *	m_pVB;				//顶点buffer
		uint						m_uiVertexSize;		//顶点大小	
		uint						m_uiNumVertices;	//顶点数量
		uint						m_uiSizeInBytes;	//buffer的大小
		BufferUsage					m_eUsage;			//buffer如何使用
		BufferManagerD3D9 *			m_pBufferMgr;		//buffer管理器
		D3DVERTEXBUFFER_DESC		m_bufferDesc;		//buffer的描述
		bool						m_bIsLocked;		//是否Locked
	};
}
#endif