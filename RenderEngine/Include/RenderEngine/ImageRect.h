#ifndef __ImageRect_H__
#define __ImageRect_H__

namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	enum ImageRectBlendMode
	{
		IR_OPAQUE,		//不透明
		IR_TRANSPARENT,	//ColorKey
		IR_ALPHA_BLEND	//Alpha混合
	};

	/** 2D渲染，用户可以传入非2^N尺寸的图像，根据显卡情况，可能会拆开图像为许多子图像
	另外提供判断某一个像素是否是透明的功能
	*/
	class _RenderEngineExport ImageRect
	{
	public:

		bool	initialize(IRenderSystem *pRenderSystem,Image& image,bool bSplit = true,const ColorValue *pColorKey = 0);
		bool	initialize(
			IRenderSystem *pRenderSystem,
			const std::string& strImageFilename,bool bSplit = true,
			const ColorValue* pColorKey = 0);

		bool	initialize(IRenderSystem *pRenderSystem,xs::Stream* pStream,const std::string& strType,bool bSplit = true,const ColorValue* pColorKey = 0);

		void 	finalize();
		void 	render(int x,int y,float z = 0,const Rect *pSourceRect = 0);
		void 	render(const Rect *pSourceRect = 0);

		uint	getWidth(){return m_width;}
		uint	getHeight(){return m_height;}

		/** (x,y)处是否透明
		@param x x坐标
		@param y y坐标
		@return true表示透明，false反之
		*/\
		bool	isTransparent(int x,int y);
	public:
		ImageRect();
		~ImageRect();
	private:
		struct ImageRect2N
		{
			Rect rc;				//尺寸
			Vector2	texcoord[4];	//贴图坐标
			ITexture*	texture;	//贴图指针
		};
	private:
		typedef std::vector<ImageRect2N>	ImageRectList;
		ImageRectList m_vImageRect;
		IRenderSystem*	m_pRenderSystem;
		ImageRectBlendMode	m_blendMode;

		uint	m_width,m_height;
		uint	m_next_power_of_two_width,m_next_power_of_two_height;

		uchar*	m_pTransparentData;
		uint	m_transparentBytesPerLine;
	private:
		/** 将图像拆分，当显卡支持Non_Power_of_2贴图，则没有拆分。
		否则会执行一个递归的拆分操作，直到所有的贴图的尺寸都是2^N尺寸
		level表示深度，当拆分的深度超过这个值，会自动停止拆分
		@param pTextureManager 贴图管理器接口
		@param image 图像数据
		@param list 目标图像框列表，拆分成的子贴图会被放到这个表中
		@param rc 当前图像的尺寸
		@param imageWidth 图像宽度(not used yet)
		@param imageHeight 图像高度(not used yet)
		@param level 递归深度
		@param bSplit 是否拆分图像，true表示拆分，false表示直接创建NextPowerOf2贴图，并Blt
		*/\
		void 	splitImageTo2N(ITextureManager *pTextureManager,const Image& image,ImageRectList& list,const Rect& rc,uint imageWidth,uint imageHeight,uint level,bool bSplit);
	};
	/** @} */
}

#endif