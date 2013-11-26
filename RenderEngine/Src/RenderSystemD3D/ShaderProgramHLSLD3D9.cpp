#include "StdAfx.h"
#include "ShaderProgramHLSLD3D9.h"
#include "ShaderFragmentHLSLD3D9.h"
#include "ShaderProgramManagerD3D9.h"
#include "ShaderManagerHLSLD3D9.h"


namespace xs
{

	ShaderProgramHLSLD3D9::ShaderProgramHLSLD3D9(ShaderProgramManagerD3D9 * pProgramMgr, ShaderManagerHLSLD3D9 * pShaderMgr):m_pVertexShader(0),
		m_pPixelShader(0),
		m_pShaderProgramManager(pProgramMgr),
		m_pShaderManager(pShaderMgr)
	{
	}

	/** 释放着色器程序
	*/
	void ShaderProgramHLSLD3D9::release()
	{
		if( m_pVertexShader ) m_pShaderManager->releaseShader(m_pVertexShader);
		if( m_pPixelShader)   m_pShaderManager->releaseShader(m_pPixelShader);
		delete this;
	}

	/** 绑定到此着色器程序
	@return 是否成功
	*/
	bool ShaderProgramHLSLD3D9::bind()
	{
		if( m_pVertexShader)
			m_pVertexShader->bind();
		if( m_pPixelShader)
			m_pPixelShader->bind();
		m_pShaderProgramManager->onShaderProgramBinded(this);

		return true;
	}

	/** 取消绑定到此着色器
	*/
	void ShaderProgramHLSLD3D9::unbind()
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
	bool ShaderProgramHLSLD3D9::addShaderFromFile(ShaderType st,const std::string& fn)
	{
		IShader * pShader = m_pShaderManager->createShaderFromFile(st, fn);
		if( 0 == pShader ) return false;

		if( ST_VERTEX_PROGRAM == st) m_pVertexShader = static_cast<ShaderFragmentHLSLD3D9 *>(pShader);
		else  m_pPixelShader = static_cast<ShaderFragmentHLSLD3D9 *>(pShader);

		return true;
	}

	/** 从内存数据中添加着色器
	@param ShaderType 着色器类型
	@param pBuffer 数据地址
	@param len 数据长度
	@return 着色器接口指针
	@see ShaderType
	*/
	bool ShaderProgramHLSLD3D9::addShaderFromMemory(ShaderType st,const uchar *pBuffer,uint len)
	{
		IShader * pShader = m_pShaderManager->createShaderFromMemory(st, pBuffer, len);
		if( 0 == pShader ) return false;

		if( ST_VERTEX_PROGRAM == st) m_pVertexShader = static_cast<ShaderFragmentHLSLD3D9 *>(pShader);
		else  m_pPixelShader = static_cast<ShaderFragmentHLSLD3D9 *>(pShader);

		return true;	
	}


	/** 连结这个着色器程序
	@return 是否成功
	*/
	bool ShaderProgramHLSLD3D9::link()	
	{
		return true;
	}

	/** 获取着色器程序类型
	*/
	ShaderProgramType ShaderProgramHLSLD3D9::getProgramType()
	{
		return SPT_HIGHLEVEL;
	}


	/** 设置统一布尔型变量
	@param	strPara 变量名
	@param	value   变量值
	@return	是否设置成功
	*/
	bool ShaderProgramHLSLD3D9::setUniformBool(const std::string & strPara,  int value)
	{
		bool vertex_ret = false;
		if( m_pVertexShader)
		{
			vertex_ret = m_pVertexShader->setUniformBool(strPara, value);
		}
		if( vertex_ret) return true;

		bool pixel_ret = false;
		if( m_pPixelShader)
		{
			pixel_ret = m_pPixelShader->setUniformBool(strPara, value);
		}
		return pixel_ret;
	}

	/** 设置统一整型变量
	@param	strPara 变量名
	@param	value	变量值
	@return	是否设置成功
	*/
	bool ShaderProgramHLSLD3D9::setUniformInt(const std::string & strPara, int value)
	{
		bool vertex_ret = false;
		if( m_pVertexShader)
		{
			vertex_ret = m_pVertexShader->setUniformInt(strPara, value);
		}
		if( vertex_ret) return true;

		bool pixel_ret = false;
		if( m_pPixelShader)
		{
			pixel_ret = m_pPixelShader->setUniformInt(strPara, value);
		}
		return pixel_ret;
	}

	/** 设置统浮点型变量
	@param	strPara 变量名
	@param	value	变量值
	@return	是否设置成功
	*/
	bool ShaderProgramHLSLD3D9::setUniformFloat(const std::string & strPara, float value)
	{
		bool vertex_ret = false;
		if( m_pVertexShader)
		{
			vertex_ret = m_pVertexShader->setUniformFloat(strPara, value);
		}
		if( vertex_ret) return true;

		bool pixel_ret = false;
		if( m_pPixelShader)
		{
			pixel_ret = m_pPixelShader->setUniformFloat(strPara, value);
		}
		return pixel_ret;
	}

