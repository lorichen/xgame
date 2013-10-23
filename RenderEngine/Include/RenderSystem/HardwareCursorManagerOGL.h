#ifndef __HardwareCursorManagerOGL_H__
#define __HardwareCursorManagerOGL_H__

#include "IHardwareCursorManager.h"

class HardwareCursorManagerOGL: public IHardwareCursorManager, public Singleton<HardwareCursorManagerOGL>
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
	
public:
	static HardwareCursorManagerOGL* instance()
	{
		return getInstancePtr();
	}

private:

};


#endif