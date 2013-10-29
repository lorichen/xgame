#include "StdAfx.h"
#include "Font.h"
#include "GDITextureFont.h"
#include "PixTextureFont.h"
#include "FontManager.h"

namespace xs
{
	void Font::release()
	{
		if( m_pFontManager )
		{
			m_pFontManager->unregisterFont(this);
		}

		safeDelete(m_pFontData);

		safeRelease(m_pFTFont);

		delete this;
	}

	void  Font::render2d(float xx,float yy,float zz,const ColorValue& color,const char* pText)
	{
		Matrix4 mtxWorldSave = m_pRenderSystem->getWorldMatrix();
		Matrix4 mtxWorld;
		mtxWorld.makeTrans(xx,yy,zz);
		m_pRenderSystem->setWorldMatrix(mtxWorldSave * mtxWorld);
		ColorValue colorSave = m_pRenderSystem->getColor();
		m_pRenderSystem->setColor(color);
		m_pFTFont->Render(pText);
		m_pRenderSystem->setColor(colorSave);
		m_pRenderSystem->setWorldMatrix(mtxWorldSave);
		m_pRenderSystem->setTexture(0,0);
	}

	void  Font::render2d(float xx,float yy,float zz,const ColorValue& color,const wchar_t* pText)
	{
		PP_BY_NAME("Font::render2d");
		Matrix4 mtxWorldSave = m_pRenderSystem->getWorldMatrix();
		Matrix4 mtxWorld;
		mtxWorld.makeTrans(xx,yy,zz);
		m_pRenderSystem->setWorldMatrix(mtxWorldSave * mtxWorld);
		ColorValue colorSave = m_pRenderSystem->getColor();
		m_pRenderSystem->setColor(color);
		m_pFTFont->Render(pText);
		m_pRenderSystem->setColor(colorSave);
		m_pRenderSystem->setWorldMatrix(mtxWorldSave);
		m_pRenderSystem->setTexture(0,0);
	}

	void  Font::render2dByScale(float xx,float yy,float zz,const ColorValue& color,const wchar_t* pText,float fScale)
	{
		PP_BY_NAME("Font::render2d");
		Matrix4 mtxWorldSave = m_pRenderSystem->getWorldMatrix();
		Matrix4 mtxWorld;
		mtxWorld.makeTrans(xx,yy,zz);
		m_pRenderSystem->setWorldMatrix(mtxWorldSave * mtxWorld);
		ColorValue colorSave = m_pRenderSystem->getColor();
		m_pRenderSystem->setColor(color);
		m_pFTFont->RenderByScale(pText,fScale);
		m_pRenderSystem->setColor(colorSave);
		m_pRenderSystem->setWorldMatrix(mtxWorldSave);
		m_pRenderSystem->setTexture(0,0);
	}
	FontRenderDirection Font::setRenderDirection( FontRenderDirection rd)
	{
		return m_pFTFont->setRenderDirection(rd);
	}

	float Font::setFontGap( float gap)
	{
		return m_pFTFont->setFontGap(gap);
	}

	void  Font::render(const char* pText)
	{
		m_pFTFont->Render(pText);
	}

	void  Font::render(const wchar_t* pText)
	{
		m_pFTFont->Render(pText);
	}

	const char*	Font::getFontName()
	{
		return m_name.c_str();
	}

	uint	Font::getFontSize()
	{
		return m_ui32Size;
	}

