#include "TxGuiReTexture.h"
#include "stringhelper\StringConv.h"
namespace xs
{


TxGuiTexture::TxGuiTexture(xsgui::Renderer* pRenderer,IRenderSystem *pRenderSystem) 
	: m_pRenderSystem(pRenderSystem),
	Texture(pRenderer)
{
	//m_pTexture = m_pRenderSystem->getTextureManager()->createEmptyTexture(FO_LINEAR,FO_LINEAR,FO_NONE,TAM_CLAMP,TAM_CLAMP);
	//by yhc ui图片模糊是线性插值导致的
	m_pTexture = m_pRenderSystem->getTextureManager()->createEmptyTexture(FO_POINT,FO_POINT,FO_NONE,TAM_CLAMP,TAM_CLAMP);
}

TxGuiTexture::TxGuiTexture(xsgui::Renderer* pRenderer,IRenderSystem *pRenderSystem,uint size,xsgui::Texture::PixelFormat pf, bool createNoWait)
	: m_pRenderSystem(pRenderSystem),
	Texture(pRenderer)
{
	if( createNoWait)
	{
		switch( pf )
		{
		case xsgui::Texture::PF_RGBA:
			m_pTexture = m_pRenderSystem->getTextureManager()->createEmptyTextureNoWait(size,size,PF_R8G8B8A8,FO_POINT,FO_POINT);
			return;
		case xsgui::Texture::PF_BGR:
			m_pTexture = m_pRenderSystem->getTextureManager()->createEmptyTextureNoWait(size,size,PF_B8G8R8,FO_POINT,FO_POINT);
			return;
		case xsgui::Texture::PF_ARGB:
			//兼容gdi bitmap的格式,避免循环每个象素进行转换
			m_pTexture = m_pRenderSystem->getTextureManager()->createEmptyTextureNoWait(size,size,PF_A8R8G8B8,FO_POINT,FO_POINT);
			return;
		default:// never get here
			m_pTexture = 0;
			return;
		}
	}
	else
	{
		switch( pf )
		{
		case xsgui::Texture::PF_RGBA:
			m_pTexture = m_pRenderSystem->getTextureManager()->createEmptyTexture(size,size,PF_R8G8B8A8,FO_POINT,FO_POINT);
			return;
		case xsgui::Texture::PF_BGR:
			m_pTexture = m_pRenderSystem->getTextureManager()->createEmptyTexture(size,size,PF_B8G8R8,FO_POINT,FO_POINT);
			return;
		case xsgui::Texture::PF_ARGB:
			//兼容gdi bitmap的格式,避免循环每个象素进行转换
			m_pTexture = m_pRenderSystem->getTextureManager()->createEmptyTexture(size,size,PF_A8R8G8B8,FO_POINT,FO_POINT);
			return;
		default:// never get here
			m_pTexture = 0;
			return;
		}
	}
}

TxGuiTexture::~TxGuiTexture()
{
	m_pRenderSystem->getTextureManager()->releaseTexture(m_pTexture);
}

/// 获得纹理的当前像素宽度
ushort TxGuiTexture::getWidth() const
{
	return m_pTexture->width();
}


/// 获得纹理的当前像素高度
ushort TxGuiTexture::getHeight() const
{
	return m_pTexture->height();
}

void	TxGuiTexture::destroyTexture()
{
	m_pRenderSystem->getTextureManager()->releaseTexture(m_pTexture);
	delete this;
}


/** 从文件加载图象到纹理对象
@param filename 文件名
*/
bool TxGuiTexture::loadFromFile(const xsgui::String& filename, const xsgui::String& resourceGroup)
{
	
	char *p = xsgui::StringConv::Utf8ToChar((char*)filename.c_str());
	bool ret = m_pTexture->loadFromFile(p);
	xsgui::StringConv::DeletePtr(p);

	return ret;
}


/** 从内存加载图象到纹理对象
@param buffPtr 图象数据指针
@param width 图象的宽度
@param height 图象的高度
*/
bool TxGuiTexture::loadFromMemory(const void* buffPtr, uint width, uint height, xsgui::Texture::PixelFormat pixelFormat)
{
	bool ret = true;
	if(pixelFormat == xsgui::Texture::PF_RGBA)
		ret = m_pTexture->loadFromRawData((uchar*)buffPtr,width,height,PF_R8G8B8A8);
	else
		ret = m_pTexture->loadFromRawData((uchar*)buffPtr,width,height,PF_B8G8R8);

	return ret;
}
/** 设置贴图区域数据,像素格式需要用户自己保证符合
@param level 要设置数据的图像Mipmaps等级,从0开始
@param left 左边开始下标
@param top 上边开始下标
@param width 宽度
@param height 高度
@param pData 设置的贴图数据
@return true表示创建成功，false表示失败
*/
bool TxGuiTexture::setSubData(uint level,uint left,uint top,uint width,uint height,void  *pData)
{
	return m_pTexture->setSubData(level,left,top,width,height,pData);
}
}