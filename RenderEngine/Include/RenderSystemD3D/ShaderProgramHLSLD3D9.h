#ifndef __ShaderProgramHLSLD3D9_H__
#define __ShaderProgramHLSLD3D9_H__

namespace xs
{
	class ShaderFragmentHLSLD3D9;
	class ShaderProgramManagerD3D9;
	class ShaderManagerHLSLD3D9;

	class ShaderProgramHLSLD3D9 : public IHighLevelShaderProgram
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

		/** 设置统一布尔型变量
		@param	strPara 变量名
		@param	value   变量值
		@return	是否设置成功
		*/
		virtual bool			setUniformBool(const std::string & strPara,  int value);

		/** 设置统一整型变量
		@param	strPara 变量名
		@param	value	变量值
		@return	是否设置成功
		*/
		virtual bool			setUniformInt(const std::string & strPara, int value);

		/** 设置统浮点型变量
		@param	strPara 变量名
		@param	value	变量值
		@return	是否设置成功
		*/
		virtual bool			setUniformFloat(const std::string & strPara, float value);

		/** 设置统一向量变量
		@param	strPara 变量名
		@param	value	变量值
		@return	是否设置成功
		*/
		virtual bool			setUniformVector4(const std::string & strPara, const Vector4 & value);

		/** 设置一般的矩阵
		@param	strPara 变量名
		@param	value	变量值，矩阵的数据，是行主序的
		@return			是否设置成功
		*/
		virtual bool			setUniformMatrix(const std::string & strPara, const Matrix4 &  value,bool normal = true);


		/** 设置统一布尔型数组
		@param	strPara 变量名
		@param	count	数组大小
		@param	pValue  数组地址
		@return	是否设置成功
		*/
		virtual bool			setUniformBoolArray(const std::string & strPara, uint count,  int * pValue);

		/** 设置统一整型数组
		@param	strPara 变量名
		@param	count	数组大小
		@param	pValue  数组地址
		@return	是否设置成功
		*/
		virtual bool			setUniformIntArray(const std::string & strPara, uint count,  int * pValue);

		/** 设置统一浮点型数组
		@param	strPara 变量名
		@param	count	数组大小
		@param	pValue  数组地址
		@return	是否设置成功
		*/
		virtual bool			setUniformFloatArray(const std::string & strPara, uint count,  float * pValue);

		/** 设置统一向量型数组
		@param	strPara 变量名
		@param	count	数组大小
		@param	pValue  数组地址
		@return	是否设置成功
		*/
		virtual bool			setUniformVector4Array(const std::string & strPara, uint count,  xs::Vector4 * pValue);

		/** 设置统一矩阵型数组
		@param	strPara 变量名
		@param	count	数组大小
		@param	pValue  数组地址
		@return	是否设置成功
		*/
		virtual bool			setUniformMatrixArray(const std::string & strPara, uint count,  xs::Matrix4 * pValue);


		////////////////////////////////////////////////////////////////////////////////////

		///** 设置一般的矩阵
		//@param	strPara 变量名
		//@param	row 矩阵的行数
		//@param	column 矩阵的列数
		//@param	pData 矩阵的数据，是行主序的
		//*/
		//virtual bool			setUniformMatrix(const std::string strPara,  const Matrix4 &  mtx);

		///** 设置统一整型变量数值
		//@param strPara	变量名称
		//@param count	整型变量的个数
		//@param pData	变量的值
		//*/
		//virtual bool 			setUniformIntArray(const std::string& strPara, uint count, int * pData );

		///** 设置统一布尔变量数值
		//@param strPara	变量名称
		//@param count	布尔变量的个数
		//@param pData	变量的值
		//*/
		//virtual bool 			setUniformBoolArray(const std::string& strPara,uint count, bool * pData );

		///** 设置统一浮点变量数值
		//@param strPara 变量名称
		//@param count	浮点变量的个数
		//@param pData 变量的值
		//*/
		//virtual bool 			setUniformFloatArray(const std::string& strPara,uint count, float * pData );

		///** 设置统一矩阵变量
		//@param strPara	变量名称
		//@param count	矩阵变量的个数
		//@param pData	变量的值
		//*/
		//virtual bool 			setUniformMatrixArray(const std::string& strPara,uint count, xs::Matrix4 * pData );


		/** 将采样器绑定到shader的常量
		*/
		virtual void			bindSampler( uint smaplerNum );

		/** 将世界，摄像机，投影矩阵绑定到shader的常量
		*/
		virtual void			bindTransformMatrix( uint flag );


	public:
		ShaderProgramHLSLD3D9(ShaderProgramManagerD3D9 * pProgramMgr, ShaderManagerHLSLD3D9 * pShaderMgr);
	private:
		~ShaderProgramHLSLD3D9() {};

	private:
		ShaderFragmentHLSLD3D9 *	m_pVertexShader;
		ShaderFragmentHLSLD3D9 *	m_pPixelShader;
		ShaderManagerHLSLD3D9 *		m_pShaderManager;
		ShaderProgramManagerD3D9*	m_pShaderProgramManager;
	};
}

#endif