	bool Font::create(FontManager* pFontMgr, 
		IRenderSystem* pRenderSystem,
		const std::string & name, 
		const std::string & szFontPath,
		uint size,
		FontType fontType)
	{
		if( 0 == pFontMgr)
			return false;

		if(0 == pRenderSystem)
			return false;

		if( name.empty() || szFontPath.empty())
			return false;


		//初始化成员
		m_pRenderSystem = pRenderSystem;
		m_ui32Size = size;
		m_type = fontType;
		m_name = name;
		m_strFontName = szFontPath;
		m_pFontManager = pFontMgr;

		//创建字体
		xs::autostring wszFontPath(szFontPath.c_str());
		switch(fontType)
		{
		case FontType_GDI:
			{
				/*
				GDITextureFont* pGDIFont = new GDITextureFont();
				if( !pGDIFont->create(pRenderSystem, wszFontPath.c_wstr(), size) )
				{
					safeRelease(pGDIFont);
					return false;
				}
				else
				{
					m_pFTFont = pGDIFont;
				}
				*/
				return 0;
			}
			break;
		case FontType_PIX:
			{
				PixTextureFont* pPixFont = new PixTextureFont();
				if( !pPixFont->create(pRenderSystem, wszFontPath.c_wstr(), size) )
				{
					safeRelease(pPixFont);
					return false;
				}
				else
				{
					m_pFTFont = pPixFont;
				}
			}
			break;
		default:
			return false;
		}
		m_pFontManager->registerFont(this);
		
		return true;
	}

//	bool	Font::create(IRenderSystem *pRenderSystem,ITextureManager *pTextureManager,const char* szFontName,uint size,FontType fonttype)
//	{
//		m_pRenderSystem = pRenderSystem;
//		m_strFontName = szFontName;
//		m_ui32Size = size;
//		xs::autostring wString(szFontName);
//		if(fonttype==FontType_GDI)
//			m_pFTFont = ::new GDITextureFont(pRenderSystem,pTextureManager, wString.c_wstr(), size);
//		else
//			m_pFTFont = ::new PixTextureFont(pRenderSystem,pTextureManager, wString.c_wstr(), size);
//		
//
////crr remove. xs::getFileSystem()->open支持标准文件系统，mpk打包文件系统, 不需要这样的判断
////不需要这样的判断
//// 		if(isFile(szFontName))
//// 		{
//// 			m_pFTFont = ::new FTTextureFont(pRenderSystem,pTextureManager,szFontName);
//// 		}
//// 		else
//		/*{
//			xs::CStreamHelper pStream = xs::getFileSystem()->open(szFontName);
//			if(!pStream)
//			{
//				return false;
//			}
//
//			uint ui32Length = pStream->getLength();
//			m_pFontData = new uchar[ui32Length];
//			if(!m_pFontData)return false;
//
//			pStream->read(m_pFontData,ui32Length);
//
//			//m_pFTFont = ::new FTTextureFont(pRenderSystem,pTextureManager,m_pFontData,ui32Length);
//			m_pFTFont = ::new GDITextureFont(pRenderSystem,pTextureManager);
//		}
//
//		if(m_pFTFont->Error_())
//		{
//			Error("Failed to Open Font "<<szFontName);
//			delete m_pFTFont;
//			return false;
//		}
//		m_pFTFont->FaceSize(size);
//		m_pFTFont->CharMap(ft_encoding_unicode);*/
//
//		return true;
//	}

	/**
	* Get the global ascender height for the face.
	*
	* @return  Ascender height
	*/
	float Font::getAscender() const
	{
		return m_pFTFont->Ascender();
	}

	/**
	* Gets the global descender height for the face.
	*
	* @return  Descender height
	*/
	float Font::getDescender() const
	{
		return m_pFTFont->Descender();
	}

	/**
	* Gets the line spacing for the font.
	*
	* @return  Line height
	*/
	float Font::getLineHeight() const
	{
		float fHeight =   m_pFTFont->LineHeight();
		return fHeight;
	}

	/**
	* Get the bounding box for a string.
	*
	* @param string    a char string
	* @param llx       lower left near x coord
	* @param lly       lower left near y coord
	* @param llz       lower left near z coord
	* @param urx       upper right far x coord
	* @param ury       upper right far y coord
	* @param urz       upper right far z coord
	*/
	void  Font::getAABB( const char* string, AABB& aabb)
	{
		float lx,ly,lz,ux,uy,uz;
		m_pFTFont->BBox(string,lx,ly,lz,ux,uy,uz);
		aabb.setExtents(lx,ly,lz,ux,uy,uz);
	}

