#include "StdAfx.h"
#include "MappingsD3D9.h"

namespace xs
{


	//获取buffer对应的D3D9使用方式
	DWORD	MappingsD3D9::getBufferUsage( BufferUsage usage)
	{
		//不用考虑BU_DISCARDABLE
		DWORD ret = 0;
		// BU_DYNAMIC
		if( (usage & BU_DYNAMIC))
		{
			ret |= D3DUSAGE_DYNAMIC;
		}

		if( usage & BU_WRITE_ONLY)
		{
			ret |= D3DUSAGE_WRITEONLY;
		}
		return ret;

	}

	D3DPOOL	MappingsD3D9::getPoolTypeByBufferUsage( BufferUsage usage)
	{
		//只有两种方式管理资源，Default Pool 和 Managed Pool
		//如果资源是可丢弃的，则使用Default Pool 
		//否则使用Managed Pool
		if( usage & BU_DYNAMIC)
			return D3DPOOL_DEFAULT;
		else
			return D3DPOOL_MANAGED;
	}

	//根据索引类型获取对应的格式
	D3DFORMAT MappingsD3D9::getIndexTypeFormat(IndexType type)
	{
		if( type == IT_16BIT)
			return D3DFMT_INDEX16;
		else 
			return D3DFMT_INDEX32;
	}
	//根据索引类型获取对应的索引大小
	UINT MappingsD3D9::getIndexTypeSize(IndexType type)
	{
		if( type == IT_16BIT)
			return sizeof(WORD);
		else 
			return sizeof(DWORD);	
	}

	//获取D3D9对应的图元类型
	D3DPRIMITIVETYPE MappingsD3D9::getPrimptiveType( PrimitiveType type)
	{
		switch (type) 
		{
		case PT_LINES:
			return D3DPT_LINELIST;
			break;

		case PT_LINE_STRIP:
			return D3DPT_LINESTRIP;
			break;

		case PT_LINE_LOOP: //没有封闭的线，用线带表示
			return D3DPT_LINESTRIP;
			break;

		case PT_TRIANGLES:
			return D3DPT_TRIANGLELIST;
			break;

		case PT_TRIANGLE_STRIP:
			return D3DPT_TRIANGLESTRIP;
			break;

		case PT_TRIANGLE_FAN:
			return D3DPT_TRIANGLEFAN;
			break;

		case PT_POINTS:
			return D3DPT_POINTLIST;
			break;

		case PT_QUADS:
			return D3DPT_TRIANGLELIST;
			break;
		}
		//不要到这里
		return D3DPT_LINESTRIP;
	}

	//获取D3D9图元类型和顶点数量对应的图元数量
	UINT MappingsD3D9::getPrimitiveCount(PrimitiveType type, uint indexCount)
	{
		switch (type) 
		{
		case PT_LINES:
			return indexCount/2;
			break;

		case PT_LINE_STRIP:
			if( indexCount < 2 )
				return 0;
			else
				return indexCount -1;
			break;

		case PT_TRIANGLES:
			return indexCount/3;
			break;

		case PT_TRIANGLE_STRIP:
			if( indexCount < 3)
				return 0;
			else 
				return indexCount - 2;
			break;

		case PT_TRIANGLE_FAN:
			if( indexCount < 3)
				return 0;
			else 
				return indexCount - 2;
			break;

		case PT_POINTS:
			return indexCount;
			break;

		//原生不支持
		case PT_LINE_LOOP:
		case PT_QUADS:
		case PT_QUAD_STRIP:
		default:
			return 0;
		}
	}

