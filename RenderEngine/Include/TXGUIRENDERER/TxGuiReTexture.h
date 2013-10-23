#ifndef __TxGuiReTexture_H__
#define __TxGuiReTexture_H__

#include "TxGuiReRenderer.h"
namespace xs
{


class TxGuiRendererAPI TxGuiTexture : public xsgui::Texture
{
public:
	TxGuiTexture(xsgui::Renderer* pRenderer,IRenderSystem *pRenderSystem);
	TxGuiTexture(xsgui::Renderer* pRenderer,IRenderSystem *pRenderSystem,uint size,xsgui::Texture::PixelFormat pf, bool createNoWait);
	virtual ~TxGuiTexture();
public:
	/// 获得纹理的当前像素宽度
	virtual	ushort getWidth() const;


	/// 获得纹理的当前像素高度
	virtual	ushort getHeight() const;


	/** 从文件加载图象到纹理对象
	@param filename 文件名
	*/
	virtual bool loadFromFile(const xsgui::String& filename, const xsgui::String& resourceGroup);


	/** 从内存加载图象到纹理对象
	@param buffPtr 图象数据指针
	@param width 图象的宽度
	@param height 图象的高度
	*/
	virtual bool loadFromMemory(const void* buffPtr, uint width, uint height,xsgui::Texture::PixelFormat pixelFormat);

	/** 设置贴图区域数据,像素格式需要用户自己保证符合
	@param level 要设置数据的图像Mipmaps等级,从0开始
	@param left 左边开始下标
	@param top 上边开始下标
	@param width 宽度
	@param height 高度
	@param pData 设置的贴图数据
	@return true表示创建成功，false表示失败
	*/
	virtual bool			setSubData(uint level,uint left,uint top,uint width,uint height,void  *pData);
public:
	void	destroyTexture();

	ITexture*		m_pTexture;
	IRenderSystem*	m_pRenderSystem;
};

}
#endif