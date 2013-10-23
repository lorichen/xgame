
#include "StdAfx.h"
#include "Image.h"
#include "ImageCodec.h"

/* Use new scaling code when possible */
#define NEWSCALING

#ifdef NEWSCALING
#include "ImageResampler.h"
#endif

namespace xs 
{

	//-----------------------------------------------------------------------------
	Image::Image()
		: m_uWidth(0),
		m_uHeight(0),
		m_uDepth(0),
		m_uSize(0),
		m_uNumMipmaps(0),
		m_uFlags(0),
		m_pBuffer( NULL ),
		m_bAutoDelete( true )
	{
	}

	//-----------------------------------------------------------------------------
	Image::Image( const Image &img )
		: m_pBuffer( NULL ),
		m_bAutoDelete( true )
	{
		// call assignment operator
		*this = img;
	}

	//-----------------------------------------------------------------------------
	Image::~Image()
	{
		//Only delete if this was not a dynamic image (meaning app holds & destroys buffer)
		if(m_bAutoDelete)
		{
			safeDeleteArray(m_pBuffer);
		}
	}

	//-----------------------------------------------------------------------------
	Image & Image::operator = ( const Image &img )
	{
		if(m_bAutoDelete)
		{
			safeDeleteArray(m_pBuffer);
		}
		m_uWidth = img.m_uWidth;
		m_uHeight = img.m_uHeight;
		m_uDepth = img.m_uDepth;
		m_eFormat = img.m_eFormat;
		m_uSize = img.m_uSize;
		m_uFlags = img.m_uFlags;
		m_ucPixelSize = img.m_ucPixelSize;
		m_uNumMipmaps = img.m_uNumMipmaps;
		m_bAutoDelete = img.m_bAutoDelete;
		//Only create/copy when previous data was not dynamic data
		if( m_bAutoDelete )
		{
			m_pBuffer = new uchar[ m_uSize ];
			memcpy( m_pBuffer, img.m_pBuffer, m_uSize );
		}
		else
		{
			m_pBuffer = img.m_pBuffer;
		}

		return *this;
	}

	//-----------------------------------------------------------------------------
	void Image::flipAroundY()
	{
		if( !m_pBuffer )
		{
			throw("Can not flip an unitialized texture");
		}
        
         m_uNumMipmaps = 0; // Image operations lose precomputed mipmaps

		uchar	*pTempBuffer1 = NULL;
		ushort	*pTempBuffer2 = NULL;
		uchar	*pTempBuffer3 = NULL;
		uint	*pTempBuffer4 = NULL;

		uchar	*src1 = m_pBuffer, *dst1 = NULL;
		ushort	*src2 = (ushort *)m_pBuffer, *dst2 = NULL;
		uchar	*src3 = m_pBuffer, *dst3 = NULL;
		uint	*src4 = (uint *)m_pBuffer, *dst4 = NULL;

		ushort y;
		switch (m_ucPixelSize)
		{
		case 1:
			pTempBuffer1 = new uchar[m_uWidth * m_uHeight];
			for (y = 0; y < m_uHeight; y++)
			{
				dst1 = (pTempBuffer1 + ((y * m_uWidth) + m_uWidth - 1));
				for (ushort x = 0; x < m_uWidth; x++)
					memcpy(dst1--, src1++, sizeof(uchar));
			}

			memcpy(m_pBuffer, pTempBuffer1, m_uWidth * m_uHeight * sizeof(uchar));
			delete [] pTempBuffer1;
			break;

		case 2:
			pTempBuffer2 = new ushort[m_uWidth * m_uHeight];
			for (y = 0; y < m_uHeight; y++)
			{
				dst2 = (pTempBuffer2 + ((y * m_uWidth) + m_uWidth - 1));
				for (ushort x = 0; x < m_uWidth; x++)
					memcpy(dst2--, src2++, sizeof(ushort));
			}

			memcpy(m_pBuffer, pTempBuffer2, m_uWidth * m_uHeight * sizeof(ushort));
			delete [] pTempBuffer2;
			break;

		case 3:
			pTempBuffer3 = new uchar[m_uWidth * m_uHeight * 3];
			for (y = 0; y < m_uHeight; y++)
			{
				uint offset = ((y * m_uWidth) + (m_uWidth - 1)) * 3;
				dst3 = pTempBuffer3;
				dst3 += offset;
				for (ushort x = 0; x < m_uWidth; x++)
				{
					memcpy(dst3, src3, sizeof(uchar) * 3);
					dst3 -= 3; src3 += 3;
				}
			}

			memcpy(m_pBuffer, pTempBuffer3, m_uWidth * m_uHeight * sizeof(uchar) * 3);
			delete [] pTempBuffer3;
			break;

		case 4:
			pTempBuffer4 = new uint[m_uWidth * m_uHeight];
			for (y = 0; y < m_uHeight; y++)
			{
				dst4 = (pTempBuffer4 + ((y * m_uWidth) + m_uWidth - 1));
				for (ushort x = 0; x < m_uWidth; x++)
					memcpy(dst4--, src4++, sizeof(uint));
			}

			memcpy(m_pBuffer, pTempBuffer4, m_uWidth * m_uHeight * sizeof(uint));
			delete [] pTempBuffer4;
			break;

		default:
			throw("Unknown pixel depth");
			break;
		}
	}

