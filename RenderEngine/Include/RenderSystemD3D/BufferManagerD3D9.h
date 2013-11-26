#ifndef __BufferManagerD3D9_H__
#define __BufferManagerD3D9_H__
#include <set>

namespace xs
{
	class BufferManagerD3D9;
	class RenderSystemD3D9;

	class BufferManagerD3D9Creater //buffer manager 工厂
	{
	public:
		static BufferManagerD3D9 * create( RenderSystemD3D9 * pRenderSystem);
	};

	class BufferManagerD3D9 : public IBufferManager
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

	public:
		void	addIndexBuffer( IIndexBuffer * pIB);
		void	removeIndexBuffer( IIndexBuffer * pIB);
		void	addVertexBuffer( IVertexBuffer * pVB);
		void	removeVertexBuffer( IVertexBuffer * pVB);

	public:
		/** 释放Buffer管理器
		*/
		void release();

		/** 获取关联的渲染系统
		*/
		inline RenderSystemD3D9 * getRenderSystem() { return m_pRenderSystem; }

	private:
		friend class BufferManagerD3D9Creater;
		BufferManagerD3D9();
		~BufferManagerD3D9();	
	private:
		RenderSystemD3D9	*	m_pRenderSystem;//关联的渲染系统
		std::set< IIndexBuffer * >			m_vIndexBufferContainer;
		std::set< IVertexBuffer * >			m_vVertexBufferContainer;
		std::set< VertexDeclaration * >		m_vVertexDeclarationContainer;
		std::set< VertexBufferBinding * >	m_vVertexBufferBindingContainer;
		xs::Mutex							m_mutex;
	};

}
#endif