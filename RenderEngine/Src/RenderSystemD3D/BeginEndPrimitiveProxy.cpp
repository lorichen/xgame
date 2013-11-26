#include "StdAfx.h"
#include "BeginEndPrimitiveProxy.h"
#include "IndexBufferD3D9.h"
#include "VertexBufferD3D9.h"
#include "RenderSystemD3D9.h"
#include "RenderTargetD3D9.h"
#include "RenderTargetManagerD3D9.h"
#include "MappingsD3D9.h"
#include "D3D9Device.h"

namespace xs
{


	BeginEndPrimitiveProxy * BeginEndPrimitiveProxyCreater::create( RenderSystemD3D9 * pRenderSystem)
	{
		BeginEndPrimitiveProxy * pProxy = new BeginEndPrimitiveProxy();
		if( pProxy->create( pRenderSystem) )
		{
			return pProxy;
		}
		else
		{
			pProxy->release();
			return 0;
		}	
	}

	BeginEndPrimitiveProxy::BeginEndPrimitiveProxy():m_bSetNormal(false),
		m_vCurrentNormal(0,1,0),
		m_bSetDiffuse(false),
		m_dwCurrentDiffuse(D3DCOLOR_ARGB(255,255,255,255)),
		m_TexcoordCount(0),
		m_bBetweenBeginEnd(false),
		m_CurrentPrimitiveType(PT_POINTS),
		m_pRenderSystem(0),
		m_bIsFVFdetermined(false),
		m_dwFVF(0),
		m_uiCurrentVertexSize(0),
		m_uiCurrentStackPointer(0),
		m_uiVertexCount(0)
	{
		for( uint i =0; i<cs_MaxTexcoordCount; ++i)
			m_vCurrentTexcoord[i] = xs::Vector2::ZERO;

		memset(m_VertexStack, 0, sizeof(m_VertexStack));

		//写索引
		uint indexNum = sizeof(m_IndexBufferForDrawQuads) / sizeof(ushort);
		for( uint i=0; i< indexNum; i +=6)
		{
			uint vertexIndexBase = (i/6)*4;
			m_IndexBufferForDrawQuads[i+0] = vertexIndexBase;
			m_IndexBufferForDrawQuads[i+1] = vertexIndexBase + 1;
			m_IndexBufferForDrawQuads[i+2] = vertexIndexBase + 2;

			m_IndexBufferForDrawQuads[i+3] = vertexIndexBase;
			m_IndexBufferForDrawQuads[i+4] = vertexIndexBase+2;
			m_IndexBufferForDrawQuads[i+5] = vertexIndexBase+3;

		}
	}

	BeginEndPrimitiveProxy::~BeginEndPrimitiveProxy()
	{
		//nothing to do
	}

	bool BeginEndPrimitiveProxy::create(RenderSystemD3D9 * pRenderSystem)
	{
		if( NULL == pRenderSystem )
			return false;
		m_pRenderSystem = pRenderSystem;

		//设置当前属性
		setColor(m_pRenderSystem->getColor() );
		setNormal( m_pRenderSystem->getNormal() );
		setEdgeFlag(m_pRenderSystem->getEdgeFlag() );
		for( uint i = 0; i<cs_MaxTexcoordCount; ++i)
		{
			setMultiTexcoord(i, m_pRenderSystem->getMultiTexcoord(i));
		}

		return true;
	}

	void BeginEndPrimitiveProxy::release()
	{
		delete this;
	}


	void BeginEndPrimitiveProxy::toUnBeginPrimitive()
	{
		m_bBetweenBeginEnd = false;
		m_CurrentPrimitiveType = PT_POINTS;

		m_bIsFVFdetermined = false;
		m_dwFVF = 0;
		m_uiCurrentVertexSize = 0;
		m_uiVertexCount = 0;

		m_uiCurrentStackPointer = 0;

		m_bSetNormal = false;
		m_bSetDiffuse = false;
		m_TexcoordCount = 0;
	}

	void BeginEndPrimitiveProxy::beginPrimitive(PrimitiveType pt)
	{
		m_bBetweenBeginEnd = true;
		m_CurrentPrimitiveType = pt;
		
		m_bIsFVFdetermined = false;
		m_dwFVF = 0;
		m_uiCurrentVertexSize = 0;
		m_uiVertexCount = 0;

		m_uiCurrentStackPointer = 0;

		m_bSetNormal = false;
		m_bSetDiffuse = false;
		m_TexcoordCount = 0;
	}

