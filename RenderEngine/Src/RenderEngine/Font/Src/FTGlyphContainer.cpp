#include    "FTGlyphContainer.h"
#include    "FTGlyph.h"
#include    "FTFace.h"
#include    "FTCharmap.h"

namespace xs{



FTGlyphContainer::FTGlyphContainer( FTFace* f)
:   face(f),
    err(0)
{
    glyphs.push_back( NULL);
    charMap = new FTCharmap( face);
}


FTGlyphContainer::~FTGlyphContainer()
{
    GlyphVector::iterator glyphIterator;
    for( glyphIterator = glyphs.begin(); glyphIterator != glyphs.end(); ++glyphIterator)
    {
        delete *glyphIterator;
    }
    
    glyphs.clear();
    delete charMap;
}


bool FTGlyphContainer::CharMap( FT_Encoding encoding)
{
    bool result = charMap->CharMap( encoding);
    err = charMap->Error_();
    return result;
}


uint FTGlyphContainer::FontIndex( const uint characterCode) const
{
    return charMap->FontIndex( characterCode);
}


void  FTGlyphContainer::Add( FTGlyph* tempGlyph, const uint characterCode)
{
    charMap->InsertIndex( characterCode, glyphs.size());
    glyphs.push_back( tempGlyph);
}


const FTGlyph* const FTGlyphContainer::Glyph( const uint characterCode) const
{
    int index = charMap->GlyphListIndex( characterCode);
    return glyphs[index];
}


FTBBox FTGlyphContainer::BBox( const uint characterCode) const
{
    return glyphs[charMap->GlyphListIndex( characterCode)]->BBox();
}


float FTGlyphContainer::Advance( const uint characterCode, const uint nextCharacterCode)
{
    uint left = charMap->FontIndex( characterCode);
    uint right = charMap->FontIndex( nextCharacterCode);

    float width = face->KernAdvance( left, right).X();
    width += glyphs[charMap->GlyphListIndex( characterCode)]->Advance().X();
    
    return width;
}


FTPoint FTGlyphContainer::Render( const uint characterCode, const uint nextCharacterCode, FTPoint penPosition, FontRenderDirection direction, float fontGap)
{
	FTPoint newPen(penPosition);
	switch( direction )
	{
	case FRD_RIGHT2LEFT:
		{
			uint left = charMap->FontIndex( characterCode);
			uint right = charMap->FontIndex( nextCharacterCode);
			FTPoint tmp(face->KernAdvance( left, right));
			newPen +=FTPoint( -tmp.X(), tmp.Y(), tmp.Z());
			if( !face->Error_())
			{
				FTPoint tmp2(glyphs[charMap->GlyphListIndex( characterCode)]->Render( penPosition));
				newPen +=FTPoint(-tmp2.X(), tmp2.Y(), tmp2.Z());
			}
			return newPen += FTPoint(-fontGap, 0.0, 0.0);			
		}
		break;
	case FRD_TOP2BOTTOM:
		{
			if( !face->Error_() )
			{
				glyphs[charMap->GlyphListIndex( characterCode)]->Render( penPosition);
				const xs::FTBBox & glyphbox = glyphs[charMap->GlyphListIndex( characterCode)]->BBox();
				newPen += FTPoint(0.0, glyphbox.upperY - glyphbox.lowerY + fontGap, 0.0);
			}
			return newPen;		
		}
		break;
	case FRD_LEFT2RIGHT:
		{
			uint left = charMap->FontIndex( characterCode);
			uint right = charMap->FontIndex( nextCharacterCode);
			newPen += face->KernAdvance( left, right);		        
			if( !face->Error_())
			{
				newPen += glyphs[charMap->GlyphListIndex( characterCode)]->Render( penPosition);
			}
			return newPen += FTPoint(fontGap, 0.0, 0.0);		
		}
		break;
	case FRD_BOTTOM2TOP:
		{
			if( !face->Error_() )
			{
				glyphs[charMap->GlyphListIndex( characterCode)]->Render( penPosition);
				const xs::FTBBox & glyphbox = glyphs[charMap->GlyphListIndex( characterCode)]->BBox();
				newPen += FTPoint(0.0, glyphbox.lowerY - glyphbox.upperY - fontGap, 0.0);
			}
			return newPen;
		}
		break;
	default:
		return penPosition;
		break;
	}
	return penPosition;
    //FTPoint kernAdvance, advance;
    //
    //uint left = charMap->FontIndex( characterCode);
    //uint right = charMap->FontIndex( nextCharacterCode);

    //kernAdvance = face->KernAdvance( left, right);
    //    
    //if( !face->Error_())
    //{
    //    advance = glyphs[charMap->GlyphListIndex( characterCode)]->Render( penPosition);
    //}
    //
    //kernAdvance += advance;
    //return kernAdvance;
}

}