	/** 设置统一向量变量
	@param	strPara 变量名
	@param	value	变量值
	@return	是否设置成功
	*/
	bool			ShaderProgramHLSLD3D9::setUniformVector4(const std::string & strPara, const Vector4 & value)
	{
		bool vertex_ret = false;
		if( m_pVertexShader)
		{
			vertex_ret = m_pVertexShader->setUniformVector4(strPara, value);
		}
		if( vertex_ret) return true;

		bool pixel_ret = false;
		if( m_pPixelShader)
		{
			pixel_ret = m_pPixelShader->setUniformVector4(strPara, value);
		}
		return pixel_ret;
	}

	/** 设置一般的矩阵
	@param	strPara 变量名
	@param	value	变量值，矩阵的数据，是行主序的
	@return			是否设置成功
	*/
	bool			ShaderProgramHLSLD3D9::setUniformMatrix(const std::string & strPara, const Matrix4 &  value,bool normal)
	{
		bool vertex_ret = false;
		if( m_pVertexShader)
		{
			vertex_ret = m_pVertexShader->setUniformMatrix(strPara, value);
		}
		if( vertex_ret) return true;

		bool pixel_ret = false;
		if( m_pPixelShader)
		{
			pixel_ret = m_pPixelShader->setUniformMatrix(strPara, value);
		}
		return pixel_ret;
	}


	/** 设置统一布尔型数组
	@param	strPara 变量名
	@param	count	数组大小
	@param	pValue  数组地址
	@return	是否设置成功
	*/
	bool ShaderProgramHLSLD3D9::setUniformBoolArray(const std::string & strPara, uint count,  int * pValue)
	{
		bool vertex_ret = false;
		if( m_pVertexShader)
		{
			vertex_ret = m_pVertexShader->setUniformBoolArray(strPara, count, pValue);
		}
		if( vertex_ret) return true;

		bool pixel_ret = false;
		if( m_pPixelShader)
		{
			pixel_ret = m_pPixelShader->setUniformBoolArray(strPara, count, pValue);
		}
		return pixel_ret;
	}

	/** 设置统一整型数组
	@param	strPara 变量名
	@param	count	数组大小
	@param	pValue  数组地址
	@return	是否设置成功
	*/
	bool			ShaderProgramHLSLD3D9::setUniformIntArray(const std::string & strPara, uint count,  int * pValue)
	{
		bool vertex_ret = false;
		if( m_pVertexShader)
		{
			vertex_ret = m_pVertexShader->setUniformIntArray(strPara, count, pValue);
		}
		if( vertex_ret) return true;

		bool pixel_ret = false;
		if( m_pPixelShader)
		{
			pixel_ret = m_pPixelShader->setUniformIntArray(strPara, count, pValue);
		}
		return pixel_ret;
	}

	/** 设置统一浮点型数组
	@param	strPara 变量名
	@param	count	数组大小
	@param	pValue  数组地址
	@return	是否设置成功
	*/
	bool			ShaderProgramHLSLD3D9::setUniformFloatArray(const std::string & strPara, uint count,  float * pValue)
	{
		bool vertex_ret = false;
		if( m_pVertexShader)
		{
			vertex_ret = m_pVertexShader->setUniformFloatArray(strPara, count, pValue);
		}
		if( vertex_ret) return true;

		bool pixel_ret = false;
		if( m_pPixelShader)
		{
			pixel_ret = m_pPixelShader->setUniformFloatArray(strPara, count, pValue);
		}
		return pixel_ret;
	}

	/** 设置统一向量型数组
	@param	strPara 变量名
	@param	count	数组大小
	@param	pValue  数组地址
	@return	是否设置成功
	*/
	bool			ShaderProgramHLSLD3D9::setUniformVector4Array(const std::string & strPara, uint count,  xs::Vector4 * pValue)
	{
		bool vertex_ret = false;
		if( m_pVertexShader)
		{
			vertex_ret = m_pVertexShader->setUniformVector4Array(strPara, count, pValue);
		}
		if( vertex_ret) return true;

		bool pixel_ret = false;
		if( m_pPixelShader)
		{
			pixel_ret = m_pPixelShader->setUniformVector4Array(strPara, count, pValue);
		}
		return pixel_ret;
	}

	/** 设置统一矩阵型数组
	@param	strPara 变量名
	@param	count	数组大小
	@param	pValue  数组地址
	@return	是否设置成功
	*/
	bool			ShaderProgramHLSLD3D9::setUniformMatrixArray(const std::string & strPara, uint count,  xs::Matrix4 * pValue)
	{
		bool vertex_ret = false;
		if( m_pVertexShader)
		{
			vertex_ret = m_pVertexShader->setUniformMatrixArray(strPara, count, pValue);
		}
		if( vertex_ret) return true;

		bool pixel_ret = false;
		if( m_pPixelShader)
		{
			pixel_ret = m_pPixelShader->setUniformMatrixArray(strPara, count, pValue);
		}
		return pixel_ret;
	}


	/** 将采样器绑定到shader的常量
	*/
	void ShaderProgramHLSLD3D9::bindSampler(  uint smaplerNum  )
	{
		//shader 内部绑定
	}

	/** 将世界，摄像机，投影矩阵绑定到shader的常量
	*/
	void ShaderProgramHLSLD3D9::bindTransformMatrix(  uint flag  )
	{
		//只对vertex shader 有效
		if(m_pVertexShader) m_pVertexShader->bindTransformMatrix( flag );
	}
}