	void BeginEndPrimitiveProxy::setColor( const xs::ColorValue & color)
	{
		int a = color.a * 255.0f;
		if( a > 255 )
			a = 255;
		else if( a < 0)
			a = 0;

		int r = color.r * 255.0f;
		if( a > 255)
			a = 255;
		else if( a< 0)
			a = 0;

		int g = color.g * 255.0f;
		if( g > 255)
			g = 255;
		else if(g < 0)
			g = 0;

		int b = color.b * 255.0f;
		if( b > 255)
			b = 255;
		else if( b < 0)
			b = 0;

		m_dwCurrentDiffuse = D3DCOLOR_ARGB(a, r, g, b);

		if( m_bBetweenBeginEnd && !m_bIsFVFdetermined)
		{
			m_bSetDiffuse = true;
		}
	}

	void BeginEndPrimitiveProxy::setNormal( const xs::Vector3 & normal)
	{
		m_vCurrentNormal = normal;

		if( m_bBetweenBeginEnd && !m_bIsFVFdetermined )
		{
			m_bSetNormal = true;
		}
	}

	void BeginEndPrimitiveProxy::setTexcoord( const xs::Vector2 & texcoord)
	{
		setMultiTexcoord(0, texcoord);
	}

	void BeginEndPrimitiveProxy::setMultiTexcoord(ushort unit, const xs::Vector2 & texcoord)
	{
		if( unit >= cs_MaxTexcoordCount)//纹理映射单元越界
			return;

		//设置当前纹理坐标
		m_vCurrentTexcoord[unit] = texcoord;
		if( m_bBetweenBeginEnd && !m_bIsFVFdetermined && static_cast<uint>(unit+1) > m_TexcoordCount)
			m_TexcoordCount = unit + 1;
	}

	void BeginEndPrimitiveProxy::setEdgeFlag(bool bEdgeFlag)
	{
		//不支持
	}

	void	BeginEndPrimitiveProxy::sendVertex(const xs::Vector2 & vertex)
	{
		xs::Vector3 v3(vertex.x, vertex.y, 0.0f);
		sendVertex( v3);
	}

	void	BeginEndPrimitiveProxy::sendVertex(const Vector4& vertex)
	{
		Assert( !Math::RealEqual(vertex.w, 0.0f));
		float rcp = 1.0f / vertex.w;
		xs::Vector3 v3( vertex.x * rcp, vertex.y * rcp, vertex.z * rcp);
		sendVertex(v3);
	}


	void BeginEndPrimitiveProxy::determineFVF()
	{
		if(m_bIsFVFdetermined )
			return ;

		m_dwFVF = 0;
		m_uiCurrentVertexSize = 0;
		m_bIsFVFdetermined = true;

		//顶点
		m_dwFVF |= D3DFVF_XYZ;

		//法向量
		if( m_bSetNormal )
		{
			m_dwFVF |= D3DFVF_NORMAL;
		}

		//diffuse 颜色
		if( m_bSetDiffuse )
		{
			m_dwFVF |= D3DFVF_DIFFUSE;
		}

		//纹理
		if( m_TexcoordCount > 0 )
		{
			switch( m_TexcoordCount )
			{
			case 1:
				m_dwFVF |= D3DFVF_TEX1;
				break;
			case 2:
				m_dwFVF |= D3DFVF_TEX2;
				break;
			case 3:
				m_dwFVF |= D3DFVF_TEX3;
				break;
			case 4:
				m_dwFVF |= D3DFVF_TEX4;
				break;
			case 5:
				m_dwFVF |= D3DFVF_TEX5;
				break;
			case 6:
				m_dwFVF |= D3DFVF_TEX6;
				break;
			case 7:
				m_dwFVF |= D3DFVF_TEX7;
				break;
			case 8:
				m_dwFVF |= D3DFVF_TEX8;
				break;
			default:
				break;
			}
		}
		
		//获取顶点大小
		m_uiCurrentVertexSize = D3DXGetFVFVertexSize(m_dwFVF);
	}

