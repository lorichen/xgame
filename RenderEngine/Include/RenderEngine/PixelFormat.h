
#ifndef __PixelFormat_H__
#define __PixelFormat_H__

#include "point.h"
#include "rect.h"

namespace xs 
{
	/*! \addtogroup RenderEngine
	*  äÖÈ¾ÒýÇæ
	*  @{
	*/

    /** The pixel format used for images, textures, and render surfaces */
    enum PixelFormat
    {
        /// Unknown pixel format.
        PF_UNKNOWN = 0,
        /// 8-bit pixel format, all bits luminace.
        PF_L8 = 1,
		PF_BYTE_L = PF_L8,
        /// 16-bit pixel format, all bits luminace.
        PF_L16 = 2,
		PF_SHORT_L = PF_L16,
        /// 8-bit pixel format, all bits alpha.
        PF_A8 = 3,
		PF_BYTE_A = PF_A8,
        /// 8-bit pixel format, 4 bits alpha, 4 bits luminace.
        PF_A4L4 = 4,
		/// 2 uchar pixel format, 1 uchar luminance, 1 uchar alpha
		PF_BYTE_LA = 5,
        /// 16-bit pixel format, 5 bits red, 6 bits green, 5 bits blue.
        PF_R5G6B5 = 6,
		/// 16-bit pixel format, 5 bits red, 6 bits green, 5 bits blue.
        PF_B5G6R5 = 7,
        /// 8-bit pixel format, 2 bits blue, 3 bits green, 3 bits red.
        PF_R3G3B2 = 32,
        /// 16-bit pixel format, 4 bits for alpha, red, green and blue.
        PF_A4R4G4B4 = 8,
        /// 16-bit pixel format, 5 bits for blue, green, red and 1 for alpha.
        PF_A1R5G5B5 = 9,
        /// 24-bit pixel format, 8 bits for red, green and blue.
        PF_R8G8B8 = 10,
        /// 24-bit pixel format, 8 bits for blue, green and red.
        PF_B8G8R8 = 11,
        /// 32-bit pixel format, 8 bits for alpha, red, green and blue.
        PF_A8R8G8B8 = 12,
        /// 32-bit pixel format, 8 bits for blue, green, red and alpha.
        PF_A8B8G8R8 = 13,
        /// 32-bit pixel format, 8 bits for blue, green, red and alpha.
        PF_B8G8R8A8 = 14,
		/// 32-bit pixel format, 8 bits for red, green, blue and alpha.
		PF_R8G8B8A8 = 29,
        /// 32-bit pixel format, 8 bits for red, 8 bits for green, 8 bits for blue
        /// like PF_A8R8G8B8, but alpha will get discarded
        PF_X8R8G8B8 = 27,
        /// 32-bit pixel format, 8 bits for blue, 8 bits for green, 8 bits for red
        /// like PF_A8B8G8R8, but alpha will get discarded
        PF_X8B8G8R8 = 28,
#if RE_ENDIAN == RE_ENDIAN_BIG
		/// 3 uchar pixel format, 1 uchar for red, 1 uchar for green, 1 uchar for blue
		PF_BYTE_RGB = PF_R8G8B8,
		/// 3 uchar pixel format, 1 uchar for blue, 1 uchar for green, 1 uchar for red
		PF_BYTE_BGR = PF_B8G8R8,
		/// 4 uchar pixel format, 1 uchar for blue, 1 uchar for green, 1 uchar for red and one uchar for alpha
		PF_BYTE_BGRA = PF_B8G8R8A8,
		/// 4 uchar pixel format, 1 uchar for red, 1 uchar for green, 1 uchar for blue, and one uchar for alpha
		PF_BYTE_RGBA = PF_R8G8B8A8,
#else
		/// 3 uchar pixel format, 1 uchar for red, 1 uchar for green, 1 uchar for blue
		PF_BYTE_RGB = PF_B8G8R8,
		/// 3 uchar pixel format, 1 uchar for blue, 1 uchar for green, 1 uchar for red
		PF_BYTE_BGR = PF_R8G8B8,
		/// 4 uchar pixel format, 1 uchar for blue, 1 uchar for green, 1 uchar for red and one uchar for alpha
		PF_BYTE_BGRA = PF_A8R8G8B8,
		/// 4 uchar pixel format, 1 uchar for red, 1 uchar for green, 1 uchar for blue, and one uchar for alpha
		PF_BYTE_RGBA = PF_A8B8G8R8,
#endif        
        /// 32-bit pixel format, 2 bits for alpha, 10 bits for red, green and blue.
        PF_A2R10G10B10 = 15,
        /// 32-bit pixel format, 10 bits for blue, green and red, 2 bits for alpha.
        PF_A2B10G10R10 = 16,
		/// DDS (DirectDraw Surface) DXT1 format
		PF_RGB_DXT1 = 17,
        /// DDS (DirectDraw Surface) DXT1 format
        PF_DXT1 = 18,
        /// DDS (DirectDraw Surface) DXT2 format
        PF_DXT2 = 19,
        /// DDS (DirectDraw Surface) DXT3 format
        PF_DXT3 = 20,
        /// DDS (DirectDraw Surface) DXT4 format
        PF_DXT4 = 21,
        /// DDS (DirectDraw Surface) DXT5 format
        PF_DXT5 = 22,
		// 16-bit pixel format, 16 bits (float) for red
        PF_FLOAT16_R = 33,
        // 48-bit pixel format, 16 bits (float) for red, 16 bits (float) for green, 16 bits (float) for blue
        PF_FLOAT16_RGB = 23,
        // 64-bit pixel format, 16 bits (float) for red, 16 bits (float) for green, 16 bits (float) for blue, 16 bits (float) for alpha
        PF_FLOAT16_RGBA = 24,
		// 16-bit pixel format, 16 bits (float) for red
        PF_FLOAT32_R = 34,
       // 96-bit pixel format, 32 bits (float) for red, 32 bits (float) for green, 32 bits (float) for blue
        PF_FLOAT32_RGB = 25,
        // 128-bit pixel format, 32 bits (float) for red, 32 bits (float) for green, 32 bits (float) for blue, 32 bits (float) for alpha
        PF_FLOAT32_RGBA = 26,
		// Depth texture format
		PF_DEPTH = 30,
		// 64-bit pixel format, 16 bits for red, green, blue and alpha
		PF_SHORT_RGBA = 31,
		// Number of pixel formats currently defined
        PF_COUNT = 35
    };

