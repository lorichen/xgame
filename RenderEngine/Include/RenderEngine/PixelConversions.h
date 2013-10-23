
	/*! \addtogroup RenderEngine
	*  äÖÈ¾ÒýÇæ
	*  @{
	*/

/** Internal include file -- do not use externally */
using namespace xs;

// NB VC6 can't handle these templates
#if RE_COMPILER != RE_COMPILER_MSVC || RE_COMP_VER >= 1300

#define FMTCONVERTERID(from,to) (((from)<<8)|(to))

/**
 * Convert a box of pixel from one type to another. Who needs automatic code 
 * generation when we have C++ templates and the policy design pattern.
 * 
 * @param   U       Policy class to facilitate pixel-to-pixel conversion. This class
 *    has at least two typedefs: SrcType and DstType. SrcType is the source element type,
 *    dstType is the destination element type. It also has a static method, pixelConvert, that
 *    converts a srcType into a dstType.
 */

template <class U> struct PixelBoxConverter 
{
    static const int ID = U::ID;
    static void  conversion(const PixelBox &src, const PixelBox &dst)
    {
        typename U::SrcType *srcptr = static_cast<typename U::SrcType*>(src.data);
        typename U::DstType *dstptr = static_cast<typename U::DstType*>(dst.data);
        const uint srcSliceSkip = src.getSliceSkip();
        const uint dstSliceSkip = dst.getSliceSkip();
        const uint k = src.right - src.left;
        for(uint z=src.front; z<src.back; z++) 
        {
            for(uint y=src.top; y<src.bottom; y++)
            {
                for(uint x=0; x<k; x++)
                {
                    dstptr[x] = U::pixelConvert(srcptr[x]);
                }
                srcptr += src.rowPitch;
                dstptr += dst.rowPitch;
            }
            srcptr += srcSliceSkip;
            dstptr += dstSliceSkip;
        }    
    }
};

template <typename T, typename U, int id> struct PixelConverter {
    static const int ID = id;
    typedef T SrcType;
    typedef U DstType;    
    
    //inline static DstType pixelConvert(const SrcType &inp);
};


/** Type for PF_R8G8B8/PF_B8G8R8 */
struct Col3b {
    Col3b(uint a, uint b, uint c): 
        x((uchar)a), y((uchar)b), z((uchar)c) { }
    uchar x,y,z;
};
/** Type for PF_FLOAT32_RGB */
struct Col3f {
	Col3f(float r, float g, float b):
		r(r), g(g), b(b) { }
	float r,g,b;
};
/** Type for PF_FLOAT32_RGBA */
struct Col4f {
	Col4f(float r, float g, float b, float a):
		r(r), g(g), b(b), a(a) { }
	float r,g,b,a;
};

struct A8R8G8B8toA8B8G8R8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_A8R8G8B8, PF_A8B8G8R8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<16)|(inp&0xFF00FF00)|((inp&0x00FF0000)>>16);
    }
};

struct A8R8G8B8toB8G8R8A8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_A8R8G8B8, PF_B8G8R8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
    }
};

struct A8R8G8B8toR8G8B8A8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_A8R8G8B8, PF_R8G8B8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x00FFFFFF)<<8)|((inp&0xFF000000)>>24);
    }
};

struct A8B8G8R8toA8R8G8B8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_A8B8G8R8, PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<16)|(inp&0xFF00FF00)|((inp&0x00FF0000)>>16);
    }
};

struct A8B8G8R8toB8G8R8A8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_A8B8G8R8, PF_B8G8R8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x00FFFFFF)<<8)|((inp&0xFF000000)>>24);
    }
};

struct A8B8G8R8toR8G8B8A8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_A8B8G8R8, PF_R8G8B8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
    }
};

struct B8G8R8A8toA8R8G8B8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_B8G8R8A8, PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
    }
};

struct B8G8R8A8toA8B8G8R8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_B8G8R8A8, PF_A8B8G8R8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<24)|((inp&0xFFFFFF00)>>8);
    }
};

struct B8G8R8A8toR8G8B8A8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_B8G8R8A8, PF_R8G8B8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x0000FF00)<<16)|(inp&0x00FF00FF)|((inp&0xFF000000)>>16);
    }
};

struct R8G8B8A8toA8R8G8B8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_R8G8B8A8, PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<24)|((inp&0xFFFFFF00)>>8);
    }
};

struct R8G8B8A8toA8B8G8R8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_R8G8B8A8, PF_A8B8G8R8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x000000FF)<<24)|((inp&0x0000FF00)<<8)|((inp&0x00FF0000)>>8)|((inp&0xFF000000)>>24);
    }
};

struct R8G8B8A8toB8G8R8A8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_R8G8B8A8, PF_B8G8R8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x0000FF00)<<16)|(inp&0x00FF00FF)|((inp&0xFF000000)>>16);
    }
};

struct A8B8G8R8toL8: public PixelConverter <uint, uchar, FMTCONVERTERID(PF_A8B8G8R8, PF_L8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return (uchar)(inp&0x000000FF);
    }
};

