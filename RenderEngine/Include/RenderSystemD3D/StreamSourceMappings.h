#ifndef __StreamSourceMappings_H__
#define __StreamSourceMappings_H__

namespace xs
{
	class RenderSystemD3D9;
	class StreamSourceMappings;

	class StreamSourceMappingsCreater
	{
	public:
		static StreamSourceMappings * create(RenderSystemD3D9 * pRenderSystem);
	};

	class StreamSourceMappings
	{
	public:
		/** 绑定顶点位置VBO
		*/
		void	setVertexVertexBuffer(IVertexBuffer* vertexBuffer,uint start);

		/** 绑定散射光VBO
		*/
		void	setDiffuseVertexBuffer(IVertexBuffer* vertexBuffer,uint start);

		/** 绑定镜面光VBO
		*/
		void	setSpecularVertexBuffer(IVertexBuffer* vertexBuffer,uint start);

		/** 绑定法向量VBO
		*/
		void	setNormalVertexBuffer(IVertexBuffer* vertexBuffer,uint start);
		
		/** 绑定纹理VBO
		*/
		void	setTexcoordVertexBuffer(ushort unit,IVertexBuffer* vertexBuffer,uint start);

		/** 绑定顶点混合的权重
		*/
		void		setBlendWeightVertexBuffer(IVertexBuffer* vertexBuffer, uint start);

		/** 绑定顶点混合的矩阵索引
		*/
		void		setBlendIndicesVertexBuffer(IVertexBuffer* vertexBuffer, uint start);

		/** 设置索引缓存
		*/
		void	setIndexBuffer(IIndexBuffer* indexBuffer);

		/** 获取相应的的顶点声明
		*/
		IDirect3DVertexDeclaration9 * getVertexDeclaration();


		/** 释放该对象
		*/
		void release();

		/** 获取有多少个顶点
		*/
		UINT	getNumVertices();

	private:
		friend class StreamSourceMappingsCreater;
		StreamSourceMappings();
		~StreamSourceMappings();
	private:
		enum VertexDeclarationType
		{
			EVDT_Position =			0x00000001,		//位置
			EVDT_Normal =			0x00000002,		//法向量
			EVDT_DiffuseColor =		0x00000004,		//diffuse颜色
			EVDT_SpecularColor =	0x00000008,		//specular颜色
			EVDT_Tex0 =				0x00000010,		//0号纹理映射单元
			EVDT_Tex1 =				0x00000020,		//1号纹理映射单元
			EVDT_Tex2 =				0x00000040,		//2号纹理映射单元
			EVDT_Tex3 =				0x00000080,		//3号纹理映射单元
			EVDT_Tex4 =				0x00000100,		//4号纹理映射单元
			EVDT_Tex5 =				0x00000200,		//5号纹理映射单元
			EVDT_Tex6 =				0x00000400,		//6号纹理映射单元
			EVDT_Tex7 =				0x00000800,		//7号纹理映射单元
			EVDT_BlendWeight =		0x00001000,		//顶点混合权重
			EVDT_BlendIndices =		0x00002000,		//顶点混合的矩阵索引
			EVDT_TypeNum  =			14,				//声明的数量	
		};
		DWORD m_dwStreamsFlag;						//流标识号
	private:
		uint getVertexType2StreamIndex(VertexDeclarationType type);
		struct VertexDeclarationRecord
		{
			bool m_bValid;//是否有效
			D3DVERTEXELEMENT9 m_element;//
		};
		VertexDeclarationRecord m_streamBindRecord[EVDT_TypeNum];


		typedef std::map<DWORD, IDirect3DVertexDeclaration9 * > VertexDeclarationContainer;
		typedef VertexDeclarationContainer::iterator		VertexDeclarationContainerIterator;
		VertexDeclarationContainer	m_vDecls; //顶点声明集合;
		RenderSystemD3D9 *			m_pRenderSystem;//渲染系统指针

		UINT						m_uiNumVertices;//顶点的数量
	};
}

#endif