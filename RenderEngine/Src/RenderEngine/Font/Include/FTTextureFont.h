#ifndef     __FTTextureFont__
#define     __FTTextureFont__

#include "FTFont.h"
#include "FTVector.h"
#include "FT.h"

namespace xs{

class FTTextureGlyph;


/**
 * FTTextureFont is a specialisation of the FTFont class for handling
 * Texture mapped fonts
 *
 * @see     FTFont
 */
class FTTextureFont : public FTFont
{
    public:
        /**
         * Open and read a font file. Sets Error_ flag.
         *
         * @param fontFilePath  font file path.
         */
        FTTextureFont(IRenderSystem *pRenderSystem,ITextureManager *pTextureManager, const char* fontFilePath);
        
        /**
         * Open and read a font from a buffer in memory. Sets Error_ flag.
         *
         * @param pBufferBytes  the in-memory buffer
         * @param bufferSizeInBytes  the length of the buffer in bytes
         */
        FTTextureFont(IRenderSystem *pRenderSystem,ITextureManager *pTextureManager, const uchar*pBufferBytes, size_t bufferSizeInBytes);
        
        /**
         * Destructor
         */
        virtual ~FTTextureFont();

        /**
            * Set the char size for the current face.
         *
         * @param size      the face size in points (1/72 inch)
         * @param res       the resolution of the target device.
         * @return          <code>true</code> if size was set correctly
         */
        virtual bool FaceSize( const uint size, const uint res = 72);

        /**
         * Renders a string of characters
         * 
         * @param string    'C' style string to be output.   
         */
        virtual void  Render( const char* string);
        
        /**
         * Renders a string of characters
         * 
         * @param string    wchar_t string to be output.     
         */
        virtual void  Render( const wchar_t* string);

        
    private:
        /**
         * Construct a FTTextureGlyph.
         *
         * @param glyphIndex The glyph index NOT the char code.
         * @return  An FTTextureGlyph or <code>null</code> on failure.
         */
        inline virtual FTGlyph* MakeGlyph( uint glyphIndex);
                
        /**
         * Get the size of a block of memory required to layout the glyphs
         *
         * Calculates a width and height based on the glyph sizes and the
         * number of glyphs. It over estimates.
         */
        inline void  CalculateTextureSize();

        /**
         * Creates a 'blank' OpenGL texture object.
         *
         * The format is GL_ALPHA and the params are
         * GL_TEXTURE_WRAP_S = GL_CLAMP
         * GL_TEXTURE_WRAP_T = GL_CLAMP
         * GL_TEXTURE_MAG_FILTER = GL_LINEAR
         * GL_TEXTURE_MIN_FILTER = GL_LINEAR
         * Note that mipmapping is NOT used
         */
        inline ITexture* CreateTexture();

        /**
         *An array of texture ids
         */
         FTVector<ITexture*> vTextures;
        
        /**
         * The max height for glyphs in the current font
         */
        int glyphHeight;

        /**
         * The max width for glyphs in the current font
         */
        int glyphWidth;

        /**
         * A value to be added to the height and width to ensure that
         * glyphs don't overlap in the texture
         */
        uint padding;
        
        /**
         *
         */
        uint numGlyphs;
        
        /**
         */
        uint remGlyphs;

        /**
         */
        int xOffset;

        /**
         */
        int yOffset;

		/**
		*/
		int fontHeight;
private:
	IRenderSystem*		m_pRenderSystem;
	ITextureManager*	m_pTextureManager;
};

}
#endif // __FTTextureFont__


