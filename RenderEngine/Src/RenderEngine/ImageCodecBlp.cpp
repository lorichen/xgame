#include "StdAfx.h"
#include "PixelFormat.h"
#include "ImageCodecBlp.h" 

using namespace xs;
//+-----------------------------------------------------------------------------
//| Buffer class
//+-----------------------------------------------------------------------------
class BUFFER
{
public:
	BUFFER();
	BUFFER(char *pData,int size);
	~BUFFER();

	void Clear();
	bool Resize(int NewSize);

	char* GetData() const;
	int GetSize() const;

	BUFFER& operator =(const BUFFER& CopyObject);
	BUFFER& operator =(const std::string& CopyString);

	char& operator [](int Index) const;

protected:
	char* Data;
	int Size;
	bool Self;
};

//+-----------------------------------------------------------------------------
//| Constructor
//+-----------------------------------------------------------------------------
BUFFER::BUFFER()
{
	Data = NULL;
	Size = 0;
	Self = true;
}

BUFFER::BUFFER(char *pData,int size)
{
	Data = pData;
	Size = size;
	Self = false;
}


//+-----------------------------------------------------------------------------
//| Destructor
//+-----------------------------------------------------------------------------
BUFFER::~BUFFER()
{
	Clear();
}


//+-----------------------------------------------------------------------------
//| Clears the buffer
//+-----------------------------------------------------------------------------
void BUFFER::Clear()
{
	if(Self)safeDeleteArray(Data);
	Size = 0;
}


//+-----------------------------------------------------------------------------
//| Resizes the buffer
//+-----------------------------------------------------------------------------
bool BUFFER::Resize(int NewSize)
{
	Clear();

	if(NewSize < 0)
	{
		Error("Unable to resize buffer, size is too small!");
		return FALSE;
	}

	Data = new char[NewSize + 1];
	if(Data == NULL)
	{
		Error("Unable to resize buffer, memory allocation failed!");
		return FALSE;
	}

	Size = NewSize;
	Data[Size] = '\0';

	return true;
}


//+-----------------------------------------------------------------------------
//| Returns the data from the buffer
//+-----------------------------------------------------------------------------
char* BUFFER::GetData() const
{
	return Data;
}


//+-----------------------------------------------------------------------------
//| Returns the buffer size
//+-----------------------------------------------------------------------------
int BUFFER::GetSize() const
{
	return Size;
}


//+-----------------------------------------------------------------------------
//| Assigns the contents from another buffer
//+-----------------------------------------------------------------------------
BUFFER& BUFFER::operator =(const BUFFER& CopyObject)
{
	Resize(CopyObject.Size);
	std::memcpy(Data, CopyObject.GetData(), Size);

	return (*this);
}


//+-----------------------------------------------------------------------------
//| Assigns the contents from a string
//+-----------------------------------------------------------------------------
BUFFER& BUFFER::operator =(const std::string& CopyString)
{
	Resize(static_cast<int>(CopyString.size()));
	std::memcpy(Data, CopyString.c_str(), Size);

	return (*this);
}


//+-----------------------------------------------------------------------------
//| References a byte in the buffer
//+-----------------------------------------------------------------------------
char& BUFFER::operator [](int Index) const
{
	return Data[Index];
}

#define XMD_H
#include "ijg/jpeglib.h"


//+-----------------------------------------------------------------------------
//| Source manager structure
//+-----------------------------------------------------------------------------
struct JPEG_SOURCE_MANAGER
{
	JPEG_SOURCE_MANAGER()
	{
		SourceBuffer = NULL;
		SourceBufferSize = 0;
		Buffer = NULL;
	}

	jpeg_source_mgr Manager;
	uchar* SourceBuffer;
	long SourceBufferSize;
	JOCTET* Buffer;
};


//+-----------------------------------------------------------------------------
//| Destination manager structure
//+-----------------------------------------------------------------------------
struct JPEG_DESTINATION_MANAGER
{
	JPEG_DESTINATION_MANAGER()
	{
		DestinationBuffer = NULL;
		DestinationBufferSize = 0;
		Buffer = NULL;
	}

