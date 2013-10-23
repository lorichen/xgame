


#include <cassert>
#include <string> // For memset

#include "FTTextureFont.h"
#include "FTTextureGlyph.h"





namespace xs{

#define TEXTURE_SIZE	256

FTTextureFont::FTTextureFont(IRenderSystem *pRenderSystem,ITextureManager *pTextureManager, const char* fontFilePath)
:   FTFont( fontFilePath),m_pRenderSystem(pRenderSystem),m_pTextureManager(pTextureManager),
    glyphHeight(0),
    glyphWidth(0),
    padding(3),
    xOffset(0),
    yOffset(0),
	fontHeight(0)
{
    remGlyphs = numGlyphs = face.GlyphCount();
}


FTTextureFont::FTTextureFont(IRenderSystem *pRenderSystem,ITextureManager *pTextureManager, const uchar*pBufferBytes, size_t bufferSizeInBytes)
:   FTFont( pBufferBytes, bufferSizeInBytes),m_pRenderSystem(pRenderSystem),m_pTextureManager(pTextureManager),
    glyphHeight(0),
    glyphWidth(0),
    padding(3),
    xOffset(0),
    yOffset(0)
{
    remGlyphs = numGlyphs = face.GlyphCount();
}


FTTextureFont::~FTTextureFont()
{
    STLReleaseSequence(vTextures);
}


FTGlyph* FTTextureFont::MakeGlyph( uint glyphIndex)
{
    FT_GlyphSlot ftGlyph = face.Glyph( glyphIndex, FT_LOAD_FORCE_AUTOHINT |(false ? FT_LOAD_TARGET_NORMAL : FT_LOAD_TARGET_MONO));
    
    if( ftGlyph)
    {
        glyphHeight = static_cast<int>( charSize.Height());
        glyphWidth = static_cast<int>( charSize.Width());
        
        if( vTextures.empty())
        {
            vTextures.push_back( CreateTexture());
            xOffset = yOffset = padding;
        }
        
        if( xOffset > ( TEXTURE_SIZE - glyphWidth))
        {
            xOffset = padding;
            yOffset += glyphHeight;
            
            if( yOffset > ( TEXTURE_SIZE - glyphHeight))
            {
                vTextures.push_back( CreateTexture());
                yOffset = padding;
            }
        }

        FTTextureGlyph* tempGlyph = new FTTextureGlyph(fontHeight,m_pRenderSystem, ftGlyph, vTextures[vTextures.size() - 1],
                                                        xOffset, yOffset, TEXTURE_SIZE, TEXTURE_SIZE);
        xOffset += static_cast<int>( tempGlyph->BBox().upperX - tempGlyph->BBox().lowerX + padding);
        
        --remGlyphs;
        return tempGlyph;
    }
    
    err = face.Error_();
    return NULL;
}

ITexture* FTTextureFont::CreateTexture()
{
	/*
	glBindTexture( GL_TEXTURE_2D, textID);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, textureWidth, textureHeight, 0, GL_ALPHA, GL_UNSIGNED_BYTE, textureMemory);
	 */
	
	//return m_pTextureManager->createEmptyTexture(TEXTURE_SIZE,TEXTURE_SIZE,PF_R8G8B8A8,FO_LINEAR,FO_LINEAR,FO_NONE);

	//by yhc, 现在用GDI+来绘制字体象素格式改了
	return m_pTextureManager->createEmptyTextureNoWait(TEXTURE_SIZE,TEXTURE_SIZE,PF_A8R8G8B8,FO_LINEAR,FO_LINEAR,FO_NONE);
	
}


bool FTTextureFont::FaceSize( const uint size, const uint res)
{
    if( !vTextures.empty())
    {
		STLReleaseSequence(vTextures);
        remGlyphs = numGlyphs = face.GlyphCount();
    }

    if(FTFont::FaceSize( size, res))
	{
		fontHeight = LineHeight() + Descender();
		return true;
	}

	return false;
}


void  FTTextureFont::Render( const char* string)
{
	SceneBlendFactor src,dst;
	m_pRenderSystem->getSceneBlending(src,dst);
	m_pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);
	FTTextureGlyph::ResetActiveTexture();
	FTFont::Render(string);
	m_pRenderSystem->setSceneBlending(src,dst);
	m_pRenderSystem->setTexture(0,0);
/*
    glPushAttrib( GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // GL_ONE

    FTTextureGlyph::ResetActiveTexture();
    
    FTFont::Render(m_pRenderSystem, string);

    glPopAttrib();
*/
}


void  FTTextureFont::Render( const wchar_t* string)
{
	SceneBlendFactor src,dst;
	m_pRenderSystem->getSceneBlending(src,dst);
	m_pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);
	FTTextureGlyph::ResetActiveTexture();
	FTFont::Render(string);
	m_pRenderSystem->setSceneBlending(src,dst);
/*
    glPushAttrib( GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    
	glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // GL_ONE
    
    FTTextureGlyph::ResetActiveTexture();
    
    FTFont::Render(m_pRenderSystem, string);
    
	glPopAttrib();
*/
}

}

