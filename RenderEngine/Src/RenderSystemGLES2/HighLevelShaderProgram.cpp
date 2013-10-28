#include "StdAfx.h"
#include "HighLevelShaderProgram.h"
#include "HighLevelShader.h"
#include "HighLevelShaderManager.h"
#include "ShaderProgramManagerOGL.h"
namespace xs
{
	void	HighLevelShaderProgram::release()
	{
		ShaderContainerIterator it = m_vShaders.begin();
		for( ; it != m_vShaders.end(); ++it)
			m_pShaderManger->releaseShader( *it);

		m_vShaders.clear();
		glDeleteObjectARB(m_handle);
		delete this;
	}
	
	bool	HighLevelShaderProgram::bind()
	{
		glUseProgramObjectARB(m_handle);
		m_pShaderProgramManager->onShaderProgramBinded(this);
		return true;
	}

	void	HighLevelShaderProgram::unbind()
	{
		glUseProgramObjectARB(0);
		m_pShaderProgramManager->onShaderProgramUnbinded(this);
	}

	bool HighLevelShaderProgram::addShaderFromFile(ShaderType st,const std::string& fn)
	{
		IShader * pShader = m_pShaderManger->createShaderFromFile(st, fn);
		if( 0 == pShader ) return false;

		m_vShaders.push_back(pShader);
		glAttachObjectARB(m_handle,static_cast<HighLevelShader*>(pShader)->getHandle());
		return true;
	}

	bool HighLevelShaderProgram::addShaderFromMemory(ShaderType st,const uchar *pBuffer,uint len)
	{
		IShader * pShader = m_pShaderManger->createShaderFromMemory(st, pBuffer, len);
		if( 0 == pShader ) return false;

		m_vShaders.push_back(pShader);
		glAttachObjectARB(m_handle,static_cast<HighLevelShader*>(pShader)->getHandle());
		return true;
	}

	bool	HighLevelShaderProgram::link()
	{
		glLinkProgramARB(m_handle);
		GLint bLinked;
		glGetObjectParameterivARB(m_handle,GL_OBJECT_LINK_STATUS_ARB,&bLinked);
		return bLinked != 0;
	}

	/** 获取着色器程序类型
	*/
	ShaderProgramType HighLevelShaderProgram::getProgramType()
	{
		return SPT_HIGHLEVEL;
	}

	bool HighLevelShaderProgram::create()
	{
		m_handle = glCreateProgramObjectARB();
		return m_handle != 0;
	}



	/** 设置统一布尔型变量
	@param	strPara 变量名
	@param	value   变量值
	@return	是否设置成功
	*/
	bool HighLevelShaderProgram::setUniformBool(const std::string & strPara,  int value)
	{
		GLint loc = glGetUniformLocationARB(m_handle, strPara.c_str() );
		if( loc < 0 ) return false;

		glUniform1iARB(loc,(GLint)value);

		return true;
	}

	/** 设置统一整型变量
	@param	strPara 变量名
	@param	value	变量值
	@return	是否设置成功
	*/
	bool HighLevelShaderProgram::setUniformInt(const std::string & strPara, int value)
	{
		GLint loc = glGetUniformLocationARB(m_handle, strPara.c_str() );
		if( loc < 0 ) return false;

		glUniform1iARB(loc,(GLint)value);

		return true;
	}

	/** 设置统浮点型变量
	@param	strPara 变量名
	@param	value	变量值
	@return	是否设置成功
	*/
	bool HighLevelShaderProgram::setUniformFloat(const std::string & strPara, float value)
	{
		GLint loc = glGetUniformLocationARB(m_handle, strPara.c_str() );
		if( loc < 0 ) return false;

		glUniform1fARB(loc,(GLfloat)value);

		return true;	
	}

	/** 设置统一向量变量
	@param	strPara 变量名
	@param	value	变量值
	@return	是否设置成功
	*/
	bool			HighLevelShaderProgram::setUniformVector4(const std::string & strPara, const Vector4 & value)
	{
		GLint loc = glGetUniformLocationARB(m_handle, strPara.c_str() );
		if( loc < 0 ) return false;

		glUniform4fARB(loc,value.x, value.y, value.z, value.w);

		return true;
	}