	jpeg_destination_mgr Manager;
	uchar* DestinationBuffer;
	long DestinationBufferSize;
	JOCTET* Buffer;
};


//+-----------------------------------------------------------------------------
//| Jpeg class
//+-----------------------------------------------------------------------------
class JPEG
{
public:
	JPEG();
	~JPEG();

	bool Write(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int Width, int Height, int Quality);
	bool Read(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int* Width = NULL, int* Height = NULL);

protected:
	static void SetMemorySource(jpeg_decompress_struct* Info, uchar* Buffer, ulong Size);
	static void SetMemoryDestination(jpeg_compress_struct* Info, uchar* Buffer, ulong Size);

	static void SourceInit(jpeg_decompress_struct* Info);
	static boolean SourceFill(jpeg_decompress_struct* Info);
	static void SourceSkip(jpeg_decompress_struct* Info, long NrOfBytes);
	static void SourceTerminate(jpeg_decompress_struct* Info);

	static void DestinationInit(jpeg_compress_struct* Info);
	static boolean DestinationEmpty(jpeg_compress_struct* Info);
	static void DestinationTerminate(jpeg_compress_struct* Info);
};


//+-----------------------------------------------------------------------------
//| Global objects
//+-----------------------------------------------------------------------------
JPEG Jpeg;

//+-----------------------------------------------------------------------------
//| Constructor
//+-----------------------------------------------------------------------------
JPEG::JPEG()
{
	//Empty
}


//+-----------------------------------------------------------------------------
//| Destructor
//+-----------------------------------------------------------------------------
JPEG::~JPEG()
{
	//Empty
}


//+-----------------------------------------------------------------------------
//| Writes JPEG data
//+-----------------------------------------------------------------------------
bool JPEG::Write(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int Width, int Height, int Quality)
{
	int Stride;
	int RealSize;
	int DummySize;
	BUFFER TempBuffer;
	JSAMPROW Pointer[1];
	jpeg_compress_struct Info;
	jpeg_error_mgr ErrorManager;

	Info.err = jpeg_std_error(&ErrorManager);

	DummySize = ((Width * Height * 4) * 2) + 10000;
	TempBuffer.Resize(DummySize);

	jpeg_create_compress(&Info);

	SetMemoryDestination(&Info, reinterpret_cast<uchar*>(TempBuffer.GetData()), TempBuffer.GetSize());

	Info.image_width = Width;
	Info.image_height = Height;
	Info.input_components = 4;
	Info.in_color_space = JCS_UNKNOWN;

	jpeg_set_defaults(&Info);
	jpeg_set_quality(&Info, Quality, true);
	jpeg_start_compress(&Info, true);

	Stride = Width * 4;
	while(Info.next_scanline < Info.image_height)
	{
		Pointer[0] = reinterpret_cast<JSAMPROW>(&SourceBuffer[Info.next_scanline * Stride]);
		jpeg_write_scanlines(&Info, Pointer, 1);
	}

	jpeg_finish_compress(&Info);

	RealSize = DummySize - static_cast<int>(Info.dest->free_in_buffer);
	TargetBuffer.Resize(RealSize);

	std::memcpy(&TargetBuffer[0], &TempBuffer[0], RealSize);

	jpeg_destroy_compress(&Info);

	return true;
}


