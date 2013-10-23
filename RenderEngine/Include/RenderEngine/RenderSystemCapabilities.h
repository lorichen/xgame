#ifndef __RenderSystemCapabilities_H__
#define __RenderSystemCapabilities_H__

namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	/// Enum describing the different hardware capabilities we want to check for
	enum Capabilities
	{
		//RSC_MULTITEXTURE          = 0x00000001,
		/// Supporta generating mipmaps in hardware
		RSC_AUTOMIPMAP              = 0x00000002,
		RSC_BLENDING                = 0x00000004,
		/// Supports anisotropic texture filtering
		RSC_ANISOTROPY              = 0x00000008,
		/// Supports fixed-function DOT3 texture blend
		RSC_DOT3                    = 0x00000010,
		/// Supports cube mapping
		RSC_CUBEMAPPING             = 0x00000020,
		/// Supports hardware stencil buffer
		RSC_HWSTENCIL               = 0x00000040,
		/// Supports hardware vertex and index buffers
		RSC_VBO                     = 0x00000080,
		/// Supports vertex programs (vertex shaders)
		RSC_VERTEX_PROGRAM          = 0x00000200,
		/// Supports fragment programs (pixel shaders)
		RSC_FRAGMENT_PROGRAM        = 0x00000400,
		/// ARB Shader
		RSC_LOWLEVEL_SHADER			= RSC_VERTEX_PROGRAM | RSC_FRAGMENT_PROGRAM,
		/// Supports compressed textures
		RSC_TEXTURE_COMPRESSION     = 0x00000800,
		/// Supports compressed textures in the DXT/ST3C formats
		RSC_TEXTURE_COMPRESSION_DXT = 0x00001000,
		/// Supports compressed textures in the VTC format
		RSC_TEXTURE_COMPRESSION_VTC = 0x00002000,
		/// Supports performing a scissor test to exclude areas of the screen
		RSC_SCISSOR_TEST            = 0x00004000,
		/// Supports separate stencil updates for both front and back faces
		RSC_TWO_SIDED_STENCIL       = 0x00008000,
		/// Supports wrapping the stencil value at the range extremeties
		RSC_STENCIL_WRAP            = 0x00010000,
		/// Supports hardware occlusion queries
		RSC_HWOCCLUSION				= 0x00020000,
		/// Supports user clipping planes
		RSC_USER_CLIP_PLANES		= 0x00040000,
		/// Supports the VET_UBYTE4 vertex element type
		RSC_VERTEX_FORMAT_UBYTE4	= 0x00080000,
		/// Supports infinite far plane projection
		RSC_INFINITE_FAR_PLANE      = 0x00100000,
		/// Supports hardware render-to-texture (bigger than framebuffer)
		RSC_HWRENDER_TO_TEXTURE     = 0x00200000,
		/// Supports float textures and render targets
		RSC_TEXTURE_FLOAT           = 0x00400000,
		/// Supports non-power of two textures
		RSC_NON_POWER_OF_2_TEXTURES = 0x00800000,
		/// Supports 3d (volume) textures
		RSC_TEXTURE_3D				= 0x01000000,
		/// Supports HighLevelShaderLanguage
		RSC_HIGHLEVEL_SHADER		= 0x02000000
	};

	/** singleton class for storing the capabilities of the graphics card. 
	@remarks
	This class stores the capabilities of the graphics card.  This
	information is set by the individual render systems.
	*/
	class _RenderEngineExport RenderSystemCapabilities
	{
	private:
		/// The number of world matricies available
		ushort mNumWorldMatrices;
		/// The number of texture units available
		ushort mNumTextureUnits;
		/// The stencil buffer bit depth
		ushort mStencilBufferBitDepth;
		/// The number of matrices available for hardware blending
		ushort mNumVertexBlendMatrices;
		/// Stores the capabilities flags.
		int mCapabilities;
		/// The best vertex program that this card / rendersystem supports
		std::string mMaxVertexProgramVersion;
		/// The best fragment program that this card / rendersystem supports
		std::string mMaxFragmentProgramVersion;
		/// The number of Floating-point constants vertex programs support
		ushort mVertexProgramConstantFloatCount;           
		/// The number of integer constants vertex programs support
		ushort mVertexProgramConstantIntCount;           
		/// The number of Boolean constants vertex programs support
		ushort mVertexProgramConstantBoolCount;           
		/// The number of Floating-point constants fragment programs support
		ushort mFragmentProgramConstantFloatCount;           
		/// The number of integer constants fragment programs support
		ushort mFragmentProgramConstantIntCount;           
		/// The number of Boolean constants fragment programs support
		ushort mFragmentProgramConstantBoolCount;
		/// Max TextureSize
		uint mMaxTextureSize;

		//定义可用的活动定点属性的数量，Oranrktook Min = 16
		uint mMaxVertexAttribs;
		//定义可用于定点着色器的Uniform变量的数量，Oranrktook Min = 512 ???
		uint mMaxVertexUniformComponents;
		//定义可用于Varying变量的浮点数变量的数量，Oranrktook Min = 32
		uint mMaxVaryingFloats;
		//定义可用来从顶点和片元处理器中访问的纹理贴图的硬件单元的总数，Oranrktook Min = 2
		uint mMaxCombinedTextureImageUnits;
		//定义可用来从顶点处理器中访问的纹理贴图的硬件单元的总数，Oranrktook Min = 0
		uint mMaxVertexTextureImageUnits;
		//定义可用的坐标集的数量，Oranrktook Min = 2
		uint mMaxTextureCoords;
		//定义可用于片元着色器Uniform变量的部分(也就是浮点数)的数量，Oranrktook Min = 64
		uint mMaxFragmentUniformComponents;

	public:	
		RenderSystemCapabilities ();
		~RenderSystemCapabilities ();

		void  setNumWorldMatricies(ushort num)
		{
			mNumWorldMatrices = num;
		}

		void  setNumTextureUnits(ushort num)
		{
			mNumTextureUnits = num;
		}

		void  setStencilBufferBitDepth(ushort num)
		{
			mStencilBufferBitDepth = num;
		}

		void  setNumVertexBlendMatrices(ushort num)
		{
			mNumVertexBlendMatrices = num;
		}

		ushort getNumWorldMatricies(void ) const
		{ 
			return mNumWorldMatrices;
		}

		/** Returns the number of texture units the current output hardware
		supports.

		For use in rendering, this determines how many texture units the
		are available for multitexturing (i.e. rendering multiple 
		textures in a single pass). Where a Material has multiple 
		texture layers, it will try to use multitexturing where 
		available, and where it is not available, will perform multipass
		rendering to achieve the same effect.
		*/
		ushort getNumTextureUnits(void ) const
		{
			return mNumTextureUnits;
		}

		/** Determines the bit depth of the hardware accelerated stencil 
		buffer, if supported.
		@remarks
		If hardware stencilling is not supported, the software will
		provide an 8-bit software stencil.
		*/
		ushort getStencilBufferBitDepth(void ) const
		{
			return mStencilBufferBitDepth;
		}

		/** Returns the number of matrices available to hardware vertex 
		blending for this rendering system. */
		ushort numVertexBlendMatrices(void ) const
		{
			return mNumVertexBlendMatrices;
		}

		/** Adds a capability flag to mCapabilities
		*/
		void  setCapability(const Capabilities c) 
		{ 
			mCapabilities |= c;
		}

		/** Checks for a capability
		*/
		inline bool hasCapability(const Capabilities c) const
		{
			if(mCapabilities & c)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		/// Gets the best low-level vertex program version supported
		const std::string& getMaxVertexProgramVersion(void ) const
		{
			return mMaxVertexProgramVersion;
		}
		/// Gets the best fragment program that this card / rendersystem supports
		const std::string& getMaxFragmentProgramVersion(void ) const
		{
			return mMaxFragmentProgramVersion;
		}
		/// The number of Floating-point constants vertex programs support
		ushort getVertexProgramConstantFloatCount(void ) const
		{
			return mVertexProgramConstantFloatCount;           
		}
		/// The number of integer constants vertex programs support
		ushort getVertexProgramConstantIntCount(void ) const
		{
			return mVertexProgramConstantIntCount;           
		}
		/// The number of Boolean constants vertex programs support
		ushort getVertexProgramConstantBoolCount(void ) const
		{
			return mVertexProgramConstantBoolCount;           
		}
		/// The number of Floating-point constants fragment programs support
		ushort getFragmentProgramConstantFloatCount(void ) const
		{
			return mFragmentProgramConstantFloatCount;           
		}
		/// The number of integer constants fragment programs support
		ushort getFragmentProgramConstantIntCount(void ) const
		{
			return mFragmentProgramConstantIntCount;           
		}
		/// The number of Boolean constants fragment programs support
		ushort getFragmentProgramConstantBoolCount(void ) const
		{
			return mFragmentProgramConstantBoolCount;           
		}

		/// sets the best low-level vertex program version supported
		void  setMaxVertexProgramVersion(const std::string& ver)
		{
			mMaxVertexProgramVersion = ver;
		}
		/// sets the best fragment program that this card / rendersystem supports
		void  setMaxFragmentProgramVersion(const std::string& ver)
		{
			mMaxFragmentProgramVersion = ver;
		}
		/// The number of Floating-point constants vertex programs support
		void  setVertexProgramConstantFloatCount(ushort c)
		{
			mVertexProgramConstantFloatCount = c;           
		}
		/// The number of integer constants vertex programs support
		void  setVertexProgramConstantIntCount(ushort c)
		{
			mVertexProgramConstantIntCount = c;           
		}
		/// The number of Boolean constants vertex programs support
		void  setVertexProgramConstantBoolCount(ushort c)
		{
			mVertexProgramConstantBoolCount = c;           
		}
		/// The number of Floating-point constants fragment programs support
		void  setFragmentProgramConstantFloatCount(ushort c)
		{
			mFragmentProgramConstantFloatCount = c;           
		}
		/// The number of integer constants fragment programs support
		void  setFragmentProgramConstantIntCount(ushort c)
		{
			mFragmentProgramConstantIntCount = c;           
		}
		/// The number of Boolean constants fragment programs support
		void  setFragmentProgramConstantBoolCount(ushort c)
		{
			mFragmentProgramConstantBoolCount = c;           
		}

		void  setMaxTextureSize(uint maxTextureSize)
		{
			mMaxTextureSize = maxTextureSize;
		}

		uint getMaxTextureSize()
		{
			return mMaxTextureSize;
		}

		void  setMaxVertexAttribs(uint maxVertexAttribs)
		{
			mMaxVertexAttribs = maxVertexAttribs;
		}

		void  setMaxVertexUniformComponents(uint maxVertexUniformComponents)
		{
			mMaxVertexUniformComponents = maxVertexUniformComponents;
		}

		void  setMaxVaryingFloats(uint maxVaryingFloats)
		{
			mMaxVaryingFloats = maxVaryingFloats;
		}

		void  setMaxCombinedTextureImageUnits(uint maxCombinedTextureImageUnits)
		{
			mMaxCombinedTextureImageUnits = maxCombinedTextureImageUnits;
		}

		void  setMaxVertexTextureImageUnits(uint maxVertexTextureImageUnits)
		{
			mMaxVertexTextureImageUnits = maxVertexTextureImageUnits;
		}

		void  setMaxTextureCoords(uint maxTextureCoords)
		{
			mMaxTextureCoords = maxTextureCoords;
		}

		void  setMaxFragmentUniformComponents(uint maxFragmentUniformComponents)
		{
			mMaxFragmentUniformComponents = maxFragmentUniformComponents;
		}
	};
	/** @} */
};

#endif // __RenderSystemCapabilities__

