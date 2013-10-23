#include "FTFace.h"
#include "FTCharmap.h"

namespace xs{



FTCharmap::FTCharmap( FTFace* face)
:   ftFace( *(face->Face())),
    err(0)
{
    if( !ftFace->charmap)
    {
        err = FT_Set_Charmap( ftFace, ftFace->charmaps[0]);
    }
    
    ftEncoding = ftFace->charmap->encoding;
}


FTCharmap::~FTCharmap()
{
    charMap.clear();
}


bool FTCharmap::CharMap( FT_Encoding encoding)
{
    if( ftEncoding == encoding)
    {
        return true;
    }
    
    err = FT_Select_Charmap( ftFace, encoding );
    
    if( !err)
    {
        ftEncoding = encoding;
    }
    else
    {
        ftEncoding = ft_encoding_none;
    }
        
    charMap.clear();
    return !err;
}


uint FTCharmap::GlyphListIndex( uint characterCode )
{
    return charMap.find( characterCode);
}


uint FTCharmap::FontIndex( uint characterCode )
{
    return FT_Get_Char_Index( ftFace, characterCode);
}


void  FTCharmap::InsertIndex( const uint characterCode, const uint containerIndex)
{
    charMap.insert( characterCode, containerIndex);
}

}