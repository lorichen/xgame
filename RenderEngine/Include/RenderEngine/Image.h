#ifndef __Image_H__
#define __Image_H__

#include "PixelFormat.h"

namespace xs
{
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/

    /** Class representing an image file.
        @remarks
            The Image class usually holds uncompressed image data and is the
            only object that can be loaded in a texture. Image  objects handle 
            image data decoding themselves by the means of locating the correct 
            ImageCodec object for each data type.
        @par
            Typically, you would want to use an Image object to load a texture
            when extra processing needs to be done on an image before it is
            loaded or when you want to blit to an existing texture.
    */
    class _RenderEngineExport Image
    {
    public:
    	typedef xs::Box Box;
    	typedef xs::Rect Rect;
    public:
        /** Standard constructor.
        */
        Image();
        /** Copy-constructor - copies all the data from the target image.
        */
        Image( const Image &img );

        /** Standard destructor.
        */
        ~Image();

        /** Assignment operator - copies all the data from the target image.
        */
        Image & operator = ( const Image & img );

        /** Flips (mirrors) the image around the Y-axis. 
            @remarks
                An example of an original and flipped image:
                <pre>                
                originalimg
                00000000000
                00000000000
                00000000000
                00000000000
                00000000000
                ------------> flip axis
                00000000000
                00000000000
                00000000000
                00000000000
                00000000000
                originalimg
                </pre>
        */
        void  flipAroundY();

        /** Flips (mirrors) the image around the X-axis.
            @remarks
                An example of an original and flipped image:
                <pre>
                        flip axis
                            |
                originalimg|gmilanigiro
                00000000000|00000000000
                00000000000|00000000000
                00000000000|00000000000
                00000000000|00000000000
                00000000000|00000000000
                </pre>
        */                 
        void  flipAroundX();

        /** Stores a pointer to raw data in memory. The pixel format has to be specified.
            @remarks
                This method loads an image into memory held in the object. The 
                pixel format will be either greyscale or RGB with an optional
                Alpha component.
                The type can be determined by calling getFormat().             
            @param
                The data pointer
            @param
				Width of image
            @param
				Height of image
			@param
                Image Depth (in 3d images, numbers of layers, otherwhise 1)
            @param
				Pixel Format
            @param
                if memory associated with this buffer is to be destroyed
                with the Image object.
			@param
				the number of faces the image data has inside (6 for cubemaps, 1 otherwise)
            @param
                the number of mipmaps the image data has inside
            @note
                 The memory associated with this buffer is NOT destroyed with the
                 Image object, unless autoDelete is set to true.
			@remarks 
				The size of the buffer must be numFaces*PixelUtil::getMemorySize(width, height, depth, format)
         */
		bool loadDynamicImage( uchar* pData, uint uWidth, uint uHeight,
							uint depth,
							 PixelFormat eFormat, bool autoDelete = false, 
							 uint numFaces = 1, uint numMipMaps = 0);
		
		/** Stores a pointer to raw data in memory. The pixel format has to be specified.
            @remarks
                This method loads an image into memory held in the object. The 
                pixel format will be either greyscale or RGB with an optional
                Alpha component.
                The type can be determined by calling getFormat().             
            @param
                The data pointer
            @param
				Width of image
            @param
				Height of image
            @param
				Pixel Format
            @note
                 The memory associated with this buffer is NOT destroyed with the
                 Image object.
			@remarks This function is deprecated; one should really use the
				Image::loadDynamicImage(pData, width, height, depth, format, ...) to be compatible
				with future rkt versions.
         */
 		bool loadDynamicImage( uchar* pData, uint uWidth,
								 uint uHeight, PixelFormat eFormat)
		{
			return loadDynamicImage(pData, uWidth, uHeight, 1, eFormat);
		}
		/** Loads raw data from a stream. See the function
			loadDynamicImage for a description of the parameters.
			@remarks 
				The size of the buffer must be numFaces*PixelUtil::getMemorySize(width, height, depth, format)
        */
        bool loadRawData( 
            xs::Stream* stream, 
            uint uWidth, uint uHeight, uint uDepth,
            PixelFormat eFormat,
			uint numFaces = 1, uint numMipMaps = 0);
        /** Loads raw data from a stream. The pixel format has to be specified. 
			@remarks This function is deprecated; one should really use the
				Image::loadRawData(stream, width, height, depth, format, ...) to be compatible
				with future rkt versions.
        */
        bool loadRawData( 
            xs::Stream* stream, 
            uint uWidth, uint uHeight, 
            PixelFormat eFormat )
		{
			return loadRawData(stream, uWidth, uHeight, 1, eFormat);
		}

        /** Loads an image file.
            @remarks
                This method loads an image into memory held in the object. The 
                pixel format will be either greyscale or RGB with an optional
                Alpha component.
                The type can be determined by calling getFormat().             
            @param
                strFileName Name of a file file to load.
            @note
                The memory associated with this buffer is destroyed with the
                Image object.
        */
        bool load( const std::string& strFileName);

