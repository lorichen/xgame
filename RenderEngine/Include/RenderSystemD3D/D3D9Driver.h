#ifndef __D3D9Driver_H__
#define __D3D9Driver_H__


namespace xs
{
	class D3D9Driver;
	class RenderSystemD3D9;

	struct D3D9DriverCreater
	{
		static D3D9Driver * create(RenderSystemD3D9 * pRenderSystem, uint uiAdapterOrdinal);//创建d3d9 驱动程序对象
	};


	class D3D9Driver
	{
	public:
		inline const D3DADAPTER_IDENTIFIER9 & getIdentifier() { return m_AdapterIdentifier; } ;
		const D3DDISPLAYMODE & getCurrentDisplayMode()  { return m_DesktopDisplayMode;};
		void  release();
		inline uint getAdapterOrdinal() { return m_uiAdapterOrdinal;} 
		bool  getDisplayMode(UINT width, UINT height, D3DFORMAT format, D3DDISPLAYMODE & mode);

	public://处理设备丢失
		void	onDeviceLost();
		void	onDeviceReset();
	private:
		void enumerateDisplayMode();

	private:
		friend struct D3D9DriverCreater;
		bool create(RenderSystemD3D9 * pRenderSystem,uint uiAdapterOrdinal);
		D3D9Driver();
		D3DADAPTER_IDENTIFIER9	m_AdapterIdentifier;// 显卡标识	
		D3DDISPLAYMODE			m_DesktopDisplayMode;//桌面显示模式	
		uint m_uiAdapterOrdinal;						//显卡序号
		RenderSystemD3D9 *		m_pRenderSystem;//渲染系统
		std::vector<D3DDISPLAYMODE> m_displayModeList;//显示模式列表
	};
}


#endif