#ifndef __ShaderFragmentD3D9_H__
#define __ShaderFragmentD3D9_H__

#include "RenderEngine/Manager.h"
#include "D3D9ResourceManager.h"

namespace xs
{
	class RenderSystemD3D9;

	class ShaderFragmentD3D9: public ManagedItem, public IShader, public D3D9Resource
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

		//D3D9Resource接口
		public:
		// 设备丢失，在这里释放Default Pool管理的资源
		virtual void notifyOnDeviceLost() {}

		//设备恢复, 在这里重建Default Pool 管理的资源
		virtual void notifyOnDeviceReset() {}

	public:

		/** 设置统一浮点四维向量
		@param startRegister	起始的浮点常量寄存器序号
		@param pData			浮点数据指针
		@param countVector4		四维浮点向量的个数
		*/
		bool		setUniformFloatVector4(uint startRegister, float * pData, uint countVector4);

		/** 设置统一整型四维向量
		@param startRegister	起始的整型常量寄存器序号
		@param pData			整型数据指针
		@param countVector4		四维整形向量的个数
		*/
		bool		setUniformIntVector4(uint startRegister, int * pData, uint countVector4);

		/** 设置统一布尔变量
		@param startRegister	起始的布尔常量寄存器序号
		@param pData			布尔数据指针，0表示false，非0表示true
		@param countBool		布尔数据的个数
		*/
		bool		setUniformBool(uint startRegister, int * pData, uint countBool);

		/** 将世界，摄像机，投影等矩阵绑定到shader的常量,适合低shader，只能同时绑定一个矩阵
		*/
		void				bindTransformMatrix(uint index, uint type, uint bindRow = 4);

	public:
		ShaderFragmentD3D9(const std::string& name);
		~ShaderFragmentD3D9();
		bool create(RenderSystemD3D9 * pRenderSystem, ShaderType st,const uchar *pBuffer,uint len);
		void release();

	private:
		ShaderType	m_type;
		RenderSystemD3D9 * m_pRenderSystem;
		void	*	m_pShader;//m_pShader 会根据shader类型转化成顶点shader或者像素shader指针
	};
}

#endif