	//-----------------------------------------------------------------------------
	void Image::flipAroundX()
	{
		if( !m_pBuffer )
		{
			throw("Can not flip an unitialized texture");
		}
        
        m_uNumMipmaps = 0; // Image operations lose precomputed mipmaps

		uint rowSpan = m_uWidth * m_ucPixelSize;

		uchar *pTempBuffer = new uchar[ rowSpan * m_uHeight ];
		uchar *ptr1 = m_pBuffer, *ptr2 = pTempBuffer + ( ( m_uHeight - 1 ) * rowSpan );

		for( ushort i = 0; i < m_uHeight; i++ )
		{
			memcpy( ptr2, ptr1, rowSpan );
			ptr1 += rowSpan; ptr2 -= rowSpan;
		}

		memcpy( m_pBuffer, pTempBuffer, rowSpan * m_uHeight);

		delete [] pTempBuffer;

	}

	//-----------------------------------------------------------------------------
	bool Image::loadDynamicImage( uchar* pData, uint uWidth, uint uHeight, 
		uint depth,
		PixelFormat eFormat, bool autoDelete, 
		uint numFaces, uint numMipMaps)
	{
		if( m_pBuffer && m_bAutoDelete )
		{
			delete[] m_pBuffer;
			m_pBuffer = NULL;
		}
		// Set image metadata
		m_uWidth = uWidth;
		m_uHeight = uHeight;
		m_uDepth = depth;
		m_eFormat = eFormat;
		m_ucPixelSize = PixelUtil::getNumElemBytes( m_eFormat );
		m_uNumMipmaps = numMipMaps;
		m_uFlags = 0;
		// Set flags
		if (PixelUtil::isCompressed(eFormat))
			m_uFlags |= IF_COMPRESSED;
		if (m_uDepth != 1)
			m_uFlags |= IF_3D_TEXTURE;
		if(numFaces == 6)
			m_uFlags |= IF_CUBEMAP;
		if(numFaces != 6 && numFaces != 1)
		{
			//Error("Number of faces currently must be 6 or 1.");
			return false;
		}

		m_uSize = calculateSize(numMipMaps, numFaces, uWidth, uHeight, depth, eFormat);
		m_pBuffer = pData;
		m_bAutoDelete = autoDelete;

		return true;
	}

	//-----------------------------------------------------------------------------
	bool Image::loadRawData(
		xs::Stream* stream, 
		uint uWidth, uint uHeight, uint uDepth,
		PixelFormat eFormat,
		uint numFaces, uint numMipMaps)
	{
		uint size = calculateSize(numMipMaps, numFaces, uWidth, uHeight, uDepth, eFormat);
		if (size != stream->getLength())
		{
			//Error("Stream size does not match calculated image size");
			return false;
		}

		uchar *buffer = new uchar[ size ];
		stream->read(buffer, size);

		return loadDynamicImage(buffer,
			uWidth, uHeight, uDepth,
			eFormat, true, numFaces, numMipMaps);

	}

