#include    "FTTextureGlyph.h"

namespace xs{


ITexture* FTTextureGlyph::activeTexture = 0;

FTTextureGlyph::FTTextureGlyph(int fontHeight,IRenderSystem *pRenderSystem, FT_GlyphSlot glyph, ITexture* pTexture, int xOffset, int yOffset, int width, int height)
:   FTGlyph( glyph),m_pRenderSystem(pRenderSystem),
    destWidth(0),
    destHeight(0),
    texture(pTexture),
	fontHeight(fontHeight)
{
    err = FT_Render_Glyph( glyph, FT_RENDER_MODE_NORMAL);
    if( err || glyph->format != ft_glyph_format_bitmap)
    {
        return;
    }

    FT_Bitmap      bitmap = glyph->bitmap;

    destWidth  = bitmap.width;
    destHeight = bitmap.rows;

    if( destWidth && destHeight)
    {
		/*for(int i = 0;i < destHeight;i++)
		for(int j = 0;j < destWidth;j++)
		{
			char c = bitmap.buffer[j * destWidth + i];
			if(c > 0)
			{
				for(int k = -1;k <= 1;k++)
				for(int l = -1;l <= 1;l++)
				{
					int x = i + k;
					int y = j + l;
					if(x >= 0 && x < destWidth && y >= 0 && y < destHeight)
					{
						if(bitmap.buffer[y * destWidth + x] == 0)
							bitmap.buffer[y * destWidth + x] = c / 2;
					}
				}
			}
		}*/

//#define TYPE_1
#ifdef TYPE_0
		uint *pData = new uint[(destWidth + 2) * (destHeight + 2)];
		memset(pData,0,sizeof(uint) * (destWidth + 2) * (destHeight + 2));
		for(int i = 0;i < destHeight;i++)
		for(int j = 0;j < destWidth;j++)
		{
			uchar c = bitmap.buffer[i * destWidth + j];
			if(c > 0)
			{
				pData[(i + 1) * (destWidth + 2) + j + 1] = ((c << 24) | (0xFF << 16) | (0xFF << 8) | 0xFF);
				for(int k = -1;k <= 1;k++)
				for(int l = -1;l <= 1;l++)
				{
					if(k !=0 && l != 0)
					{
						int x = i + 1 + k;
						int y = j + 1 + l;
						if(pData[x * (destWidth + 2) + y] == 0)
						{
							pData[x * (destWidth + 2) + y] = 0xFF000000;
						}
					}
				}
			}
		}
		destWidth += 2;
		destHeight += 2;
#else
		#ifdef TYPE_1
			uint *pData = new uint[(destWidth + 1) * (destHeight + 1)];
			memset(pData,0,sizeof(uint) * (destWidth + 1) * (destHeight + 1));
			for(int i = 0;i < destHeight;i++)
			for(int j = 0;j < destWidth;j++)
			{
				uchar c = bitmap.buffer[i * destWidth + j];
				if(c > 0)
				{
					pData[(i + 1) * (destWidth + 1) + j + 1] = ((0xFF << 24) | (0 << 16) | (0 << 8) | 0);
					pData[i * (destWidth + 1) + j] = (c << 24) | (0xFF << 16) | (0xFF << 8) | 0xFF;
				}
			}
			destWidth += 1;
			destHeight += 1;
		#else
			#ifdef TYPE_2
				uint *pData = new uint[(destWidth + 2) * (destHeight + 2)];
				memset(pData,0,sizeof(uint) * (destWidth + 2) * (destHeight + 2));
				for(int i = 0;i < destHeight;i++)
				for(int j = 0;j < destWidth;j++)
				{
					uchar c = bitmap.buffer[i * destWidth + j];
					if(c > 0)
					{
						pData[(i + 2) * (destWidth + 2) + j + 2] = ((0xFF << 24) | (0 << 16) | (0 << 8) | 0);
						pData[i * (destWidth + 2) + j] = ((c << 24) | (0xFF << 16) | (0xFF << 8) | 0xFF);
					}
				}
				destWidth += 2;
				destHeight += 2;
			#else
				uint *pData = new uint[destWidth * destHeight];
				memset(pData,0,sizeof(uint) * destWidth * destHeight);
				for(int i = 0;i < destHeight;i++)
				for(int j = 0;j < destWidth;j++)
				{
					uchar c = bitmap.buffer[i * destWidth + j];
					if(c > 0)
					{
						pData[i * destWidth + j] = (c << 24) | (0xFF << 16) | (0xFF << 8) | 0xFF;
					}
				}
			#endif
		#endif
#endif
		//pData = new uint[destWidth * destHeight];
		/*for(int i = 0;i < destHeight;i++)
		for(int j = 0;j < destWidth;j++)
		{
			uchar c = bitmap.buffer[i * destWidth + j];
			if(c > 0)pData[i * (destWidth + 1) + j] = ((c << 24) | (0xFF << 16) | (0xFF << 8) | 0xFF);
		}*/
		/*for(int i = 0;i < destHeight;i++)
		for(int j = 0;j < destWidth;j++)
		{
			uchar c = bitmap.buffer[i * destWidth + j];
			if(c > 32)
			{
				for(int k = -1;k <= 1;k++)
				for(int l = -1;l <= 1;l++)
				{
					int m = i + k;
					int n = j + l;
					if(m >= 0 && n >= 0 && m < destHeight && n < destWidth)
					{
						uchar c = bitmap.buffer[m * destWidth + n];
						if(c <= 32)pData[m * destWidth + n] = 0xFF000000;
					}
				}
			}
		}*/
		texture->setSubData(0,xOffset,yOffset,destWidth,destHeight,pData);
		delete[] pData;
        //glPushClientAttrib( GL_CLIENT_PIXEL_STORE_BIT);
        //glPixelStorei( GL_UNPACK_LSB_FIRST, GL_FALSE);
        //glPixelStorei( GL_UNPACK_ROW_LENGTH, 0);
        //glPixelStorei( GL_UNPACK_ALIGNMENT, 1);

        //glBindTexture( GL_TEXTURE_2D, glTextureID);
        //glTexSubImage2D( GL_TEXTURE_2D, 0, xOffset, yOffset, destWidth, destHeight, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap.buffer);

        //glPopClientAttrib();
    }


//      0    
//      +----+
//      |    |
//      |    |
//      |    |
//      +----+
//           1
    
    uv[0].X( static_cast<float>(xOffset) / static_cast<float>(width));
    uv[0].Y( static_cast<float>(yOffset) / static_cast<float>(height));
    uv[1].X( static_cast<float>( xOffset + destWidth) / static_cast<float>(width));
    uv[1].Y( static_cast<float>( yOffset + destHeight) / static_cast<float>(height));
    
    pos.X( glyph->bitmap_left);
    pos.Y( glyph->bitmap_top);
}


FTTextureGlyph::~FTTextureGlyph()
{}


const FTPoint& FTTextureGlyph::Render(const FTPoint& pen)
{
    if( activeTexture != texture)
    {
		m_pRenderSystem->setTexture(0,texture);
        activeTexture = texture;
    }


	//const Matrix4& mtxWorldSave = m_pRenderSystem->getWorldMatrix();
	Matrix4 mtxWorldSave = m_pRenderSystem->getWorldMatrix();
	Matrix4 mtxWorld;
	mtxWorld.makeTrans(pen.X(),  pen.Y(), 0.0f);
	m_pRenderSystem->setWorldMatrix(mtxWorldSave * mtxWorld);

	m_pRenderSystem->beginPrimitive(PT_QUADS);
		/*m_pRenderSystem->setTexcoord(Vector2(uv[0].X(), uv[0].Y()));
		m_pRenderSystem->sendVertex(Vector2(pos.X(), pos.Y()));

		m_pRenderSystem->setTexcoord(Vector2(uv[0].X(), uv[1].Y()));
		m_pRenderSystem->sendVertex(Vector2(pos.X(), pos.Y() - destHeight));

		m_pRenderSystem->setTexcoord(Vector2(uv[1].X(), uv[1].Y()));
		m_pRenderSystem->sendVertex(Vector2(destWidth + pos.X(), pos.Y() - destHeight));

		m_pRenderSystem->setTexcoord(Vector2(uv[1].X(), uv[0].Y()));
		m_pRenderSystem->sendVertex(Vector2(destWidth + pos.X(), pos.Y()));*/

		m_pRenderSystem->setTexcoord(Vector2(uv[0].X(), uv[0].Y()));
		m_pRenderSystem->sendVertex(Vector2(pos.X(), fontHeight - pos.Y()));

		m_pRenderSystem->setTexcoord(Vector2(uv[0].X(), uv[1].Y()));
		m_pRenderSystem->sendVertex(Vector2(pos.X(), fontHeight - pos.Y() + destHeight));

		m_pRenderSystem->setTexcoord(Vector2(uv[1].X(), uv[1].Y()));
		m_pRenderSystem->sendVertex(Vector2(pos.X() + destWidth, fontHeight - pos.Y() + destHeight));

		m_pRenderSystem->setTexcoord(Vector2(uv[1].X(), uv[0].Y()));
		m_pRenderSystem->sendVertex(Vector2(pos.X() + destWidth, fontHeight - pos.Y()));
	m_pRenderSystem->endPrimitive();

	m_pRenderSystem->setWorldMatrix(mtxWorldSave);
/*
	glTranslatef( pen.X(),  pen.Y(), 0.0f);

    glBegin( GL_QUADS);
        glTexCoord2f( uv[0].X(), uv[0].Y());
        glVertex2f( pos.X(), pos.Y());

        glTexCoord2f( uv[0].X(), uv[1].Y());
        glVertex2f( pos.X(), pos.Y() - destHeight);

        glTexCoord2f( uv[1].X(), uv[1].Y());
        glVertex2f( destWidth + pos.X(), pos.Y() - destHeight);
        
        glTexCoord2f( uv[1].X(), uv[0].Y());
        glVertex2f( destWidth + pos.X(), pos.Y());
    glEnd();
*/
    return advance;
}


}