	void	BeginEndPrimitiveProxy::sendVertex(const Vector3& vertex)
	{
		//是否在begin和end之间
		if( !m_bBetweenBeginEnd) 
			return;

		//确定FVF
		if( !m_bIsFVFdetermined)
		{
			determineFVF();
		}
		
		//判定是否越界
		if( m_uiCurrentVertexSize + m_uiCurrentStackPointer > cs_uiStackSize )
			return;

		//更新buffer

		//更新位置
		m_VertexStack;
		float* pPos = (float*)(&m_VertexStack[m_uiCurrentStackPointer]);
		pPos[0] = vertex.x;
		pPos[1] = vertex.y;
		pPos[2] = vertex.z;
		m_uiCurrentStackPointer +=12;

		//更新法线
		if( m_bSetNormal)
		{
			float* pNormal = (float*)(&m_VertexStack[m_uiCurrentStackPointer]);
			pNormal[0] = m_vCurrentNormal.x;
			pNormal[1] = m_vCurrentNormal.y;
			pNormal[2] = m_vCurrentNormal.z;
			m_uiCurrentStackPointer +=12;
		}

		//更新颜色
		if( m_bSetDiffuse)
		{
			DWORD* pColor = (DWORD*)(&m_VertexStack[m_uiCurrentStackPointer]);
			*pColor = m_dwCurrentDiffuse;
			m_uiCurrentStackPointer +=4;
		}

		//更新纹理
		if( m_TexcoordCount > 0 )
		{
			for( uint i=0; i< m_TexcoordCount; ++i)
			{
				float * pTexcoord = (float*)(&m_VertexStack[m_uiCurrentStackPointer]);
				pTexcoord[0] = m_vCurrentTexcoord[i].x;
				pTexcoord[1] = m_vCurrentTexcoord[i].y;
				m_uiCurrentStackPointer +=8;
			}
		}

		//增加顶点数
		++m_uiVertexCount;
	}

