#ifndef __D3D9Device_H__
#define __D3D9Device_H__

/*
	我们要希望支持多个渲染系统
*/

namespace xs
{
	class D3D9Device;
	class D3D9Driver;

	//d3d9 设备工场
	struct	D3D9DeviceCreater
	{
		/*
		根据渲染引擎创建参数创建设备,这个函数的调用一定要在驱动程序对象和渲染系统对象创建好后调用
		@param pRenderSystem 渲染系统指针
		@param uiAdapterOrdinal 显卡序号
		@param param 创建参数
		@param dwBehaviorFlags 设备行为标志
		@return 设备对象
		*/
		static D3D9Device * create( 
			RenderSystemD3D9 * pRenderSystem,
			uint uiAdapterOrdinal,
			const D3DDISPLAYMODE &  devMode,
			const RenderEngineCreationParameters &  param,
			DWORD dwBehaviorFlags);

		/*
		自定义创建设备,这个函数的调用一定要在驱动程序对象和渲染系统对象创建好后调用
		@param pRenderSystem 渲染系统指针
		@param uiAdapterOrdinal 显卡序号
		@param dwBehaviorFlags 设备创建标志
		@param pPresentParams 交换链参数
		@return 是否创建成功
		*/
		static D3D9Device * create( 
			RenderSystemD3D9 * pRenderSystem,
			uint uiAdapterOrdinal,
			HWND hFocusWindow,
			DWORD dwBehaviorFlags,
			D3DPRESENT_PARAMETERS * pPresentParams);
	};

	class D3D9Device
	{
	public:
		/**释放设备
		*/
		void release();

		/** 获取显示参数
		*/
		inline const D3DPRESENT_PARAMETERS & getPresentParams() { return m_PresentParams; };

		/**获取IDirect3DDevice9对象
		*/
		inline IDirect3DDevice9 * getDevice() {return m_pDevice;}

		/** 获取渲染能力
		*/
		inline RenderSystemCapabilities * getCapabilities() { return &m_capabilities; }

		/** 获取D3D9渲染能力
		*/	
		inline const D3DCAPS9 &  getD3D9Caps() { return m_caps; }

		/** 获取设备支持的纹理格式
		*/
		D3DFORMAT getClosetSupportedTextureFormat( PixelFormat pf);

		/**	设备重置
		*/
		bool	reset(bool fullscreen, uint width, uint height);
		bool	reset();
		bool	reset( bool bVerticalSync);
	
	private:
		/** 初始化渲染能力
		*/
		void initCapabilities();
		/** 初始化vs渲染能力
		*/
		void initVertexShaderCaps();
		/** 初始化ps渲染能力
		*/
		void initPixelShaderCaps();

		/** 初始化支持的纹理格式
		*/
		void initClosetSuppoertedTextureFormat();	

		/** 重置设备
		*/
		bool _reset( bool fullscreen, uint width, uint height, bool bVerticalSync);

	private:
		friend struct D3D9DeviceCreater;
		D3D9Device();
		~D3D9Device	();	

	private:
		DWORD m_dwBehaviorFlags;				//设备行为方式
		HWND		m_hFocusWindow;				//焦点窗口
		D3DPRESENT_PARAMETERS m_PresentParams;	//交换链参数
		IDirect3DDevice9 * m_pDevice;			//d3d9设备接口
		D3DCAPS9	m_caps;						//设备能力
		D3DDEVICE_CREATION_PARAMETERS m_createParams; //创建参数
		RenderSystemCapabilities m_capabilities;//设备渲染能力,和m_caps等价,输出给外部用
		RenderSystemD3D9 *		m_pRenderSystem;//渲染系统指针

	private: 
		//支持的格式
		typedef std::map<PixelFormat, D3DFORMAT> ClosetSupportedFormatContainer;
		typedef ClosetSupportedFormatContainer::iterator ClosetSupportedFormatContainerIterator;
		typedef std::map<PixelFormat, std::vector<D3DFORMAT> > PossibleSupportedFormatContainer;
		typedef PossibleSupportedFormatContainer::iterator PossibleSupportedFormatContainerIterator;
		ClosetSupportedFormatContainer m_supportedTextureFormat; //支持的最接近的纹理格式
	};

}

#endif