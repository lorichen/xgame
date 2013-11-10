#include "StdAfx.h"
#include "PixelFormat.h"
#include "ImageCodecJpg.h" 

extern "C"
{
    #include "IJG/jconfig.h"
    #include "IJG/cdjpeg.h"
}

namespace xs
{
	const char*	ImageCodecJpg::getType() const
	{
		static std::string strType = "jpg";
		return strType.c_str();
	}

#define INPUT_BUF_SIZE  4096
	/**
	Structure dictated by IJG.
	*/
	class memory_source_mgr
	{
	public:
		struct jpeg_source_mgr  pub;
		int                     source_size;
		unsigned char*          source_data;
		boolean                 start_of_data;
		JOCTET*                 buffer;
	};

	typedef memory_source_mgr* mem_src_ptr;

	/**
	Signature dictated by IJG.
	*/
	static void  init_source(
		j_decompress_ptr        cinfo)
	{
		mem_src_ptr src = (mem_src_ptr) cinfo->src;
		src->start_of_data = TRUE;
	}
		/**
		Signature dictated by IJG.
		*/
	static boolean fill_input_buffer(
		j_decompress_ptr        cinfo)
	{

		mem_src_ptr src = (mem_src_ptr) cinfo->src;

		size_t bytes_read = 0;

		if (src->source_size > INPUT_BUF_SIZE)
			bytes_read = INPUT_BUF_SIZE;
		else
			bytes_read = src->source_size;

		memcpy (src->buffer, src->source_data, bytes_read);

		src->source_data += bytes_read;
		src->source_size -= bytes_read;

		src->pub.next_input_byte = src->buffer;
		src->pub.bytes_in_buffer = bytes_read;
		src->start_of_data = FALSE;


		return TRUE;
	}


	/**
	Signature dictated by IJG.
	*/
	static void  skip_input_data(
		j_decompress_ptr        cinfo,
		long                    num_bytes)
	{

		mem_src_ptr src = (mem_src_ptr)cinfo->src;

		if (num_bytes > 0) {
			while (num_bytes > (long) src->pub.bytes_in_buffer) {
				num_bytes -= (long) src->pub.bytes_in_buffer;
				boolean s = fill_input_buffer(cinfo);
				ASSERT(s); (void )s;
			}

			src->pub.next_input_byte += (size_t) num_bytes;
			src->pub.bytes_in_buffer -= (size_t) num_bytes;
		}
	}


	/**
	Signature dictated by IJG.
	*/
	static void  term_source(
		j_decompress_ptr        cinfo)
	{
		(void )cinfo;
		// Intentionally empty
	}

	/**
	Signature dictated by IJG.
	*/
	static void  jpeg_memory_src(j_decompress_ptr cinfo,JOCTET *buffer,int size)
	{
		mem_src_ptr src;

		if (cinfo->src == NULL)
		{
			// First time for this JPEG object
			cinfo->src = (struct jpeg_source_mgr*)
				(*cinfo->mem->alloc_small)(
				(j_common_ptr) cinfo,
				JPOOL_PERMANENT,
				sizeof(memory_source_mgr));

			src = (mem_src_ptr)cinfo->src;

			src->buffer = (JOCTET*)
				(*cinfo->mem->alloc_small)(
				(j_common_ptr) cinfo,
				JPOOL_PERMANENT,
				INPUT_BUF_SIZE * sizeof(JOCTET));
		}

		src = (mem_src_ptr)cinfo->src;
		src->pub.init_source        = init_source;
		src->pub.fill_input_buffer  = fill_input_buffer;
		src->pub.skip_input_data    = skip_input_data;

		// use default method
		src->pub.resync_to_restart  = jpeg_resync_to_restart;
		src->pub.term_source        = term_source;
		src->source_data            = buffer;
		src->source_size            = size;

		// forces fill_input_buffer on first read
		src->pub.bytes_in_buffer    = 0;

		// until buffer loaded
		src->pub.next_input_byte = NULL; 
	}

	bool ImageCodecJpg::getDimension(xs::Stream* input,uint& width,uint& height,uint& depth)
	{
		struct jpeg_decompress_struct   cinfo;
		struct jpeg_error_mgr           jerr;
		int                           loc = 0;

		// We have to set up the error handler, in case initialization fails.
		cinfo.err = jpeg_std_error(&jerr);

		// Initialize the JPEG decompression object.
		jpeg_create_decompress(&cinfo);

		// Specify data source (eg, a file, for us, memory)
		uint fileLength = input->getLength();

		SharedPtr<uchar>safeBuffer(new uchar[fileLength]);
		uchar *pMem = safeBuffer.getPointer();

		input->read(pMem,fileLength);
		jpeg_memory_src(&cinfo,pMem,fileLength);

		// Read the parameters with jpeg_read_header()
		jpeg_read_header(&cinfo,TRUE);

		// Start decompressor
		jpeg_start_decompress(&cinfo);

		// Get and set the values of interest to this object
		// Prepare the pointer object for the pixel data
		width			= cinfo.output_width;
		height			= cinfo.output_height;
		depth			= 1;

		// Release JPEG decompression object
		jpeg_destroy_decompress(&cinfo);

		return true;
	}


