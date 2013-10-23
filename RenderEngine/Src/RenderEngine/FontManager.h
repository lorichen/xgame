#ifndef __FontManager_H__
#define __FontManager_H__

namespace xs
{
	class FontManager : public IFontManager
	{
		typedef std::map<IFont*,std::string>		FontString;
		typedef std::map<std::string,IFont*>		StringFont;
	public:
		/** 创建字体
		@param IRenderSystem* 渲染系统接口
		@param ITextureManager* 贴图管理器接口
		@param name 字体文件名
		@param szFontName 字体命名
		@param size 字体大小
		@return 字体接口
		*/
		virtual IFont*	createFont(IRenderSystem*,ITextureManager*,const char* name,const char* szFontName,uint size,FontType fonttype);

		/** 销毁字体
		@param IFont* 要销毁的字体接口指针
		*/
		virtual void 	releaseFont(IFont*);							

		/** 通过名字方式销毁字体
		@param name 字体名称
		*/
		virtual void 	releaseFont(const char* name);						

		/** 销毁所有加载的字体
		*/
		virtual void 	releaseAll();									

		/** 通过名字方式获得字体接口
		@param name 字体名称
		@return 字体接口
		*/
		virtual IFont*	getFontByName(const char* name);
		//渲染所有文字
		virtual void RenderAllWord();
	public:
		bool registerFont(IFont* pFont);//向字体管理器注册字体
		void unregisterFont(IFont* pFont);//向字体管理器反注册字体
	protected:
		FontManager();
	public:
		static FontManager*	Instance()
		{
			static FontManager fm;
			return &fm;
		}
		~FontManager();
	private:
		FontString		m_fs;
		StringFont		m_sf;
	};

}

#endif