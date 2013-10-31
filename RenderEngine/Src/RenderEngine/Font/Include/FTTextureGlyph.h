#ifndef     __FTTextureGlyph__
#define     __FTTextureGlyph__


#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "FT.h"
#include "FTGlyph.h"

namespace xs{

/**
 * FTTextureGlyph is a specialisation of FTGlyph for creating texture
 * glyphs.
 * 
 * @see FTGlyphContainer
 *
 */
class FTTextureGlyph : public FTGlyph
{
    public:
        /**
         * Constructor
         *
         * @param glyph     The Freetype glyph to be processed
         * @param id        The id of the texture that this glyph will be
         *                  drawn in
         * @param xOffset   The x offset into the parent texture to draw
         *                  this glyph
         * @param yOffset   The y offset into the parent texture to draw
         *                  this glyph
         * @param width     The width of the parent texture
         * @param height    The height (number of rows) of the parent texture
         */
        FTTextureGlyph(int lineHeight,IRenderSystem *pRenderSystem, FT_GlyphSlot glyph, ITexture* pTexture, int xOffset, int yOffset, int width, int height);

        /**
         * Destructor
         */
        virtual ~FTTextureGlyph();

        /**
         * Renders this glyph at the current pen position.
         *
         * @param pen   The current pen position.
         * @return      The advance distance for this glyph.
         */
        virtual const FTPoint& Render(const FTPoint& pen);
        
        /**
         * Reset the currently active texture to zero to get into a known state before
         * drawing a string. This is to get round possible threading issues.
         */
        static void  ResetActiveTexture(){ activeTexture = 0;}
        
    private:
        /**
         * The width of the glyph 'image'
         */
        int destWidth;

        /**
         * The height of the glyph 'image'
         */
        int destHeight;

        /**
         * Vector from the pen position to the topleft corner of the pixmap
         */
        FTPoint pos;
        
        /**
         * The texture co-ords of this glyph within the texture.
         */
        FTPoint uv[2];
        
        /**
         * The texture index that this glyph is contained in.
         */
        ITexture* texture;

        /**
         * The texture index of the currently active texture
         *
         * We keep track of the currently active texture to try to reduce the number
         * of texture bind operations.
         */
        static ITexture* activeTexture;

		IRenderSystem *m_pRenderSystem;

		int fontHeight;
};


}

#endif  //  __FTTextureGlyph__
