#ifndef __HighLevelShaderManager_H__
#define __HighLevelShaderManager_H__

#include "RenderEngine/Manager.h"

namespace xs
{
	class HighLevelShaderManager : public IShaderManager,public Manager<IShader*>
	{
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


		/** 释放自己
		*/
		virtual void			release();

	public:
		static HighLevelShaderManager*	Instance()
		{
			static HighLevelShaderManager sm;
			return &sm;
		}
	private:
		IShader*	_createShaderFromMemory(ShaderType st,const uchar* pBuffer,uint len,const std::string& name);
		void doDelete(IShader* pShader);
		const char* getUniqueString();

	protected:
		HighLevelShaderManager() : m_ui32Num(0)/*,m_pCurrentShaderProgram(0) */{}
	
	private:
		uint	m_ui32Num;
		//IShaderProgram * m_pCurrentShaderProgram;
	};
}

#endif