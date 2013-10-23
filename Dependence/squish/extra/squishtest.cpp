/* -----------------------------------------------------------------------------

Copyright (c) 2006 Simon Brown                          si@sjbrown.co.uk

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the 
"Software"), to	deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to 
permit persons to whom the Software is furnished to do so, subject to 
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

-------------------------------------------------------------------------- */

/*! @file

@brief	This program tests the error for 1 and 2-colour DXT compression.

This tests the effectiveness of the DXT compression algorithm for all
possible 1 and 2-colour blocks of pixels.
*/

#include <squish.h>
#include <iostream>
#include <cmath>
#include <cfloat>
#include <ddraw.h>
#include "CxImage\ximage.h"
#include "CxImage\xfile.h"



using namespace squish;


int main(int argc, char** argv)
{
	squish::u8 pixels[64*64*4];  // 16 pixels of input
	squish::u8 block[64*64];      // 8 bytes of output

	CxImage tga;
	bool ret = tga.Load("64x64.tga", CXIMAGE_FORMAT_TGA);

	squish::u8* idx = pixels;
	squish::u8* idx2 = block;


	//every 4x4
	for(size_t h = 0; h<16; ++h)
	{
		for(size_t w = 0; w<16; ++w)
		{
			for(size_t y = 0; y<4; ++y)
			{
				for(size_t x = 0; x<4; ++x)
				{
					RGBQUAD c = tga.GetPixelColor(w*4+x, 63-h*4-y);
					//memcpy(idx, &(tga.GetPixelColor(w*4+x, 63-y-h*4) ), 4);
					idx[0] = c.rgbRed;
					idx[1] = c.rgbGreen;
					idx[2] = c.rgbBlue;
					idx[3] = c.rgbReserved;
					idx += 4;
				}	
			}
			squish::Compress( idx-16*4, idx2, squish::kDxt5 );
			idx2 += 16;
		}
	}
	
	
	FILE* fin_dds = fopen("64x64.dds", "rb");//for head
	FILE* fin_dds2 = fopen("64x64_2.dds", "wb");//for sq write

	DWORD dwMagic;
	fread(&dwMagic, sizeof(DWORD), 1, fin_dds);
	fwrite(&dwMagic, sizeof(DWORD), 1, fin_dds2);

	DDSURFACEDESC2 dds_head;
	fread(&dds_head, sizeof(DDSURFACEDESC2), 1, fin_dds);
	fwrite(&dds_head, sizeof(DDSURFACEDESC2), 1, fin_dds2);

	fwrite(block, 1, 64*64, fin_dds2);

	fclose(fin_dds);
	fclose(fin_dds2);
	//
	//FILE* fout = fopen("test", "wb");
	//while(!feof(fin_dds) )
	//{
	//	DWORD text;
	//	fread(&text, sizeof(DWORD), 1, fin_dds);
	//	fwrite(&text, sizeof(DWORD), 1, fout);
	//	int i = 0;
	//}

	//fclose(fin_dds);
	//fclose(fout);
	//



	/* write some pixel data */

	//// compress the 4x4 block using DXT1 compression
	squish::Compress( pixels, block, squish::kDxt5 );
	int i = 0;
	memset(pixels, 0, 16*4);
	//// decompress the 4x4 block using DXT1 compression
	//squish::Decompress( pixels, block, squish::kDxt5 );
}
