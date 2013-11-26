#ifndef __ShaderManagerHLSLD3D9_H__
#define __ShaderManagerHLSLD3D9_H__

#include "RenderEngine/Manager.h"

namespace xs
{

	class RenderSystemD3D9;
	class ShaderManagerHLSLD3D9;

	class ShaderManagerHLSLD3D9Creater
	{
	public:
		static ShaderManagerHLSLD3D9 * create(RenderSystemD3D9 * pRenderSystem);
	};

	class ShaderManagerHLSLD3D9 : public IShaderManager, public Manager<IShader*>
	{
		//IShaderManager接口
	public:
		/** 从文件中创建着色器
		@param ShaderType 着色器类型
		@param fn 文件名
		@return 着色器接口指针
		*/
		virtual IShader*		createShaderFromFile(ShaderType st,const std::string& fn);	

		/** 从内存数据中创建着色器
		@param ShaderType 着色器类型
		@param pBuffer 数据地址
		@param len 数据长度
		@return 着色器接口指针
		@see ShaderType
		*/
		virtual IShader*		createShaderFromMemory(ShaderType st,const uchar *pBuffer,uint len);	


		/** 销毁一个着色器
		@param IShader* 着色器指针
		*/
		virtual void 			releaseShader(IShader*);				

		/** 销毁所有的着色器
		*/
		virtual void 			releaseAllShader();

		/** 释放管理器
		*/
		virtual void	release();

		//Manager接口
	private:
		void doDelete(IShader* pShader);

	private:
		friend class ShaderManagerHLSLD3D9Creater;
		ShaderManagerHLSLD3D9();
		~ShaderManagerHLSLD3D9();
		bool create( RenderSystemD3D9 * pRenderSystem);

	private:
		const char* getUniqueString();
		IShader * _createShaderFromMemory(RenderSystemD3D9 * pRenderSystem, const std::string & name, ShaderType st,const uchar *pBuffer,uint len);

	private:
		uint	m_ui32Num;
		RenderSystemD3D9 * m_pRenderSystem;
	};
}

#endif