//+-----------------------------------------------------------------------------
//| Reads JPEG data
//+-----------------------------------------------------------------------------
bool JPEG::Read(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int* Width, int* Height)
{
	int i;
	int Stride;
	int Offset;
	char Opaque;
	JSAMPARRAY Pointer;
	jpeg_decompress_struct Info;
	jpeg_error_mgr ErrorManager;

	Info.err = jpeg_std_error(&ErrorManager);

	jpeg_create_decompress(&Info);
	SetMemorySource(&Info, reinterpret_cast<uchar*>(SourceBuffer.GetData()), SourceBuffer.GetSize());
	jpeg_read_header(&Info, true);
	jpeg_start_decompress(&Info);

	if((Info.output_components != 3) && (Info.output_components != 4))
	{
		Error("Nr of channels must be 3 or 4!");
		return FALSE;
	}

	TargetBuffer.Resize(Info.output_width * Info.output_height * 4);
	Stride = Info.output_width * Info.output_components;
	Offset = 0;

	Pointer = (*Info.mem->alloc_sarray)(reinterpret_cast<j_common_ptr>(&Info), JPOOL_IMAGE, Stride, 1);
	while(Info.output_scanline < Info.output_height)
	{
		jpeg_read_scanlines(&Info, Pointer, 1);
		std::memcpy(&TargetBuffer[Offset], Pointer[0], Stride);
		Offset += Stride;
	}

	jpeg_finish_decompress(&Info);

	(*reinterpret_cast<BYTE*>(&Opaque)) = 255;

	if(Info.output_components == 3)
	{
		for(i = (Info.output_width * Info.output_height - 1); i >= 0; i--)
		{
			TargetBuffer[(i * 4) + 3] = Opaque;
			TargetBuffer[(i * 4) + 2] = TargetBuffer[(i * 3) + 2];
			TargetBuffer[(i * 4) + 1] = TargetBuffer[(i * 3) + 1];
			TargetBuffer[(i * 4) + 0] = TargetBuffer[(i * 3) + 0];
		}
	}

	if(Width != NULL) (*Width) = Info.output_width;
	if(Height != NULL) (*Height) = Info.output_height;

	jpeg_destroy_decompress(&Info);

	return true;
}


//+-----------------------------------------------------------------------------
//| Sets the memory source
//+-----------------------------------------------------------------------------
void JPEG::SetMemorySource(jpeg_decompress_struct* Info, uchar* Buffer, ulong Size)
{
	JPEG_SOURCE_MANAGER* SourceManager;

	Info->src = reinterpret_cast<jpeg_source_mgr*>((*Info->mem->alloc_small)(reinterpret_cast<j_common_ptr>(Info), JPOOL_PERMANENT, sizeof(JPEG_SOURCE_MANAGER)));
	SourceManager = reinterpret_cast<JPEG_SOURCE_MANAGER*>(Info->src);

	SourceManager->Buffer = reinterpret_cast<JOCTET*>((*Info->mem->alloc_small)(reinterpret_cast<j_common_ptr>(Info), JPOOL_PERMANENT, Size * sizeof(JOCTET)));
	SourceManager->SourceBuffer = Buffer;
	SourceManager->SourceBufferSize = Size;
	SourceManager->Manager.init_source = SourceInit;
	SourceManager->Manager.fill_input_buffer = SourceFill;
	SourceManager->Manager.skip_input_data = SourceSkip;
	SourceManager->Manager.resync_to_restart = jpeg_resync_to_restart;
	SourceManager->Manager.term_source = SourceTerminate;
	SourceManager->Manager.bytes_in_buffer = 0;
	SourceManager->Manager.next_input_byte = NULL;
}


//+-----------------------------------------------------------------------------
//| Sets the memory destination
//+-----------------------------------------------------------------------------
void JPEG::SetMemoryDestination(jpeg_compress_struct* Info, uchar* Buffer, ulong Size)
{
	JPEG_DESTINATION_MANAGER* DestinationManager;

	Info->dest = reinterpret_cast<jpeg_destination_mgr*>((*Info->mem->alloc_small)(reinterpret_cast<j_common_ptr>(Info), JPOOL_PERMANENT, sizeof(JPEG_DESTINATION_MANAGER)));
	DestinationManager = reinterpret_cast<JPEG_DESTINATION_MANAGER*>(Info->dest);

	DestinationManager->Buffer = NULL;
	DestinationManager->DestinationBuffer = Buffer;
	DestinationManager->DestinationBufferSize = Size;
	DestinationManager->Manager.init_destination = DestinationInit;
	DestinationManager->Manager.empty_output_buffer = DestinationEmpty;
	DestinationManager->Manager.term_destination = DestinationTerminate;
}


//+-----------------------------------------------------------------------------
//| Initiates the memory source
//+-----------------------------------------------------------------------------
void JPEG::SourceInit(jpeg_decompress_struct* Info)
{
	//Empty
}


