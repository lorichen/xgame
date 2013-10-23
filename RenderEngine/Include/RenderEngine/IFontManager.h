#ifndef __I_FontManager_H__
#define __I_FontManager_H__

namespace xs
{
	/*! addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	/** 字体接口
	*/

	/**文字的绘制方向,默认从左到右
	*/
	enum FontRenderDirection
	{
		FRD_LEFT2RIGHT = 0,//从做到右
		FRD_RIGHT2LEFT,//从右边到左边
		FRD_TOP2BOTTOM,//从上到下
		FRD_BOTTOM2TOP,//从下到上
	};
	enum FontType
	{
		FontType_GDI = 0,//大字体,GDI效果
		FontType_PIX, //点阵字体
	};

	struct IFont
	{
		virtual void release() = 0;
		/** 绘制文字
		@param pText Unicode文字信息
		*/
		virtual void  render(const wchar_t* pText) = 0;										

		/** 绘制文字
		@param pText Ascii文字信息
		*/
		virtual void  render(const char* pText) = 0;											

		/** 2D状态下在视口某位置绘制文字
		@param xx x坐标
		@param yy y坐标
		@param zz z坐标
		@param color 颜色
		@param pText Ascii文字信息
		*/
		virtual void  render2d(float xx,float yy,float zz,const ColorValue& color,const char* pText) = 0;

		/** 2D状态下在视口某位置绘制文字
		@param xx x坐标
		@param yy y坐标
		@param zz z坐标
		@param color 颜色
		@param pText Unicode文字信息
		*/
		virtual void  render2d(float xx,float yy,float zz,const ColorValue& color,const wchar_t* pText) = 0;

		/** 获得字体名称
		@return 字体名称，如"宋体"
		*/
		virtual const char*	getFontName() = 0;													

		/** 获得字体大小
		@return 字体大小
		*/
		virtual uint	getFontSize() = 0;													

		/** 获得字体Ascender
		@return 字体Ascender
		*/
		virtual float getAscender() const = 0;												

		/** 获得字体Descender
		@return 字体Descender
		*/
		virtual float getDescender() const = 0;												

		/** 获得字体LineHeight
		@return 字体LineHeight
		*/
		virtual float getLineHeight() const = 0;												

		/** 获得字体AABB的Ascii版本
		@return 字体AABB
		*/
		virtual void  getAABB( const char* string, AABB& aabb) = 0;							

		/** 获得字体AABB的Unicode版本
		@return 字体AABB
		*/
		virtual void  getAABB( const wchar_t* string, AABB &aabb) = 0;						

		/** 获得字体Advance的Unicode版本
		@return 字体Advance
		*/
		virtual float getAdvance( const wchar_t* string) = 0;								

		/** 获得字体Advance的Ascii版本
		@return 字体Advance
		*/
		virtual float getAdvance( const char* string) = 0;									

		/** 获得字体Advance的Unicode版本
		@return 字体Advance
		*/
		virtual float getAdvance( const wchar_t c) = 0;										

		/** 获得字体Advance的Ascii版本
		@return 字体Advance
		*/
		virtual float getAdvance( const char c) = 0;

		/**设置文字的绘制方向
		@return 旧的绘制方向
		*/
		virtual FontRenderDirection setRenderDirection( FontRenderDirection rd) = 0;

		/**设置文字的间距
		@return 旧的间距
		*/
		virtual float setFontGap( float gap)=0;
		
		//add by yhc
		//文字效果
		//颜色渐变
		virtual void GradientText(ColorValue& color1,ColorValue& color2) = 0;
		//单描边
		virtual void TextOutline(ColorValue& clrOutline, int nThickness)= 0;
		//双描边
		virtual void TextDblOutline(ColorValue& clrOutline1, ColorValue& clrOutline2, int nThickness1, int nThickness2)= 0;
		//发光
		virtual void TextGlow(ColorValue& clrOutline, int nThickness)= 0;
		//阴影
		virtual void Shadow(ColorValue& color, int nThickness,int nOffsetX,int nOffsetY)= 0;
		//发散的阴影
		virtual void DiffusedShadow(ColorValue& color, int nThickness,int nOffsetX,int nOffsetY)= 0;
		//突出字,阴影在后
		virtual void Extrude(ColorValue& color, int nThickness,int nOffsetX,int nOffsetY)= 0;
		//有缩放的字
		virtual void  render2dByScale(float xx,float yy,float zz,const ColorValue& color,const wchar_t* pText,float fScale)= 0;

		//预初始化一些常用字
		virtual void InitText(wchar_t *pStr) = 0;
	};

	struct IRenderSystem;
	struct ITextureManager;

	/** 字体管理器接口
	*/
	struct IFontManager
	{
		/** 创建字体
		@param IRenderSystem* 渲染系统接口
		@param ITextureManager* 贴图管理器接口
		@param name 字体文件名
		@param szFontName 字体命名
		@param size 字体大小
		@return 字体接口
		*/
		virtual IFont*	createFont(IRenderSystem*,ITextureManager*,const char* name,const char* szFontName,uint size,FontType fonttype) = 0;

		/** 销毁字体
		@param IFont* 要销毁的字体接口指针
		*/
		virtual void 	releaseFont(IFont*) = 0;							

		/** 通过名字方式销毁字体
		@param name 字体名称
		*/
		virtual void 	releaseFont(const char* name) = 0;						

		/** 销毁所有加载的字体
		*/
		virtual void 	releaseAll() = 0;									

		/** 通过名字方式获得字体接口
		@param name 字体名称
		@return 字体接口
		*/
		virtual IFont*	getFontByName(const char* name) = 0;

		//渲染所有文字
		virtual void RenderAllWord()= 0;
	};
	/** @} */

}
#endif