	void BeginEndPrimitiveProxy::endPrimitive()
	{
		//1，检测
		if( m_uiVertexCount <= 0 )//没有顶点处理
		{
			toUnBeginPrimitive();
			return;
		}
		if( !m_bBetweenBeginEnd )//没有使用beginPrimitive函数
		{
			toUnBeginPrimitive();
			return;
		}

		//2,图元预处理
		switch( m_CurrentPrimitiveType)
		{
		case PT_QUAD_STRIP://不支持
			{
				toUnBeginPrimitive();
				return;
			}
			break;
		case PT_LINES:
		case PT_LINE_STRIP:
			{
				//顶点数小于2
				if(m_uiVertexCount < 2)
				{
					toUnBeginPrimitive();
					return;
				}
			}
			break;
		case PT_LINE_LOOP:
			{
				if(m_uiVertexCount < 2)
				{
					//顶点数小于2
					toUnBeginPrimitive();
					return;
				}
				if( m_uiCurrentStackPointer + m_uiCurrentVertexSize <= cs_uiStackSize )
				{
					for( uint i =0; i < m_uiCurrentVertexSize; ++i)
					{
						m_VertexStack[m_uiCurrentStackPointer+i] = m_VertexStack[i];
					}
					m_uiCurrentStackPointer += m_uiCurrentVertexSize;
					++m_uiVertexCount;
				}
				m_CurrentPrimitiveType = PT_LINE_STRIP;
			}
			break;
		case PT_TRIANGLES:
			{
				if( m_uiVertexCount < 3)
				{
					toUnBeginPrimitive();
					return;
				}
				//去掉无效的顶点
				m_uiVertexCount = (m_uiVertexCount / 3) *3;
				m_uiCurrentStackPointer = m_uiVertexCount * m_uiCurrentVertexSize;
			}
		case PT_TRIANGLE_STRIP:
		case PT_TRIANGLE_FAN:
			{
				//顶点数少于3
				if( m_uiVertexCount < 3)
				{
					toUnBeginPrimitive();
					return;
				}
			}
			break;
		case PT_QUADS:
			{
				//不能绘制矩形
				if( m_uiVertexCount < 4 )
				{
					toUnBeginPrimitive();
					return;
				}

				//去掉无效数据
				m_uiVertexCount = (m_uiVertexCount/4)*4;
				m_uiCurrentStackPointer = m_uiVertexCount * m_uiCurrentVertexSize;
			}
			break;
		case PT_POINTS:
			{
				//nothing to do
			}
			break;
		default:
			{
				toUnBeginPrimitive();
				return;
			}
			break;
		}

		//3,绘制
		IDirect3DDevice9* pD3DDevice = m_pRenderSystem->getD3D9Device()->getDevice();
		pD3DDevice->SetFVF(m_dwFVF);
		if( m_CurrentPrimitiveType == PT_QUADS )
		{
			UINT primitiveCount = (m_uiVertexCount/4)*2;
			D3DPRIMITIVETYPE d3dpt = D3DPT_TRIANGLELIST;

			pD3DDevice->DrawIndexedPrimitiveUP(d3dpt,
				0,
				m_uiVertexCount,
				primitiveCount,
				m_IndexBufferForDrawQuads,
				D3DFMT_INDEX16,
				m_VertexStack,
				m_uiCurrentVertexSize
				);
		}
		else
		{
			UINT primitiveCount = MappingsD3D9::getPrimitiveCount(m_CurrentPrimitiveType, m_uiVertexCount);
			D3DPRIMITIVETYPE d3dpt = MappingsD3D9::getPrimptiveType(m_CurrentPrimitiveType);
			pD3DDevice->DrawPrimitiveUP( d3dpt, 
				primitiveCount,
				m_VertexStack,
				m_uiCurrentVertexSize);
		}

		//4,重置
		toUnBeginPrimitive();
		return;



		////1，检查
		//if( m_positionBufferPtr == 0 )//没有顶点处理
		//{
		//	reset();
		//	return;
		//}
		//if( !m_bBetweenBeginEnd) //没有使用beginPrimitive函数
		//{
		//	reset();
		//	return;
		//}

		////2,图元预处理
		//switch( m_curPrimitiveType )
		//{
		//case PT_QUAD_STRIP: //暂时不支持
		//	{
		//		reset();
		//		return;
		//	}
		//	break;
		//case PT_LINES:
		//case PT_LINE_STRIP:
		//	{
		//		//顶点数少于2
		//		if( m_positionBufferPtr < 2)
		//		{
		//			reset();
		//			return;
		//		}
		//	}
		//	break;
		//case PT_LINE_LOOP: //用PT_LINE_STRIP实现
		//	{
		//		//顶点数少于2
		//		if( m_positionBufferPtr < 2)
		//		{
		//			reset();
		//			return;
		//		}

		//		//再压入开始的顶点
		//		xs::Vector3 vStartVertex(m_positionBuffer[0],m_positionBuffer[1],m_positionBuffer[2]);
		//		sendVertex( vStartVertex);
		//	}
		//	break;
		//case PT_TRIANGLES:
		//	{
		//		//顶点数少于3
		//		if( m_positionBufferPtr < 3)
		//		{
		//			reset();
		//			return;
		//		}
		//		m_positionBufferPtr = (m_positionBufferPtr/3) * 3;//去掉无效的顶点	
		//	}
		//case PT_TRIANGLE_STRIP:
		//case PT_TRIANGLE_FAN:
		//	{
		//		//顶点数少于3
		//		if( m_positionBufferPtr < 3)
		//		{
		//			reset();
		//			return;
		//		}	
		//	}
		//	break;
		//case PT_QUADS:
		//	{
		//		//不能绘制矩形
		//		if( m_positionBufferPtr < 4 )
		//		{
		//			reset();
		//			return;
		//		}

		//		//去掉无效数据
		//		m_positionBufferPtr = ( m_positionBufferPtr / 4) * 4;
		//	}
		//	break;
		//case PT_POINTS:
		//	{
		//		//nothing to do
		//	}
		//	break;
		//default: //不会到这一步
		//	{
		//		reset();
		//		return;
		//	}
		//	break;
		//}

		////3,设置缓存，并且绘制
		//m_pPositionBuffer->writeData(0, m_positionBufferPtr * sizeof(float) * 3, m_positionBuffer, true);
		//m_pRenderSystem->setVertexVertexBuffer( m_pPositionBuffer, 0 );

		////颜色一定要绘制，忽略标志
		////if( m_bVertxAttribFlag[eDiffuseColorFlag])//颜色
		////{
		//	m_pColorBuffer->writeData(0, m_positionBufferPtr * sizeof(DWORD), m_diffuseColorBuffer, true);
		//	m_pRenderSystem->setDiffuseVertexBuffer( m_pColorBuffer, 0);
		////}

		//if( m_bVertxAttribFlag[eNormalFlag] ) //法向量
		//{
		//	m_pNormalBuffer->writeData(0, m_positionBufferPtr * sizeof(float)*3, m_normalBuffer, true);
		//	m_pRenderSystem->setNormalVertexBuffer( m_pNormalBuffer,0);
		//}

		//for( uint i=0; i< s_maxTextureUint; ++i)
		//{
		//	if( m_bVertxAttribFlag[eTexcoord0+i] )//纹理坐标
		//	{
		//		m_pTexcoordBuffer[i]->writeData(0, m_positionBufferPtr * sizeof(float) * 2, m_texcoordBuffer[i], true);
		//		m_pRenderSystem->setTexcoordVertexBuffer(i, m_pTexcoordBuffer[i], 0);
		//	}
		//}

		//if( m_curPrimitiveType == PT_QUADS)//如果为四边形，用索引绘制
		//{
		//	m_pRenderSystem->setIndexBuffer(m_pIndexBuffer);
		//	m_pRenderSystem->drawIndexedPrimitive(PT_TRIANGLES, (m_positionBufferPtr/4)*6, IT_16BIT);
		//}
		//else
		//{
		//	m_pRenderSystem->drawPrimitive( m_curPrimitiveType, 0, m_positionBufferPtr);
		//}
		//
		////4,取消缓存设置
		//m_pRenderSystem->setVertexVertexBuffer(0,0);

		////if( m_bVertxAttribFlag[eDiffuseColorFlag] )//颜色
		//	m_pRenderSystem->setDiffuseVertexBuffer(0,0);

		//if( m_bVertxAttribFlag[eNormalFlag] ) //法向量
		//	m_pRenderSystem->setNormalVertexBuffer(0,0);

		//for( uint i=0; i<s_maxTextureUint; ++i) //纹理坐标
		//{
		//	if( m_bVertxAttribFlag[eTexcoord0+i] )
		//		m_pRenderSystem->setTexcoordVertexBuffer( i, 0,0);
		//}


		////5, 复位
		//reset();
	}





