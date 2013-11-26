#ifndef	__HardwareCursorManagerD3D9_H__
#define __HardwareCursorManagerD3D9_H__

#include "IHardwareCursorManager.h"

namespace xs
{

	class HardwareCursorManagerD3D9;
	class RenderSystemD3D9;

	class HardwareCursorManagerD3D9Creater
	{
	public:
		static HardwareCursorManagerD3D9 * create(RenderSystemD3D9 * pRenderSystem);
	};

	class	HardwareCursorManagerD3D9: public IHardwareCursorManager
	{
		//<<interface>>
	public:

		/************************************************************************/
		/* 设置是否显示鼠标
		@param show 是否显示鼠标
		*/
		/************************************************************************/
		virtual void	showCursor( bool show);


		/************************************************************************/
		/*   设置光标的位置  
		@param	x 屏幕横坐标
		@param	y 屏幕纵坐标
		@param	updateImmediate 是否立即重绘光标
		*/
		/************************************************************************/
		virtual	void	setCursorPosition( int x, int y, bool updateImmediate);	

		/************************************************************************/
		/* 设置鼠标热点                                                         */
		/************************************************************************/
		virtual void	setCursorHotspot(uint x, uint y);

		/************************************************************************/
		/* 设置鼠标图片
		@param pTexture 鼠标图片，要释放pTexure，先取消图片设置
		*/
		/************************************************************************/
		virtual void	setCursorImage(ITexture* pTexture);

		/************************************************************************/
		/*设置是否可用                                                         */
		/************************************************************************/
		virtual bool	isAvailable();

		
		//处理设备丢失
	public:
		void notifyOnDeviceLost();
		void notifyOnDeviceReset();

	private:
		void setCursorProperties();
		void setDefaultCursor();

	public:
		HardwareCursorManagerD3D9();
		~HardwareCursorManagerD3D9(){};
		bool	create(RenderSystemD3D9 * pRenderSystem);
		void	release();

	private:
		RenderSystemD3D9 * m_pRenderSystem;
		IDirect3DDevice9 * m_pD3D9device;

		ITexture*		m_pCurTexture;//当前光标
		uint			m_uiXHotspot;//x方向热点
		uint			m_uiYHotspot;//y方向热点
		ITexture*		m_pDefaultTexture;//默认光标
	};
}

#endif