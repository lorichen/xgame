#ifndef __BufferManager_H__
#define __BufferManager_H__

namespace xs
{
	class BufferManager : public IBufferManager
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
		BufferManager(){}
	public:
		static IBufferManager*	Instance()
		{
			static BufferManager bm;
			return &bm;
		}
	public:
		static GLenum BufferManager::getGLUsage(uint usage);
		static GLenum BufferManager::getGLType(VertexElementType type);
	private:
		xs::Mutex m_mutex;
	};

}
#endif