	//bool BeginEndPrimitiveProxy::create(RenderSystemD3D9 * pRenderSystem)
	//{
	//	if( NULL == pRenderSystem )
	//		return false;
	//	m_pRenderSystem = pRenderSystem;

	//	//创建index buffer
	//	IBufferManager * pBufferMgr = m_pRenderSystem->getBufferManager();
	//	if( NULL == pBufferMgr)
	//		return false;
	//	m_pIndexBuffer = pBufferMgr->createIndexBuffer(IT_16BIT, (s_vertexAttribBufferSize / 4) * 6, BU_STATIC_WRITE_ONLY/*BU_DYNAMIC_WRITE_ONLY_DISCARDABLE*/);
	//	if( NULL == m_pIndexBuffer)
	//	{
	//		release();
	//		return false;
	//	}
	//	//写IndexBuffer
	//	ushort indexBuffer[ (s_vertexAttribBufferSize / 4) * 6];// 缓存索引，用于绘制四边形
	//	uint quadsNum = s_vertexAttribBufferSize / 4;
	//	for( uint i = 0; i < quadsNum; ++i )
	//	{
	//		uint indexIndexBase = i * 6;
	//		uint vertexIndexBase = i *4;
	//		indexBuffer[ indexIndexBase + 0] = vertexIndexBase;
	//		indexBuffer[ indexIndexBase + 1] = vertexIndexBase + 1;
	//		indexBuffer[ indexIndexBase + 2] = vertexIndexBase + 2;

	//		indexBuffer[ indexIndexBase + 3] = vertexIndexBase;
	//		indexBuffer[ indexIndexBase + 4] = vertexIndexBase + 2;
	//		indexBuffer[ indexIndexBase + 5] = vertexIndexBase + 3;
	//	}
	//	m_pIndexBuffer->writeData(0, quadsNum * 6 * sizeof(ushort), indexBuffer, false);