	bool ImageCodecJpg::decode(xs::Stream* input,ImageData& data,const void  *p1,const void  *p2) const
	{
		struct jpeg_decompress_struct   cinfo;
		struct jpeg_error_mgr           jerr;
		int                           loc = 0;

		// We have to set up the error handler, in case initialization fails.
		cinfo.err = jpeg_std_error(&jerr);

		// Initialize the JPEG decompression object.
		jpeg_create_decompress(&cinfo);

		// Specify data source (eg, a file, for us, memory)
		uint fileLength = input->getLength();

		SharedPtr<uchar>safeBuffer(new uchar[fileLength]);
		uchar *pMem = safeBuffer.getPointer();

		input->read(pMem,fileLength);
		jpeg_memory_src(&cinfo,pMem,fileLength);

		// Read the parameters with jpeg_read_header()
		jpeg_read_header(&cinfo,TRUE);

		// Set parameters for decompression
		// (We do nothing here since the defaults are fine)

		// Start decompressor
		jpeg_start_decompress(&cinfo);

		// JSAMPLEs per row in output buffer
		int bpp         = cinfo.output_components;

		if(bpp != 1 && bpp != 3 && bpp != 4)
		{
			Error("JPG Channels must be 1,3,4;");
			return false;
		}
		int row_stride  = cinfo.output_width * bpp;

		// Get and set the values of interest to this object
		// Prepare the pointer object for the pixel data
		data.width			= cinfo.output_width;
		data.height			= cinfo.output_height;
		data.depth			= 1;
		data.num_mipmaps	= 1;
		//渲染引擎是大头的，因此PF_R8G8B8不被D3D9支持，所以将解码后的格式改为PF_B8G8R8
		//data.format			= PF_R8G8B8;
		data.format			= PF_B8G8R8;
		data.flags			= 0;
		data.size			=	data.width * data.height * 3;
		data.pData			=	new uchar[data.size];

		// Make a one-row-high sample array that will go away when done with image
		JSAMPARRAY temp = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo,JPOOL_IMAGE,row_stride,1);

		// Read data on a scanline by scanline basis
		while(cinfo.output_scanline < cinfo.output_height)
		{
			// We may need to adjust the output based on the
			// number of channels it has.
			switch (bpp) 
			{
			case 1:
				// Grayscale; decompress to temp.
				jpeg_read_scanlines(&cinfo, temp, 1);
				// Expand to three channels
				{
					uchar* scan     = &(data.pData[(loc/1) * 3]);
					uchar* endScan  = scan + (data.width * 3);
					uchar* t        = *temp;

					while (scan < endScan)
					{
						uchar value = t[0];

						// Spread the value 3x.
						scan[0] = value;
						scan[1] = value;
						scan[2] = value;

						scan    += 3;
						t       += 1;
					}
				}
				break;
			case 3:
				// Read directly into the array
				{

#if 0
					// Need one extra level of indirection.
					uchar*     scan = data.pData + loc;
					JSAMPARRAY ptr  = &scan;
					jpeg_read_scanlines(&cinfo, ptr, 1);

#else //将格式转化为 PF_B8G8R8 
					// Need one extra level of indirection.
					uchar*     scan = data.pData + (loc/3)*3;
					JSAMPARRAY ptr  = &scan;
					jpeg_read_scanlines(&cinfo, ptr, 1);
					uchar *		endScan = scan + data.width * 3;
					uchar swap = 0;
					while( scan < endScan)
					{
						swap = scan[0];
						scan[0] = scan[2];
						scan[2] = swap;
						scan += 3;
					}
#endif 
				}
				break;
			case 4:
				// RGBA; decompress to temp.
				jpeg_read_scanlines(&cinfo, temp, 1);
				// Drop the 3rd channel
				{
#if 0
					uchar* scan     = &(data.pData[loc * 3]);//这个指针有错误
					uchar* endScan  = scan + data.width * 3;
					uchar* t        = *temp;

					while (scan < endScan)
					{
						scan[0] = t[0];
						scan[1] = t[1];
						scan[2] = t[2];
						scan    += 3;
						t       += 4;
					}
#else	//将格式转化为 PF_B8G8R8 
					uchar* scan     = &( data.pData[(loc/4)*3] );
					uchar* endScan  = scan + data.width * 3;
					uchar* t        = *temp;
					while (scan < endScan)
					{
						scan[0] = t[2];
						scan[1] = t[1];
						scan[2] = t[0];
						scan    += 3;
						t       += 4;
					}
#endif
				}
				break;
			}

			loc += row_stride;
		}

		// Finish decompression
		jpeg_finish_decompress(&cinfo);

		// Release JPEG decompression object
		jpeg_destroy_decompress(&cinfo);

		return true;
	}

	bool ImageCodecJpg::code(void ) const
	{
		Error("Code to JPG Not Implemented Yet!");
		return false;
	}

	bool ImageCodecJpg::codeToFile(xs::FileStream&,const ImageData& data) const
	{
		Error("Code to JPG Not Implemented Yet!");
		return false;
	}

}