#ifndef     __FTFace__
#define     __FTFace__

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "FT.h"
#include "FTPoint.h"
#include "FTSize.h"

namespace xs{


/**
 * FTFace class provides an abstraction layer for the Freetype Face.
 *
 * @see "Freetype 2 Documentation"
 *
 */
class FTFace
{
    public:
        /**
         * Opens and reads a face file. Error_ is set.
         *
         * @param fontFilePath  font file path.
         */
        FTFace( const char* fontFilePath);

        /**
         * Read face data from an in-memory buffer. Error_ is set.
         *
         * @param pBufferBytes  the in-memory buffer
         * @param bufferSizeInBytes  the length of the buffer in bytes
         */
        FTFace( const uchar*pBufferBytes, size_t bufferSizeInBytes );

        /**
         * Destructor
         *
         * Disposes of the current Freetype Face.
         */
        virtual ~FTFace();

        /**
         * Attach auxilliary file to font (e.g., font metrics).
         *
         * @param fontFilePath  auxilliary font file path.
         * @return          <code>true</code> if file has opened
         *                  successfully.
         */
        bool Attach( const char* fontFilePath);

        /**
         * Attach auxilliary data to font (e.g., font metrics) from memory
         *
         * @param pBufferBytes  the in-memory buffer
         * @param bufferSizeInBytes  the length of the buffer in bytes
         * @return          <code>true</code> if file has opened
         *                  successfully.
         */
        bool Attach( const uchar*pBufferBytes, size_t bufferSizeInBytes);

        /**
         * Get the freetype face object..
         *
         * @return pointer to an FT_Face.
         */
        FT_Face* Face() const { return ftFace;}
        
        /**
         * Sets the char size for the current face.
         *
         * This doesn't guarantee that the size was set correctly. Clients
         * should check errors.
         *
         * @param size      the face size in points (1/72 inch)
         * @param res       the resolution of the target device.
         * @return          <code>FTSize</code> object
         */
        const FTSize& Size( const uint size, const uint res);

        /**
         * Get the number of character maps in this face.
         *
         * @return character map count.
         */
        uint CharMapCount();

        /**
         * Get a list of character maps in this face.
         *
         * @return pointer to the first encoding.
         */
        FT_Encoding* CharMapList();
        
        /**
         * Gets the kerning vector between two glyphs
         */
        FTPoint KernAdvance( uint index1, uint index2);

        /**
         * Loads and creates a Freetype glyph.
         */
        FT_GlyphSlot Glyph( uint index, FT_Int load_flags);

        /**
         * Gets the number of glyphs in the current face.
         */
        uint GlyphCount() const { return numGlyphs;}

        /**
         * Queries for errors.
         *
         * @return  The current error code.
         */
        FT_Error Error_() const { return err; }
        
    private:
        /**
         * The Freetype face
         */
        FT_Face* ftFace;

        /**
         * The size object associated with this face
         */
        FTSize  charSize;
        
        /**
         * The number of glyphs in this face
         */
        int numGlyphs;
        
        FT_Encoding* fontEncodingList;

        /**
         * This face has kerning tables
         */
         bool hasKerningTable;

        /**
         * Current error code. Zero means no error.
         */
        FT_Error err;
};


}

#endif  //  __FTFace__
