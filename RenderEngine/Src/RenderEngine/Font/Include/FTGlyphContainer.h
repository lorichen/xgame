#ifndef     __FTGlyphContainer__
#define     __FTGlyphContainer__

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "FT.h"
#include "FTBBox.h"
#include "FTPoint.h"
#include "FTVector.h"

namespace xs{


class FTFace;
class FTGlyph;
class FTCharmap;

/**
 * FTGlyphContainer holds the post processed FTGlyph objects.
 *
 * @see FTGlyph
 */
class FTGlyphContainer
{
        typedef FTVector<FTGlyph*> GlyphVector;
    public:
        /**
         * Constructor
         *
         * @param face      The Freetype face
         */
        FTGlyphContainer( FTFace* face);

        /**
         * Destructor
         */
        ~FTGlyphContainer();

        /**
         * Sets the character map for the face.
         *
         * @param encoding      the Freetype encoding symbol. See above.
         * @return              <code>true</code> if charmap was valid
         *                      and set correctly
         */
        bool CharMap( FT_Encoding encoding);

        /**
         * Get the font index of the input character.
         *
         * @param characterCode The character code of the requested glyph in the
         *                      current encoding eg apple roman.
         * @return      The font index for the character.
         */
        uint FontIndex( const uint characterCode ) const;
        
        /**
         * Adds a glyph to this glyph list.
         *
         * @param glyph         The FTGlyph to be inserted into the container
         * @param characterCode The char code of the glyph NOT the glyph index.
         */
        void  Add( FTGlyph* glyph, const uint characterCode);

        /**
         * Get a glyph from the glyph list
         *
         * @param characterCode The char code of the glyph NOT the glyph index      
         * @return              An FTGlyph or <code>null</code> is it hasn't been
         * loaded.
         */
        const FTGlyph* const Glyph( const uint characterCode) const;

        /**
         * Get the bounding box for a character.
         * @param characterCode The char code of the glyph NOT the glyph index      
         */
        FTBBox BBox( const uint characterCode) const;
        
        /**
        * Returns the kerned advance width for a glyph.
        *
        * @param characterCode     glyph index of the character
        * @param nextCharacterCode the next glyph in a string
        * @return                  advance width
        */
        float Advance( const uint characterCode, const uint nextCharacterCode);
        
        /**
         * Renders a character
         * @param characterCode      the glyph to be Rendered
         * @param nextCharacterCode  the next glyph in the string. Used for kerning.
         * @param penPosition        the position to Render the glyph
         * @return                   The distance to advance the pen position after Rendering
         */
        FTPoint Render( const uint characterCode, const uint nextCharacterCode, FTPoint penPosition, FontRenderDirection direction, float fontGap);
        
        /**
         * Queries the Font for errors.
         *
         * @return  The current error code.
         */
        FT_Error Error_() const { return err;}

    private:
        /**
         * The FT face
         */
        FTFace* face;

        /**
         * The Character Map object associated with the current face
         */
        FTCharmap* charMap;

        /**
         * A structure to hold the glyphs
         */
        GlyphVector glyphs;

        /**
         * Current error code. Zero means no error.
         */
        FT_Error err;
};


}

#endif  //  __FTGlyphContainer__
