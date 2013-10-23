#ifndef __I_TextureManager_H__
#define __I_TextureManager_H__

namespace xs
{
	/*! addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	/*不要在渲染循环中创建纹理，如果一定要创建，使用后缀为NoWait字样的函数，用其它的可能会引起死锁,
		比如createEmptyTexture函数对应的函数为createEmptyTextureNoWait。
	*/


	class Image;

	/** 贴图接口
	*/
	struct ITexture
	{
		/** 释放贴图
		*/
		virtual void 			release() = 0;																	
		
		/** 获取贴图宽度
		@return 贴图宽度值
		*/
		virtual uint			width() = 0;																	
		
		/** 获取贴图高度
		@return 贴图高度值
		*/
		virtual uint			height() = 0;																	
		
		/** 获取贴图深度值
		@return 贴图深度值
		*/
		virtual uint			depth() = 0;																	
		
		/** 获取贴图Mipmaps数目，最少为1
		@return Mipmaps数目
		*/
		virtual uint			getNumMipmaps() = 0;															
		
		/** 获取贴图面数，cubemap为6，其他情况是1
		@return 贴图面数
		*/
		virtual uint			getNumFaces() = 0;																
		
		/** 是否有Alpha通道
		@return true有通道，false则没有
		*/
		virtual bool			hasAlpha() = 0;																	
		
		/** 获取贴图像素格式
		@return 像素格式
		*/
		virtual PixelFormat		getPixelFormat() = 0;															
		
		/** 贴图是否被压缩
		@return true是压缩格式，false未压缩
		*/
		virtual bool			isCompressed() = 0;																
		
		/** 获取贴图名称，可能是文件名或者是系统生成的唯一名称
		@return 贴图名称
		*/
		virtual const std::string&	getName() = 0;																	
		
		/** 从文件中创建贴图
		@param name 文件名
		@return true表示创建成功，false表示失败
		*/
		virtual bool			loadFromFile(const std::string& name) = 0;											
		
		/** 从Raw中创建贴图，Raw的每个单元都应该是pf格式
		@param pBuffer 内存指针
		@param width 宽度
		@param height 高度
		@param pf 像素格式PF_R8G8B8等等
		@return true表示创建成功，false表示失败
		*/
		virtual bool			loadFromRawData(uchar *pBuffer,uint width,uint height,PixelFormat pf) = 0;	
		
		/** 从Image中创建贴图
		@param image 图像数据
		@param pRect 子图像区域,有时候不需要图像的全部
		@return true表示创建成功，false表示失败
		*/
		virtual bool			loadFromImage(const Image& image,const Rect* pRect = 0) = 0;
		
		/** 设置贴图区域数据,像素格式需要用户自己保证符合
		@param level 要设置数据的图像Mipmaps等级,从0开始
		@param left 左边开始下标
		@param top 上边开始下标
		@param width 宽度
		@param height 高度
		@param pData 设置的贴图数据
		@return true表示创建成功，false表示失败
		*/
		virtual bool			setSubData(uint level,uint left,uint top,uint width,uint height,void  *pData) = 0;	
		
		/** 设置贴图区域数据,从图像获取数据,从image的pRect获取数据blt到贴图的(0,0)
		@param level 要设置数据的图像Mipmaps等级,从0开始
		@param image 图像数据
		@param pRect 图像的子区域
		@return true表示创建成功，false表示失败
		*/
		virtual bool			setSubData(uint level,const Image& image,const Rect* pRect) = 0;


		/**获取占用内存大小
		*/
		virtual size_t			getMemorySize() = 0;

		/** 设置用户自定义信息
		*/
		virtual void			setCustomInfo(const std::string & key, const std::string & val) = 0;

		/** 获取用户自定义信息
		@return 返回false表示没有这个自定义的信息
		*/
		virtual bool			getCustomInfo(const std::string & key, std::string & val)=0;
	};


	/** 贴图管理器接口
	*/
	struct ITextureManager
	{
		/** 从图像数据创建贴图,贴图是width*height的尺寸,并把image的pRect区域blt到贴图上
		@param image 图像数据
		@param pRect 图像的子区域
		@param width 宽度
		@param height 高度
		@param minFO Min Filter
		@param magFO Max Filter
		@param mipFO Mip Filter
		@param s 贴图S寻址方式
		@param t 贴图T寻址方式
		@return 贴图指针
		*/
		virtual ITexture*	createTextureFromImage(																
			const Image& image,																
			const Rect* pRect,																
			uint width,uint height,														
			FilterOptions minFO = FO_LINEAR,												
			FilterOptions magFO = FO_LINEAR,												
			FilterOptions mipFO = FO_NONE,													
			TextureAddressingMode s = TAM_WRAP,											
			TextureAddressingMode t = TAM_WRAP) = 0;										

