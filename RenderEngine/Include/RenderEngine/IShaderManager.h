#ifndef __I_ShaderManager_H__
#define __I_ShaderManager_H__
/*

写GLSL的shader时候，绑定采样器对应表
"texture0": 0
"texture1": 1
"texture2": 2
"texture3": 3
"texture4": 4
"texture5": 5
"texture6": 6 
"texture7": 7


写HLSL， 变换矩阵对应表，只对vertex shader 有效
"WORLD_MATRIX": world matrix
"VIEW_MATRIX": view matrix
"WORLD_VIEW_MATRIX": world view matrix
"PROJECTION_MATRIX": projection matrix
"VIEW_PROJECTION_MATRIX": view projection matrix
"WORLD_VIEW_PROJECTION_MATRIX": world view projection matrix
"WORLD_NORMAL_MATRIX":transform normal from object  to world space
"VIEW_NORMAL_MATRIX":transform normal from  world space to view space
"WORLD_VIEW_NORMAL_MATRIX":transform normal from object space to view space

*/

namespace xs
{
	/*! addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	enum ShaderType
	{
		ST_VERTEX_PROGRAM,
		ST_FRAGMENT_PROGRAM
	};

	/** 着色器接口,这个接口暂时给内部使用，外部获取不到
	*/
	struct IShader
	{
		/** 获得着色器类型顶点着色器还是像素着色器
		@return 着色器类型
		@see ShaderType
		*/
		virtual ShaderType		getType() = 0;

		/** 编译着色器
		@return 是否成功
		*/
		virtual bool			compile() = 0;

		/** 绑定到此着色器
		@return 是否绑定成功
		*/
		virtual bool			bind() = 0;	

		/** 取消绑定此着色器，和bind配套使用
		*/
		virtual void 			unbind() = 0;
	};


	/**变换矩阵类型
	*/
	enum TransformMatrixType
	{
		// world matrix
		TMT_WORLD = 0x1,
		// view matrix
		TMT_VIEW = 0x2,
		// world view matrix
		TMT_WORLD_VIEW = 0x4,
		// projection matrix
		TMT_PROJECTION = 0x8,
		// view projection matrix
		TMT_VIEW_PROJECTION = 0x10,
		// world view and projection matrix
		TMT_WORLD_VIEW_PROJECTION = 0x20,
		// transform normal from object  to world space
		TMT_WORLD_NORMAL = 0x40,
		// transform normal from  world space to view space
		TMT_VIEW_NORMAL = 0x80,
		// transform normal from object space to view space
		TMT_WORLD_VIEW_NORMAL = 0x100,
	};

	/** 着色器程序类型
	*/
	enum ShaderProgramType
	{
		SPT_LOWLEVEL, //低阶shader程序
		SPT_HIGHLEVEL,//高阶shader程序
	};

	/** 着色器程序接口
	*/
	struct IShaderProgram
	{
		/** 获取着色器程序类型
		*/
		virtual ShaderProgramType getProgramType()=0;

		/** 释放着色器程序
		*/
		virtual void 			release() = 0;

		/** 绑定到此着色器程序
		@return 是否成功
		*/
		virtual bool			bind() = 0;	

		/** 取消绑定到此着色器
		*/
		virtual void 			unbind() = 0;

		/** 从文件中添加着色器
		@param ShaderType 着色器类型
		@param fn 文件名
		@return 着色器接口指针
		*/
		virtual bool			addShaderFromFile(ShaderType st,const std::string& fn) = 0;	

		/** 从内存数据中添加着色器
		@param ShaderType 着色器类型
		@param pBuffer 数据地址
		@param len 数据长度
		@return 着色器接口指针
		@see ShaderType
		*/
		virtual bool		addShaderFromMemory(ShaderType st,const uchar *pBuffer,uint len) = 0;

		/** 连结这个着色器程序
		@return 是否成功
		*/
		virtual bool			link() = 0;		
	};

	struct ILowLevelShaderProgram: public IShaderProgram
	{
		/** 设置统一浮点四维向量
		@param startRegister	起始的浮点常量寄存器序号
		@param pData			浮点数据指针
		@param countVector4		四维浮点向量的个数
		*/
		virtual	bool		setUniformFloatVector4(uint startRegister, float * pData, uint countVector4)=0;

		/** 设置统一整型四维向量
		@param startRegister	起始的整型常量寄存器序号
		@param pData			整型数据指针
		@param countVector4		四维整形向量的个数
		*/
		virtual	bool		setUniformIntVector4(uint startRegister, int * pData, uint countVector4)=0;

		/** 设置统一布尔变量
		@param startRegister	起始的布尔常量寄存器序号
		@param pData			布尔数据指针，0表示false，非0表示true
		@param countBool		布尔数据的个数
		*/
		virtual	bool		setUniformBool(uint startRegister, int * pData, uint countBool)=0;


