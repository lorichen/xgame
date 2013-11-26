#include "StdAfx.h"
#include "ShaderProgramD3D9.h"
#include "ShaderFragmentD3D9.h"
#include "ShaderManagerD3D9.h"
#include "ShaderProgramManagerD3D9.h"


namespace xs
{

	ShaderProgramD3D9::ShaderProgramD3D9(ShaderProgramManagerD3D9 * pProgramMgr, ShaderManagerD3D9 * pShaderMgr):m_pVertexShader(0),
		m_pPixelShader(0),
		m_pShaderManager(pShaderMgr),
		m_pShaderProgramManager(pProgramMgr)
	{
	}

	/** 释放着色器程序
	*/
	void ShaderProgramD3D9::release()
	{
		if( m_pVertexShader) m_pShaderManager->releaseShader( m_pVertexShader );
		if( m_pPixelShader ) m_pShaderManager->releaseShader( m_pPixelShader  );

		delete this;
	}

	/** 绑定到此着色器程序
	@return 是否成功
	*/
	bool ShaderProgramD3D9::bind()
	{
		if( m_pVertexShader)
			m_pVertexShader->bind();
		if( m_pPixelShader)
			m_pPixelShader->bind();

		m_pShaderProgramManager->onShaderProgramBinded( this);
		return true;
	}

	/** 取消绑定到此着色器
	*/
	void ShaderProgramD3D9::unbind()
	{
		if( m_pVertexShader)
			m_pVertexShader->unbind();
		if( m_pPixelShader)
			m_pPixelShader->unbind();

		m_pShaderProgramManager->onShaderProgramUnbinded(this);
	}

	/** 从文件中添加着色器
	@param ShaderType 着色器类型
	@param fn 文件名
	@return 着色器接口指针
	*/
	bool ShaderProgramD3D9::addShaderFromFile(ShaderType st,const std::string& fn)
	{
		IShader * pShader = m_pShaderManager->createShaderFromFile(st, fn);
		if( 0 == pShader ) return false;

		if( ST_VERTEX_PROGRAM == st) m_pVertexShader = static_cast< xs::ShaderFragmentD3D9 *>(pShader);
		else m_pPixelShader = static_cast< xs::ShaderFragmentD3D9 *>(pShader);
		return true;
	}

	/** 从内存数据中添加着色器
	@param ShaderType 着色器类型
	@param pBuffer 数据地址
	@param len 数据长度
	@return 着色器接口指针
	@see ShaderType
	*/
	bool ShaderProgramD3D9::addShaderFromMemory(ShaderType st,const uchar *pBuffer,uint len)
	{
		IShader * pShader = m_pShaderManager->createShaderFromMemory(st, pBuffer, len);
		if( 0 == pShader ) return false;

		if( ST_VERTEX_PROGRAM == st) m_pVertexShader = static_cast< xs::ShaderFragmentD3D9 *>(pShader);
		else m_pPixelShader = static_cast< xs::ShaderFragmentD3D9 *>(pShader);	
		return true;
	}


	/** 连结这个着色器程序
	@return 是否成功
	*/
	bool ShaderProgramD3D9::link()	
	{
		return true;
	}

	/** 获取着色器程序类型
	*/
	ShaderProgramType ShaderProgramD3D9::getProgramType()
	{
		return SPT_LOWLEVEL;
	}

	/** 设置统一浮点四维向量
	@param startRegister	起始的浮点常量寄存器序号
	@param pData			浮点数据指针
	@param countVector4		四维浮点向量的个数
	*/
	bool ShaderProgramD3D9::setUniformFloatVector4(uint startRegister, float * pData, uint countVector4)
	{
		bool vertex_ret = false;
		if( m_pVertexShader)
		{
			vertex_ret = m_pVertexShader->setUniformFloatVector4(startRegister, pData, countVector4);
		}
		if( vertex_ret) return true;
		bool pixel_ret = false;
		if( m_pPixelShader)
		{
			pixel_ret = m_pPixelShader->setUniformFloatVector4(startRegister, pData, countVector4);
		}

		return pixel_ret;
	}

	/** 设置统一整型四维向量
	@param startRegister	起始的整型常量寄存器序号
	@param pData			整型数据指针
	@param countVector4		四维整形向量的个数
	*/
	bool ShaderProgramD3D9::setUniformIntVector4(uint startRegister, int * pData, uint countVector4)
	{
		bool vertex_ret = false;
		if( m_pVertexShader)
		{
			vertex_ret = m_pVertexShader->setUniformIntVector4(startRegister, pData, countVector4);
		}
		if( vertex_ret) return true;

		bool pixel_ret = false;
		if( m_pPixelShader)
		{
			pixel_ret = m_pPixelShader->setUniformIntVector4(startRegister, pData, countVector4);
		}

		return pixel_ret;
	}

	/** 设置统一布尔变量
	@param startRegister	起始的布尔常量寄存器序号
	@param pData			布尔数据指针，0表示false，非0表示true
	@param countBool		布尔数据的个数
	*/
	bool ShaderProgramD3D9::setUniformBool(uint startRegister, int * pData, uint countBool)
	{
		bool vertex_ret = false;
		if( m_pVertexShader)
		{
			vertex_ret = m_pVertexShader->setUniformBool(startRegister, pData, countBool);
		}
		if( vertex_ret) return true;

		bool pixel_ret = false;
		if( m_pPixelShader)
		{
			pixel_ret = m_pPixelShader->setUniformBool(startRegister, pData, countBool);
		}
		return pixel_ret;
	}

	/** 将世界，摄像机，投影等矩阵绑定到shader的常量,适合低shader，只能同时绑定一个矩阵
	*/
	void ShaderProgramD3D9::bindTransformMatrix(uint index, uint type, uint bindRow)
	{
		//只对vertex shader有效
		if( m_pVertexShader)
		{
			m_pVertexShader->bindTransformMatrix(index, type, bindRow);
		}
	}
}