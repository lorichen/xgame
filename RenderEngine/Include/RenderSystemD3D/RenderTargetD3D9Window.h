#ifndef __RenderTargetD3D9Window_H__
#define __RenderTargetD3D9Window_H__
#include "RenderTargetD3D9.h"

namespace xs
{

	class RenderTargetManagerD3D9;
	class RenderSystemD3D9;
	class RenderTargetD3D9Window: public RenderTargetD3D9
	{
		//接口
	public:
		virtual void 		release();
		virtual const ITexture*	getTexture();
		virtual bool		getTextureData(void * pData);
		virtual void 		onAttach(RenderTargetD3D9 * pPrevious);
		virtual void 		onDetach();
		virtual void		onBeginFrame(bool clearColorBuffer,bool clearDepthBuffer,bool clearStencilBuffer);
		virtual void 		onEndFrame(int layer);
		virtual	void		clearFrameBuffer(bool clearColorBuffer,bool clearDepthBuffer ,bool clearStencilBuffer);
		virtual IDirect3DSurface9 * getDepthStencilBuffer();
		virtual bool		isDefaultRenderTarget();
		virtual void onDeviceLost();
		virtual void onDeviceReset();
		virtual RECT getFrameBufferRect();
		virtual bool switchDisplayMode(RenderEngineCreationParameters* param);
		virtual bool getDisplayMode(RenderEngineCreationParameters & createParam);


		//构造和析构函数
	private:
		friend class RenderTargetManagerD3D9;
		bool createAsDefaultRenderTarget();
		bool create(HWND hwnd);//根据窗口创建渲染目标,处于窗口模式
		RenderTargetD3D9Window(RenderSystemD3D9 * pRenderSystem, bool b2d, RenderTargetManagerD3D9 * pRenderTargetMgr);
		~RenderTargetD3D9Window();	
	public:
		bool					m_bIsDefault;//是否关联的渲染目标是设备的属性
		IDirect3DSwapChain9		* m_pSwapChain;//交换链
		D3DPRESENT_PARAMETERS	m_PresentParam;//交换链参数
		IDirect3DSurface9	*	m_pDepthStencilBuffer;//关联的深度模版缓存
		HWND					m_hWnd;//管理的窗口句柄	
	};

}
#endif