	//-----------------------------------------------------------------------------
	bool Image::getDimension(xs::Stream* stream,const std::string& type,uint& width,uint& height,uint& depth)
	{
		ImageCodec * pCodec = ImageCodec::getCodec(type.c_str());
		if(!pCodec)
		{
			//Error("Image::getDimension invalid extension!\n");
			return false;
		}

		return pCodec->getDimension(stream,width,height,depth);
	}

	//-----------------------------------------------------------------------------
	bool Image::getDimension(const std::string& strFileName,uint& width,uint& height,uint& depth)
	{
		std::string strExt = getFileExt(strFileName);

		ImageCodec * pCodec = ImageCodec::getCodec(strExt.c_str());
		if(!pCodec)
		{
			//Error("Image::getDimension invalid extension "<<strFileName.c_str()<<endl);
			return false;
		}
		xs::CStreamHelper data = xs::getFileSystem()->open(strFileName.c_str());
		if(!data)
		{
			//Error("Image::getDimemsion unable to open "<<strFileName.c_str()<<endl);
			return false;
		}

		return pCodec->getDimension(data,width,height,depth);
	}

	//-----------------------------------------------------------------------------
	bool Image::load(const std::string& strFileName)
	{
		if( m_pBuffer && m_bAutoDelete )
		{
			delete[] m_pBuffer;
			m_pBuffer = NULL;
		}

		std::string strExt = getFileExt(strFileName);

		ImageCodec * pCodec = ImageCodec::getCodec(strExt.c_str());
		if( !pCodec )
		{
			//Error(std::string(std::string("Unable to load image file") + strFileName + " - invalid extension.\n").c_str());
			return false;
		}

		xs::CStreamHelper data = xs::getFileSystem()->open(strFileName.c_str());
		if(!data)
		{
			//crr add 
			//部分文件不是以game.exe所在位置算相对路径，而是默认从data开始
			//这里加上data重试，fixme
			std::string strTmp = "data\\" + strFileName;

			WarningLn(std::string(std::string("need try to prefix data\\  to open ") + strFileName).c_str());
			data = xs::getFileSystem()->open(strTmp.c_str());
			if(!data)
			{
				//ErrorLn(std::string(std::string("Unable to open ") + strFileName).c_str());
				return false;
			}
		}

		ImageCodec::ImageData result;
		if(!pCodec->decode((Stream*)data,result))
		{
			//Error(std::string(std::string("Unable to decode") + strFileName).c_str());
			return false;
		}

		// Get the format and compute the pixel size
		m_uWidth = result.width;
		m_uHeight = result.height;
		m_uDepth = result.depth;
		m_uSize = result.size;
		m_eFormat = result.format;
		m_uNumMipmaps = result.num_mipmaps;
		m_ucPixelSize = PixelUtil::getNumElemBytes( m_eFormat );
		m_uFlags = result.flags;
		m_pBuffer = result.pData;

		return true;
	}
	//-----------------------------------------------------------------------------
	bool Image::save(const std::string& filename)
	{
		if( !m_pBuffer )
		{
			//Error("No image data loaded");
			return false;
		}

		std::string strExt = getFileExt(filename);

		ImageCodec * pCodec = ImageCodec::getCodec(strExt.c_str());
		if( !pCodec )
		{
			//Error(std::string(std::string("Unable to save image file") + filename + " - invalid extension.").c_str());
			return false;
		}

		ImageCodec::ImageData data;
		data.width = m_uWidth;
		data.height = m_uHeight;
		data.depth = m_uDepth;
		data.size = m_uSize;
		data.format = m_eFormat;
		data.num_mipmaps = m_uNumMipmaps;
		data.flags = m_uFlags;
		data.pData = m_pBuffer;
		xs::FileStream file(filename.c_str());
		if(!file.open("wb+"))
		{
			//Error(std::string(std::string("Unable to save image file") + filename).c_str());
			return false;
		}

		bool bResult = pCodec->codeToFile(file,data);
		file.close();

		return bResult;
	}
	//-----------------------------------------------------------------------------
	bool Image::load(xs::Stream* stream, const std::string& type ,const void *p1,const void *p2)
	{
		std::string strType = type;

		ImageCodec * pCodec = ImageCodec::getCodec(strType.c_str());
		if( !pCodec )
		{
			//Error("Unable to load image - invalid extension.");
			return false;
		}

		if( m_pBuffer && m_bAutoDelete )
		{
			delete[] m_pBuffer;
			m_pBuffer = NULL;
		}

		ImageCodec::ImageData result;
		if(!pCodec->decode(stream,result,p1,p2))
		{
			//Error("Unable to decode stream");
			return false;
		}

		// Get the format and compute the pixel size
		m_uWidth = result.width;
		m_uHeight = result.height;
		m_uDepth = result.depth;
		m_uSize = result.size;
		m_eFormat = result.format;
		m_uNumMipmaps = result.num_mipmaps;
		m_ucPixelSize = PixelUtil::getNumElemBytes( m_eFormat );
		m_uFlags = result.flags;
		m_pBuffer = result.pData;

		return true;

	}

