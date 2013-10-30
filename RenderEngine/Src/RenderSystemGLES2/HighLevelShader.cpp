#include "StdAfx.h"
#include "HighLevelShader.h"

namespace xs
{
	GLenum getGLHighLevelShaderType(ShaderType st)
	{
		switch(st)
		{
		case ST_VERTEX_PROGRAM:
			return GL_VERTEX_SHADER;
		case ST_FRAGMENT_PROGRAM:
			return GL_FRAGMENT_SHADER;
		}
		return GL_VERTEX_SHADER;
	}

	bool HighLevelShader::create(ShaderType st,const uchar *pBuffer,uint len)
	{
		m_type = st;

		m_handle = glCreateShader(getGLHighLevelShaderType(st));
		if(m_handle)
		{
			glShaderSource(m_handle,1,(const char**)&pBuffer,NULL);
			glCompileShader(m_handle);

			GLint bShaderCompiled;
			glGetShaderiv(m_handle, GL_COMPILE_STATUS, &bShaderCompiled);
			if (!bShaderCompiled)
			{
				int i32InfoLogLength, i32CharsWritten;
				glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &i32InfoLogLength);
				char* pszInfoLog = new char[i32InfoLogLength];
				glGetShaderInfoLog(m_handle, i32InfoLogLength, &i32CharsWritten, pszInfoLog);
				printf("Failed to compile fragment shader! : %s\n",pszInfoLog);
				delete[] pszInfoLog;
				glDeleteShader(m_handle);
				return false;
			}
			return true;
		}
		return false;
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

	GLuint HighLevelShader::getHandle()
	{
		return m_handle;
	}
}