	//获取对应的D3D9矩阵
	D3DXMATRIX MappingsD3D9::getMatrix( const xs::Matrix4 & mat)
	{
		//D3D9按行存储的
		return D3DXMATRIX(
            mat[0][0], mat[1][0], mat[2][0], mat[3][0],
            mat[0][1], mat[1][1], mat[2][1], mat[3][1],
            mat[0][2], mat[1][2], mat[2][2], mat[3][2],
            mat[0][3], mat[1][3], mat[2][3], mat[3][3]);	
	}
	//获取D3D9矩阵对应的本引擎矩阵
	xs::Matrix4 MappingsD3D9::getMatrix(const D3DXMATRIX & mat)
	{
		//D3D9按行存储的
		return Matrix4(
            mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
            mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
            mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
            mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]);
	}

	//获取D3D9光照参数
	void MappingsD3D9::getLight(/*const */xs::Light & lit, D3DLIGHT9 & d3d9lit)
	{
		//类型
		switch(lit.getType())
		{
		case LT_POINT:
			d3d9lit.Type = D3DLIGHT_POINT;
			break;
		case LT_DIRECTIONAL:
			d3d9lit.Type = D3DLIGHT_DIRECTIONAL;
			break;
		case LT_SPOTLIGHT:
			d3d9lit.Type = D3DLIGHT_SPOT;
			break;
		}

		//位置
		Vector3 pos = lit.getPosition();
		d3d9lit.Position.x = pos.x;
		d3d9lit.Position.y = pos.y;
		d3d9lit.Position.z = pos.z;
		
		//方向
		Vector3 dir = lit.getDirection();
		d3d9lit.Direction.x = dir.x;
		d3d9lit.Direction.y = dir.y;
		d3d9lit.Direction.z = dir.z;

		//环境光
		xs::ColorValue ambient = lit.getAmbientColor();
		d3d9lit.Ambient.a = ambient.a;
		d3d9lit.Ambient.r = ambient.r;
		d3d9lit.Ambient.g = ambient.g;
		d3d9lit.Ambient.b = ambient.b;

		//散射光
		xs::ColorValue diffuse = lit.getDiffuseColor();
		d3d9lit.Diffuse.a = diffuse.a;
		d3d9lit.Diffuse.r = diffuse.r;
		d3d9lit.Diffuse.g = diffuse.g;
		d3d9lit.Diffuse.b = diffuse.b;

		//镜面光
		xs::ColorValue specular = lit.getSpecularColor();
		d3d9lit.Specular.a = specular.a;
		d3d9lit.Specular.r = specular.r;
		d3d9lit.Specular.g = specular.g;
		d3d9lit.Specular.b = specular.b;

		//衰减
		d3d9lit.Attenuation0 = lit.getAttenuationConstant();
		d3d9lit.Attenuation1 = lit.getAttenuationLinear();
		d3d9lit.Attenuation2 = lit.getAttenuationQuadric();
		d3d9lit.Range = lit.getAttenuationRange();

		//聚光灯效果
		d3d9lit.Theta = lit.getSpotlightInnerAngle().valueRadians();
		d3d9lit.Phi = lit.getSpotlightOuterAngle().valueRadians(); 
		d3d9lit.Falloff = lit.getSpotlightFalloff();

		return;
	}
	//获取引擎光照参数
	void MappingsD3D9::getLight(const D3DLIGHT9 & d3d9lit, xs::Light  & lit)
	{
		//类型
		switch(d3d9lit.Type)
		{
		case D3DLIGHT_POINT :
			lit.setType(LT_POINT);
			break;
		case D3DLIGHT_DIRECTIONAL :
			lit.setType(LT_DIRECTIONAL);
			break;
		case D3DLIGHT_SPOT :
			lit.setType(LT_SPOTLIGHT);
			break;
		}

		//位置
		lit.setPosition( d3d9lit.Position.x, d3d9lit.Position.y, d3d9lit.Position.z);
		
		//方向
		lit.setDirection( d3d9lit.Direction.x, d3d9lit.Direction.y, d3d9lit.Direction.z);

		//环境光
		lit.setAmbientColor( xs::ColorValue(d3d9lit.Ambient.r, d3d9lit.Ambient.g, d3d9lit.Ambient.b, d3d9lit.Ambient.a) );

		//散射光
		lit.setDiffuseColor( xs::ColorValue(d3d9lit.Diffuse.r, d3d9lit.Diffuse.g, d3d9lit.Diffuse.b, d3d9lit.Diffuse.a) );

		//镜面光
		lit.setSpecularColor( xs::ColorValue(d3d9lit.Specular.r, d3d9lit.Specular.g, d3d9lit.Specular.b, d3d9lit.Specular.a) );

		//衰减
		lit.setAttenuation(d3d9lit.Range, d3d9lit.Attenuation0, d3d9lit.Attenuation1,  d3d9lit.Attenuation2 );

		//聚光灯效果
#ifndef RE_FORCE_ANGLE_TYPES
		if (lit.getType() == LT_SPOTLIGHT)
		{
			lit.setSpotlightRange( d3d9lit.Theta, d3d9lit.Phi, d3d9lit.Falloff);
		}		
#endif
		return;		
	}

	//获D3D9取填充模式
	D3DFILLMODE MappingsD3D9::getFillMode( FillMode fm)
	{
		switch( fm)
		{
		case FM_POINTS:
			return D3DFILL_POINT;
		case FM_WIREFRAME:
			return D3DFILL_WIREFRAME;
		case FM_SOLID:
			return D3DFILL_SOLID;
		}
		//不会到这里
		return D3DFILL_SOLID;
	}

	//获取D3D9背面拣选模式
	D3DCULL MappingsD3D9::getCullingMode(CullingMode cm)
	{
		switch( cm)
		{
		case CULL_NONE:
			return D3DCULL_NONE;
		case CULL_CLOCKWISE:
			return D3DCULL_CW;
		case CULL_COUNTERCLOCKWISE:
			return D3DCULL_CCW;
		}
		//不会到这里
		return D3DCULL_NONE;
	}

	//获取D3D9雾的模式
	D3DFOGMODE MappingsD3D9::getFogMode(FogMode fm)
	{
		switch( fm)
		{
		case FOG_NONE:
			return D3DFOG_NONE;
		case FOG_LINEAR:
			return D3DFOG_LINEAR;
		case FOG_EXP:
			return D3DFOG_EXP;
		case FOG_EXP2:
			return D3DFOG_EXP2;
		}
		//不会到这里
		return D3DFOG_EXP;
	}

	//获取D3D9深度检测函数
	D3DCMPFUNC MappingsD3D9::getDepthFunc(CompareFunction cf)
	{
		switch( cf)
		{
		case CMPF_ALWAYS_FAIL:
			return  D3DCMP_NEVER;
		case CMPF_ALWAYS_PASS:
			return D3DCMP_ALWAYS;
		case CMPF_LESS:
			return D3DCMP_LESS;
		case CMPF_LESS_EQUAL:
			return D3DCMP_LESSEQUAL;
		case CMPF_EQUAL:
			return D3DCMP_EQUAL;
		case CMPF_GREATER_EQUAL:
			return D3DCMP_GREATEREQUAL;
		case CMPF_GREATER:
			return D3DCMP_GREATER;
		}
		//不会到这里
		return D3DCMP_LESS;
	}

	//获取D3D9Alpha检测函数
	D3DCMPFUNC MappingsD3D9::getAlphaFunc(CompareFunction cf)
	{
		switch( cf)
		{
		case CMPF_ALWAYS_FAIL:
			return  D3DCMP_NEVER;
		case CMPF_ALWAYS_PASS:
			return D3DCMP_ALWAYS;
		case CMPF_LESS:
			return D3DCMP_LESS;
		case CMPF_LESS_EQUAL:
			return D3DCMP_LESSEQUAL;
		case CMPF_EQUAL:
			return D3DCMP_EQUAL;
		case CMPF_GREATER_EQUAL:
			return D3DCMP_GREATEREQUAL;
		case CMPF_GREATER:
			return D3DCMP_GREATER;
		}
		//不会到这里
		return D3DCMP_ALWAYS;
	}

	//获取D3D9模版检测函数
	D3DCMPFUNC MappingsD3D9::getStencilFunc(CompareFunction cf)
	{
		switch( cf)
		{
		case CMPF_ALWAYS_FAIL:
			return  D3DCMP_NEVER;
		case CMPF_ALWAYS_PASS:
			return D3DCMP_ALWAYS;
		case CMPF_LESS:
			return D3DCMP_LESS;
		case CMPF_LESS_EQUAL:
			return D3DCMP_LESSEQUAL;
		case CMPF_EQUAL:
			return D3DCMP_EQUAL;
		case CMPF_GREATER_EQUAL:
			return D3DCMP_GREATEREQUAL;
		case CMPF_GREATER:
			return D3DCMP_GREATER;
		}
		//不会到这里
		return D3DCMP_ALWAYS;
	}

	//获取D3D9模版操作方式
	D3DSTENCILOP  MappingsD3D9::getStencilOp( StencilOperation so)
	{
		switch ( so )
		{
		case SOP_KEEP:
			return D3DSTENCILOP_KEEP;
		case SOP_ZERO:
			return D3DSTENCILOP_ZERO;
		case SOP_REPLACE:
			return D3DSTENCILOP_REPLACE;
		case SOP_INCREMENT:
			return D3DSTENCILOP_INCRSAT;
		case SOP_DECREMENT:
			return D3DSTENCILOP_DECRSAT;
		case SOP_INCREMENT_WRAP:
			return D3DSTENCILOP_INCR;
		case SOP_DECREMENT_WRAP:
			return D3DSTENCILOP_DECR;
		case SOP_INVERT:
			return D3DSTENCILOP_INVERT;
		}
		//不会到这里
		return  D3DSTENCILOP_KEEP;
	}


	//获取D3D9 alpha混合方式
	D3DBLEND  MappingsD3D9::getBlendFactor(SceneBlendFactor factor)
	{
		switch( factor)
		{
		case SBF_ONE:
			return D3DBLEND_ONE;
		case SBF_ZERO:
			return D3DBLEND_ZERO;
		case SBF_DEST_COLOR:
			return D3DBLEND_DESTCOLOR;
		case SBF_SOURCE_COLOR:
			return D3DBLEND_SRCCOLOR;
		case SBF_ONE_MINUS_DEST_COLOR:
			return D3DBLEND_INVDESTCOLOR;
		case SBF_ONE_MINUS_SOURCE_COLOR:
			return D3DBLEND_INVSRCCOLOR;
		case SBF_DEST_ALPHA:
			return D3DBLEND_DESTALPHA;
		case SBF_SOURCE_ALPHA:
			return D3DBLEND_SRCALPHA;
		case SBF_ONE_MINUS_DEST_ALPHA:
			return D3DBLEND_INVDESTALPHA;
		case SBF_ONE_MINUS_SOURCE_ALPHA:
			return  D3DBLEND_INVSRCALPHA;		
		}
		//不会到这里
		return D3DBLEND_ONE;	
	}

	//获取D3D9格式
	//格式装换有些乱，对照着OGL的格式转换来的。正确性由它保证
	D3DFORMAT MappingsD3D9::getFormat(PixelFormat pf)
	{
		switch(pf)
		{		
		case PF_A8:
			return D3DFMT_A8;
		case PF_L8:
			return D3DFMT_L8;
		case PF_L16:
			return D3DFMT_L16;
		case PF_R3G3B2: 
			return D3DFMT_R3G3B2;
		case PF_A1R5G5B5:
			return D3DFMT_A1R5G5B5;
		//case PF_R5G6B5: //最接近内部格式 return PF_B5G6R5;
		case PF_B5G6R5: 
			return D3DFMT_R5G6B5;
		case PF_A4R4G4B4:
			return D3DFMT_A4R4G4B4;
#if	RE_ENDIAN == RE_ENDIAN_BIG
		// Formats are in native endian, so R8G8B8 on little endian is
        // BGR, on big endian it is RGB.
		//case PF_R8G8B8: //最接近内部格式 PF_B8G8R8
		case PF_B8G8R8:
			return D3DFMT_R8G8B8;
		case PF_R8G8B8A8:
			return D3DFMT_A8B8G8R8 ;
		case PF_B8G8R8A8:
			return D3DFMT_A8R8G8B8;
#else
		case PF_R8G8B8:
			return D3DFMT_R8G8B8;
		//case PF_B8G8R8: //最接近内部格式 PF_R8G8B8
		//case PF_R8G8B8A8: //没有对应的格式
		//case PF_B8G8R8A8: //没有对应的格式
#endif
		case PF_X8R8G8B8:
			return D3DFMT_X8R8G8B8;
		case PF_A8R8G8B8:
			return D3DFMT_A8R8G8B8;
		case PF_X8B8G8R8:
			return D3DFMT_X8B8G8R8;
        case PF_A8B8G8R8:
            return D3DFMT_A8B8G8R8;	
		case PF_A2R10G10B10:
			return D3DFMT_A2R10G10B10;
		case PF_A2B10G10R10:
			return D3DFMT_A2B10G10R10;
		case PF_FLOAT16_R:
			return D3DFMT_R16F;
		//case PF_FLOAT16_RGB: //最接近内部格式PF_FLOAT16_RGBA
		case PF_FLOAT16_RGBA:
			return D3DFMT_A16B16G16R16F;
		case PF_FLOAT32_R:
			return D3DFMT_R32F;
		//case PF_FLOAT32_RGB: //最接近内部格式PF_FLOAT32_RGBA
		case PF_FLOAT32_RGBA:
			return D3DFMT_A32B32G32R32F;
		case PF_SHORT_RGBA:
			return D3DFMT_A16B16G16R16;
		//case PF_RGB_DXT1: //最接近内部格式D3DFMT_DXT1
		case PF_DXT1:
			return D3DFMT_DXT1;
		case PF_DXT2:
			return D3DFMT_DXT2;
		case PF_DXT3:
			return D3DFMT_DXT3;
		case PF_DXT4:
			return D3DFMT_DXT4;
		case PF_DXT5:
			return D3DFMT_DXT5;
		case PF_UNKNOWN:
		default:
			return D3DFMT_UNKNOWN;
		}
	}

	//获取接近的D3D9格式
	PixelFormat MappingsD3D9::getClosestSupportedFormat(PixelFormat pf)
	{
		if( getFormat( pf) != PF_UNKNOWN)
			return pf;
		else
		{
			switch(pf)
			{
			case PF_R5G6B5:
				return PF_B5G6R5;
			case PF_R8G8B8:
				return PF_B8G8R8;
			case PF_FLOAT16_RGB:
				return PF_FLOAT16_RGBA;
			case PF_FLOAT32_RGB:
				return PF_FLOAT32_RGBA;
			case PF_RGB_DXT1:
				return PF_DXT1;
			case PF_UNKNOWN:
			default:
				return PF_UNKNOWN;
			}
		}
	}

	//获取D3D9的过滤器种类
	D3DTEXTUREFILTERTYPE MappingsD3D9::getFilterType(FilterOptions fo)
	{
		switch( fo)
		{
		/// No filtering, used for FILT_MIP to turn off mipmapping
		case FO_NONE:
			return D3DTEXF_NONE;
		/// Use the closest pixel
		case FO_POINT:
			return D3DTEXF_POINT;
		/// Average of a 2x2 pixel area, denotes bilinear for MIN and MAG, trilinear for MIP
		case FO_LINEAR:
			return D3DTEXF_LINEAR;
		/// Similar to FO_LINEAR, but compensates for the angle of the texture plane
		case FO_ANISOTROPIC:
			return D3DTEXF_ANISOTROPIC;	
		default://不会到这里
			return D3DTEXF_LINEAR;
		}
	}

	//获取引擎的过滤器类型
	FilterOptions MappingsD3D9::getFilterType( D3DTEXTUREFILTERTYPE texf)
	{
		switch ( texf)
		{
		case D3DTEXF_NONE:
			return FO_NONE;
		case D3DTEXF_POINT:
			return FO_POINT;
		case D3DTEXF_LINEAR:
			return FO_LINEAR;
		case D3DTEXF_ANISOTROPIC:
			return FO_ANISOTROPIC;
		default: //不会到这里
			return FO_LINEAR;
		}
	}

	//获取D3D9的寻址模式 TAM_CLAMP_TO_EDGE 和 TAM_CLAMP一样?
	D3DTEXTUREADDRESS	MappingsD3D9::getAddressMode( TextureAddressingMode addr)
	{
		switch( addr)
		{
		/// Texture wraps at values over 1.0
		case TAM_WRAP:
			return D3DTADDRESS_WRAP;
		/// Texture mirrors (flips) at joins over 1.0
		case TAM_MIRROR:
			return D3DTADDRESS_MIRROR;
		/// Texture clamps at 1.0
		case TAM_CLAMP_TO_EDGE:
			return D3DTADDRESS_CLAMP;
		//
		case TAM_CLAMP:  // clamp ??
			return D3DTADDRESS_CLAMP;
		default://不会到这里
			return D3DTADDRESS_WRAP;
		}
	}

	//获取引擎的寻址模式 TAM_CLAMP_TO_EDGE 和 TAM_CLAMP一样?
	TextureAddressingMode MappingsD3D9::getAddressMode( D3DTEXTUREADDRESS addr)
	{
		//
		switch( addr)
		{
		case D3DTADDRESS_WRAP:
			return TAM_WRAP;
		case D3DTADDRESS_MIRROR:
			return TAM_MIRROR;
		case D3DTADDRESS_CLAMP:
			return TAM_CLAMP;
		default://不会到这里
			return TAM_WRAP;
		}
	}

	//获取纹理Stage的操作函数
	D3DTEXTUREOP MappingsD3D9::getTextureStageOperator(TextureStageOperator op)
	{
		switch(op)
		{
		case TSO_REPLACE:
			return D3DTOP_SELECTARG1;
		case TSO_MODULATE:
			return D3DTOP_MODULATE;
		case TSO_MODELATE2X:
			return D3DTOP_MODULATE2X;
		case TSO_ADD:
			return D3DTOP_ADD;
		case TSO_ADD_SIGNED:
			return D3DTOP_ADDSIGNED;
		case TSO_SUBTRACT:
			return D3DTOP_SUBTRACT;
		default://绝不会到这里的
			return D3DTOP_MODULATE;
		}
	}
	//获取纹理的Stage的操作数
	DWORD MappingsD3D9::getTextureStageArgument(TextureStageArgument arg)
	{
		switch( arg)
		{
		case TSA_TEXTURE:
			return D3DTA_TEXTURE;
		case TSA_PREVIOUS:
			return D3DTA_CURRENT;
		default://绝不会到这里
			return D3DTA_TEXTURE;
		}
	}

	//矩阵转换
	void MappingsD3D9::getMatrix(const xs::Matrix4 & in, D3DXMATRIX & out)
	{
		//D3D9是处理行向量，本引擎处理列向量
		out.m[0][0] = in[0][0]; out.m[0][1] = in[1][0]; out.m[0][2] = in[2][0]; out.m[0][3] = in[3][0];
		out.m[1][0] = in[0][1]; out.m[1][1] = in[1][1]; out.m[1][2] = in[2][1]; out.m[1][3] = in[3][1];
		out.m[2][0] = in[0][2]; out.m[2][1] = in[1][2]; out.m[2][2] = in[2][2]; out.m[2][3] = in[3][2];
		out.m[3][0] = in[0][3]; out.m[3][1] = in[1][3]; out.m[3][2] = in[2][3]; out.m[3][3] = in[3][3];
	}

	//矩阵转换
	void MappingsD3D9::getMatrix(const D3DXMATRIX & in, xs::Matrix4 & out)
	{
		//D3D9是处理行向量，本引擎处理列向量
		out[0][0] = in.m[0][0]; out[0][1] = in.m[1][0]; out[0][2] = in.m[2][0]; out[0][3] = in.m[3][0];
		out[1][0] = in.m[0][1]; out[1][1] = in.m[1][1]; out[1][2] = in.m[2][1]; out[1][3] = in.m[3][1];
		out[2][0] = in.m[0][2]; out[2][1] = in.m[1][2]; out[2][2] = in.m[2][2]; out[2][3] = in.m[3][2];
		out[3][0] = in.m[0][3]; out[3][1] = in.m[1][3]; out[3][2] = in.m[2][3]; out[3][3] = in.m[3][3];	
	}
}