    /**
     * Flags defining some on/off properties of pixel formats
     */
    enum PixelFormatFlags {
        // This format has an alpha channel
        PFF_HASALPHA        = 0x00000001,      
        // This format is compressed. This invalidates the values in elemBytes,
        // elemBits and the bit counts as these might not be fixed in a compressed format.
        PFF_COMPRESSED    = 0x00000002,
        // This is a Floating point format
        PFF_FLOAT           = 0x00000004,         
        // This is a depth format (for depth textures)
        PFF_DEPTH           = 0x00000008,
        // Format is in native endian. Generally true for the 16, 24 and 32 bits
        // formats which can be represented as machine integers.
        PFF_NATIVEENDIAN    = 0x00000010,
        // This is an intensity format instead of a RGB one. The luminance
        // replaces R,G and B. (but not A)
        PFF_LUMINANCE       = 0x00000020
    };

	/**
	*/
    /*
	struct _RenderEngineExport Point
	{
		int x,y;

		Point(){}
		Point(int x,int y) : x(x),y(y){}
		Point& operator = (const Point& pt)
		{
			x = pt.x;
			y = pt.y;
			return *this;
		}
		bool operator==(const Point& pt)
		{
			return x == pt.x && y == pt.y;
		}
		bool operator!=(const Point& pt)
		{
			return x != pt.x || y != pt.y;
		}
	};
    */

	/** Structure used to define a rectangle in a 2-D integer space.
	*/
    /*
	struct _RenderEngineExport Rect
	{
		int left, top, right, bottom;

		Rect()
		{
		}
		Rect( int l, int t, int r, int b )
		{
			left = l;
			top = t;   
			right = r;
			bottom = b;                
		}
		Rect& operator = ( const Rect& other )
		{
			left = other.left;
			top = other.top;
			right = other.right;
			bottom = other.bottom;       

			return *this;
		}

		void  moveTo00()
		{
			if(left > right){int t = left - right;left = 0;right = left + t;}
			else {int t = right - left;left = 0;right = left + t;}
			if(top > bottom){int t = top - bottom;top = 0;bottom = top + t;}
			else {int t = bottom - top;top = 0;bottom = top + t;}
		}

		bool operator==(const Rect& other) const
		{
			return left == other.left && right == other.right && top == other.top && bottom == other.bottom;
		}

		bool operator!=(const Rect& other) const
		{
			return !(*this == other);
		}

		int width() const
		{
			return right - left + 1;
		}
		int height() const
		{
			return bottom - top + 1;
		}
		bool intersect(const Rect& rcOther,Rect& rcIntersect) const
		{
			Rect rc = *this;
			if(rc.left < rcOther.left)rc.left = rcOther.left;
			if(rc.top < rcOther.top)rc.top = rcOther.top;
			if(rc.right > rcOther.right)rc.right = rcOther.right;
			if(rc.bottom > rcOther.bottom)rc.bottom = rcOther.bottom;

			if(rc.right < rc.left || rc.bottom < rc.top)return false;

			rcIntersect = rc;
			return true;
		}
		bool ptInRect(const Point& pt)
		{
			return pt.x >= left && pt.x <= right && pt.y >= top && pt.y <= bottom;
		}
	};
    */

