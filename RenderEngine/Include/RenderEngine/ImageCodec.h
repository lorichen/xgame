#ifndef __ImageCodec_H__
#define __ImageCodec_H__

#include "PixelFormat.h"

#define MAKE556WORD(r,g,b) ((((WORD)(r) >> 3) << 11) | (((WORD)(g) >> 3) << 6)  | ((WORD)(b) >> 2));
#define MAKE565WORD(r,g,b) ((((WORD)(r) >> 3) << 11) | (((WORD)(g) >> 2) << 5)  | ((WORD)(b) >> 3));
#define MAKE555WORD(r,g,b) ((((WORD)(r) >> 3) << 10) | (((WORD)(g) >> 3) << 5)  | ((WORD)(b) >> 3));
#define MAKE1555WORD(a,r,g,b) ((((WORD)(r) >> 3) << 10) | (((WORD)(g) >> 3) << 5)  | ((WORD)(b) >> 3) | ((WORD)(a) << 15));
#define MAKE565RGB(w,r,g,b) (r = (w & 0xF800) >> 8, g = (w & 0x07E0) >> 3, b = (w & 0x001F) << 3); 
#define MAKE555RGB(w,r,g,b) (r = (w & 0x7C00) >> 7, g = (w & 0x03E0) >> 2, b = (w & 0x001F) << 3); 
#define MAKE444RGB(w,r,g,b) (r = (w & 0x0F00) >> 4, g = (w & 0x00F0) >> 0, b = (w & 0x000F) << 4); 

namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	class _RenderEngineExport ImageCodec
	{
		struct ltstr
		{
			bool operator()(const std::string& s1, const std::string& s2) const
			{
				return _stricmp(s1.c_str(), s2.c_str()) < 0;
			}
		};


	public:
		class ImageData
		{
		public:
			ImageData():
			  height(0), width(0), depth(1), size(0),
				  num_mipmaps(0), flags(0), format(PF_UNKNOWN),pData(0),pTransparenceData(0)
			  {
			  }
			  uint height;
			  uint width;
			  uint depth;
			  uint size;

			  ushort num_mipmaps;		//最少有1级Mipmaps
			  uint flags;				//See ImageFlags

			  PixelFormat format;

			  uchar*	pData;
			  uchar*	pTransparenceData;	//透明度数据，记录某个颜色的Alpha值
		};
	private:
		typedef std::map<std::string,ImageCodec*,ltstr> CodecList;
		static CodecList	m_codecs;
	public:
		/** Registers a new codec in the database.
		*/
		static void  registerCodec(ImageCodec*	pCodec)
		{
			std::string strType = pCodec->getType();
			m_codecs[strType] = pCodec;
		}

		/** Unregisters a codec from the database.
		*/
		static void  unregisterCodec(ImageCodec*	pCodec)
		{
			std::string strType = pCodec->getType();
			m_codecs.erase(strType);
		}
		/** Gets the codec registered for the passed in file extension. */
		static ImageCodec* getCodec(const char* extension);
	public:
		/** Returns the type of the codec as a std::string
		*/
		#define ImageCodecInterface(pure) \
			virtual const char*	getType() const ## pure	\
			virtual bool decode(xs::Stream*,ImageData& data,const void  *p1 = 0,const void  *p2 = 0) const ## pure	\
			virtual bool code(void ) const ## pure \
			virtual bool codeToFile(xs::FileStream&,const ImageData& data) const ## pure	\
			virtual bool getDimension(xs::Stream*,uint& width,uint& height,uint& depth) ## pure

		ImageCodecInterface(=0;)
	};
	/** @} */

}
#endif