//+-----------------------------------------------------------------------------
//| Fills the memory source
//+-----------------------------------------------------------------------------
boolean JPEG::SourceFill(jpeg_decompress_struct* Info)
{
	JPEG_SOURCE_MANAGER* SourceManager;

	SourceManager = reinterpret_cast<JPEG_SOURCE_MANAGER*>(Info->src);

	SourceManager->Buffer = SourceManager->SourceBuffer;
	SourceManager->Manager.next_input_byte = SourceManager->Buffer;
	SourceManager->Manager.bytes_in_buffer = SourceManager->SourceBufferSize;

	return true;
}


//+-----------------------------------------------------------------------------
//| Skips the memory source
//+-----------------------------------------------------------------------------
void JPEG::SourceSkip(jpeg_decompress_struct* Info, LONG NrOfBytes)
{
	JPEG_SOURCE_MANAGER* SourceManager;

	SourceManager = reinterpret_cast<JPEG_SOURCE_MANAGER*>(Info->src);

	if(NrOfBytes > 0)
	{
		while(NrOfBytes > static_cast<LONG>(SourceManager->Manager.bytes_in_buffer))
		{
			NrOfBytes -= static_cast<LONG>(SourceManager->Manager.bytes_in_buffer);
			SourceFill(Info);
		}

		SourceManager->Manager.next_input_byte += NrOfBytes;
		SourceManager->Manager.bytes_in_buffer -= NrOfBytes;
	}
}


//+-----------------------------------------------------------------------------
//| Terminates the memory source
//+-----------------------------------------------------------------------------
void JPEG::SourceTerminate(jpeg_decompress_struct* Info)
{
	//Empty
}


//+-----------------------------------------------------------------------------
//| Initiates the memory destination
//+-----------------------------------------------------------------------------
void JPEG::DestinationInit(jpeg_compress_struct* Info)
{
	JPEG_DESTINATION_MANAGER* DestinationManager;

	DestinationManager = reinterpret_cast<JPEG_DESTINATION_MANAGER*>(Info->dest);

	DestinationManager->Buffer = DestinationManager->DestinationBuffer;
	DestinationManager->Manager.next_output_byte = DestinationManager->Buffer;
	DestinationManager->Manager.free_in_buffer = DestinationManager->DestinationBufferSize;
}


//+-----------------------------------------------------------------------------
//| Empties the memory destination
//+-----------------------------------------------------------------------------
boolean JPEG::DestinationEmpty(jpeg_compress_struct* Info)
{
	JPEG_DESTINATION_MANAGER* DestinationManager;

	DestinationManager = reinterpret_cast<JPEG_DESTINATION_MANAGER*>(Info->dest);

	DestinationManager->Manager.next_output_byte = DestinationManager->Buffer;
	DestinationManager->Manager.free_in_buffer = DestinationManager->DestinationBufferSize;

	return true;
}


//+-----------------------------------------------------------------------------
//| Terminates the memory destination
//+-----------------------------------------------------------------------------
void JPEG::DestinationTerminate(jpeg_compress_struct* Info)
{
	//Empty
}


namespace xs
{

	const char*	ImageCodecBlp::getType() const
	{
		static std::string strType = "blp";
		return strType.c_str();
	}

	bool ImageCodecBlp::getDimension(xs::Stream* input,uint& width,uint& height,uint& depth)
	{
		if(input->getLength() < 4)
		{
			Error("Invalid BLP File - invalid size\n");
			return false;
		}

		uint cc;
		input->read(&cc,4);
		switch(cc)
		{
		case '1PLB':
			{
				BLP_HEADER header;
				input->read(&header,sizeof(header));
				width = header.Width;
				height = header.Height;
				depth = 1;
				return true;
			}
		case '2PLB':
			{
				uchar attr[4];
				input->seek(4,SEEK_CUR);
				input->read(attr,4);	//attr[0]表示压缩方式，attr[1]表示alpha位数
				input->read(&width,4);
				input->read(&height,4);
				depth = 1;
				return true;
			}
		}

		return false;
	}

