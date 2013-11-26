#include "StdAfx.h"
#include "ShaderProgramManagerD3D9.h"
#include "RenderSystemD3D9.h"
#include "ShaderManagerD3D9.h"
#include "ShaderManagerHLSLD3D9.h"
#include "ShaderProgramD3D9.h"
#include "ShaderProgramHLSLD3D9.h"

namespace xs
{
	ShaderProgramManagerD3D9* ShaderProgramManagerD3D9Creater::create(RenderSystemD3D9* pRenderSystem)
	{
		if( 0 == pRenderSystem) 
			return 0;

		ShaderProgramManagerD3D9* pMgr = new ShaderProgramManagerD3D9();
		if( !pMgr->create(pRenderSystem) )
		{
			safeRelease(pMgr);
			return 0;
		}
		else
		{
			return pMgr;
		}
	}


	ShaderProgramManagerD3D9::ShaderProgramManagerD3D9():m_pLowLevelShaderMgr(0),
		m_pHighLevelShaderMgr(0),
		m_pCurrentShaderProgram(0)
	{
	}
	
	bool ShaderProgramManagerD3D9::create(RenderSystemD3D9 * pRenderSystem)
	{
		if( 0 == pRenderSystem) 
			return false;

		m_pLowLevelShaderMgr = ShaderManagerD3D9Creater::create(pRenderSystem);
		if( 0 == m_pLowLevelShaderMgr)
			return false;

		m_pHighLevelShaderMgr = ShaderManagerHLSLD3D9Creater::create(pRenderSystem);
		if( 0 == m_pHighLevelShaderMgr)
			return false;

		return true;
	}

	void ShaderProgramManagerD3D9::release()
	{
		safeRelease(m_pLowLevelShaderMgr);
		safeRelease(m_pHighLevelShaderMgr);
		delete this;
	}

	ShaderProgramManagerD3D9::~ShaderProgramManagerD3D9()
	{
	}


	void ShaderProgramManagerD3D9::onShaderProgramBinded(IShaderProgram * pProgram)
	{
		m_pCurrentShaderProgram = pProgram;
	}

	void ShaderProgramManagerD3D9::onShaderProgramUnbinded(IShaderProgram * pProgram)
	{
		if( m_pCurrentShaderProgram == pProgram)
			m_pCurrentShaderProgram = 0; 
	}

	IShaderManager* ShaderProgramManagerD3D9::getLowLevelShaderManager()
	{
		return m_pLowLevelShaderMgr;	
	}

	IShaderManager* ShaderProgramManagerD3D9::getHighLevelShaderManager()
	{
		return m_pHighLevelShaderMgr;		
	}


	IShaderProgram*	 ShaderProgramManagerD3D9::createShaderProgram(ShaderProgramType type)
	{
		switch( type)
		{
		case SPT_LOWLEVEL:
			{
				return new ShaderProgramD3D9(this, m_pLowLevelShaderMgr);
			}
			break;
		case SPT_HIGHLEVEL:
			{
				return new ShaderProgramHLSLD3D9(this, m_pHighLevelShaderMgr);
			}
			break;
		default:
			return 0;
		}
	}

	IShaderProgram* ShaderProgramManagerD3D9::getCurrentShaderProgam()
	{
		return m_pCurrentShaderProgram;
	}

}