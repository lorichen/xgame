#include "StdAfx.h"
#include "PixelFormat.h"
#include "ImageCodecTga.h" 

namespace xs
{
	const char*	ImageCodecTga::getType() const
	{
		static std::string strType = "tga";
		return strType.c_str();
	}

	enum TgaType
	{
		TGA_RGB	= 2,		// This tells us it's a normal RGB (really BGR) file
		TGA_A	= 3,		// This tells us it's a ALPHA file
		TGA_RLE = 10		// This tells us that the targa is Run-Length Encoded (RLE)
	};

	bool ImageCodecTga::getDimension(xs::Stream* input,uint& width,uint& height,uint& depth)
	{
		char length = 0;					// The length in bytes to the pixels
		char imageType = 0;					// The image type (RLE, RGB, Alpha...)

		// Read in the length in bytes from the header to the pixel data
		input->read(&length,sizeof(char));

		// Jump over one uchar
		input->seek(1,SEEK_CUR);

		// Read in the imageType (RLE, RGB, etc...)
		input->read(&imageType,sizeof(char));

		// Skip past general information we don't care about
		input->seek(9,SEEK_CUR);

		// Read the width, height and bits per pixel (16, 24 or 32)
		short w;
		short h;
		input->read(&w,sizeof(short));
		input->read(&h,sizeof(short));
		width = w;
		height = h;
		depth = 1;

		return true;
	}