		/** 从图像数据创建贴图,贴图的大小是pRect的宽度,如果pRect则为整张图片的尺寸
		@param image 图像数据
		@param pRect 图像的子区域
		@param minFO Min Filter
		@param magFO Max Filter
		@param mipFO Mip Filter
		@param s 贴图S寻址方式
		@param t 贴图T寻址方式
		@return 贴图指针
		*/
		virtual ITexture*	createTextureFromImage(																
			const Image& image,																
			const Rect* pRect = 0,																
			FilterOptions minFO = FO_LINEAR,												
			FilterOptions magFO = FO_LINEAR,												
			FilterOptions mipFO = FO_NONE,													
			TextureAddressingMode s = TAM_WRAP,											
			TextureAddressingMode t = TAM_WRAP) = 0;										

		/** 从图像文件创建贴图,在有些显卡上支持非2^n贴图,但大多数时候用户需要使用2^N贴图
		@param name 文件名
		@param minFO Min Filter
		@param magFO Max Filter
		@param mipFO Mip Filter
		@param s 贴图S寻址方式
		@param t 贴图T寻址方式
		@return 贴图指针
		*/
		virtual ITexture*	createTextureFromFile(																
			const std::string& name,																
			FilterOptions minFO = FO_LINEAR,												
			FilterOptions magFO = FO_LINEAR,												
			FilterOptions mipFO = FO_NONE,													
			TextureAddressingMode s = TAM_WRAP,											
			TextureAddressingMode t = TAM_WRAP) = 0;										

		/** 从Raw数据创建贴图
		@param pBuffer 数据指针
		@param width pBuffer图像数据的宽度
		@param height pBuffer图像数据的高度
		@param minFO Min Filter
		@param magFO Max Filter
		@param mipFO Mip Filter
		@param s 贴图S寻址方式
		@param t 贴图T寻址方式
		@return 贴图指针
		*/
		virtual ITexture*	createTextureFromRawData(															
			uchar* pBuffer,																	
			uint width,uint height,PixelFormat pf,										
			FilterOptions minFO = FO_LINEAR,												
			FilterOptions magFO = FO_LINEAR,												
			FilterOptions mipFO = FO_NONE,													
			TextureAddressingMode s = TAM_WRAP,											
			TextureAddressingMode t = TAM_WRAP) = 0;										

		/** 创建空的贴图,未指定宽度、高度和像素格式
		@param minFO Min Filter
		@param magFO Max Filter
		@param mipFO Mip Filter
		@param s 贴图S寻址方式
		@param t 贴图T寻址方式
		@return 贴图指针
		*/
		virtual ITexture*	createEmptyTexture(																	
			FilterOptions minFO = FO_LINEAR,												
			FilterOptions magFO = FO_LINEAR,												
			FilterOptions mipFO = FO_NONE,													
			TextureAddressingMode s = TAM_WRAP,											
			TextureAddressingMode t = TAM_WRAP,const std::string& name= "") = 0;			

		/** 创建空的贴图,并指定宽度、高度和像素格式
		@param width 宽度
		@param height 高度
		@param pf 像素格式
		@param minFO Min Filter
		@param magFO Max Filter
		@param mipFO Mip Filter
		@param s 贴图S寻址方式
		@param t 贴图T寻址方式
		@return 贴图指针
		*/
		virtual ITexture*	createEmptyTexture(																	
			uint width,uint height,PixelFormat pf,										
			FilterOptions minFO = FO_LINEAR,												
			FilterOptions magFO = FO_LINEAR,												
			FilterOptions mipFO = FO_NONE,													
			TextureAddressingMode s = TAM_WRAP,											
			TextureAddressingMode t = TAM_WRAP) = 0;

		/** 创建空的贴图,并指定宽度、高度和像素格式。不需要等待渲染结束，这个函数会影响帧率，但是
		在渲染中调用不会锁定。
		@param width 宽度
		@param height 高度
		@param pf 像素格式
		@param minFO Min Filter
		@param magFO Max Filter
		@param mipFO Mip Filter
		@param s 贴图S寻址方式
		@param t 贴图T寻址方式
		@return 贴图指针
		*/
		virtual ITexture*	createEmptyTextureNoWait(																	
			uint width,uint height,PixelFormat pf,										
			FilterOptions minFO = FO_LINEAR,												
			FilterOptions magFO = FO_LINEAR,												
			FilterOptions mipFO = FO_NONE,													
			TextureAddressingMode s = TAM_WRAP,											
			TextureAddressingMode t = TAM_WRAP) = 0;

		/** 根据文件名取得贴图指针，也可以是自定义的贴图名称
		@param name 文件名或者自定义的贴图名称
		@return 贴图指针
		*/
		virtual ITexture*	getTexture(const std::string& name) = 0;											

		/** 释放贴图
		@param pTexture 贴图的指针
		*/
		virtual void 	releaseTexture(ITexture* pTexture) = 0;															

		/** 根据名称释放贴图
		@param name 贴图的名称
		*/
		virtual void 	releaseTextureByName(const std::string& name) = 0;										

		/** 释放所有贴图
		*/
		virtual void 	releaseAll() = 0;
	};
	/** @} */
}
#endif