	/**
	* Get the bounding box for a string.
	*
	* @param string    a wchar_t string
	* @param llx       lower left near x coord
	* @param lly       lower left near y coord
	* @param llz       lower left near z coord
	* @param urx       upper right far x coord
	* @param ury       upper right far y coord
	* @param urz       upper right far z coord
	*/
	void  Font::getAABB( const wchar_t* string, AABB &aabb)
	{
		float lx,ly,lz,ux,uy,uz;
		m_pFTFont->BBox(string,lx,ly,lz,ux,uy,uz);
		aabb.setExtents(lx,ly,lz,ux,uy,uz);
	}

	/**
	* Get the advance width for a string.
	*
	* @param string    a wchar_t string
	* @return      advance width
	*/
	float Font::getAdvance( const wchar_t* string)
	{
		return m_pFTFont->Advance(string);
	}

	/**
	* Get the advance width for a string.
	*
	* @param string    a char string
	* @return      advance width
	*/
	float Font::getAdvance( const char* string)
	{
		return m_pFTFont->Advance(string);
	}

	float Font::getAdvance( const wchar_t c)
	{
		wchar_t string[2];
		string[0] = c;
		string[1] = 0;
		return getAdvance(string);
	}

	float Font::getAdvance( const char c)
	{
		char string[2];
		string[0] = c;
		string[1] = 0;
		return getAdvance(string);
	}

	//add by yhc
	//文字效果
	//颜色渐变
	void Font::GradientText(ColorValue& color1,ColorValue& color2) 
	{
		m_pFTFont->GradientText(Gdiplus::Color(color1.val[3]*255,color1.val[0],color1.val[1],color1.val[2]),Gdiplus::Color(color2.val[3]*255,color2.val[0],color2.val[1],color2.val[2]));
	}
	//单描边
	void Font::TextOutline(ColorValue& clrOutline, int nThickness)
	{
		m_pFTFont->TextOutline(Gdiplus::Color(clrOutline.val[3]*255,clrOutline.val[0],clrOutline.val[1],clrOutline.val[2]),nThickness);
	}
	//双描边
	void Font::TextDblOutline(ColorValue& clrOutline1, ColorValue& clrOutline2, int nThickness1, int nThickness2)
	{
		m_pFTFont->TextDblOutline(Gdiplus::Color(clrOutline1.val[3]*255,clrOutline1.val[0],clrOutline1.val[1],clrOutline1.val[2]),Gdiplus::Color(clrOutline2.val[3]*255,clrOutline2.val[0],clrOutline2.val[1],clrOutline2.val[2]),nThickness1,nThickness2);
	}
	//发光
	void Font::TextGlow(ColorValue& clrOutline, int nThickness)
	{
		m_pFTFont->TextGlow(Gdiplus::Color(clrOutline.val[3]*255,clrOutline.val[0],clrOutline.val[1],clrOutline.val[2]),nThickness);
	}
	//阴影
	void Font::Shadow(ColorValue& color, int nThickness,int nOffsetX,int nOffsetY)
	{
		m_pFTFont->Shadow(Gdiplus::Color(color.val[3]*255,color.val[0],color.val[1],color.val[2]),nThickness,Gdiplus::Point(nOffsetX,nOffsetY));
	}
	//发散的阴影
	void Font::DiffusedShadow(ColorValue& color, int nThickness,int nOffsetX,int nOffsetY)
	{
		m_pFTFont->DiffusedShadow(Gdiplus::Color(color.val[3]*255,color.val[0],color.val[1],color.val[2]),nThickness,Gdiplus::Point(nOffsetX,nOffsetY));
	}
	//突出字,阴影在后
	void Font::Extrude(ColorValue& color, int nThickness,int nOffsetX,int nOffsetY)
	{
		m_pFTFont->Extrude(Gdiplus::Color(color.val[3]*255,color.val[0],color.val[1],color.val[2]),nThickness,Gdiplus::Point(nOffsetX,nOffsetY));
	}

	//预初始化一些常用字
	void Font::InitText(wchar_t *pStr)
	{
		m_pFTFont->InitText(pStr);
	}
}