		/** 将世界，摄像机，投影等矩阵绑定到shader的常量,适合低shader，只能同时绑定一个矩阵
		@param index	常量寄存器序号
		@param type		是TransformMatrixType，指示绑定不同的矩阵类型
		@param bindRow	指定绑定多少行
		*/
		virtual void			bindTransformMatrix(uint index, uint type, uint bindRow =4)=0;
	};

	struct IHighLevelShaderProgram: public IShaderProgram
	{
		/** 设置统一布尔型变量
		@param	strPara 变量名
		@param	value   变量值
		@return	是否设置成功
		*/
		virtual bool			setUniformBool(const std::string & strPara,  int value)=0;

		/** 设置统一整型变量
		@param	strPara 变量名
		@param	value	变量值
		@return	是否设置成功
		*/
		virtual bool			setUniformInt(const std::string & strPara, int value)=0;

		/** 设置统浮点型变量
		@param	strPara 变量名
		@param	value	变量值
		@return	是否设置成功
		*/
		virtual bool			setUniformFloat(const std::string & strPara, float value)=0;

		/** 设置统一向量变量
		@param	strPara 变量名
		@param	value	变量值
		@return	是否设置成功
		*/
		virtual bool			setUniformVector4(const std::string & strPara, const Vector4 & value)=0;

		/** 设置一般的矩阵
		@param	strPara 变量名
		@param	value	变量值，矩阵的数据，是行主序的
		@return			是否设置成功
		*/
		virtual bool			setUniformMatrix(const std::string & strPara, const Matrix4 &  value)=0;


		/** 设置统一布尔型数组
		@param	strPara 变量名
		@param	count	数组大小
		@param	pValue  数组地址
		@return	是否设置成功
		*/
		virtual bool			setUniformBoolArray(const std::string & strPara, uint count,  int * pValue)=0;

		/** 设置统一整型数组
		@param	strPara 变量名
		@param	count	数组大小
		@param	pValue  数组地址
		@return	是否设置成功
		*/
		virtual bool			setUniformIntArray(const std::string & strPara, uint count,  int * pValue)=0;

		/** 设置统一浮点型数组
		@param	strPara 变量名
		@param	count	数组大小
		@param	pValue  数组地址
		@return	是否设置成功
		*/
		virtual bool			setUniformFloatArray(const std::string & strPara, uint count,  float * pValue)=0;

		/** 设置统一向量型数组
		@param	strPara 变量名
		@param	count	数组大小
		@param	pValue  数组地址
		@return	是否设置成功
		*/
		virtual bool			setUniformVector4Array(const std::string & strPara, uint count,  xs::Vector4 * pValue)=0;

		/** 设置统一矩阵型数组
		@param	strPara 变量名
		@param	count	数组大小
		@param	pValue  数组地址
		@return	是否设置成功
		*/
		virtual bool			setUniformMatrixArray(const std::string & strPara, uint count,  xs::Matrix4 * pValue)=0;

		/** 将采样器绑定到shader的常量
		@param	samplerNum 绑定多少个采样器
		*/
		virtual void			bindSampler(uint smaplerNum) = 0;

		/** 将世界，摄像机，投影等矩阵绑定到shader的常量,适合高阶shader，能同时绑定几个矩阵
		@param flag 是TransformMatrixType的或，指示绑定不同的矩阵类型
		*/
		virtual void			bindTransformMatrix( uint flag) = 0;
	};

	/** 着色器管理器指针，这个接口暂时给内部使用，外部获取不到
	*/
	struct IShaderManager
	{
		/** 从文件中创建着色器，被IShaderProgram调用
		@param ShaderType 着色器类型
		@param fn 文件名
		@return 着色器接口指针
		*/
		virtual IShader*		createShaderFromFile(ShaderType st,const std::string& fn) = 0;	

		/** 从内存数据中创建着色器，被IShaderProgram调用
		@param ShaderType 着色器类型
		@param pBuffer 数据地址
		@param len 数据长度
		@return 着色器接口指针
		@see ShaderType
		*/
		virtual IShader*		createShaderFromMemory(ShaderType st,const uchar *pBuffer,uint len) = 0;	

		/** 销毁一个着色器，被IShaderProgram调用
		@param IShader* 着色器指针
		*/
		virtual void 			releaseShader(IShader*) = 0;				

		/** 销毁所有的着色器
		*/
		virtual void 			releaseAllShader() = 0;

		/** 释放自己
		*/
		virtual void			release() = 0;
	};


	/** 着色器程序管理器
	*/
	struct IShaderProgramManager
	{
		/** 创建着色器程序
		@return 着色器程序接口指针
		*/
		virtual IShaderProgram*		createShaderProgram(ShaderProgramType type) = 0;

		/**	 获取正在用于渲染的着色器程序
		@return 着色器程序接口指针
		*/
		virtual IShaderProgram*		getCurrentShaderProgam() = 0;

		/** 释放自己
		*/
		virtual void			release() = 0;
	};

	/** @} */
}

#endif