	//-----------------------------------------------------------------------------
	uchar* Image::getData()
	{
		return m_pBuffer;
	}

	//-----------------------------------------------------------------------------
	const uchar* Image::getData() const
	{
		ASSERT( m_pBuffer );
		return m_pBuffer;
	}

	const uchar * Image::getData() volatile
	{
		ASSERT( m_pBuffer);
		return m_pBuffer;
	}

	//-----------------------------------------------------------------------------
	uint Image::getSize() const
	{
		return m_uSize;
	}

	//-----------------------------------------------------------------------------
	uint Image::getLineSize() const
	{
		return m_uWidth * m_ucPixelSize;
	}

	//-----------------------------------------------------------------------------
	uchar Image::getPixelSize() const
	{
		return m_ucPixelSize;
	}

	//-----------------------------------------------------------------------------
	uint Image::getNumMipmaps() const
	{
		return m_uNumMipmaps;
	}

	//-----------------------------------------------------------------------------
	bool Image::hasFlag(const ImageFlags imgFlag) const
	{
		if(m_uFlags & imgFlag)
			return true;

		return false;
	}

	//-----------------------------------------------------------------------------
	uint Image::getDepth() const
	{
		return m_uDepth;
	}
	//-----------------------------------------------------------------------------
	uint Image::getWidth() const
	{
		return m_uWidth;
	}

	//-----------------------------------------------------------------------------
	uint Image::getHeight() const
	{
		return m_uHeight;
	}
	//-----------------------------------------------------------------------------
	uint Image::getNumFaces(void) const
	{
		if(hasFlag(IF_CUBEMAP))
			return 6;
		return 1;
	}
	//-----------------------------------------------------------------------------
	uint Image::getRowSpan() const
	{
		return m_uWidth * m_ucPixelSize;
	}

	//-----------------------------------------------------------------------------
	PixelFormat Image::getFormat() const
	{
		return m_eFormat;
	}

	//-----------------------------------------------------------------------------
	uchar Image::getBPP() const
	{
		return m_ucPixelSize * 8;
	}

	//-----------------------------------------------------------------------------
	int Image::getFlags() const
	{
		return m_uFlags;
	}

	//-----------------------------------------------------------------------------
	bool Image::getHasAlpha(void) const
	{
		return PixelUtil::getFlags(m_eFormat) & PFF_HASALPHA;
	}