        /** Loads an image file from a stream.
            @remarks
                This method works in the same way as the filename-based load 
                method except it loads the image from a DataStream object. 
				This DataStream is expected to contain the 
                encoded data as it would be held in a file. 
            @param
                stream The source data.
            @param
                type The type of the image. Used to decide what decompression
                codec to use.
			@param
				p1 the param1
			@param
				p2 the param2
            @see
                Image::load( const std::string& strFileName )
        */
        bool load(xs::Stream* stream, const std::string& type,const void  *p1 = 0,const void  *p2 = 0);

		/** Get image width & height & depth from a stream without load it.
		*/
		static bool getDimension(xs::Stream* stream,const std::string& type,uint& width,uint& height,uint& depth);

		/** Get image dimensions from file without load it.
		*/
		static bool getDimension(const std::string& strFileName,uint& width,uint& height,uint& depth);

        /** Save the image as a file. */
        bool save(const std::string& filename);

        /** Returns a pointer to the internal image buffer.
        */
        uchar* getData(void );

        /** Returns a const pointer to the internal image buffer.
        */
        const uchar * getData() const;   

		const uchar * getData() volatile;

        /** Returns the size of the data buffer.
        */
        uint getSize() const;

		/** Returns the size of one line
		*/
		uint getLineSize() const;
		/** Returns the size of one pixel
		*/
		uchar getPixelSize() const;

        /** Returns the number of mipmaps contained in the image.
        */
        uint getNumMipmaps() const;

        /** Returns true if the image has the appropriate flag set.
        */
        bool hasFlag(const ImageFlags imgFlag) const;

        /** Gets the width of the image in pixels.
        */
        uint getWidth(void ) const;

        /** Gets the height of the image in pixels.
        */
        uint getHeight(void ) const;

        /** Gets the depth of the image.
        */
        uint getDepth(void ) const;
		
		/** Get the numer of faces of the image. This is usually 6 for a cubemap, and
		    1 for a normal image.
		*/
		uint getNumFaces(void ) const;

        /** Gets the physical width in bytes of each row of pixels.
        */
        uint getRowSpan(void ) const;

        /** Returns the image format.
        */
        PixelFormat getFormat() const;

        /** Returns the number of bits per pixel.
        */
        uchar getBPP() const;

		/** Returns Flags
		*/
		int	getFlags() const;

        /** Returns true if the image has an alpha component.
        */
        bool getHasAlpha() const;

		/** Build Image from image's rc area
		 */
		bool subImage(const Image& image,const Rect& rc);

		/** SetColorKey,the alpha will be 
		*/
		bool setColorKey(const ColorValue& color);
		/** Does gamma adjustment.
            @note
                Basic algo taken from Titan Engine, copyright (c) 2000 Ignacio 
                Castano Iguado
        */
        static void  applyGamma( uchar *buffer, float gamma, uint size, uchar bpp );

        /**
         * Get colour value from a certain location in the image. The z coordinate
         * is only valid for cubemaps and volume textures. This uses the first (largest)
         * mipmap.
         */
        ColorValue getColorAt(int x, int y, int z);
        
        /**
         * Get a PixelBox encapsulating the image data of a mipmap
         */
        PixelBox getPixelBox(uint face = 0, uint mipmap = 0) const;

		enum Filter
		{
			FILTER_NEAREST,
			FILTER_LINEAR,
			FILTER_BILINEAR,
			FILTER_BOX,
			FILTER_TRIANGLE,
			FILTER_BICUBIC
		};
		/** Scale a 1D, 2D or 3D image volume. 
			@param 	src			PixelBox containing the source pointer, dimensions and format
			@param 	dst			PixelBox containing the destination pointer, dimensions and format
			@param 	filter		Which filter to use
			@remarks 	This function can do pixel format conversion in the process.
			@note	dst and src can point to the same PixelBox object without any problem
		*/
		static void  scale(const PixelBox &src, const PixelBox &dst, Filter filter = FILTER_BILINEAR);
		
		/** Resize a 2D image, applying the appropriate filter. */
		void  resize(ushort width, ushort height, Filter filter = FILTER_BILINEAR);
		
        // Static function to calculate size in bytes from the number of mipmaps, faces and the dimensions
        static uint calculateSize(uint mipmaps, uint faces, uint width, uint height, uint depth, PixelFormat format);
    private:
        // The width of the image in pixels
        uint m_uWidth;
        // The height of the image in pixels
        uint m_uHeight;
        // The depth of the image
        uint m_uDepth;
        // The size of the image buffer
        uint m_uSize;
        // The number of mipmaps the image contains
        uint m_uNumMipmaps;
        // Image specific flags.
        int m_uFlags;

        // The pixel format of the image
        PixelFormat m_eFormat;

        // The number of bytes per pixel
        uchar m_ucPixelSize;
        uchar* m_pBuffer;

		// A bool to determine if we delete the buffer or the calling app does
		bool m_bAutoDelete;
	private:
		static std::string getFileExt(const std::string& strFilename);
    };
	/** @} */

} // namespace

#endif
