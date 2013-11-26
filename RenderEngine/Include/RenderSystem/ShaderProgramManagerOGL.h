#ifndef __ShaderProgramManagerOGL_H__
#define __ShaderProgramManagerOGL_H__

namespace xs
{
	class ShaderManager;
	class HighLevelShaderManager;

	class ShaderProgramManagerOGL: public IShaderProgramManager
	{ 
		//<<IShaderProgramManager>>
	public:
		/** 创建着色器程序
		@return 着色器程序接口指针
		*/
		virtual IShaderProgram*		createShaderProgram(ShaderProgramType type);

		/**	 获取正在用于渲染的着色器程序
		@return 着色器程序接口指针
		*/
		virtual IShaderProgram*		getCurrentShaderProgam();

		/** 释放自己
		*/
		virtual void			release();
		
	public:
		void	onShaderProgramBinded(IShaderProgram * pProgram);
		void	onShaderProgramUnbinded(IShaderProgram * pProgram);
		ShaderManager*			getLowLevelShaderManager();
		HighLevelShaderManager*	getHighLevelShaderManager();

	public:
		static ShaderProgramManagerOGL*	Instance()
		{
			static ShaderProgramManagerOGL spm;
			return &spm;
		}
	protected:
		ShaderProgramManagerOGL():m_pCurrentShaderProgram(0) {};

	private:
		IShaderProgram * m_pCurrentShaderProgram;
	};

}
#endif