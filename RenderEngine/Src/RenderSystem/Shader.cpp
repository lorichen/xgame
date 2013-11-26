#include "StdAfx.h"
#include "Shader.h"

namespace xs
{
	extern GLenum getGLShaderType(ShaderType st);

	bool Shader::create(ShaderType st,const uchar *pBuffer,uint len)
	{
		glGenProgramsARB(1,&m_id);
		if(m_id)
		{
			m_type = st;
			m_GLType = getGLShaderType(st);
			glBindProgramARB(m_GLType,m_id);
			glProgramStringARB(m_GLType,GL_PROGRAM_FORMAT_ASCII_ARB,len,pBuffer);
			GLenum e = glGetError();
			if(e)
			{
				glDeleteProgramsARB(1,&m_id);
				char str[256];
				sprintf(str,"%s\n",gluErrorString(e));
				Error(str);
				return false;
			}
			return true;
		}
		return false;
	}

	bool			Shader::compile()
	{
		return true;
	}

	bool Shader::bind()
	{
		glBindProgramARB(m_GLType,m_id);
		glEnable(m_GLType);

		return true;
	}

	void Shader::unbind()
	{
		glDisable(m_GLType);
	}

	ShaderType Shader::getType()
	{
		return m_type;
	}

	GLuint	Shader::getGLShaderID(){return m_id;}


	/** 设置统一浮点变量数值
	@param startRegister	常量寄存器序号
	@param pData			变量的值
	@param countVector4		浮点变量的个数	
	*/
	bool Shader::setProgramLocalFloatVector4(uint startRegister, float * pData, uint countVector4)
	{
		if( 0 == pData || countVector4 <= 0 )
			return false;
		for( uint i =0; i<countVector4; ++i)
		{
			glProgramLocalParameter4fvARB(m_GLType, startRegister, pData);
			++startRegister;
			pData +=4;
		}
		return true;
	}
}