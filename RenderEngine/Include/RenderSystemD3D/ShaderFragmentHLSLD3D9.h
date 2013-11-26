#ifndef __ShaderFragmentHLSLD3D9_H__
#define __ShaderFragmentHLSLD3D9_H__

#include "RenderEngine/Manager.h"
#include "D3D9ResourceManager.h"

namespace xs
{
	class RenderSystemD3D9;

	class ShaderFragmentHLSLD3D9: public ManagedItem, public IShader, public D3D9Resource
	{
		// IShader接口
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
		
		//<<D3D9Resource接口>>
	public:
		// 设备丢失，在这里释放Default Pool管理的资源
		virtual void notifyOnDeviceLost() {}

		//设备恢复, 在这里重建Default Pool 管理的资源
		virtual void notifyOnDeviceReset() {}

	public:

		/** 设置统一布尔型变量
		@param	strPara 变量名
		@param	value   变量值
		@return	是否设置成功
		*/
		bool			setUniformBool(const std::string & strPara,  int value);

		/** 设置统一整型变量
		@param	strPara 变量名
		@param	value	变量值
		@return	是否设置成功
		*/
		bool			setUniformInt(const std::string & strPara, int value);

		/** 设置统浮点型变量
		@param	strPara 变量名
		@param	value	变量值
		@return	是否设置成功
		*/
		bool			setUniformFloat(const std::string & strPara, float value);

		/** 设置统一向量变量
		@param	strPara 变量名
		@param	value	变量值
		@return	是否设置成功
		*/
		bool			setUniformVector4(const std::string & strPara, const Vector4 & value);

		/** 设置一般的矩阵
		@param	strPara 变量名
		@param	value	变量值，矩阵的数据，是行主序的
		@return			是否设置成功
		*/
		bool			setUniformMatrix(const std::string & strPara, const Matrix4 &  value);


		/** 设置统一布尔型数组
		@param	strPara 变量名
		@param	count	数组大小
		@param	pValue  数组地址
		@return	是否设置成功
		*/
		bool			setUniformBoolArray(const std::string & strPara, uint count,  int * pValue);

		/** 设置统一整型数组
		@param	strPara 变量名
		@param	count	数组大小
		@param	pValue  数组地址
		@return	是否设置成功
		*/
		bool			setUniformIntArray(const std::string & strPara, uint count,  int * pValue);

		/** 设置统一浮点型数组
		@param	strPara 变量名
		@param	count	数组大小
		@param	pValue  数组地址
		@return	是否设置成功
		*/
		bool			setUniformFloatArray(const std::string & strPara, uint count,  float * pValue);

		/** 设置统一向量型数组
		@param	strPara 变量名
		@param	count	数组大小
		@param	pValue  数组地址
		@return	是否设置成功
		*/
		bool			setUniformVector4Array(const std::string & strPara, uint count,  xs::Vector4 * pValue);

		/** 设置统一矩阵型数组
		@param	strPara 变量名
		@param	count	数组大小
		@param	pValue  数组地址
		@return	是否设置成功
		*/
		bool			setUniformMatrixArray(const std::string & strPara, uint count,  xs::Matrix4 * pValue);


		/** 将世界，摄像机，投影矩阵绑定到shader的常量
		*/
		void			bindTransformMatrix( uint flag );


	public:
		ShaderFragmentHLSLD3D9(const std::string& name);
		~ShaderFragmentHLSLD3D9();
		bool create(RenderSystemD3D9 * pRenderSystem, ShaderType st,const uchar *pBuffer,uint len);
		void release();

	private:
		ShaderType	m_type;
		RenderSystemD3D9 * m_pRenderSystem;
		void	*	m_pShader;//m_pShader 会根据shader类型转化成顶点shader或者像素shader指针
		ID3DXConstantTable * m_pConstantTable; 
	};
}

#endif