	bool ImageCodecBlp::loadCompressedBLP1(const BLP_HEADER& header,xs::Stream* input,ImageData& data) const
	{
		uint jpegHeaderSize;
		input->read(&jpegHeaderSize,sizeof(jpegHeaderSize));

		BUFFER TempBuffer;

		int size = header.Size[0] + jpegHeaderSize;
		uchar *buffer = new uchar[size];
		BUFFER TempBuffer2((char*)buffer,size);
		input->read(&TempBuffer2[0],jpegHeaderSize);
		input->seek(header.Offset[0]);
		input->read(&TempBuffer2[jpegHeaderSize],header.Size[0]);

		Jpeg.Read(TempBuffer2,TempBuffer);

		data.pData = new uchar[TempBuffer.GetSize()];
		memcpy(data.pData,TempBuffer.GetData(),TempBuffer.GetSize());
		delete[] buffer;

		data.depth = 1;
		data.flags = 0;
		data.format = PF_A8R8G8B8;
		data.height = header.Height;
		data.width = header.Width;
		data.num_mipmaps = 1;
		data.size = TempBuffer.GetSize();

		return true;
	}

	bool ImageCodecBlp::loadUnCompressedBLP1(const BLP_HEADER& header,xs::Stream* input,ImageData& data) const
	{
		return false;
	}

	bool ImageCodecBlp::decodeBLP1(xs::Stream* input,ImageData& data) const
	{
		BLP_HEADER header;
		input->read(&header,sizeof(header));

		if(header.Compression == 0)
			return loadCompressedBLP1(header,input,data);

		return loadUnCompressedBLP1(header,input,data);
	}

