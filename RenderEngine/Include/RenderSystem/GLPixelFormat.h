#ifndef __GLPixelFormat_H__
#define __GLPixelFormat_H__

namespace xs
{
	/**
	* Class to do pixel format mapping between GL and RenderEngine
	*/
	class GLPixelUtil 
	{
	public:
		/** Takes the rkt pixel format and returns the appropriate GL one
			@returns a GLenum describing the format, or 0 if there is no exactly matching 
			one (and conversion is needed)
		*/
		static GLenum getGLOriginFormat(PixelFormat mFormat);
	
		/** Takes the rkt pixel format and returns type that must be provided
			to GL as data type for reading it into the GPU
			@returns a GLenum describing the data type, or 0 if there is no exactly matching 
			one (and conversion is needed)
		*/
		static GLenum getGLOriginDataType(PixelFormat mFormat);
	
		/**	Takes the rkt pixel format and returns the type that must be provided
			to GL as internal format.
		*/
		static GLenum getClosestGLInternalFormat(PixelFormat mFormat);
		
		/**	Function to get the closest matching rkt format to an internal GL format. To be
			precise, the format will be chosen that is most efficient to transfer to the card 
			without losing precision.
			@remarks It is valid for this function to always return PF_A8R8G8B8.
		*/
		static PixelFormat getClosestREFormat(GLenum fmt);
	
		/** Returns the maximum number of Mipmaps that can be generated until we reach
			the mininum format possible. This does not count the base level.
			@param width
				The width of the area
			@param height
				The height of the area
			@param depth
				The depth of the area
			@param format
				The format of the area
			@remarks
				In case that the format is non-compressed, this simply returns
				how many times we can divide this texture in 2 until we reach 1x1.
				For compressed formats, constraints apply on minimum size and alignment
				so this might differ.
		*/
		static size_t getMaxMipmaps(size_t width, size_t height, size_t depth, PixelFormat format);
	};
};

#endif