	/** Structure used to define a box in a 3-D integer space.
	Note that the left, top, and front edges are included but the right, 
	bottom and top ones are not.
	*/
	struct _RenderEngineExport Box
	{
		uint left, top, right, bottom, front, back;
		/// Parameterless constructor for setting the members manually
		Box()
		{
		}
		/** Define a box from left, top, right and bottom coordinates
		This box will have depth one (front=0 and back=1).
		@param	l	x value of left edge
		@param	t	y value of top edge
		@param	r	x value of right edge
		@param	b	y value of bottom edge
		@note Note that the left, top, and front edges are included 
		but the right, bottom and top ones are not.
		*/
		Box( uint l, uint t, uint r, uint b ):
		left(l),
			top(t),   
			right(r),
			bottom(b),
			front(0),
			back(1)
		{
			ASSERT(right >= left && bottom >= top && back >= front);
		}
		/** Define a box from left, top, front, right, bottom and back
		coordinates.
		@param	l	x value of left edge
		@param	t	y value of top edge
		@param  ff  z value of front edge
		@param	r	x value of right edge
		@param	b	y value of bottom edge
		@param  bb  z value of back edge
		@note Note that the left, top, and front edges are included 
		but the right, bottom and top ones are not.
		*/
		Box( uint l, uint t, uint ff, uint r, uint b, uint bb ):
		left(l),
			top(t),   
			right(r),
			bottom(b),
			front(ff),
			back(bb)
		{
			ASSERT(right >= left && bottom >= top && back >= front);
		}

		/// Return true if the other box is a part of this one
		bool contains(const Box &def) const
		{
			return (def.left >= left && def.top >= top && def.front >= front &&
				def.right <= right && def.bottom <= bottom && def.back <= back);
		}

		/// Get the width of this box
		uint getWidth() const { return right-left; }
		/// Get the height of this box
		uint getHeight() const { return bottom-top; }
		/// Get the depth of this box
		uint getDepth() const { return back-front; }
	};

	/** A primitive describing a volume (3D), image (2D) or line (1D) of pixels in memory.
     	In case of a rectangle, depth must be 1. 
     	Pixels are stored as a succession of "depth" slices, each containing "height" rows of 
     	"width" pixels.
    */
    class _RenderEngineExport PixelBox: public Box {
    public:
    	/// Parameter constructor for setting the members manually
    	PixelBox() {}
		/** Constructor providing extents in the form of a Box object. This constructor
    		assumes the pixel data is laid out consecutively in memory. (this
    		means row after row, slice after slice, with no space in between)
    		@param extents	    Extents of the region defined by data
    		@param pixelFormat	Format of this buffer
    		@param pixelData	Pointer to the actual data
    	*/
		PixelBox(const Box &extents, PixelFormat pixelFormat, void  *pixelData=0):
			Box(extents), data(pixelData), format(pixelFormat)
		{
			setConsecutive();
		}
    	/** Constructor providing width, height and depth. This constructor
    		assumes the pixel data is laid out consecutively in memory. (this
    		means row after row, slice after slice, with no space in between)
    		@param width	    Width of the region
    		@param height	    Height of the region
    		@param depth	    Depth of the region
    		@param pixelFormat	Format of this buffer
    		@param pixelData    Pointer to the actual data
    	*/
    	PixelBox(uint width, uint height, uint depth, PixelFormat pixelFormat, void  *pixelData=0):
    		Box(0, 0, 0, width, height, depth),
    		data(pixelData), format(pixelFormat)
    	{
    		setConsecutive();
    	}
    	
