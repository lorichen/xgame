#ifndef __TextureManager_H__
#define __TextureManager_H__

#include "Singleton.h"
#include "RenderEngine/Manager.h"

//crr make it singleton 2010-06-07
namespace xs
{

	class TextureManager : public Manager<ITexture*> , public ITextureManager, public Singleton<TextureManager>
	{
	public:
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
			TextureAddressingMode s = TAM_CLAMP_TO_EDGE,											
			TextureAddressingMode t = TAM_CLAMP_TO_EDGE);										

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
			TextureAddressingMode s = TAM_CLAMP_TO_EDGE,											
			TextureAddressingMode t = TAM_CLAMP_TO_EDGE);										

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
			TextureAddressingMode s = TAM_CLAMP_TO_EDGE,											
			TextureAddressingMode t = TAM_CLAMP_TO_EDGE);										

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
			TextureAddressingMode s = TAM_CLAMP_TO_EDGE,											
			TextureAddressingMode t = TAM_CLAMP_TO_EDGE);										

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
			TextureAddressingMode s = TAM_CLAMP_TO_EDGE,											
			TextureAddressingMode t = TAM_CLAMP_TO_EDGE,const std::string& name= "");			

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
			TextureAddressingMode s = TAM_CLAMP_TO_EDGE,											
			TextureAddressingMode t = TAM_CLAMP_TO_EDGE);	

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
			TextureAddressingMode s = TAM_CLAMP_TO_EDGE,											
			TextureAddressingMode t = TAM_CLAMP_TO_EDGE);

		/** 根据文件名取得贴图指针，也可以是自定义的贴图名称
		@param name 文件名或者自定义的贴图名称
		@return 贴图指针
		*/
		virtual ITexture*	getTexture(const std::string& name);											

		/** 释放贴图
		@param pTexture 贴图的指针
		*/
		virtual void 	releaseTexture(ITexture* pTexture);															

		/** 根据名称释放贴图
		@param name 贴图的名称
		*/
		virtual void 	releaseTextureByName(const std::string& name);										

		/** 释放所有贴图
		*/
		virtual void 	releaseAll();

		/**获取渲染线程的ID //added by xxh 20091029
		@return 渲染线程的ID,0：表示还没有初始化
		@remarks 此函数用于获取渲染线程的ID，在opengl多线程环境下用于判断是否在同一个线程
		*/
		inline ulong getRenderThreadID() { return m_ulRenderThreadId; }

//	protected:
		TextureManager() : m_ui32Num(0){ m_ulRenderThreadId = static_cast<ulong> ( GetCurrentThreadId() ); }
	public:
		static TextureManager*	Instance()
		{
			//static TextureManager tm;
			//return &tm;
			return getInstancePtr();
		}

		~TextureManager(){}
	private:
		void doDelete(ITexture* pTexture);
		const char* getUniqueString();

		uint	m_ui32Num;

		ulong	m_ulRenderThreadId;//addedy by xxh，当前渲染线程的id
	};

}
#endif