	//-----------------------------------------------------------------------------
	bool Image::subImage(const Image& img,const Rect& rc)
	{
		if(PFF_COMPRESSED & PixelUtil::getFlags(img.m_eFormat))
		{
			//Error("Image::subImage not support compressed image yet!\n");
			return false;
		}
		if(m_uNumMipmaps > 1)
		{
			//Error("Image::subImage not support mipmaps yet!\n");
			return false;
		}

		if(img.m_uDepth != 1)
		{
			//Error("Image::subImage img.m_uDepth != 1");
			return false;
		}

		if( m_pBuffer && m_bAutoDelete )
		{
			delete[] m_pBuffer;
			m_pBuffer = NULL;
		}
		int w = img.getWidth();
		int h = img.getHeight();
		Rect rect = rc;
		if(rect.left < 0)rect.left = 0;
		if(rect.top < 0)rect.top = 0;
		if(rect.right >= w)rect.right = w - 1;
		if(rect.bottom >= h)rect.bottom = h - 1;
		if(rect.bottom < rect.top || rect.right < rect.left)
		{
			//Error("Image::subImage rc is invalid!");
			return false;
		}

		m_ucPixelSize = img.m_ucPixelSize;
		m_uWidth = rect.right - rect.left + 1;
		m_uHeight = rect.bottom - rect.top + 1;
		m_uDepth = img.m_uDepth;
		m_eFormat = img.m_eFormat;
		m_uSize = PixelUtil::getMemorySize(m_uWidth, m_uHeight, 1, m_eFormat);
		m_uFlags = img.m_uFlags;
		m_uNumMipmaps = 1;
		m_bAutoDelete = true;
		if( m_bAutoDelete )
		{
			m_pBuffer = new uchar[ m_uSize ];
			for(int i = rect.top;i <= rect.bottom;i++)
			{
				uchar *pBuffer = img.m_pBuffer + i * w * img.m_ucPixelSize + rect.left * m_ucPixelSize;
				memcpy(m_pBuffer + (i - rect.top) * m_uWidth * m_ucPixelSize,pBuffer,m_uWidth * m_ucPixelSize);
				if(m_pBuffer + (i - rect.top) * m_uWidth * m_ucPixelSize + m_uWidth * m_ucPixelSize > m_pBuffer + m_uSize)
				{
					//Error("Image::subImage ooo!");
				}
			}
		}

		return true;
	}