struct L8toA8B8G8R8: public PixelConverter <uchar, uint, FMTCONVERTERID(PF_L8, PF_A8B8G8R8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return 0xFF000000|(((uint)inp)<<0)|(((uint)inp)<<8)|(((uint)inp)<<16);
    }
};

struct A8R8G8B8toL8: public PixelConverter <uint, uchar, FMTCONVERTERID(PF_A8R8G8B8, PF_L8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return (uchar)((inp&0x00FF0000)>>16);
    }
};

struct L8toA8R8G8B8: public PixelConverter <uchar, uint, FMTCONVERTERID(PF_L8, PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return 0xFF000000|(((uint)inp)<<0)|(((uint)inp)<<8)|(((uint)inp)<<16);
    }
};

struct B8G8R8A8toL8: public PixelConverter <uint, uchar, FMTCONVERTERID(PF_B8G8R8A8, PF_L8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return (uchar)((inp&0x0000FF00)>>8);
    }
};

struct L8toB8G8R8A8: public PixelConverter <uchar, uint, FMTCONVERTERID(PF_L8, PF_B8G8R8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return 0x000000FF|(((uint)inp)<<8)|(((uint)inp)<<16)|(((uint)inp)<<24);
    }
};

struct L8toL16: public PixelConverter <uchar, ushort, FMTCONVERTERID(PF_L8, PF_L16)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return (ushort)((((uint)inp)<<8)|(((uint)inp)));
    }
};

struct L16toL8: public PixelConverter <ushort, uchar, FMTCONVERTERID(PF_L16, PF_L8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return (uchar)(inp>>8);
    }
};

struct R8G8B8toB8G8R8: public PixelConverter <Col3b, Col3b, FMTCONVERTERID(PF_R8G8B8, PF_B8G8R8)>
{
    inline static DstType pixelConvert(const SrcType &inp)
    {
        return Col3b(inp.z, inp.y, inp.x);
    }  
};

struct B8G8R8toR8G8B8: public PixelConverter <Col3b, Col3b, FMTCONVERTERID(PF_B8G8R8, PF_R8G8B8)>
{
    inline static DstType pixelConvert(const SrcType &inp)
    {
        return Col3b(inp.z, inp.y, inp.x);
    }  
};

// X8Y8Z8 ->  X8<<xshift Y8<<yshift Z8<<zshift A8<<ashift
template <int id, uint xshift, uint yshift, uint zshift, uint ashift> struct Col3btoUint32swizzler:
    public PixelConverter <Col3b, uint, id>
{
    inline static uint pixelConvert(const Col3b &inp)
    {
#if RE_ENDIAN == RE_ENDIAN_BIG
        return (0xFF<<ashift) | (((uint)inp.x)<<xshift) | (((uint)inp.y)<<yshift) | (((uint)inp.z)<<zshift);
#else
        return (0xFF<<ashift) | (((uint)inp.x)<<zshift) | (((uint)inp.y)<<yshift) | (((uint)inp.z)<<xshift);
#endif
    }
};

struct R8G8B8toA8R8G8B8: public Col3btoUint32swizzler<FMTCONVERTERID(PF_R8G8B8, PF_A8R8G8B8), 16, 8, 0, 24> { };
struct B8G8R8toA8R8G8B8: public Col3btoUint32swizzler<FMTCONVERTERID(PF_B8G8R8, PF_A8R8G8B8), 0, 8, 16, 24> { };
struct R8G8B8toA8B8G8R8: public Col3btoUint32swizzler<FMTCONVERTERID(PF_R8G8B8, PF_A8B8G8R8), 0, 8, 16, 24> { };
struct B8G8R8toA8B8G8R8: public Col3btoUint32swizzler<FMTCONVERTERID(PF_B8G8R8, PF_A8B8G8R8), 16, 8, 0, 24> { };
struct R8G8B8toB8G8R8A8: public Col3btoUint32swizzler<FMTCONVERTERID(PF_R8G8B8, PF_B8G8R8A8), 8, 16, 24, 0> { };
struct B8G8R8toB8G8R8A8: public Col3btoUint32swizzler<FMTCONVERTERID(PF_B8G8R8, PF_B8G8R8A8), 24, 16, 8, 0> { };

struct A8R8G8B8toR8G8B8: public PixelConverter <uint, Col3b, FMTCONVERTERID(PF_A8R8G8B8, PF_BYTE_RGB)>
{
    inline static DstType pixelConvert(uint inp)
    {
        return Col3b((uchar)(inp>>16)&0xFF, (uchar)(inp>>8)&0xFF, (uchar)(inp>>0)&0xFF);
    }
};
struct A8R8G8B8toB8G8R8: public PixelConverter <uint, Col3b, FMTCONVERTERID(PF_A8R8G8B8, PF_BYTE_BGR)>
{
    inline static DstType pixelConvert(uint inp)
    {
        return Col3b((uchar)(inp>>0)&0xFF, (uchar)(inp>>8)&0xFF, (uchar)(inp>>16)&0xFF);
    }
};