	//	//创建其它的buffer
	//	m_pPositionBuffer = pBufferMgr->createVertexBuffer( sizeof(float)*3, s_vertexAttribBufferSize, BU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	//	if( NULL == m_pPositionBuffer)
	//	{
	//		release();
	//		return false;
	//	}
	//	m_pColorBuffer = pBufferMgr->createVertexBuffer( sizeof(DWORD), s_vertexAttribBufferSize, BU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	//	if( NULL == m_pColorBuffer)
	//	{
	//		release();
	//		return false;
	//	}
	//	m_pNormalBuffer= pBufferMgr->createVertexBuffer( sizeof(float)*3, s_vertexAttribBufferSize, BU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	//	if(NULL == m_pNormalBuffer)
	//	{
	//		release();
	//		return false;
	//	}
	//
	//	for( uint i =0; i< s_maxTextureUint; ++i)
	//	{
	//		m_pTexcoordBuffer[i] = pBufferMgr->createVertexBuffer( sizeof(float)*2, s_vertexAttribBufferSize, BU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	//		if( NULL == m_pTexcoordBuffer[i])
	//		{
	//			release();
	//			return false;
	//		}		
	//	}

	//	//设置当前属性

	//	setColor(m_pRenderSystem->getColor() );
	//	setNormal( m_pRenderSystem->getNormal() );
	//	setEdgeFlag(m_pRenderSystem->getEdgeFlag() );
	//	for( uint i = 0; i<s_maxTextureUint; ++i)
	//	{
	//		setMultiTexcoord(i, m_pRenderSystem->getMultiTexcoord(i));
	//	}

	//	return true;
	//}

	//BeginEndPrimitiveProxy::BeginEndPrimitiveProxy():
	//m_positionBufferPtr(0),
	//m_currentNormal(0.0f,0.0f,1.0f),//initial value
	//m_currentColor( D3DCOLOR_ARGB(255,255,255,255) ),//initial value
	//m_bBetweenBeginEnd(false),
	//m_pIndexBuffer(0),
	//m_pPositionBuffer(0),
	//m_pColorBuffer(0),
	//m_pNormalBuffer(0),
	//m_pRenderSystem(0)
	//{
	//	for( uint i =0; i <s_maxTextureUint; ++i) //关于纹理
	//	{
	//		m_currentTexcoord[i] = xs::Vector2(0.0f,0.0f);//initial value
	//		m_pTexcoordBuffer[i] = 0;
	//	}

	//	for( uint i=0; i<eMaxFlag; ++i) //设置设置标志
	//	{
	//		m_bVertxAttribFlag[i] = false;
	//	}
	//}

	//BeginEndPrimitiveProxy::~BeginEndPrimitiveProxy()
	//{
	//	//不要做任何事情
	//}

	//void BeginEndPrimitiveProxy::release()
	//{
	//	//释放VBO
	//	safeRelease(m_pIndexBuffer);
	//	safeRelease(m_pPositionBuffer);
	//	safeRelease(m_pColorBuffer);
	//	safeRelease(m_pNormalBuffer);
	//	for( uint i=0; i<s_maxTextureUint; ++i)
	//	{
	//		safeRelease(m_pTexcoordBuffer[i]);
	//	}

	//	//释放自己
	//	delete this;
	//}

	//void BeginEndPrimitiveProxy::reset()
	//{	
	//	//重设顶点指针
	//	m_positionBufferPtr = 0;

	//	//重设设置标志
	//	for( uint i=0; i < eMaxFlag; ++i)
	//	{
	//		m_bVertxAttribFlag[i] = false;
	//	}

	//	//设置绘制标志
	//	m_bBetweenBeginEnd = false;
	//}

	//void BeginEndPrimitiveProxy::beginPrimitive(PrimitiveType pt)
	//{
	//	//重置
	//	reset();
	//	//当前类型
	//	m_curPrimitiveType = pt;
	//	//开始绘制
	//	m_bBetweenBeginEnd = true;
	//}

	//void BeginEndPrimitiveProxy::endPrimitive()
	//{
	//	//1，检查
	//	if( m_positionBufferPtr == 0 )//没有顶点处理
	//	{
	//		reset();
	//		return;
	//	}
	//	if( !m_bBetweenBeginEnd) //没有使用beginPrimitive函数
	//	{
	//		reset();
	//		return;
	//	}

	//	//2,图元预处理
	//	switch( m_curPrimitiveType )
	//	{
	//	case PT_QUAD_STRIP: //暂时不支持
	//		{
	//			reset();
	//			return;
	//		}
	//		break;
	//	case PT_LINES:
	//	case PT_LINE_STRIP:
	//		{
	//			//顶点数少于2
	//			if( m_positionBufferPtr < 2)
	//			{
	//				reset();
	//				return;
	//			}
	//		}
	//		break;
	//	case PT_LINE_LOOP: //用PT_LINE_STRIP实现
	//		{
	//			//顶点数少于2
	//			if( m_positionBufferPtr < 2)
	//			{
	//				reset();
	//				return;
	//			}

