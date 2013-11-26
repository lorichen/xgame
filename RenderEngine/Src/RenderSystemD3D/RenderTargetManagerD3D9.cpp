#include "StdAfx.h"
#include "RenderTargetManagerD3D9.h"
#include "RenderTargetD3D9.h"
#include "RenderTargetD3D9Texture.h"
#include "RenderTargetD3D9Window.h"
#include "D3D9Device.h"
#include "RenderSystemD3D9.h"

namespace xs
{

	RenderTargetManagerD3D9 * RenderTargetManagerD3D9Creater::create(RenderSystemD3D9 * pRenderSystem)
	{
		//检测
		if( NULL == pRenderSystem )
			return 0;

		//创建渲染目标管理器
		RenderTargetManagerD3D9 * pMgr = new RenderTargetManagerD3D9();
		if( NULL == pMgr)
			return 0;

		if( !pMgr->create( pRenderSystem) )
		{
			pMgr->release();
			return 0;
		}
		else
		{
			return pMgr;
		}
	}

	RenderTargetManagerD3D9::RenderTargetManagerD3D9():
	m_pRenderSystem(0),
	m_currentRenderTargetID(0),
	m_pCurrentRenderTarget(0),
	m_uiRTTID(1)
	{
		m_vRenderTargets.clear();		
	}


	RenderTargetManagerD3D9::~RenderTargetManagerD3D9()
	{
		m_vRenderTargets.clear();
	}

	bool RenderTargetManagerD3D9::create(RenderSystemD3D9 * pRenderSystem)
	{
		if( NULL == pRenderSystem)
			return false;
		m_pRenderSystem = pRenderSystem;

		//添加默认渲染目标
		RenderTargetD3D9Window * pRTW = new RenderTargetD3D9Window(m_pRenderSystem, false,this);
		if( NULL == pRTW)
		{
			delete pRTW;
			return false;
		}
		if( !pRTW->createAsDefaultRenderTarget() )
		{
			pRTW->release();
			return false;
		}
		m_vRenderTargets[(uint)pRTW->m_hWnd] = pRTW;
		m_currentRenderTargetID = (uint)pRTW->m_hWnd;
		m_pCurrentRenderTarget = pRTW;


		return true;
	}

	/**释放渲染目标管理器
	*/
	void RenderTargetManagerD3D9::release()
	{	
		//取消当前渲染目标
		setCurrentRenderTarget(0);

		//需要在里面释放所有的渲染目标
		RenderTargetContainerIterator it = m_vRenderTargets.begin();
		for( ; it != m_vRenderTargets.end(); ++it)
		{
			(*it).second->release();
		}		

		delete this;
	}

	/** 设备丢失，或者设备重置前调用此函数释放资源
	*/
	void RenderTargetManagerD3D9::onDeviceLost()
	{
		RenderTargetContainerIterator it = m_vRenderTargets.begin();
		for(  ; it!= m_vRenderTargets.end(); ++it)
		{
			it->second->onDeviceLost();
		}
	}


	/** 设备重置后调用此函数释放资源
	*/
	void RenderTargetManagerD3D9::onDeviceReset()
	{
		RenderTargetContainerIterator it = m_vRenderTargets.begin();
		for(  ; it!= m_vRenderTargets.end(); ++it)
		{
			it->second->onDeviceReset();
		}

		//重新设置渲染目标
		if( m_pCurrentRenderTarget )
		{
			m_pCurrentRenderTarget->onAttach(m_pCurrentRenderTarget);
			m_pCurrentRenderTarget->resetRenderState();
		}
	}

	/**添加渲染到窗口的渲染目标
	@param hwnd 渲染窗口句柄
	*/
	bool RenderTargetManagerD3D9::addRenderTarget(uint hwnd)
	{
		if( m_vRenderTargets.find(hwnd) != m_vRenderTargets.end() )
			return false;//已经存在一个这样的渲染目标了

		//需要再这里添加新的交换链
		RenderTargetD3D9Window * pRT = new RenderTargetD3D9Window(m_pRenderSystem, false, this);
		if(NULL == pRT)
			return false;

		if( !pRT->create( (HWND)hwnd) )
		{
			pRT->release();
			return false;
		}

		m_vRenderTargets[hwnd] = pRT;
		return true;
	}

