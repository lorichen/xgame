#ifndef __Shader_H__
#define __Shader_H__

#include "RenderEngine/Manager.h"

namespace xs
{
	class Shader : public ManagedItem,public IShader
	{
	public:
		/** 获得着色器类型顶点着色器还是像素着色器
		@return 着色器类型
		@see ShaderType
		*/
		virtual ShaderType		getType();

		/** 编译着色器
		@return 是否成功
		*/
		virtual bool			compile();

		/** 绑定到此着色器
		@return 是否绑定成功
		*/
		virtual bool			bind();	

		/** 取消绑定此着色器，和bind配套使用
		*/
		virtual void 			unbind();

	public:

		/** 设置统一浮点变量数值
		@param startRegister	常量寄存器序号
		@param pData			变量的值
		@param countVector4		浮点变量的个数	
		*/
		bool				setProgramLocalFloatVector4(uint startRegister, float * pData, uint countVector4);

	public:
		Shader(const std::string& name) : m_id(0),ManagedItem(name){}
		bool create(ShaderType st,const uchar *pBuffer,uint len);
		GLuint	getGLShaderID();

	private:
		GLuint	m_id;
		ShaderType	m_type;
		GLenum		m_GLType;
	};
}

#endif