	//			//再压入开始的顶点
	//			xs::Vector3 vStartVertex(m_positionBuffer[0],m_positionBuffer[1],m_positionBuffer[2]);
	//			sendVertex( vStartVertex);
	//		}
	//		break;
	//	case PT_TRIANGLES:
	//		{
	//			//顶点数少于3
	//			if( m_positionBufferPtr < 3)
	//			{
	//				reset();
	//				return;
	//			}
	//			m_positionBufferPtr = (m_positionBufferPtr/3) * 3;//去掉无效的顶点	
	//		}
	//	case PT_TRIANGLE_STRIP:
	//	case PT_TRIANGLE_FAN:
	//		{
	//			//顶点数少于3
	//			if( m_positionBufferPtr < 3)
	//			{
	//				reset();
	//				return;
	//			}	
	//		}
	//		break;
	//	case PT_QUADS:
	//		{
	//			//不能绘制矩形
	//			if( m_positionBufferPtr < 4 )
	//			{
	//				reset();
	//				return;
	//			}

	//			//去掉无效数据
	//			m_positionBufferPtr = ( m_positionBufferPtr / 4) * 4;
	//		}
	//		break;
	//	case PT_POINTS:
	//		{
	//			//nothing to do
	//		}
	//		break;
	//	default: //不会到这一步
	//		{
	//			reset();
	//			return;
	//		}
	//		break;
	//	}

	//	//3,设置缓存，并且绘制
	//	m_pPositionBuffer->writeData(0, m_positionBufferPtr * sizeof(float) * 3, m_positionBuffer, true);
	//	m_pRenderSystem->setVertexVertexBuffer( m_pPositionBuffer, 0 );

	//	//颜色一定要绘制，忽略标志
	//	//if( m_bVertxAttribFlag[eDiffuseColorFlag])//颜色
	//	//{
	//		m_pColorBuffer->writeData(0, m_positionBufferPtr * sizeof(DWORD), m_diffuseColorBuffer, true);
	//		m_pRenderSystem->setDiffuseVertexBuffer( m_pColorBuffer, 0);
	//	//}

	//	if( m_bVertxAttribFlag[eNormalFlag] ) //法向量
	//	{
	//		m_pNormalBuffer->writeData(0, m_positionBufferPtr * sizeof(float)*3, m_normalBuffer, true);
	//		m_pRenderSystem->setNormalVertexBuffer( m_pNormalBuffer,0);
	//	}

	//	for( uint i=0; i< s_maxTextureUint; ++i)
	//	{
	//		if( m_bVertxAttribFlag[eTexcoord0+i] )//纹理坐标
	//		{
	//			m_pTexcoordBuffer[i]->writeData(0, m_positionBufferPtr * sizeof(float) * 2, m_texcoordBuffer[i], true);
	//			m_pRenderSystem->setTexcoordVertexBuffer(i, m_pTexcoordBuffer[i], 0);
	//		}
	//	}

	//	if( m_curPrimitiveType == PT_QUADS)//如果为四边形，用索引绘制
	//	{
	//		m_pRenderSystem->setIndexBuffer(m_pIndexBuffer);
	//		m_pRenderSystem->drawIndexedPrimitive(PT_TRIANGLES, (m_positionBufferPtr/4)*6, IT_16BIT);
	//	}
	//	else
	//	{
	//		m_pRenderSystem->drawPrimitive( m_curPrimitiveType, 0, m_positionBufferPtr);
	//	}
	//	
	//	//4,取消缓存设置
	//	m_pRenderSystem->setVertexVertexBuffer(0,0);

	//	//if( m_bVertxAttribFlag[eDiffuseColorFlag] )//颜色
	//		m_pRenderSystem->setDiffuseVertexBuffer(0,0);

	//	if( m_bVertxAttribFlag[eNormalFlag] ) //法向量
	//		m_pRenderSystem->setNormalVertexBuffer(0,0);

	//	for( uint i=0; i<s_maxTextureUint; ++i) //纹理坐标
	//	{
	//		if( m_bVertxAttribFlag[eTexcoord0+i] )
	//			m_pRenderSystem->setTexcoordVertexBuffer( i, 0,0);
	//	}


