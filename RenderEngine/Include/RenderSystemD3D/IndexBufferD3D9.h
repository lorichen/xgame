#ifndef __IndexBufferD3D9_H__
#define __IndexBufferD3D9_H__

#include "D3D9ResourceManager.h"

namespace xs
{
	class IndexBufferD3D9;
	class BufferManagerD3D9;

	class IndexBufferD3D9Creater
	{
	public:
		static IndexBufferD3D9 * create(BufferManagerD3D9 * pMgr,IndexType itype,uint numIndices,BufferUsage usage, CreateBufferFlag flag );
	};

	class IndexBufferD3D9 : public IIndexBuffer,  public D3D9Resource
	{
		//IIndexBuffer接口
	public:
		/** 获得索引缓冲区类型，参考IndexType枚举类型
		@see IndexType
		@return 缓冲区类型
		*/
		virtual IndexType	getType(void ) const;							

		/** 获取索引的数量
		@return 索引的数量
		*/
		virtual uint		getNumIndices(void ) const;						

		/** 获得索引的长度(字节)
		@return 每个索引的长度，一般为2,4
		*/
		virtual uint		getIndexSize(void ) const;						

		/** 获得索引缓冲区的使用方式，参考BufferUsage
		@see BufferUsage
		@return 索引缓冲区的使用方式
		*/
		virtual BufferUsage	getUsage() const;

		/** 加锁缓冲区，从来可以直接存取数据
		@param offset 从数据开始的偏移（字节）
		@param length 要加锁的数据区的长度
		@param options 加锁的方式
		@see LockOptions
		@return 直接数据的地址
		*/
		virtual void *		lock(uint offset,uint length,LockOptions options);						

		/** 解锁缓冲区，和lock配套使用
		*/
		virtual void 		unlock();		

		/** 获取索引缓冲区是否处于加锁状态
		@return true表示处于加锁状态，false则未加锁
		*/
		virtual bool		isLocked();		

		/** 释放缓冲区对象
		*/
		virtual void 		release();		

		/** 从缓冲区读取数据，放到指定的缓存
		@param offset 从数据开始的偏移值
		@param length 要读取的数据长度
		@param pDest 目标缓冲区地址
		*/
		virtual void 	readData(uint offset,uint length,void * pDest);	

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
			bool discardWholeBuffer = false);

		//D3D9Resource回调接口，不要主动调这些函数，使用D3D9ResourceManager的方法调用
	public:

		// 设备丢失，在这里释放Default Pool管理的资源
		virtual void notifyOnDeviceLost();

		//设备恢复, 在这里重建Default Pool 管理的资源
		virtual void notifyOnDeviceReset();

	public:
		inline IDirect3DIndexBuffer9 * getD3DIndexBuffer9() { return m_pIB; }

	private:
		friend class IndexBufferD3D9Creater;
		IndexBufferD3D9();
		~IndexBufferD3D9();
		bool create(BufferManagerD3D9 * pMgr,IndexType itype,uint numIndices,BufferUsage usage, CreateBufferFlag flag );
	private:
		BufferManagerD3D9 *		m_pBufferMgr;	//buffer管理器
		IDirect3DIndexBuffer9 * m_pIB;			//index buffer
		IndexType				m_eIndexType;	//索引类型
		uint					m_uiNumIndices;	//索引数量
		uint					m_uiIndexSize;	//索引的大小
		BufferUsage				m_eUsage;		//buffer使用方式
		D3DINDEXBUFFER_DESC		m_bufferDesc;	//buffer描述
		bool					m_bIsLocked;	//是否被锁定
	};
}
#endif