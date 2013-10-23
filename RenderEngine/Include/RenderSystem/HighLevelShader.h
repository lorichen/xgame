#ifndef __HighLevelShader_H__
#define __HighLevelShader_H__

#include "RenderEngine/Manager.h"

namespace xs
{
	class HighLevelShader : public ManagedItem,public IShader
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
		HighLevelShader(const std::string& name) : ManagedItem(name){}

		bool create(ShaderType st,const uchar *pBuffer,uint len);
		GLhandleARB getHandle();
	private:
		ShaderType	m_type;
		GLenum		m_GLType;
		GLhandleARB	m_handle;
	};
}

#endif