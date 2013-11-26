#ifndef __ShaderProgram_H__
#define __ShaderProgram_H__

namespace xs
{
	class ShaderManager;
	class ShaderProgramManagerOGL;

	class ShaderProgram : public ILowLevelShaderProgram
	{
	public:
		/** 释放着色器程序
		*/
		virtual void 			release();

		/** 绑定到此着色器程序
		@return 是否成功
		*/
		virtual bool			bind();	

		/** 取消绑定到此着色器
		*/
		virtual void 			unbind();

		/** 从文件中添加着色器
		@param ShaderType 着色器类型
		@param fn 文件名
		@return 着色器接口指针
		*/
		virtual bool			addShaderFromFile(ShaderType st,const std::string& fn);	

		/** 从内存数据中添加着色器
		@param ShaderType 着色器类型
		@param pBuffer 数据地址
		@param len 数据长度
		@return 着色器接口指针
		@see ShaderType
		*/
		virtual bool		addShaderFromMemory(ShaderType st,const uchar *pBuffer,uint len);

		/** 连结这个着色器程序
		@return 是否成功
		*/
		virtual bool			link();

		/** 获取着色器程序类型
		*/
		virtual ShaderProgramType getProgramType();

		/** 设置统一浮点四维向量
		@param startRegister	起始的浮点常量寄存器序号
		@param pData			浮点数据指针
		@param countVector4		四维浮点向量的个数
		*/
		virtual	bool		setUniformFloatVector4(uint startRegister, float * pData, uint countVector4);

		/** 设置统一整型四维向量
		@param startRegister	起始的整型常量寄存器序号
		@param pData			整型数据指针
		@param countVector4		四维整形向量的个数
		*/
		virtual	bool		setUniformIntVector4(uint startRegister, int * pData, uint countVector4);

		/** 设置统一布尔变量
		@param startRegister	起始的布尔常量寄存器序号
		@param pData			布尔数据指针，0表示false，非0表示true
		@param countBool		布尔数据的个数
		*/
		virtual	bool		setUniformBool(uint startRegister, int * pData, uint countBool);


		/** 将世界，摄像机，投影等矩阵绑定到shader的常量,适合低shader，只能同时绑定一个矩阵
		*/
		virtual void			bindTransformMatrix(uint index, uint type, uint bindRow=4);

	public:
		ShaderProgram(ShaderProgramManagerOGL* pProgramMgr, ShaderManager * pShaderMgr):m_pShaderManager(pShaderMgr),
			m_pShaderProgramManager(pProgramMgr) {};
		~ShaderProgram(){};

	private:
		typedef std::vector<IShader *>		ShaderContainer;
		typedef ShaderContainer::iterator	ShaderContainerIterator;
		ShaderContainer						m_vShaders;
		ShaderManager*						m_pShaderManager;
		ShaderProgramManagerOGL*			m_pShaderProgramManager;
	};
}

#endif