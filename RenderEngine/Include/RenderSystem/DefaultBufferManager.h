#ifndef __DefaultBufferManager_H__
#define __DefaultBufferManager_H__

namespace xs 
{
	struct IBufferManager;

	class DefaultBufferManager : public IBufferManager
	{
	public:
		/** 创建顶点缓冲区
		@param vertexSize 每个顶点的数据长度(字节)
		@param numVertices 顶点的数量
		@param usage 缓冲区的使用方式，参考BufferUsage
		@see BufferUsage
		@return 顶点缓冲区接口
		*/
		virtual IVertexBuffer*	createVertexBuffer(uint vertexSize,uint numVertices,BufferUsage usage, CreateBufferFlag flag = CBF_DEFAULT);

		/** 创建索引缓冲区
		@param itype 索引类型
		@see IndexType
		@param numIndices 索引的数量
		@param usage 缓冲区的使用方式
		@see BufferUsage
		@return 索引缓冲区接口
		*/
		virtual IIndexBuffer*	createIndexBuffer(IndexType itype,uint numIndices,BufferUsage usage, CreateBufferFlag flag = CBF_DEFAULT);	

		/** 创建顶点的定义
		@param itype 索引类型
		@see IndexType
		@param numIndices 索引的数量
		@param usage 缓冲区的使用方式
		@see BufferUsage
		@return 索引缓冲区接口
		*/
		virtual VertexDeclaration* createVertexDeclaration();				

		/** 创建顶点缓冲区绑定
		@return 顶点缓冲区绑定指针
		*/
		virtual VertexBufferBinding* createVertexBufferBinding();			

		/** 销毁顶点定义
		@param decl 顶点定义
		*/
		virtual void  destroyVertexDeclaration(VertexDeclaration* decl);		

		/** 销毁顶点缓冲区绑定
		@param binding 顶点缓冲区绑定
		*/
		virtual void  destroyVertexBufferBinding(VertexBufferBinding* binding);
	protected:
		DefaultBufferManager(){}
	public:
		static IBufferManager*	Instance()
		{
			static DefaultBufferManager bm;
			return &bm;
		}
	};

	class DefaultIndexBuffer : public IIndexBuffer
	{
	public:
		/** 获得索引缓冲区类型，参考IndexType枚举类型
		@see IndexType
		@return 缓冲区类型
		*/
		virtual IndexType	getType(void ) const ;							

		/** 获取索引的数量
		@return 索引的数量
		*/
		virtual uint		getNumIndices(void ) const ;						

		/** 获得索引的长度(字节)
		@return 每个索引的长度，一般为2,4
		*/
		virtual uint		getIndexSize(void ) const ;						

		/** 获得索引缓冲区的使用方式，参考BufferUsage
		@see BufferUsage
		@return 索引缓冲区的使用方式
		*/
		virtual BufferUsage	getUsage() const ;

		/** 加锁缓冲区，从来可以直接存取数据
		@param offset 从数据开始的偏移（字节）
		@param length 要加锁的数据区的长度
		@param options 加锁的方式
		@see LockOptions
		@return 直接数据的地址
		*/
		virtual void *		lock(uint offset,uint length,LockOptions options) ;						

		/** 解锁缓冲区，和lock配套使用
		*/
		virtual void 		unlock() ;		

		/** 获取索引缓冲区是否处于加锁状态
		@return true表示处于加锁状态，false则未加锁
		*/
		virtual bool		isLocked() ;		

		/** 释放缓冲区对象
		*/
		virtual void 		release() ;		

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
	protected:
		uchar* mpData;
		uint m_ui32NumIndexes;
		uint m_ui32IndexSize;
		uint		m_ui32SizeInBytes;
	public:
		DefaultIndexBuffer(IndexType idxType, uint numIndexes,BufferUsage usage);
		~DefaultIndexBuffer();
		void* getDataPtr(size_t offset) const { return (void*)(mpData + offset); }
	};

	class DefaultVertexBuffer : public IVertexBuffer
	{
	public:
		/** 释放定点缓冲区对象
		*/
		virtual void 	release() ;

		/** 获得顶点大小，每个顶点占用的字节数
		@return 顶点大小
		*/
		virtual uint	getVertexSize() const ;

		/** 获得顶点的数量
		@return 顶点数量
		*/
		virtual uint	getNumVertices() const ;

		/** Buffer的使用方式
		@return Buffer的使用方式
		@see BufferUsage
		*/
		virtual BufferUsage	getUsage() const ;

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
		virtual void 	unlock() ;

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
		virtual IVertexBuffer*	clone() ;
	protected:
		uchar* mpData;
		uint		m_ui32NumVertices;
		uint		m_ui32VertexSize;
		uint		m_ui32SizeInBytes;
		IBufferManager*	m_pBufferManager;
	public:
		DefaultVertexBuffer(IBufferManager *pBufferManager,uint vertexSize,uint numVertices,BufferUsage usage);
		~DefaultVertexBuffer();

		void* getDataPtr(size_t offset) const { return (void*)(mpData + offset); }
	};

}

#endif
