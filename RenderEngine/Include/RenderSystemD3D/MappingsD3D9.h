#ifndef __MappingsD3D9_H__
#define __MappingsD3D9_H__

namespace xs
{
	/** 将渲染引擎的类型映射到D3D9 或者做逆向的映射
	*/
	struct MappingsD3D9 
	{
		//获取buffer对应的D3D9使用方式
		static	DWORD	getBufferUsage( BufferUsage usage);
		//根据buffer的用途决定使用哪种池管理
		static	D3DPOOL	getPoolTypeByBufferUsage( BufferUsage usage);
		//根据索引类型获取对应的格式
		static D3DFORMAT getIndexTypeFormat(IndexType type);
		//根据索引类型获取对应的索引大小
		static UINT		getIndexTypeSize(IndexType type);
		//获取D3D9对应的图元类型,不支持绘制四边形，四边形带，封闭曲线，都用线带表示)
		static D3DPRIMITIVETYPE getPrimptiveType( PrimitiveType type);
		//获取D3D9图元类型和顶点数量对应的图元数量
		static UINT	getPrimitiveCount(PrimitiveType type, uint indexCount);
		//获取对应的D3D9矩阵
		static D3DXMATRIX	getMatrix( const xs::Matrix4 & mat);
		//获取D3D9矩阵对应的本引擎矩阵
		static xs::Matrix4 getMatrix(const D3DXMATRIX & mat);
		//矩阵转换
		static void getMatrix(const xs::Matrix4 & in, D3DXMATRIX & out);
		//矩阵转换
		static void getMatrix(const D3DXMATRIX & in, xs::Matrix4 & out);
		//获取D3D9光照参数
		static void getLight(/*const*/ xs::Light & lit, D3DLIGHT9 & d3d9lit);
		//获取引擎光照参数
		static void getLight(const D3DLIGHT9 & d3d9lit, xs::Light & lit);
		//获D3D9取填充模式
		static D3DFILLMODE getFillMode( FillMode fm);
		//获取D3D9背面拣选模式
		static D3DCULL getCullingMode(CullingMode cm);
		//获取D3D9雾的模式
		static D3DFOGMODE getFogMode(FogMode fm);
		//获取D3D9深度检测函数
		static D3DCMPFUNC getDepthFunc(CompareFunction cf);
		//获取D3D9Alpha检测函数
		static D3DCMPFUNC getAlphaFunc(CompareFunction cf);
		//获取D3D9模版检测函数
		static D3DCMPFUNC getStencilFunc(CompareFunction cf);
		//获取D3D9模版操作方式
		static D3DSTENCILOP  getStencilOp( StencilOperation so);
		//获取D3D9 alpha混合方式
		static D3DBLEND  getBlendFactor(SceneBlendFactor factor);
		//获取D3D9格式
		static D3DFORMAT getFormat(PixelFormat pf);
		//获取接近的D3D9格式
		static PixelFormat getClosestSupportedFormat(PixelFormat pf);
		//获取D3D9的过滤器种类
		static D3DTEXTUREFILTERTYPE getFilterType(FilterOptions fo);
		//获取引擎的过滤器类型
		static FilterOptions		getFilterType( D3DTEXTUREFILTERTYPE texf);
		//获取D3D9的寻址模式
		static D3DTEXTUREADDRESS	getAddressMode( TextureAddressingMode addr);
		//获取引擎的寻址模式
		static TextureAddressingMode getAddressMode( D3DTEXTUREADDRESS addr);
		//获取纹理Stage的操作函数
		static D3DTEXTUREOP		getTextureStageOperator(TextureStageOperator op);
		//获取纹理的Stage的操作数
		static DWORD			getTextureStageArgument(TextureStageArgument arg);

	};
}

#endif