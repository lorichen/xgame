#ifndef __BeginEndPrimitiveProxy_H__
#define __BeginEndPrimitiveProxy_H__

/** 说明:
1,支持的图元类型:
	PT_POINTS, 
	PT_LINES, PT_LINE_STRIP, PT_LINE_LOOP(用PT_LINE_STRIP实现),
	PT_TRIANGLES,PT_TRIANGLE_STRIP,PT_TRIANGLE_FAN
	PT_QUADS(用TPT_TRIANGLES实现)

2,支持的顶点属性:
	位置，法线，颜色，纹理坐标（支持多重纹理坐标)


*/


namespace xs
{
	class BeginEndPrimitiveProxy;
	class RenderSystemD3D9;
	class VertexBufferD3D9;
	class IndexBufferD3D9;

	class BeginEndPrimitiveProxyCreater
	{
	public:
		static BeginEndPrimitiveProxy * create( RenderSystemD3D9 * pRenderSystem);
	};

	class BeginEndPrimitiveProxy
	{
		//渲染引擎接口
	public:
		/**	开始绘制指定的图元
		*/
		void	beginPrimitive(PrimitiveType pt);

		/** 结束绘制指定的图元(真正地传送图元数据)
		*/
		void	endPrimitive();

		/** 传送顶点位置数据
		*/
		void	sendVertex(const Vector2& vertex);
		void	sendVertex(const Vector3& vertex);
		void	sendVertex(const Vector4& vertex);

		/** 传送顶点法线数据
		*/
		void	setNormal(const Vector3& normal);

		/** 传送顶点颜色数据
		*/
		void	setColor(const ColorValue& color);

		/** 传送顶点0号纹理单元的纹理坐标数据
		*/
		void	setTexcoord(const Vector2& texcoord);

		/** 传送顶点纹理单元的纹理坐标数据
		*/
		void 	setMultiTexcoord(ushort unit,const Vector2& texcoord);

		/** 设置边界标志
		*/
		void setEdgeFlag(bool bEdgeFlag);

		/** 释放函数
		*/
		void release();

	private:
		///** 复位函数
		//*/
		//void reset();

		/** 回到未开始状态
		*/
		void toUnBeginPrimitive();

		/** 确定FVF
		*/
		void determineFVF();

	private:
		friend class BeginEndPrimitiveProxyCreater;
		bool	create(RenderSystemD3D9 * pRenderSystem);//创建函数
		BeginEndPrimitiveProxy();
		~BeginEndPrimitiveProxy();

	private:
		bool	m_bSetNormal;//是否设置了法线
		xs::Vector3 m_vCurrentNormal;//当前的法线

		bool	m_bSetDiffuse;//是否设置了当前的散射光
		DWORD	m_dwCurrentDiffuse;//当前的散射光

		uint	m_TexcoordCount;//设置的纹理坐标的计数
		const	static uint cs_MaxTexcoordCount = 8;//最大纹理坐标计数
		xs::Vector2	m_vCurrentTexcoord[cs_MaxTexcoordCount];//当前纹理坐标

		bool	m_bBetweenBeginEnd;//是否处于beginPrimitive 和 endPrimitive中	
		PrimitiveType	m_CurrentPrimitiveType;//当前的图元
		RenderSystemD3D9 * m_pRenderSystem;//渲染系统指针

		bool	m_bIsFVFdetermined;//灵活顶点格式是否已经确定
		DWORD	m_dwFVF;//灵活顶点格式	
		uint	m_uiCurrentVertexSize;//当前顶点的大小
		uint	m_uiVertexCount;//提交的顶点数

		const static uint	cs_uiStackSize = 48*2048;//顶点堆栈大小
		uint	m_uiCurrentStackPointer;//当前堆栈顶点
		uchar	m_VertexStack[cs_uiStackSize];//顶点堆栈
		ushort	m_IndexBufferForDrawQuads[((cs_uiStackSize/12)/4)*6];//绘制四边形用的index buffer
	};
}

#endif