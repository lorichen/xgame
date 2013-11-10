#ifndef     __PIXTextureFont__
#define     __PIXTextureFont__

#include "Font.h"

//点阵字体
namespace xs
{
	class PixTextureFont : public ITextureFont
	{
	public:
		//释放
		virtual void release();
		//升高值
		virtual float Ascender() ;
		//降低值
		virtual float Descender() ;
		//行距
		virtual float LineHeight() ;

		//字符串的包围盒
		virtual void  BBox( const wchar_t* string, float& llx, float& lly, float& llz, float& urx, float& ury, float& urz);

		virtual float Advance( const wchar_t* string);
		//渲染一个字符号串
		virtual void  Render(const wchar_t* c );
		//字符串的包围盒
		void  BBox( const char* string, float& llx, float& lly, float& llz, float& urx, float& ury, float& urz);
		virtual float Advance( const char* string);
		//渲染一个字符号串
		virtual void  Render(const char* string );
		//
		virtual FontRenderDirection setRenderDirection( FontRenderDirection rd);
		//
		virtual float setFontGap( float gap);

	public:
		PixTextureFont();
		bool create(IRenderSystem *pRenderSystem,const wchar_t* szFontName,uint size);
		~PixTextureFont(){};	

	private:

		void drawGlyphToBuffer(CharTextureInfo * pInfo,wchar_t szText,DWORD dwSize);

		//描边效果
		void effectGlyphToBuffer (CharTextureInfo * pInfo,wchar_t  szText,DWORD dwSize);
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		//颜色渐变
		virtual void GradientText(Color color1,Color color2) {}

		//单描边
		virtual void TextOutline(Gdiplus::Color clrOutline, int nThickness) {}

		//双描边
		virtual void TextDblOutline(Gdiplus::Color clrOutline1, 
			Gdiplus::Color clrOutline2, 
			int nThickness1, 
			int nThickness2)
		{
		}

		//发光
		virtual void TextGlow( Gdiplus::Color clrOutline,  int nThickness) {}

		//阴影
		virtual void Shadow( Gdiplus::Color color, int nThickness,Gdiplus::Point ptOffset){}

		//发散的阴影
		virtual void DiffusedShadow(Gdiplus::Color color, int nThickness,Gdiplus::Point ptOffset) {}

		//突出字,阴影在后
		virtual void Extrude(Gdiplus::Color color, int nThickness,Gdiplus::Point ptOffset) {}
#endif

		virtual void  RenderByScale(const wchar_t* c,float fScale) {}

		//  获取渲染时用到的纹理信息
		CharTextureInfo * GetCharTextureInfo(wchar_t wc);

		//预初始化一些常用字
		virtual void InitText(wchar_t *pStr){};

	private:
		//字体信息,名字大小等
		//std::string	m_strFontName;
		std::wstring  m_strFontName;
		uint	m_ui32Size;

		//是否描边
		bool m_bOutline;
#if (TARGET_PLATFORM == PLATFORM_WIN32)
		HDC m_hdc;
		HFONT m_hFont;
		TEXTMETRIC m_textmetric;
		//字体颜色
		Gdiplus::Color fontcolor;
		//描边颜色
		Gdiplus::Color effectcolor;
#endif
	private:
		//保存已渲染过的字符的纹理信息
		typedef std::map<wchar_t,CharTextureInfo>		FontTexture;
		FontTexture m_FontTexture;

		//当前绘制的纹理位置
		uint m_nDrawTextleft;
		uint m_nDrawTextTop;

		//设置各种字体效果

		//颜色渐变用的刷子

		//字符串前面空白的象素
		int m_nBlankPixX,m_nBlankPixY;

	private:
		xs::IVertexBuffer * m_pVB;//positon vertx buffer
		xs::IVertexBuffer * m_pTexVB; //texcoord vertex buffer

	};
}
#endif // __FTTextureFont__