        /// The data pointer 
        void  *data;
        /// The pixel format 
        PixelFormat format;
        /** Number of elements between the leftmost pixel of one row and the left
         	pixel of the next. This value must always be equal to getWidth() (consecutive) 
			for compressed formats.
        */
        uint rowPitch;
        /** Number of elements between the top left pixel of one (depth) slice and 
         	the top left pixel of the next. This can be a negative value. Must be a multiple of
         	rowPitch. This value must always be equal to getWidth()*getHeight() (consecutive) 
			for compressed formats.
        */
        uint slicePitch;
        
        /** Set the rowPitch and slicePitch so that the buffer is laid out consecutive 
         	in memory.
        */        
        void  setConsecutive()
        {
            rowPitch = getWidth();
            slicePitch = getWidth()*getHeight();
        }
        /**	Get the number of elements between one past the rightmost pixel of 
         	one row and the leftmost pixel of the next row. (IE this is zero if rows
         	are consecutive).
        */
        uint getRowSkip() const { return rowPitch - getWidth(); }
        /** Get the number of elements between one past the right bottom pixel of
         	one slice and the left top pixel of the next slice. (IE this is zero if slices
         	are consecutive).
        */
        uint getSliceSkip() const { return slicePitch - (getHeight() * rowPitch); }

        /** Return whether this buffer is laid out consecutive in memory (ie the pitches
         	are equal to the dimensions)
        */        
        bool isConsecutive() const 
		{ 
			return rowPitch == getWidth() && slicePitch == getWidth()*getHeight(); 
		}
        /** Return the size (in bytes) this image would take if it was
        	laid out consecutive in memory
      	*/
      	uint getConsecutiveSize() const;
      	/** Return a subvolume of this PixelBox.
      		@param def	Defines the bounds of the subregion to return
      		@returns	A pixel box describing the region and the data in it
      		@remarks	This function does not copy any data, it just returns
      			a PixelBox object with a data pointer pointing somewhere inside 
      			the data of object.
      		@throws	Exception(ERR_INVALIDPARAMS) if def is not fully contained
      	*/
      	PixelBox getSubVolume(const Box &def) const;
    };
    

    /**
     * Some utility functions for packing and unpacking pixel data
     */
    class _RenderEngineExport PixelUtil {
    public:
        /** Returns the size in bytes of an element of the given pixel format.
         @returns
               The size in bytes of an element. See Remarks.
         @remarks
               Passing PF_UNKNOWN will result in returning a size of 0 bytes.
        */
        static uint getNumElemBytes( PixelFormat format );

        /** Returns the size in bits of an element of the given pixel format.
          @returns
               The size in bits of an element. See Remarks.
           @remarks
               Passing PF_UNKNOWN will result in returning a size of 0 bits.
        */
        static uint getNumElemBits( PixelFormat format );

		/** Returns the size in memory of a region with the given extents and pixel
			format with consecutive memory layout.
			@param width
				The width of the area
			@param height
				The height of the area
			@param depth
				The depth of the area
			@param format
				The format of the area
		  	@returns
		  		The size in bytes
			@remarks
				In case that the format is non-compressed, this simply returns
				width*height*depth*PixelUtil::getNumElemBytes(format). In the compressed
				case, this does serious magic.
		*/
		static uint getMemorySize(uint width, uint height, uint depth, PixelFormat format);
		
        /** Returns the property flags for this pixel format
          @returns
               A bitfield combination of PFF_HASALPHA, PFF_ISCOMPRESSED,
               PFF_FLOAT, PFF_DEPTH, PFF_NATIVEENDIAN, PFF_LUMINANCE
          @remarks
               This replaces the seperate functions for formatHasAlpha, formatIsFloat, ...
        */
        static uint getFlags( PixelFormat format );

        /** Shortcut method to determine if the format has an alpha component */
        static bool hasAlpha(PixelFormat format);
        /** Shortcut method to determine if the format is Floating point */
        static bool isFloatingPoint(PixelFormat format);
        /** Shortcut method to determine if the format is compressed */
        static bool isCompressed(PixelFormat format);
        /** Shortcut method to determine if the format is a depth format. */
        static bool isDepth(PixelFormat format);
        /** Shortcut method to determine if the format is in native endian format. */
        static bool isNativeEndian(PixelFormat format);
        /** Shortcut method to determine if the format is a luminance format. */
        static bool isLuminance(PixelFormat format);
		
