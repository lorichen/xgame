#include "StdAfx.h"
#include "ShaderProgramManagerOGL.h"
#include "ShaderProgram.h"
#include "ShaderManager.h"
#include "HighLevelShaderManager.h"
#include "HighLevelShaderProgram.h"

namespace xs
{

	IShaderProgram* ShaderProgramManagerOGL::createShaderProgram(ShaderProgramType type)
	{
		switch( type)
		{
		case SPT_LOWLEVEL:
			{
				return new ShaderProgram(this,ShaderManager::Instance() );
			}
			break;
		case SPT_HIGHLEVEL:
			{
				HighLevelShaderProgram *pProgram = new HighLevelShaderProgram(this,HighLevelShaderManager::Instance());
				if( pProgram )
				{
					if( !pProgram->create() )
					{
						delete pProgram;
						return 0;
					}
				}
				return pProgram;	
			}
			break;
		default:
			return 0;
		}
	}

	IShaderProgram*	 ShaderProgramManagerOGL::getCurrentShaderProgam()
	{
		return m_pCurrentShaderProgram;
	}


	void ShaderProgramManagerOGL::release()
	{
		ShaderManager::Instance()->release();
		HighLevelShaderManager::Instance()->release();
		m_pCurrentShaderProgram = 0;

	}

	void ShaderProgramManagerOGL::onShaderProgramBinded(IShaderProgram * pProgram)
	{
		m_pCurrentShaderProgram = pProgram;
	}
	
	void	ShaderProgramManagerOGL::onShaderProgramUnbinded(IShaderProgram * pProgram)
	{
		if( m_pCurrentShaderProgram == pProgram )
			m_pCurrentShaderProgram = 0;
	}

	ShaderManager* ShaderProgramManagerOGL::getLowLevelShaderManager()
	{
		return ShaderManager::Instance();
	}

	HighLevelShaderManager*	ShaderProgramManagerOGL::getHighLevelShaderManager()
	{
		return HighLevelShaderManager::Instance();
	}

}