	bool ImageCodecBlp::decodeBLP2(xs::Stream* input,ImageData& data) const
	{
		uint offsets[16],sizes[16],w,h;

		uchar attr[4];

		input->seek(4,SEEK_CUR);
		input->read(attr,4);	//attr[0]表示压缩方式，attr[1]表示alpha位数
		input->read(&w,4);
		input->read(&h,4);
		input->read(offsets,4 * 16);
		input->read(sizes,4 * 16);

		data.height = h;
		data.width = w;
		data.depth = 1;
		data.size = 0;
		data.num_mipmaps = 0;
		data.flags = 0;
		data.format = PF_UNKNOWN;
		data.pData = 0;

		if(attr[0] == 3)
		{
			data.format = PF_A8R8G8B8;
			int iW = w;
			int iH = h;
			uint iSize = 0;
			for(uint i = 0;i < 16;i++)
			{
				if(iW == 0)iW = 1;
				if(iH == 0)iH = 1;
				if(offsets[i] && sizes[i])
				{
					iSize += PixelUtil::getMemorySize(iW,iH,data.depth,data.format);//(w+3)/4) * ((h+3)/4) * blocksize;
					data.num_mipmaps++;
				}
				else break;
				iW >>= 1;
				iH >>= 1;
			}
			data.size = iSize;

			uchar *buf = new uchar[sizes[0]];
			data.pData = new uchar[iSize];

			uint offset = 0;

			iW = w;
			iH = h;
			// do every mipmap level
			for (uint i = 0; i < 16;i++) 
			{
				if(iW == 0)iW = 1;
				if(iH == 0)iH = 1;
				if (offsets[i] && sizes[i])
				{
					input->seek(offsets[i]);
					int iSize = PixelUtil::getMemorySize(iW,iH,data.depth,data.format);//((w + 3) / 4) * ((h + 3) / 4) * blocksize;
					input->read(buf,iSize);
					memcpy(data.pData + offset,buf,iSize);
					offset += iSize;
				}
				else break;
				iW >>= 1;
				iH >>= 1;
			}
			delete[] buf;
		}
		else if(attr[0] == 2)
		{
			data.flags = IF_COMPRESSED;
			data.format = PF_DXT1;
			int blocksize = 8;

			if (attr[1]==8) 
			{
				// dxt3 or 5
				//if (attr[2]) format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				//else format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				data.format = PF_DXT3;
				if(attr[2] == 7)
				{
					data.format = PF_DXT5;
				}
				blocksize = 16;
			}
			else 
			{
				if(!attr[3]) 
				{
					data.format = PF_RGB_DXT1;
				}
			}

			int iW = w;
			int iH = h;
			uint iSize = 0;
			for(uint i = 0;i < 16;i++)
			{
				if(iW == 0)iW = 1;
				if(iH == 0)iH = 1;
				if(offsets[i] && sizes[i])
				{
					iSize += PixelUtil::getMemorySize(iW,iH,data.depth,data.format);//(w+3)/4) * ((h+3)/4) * blocksize;
					data.num_mipmaps++;
				}
				else break;
				iW >>= 1;
				iH >>= 1;
			}
			data.size = iSize;


			uchar *buf = new uchar[sizes[0]];
			data.pData = new uchar[iSize];

			uint offset = 0;

			iW = w;
			iH = h;
			// do every mipmap level
			for (uint i = 0; i < 16;i++) 
			{
				if(iW == 0)iW = 1;
				if(iH == 0)iH = 1;
				if (offsets[i] && sizes[i])
				{
					input->seek(offsets[i]);
					int iSize = PixelUtil::getMemorySize(iW,iH,data.depth,data.format);//((w + 3) / 4) * ((h + 3) / 4) * blocksize;
					input->read(buf,iSize);
					memcpy(data.pData + offset,buf,iSize);
					offset += iSize;
				}
				else break;
				iW >>= 1;
				iH >>= 1;
			}
			delete[] buf;
		}
		else if (attr[0] == 1) 
		{
			// uncompressed
			uint pal[256];
			input->read(pal,1024);

			uint iSize = 0;
			int iW = w;
			int iH = h;
			for(uint i = 0;i < 16;i++)
			{
				if(iW == 0)iW = 1;
				if(iH == 0)iH = 1;
				if (offsets[i] && sizes[i])
				{
					iSize += iW * iH * sizeof(uint);
					data.num_mipmaps++;
				}
				else break;
				iW >>= 1;
				iH >>= 1;
			}
			data.size = iSize;

			data.pData = new uchar[iSize];

			uchar *buf = new uchar[sizes[0]];
			uint *buf2 = new uint[w * h];
			uint *p;
			uchar *c, *a;

			uint alphabits = attr[1];
			bool hasalpha = alphabits != 0;

			//todo:
			data.format = PF_A8R8G8B8;

			int iOffset = 0;
			for (uint i=0; i<16; i++) 
			{
				if (w==0) w = 1;
				if (h==0) h = 1;
				if (offsets[i] && sizes[i]) 
				{
					input->seek(offsets[i]);
					input->read(buf,sizes[i]);

					uint cnt = 0;
					p = buf2;
					c = buf;
					a = buf + w * h;
					for(uint y = 0;y < h;y++) 
					{
						for(uint x = 0;x < w;x++)
						{
							uint k = pal[*c++];
							k = ((k & 0x00FF0000) >> 16) | ((k & 0x0000FF00)) | ((k & 0x000000FF) << 16);
							int alpha;
							if (hasalpha) 
							{
								if (alphabits == 8) 
								{
									alpha = (*a++);
								} 
								else if (alphabits == 1) 
								{
									alpha = (*a & (1 << cnt++)) ? 0xff : 0;
									if (cnt == 8) 
									{
										cnt = 0;
										a++;
									}
								}
							} 
							else 
							{
								alpha = 0xff;
							}

							k |= alpha << 24;
							*p++ = k;
						}
					}
					memcpy(data.pData + iOffset,buf2,w * h * sizeof(uint));
					iOffset += w * h * sizeof(uint);

				} else break;
				w >>= 1;
				h >>= 1;
			}

			delete[] buf2;
			delete[] buf;
		}

		return true;
	}

	bool ImageCodecBlp::decode(xs::Stream* input,ImageData& data,const void  *p1,const void  *p2) const
	{
		uint cc;
		input->read(&cc,4);

		switch(cc)
		{
		case '2PLB':
			return decodeBLP2(input,data);
		case '1PLB':
			return decodeBLP1(input,data);
		}

		return false;
	}

	bool ImageCodecBlp::code(void ) const
	{
		Error("Code to BLP Not Implemented Yet!");
		return false;
	}

	bool ImageCodecBlp::codeToFile(xs::FileStream&,const ImageData& data) const
	{
		Error("Code to BLP Not Implemented Yet!");
		return false;
	}

}