// Only conversions from X8R8G8B8 to formats with alpha need to be defined, the rest is implicitly the same
// as A8R8G8B8
struct X8R8G8B8toA8R8G8B8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_X8R8G8B8, PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return inp | 0xFF000000;
    }
};
struct X8R8G8B8toA8B8G8R8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_X8R8G8B8, PF_A8B8G8R8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x0000FF)<<16)|((inp&0xFF0000)>>16)|(inp&0x00FF00)|0xFF000000;
    }
};
struct X8R8G8B8toB8G8R8A8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_X8R8G8B8, PF_B8G8R8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x0000FF)<<24)|((inp&0xFF0000)>>8)|((inp&0x00FF00)<<8)|0x000000FF;
    }
};
struct X8R8G8B8toR8G8B8A8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_X8R8G8B8, PF_R8G8B8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0xFFFFFF)<<8)|0x000000FF;
    }
};

// X8B8G8R8
struct X8B8G8R8toA8R8G8B8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_X8B8G8R8, PF_A8R8G8B8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x0000FF)<<16)|((inp&0xFF0000)>>16)|(inp&0x00FF00)|0xFF000000;
    }
};
struct X8B8G8R8toA8B8G8R8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_X8B8G8R8, PF_A8B8G8R8)>
{
	inline static DstType pixelConvert(SrcType inp)
    {
        return inp | 0xFF000000;
    }
};
struct X8B8G8R8toB8G8R8A8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_X8B8G8R8, PF_B8G8R8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0xFFFFFF)<<8)|0x000000FF;
    }
};
struct X8B8G8R8toR8G8B8A8: public PixelConverter <uint, uint, FMTCONVERTERID(PF_X8B8G8R8, PF_R8G8B8A8)>
{
    inline static DstType pixelConvert(SrcType inp)
    {
        return ((inp&0x0000FF)<<24)|((inp&0xFF0000)>>8)|((inp&0x00FF00)<<8)|0x000000FF;
    }
};


#define CASECONVERTER(type) case type::ID : PixelBoxConverter<type>::conversion(src, dst); return 1;

inline int doOptimizedConversion(const PixelBox &src, const PixelBox &dst)
{;
    switch(FMTCONVERTERID(src.format, dst.format))
    {
        // Register converters here
		CASECONVERTER(A8R8G8B8toA8B8G8R8);
		CASECONVERTER(A8R8G8B8toB8G8R8A8);
		CASECONVERTER(A8R8G8B8toR8G8B8A8);
		CASECONVERTER(A8B8G8R8toA8R8G8B8);
		CASECONVERTER(A8B8G8R8toB8G8R8A8);
		CASECONVERTER(A8B8G8R8toR8G8B8A8);
		CASECONVERTER(B8G8R8A8toA8R8G8B8);
		CASECONVERTER(B8G8R8A8toA8B8G8R8);
		CASECONVERTER(B8G8R8A8toR8G8B8A8);
		CASECONVERTER(R8G8B8A8toA8R8G8B8);
		CASECONVERTER(R8G8B8A8toA8B8G8R8);
		CASECONVERTER(R8G8B8A8toB8G8R8A8);
        CASECONVERTER(A8B8G8R8toL8);
        CASECONVERTER(L8toA8B8G8R8);
        CASECONVERTER(A8R8G8B8toL8);
        CASECONVERTER(L8toA8R8G8B8);
        CASECONVERTER(B8G8R8A8toL8);
        CASECONVERTER(L8toB8G8R8A8);
        CASECONVERTER(L8toL16);
        CASECONVERTER(L16toL8);
        CASECONVERTER(B8G8R8toR8G8B8);
        CASECONVERTER(R8G8B8toB8G8R8);
        CASECONVERTER(R8G8B8toA8R8G8B8);
        CASECONVERTER(B8G8R8toA8R8G8B8);
        CASECONVERTER(R8G8B8toA8B8G8R8);
        CASECONVERTER(B8G8R8toA8B8G8R8);
        CASECONVERTER(R8G8B8toB8G8R8A8);
        CASECONVERTER(B8G8R8toB8G8R8A8);
		CASECONVERTER(A8R8G8B8toR8G8B8);
		CASECONVERTER(A8R8G8B8toB8G8R8);
		CASECONVERTER(X8R8G8B8toA8R8G8B8);
		CASECONVERTER(X8R8G8B8toA8B8G8R8);
		CASECONVERTER(X8R8G8B8toB8G8R8A8);
		CASECONVERTER(X8R8G8B8toR8G8B8A8);
		CASECONVERTER(X8B8G8R8toA8R8G8B8);
		CASECONVERTER(X8B8G8R8toA8B8G8R8);
		CASECONVERTER(X8B8G8R8toB8G8R8A8);
		CASECONVERTER(X8B8G8R8toR8G8B8A8);

        default:
            return 0;
    }
}
#undef CASECONVERTER

#endif // VC6 protection

	/** @} */