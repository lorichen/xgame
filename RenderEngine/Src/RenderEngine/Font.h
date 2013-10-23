#ifndef __Font_H__
#define __Font_H__

namespace xs
{
	class GDITextureFont;
	class FontManager;
	struct MyVector2
	{
		float X;
		float Y;
	};
	struct CharTextureInfo
	{
		//纹理数组索引
		int nTexIndex;

		//这个字在纹理中的区域信息
		uint left;
		uint top;
		uint width;
		uint height;
		MyVector2 uv[2];
	};

	//一个字的渲染信息
	struct CharRenderInfo
	{
		CharTextureInfo  * pInfo;
		float left;
		float top;
		ColorValue color;
		float scale;
	};
	//渲染的时候用,在同一张纹理里面的字放到一起渲染,提高效率
	typedef  std::vector<CharRenderInfo> CharInfoForTex;

	struct ITextureFont
	{
		//释放
		virtual void release() = 0;
		//升高值
		virtual float Ascender() = 0;
		//降低值
		virtual float Descender() = 0;
		//行距
		virtual float LineHeight() = 0;

		//字符串的包围盒
		virtual void  BBox( const wchar_t* string, float& llx, float& lly, float& llz, float& urx, float& ury, float& urz)= 0;

		virtual float Advance( const wchar_t* string)= 0;
		//渲染一个字符号串
		virtual void  Render(const wchar_t* c )= 0;
		//渲染一个字符号串
		virtual void  RenderByScale(const wchar_t* c,float fScale)= 0;
		//字符串的包围盒
		virtual void  BBox( const char* string, float& llx, float& lly, float& llz, float& urx, float& ury, float& urz)= 0;
		virtual float Advance( const char* string)= 0;
		//渲染一个字符号串
		virtual void  Render(const char* string )= 0;



		virtual FontRenderDirection setRenderDirection( FontRenderDirection rd)= 0;
		virtual float setFontGap( float gap)= 0;

		//颜色渐变
		virtual void GradientText(Color color1,Color color2)= 0;

		//单描边
		virtual void TextOutline(Gdiplus::Color clrOutline, 
			int nThickness)= 0;
		//双描边
		virtual void TextDblOutline(Gdiplus::Color clrOutline1, 
			Gdiplus::Color clrOutline2, 
			int nThickness1, 
			int nThickness2)= 0;

		//发光
		virtual void TextGlow(
			Gdiplus::Color clrOutline, 
			int nThickness)= 0;
		//阴影
		virtual void Shadow(
			Gdiplus::Color color, 
			int nThickness,
			Gdiplus::Point ptOffset)= 0;
		//发散的阴影
		virtual void DiffusedShadow(
			Gdiplus::Color color, 
			int nThickness,
			Gdiplus::Point ptOffset)= 0;
		//突出字,阴影在后
		virtual void Extrude(
			Gdiplus::Color color, 
			int nThickness,
			Gdiplus::Point ptOffset)= 0;

		//预初始化一些常用字
		virtual void InitText(wchar_t *pStr) = 0;
	
		//本字体用到的纹理数组
		std::vector<ITexture*> vTextures;
		//渲染的时候用,在同一张纹理里面的字放到一起渲染,提高效率
		std::vector<CharInfoForTex> vCharInfoForTex;
		IRenderSystem*		m_pRenderSystem;
		ITextureManager*	m_pTextureManager;
		//真正的渲染
		virtual void RealRender()
		{
			SceneBlendFactor src,dst;
			m_pRenderSystem->getSceneBlending(src,dst);
			m_pRenderSystem->setSceneBlending(SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);
			ColorValue oldcolor = m_pRenderSystem->getColor();
			for(int i=0;i<vCharInfoForTex.size();i++)
			{
				m_pRenderSystem->setTexture(0,vTextures[i]);
				//m_pRenderSystem->beginPrimitive(PT_TRIANGLES);

				m_pRenderSystem->beginPrimitive(PT_QUADS);
				
				//在同一张纹理内,放到一起去渲染
				for(int j=0;j<vCharInfoForTex[i].size();j++)
				{
					//超过了渲染系统的最大容量
					// by yhc 12.12
					if(j>512)
					{
						m_pRenderSystem->endPrimitive();
						m_pRenderSystem->beginPrimitive(PT_QUADS);
					}
					CharRenderInfo * pRenderInfo = &( vCharInfoForTex[i][j]);
					CharTextureInfo * pInfo = pRenderInfo->pInfo;
					m_pRenderSystem->setColor(pRenderInfo->color);
					m_pRenderSystem->setTexcoord(Vector2(pInfo->uv[0].X, pInfo->uv[0].Y));
					m_pRenderSystem->sendVertex(Vector2(pRenderInfo->left, pRenderInfo->top));

					m_pRenderSystem->setTexcoord(Vector2(pInfo->uv[0].X, pInfo->uv[1].Y));
					m_pRenderSystem->sendVertex(Vector2(pRenderInfo->left, pRenderInfo->top+pInfo->height*pRenderInfo->scale ));

					m_pRenderSystem->setTexcoord(Vector2(pInfo->uv[1].X, pInfo->uv[1].Y));
					m_pRenderSystem->sendVertex(Vector2(pRenderInfo->left+pInfo->width*pRenderInfo->scale, pRenderInfo->top+pInfo->height*pRenderInfo->scale ));

					m_pRenderSystem->setTexcoord(Vector2(pInfo->uv[1].X, pInfo->uv[0].Y));
					m_pRenderSystem->sendVertex(Vector2(pRenderInfo->left+pInfo->width*pRenderInfo->scale, pRenderInfo->top));

					
				}

				m_pRenderSystem->endPrimitive();
				vCharInfoForTex[i].clear();
				m_pRenderSystem->setColor(oldcolor);
			}
		}
	};

