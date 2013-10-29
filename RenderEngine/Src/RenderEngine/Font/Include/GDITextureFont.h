#ifndef     __GDITextureFont__
#define     __GDITextureFont__
#include "Font.h"

//GDI纹理字体
//支持描边,双重描边,阴影和颜色渐变等效果
// yhc

#if 0  //add by kevin.chen

#include "PngOutlineText.h"
namespace xs
{
	class GDITextureFont :public ITextureFont
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
		GDITextureFont();
		bool	create(IRenderSystem *pRenderSystem,const wchar_t* szFontName,uint size);
		~GDITextureFont() {};	

	private:
		//产生字符图形数据
		bool GenerateTextBitmap(wchar_t * szText);
		//
		bool LoadOneTextBitmap( CharTextureInfo * pInfo,wchar_t * szText );
		//获取空白象素
		bool GetBlankPix( wchar_t * szText );

		//  获取渲染时用到的纹理信息
		CharTextureInfo * GetCharTextureInfo(wchar_t wc);

		//预初始化一些常用字
		virtual void InitText(wchar_t *pStr);

	private:
		//字体信息,名字大小等
		//std::string	m_strFontName;
		std::wstring	m_strFontName;
		uint	m_ui32Size;
		//字体效果增加的行高
		uint    m_nFontThickness;
		//LOGFONT m_LogFont;
		FontFamily * m_pFontFamily;
		//字体绘制对象
		PngOutlineText m_PngOutlineText;
		//生成纹理用
		Gdiplus::Bitmap * m_pbmp;
		//生成纹理用
		Graphics * m_pGraphics;

	private:
		//保存已渲染过的字符的纹理信息
		typedef std::map<wchar_t,CharTextureInfo>		FontTexture;
		FontTexture m_FontTexture;

		//当前绘制的纹理位置
		uint m_nDrawTextleft;
		uint m_nDrawTextTop;

		//设置各种字体效果
		//字体颜色
		Gdiplus::Color fontcolor;
		//颜色渐变用的刷子
		Gdiplus::LinearGradientBrush * m_pGradientBrush;

		//字符串前面空白的象素
		int m_nBlankPixX,m_nBlankPixY;
	public:

		//颜色渐变
		virtual void GradientText(Color color1,Color color2);

		//单描边
		virtual void TextOutline(Gdiplus::Color clrOutline, 
			int nThickness);
		//双描边
		virtual void TextDblOutline(Gdiplus::Color clrOutline1, 
			Gdiplus::Color clrOutline2, 
			int nThickness1, 
			int nThickness2);

		//发光
		virtual void TextGlow(
			Gdiplus::Color clrOutline, 
			int nThickness);
		//阴影
		virtual void Shadow(
			Gdiplus::Color color, 
			int nThickness,
			Gdiplus::Point ptOffset);
		//发散的阴影
		virtual void DiffusedShadow(
			Gdiplus::Color color, 
			int nThickness,
			Gdiplus::Point ptOffset);
		//突出字,阴影在后
		virtual void Extrude(
			Gdiplus::Color color, 
			int nThickness,
			Gdiplus::Point ptOffset);
		virtual void  RenderByScale(const wchar_t* c,float fScale);
	};
}

#endif

#endif // __FTTextureFont__


