#include "StdAfx.h"
#include "HighLevelShader.h"

namespace xs
{
	GLenum getGLHighLevelShaderType(ShaderType st)
	{
		switch(st)
		{
		case ST_VERTEX_PROGRAM:
			return GL_VERTEX_SHADER_ARB;
		case ST_FRAGMENT_PROGRAM:
			return GL_FRAGMENT_SHADER_ARB;
		}

		return GL_VERTEX_SHADER_ARB;
	}

	bool HighLevelShader::create(ShaderType st,const uchar *pBuffer,uint len)
	{
		m_type = st;

		m_handle = glCreateShaderObjectARB(getGLHighLevelShaderType(st));
		if(m_handle)
		{
			glShaderSourceARB(m_handle,1,(const GLcharARB**)&pBuffer,NULL);
			glCompileShaderARB(m_handle);
		}

		return m_handle != 0;
	}

	bool			HighLevelShader::compile()
	{
		return true;
	}

	bool HighLevelShader::bind()
	{
		return true;
	}

	void HighLevelShader::unbind()
	{
	}

	ShaderType HighLevelShader::getType()
	{
		return m_type;
	}

	GLhandleARB HighLevelShader::getHandle()
	{
		return m_handle;
	}
}