	bool Image::setColorKey(const ColorValue& color)
	{
		if(PFF_COMPRESSED & PixelUtil::getFlags(m_eFormat))
		{
			//Error("Image::setColorKey not support compressed image yet!\n");
			//Error("DDS格式不需要ColorKey\n");
			return false;
		}

		switch(m_eFormat)
		{
		case PF_A8R8G8B8:
		case PF_X8R8G8B8:
		case PF_R8G8B8A8:
			{
				uint r = 1;
				uint g = 2;
				uint b = 3;
				uint a = 0;
				if(m_eFormat == PF_R8G8B8A8)
				{
					r = 0;
					g = 1;
					b = 2;
					a = 3;
				}
				for(uint i = 0;i < m_uHeight;i++)
				for(uint j = 0;j < m_uWidth;j++)
				{
					uchar *pPixel = m_pBuffer + (i * m_uWidth + j) * m_ucPixelSize;
					if(pPixel[r] == (uchar)(color.r * 255) && pPixel[g] == (uchar)(color.g * 255) && pPixel[b] == (uchar)(color.b * 255))
					{
						pPixel[a] = 0;
					}
					else
					{
						if(m_eFormat == PF_X8R8G8B8)pPixel[a] = 255;
					}
				}
			}
			if(m_eFormat == PF_X8R8G8B8)m_eFormat = PF_A8R8G8B8;
			return true;
		case PF_B8G8R8:
		case PF_R8G8B8:
			{
				PixelFormat pf = PF_R8G8B8A8;
				uchar ucPixelSize = PixelUtil::getNumElemBytes(pf);
				uchar *pBuffer = new uchar[m_uWidth * m_uHeight * ucPixelSize];
				for(uint i = 0;i < m_uHeight;i++)
				for(uint j = 0;j < m_uWidth;j++)
				{
					uchar *pPixelSrc = m_pBuffer + (i * m_uWidth + j) * m_ucPixelSize;
					uchar *pPixelDst = pBuffer + (i * m_uWidth + j) * ucPixelSize;
					if(m_eFormat == PF_B8G8R8)
					{
						pPixelDst[0] = pPixelSrc[2];
						pPixelDst[1] = pPixelSrc[1];
						pPixelDst[2] = pPixelSrc[0];
					}
					else
					{
						pPixelDst[0] = pPixelSrc[0];
						pPixelDst[1] = pPixelSrc[1];
						pPixelDst[2] = pPixelSrc[2];
					}
					if(pPixelDst[0] == (uchar)(color.r * 255) && pPixelDst[1] == (uchar)(color.g * 255) && pPixelDst[2] == (uchar)(color.b * 255))
					{
						pPixelDst[3] = 0;
					}
					else
					{
						pPixelDst[3] = 255;
					}
				}
				m_ucPixelSize = ucPixelSize;
				m_eFormat = pf;
				m_uSize = PixelUtil::getMemorySize(m_uWidth, m_uHeight, m_uDepth, m_eFormat);

				if(m_pBuffer && m_bAutoDelete)
				{
					delete[] m_pBuffer;
					m_pBuffer = 0;
				}
				m_bAutoDelete = true;
				m_pBuffer = pBuffer;
			}
			return true;
		case PF_R5G6B5:
			{
				PixelFormat pf = PF_A1R5G5B5;
				uchar ucPixelSize = PixelUtil::getNumElemBytes(pf);
				uchar *pBuffer = new uchar[m_uWidth * m_uHeight * ucPixelSize];
				for(uint i = 0;i < m_uHeight;i++)
				for(uint j = 0;j < m_uWidth;j++)
				{
					uchar *pPixelSrc = m_pBuffer + (i * m_uWidth + j) * m_ucPixelSize;
					uchar *pPixelDst = pBuffer + (i * m_uWidth + j) * ucPixelSize;
					ushort c = *(ushort*)pPixelSrc;
					uchar r,g,b,a;
					MAKE565RGB(c,r,g,b);
					if(r == (uchar)(color.r * 255) && g == (uchar)(color.g * 255) && b == (uchar)(color.b * 255))
					{
						a = 0;
					}
					else
					{
						a = 1;
					}
					*(ushort*)pPixelDst = MAKE1555WORD(a,r,g,b);
				}
				m_ucPixelSize = ucPixelSize;
				m_eFormat = pf;
				m_uSize = PixelUtil::getMemorySize(m_uWidth, m_uHeight, m_uDepth, m_eFormat);

				if(m_pBuffer && m_bAutoDelete)
				{
					delete[] m_pBuffer;
					m_pBuffer = 0;
				}
				m_bAutoDelete = true;
				m_pBuffer = pBuffer;
			}
			return true;
		case PF_A1R5G5B5:
			{
				for(uint i = 0;i < m_uHeight;i++)
				for(uint j = 0;j < m_uWidth;j++)
				{
					uchar *pPixelSrc = m_pBuffer + (i * m_uWidth + j) * m_ucPixelSize;
					ushort c = *(ushort*)pPixelSrc;
					uchar r,g,b;
					MAKE555RGB(c,r,g,b);
					if(r == (uchar)(color.r * 255) && g == (uchar)(color.g * 255) && b == (uchar)(color.b * 255))
					{
						c &= 0x7FFF;
					}
					else
					{
						//c |= 0x8000;
					}
					*(ushort*)pPixelSrc = c;
				}
			}
			return true;
		case PF_A4R4G4B4:
			{
				for(uint i = 0;i < m_uHeight;i++)
				for(uint j = 0;j < m_uWidth;j++)
				{
					uchar *pPixelSrc = m_pBuffer + (i * m_uWidth + j) * m_ucPixelSize;
					ushort c = *(ushort*)pPixelSrc;
					uchar r,g,b;
					MAKE444RGB(c,r,g,b);
					if(r == (uchar)(color.r * 255) && g == (uchar)(color.g * 255) && b == (uchar)(color.b * 255))
					{
						c &= 0x0FFF;
					}
					else
					{
						//c |= 0xF000;
					}
					*(ushort*)pPixelSrc = c;
				}
			}
			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------------
	void Image::applyGamma( unsigned char *buffer, float gamma, uint size, uchar bpp )
	{
		if( gamma == 1.0f )
			return;

		//NB only 24/32-bit supported
		if( bpp != 24 && bpp != 32 ) return;

		uint stride = bpp >> 3;

		for( uint i = 0, j = size / stride; i < j; i++, buffer += stride )
		{
			float r, g, b;

			r = (float)buffer[0];
			g = (float)buffer[1];
			b = (float)buffer[2];

			r = r * gamma;
			g = g * gamma;
			b = b * gamma;

			float scale = 1.0f, tmp;

			if( r > 255.0f && (tmp=(255.0f/r)) < scale )
				scale = tmp;
			if( g > 255.0f && (tmp=(255.0f/g)) < scale )
				scale = tmp;
			if( b > 255.0f && (tmp=(255.0f/b)) < scale )
				scale = tmp;

			r *= scale; g *= scale; b *= scale;

			buffer[0] = (uchar)r;
			buffer[1] = (uchar)g;
			buffer[2] = (uchar)b;
		}
	}
	//-----------------------------------------------------------------------------
	void Image::resize(ushort width, ushort height, Filter filter)
	{
		// resizing dynamic images is not supported
		ASSERT(m_bAutoDelete);
		ASSERT(m_uDepth == 1);

		// reassign buffer to temp image, make sure auto-delete is true
		Image temp;
		temp.loadDynamicImage(m_pBuffer, m_uWidth, m_uHeight, 1, m_eFormat, true);
		// do not delete[] m_pBuffer!  temp will destroy it

		// set new dimensions, allocate new buffer
		m_uWidth = width;
		m_uHeight = height;
		m_uSize = PixelUtil::getMemorySize(m_uWidth, m_uHeight, 1, m_eFormat);
		m_pBuffer = new uchar[m_uSize];
        m_uNumMipmaps = 0; // Loses precomputed mipmaps

		// scale the image from temp into our resized buffer
		Image::scale(temp.getPixelBox(), getPixelBox(), filter);
	}
	//-----------------------------------------------------------------------
	void Image::scale(const PixelBox &src, const PixelBox &scaled, Filter filter) 
	{
		ASSERT(PixelUtil::isAccessible(src.format));
		ASSERT(PixelUtil::isAccessible(scaled.format));
#ifdef NEWSCALING
		typedef SharedPtr<xs::MemoryStream>	MemStreamPtr;
		MemStreamPtr buf; // For auto-delete
		PixelBox temp;
		switch (filter) {
		case FILTER_NEAREST:
			if(src.format == scaled.format) {
				// No intermediate buffer needed
				temp = scaled;
			}
			else
			{
				// Allocate temporary buffer of destination size in source format 
				temp = PixelBox(scaled.getWidth(), scaled.getHeight(), scaled.getDepth(), src.format);
				buf = MemStreamPtr(new xs::MemoryStream(temp.getConsecutiveSize(),0));
				temp.data = buf->getBuffer();
			}
			// super-optimized: no conversion
			switch (PixelUtil::getNumElemBytes(src.format)) {
			case 1: NearestResampler<1>::scale(src, temp); break;
			case 2: NearestResampler<2>::scale(src, temp); break;
			case 3: NearestResampler<3>::scale(src, temp); break;
			case 4: NearestResampler<4>::scale(src, temp); break;
			case 6: NearestResampler<6>::scale(src, temp); break;
			case 8: NearestResampler<8>::scale(src, temp); break;
			case 12: NearestResampler<12>::scale(src, temp); break;
			case 16: NearestResampler<16>::scale(src, temp); break;
			default:
				// never reached
				ASSERT(false);
			}
			if(temp.data != scaled.data)
			{
				// Blit temp buffer
				PixelUtil::bulkPixelConversion(temp, scaled);
			}
			break;

		case FILTER_LINEAR:
		case FILTER_BILINEAR:
			switch (src.format) {
			case PF_L8: case PF_A8: case PF_BYTE_LA:
			case PF_R8G8B8: case PF_B8G8R8:
			case PF_R8G8B8A8: case PF_B8G8R8A8:
			case PF_A8B8G8R8: case PF_A8R8G8B8:
			case PF_X8B8G8R8: case PF_X8R8G8B8:
				if(src.format == scaled.format) {
					// No intermediate buffer needed
					temp = scaled;
				}
				else
				{
					// Allocate temp buffer of destination size in source format 
					temp = PixelBox(scaled.getWidth(), scaled.getHeight(), scaled.getDepth(), src.format);
					buf = MemStreamPtr(new xs::MemoryStream(temp.getConsecutiveSize(),0));
					temp.data = buf->getBuffer();
				}
				// super-optimized: uchar-oriented math, no conversion
				switch (PixelUtil::getNumElemBytes(src.format)) {
				case 1: LinearResampler_Byte<1>::scale(src, temp); break;
				case 2: LinearResampler_Byte<2>::scale(src, temp); break;
				case 3: LinearResampler_Byte<3>::scale(src, temp); break;
				case 4: LinearResampler_Byte<4>::scale(src, temp); break;
				default:
					// never reached
					ASSERT(false);
				}
				if(temp.data != scaled.data)
				{
					// Blit temp buffer
					PixelUtil::bulkPixelConversion(temp, scaled);
				}
				break;
			case PF_FLOAT32_RGB:
			case PF_FLOAT32_RGBA:
				if (scaled.format == PF_FLOAT32_RGB || scaled.format == PF_FLOAT32_RGBA)
				{
					// Float32 to Float32, avoid unpack/repack overhead
					LinearResampler_Float32::scale(src, scaled);
					break;
				}
				// else, fall through
			default:
				// non-optimized: Floating-point math, performs conversion but always works
				LinearResampler::scale(src, scaled);
			}
			break;
		default:
			// fall back to old, slow, wildly incorrect DevIL code
#endif
			throw("Scaling algorithm not implemented without DevIL");
#ifdef NEWSCALING
		}
#endif
	}

	//-----------------------------------------------------------------------------    

	ColorValue Image::getColorAt(int x, int y, int z) 
	{
		ColorValue rval = ColorValue(0,0,0,1);
		if(m_uFlags & IF_COMPRESSED)
		{
			return rval;
		}
		PixelUtil::unpackColor(&rval, m_eFormat, &m_pBuffer[m_ucPixelSize * (z * m_uWidth * m_uHeight + m_uWidth * y + x)]);
		return rval;
	}

	//-----------------------------------------------------------------------------    

	PixelBox Image::getPixelBox(uint face, uint mipmap) const
	{
		if(mipmap > getNumMipmaps())
			throw("Mipmap index out of range") ;
		if(face >= getNumFaces())
			throw("Face index out of range");
        // Calculate mipmap offset and size
        uchar *offset = const_cast<uchar*>(getData());
        uint width = getWidth(), height=getHeight(), depth=getDepth();
        uint faceSize; // Size of one face of the image
        for(uint mip=0; mip<mipmap; ++mip)
        {
            faceSize = PixelUtil::getMemorySize(width, height, depth, getFormat());
            /// Skip all faces of this mipmap
            offset += faceSize*getNumFaces(); 
            /// Half size in each dimension
            if(width!=1) width /= 2;
            if(height!=1) height /= 2;
            if(depth!=1) depth /= 2;
        }
		// We have advanced to the desired mipmap, offset to right face
        faceSize = PixelUtil::getMemorySize(width, height, depth, getFormat());
        offset += faceSize*face;
		// Return subface as pixelbox
		PixelBox src(width, height, depth, getFormat(), offset);
		return src;
	}
    //-----------------------------------------------------------------------------
    uint Image::calculateSize(uint mipmaps, uint faces, uint width, uint height, uint depth, 
        PixelFormat format)
    {
        uint size = 0;
        for(uint mip = 0;mip <= mipmaps;++mip)
        {
            size += PixelUtil::getMemorySize(width, height, depth, format) * faces; 
            if(width!=1) width /= 2;
            if(height!=1) height /= 2;
            if(depth!=1) depth /= 2;
        }
        return size;
	}
	
	std::string Image::getFileExt(const std::string& strFilename)
	{
		std::string strExt;

		uint pos = strFilename.find_last_of(".");
		if( pos == std::string::npos )
		{
			//Error("invalid extension.\n");
			return strExt;
		}

		while( pos != strFilename.length() - 1 )
			strExt += strFilename[++pos];

		return strExt;
	}
    
}
