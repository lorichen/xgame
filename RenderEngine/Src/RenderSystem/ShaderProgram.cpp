#include "StdAfx.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "ShaderManager.h"
#include "ShaderProgramManagerOGL.h"

namespace xs
{
	void ShaderProgram::release()
	{	
		ShaderContainerIterator it = m_vShaders.begin();
		for( ; it != m_vShaders.end(); ++it)
			m_pShaderManager->releaseShader( (*it) );

		delete this;
	}

	bool ShaderProgram::bind()
	{
		std::vector<IShader *>::iterator b = m_vShaders.begin(),e = m_vShaders.end();
		while(b != e)
		{
			IShader *pShader = (*b);
			pShader->bind();
			++b;
		}
		m_pShaderProgramManager->onShaderProgramBinded(this);
		return true;
	}

	void ShaderProgram::unbind()
	{
		std::vector<IShader *>::iterator b = m_vShaders.begin(),e = m_vShaders.end();
		while(b != e)
		{
			IShader *pShader = (*b);
			pShader->unbind();
			++b;
		}
		m_pShaderProgramManager->onShaderProgramUnbinded(this);
	}

	bool ShaderProgram::addShaderFromFile(ShaderType st,const std::string& fn)
	{
		IShader * pShader = m_pShaderManager->createShaderFromFile( st, fn);
		if( 0 == pShader ) return false;
		m_vShaders.push_back(pShader);
		return true;
	}

	bool ShaderProgram::addShaderFromMemory(ShaderType st,const uchar *pBuffer,uint len)
	{
		IShader * pShader = m_pShaderManager->createShaderFromMemory( st, pBuffer, len);
		if( 0 == pShader ) return false;
		m_vShaders.push_back(pShader);
		return true;	
	}

	bool ShaderProgram::link()
	{
		return true;
	}

	/** 获取着色器程序类型
	*/
	ShaderProgramType ShaderProgram::getProgramType()
	{
		return SPT_LOWLEVEL;
	}

	/** 设置统一浮点四维向量
	@param startRegister	起始的浮点常量寄存器序号
	@param pData			浮点数据指针
	@param countVector4		四维浮点向量的个数
	*/
	bool ShaderProgram::setUniformFloatVector4(uint startRegister, float * pData, uint countVector4)
	{
		ShaderContainerIterator it = m_vShaders.begin();
		for( ; it != m_vShaders.end(); ++it)
		{
			if( static_cast<Shader*>( *it)->setProgramLocalFloatVector4( startRegister, pData, countVector4) )
				return true;
		}
		return false;
	}

	/** 设置统一整型四维向量
	@param startRegister	起始的整型常量寄存器序号
	@param pData			整型数据指针
	@param countVector4		四维整形向量的个数
	*/
	bool ShaderProgram::setUniformIntVector4(uint startRegister, int * pData, uint countVector4)
	{
		//不支持整型的
		return false;	
	}

	/** 设置统一布尔变量
	@param startRegister	起始的布尔常量寄存器序号
	@param pData			布尔数据指针，0表示false，非0表示true
	@param countBool		布尔数据的个数
	*/
	bool ShaderProgram::setUniformBool(uint startRegister, int * pData, uint countBool)
	{
		//不支布尔型的
		return false;
	}

	/** 将世界，摄像机，投影等矩阵绑定到shader的常量,适合低shader，只能同时绑定一个矩阵
	*/
	void ShaderProgram::bindTransformMatrix(uint index, uint type, uint bindRow)
	{
		//内部已经绑定了
	}
}