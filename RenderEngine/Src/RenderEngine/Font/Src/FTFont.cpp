#include    "FTFace.h"
#include    "FTFont.h"
#include    "FTGlyphContainer.h"
#include    "FTBBox.h"

namespace xs{


			/** 
		 * Render direction
		 */
		FontRenderDirection renderDirection;

		/**
		 *	adjacent text gap
		 */
		float fontGap;


FTFont::FTFont( const char* fontFilePath)
:   face( fontFilePath),
    useDisplayLists(true),
    glyphList(0),
	renderDirection(FRD_LEFT2RIGHT),
	fontGap(0.0f)
{
    err = face.Error_();
    if( err == 0)
    {
        glyphList = new FTGlyphContainer( &face);
    }
}


FTFont::FTFont( const uchar*pBufferBytes, size_t bufferSizeInBytes)
:   face( pBufferBytes, bufferSizeInBytes),
    glyphList(0),
	renderDirection(FRD_LEFT2RIGHT),
	fontGap(0.0f)
{
    err = face.Error_();
    if( err == 0)
    {
        glyphList = new FTGlyphContainer( &face);
    }
}


FTFont::~FTFont()
{
    delete glyphList;
}


bool FTFont::Attach( const char* fontFilePath)
{
    if( face.Attach( fontFilePath))
    {
        err = 0;
        return true;
    }
    else
    {
        err = face.Error_();
        return false;
    }
}


bool FTFont::Attach( const uchar*pBufferBytes, size_t bufferSizeInBytes)
{
    if( face.Attach( pBufferBytes, bufferSizeInBytes))
    {
        err = 0;
        return true;
    }
    else
    {
        err = face.Error_();
        return false;
    }
}


bool FTFont::FaceSize( const uint size, const uint res )
{
    charSize = face.Size( size, res);
    err = face.Error_();
    
    if( err != 0)
    {
        return false;
    }
    
    if( glyphList != NULL)
    {
        delete glyphList;
    }
    
    glyphList = new FTGlyphContainer( &face);
    return true;
}


uint FTFont::FaceSize() const
{
    return charSize.CharSize();
}


bool FTFont::CharMap( FT_Encoding encoding)
{
    bool result = glyphList->CharMap( encoding);
    err = glyphList->Error_();
    return result;
}


uint FTFont::CharMapCount()
{
    return face.CharMapCount();
}


FT_Encoding* FTFont::CharMapList()
{
    return face.CharMapList();
}


void  FTFont::UseDisplayList( bool useList)
{
    useDisplayLists = useList;
}

float FTFont::Ascender() const
{
    return charSize.Ascender();
}


float FTFont::Descender() const
{
    return charSize.Descender();
}

float FTFont::LineHeight() const
{
    return charSize.Height();
}

void  FTFont::BBox( const char* string,
                   float& llx, float& lly, float& llz, float& urx, float& ury, float& urz)
{
    FTBBox totalBBox;

    if((NULL != string) && ('\0' != *string))
    {
        const uchar* c = (uchar*)string;
        float advance = 0;

        if(CheckGlyph( *c))
        {
            totalBBox = glyphList->BBox( *c);
            advance = glyphList->Advance( *c, *(c + 1));
        }
                
        while( *++c)
        {
            if(CheckGlyph( *c))
            {
                FTBBox tempBBox = glyphList->BBox( *c);
                tempBBox.Move( FTPoint( advance, 0.0f, 0.0f));
                totalBBox += tempBBox;
                advance += glyphList->Advance( *c, *(c + 1));
            }
        }
    }

    llx = totalBBox.lowerX;
    lly = totalBBox.lowerY;
    llz = totalBBox.lowerZ;
    urx = totalBBox.upperX;
    ury = totalBBox.upperY;
    urz = totalBBox.upperZ;
}


void  FTFont::BBox( const wchar_t* string,
                   float& llx, float& lly, float& llz, float& urx, float& ury, float& urz)
{
    FTBBox totalBBox;

    if((NULL != string) && ('\0' != *string))
    {
        const wchar_t* c = string;
        float advance = 0;

        if(CheckGlyph( *c))
        {
            totalBBox = glyphList->BBox( *c);
            advance = glyphList->Advance( *c, *(c + 1));
        }
        
        while( *++c)
        {
            if(CheckGlyph( *c))
            {
                FTBBox tempBBox = glyphList->BBox( *c);
                tempBBox.Move( FTPoint( advance, 0.0f, 0.0f));
                totalBBox += tempBBox;
                advance += glyphList->Advance( *c, *(c + 1));
            }
        }
    }

    llx = totalBBox.lowerX;
    lly = totalBBox.lowerY;
    llz = totalBBox.lowerZ;
    urx = totalBBox.upperX;
    ury = totalBBox.upperY;
    urz = totalBBox.upperZ;
}


float FTFont::Advance( const wchar_t* string)
{
    const wchar_t* c = string;
    float width = 0.0f;

    while( *c)
    {
        if(CheckGlyph( *c))
        {
            width += glyphList->Advance( *c, *(c + 1));
        }
        ++c;
    }

    return width;
}


float FTFont::Advance( const char* string)
{
    const uchar* c = (uchar*)string;
    float width = 0.0f;

    while( *c)
    {
        if(CheckGlyph( *c))
        {
            width += glyphList->Advance( *c, *(c + 1));
        }
        ++c;
    }
    
    return width;
}


void  FTFont::Render(const char* string )
{
    const uchar* c = (uchar*)string;
    pen.X(0); pen.Y(0);

    while( *c)
    {
        if(CheckGlyph( *c))
        {
            pen = glyphList->Render( *c, *(c + 1), pen, renderDirection, fontGap);
        }
        ++c;
    }
}


void  FTFont::Render(const wchar_t* string )
{
    const wchar_t* c = string;
    pen.X(0); pen.Y(0);

    while( *c)
    {
        if(CheckGlyph( *c))
        {
            pen = glyphList->Render( *c, *(c + 1), pen, renderDirection, fontGap);
        }
        ++c;
    }
}

FontRenderDirection FTFont::setRenderDirection( FontRenderDirection rd)
{
	FontRenderDirection old = renderDirection;
	renderDirection = rd;
	return old;
}


float FTFont::setFontGap( float gap)
{
	float old = fontGap;
	fontGap  = gap;
	return old;
}


bool FTFont::CheckGlyph( const uint characterCode)
{
    if( NULL == glyphList->Glyph( characterCode))
    {
        uint glyphIndex = glyphList->FontIndex( characterCode);
        FTGlyph* tempGlyph = MakeGlyph( glyphIndex);
        if( NULL == tempGlyph)
        {
            if( 0 == err)
            {
                err = 0x13;
            }
            
            return false;
        }
        glyphList->Add( tempGlyph, characterCode);
    }
    
    return true;
}

}

