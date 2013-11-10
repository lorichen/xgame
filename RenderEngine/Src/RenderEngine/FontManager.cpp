
//gdi ◊÷ÃÂ‰÷»ælib




#include "StdAfx.h"

#if (TARGET_PLATFORM == PLATFORM_WIN32)
#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;
ULONG_PTR g_gdiplusToken;
#endif

#include "FontManager.h"
#include "Font.h"


namespace xs
{
	//≥ı ºªØgui+
	void InitGuiplus()
	{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		GdiplusStartupInput gdiplusStartupInput;
		GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);
#endif
	}
    
    
    FontManager*	FontManager::Instance()
    {
        static FontManager fm;
        return &fm;
    }

	FontManager::FontManager()
	{
		InitGuiplus();
	}

	FontManager::~FontManager()
	{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		GdiplusShutdown(g_gdiplusToken);
#endif
	}

	IFont*	FontManager::createFont(IRenderSystem* pRenderSystem,ITextureManager* pTextureManager,const char* name,const char* szFontName,uint size,FontType fonttype)
	{
		IFont* pCurFont = this->getFontByName(name);
		if ( 0 != pCurFont)
		{
			return pCurFont;
		}

		Font* pFont = new Font();
		if( !pFont->create(this, pRenderSystem, name, szFontName, size, fonttype))
		{
			safeRelease( pFont);
			return 0;
		}
		else
		{
			return pFont;
		}

		return pFont;
	}
	

	void 	FontManager::releaseFont(IFont* pFont)
	{
		if( 0 == pFont )
			return;

		FontString::iterator it = m_fs.find(pFont);
		if( it == m_fs.end() )
		{
			return;
		}
		else
		{
			it->first->release();
			return;
		}
	}

	void 	FontManager::releaseFont(const char* name)
	{
		if( m_sf.find(name) != m_sf.end())
			return;

		std::string fontName(name);
		StringFont::iterator it = m_sf.find(fontName);
		if( it == m_sf.end() )
		{
			return;
		}
		else
		{
			it->second->release();
			return;
		}
	}

	void 	FontManager::releaseAll()
	{
		StringFont::iterator it = m_sf.begin();
		while( it != m_sf.end() )
		{
			it->second->release();
			it = m_sf.begin();
		}
	}
	
	IFont*	FontManager::getFontByName(const char* name)
	{
		StringFont::iterator it = m_sf.find(name);
		if(it != m_sf.end())
			return (*it).second;

		return 0;
	}

	bool FontManager::registerFont(IFont* pFont)
	{
		if( 0 == pFont)
			return false;

		std::string fontName( pFont->getFontName() );
		StringFont::iterator itsf = m_sf.find(fontName);
		if( itsf != m_sf.end())
		{
			Info("FontManager::registerFont - font has been registered!");
			return false;
		}

		FontString::iterator itfs = m_fs.find(pFont);
		if( itfs != m_fs.end())
		{
			Info("FontManager::registerFont - font has been registered!");
			return false;
		}

		m_fs[pFont] = fontName;
		m_sf[fontName] = pFont;
		return true;
	}

	void FontManager::unregisterFont(IFont* pFont)
	{
		if( 0 == pFont )
			return;

		FontString::iterator itfs = m_fs.find(pFont);
		if( itfs == m_fs.end())
			return;

		std::string fontName = pFont->getFontName();
		StringFont::iterator itsf = m_sf.find(fontName);
		if( itsf == m_sf.end() )
		{
			throw std::string("FontManager::registerFont - font and font name are not Coincided!");
		}

		m_fs.erase(itfs);
		m_sf.erase(itsf);
	}
	
	void FontManager::RenderAllWord()
	{
		PP_BY_NAME("FontManager::RenderAllWord");
		FontString::iterator e = m_fs.end();
		for(FontString::iterator it = m_fs.begin();it != e;++it)
		{
			static_cast<Font*>((*it).first)->m_pFTFont->RealRender();
		}
	}
}