	class Font : public IFont
	{
	public:
		virtual void release();
		/** 绘制文字
		@param pText Unicode文字信息
		*/
		virtual void  render(const wchar_t* pText);										

		/** 绘制文字
		@param pText Ascii文字信息
		*/
		virtual void  render(const char* pText);											

		/** 2D状态下在视口某位置绘制文字
		@param xx x坐标
		@param yy y坐标
		@param zz z坐标
		@param color 颜色
		@param pText Ascii文字信息
		*/
		virtual void  render2d(float xx,float yy,float zz,const ColorValue& color,const char* pText);

		/** 2D状态下在视口某位置绘制文字
		@param xx x坐标
		@param yy y坐标
		@param zz z坐标
		@param color 颜色
		@param pText Unicode文字信息
		*/
		virtual void  render2d(float xx,float yy,float zz,const ColorValue& color,const wchar_t* pText);

		/** 获得字体名称
		@return 字体名称，如"宋体"
		*/
		virtual const char*	getFontName();													

		/** 获得字体大小
		@return 字体大小
		*/
		virtual uint	getFontSize();													

		/** 获得字体Ascender
		@return 字体Ascender
		*/
		virtual float getAscender() const;												

		/** 获得字体Descender
		@return 字体Descender
		*/
		virtual float getDescender() const;												

		/** 获得字体LineHeight
		@return 字体LineHeight
		*/
		virtual float getLineHeight() const;												

		/** 获得字体AABB的Ascii版本
		@return 字体AABB
		*/
		virtual void  getAABB( const char* string, AABB& aabb);							

		/** 获得字体AABB的Unicode版本
		@return 字体AABB
		*/
		virtual void  getAABB( const wchar_t* string, AABB &aabb);						

		/** 获得字体Advance的Unicode版本
		@return 字体Advance
		*/
		virtual float getAdvance( const wchar_t* string);								

		/** 获得字体Advance的Ascii版本
		@return 字体Advance
		*/
		virtual float getAdvance( const char* string);									

		/** 获得字体Advance的Unicode版本
		@return 字体Advance
		*/
		virtual float getAdvance( const wchar_t c);										

		/** 获得字体Advance的Ascii版本
		@return 字体Advance
		*/
		virtual float getAdvance( const char c);

		/**设置文字的绘制方向
		@return 旧的绘制方向
		*/
		virtual FontRenderDirection setRenderDirection( FontRenderDirection rd);

		/**设置文字的间距
		@return 旧的间距
		*/
		virtual float setFontGap( float gap);

		//add by yhc
		//文字效果
		//颜色渐变
		virtual void GradientText(ColorValue& color1,ColorValue& color2) ;
		//单描边
		virtual void TextOutline(ColorValue& clrOutline, int nThickness);
		//双描边
		virtual void TextDblOutline(ColorValue& clrOutline1, ColorValue& clrOutline2, int nThickness1, int nThickness2);
		//发光
		virtual void TextGlow(ColorValue& clrOutline, int nThickness);
		//阴影
		virtual void Shadow(ColorValue& color, int nThickness,int nOffsetX,int nOffsetY);
		//发散的阴影
		virtual void DiffusedShadow(ColorValue& color, int nThickness,int nOffsetX,int nOffsetY);
		//突出字,阴影在后
		virtual void Extrude(ColorValue& color, int nThickness,int nOffsetX,int nOffsetY);

		virtual void  render2dByScale(float xx,float yy,float zz,const ColorValue& color,const wchar_t* pText,float fScale);

		//预初始化一些常用字
		virtual void InitText(wchar_t *pStr);
	public:
		~Font(){};
		Font():m_pFTFont(0),
			m_name(),
			m_strFontName(),
			m_ui32Size(16),
			m_pFontData(0),
			m_pRenderSystem(0),
			m_pFontManager(0),
			m_type(FontType_PIX){};

		bool create(FontManager* pFontMgr, 
			IRenderSystem* pRenderSystem,
			const std::string & name, 
			const std::string & szFontPath,
			uint size,
			FontType fontType);

		//bool	create(IRenderSystem *pRenderSystem,ITextureManager *pTextureManager,const char* szFontName,uint size,FontType fonttype);
	public:
	ITextureFont * m_pFTFont;

	private:
		
		std::string		m_name;//字体名字
		std::string	m_strFontName;//创建字体用的路径或者...
		FontType	m_type;
		uint	m_ui32Size;
		uchar*	m_pFontData;
		IRenderSystem*	m_pRenderSystem;
		FontManager*	m_pFontManager;
	};
}

#endif