		/** Return wether a certain image extent is valid for this image format.
			@param width
				The width of the area
			@param height
				The height of the area
			@param depth
				The depth of the area
			@param format
				The format of the area
			@remarks For non-compressed formats, this is always true. For DXT formats,
			only sizes with a width and height multiple of 4 and depth 1 are allowed.
		*/
		static bool isValidExtent(uint width, uint height, uint depth, PixelFormat format);

        /** Gives the number of bits (RGBA) for a format. See remarks.          
          @remarks      For non-colour formats (dxt, depth) this returns [0,0,0,0].
        */
        static void  getBitDepths(PixelFormat format, int rgba[4]);

		/** Gives the masks for the R, G, B and A component
		  @note			Only valid for native endian formats
        */
        static void  getBitMasks(PixelFormat format, uint rgba[4]);

        /** Gets the name of an image format
        */
        static std::string getFormatName(PixelFormat srcformat);

        /** Returns wether the format can be packed or unpacked with the packColor()
        and unpackColor() functions. This is generally not true for compressed and
        depth formats as they are special. It can only be true for formats with a
        fixed element size.
          @returns 
               true if yes, otherwise false
        */
        static bool isAccessible(PixelFormat srcformat);
        
        /** Pack a colour value to memory
        	@param colour	The colour
        	@param pf		Pixelformat in which to write the colour
        	@param dest		Destination memory location
        */
        static void  packColor(const ColorValue &colour, const PixelFormat pf,  const void * dest);
        /** Pack a colour value to memory
        	@param r,g,b,a	The four colour components, range 0x00 to 0xFF
        	@param pf		Pixelformat in which to write the colour
        	@param dest		Destination memory location
        */
        static void  packColor(const uchar r, const uchar g, const uchar b, const uchar a, const PixelFormat pf,  const void * dest);
         /** Pack a colour value to memory
        	@param r,g,b,a	The four colour components, range 0.0f to 1.0f
        					(an exception to this case exists for Floating point pixel
        					formats, which don't clamp to 0.0f..1.0f)
        	@param pf		Pixelformat in which to write the colour
        	@param dest		Destination memory location
        */
        static void  packColor(const float r, const float g, const float b, const float a, const PixelFormat pf,  const void * dest);

        /** Unpack a colour value from memory
        	@param colour	The colour is returned here
        	@param pf		Pixelformat in which to read the colour
        	@param src		Source memory location
        */
        static void  unpackColor(ColorValue *colour, PixelFormat pf,  const void * src);
        /** Unpack a colour value from memory
        	@param r,g,b,a	The colour is returned here (as uchar)
        	@param pf		Pixelformat in which to read the colour
        	@param src		Source memory location
        	@remarks 	This function returns the colour components in 8 bit precision,
        		this will lose precision when coming from PF_A2R10G10B10 or Floating
        		point formats.  
        */
        static void  unpackColor(uchar *r, uchar *g, uchar *b, uchar *a, PixelFormat pf,  const void * src);
        /** Unpack a colour value from memory
        	@param r,g,b,a	The colour is returned here (as float)
        	@param pf		Pixelformat in which to read the colour
        	@param src		Source memory location
        */
        static void  unpackColor(float *r, float *g, float *b, float *a, PixelFormat pf,  const void * src); 
        
        /** Convert consecutive pixels from one format to another. No dithering or filtering is being done. 
         	Converting from RGB to luminance takes the R channel.  In case the source and destination format match,
         	just a copy is done.
         	@param	src			Pointer to source region
         	@param	srcFormat	Pixel format of source region
         	@param   dst			Pointer to destination region
         	@param	dstFormat	Pixel format of destination region
         */
        static void  bulkPixelConversion(void  *src, PixelFormat srcFormat, void  *dest, PixelFormat dstFormat, uint count);

      	/** Convert pixels from one format to another. No dithering or filtering is being done. Converting
          	from RGB to luminance takes the R channel. 
		 	@param	src			PixelBox containing the source pixels, pitches and format
		 	@param	dst			PixelBox containing the destination pixels, pitches and format
		 	@remarks The source and destination boxes must have the same
         	dimensions. In case the source and destination format match, a plain copy is done.
        */
        static void  bulkPixelConversion(const PixelBox &src, const PixelBox &dst);
    };

	enum ImageFlags
	{
		IF_COMPRESSED = 0x00000001,
		IF_CUBEMAP    = 0x00000002,
		IF_3D_TEXTURE = 0x00000004
	};
	/** @} */
};

#endif