	bool ImageCodecTga::decode(xs::Stream* input,ImageData& data,const void *p1,const void *p2) const
	{
		/* PF_R8G8B8 在大头的情况下D3D9不支持，所以将PF_R8G8B8的格式改成PF_B8G8R8
		32位用PF_B8G8R8A8
		*/
		short width = 0, height = 0;			// The dimensions of the image
		char length = 0;					// The length in bytes to the pixels
		char imageType = 0;					// The image type (RLE, RGB, Alpha...)

		char bits = 0;						// The bits per pixel for the image (16, 24, 32)
		int channels = 0;					// The channels of the image (3 = RGA : 4 = RGBA)
		int stride = 0;						// The stride (channels * width)
		int i = 0;							// A counter

		// This function loads in a TARGA (.TGA) file and returns its data to be
		// used as a texture or what have you.  This currently loads in a 16, 24
		// and 32-bit targa file, along with RLE compressed files.  Eventually you
		// will want to do more error checking to make it more robust.  This is
		// also a perfect start to go into a modular class for an engine.
		// Basically, how it works is, you read in the header information, then
		// move your file pointer to the pixel data.  Before reading in the pixel
		// data, we check to see the if it's an RLE compressed image.  This is because
		// we will handle it different.  If it isn't compressed, then we need another
		// check to see if we need to convert it from 16-bit to 24 bit.  24-bit and
		// 32-bit textures are very similar, so there's no need to do anything special.
		// We do, however, read in an extra bit for each color.

		// Read in the length in bytes from the header to the pixel data
		input->read(&length,sizeof(char));

		// Jump over one uchar
		input->seek(1,SEEK_CUR);

		// Read in the imageType (RLE, RGB, etc...)
		input->read(&imageType,sizeof(char));

		// Skip past general information we don't care about
		input->seek(9,SEEK_CUR);

		// Read the width, height and bits per pixel (16, 24 or 32)
		input->read(&width,sizeof(short));
		input->read(&height,sizeof(short));
		input->read(&bits,sizeof(char));
		if(width == 0 || height == 0)return false;

		// Now we move the file pointer to the pixel data
		input->seek(length + 1,SEEK_CUR);

		// Check if the image is RLE compressed or not
		if(imageType != TGA_RLE)
		{
			// Check if the image is a 24 or 32-bit image
			if(bits == 24 || bits == 32)
			{
				// Calculate the channels (3 or 4) - (use bits >> 3 for more speed).
				// Next, we calculate the stride and allocate enough memory for the pixels.
				channels = bits / 8;
				stride = channels * width;

				data.size = stride * height;
				data.pData = new uchar[data.size];
				data.format = (bits == 24 ? PF_B8G8R8 : PF_B8G8R8A8  );

				// Load in all the pixel data line by line
				for(int y = 0; y < height; y++)
				{
					// Store a pointer to the current line of pixels
					uchar *pLine = &(data.pData[stride * y]);

					// Read in the current line of pixels
					input->read(pLine,stride);

					//不用改变顺序了，
					//// Go through all of the pixels and swap the B and R values since TGA
					//// files are stored as BGR instead of RGB (or use GL_BGR_EXT verses GL_RGB)
					//for(i = 0; i < stride; i += channels)
					//{
					//	uchar temp     = pLine[i];
					//	pLine[i]     = pLine[i + 2];
					//	pLine[i + 2] = temp;
					//}
				}
			}
			// Check if the image is a 16 bit image (RGB stored in 1 ushort)
			else if(bits == 16)
			{
				ushort pixels = 0;
				uchar r = 0, g = 0, b = 0;

				// Since we convert 16-bit images to 24 bit, we hardcode the channels to 3.
				// We then calculate the stride and allocate memory for the pixels.
				channels = 3;
				stride = channels * width;

				data.size = stride * height;
				data.pData = new uchar[data.size];
				data.format = PF_B8G8R8 /*PF_R8G8B8*/;

				// Load in all the pixel data pixel by pixel
				for(int i = 0; i < width * height; i++)
				{
					// Read in the current pixel
					input->read(&pixels,sizeof(ushort));

					// To convert a 16-bit pixel into an R, G, B, we need to
					// do some masking and such to isolate each color value.
					// 0x1f = 11111 in binary, so since 5 bits are reserved in
					// each ushort for the R, G and B, we bit shift and mask
					// to find each value.  We then bit shift up by 3 to get the full color.
					b = static_cast<uchar>((pixels & 0x1f) << 3);
					g = static_cast<uchar>(((pixels >> 5) & 0x1f) << 3);
					r = static_cast<uchar>(((pixels >> 10) & 0x1f) << 3);

					//// This essentially assigns the color to our array and swaps the
					//// B and R values at the same time.
					//data.pData[i * 3 + 0] = r;
					//data.pData[i * 3 + 1] = g;
					//data.pData[i * 3 + 2] = b;
					data.pData[i * 3 + 0] = b;
					data.pData[i * 3 + 1] = g;
					data.pData[i * 3 + 2] = r;
				}
			}	
			// Else return a NULL for a bad or unsupported pixel format
			else
				return false;
		}
		// Else, it must be Run-Length Encoded (RLE)
		else
		{
			// First, let me explain real quickly what RLE is.  
			// For further information, check out Paul Bourke's intro article at: 
			// http://astronomy.swin.edu.au/~pbourke/dataformats/rle/
			// 
			// Anyway, we know that RLE is a basic type compression.  It takes
			// colors that are next to each other and then shrinks that info down
			// into the color and a integer that tells how much of that color is used.
			// For instance:
			// aaaaabbcccccccc would turn into a5b2c8
			// Well, that's fine and dandy and all, but how is it down with RGB colors?
			// Simple, you read in an color count (rleID), and if that number is less than 128,
			// it does NOT have any optimization for those colors, so we just read the next
			// pixels normally.  Say, the color count was 28, we read in 28 colors like normal.
			// If the color count is over 128, that means that the next color is optimized and
			// we want to read in the same pixel color for a count of (colorCount - 127).
			// It's 127 because we add 1 to the color count, as you'll notice in the code.

			// Create some variables to hold the rleID, current colors read, channels, & stride.
			uchar rleID = 0;
			int colorsRead = 0;
			channels = bits / 8;
			stride = channels * width;

			// Next we want to allocate the memory for the pixels and create an array,
			// depending on the channel count, to read in for each pixel.

			data.size = stride * height;
			data.pData = new uchar[data.size];
			data.format = (bits == 32 ?  PF_B8G8R8A8 /*PF_A8B8G8R8*/ : PF_B8G8R8 /*PF_R8G8B8*/);
			char *pColors = new char [channels];

			// Load in all the pixel data
			while(i < width*height)
			{
				// Read in the current color count + 1
				input->read(&rleID,sizeof(uchar));

				// Check if we don't have an encoded string of colors
				if(rleID < 128)
				{
					// Increase the count by 1
					rleID++;

					// Go through and read all the unique colors found
					while(rleID)
					{
						// Read in the current color
						input->read(pColors,sizeof(char) * channels);

						// Store the current pixel in our image array
						//data.pData[colorsRead + 0] = pColors[2];
						//data.pData[colorsRead + 1] = pColors[1];
						//data.pData[colorsRead + 2] = pColors[0];
						data.pData[colorsRead + 0] = pColors[0];
						data.pData[colorsRead + 1] = pColors[1];
						data.pData[colorsRead + 2] = pColors[2];

						// If we have a 4 channel 32-bit image, assign one more for the alpha
						if(bits == 32)
							data.pData[colorsRead + 3] = pColors[3];

						// Increase the current pixels read, decrease the amount
						// of pixels left, and increase the starting index for the next pixel.
						i++;
						rleID--;
						colorsRead += channels;
					}
				}
				// Else, let's read in a string of the same character
				else
				{
					// Minus the 128 ID + 1 (127) to get the color count that needs to be read
					rleID -= 127;

					// Read in the current color, which is the same for a while
					input->read(pColors,sizeof(char) * channels);

					// Go and read as many pixels as are the same
					while(rleID)
					{
						// Assign the current pixel to the current index in our pixel array
						//data.pData[colorsRead + 0] = pColors[2];
						//data.pData[colorsRead + 1] = pColors[1];
						//data.pData[colorsRead + 2] = pColors[0];
						data.pData[colorsRead + 0] = pColors[0];
						data.pData[colorsRead + 1] = pColors[1];
						data.pData[colorsRead + 2] = pColors[2];

						// If we have a 4 channel 32-bit image, assign one more for the alpha
						if(bits == 32)
							data.pData[colorsRead + 3] = pColors[3];

						// Increase the current pixels read, decrease the amount
						// of pixels left, and increase the starting index for the next pixel.
						i++;
						rleID--;
						colorsRead += channels;
					}

				}

			}

			// Free up pColors
			delete[] pColors;
		}

		// Fill in our ImageData structure to pass back
		data.width = width;
		data.height = height;
		data.depth = 1;
		data.num_mipmaps = 1;
		data.flags = 0;

		int pitch = data.width * channels;

		// flip the image bits...
		for (uint line = 0; line < data.height / 2; ++line)
		{
			int srcOffset = (line * pitch);
			int dstOffest = ((data.height - line - 1) * pitch);

			for (int colBit = 0; colBit < pitch; ++colBit)
			{
				uchar tmp = data.pData[dstOffest + colBit];
				data.pData[dstOffest + colBit] = data.pData[srcOffset + colBit];
				data.pData[srcOffset + colBit] = tmp;
			}

		}

		return true;
	}

	bool ImageCodecTga::code(void) const
	{
		Error("Code to TGA Not Implemented Yet!");
		return false;
	}

	bool ImageCodecTga::codeToFile(xs::FileStream&,const ImageData& data) const
	{
		Error("Code to TGA Not Implemented Yet!");
		return false;
	}

}