	/**设置当前的渲染目标
	@param hwnd 窗口句柄或者RTT句柄
	@return 是否成功
	*/
	bool RenderTargetManagerD3D9::setCurrentRenderTarget(uint hwnd)
	{
		if( hwnd == m_currentRenderTargetID) return true;

		if( 0 == hwnd)
		{
			if( m_pCurrentRenderTarget)
				m_pCurrentRenderTarget->onDetach();
			m_pCurrentRenderTarget = 0;
			m_currentRenderTargetID  = 0;
			return true;
		}

		RenderTargetContainerIterator it = m_vRenderTargets.find( hwnd);
		if( it == m_vRenderTargets.end() ) return false;

		RenderTargetD3D9 * pOldRenderTarget = m_pCurrentRenderTarget;
		if( m_pCurrentRenderTarget)
			m_pCurrentRenderTarget->onDetach();

		m_currentRenderTargetID = hwnd;
		m_pCurrentRenderTarget = it->second;
		it->second->onAttach(pOldRenderTarget);
		return true;
	}

	/**获得当前的渲染目标id
	@return 当前的渲染目标的窗口句柄，或者是RTT的句柄
	*/
	uint RenderTargetManagerD3D9::getCurrentRenderTargetID() 
	{
		return m_currentRenderTargetID;
	}

	/**添加渲染到纹理的渲染目标
	@param width 宽度
	@param height 高度
	@param alpha 渲染目标是否有alpha通道
	@param min 最小滤波器
	@param mag 最大滤波器
	@param mip mipmap滤波方式
	@param s 纹理s寻址方式
	@param t 纹理t寻址方式
	@return 生成的RTT的句柄
	*/
	uint RenderTargetManagerD3D9::addRTT(
		int width,
		int height, 
		bool alpha,
		FilterOptions min , 
		FilterOptions mag , 
		FilterOptions mip ,
		TextureAddressingMode s,											
		TextureAddressingMode t)
	{
		RenderTargetD3D9Texture * pRTT = new RenderTargetD3D9Texture(m_pRenderSystem, false, this);
		if( NULL == pRTT)
			return 0;
		if( !pRTT->create(width, height, alpha, min, mag, mip, s, t) )
		{
			pRTT->release();
			return 0;
		}

		//查找不冲突的rtt id
		while( m_vRenderTargets.find( m_uiRTTID) != m_vRenderTargets.end() )
		{
			++m_uiRTTID;
		}

		m_vRenderTargets[m_uiRTTID] = pRTT;
		return m_uiRTTID++;	
	}

	/**添加Overlay RenderTarget，只能添加一个
	*/
	bool RenderTargetManagerD3D9::addOverlayRenderTarget()
	{
		//暂时不支持
		return false;
	}

	/**删除Overlay RenderTarget
	*/
	void RenderTargetManagerD3D9::removeOverlayRenderTarget()
	{
		//暂时不支持
		return;
	}

	bool RenderTargetManagerD3D9::setOverlayRenderTarget()
	{
		//暂时不支持
		return false;
	}

	/**删除渲染目标
	@param hwnd 窗口句柄
	@return 是否成功
	*/
	bool RenderTargetManagerD3D9::removeRenderTarget(uint hwnd)
	{
		RenderTargetContainerIterator it = m_vRenderTargets.find( hwnd);
		if( it == m_vRenderTargets.end() )
			return false;

		//如果是默认渲染目标，即设备的一个属性，是不能被删除的
		if( it->second->isDefaultRenderTarget() )
			return false;

		//如果删除的渲染目标是当前渲染目标
		if( hwnd == m_currentRenderTargetID)
		{
			m_pCurrentRenderTarget->onDetach();
			m_pCurrentRenderTarget = 0;
			m_currentRenderTargetID = 0;
		}
		
		it->second->release();
		m_vRenderTargets.erase(it);
		return true;
	}

	/**多线程添加RenderTarget,多线程添加渲染目标只是用于异步加载
	@param hwnd 窗口句柄
	@return 是否成功
	*/
	uint RenderTargetManagerD3D9::MTaddRenderTarget()
	{
		//引擎已经做好了，异步加载资源的工作。这里什么也不做
		return 1;
	}

	/**多线程删除渲染目标,多线程添加渲染目标只是用于异步加载
	@param hwnd 窗口句柄
	@return 是否成功
	*/
	bool RenderTargetManagerD3D9::MTremoveRenderTarget(uint key)
	{
		//引擎已经做好了，异步加载资源的工作。这里什么也不做
		//nothing to do
		return true;
	}

			/** 获取渲染目标
		@return 渲染目标
		*/
	RenderTargetD3D9 * RenderTargetManagerD3D9::getRenderTarget(uint id)
	{
		RenderTargetContainerIterator it = m_vRenderTargets.find(id);
		if( it == m_vRenderTargets.end() )
			return 0;
		else
			return it->second;
	}
}
