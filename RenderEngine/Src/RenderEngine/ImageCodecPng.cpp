#include "StdAfx.h"
#include "PixelFormat.h"
#include "ImageCodecPng.h"
#include "png/png.h"

namespace xs
{
	const char*	ImageCodecPng::getType() const
	{
		static std::string strType = "png";
		return strType.c_str();
	}

	//libpng required function signature
	void  png_read_data(png_structp png_ptr,png_bytep data,png_size_t length)
	{
		ASSERT(png_ptr->io_ptr != NULL);
		ASSERT(length >= 0);
		ASSERT(data != NULL);

		((xs::Stream*)png_ptr->io_ptr)->read(data,length);
	}

	bool ImageCodecPng::getDimension(xs::Stream* input,uint& width,uint& height,uint& depth)
	{
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,png_error,png_warning);
		if(!png_ptr)
		{
			Error("Unable to initialize PNG decoder");
			return false;
		}

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if(!info_ptr)
		{
			png_destroy_read_struct(&png_ptr,(png_infopp)NULL,(png_infopp)NULL);
			Error("Unable to initialize PNG decoder");
			return false;
		}

		png_infop end_info = png_create_info_struct(png_ptr);
		if(!end_info)
		{
			png_destroy_read_struct(&png_ptr,&info_ptr,(png_infopp)NULL);
			Error("Unable to initialize PNG decoder");
			return false;
		}

		//now that the libpng structures are setup, change the error handlers and read routines
		//to use G3D functions so that BinaryInput can be used.

		png_set_read_fn(png_ptr,(png_voidp)input,png_read_data);

		//read in sequentially so that three copies of the file are not in memory at once
		png_read_info(png_ptr,info_ptr);

		uint png_width, png_height;
		int bit_depth, color_type, interlace_type;
		//this will validate the data it extracts from info_ptr
		png_get_IHDR(png_ptr,info_ptr,(png_uint_32*)&png_width,(png_uint_32*)&png_height,&bit_depth,&color_type,
			&interlace_type,int_p_NULL,int_p_NULL);

		width = png_width;
		height = png_height;
		depth = 1;

		png_destroy_read_struct(&png_ptr,&info_ptr,&end_info);

		return true;
	}


	bool ImageCodecPng::decode(xs::Stream* input,ImageData& data,const void  *p1,const void  *p2) const
	{
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,png_error,png_warning);
		if(!png_ptr)
		{
			Error("Unable to initialize PNG decoder");
			return false;
		}

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if(!info_ptr)
		{
			png_destroy_read_struct(&png_ptr,(png_infopp)NULL,(png_infopp)NULL);
			Error("Unable to initialize PNG decoder");
			return false;
		}

		png_infop end_info = png_create_info_struct(png_ptr);
		if(!end_info)
		{
			png_destroy_read_struct(&png_ptr,&info_ptr,(png_infopp)NULL);
			Error("Unable to initialize PNG decoder");
			return false;
		}

		//now that the libpng structures are setup, change the error handlers and read routines
		//to use G3D functions so that BinaryInput can be used.

		png_set_read_fn(png_ptr,(png_voidp)input,png_read_data);

		//read in sequentially so that three copies of the file are not in memory at once
		png_read_info(png_ptr,info_ptr);

		uint png_width, png_height;
		int bit_depth, color_type, interlace_type;
		//this will validate the data it extracts from info_ptr
		png_get_IHDR(png_ptr,info_ptr,(png_uint_32*)&png_width,(png_uint_32*)&png_height,&bit_depth,&color_type,
            &interlace_type,int_p_NULL,int_p_NULL);

		if(color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		{
			png_destroy_read_struct(&png_ptr,&info_ptr,&end_info);
			Error("Unsupported PNG color type - PNG_COLOR_TYPE_GRAY_ALPHA.");
			return false;
		}

		//swap bytes of 16 bit files to least significant uchar first
		png_set_swap(png_ptr);

		png_set_strip_16(png_ptr);

		//Expand paletted colors into true RGB triplets
		if(color_type == PNG_COLOR_TYPE_PALETTE)
		{
			png_set_palette_to_rgb(png_ptr);
		}

		//Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel
		if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		{
			png_set_gray_1_2_4_to_8(png_ptr);
		}

		//Expand paletted or RGB images with transparency to full alpha channels
		//so the data will be available as RGBA quartets.
		if(png_get_valid(png_ptr,info_ptr,PNG_INFO_tRNS))
		{
			png_set_tRNS_to_alpha(png_ptr);
		}

		// Fix sub-8 bit_depth to 8bit
		if (bit_depth < 8)
		{
			png_set_packing(png_ptr);
		}

		data.width = png_width;
		data.height = png_height;
		data.depth = 1;
		data.num_mipmaps = 1;
		data.flags = 0;

		uint channels = 0;
		if (color_type == PNG_COLOR_TYPE_RGBA)
		{
			channels = 4;
			data.format = PF_R8G8B8A8;
			data.size = data.width * data.height * channels;
			data.pData = new uchar[data.size];
		}
		else if((color_type == PNG_COLOR_TYPE_RGB) || (color_type == PNG_COLOR_TYPE_PALETTE))
		{
			channels = 3;
			data.format = PF_R8G8B8;
			data.size = data.width * data.height * channels;
			data.pData = new uchar[data.size];
		}
		else if(color_type == PNG_COLOR_TYPE_GRAY)
		{
			channels = 1;
			data.format = PF_A8;
			data.size = data.width * data.height * channels;
			data.pData = new uchar[data.size];
		}
		else
		{
			Error("Unsupported PNG bit-depth or type");
			return false;
		}

		//since we are reading row by row, required to handle interlacing
		uint number_passes = png_set_interlace_handling(png_ptr);

		png_read_update_info(png_ptr,info_ptr);

		for(uint pass = 0;pass < number_passes;++pass)
		for(uint y = 0;y < (uint)data.height;++y)
		{
			png_bytep rowPointer = &data.pData[data.width * channels * y]; 
			png_read_rows(png_ptr,&rowPointer,png_bytepp_NULL,1);
		}

		//todo:if the code below is needed?
		//png_read_end(png_ptr,info_ptr);

		png_destroy_read_struct(&png_ptr,&info_ptr,&end_info);

		return true;
	}

	bool ImageCodecPng::code(void ) const
	{
		Error("Code to PNG Not Implemented Yet!");
		return false;
	}

	bool ImageCodecPng::codeToFile(xs::FileStream&,const ImageData& data) const
	{
		Error("Code to PNG Not Implemented Yet!");
		return false;
	}

}