	/** 设置一般的矩阵
	@param	strPara 变量名
	@param	value	变量值，矩阵的数据，是行主序的
	@return			是否设置成功
	*/
	bool HighLevelShaderProgram::setUniformMatrix(const std::string & strPara, const Matrix4 &  value)
	{
		GLint loc = glGetUniformLocationARB(m_handle, strPara.c_str() );
		if( loc < 0) return false;

		glUniformMatrix4fvARB(loc, 1, GL_TRUE, &value[0][0]);
		return true;
	}


	/** 设置统一布尔型数组
	@param	strPara 变量名
	@param	count	数组大小
	@param	pValue  数组地址
	@return	是否设置成功
	*/
	bool HighLevelShaderProgram::setUniformBoolArray(const std::string & strPara, uint count,  int * pValue)
	{
		if( count <= 0 || 0 == pValue) return false;
		GLint loc = glGetUniformLocationARB(m_handle, strPara.c_str() );
		if( loc < 0 ) return false;

		glUniform1ivARB(loc, (GLsizei)count, (GLint*)pValue);

		return true;	
	}

	/** 设置统一整型数组
	@param	strPara 变量名
	@param	count	数组大小
	@param	pValue  数组地址
	@return	是否设置成功
	*/
	bool HighLevelShaderProgram::setUniformIntArray(const std::string & strPara, uint count,  int * pValue)
	{
		if( count <= 0 || 0 == pValue) return false;
		GLint loc = glGetUniformLocationARB(m_handle, strPara.c_str() );
		if( loc < 0 ) return false;

		glUniform1ivARB(loc, (GLsizei)count, (GLint*)pValue);

		return true;
	}

	/** 设置统一浮点型数组
	@param	strPara 变量名
	@param	count	数组大小
	@param	pValue  数组地址
	@return	是否设置成功
	*/
	bool HighLevelShaderProgram::setUniformFloatArray(const std::string & strPara, uint count,  float * pValue)
	{
		if( count <= 0 || 0 == pValue) return false;
		GLint loc = glGetUniformLocationARB(m_handle, strPara.c_str() );
		if( loc < 0 ) return false;

		glUniform1fvARB(loc, (GLsizei)count, (const GLfloat*)pValue);

		return true;
	}

	/** 设置统一向量型数组
	@param	strPara 变量名
	@param	count	数组大小
	@param	pValue  数组地址
	@return	是否设置成功
	*/
	bool HighLevelShaderProgram::setUniformVector4Array(const std::string & strPara, uint count,  xs::Vector4 * pValue)
	{
		if( count <= 0 || 0 == pValue) return false;
		GLint loc = glGetUniformLocationARB(m_handle, strPara.c_str() );
		if( loc < 0 ) return false;

		glUniform4fvARB(loc, (GLsizei)count, (const GLfloat*)pValue->val);

		return true;
	}

	/** 设置统一矩阵型数组
	@param	strPara 变量名
	@param	count	数组大小
	@param	pValue  数组地址
	@return	是否设置成功
	*/
	bool			HighLevelShaderProgram::setUniformMatrixArray(const std::string & strPara, uint count,  xs::Matrix4 * pValue)
	{
		if( count <= 0 || 0 == pValue) return false;
		GLint loc = glGetUniformLocationARB(m_handle, strPara.c_str() );
		if( loc < 0) return false;

		glUniformMatrix4fvARB(loc, count, GL_TRUE, &pValue[0][0][0]);
		return true;
	}

	/** 将采样器绑定到shader的常量
	*/
	void HighLevelShaderProgram::bindSampler( uint smaplerNum )
	{
		const static char * s_pSamplerName [8] = 
		{
			"texture0",
			"texture1",
			"texture2",
			"texture3",
			"texture4",
			"texture5",
			"texture6",
			"texture7",
		};

		if( smaplerNum > 8 ) smaplerNum =  8;
		for( uint i=0; i < smaplerNum; ++i)
		{
			GLint loc = glGetUniformLocationARB( m_handle, s_pSamplerName[i]);
			if( loc < 0) continue;
			glUniform1iARB(loc,i);
		}
	}

	/** 将世界，摄像机，投影矩阵绑定到shader的常量
	*/
	void HighLevelShaderProgram::bindTransformMatrix( uint flag )
	{
		//内部已经绑定了
	}
}