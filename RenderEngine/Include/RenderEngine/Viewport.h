#ifndef __Viewport_H__
#define __Viewport_H__

namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	class _RenderEngineExport Viewport
	{
		friend class Camera;
	public:
		void  release();

		Viewport();
		~Viewport();

		/** 获得视口的left坐标
		*/
		int getLeft();

		/** 获得视口的top坐标
		*/
		int getTop();

		/** 获得视口宽度
		*/
		int getWidth();

		/** 获得视口的高度
		*/
		int getHeight();

		/** 设置视口的left
		*/
		void  setLeft(int left);

		/** 设置视口的top
		*/
		void  setTop(int top);

		/** 设置视口的宽度
		*/
		void  setWidth(int width);

		/** 设置视口的高度
		*/
		void  setHeight(int height);

		/** 获得视口的各种参数
		*/
		void  setValues(int left,int top,int width,int height);

		/** 设置视口的各个参数
		*/
		void  getValues(int& left,int& top,int& width,int& height);

		/** Sets the initial background color of the viewport (before rendering).
		*/
		void  setBackgroundColor(const ColorValue& color);

		/** Gets the background colour.
		*/
		const ColorValue& getBackgroundColor(void ) const;

		/** Determines whether to clear the viewport before rendering.
		@remarks
		If you expecting every pixel on the viewport to be redrawn
		every frame, you can save a little time by not clearing the
		viewport before every frame. Do so by passing 'false' to this
		method (the default is to clear every frame).
		*/
		void  setClearEveryFrame(bool clear);

		/** Determines if the viewport is cleared before every frame.
		*/
		bool getClearEveryFrame(void ) const;

	private:
		int	m_ui32Left,m_ui32Top,m_ui32Width,m_ui32Height;
		bool	m_bClear;
		ColorValue	m_color;
		Camera*	m_pCamera;
	private:
		/***/
		void  setCamera(Camera *pCamera);
	};
	/** @} */
}

#endif