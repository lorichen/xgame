#ifndef __RenderTargetManagerD3D9_H__
#define __RenderTargetManagerD3D9_H__

/*
切换渲染目标时，要设定m_pcsBeginEndPrimitiveProxy的当前属性
*/

namespace xs
{

	class RenderTargetD3D9;
	class RenderSystemD3D9;
	class RenderTargetManagerD3D9;

	class RenderTargetManagerD3D9Creater
	{
	public:
		static RenderTargetManagerD3D9 * create(RenderSystemD3D9 * pRenderSystem);
	};

	class RenderTargetManagerD3D9
	{
	public:

		/**添加Overlay RenderTarget，只能添加一个
		*/
		bool addOverlayRenderTarget();

		/**删除Overlay RenderTarget
		*/
		void removeOverlayRenderTarget();

		/** 设置overlay render target 为当前渲染目标
		*/
		bool setOverlayRenderTarget();


		/**添加渲染到窗口的渲染目标
		@param hwnd 渲染窗口句柄
		*/
		bool addRenderTarget(uint hwnd);


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
		uint addRTT(
			int width,
			int height, 
			bool alpha = false,
			FilterOptions min = FO_LINEAR, 
			FilterOptions mag = FO_LINEAR, 
			FilterOptions mip = FO_NONE,
			TextureAddressingMode s = TAM_WRAP,											
			TextureAddressingMode t = TAM_WRAP);

		/**删除渲染目标
		@param hwnd 窗口句柄
		@return 是否成功
		*/
		bool removeRenderTarget(uint hwnd);


		/**多线程添加RenderTarget,多线程添加渲染目标只是用于异步加载
		@param hwnd 窗口句柄
		@return 是否成功
		*/
		uint MTaddRenderTarget();

		/**多线程删除渲染目标,多线程添加渲染目标只是用于异步加载
		@param hwnd 窗口句柄
		@return 是否成功
		*/
		bool MTremoveRenderTarget(uint key);

		/**设置当前的渲染目标
		@param hwnd 窗口句柄或者RTT句柄
		@return 是否成功
		*/
		bool setCurrentRenderTarget(uint hwnd);

		/**获得当前的渲染目标id
		@return 当前的渲染目标的窗口句柄，或者是RTT的句柄
		*/
		uint getCurrentRenderTargetID();
		
		/**获得当前的渲染目标
		@return 当前的渲染目标的窗口句柄，或者是RTT的句柄
		*/
		RenderTargetD3D9 * getCurrentRenderTarget() { return m_pCurrentRenderTarget; }

		/** 获取渲染目标
		@return 渲染目标
		*/
		RenderTargetD3D9 * getRenderTarget(uint id);

	public:
		/**释放渲染目标管理器
		*/
		void release();

	public:
		/** 设备丢失，或者设备重置前调用此函数释放资源
		*/
		void onDeviceLost();


		/** 设备重置后调用此函数释放资源
		*/
		void onDeviceReset();

	private:
		friend class RenderTargetManagerD3D9Creater;
		RenderTargetManagerD3D9();
		~RenderTargetManagerD3D9();
		bool create(RenderSystemD3D9 * pRenderSystem);

	private:
		RenderSystemD3D9 * m_pRenderSystem;//渲染系统
		typedef stdext::hash_map<uint, RenderTargetD3D9*> RenderTargetContainer;
		typedef RenderTargetContainer::iterator RenderTargetContainerIterator;
		RenderTargetContainer m_vRenderTargets;//渲染目标
		uint m_currentRenderTargetID;//当前渲染目标id
		RenderTargetD3D9 * m_pCurrentRenderTarget;//当前渲染目标
		uint			   m_uiRTTID;//RTT的id
	};
}


#endif