	//	//5, 复位
	//	reset();
	//}

	//void BeginEndPrimitiveProxy::setColor( const xs::ColorValue & color)
	//{
	//	ColorValue newcolor(color);
	//	xs::Math::clamp( newcolor.a, 0.0f, 1.0f);
	//	xs::Math::clamp( newcolor.r, 0.0f, 1.0f);
	//	xs::Math::clamp( newcolor.g, 0.0f, 1.0f);
	//	xs::Math::clamp( newcolor.b, 0.0f, 1.0f);

	//	uchar a = xs::Math::Floor( newcolor.a * 255 + 0.99999f );
	//	uchar r = xs::Math::Floor( newcolor.r * 255 + 0.99999f );
	//	uchar g = xs::Math::Floor( newcolor.g * 255 + 0.99999f );
	//	uchar b = xs::Math::Floor( newcolor.b * 255 + 0.99999f );

	//	//设置当前颜色
	//	m_currentColor = D3DCOLOR_ARGB( a, r, g, b);
	//	m_bVertxAttribFlag[eDiffuseColorFlag] = true;
	//}

	//void BeginEndPrimitiveProxy::setNormal( const xs::Vector3 & normal)
	//{
	//	//设置当前法向量
	//	m_currentNormal = normal;
	//	m_bVertxAttribFlag[eNormalFlag] = true;	
	//}

	//void BeginEndPrimitiveProxy::setTexcoord( const xs::Vector2 & texcoord)
	//{
	//	setMultiTexcoord(0, texcoord);
	//}

	//void BeginEndPrimitiveProxy::setMultiTexcoord(ushort unit, const xs::Vector2 & texcoord)
	//{
	//	if( unit >= s_maxTextureUint)//纹理映射单元越界
	//		return;

	//	//设置当前纹理坐标
	//	m_currentTexcoord[unit] = texcoord;
	//	m_bVertxAttribFlag[eTexcoord0 + unit] = true;
	//}

	//void	BeginEndPrimitiveProxy::sendVertex(const xs::Vector2 & vertex)
	//{
	//	xs::Vector3 v3(vertex.x, vertex.y, 0.0f);
	//	sendVertex( v3);
	//}

	//void	BeginEndPrimitiveProxy::sendVertex(const Vector3& vertex)
	//{
	//	//是否在begin和end之间
	//	if( !m_bBetweenBeginEnd) 
	//		return;

	//	//Buffer越界了
	//	if( m_positionBufferPtr >= s_vertexAttribBufferSize )
	//		return;

	//	//更新位置buffer
	//	uint posPointer = m_positionBufferPtr * 3;
	//	m_positionBuffer[posPointer + 0] = vertex.x;
	//	m_positionBuffer[posPointer + 1] = vertex.y;
	//	m_positionBuffer[posPointer + 2] = vertex.z;
	//	
	//	//更新法线Buffer
	//	uint normPointer = m_positionBufferPtr * 3;
	//	m_normalBuffer[normPointer + 0 ] = m_currentNormal.x;
	//	m_normalBuffer[normPointer + 1 ] = m_currentNormal.y;
	//	m_normalBuffer[normPointer + 2 ] = m_currentNormal.z;
	//	
	//	//更新颜色buffer
	//	uint colorPointer = m_positionBufferPtr;
	//	m_diffuseColorBuffer[colorPointer] = m_currentColor;

	//	//更新纹理buffer
	//	for( uint i=0; i<s_maxTextureUint; ++i)
	//	{
	//		uint texPointer = m_positionBufferPtr * 2;
	//		m_texcoordBuffer[i][texPointer + 0] = m_currentTexcoord[i].x;
	//		m_texcoordBuffer[i][texPointer + 1] = m_currentTexcoord[i].y;
	//	}


	//	//更新指针
	//	++m_positionBufferPtr;

	//}

	//void	BeginEndPrimitiveProxy::sendVertex(const Vector4& vertex)
	//{
	//	Assert( !Math::RealEqual(vertex.w, 0.0f));
	//	xs::Vector3 v3( vertex.x / vertex.w, vertex.y / vertex.w, vertex.z / vertex.w);
	//	sendVertex(v3);
	//}

	//void BeginEndPrimitiveProxy::setEdgeFlag(bool bEdgeFlag)
	//{
	//	//不支持
	//}

}