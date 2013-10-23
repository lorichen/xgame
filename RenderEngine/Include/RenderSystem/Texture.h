#ifndef __Texture_H__
#define __Texture_H__

/*
1) modified by xxh 20091029 
原因：对应nvidi的Geforce 9400 显卡，驱动程序在低的版本时，用glCompressedTexImage2D()异步加载dds纹理，必须保证数据不能为空。而这样做
对较高的版本的驱动程序来说，会加载纹理错误。需要glCompressedTexImage2D()指定空数据，再用glCompressedTexSubImage2D上传数据。用seh不能处理，
可能本人能力有限。现在采用纹理的上传同步的方法，renderengine调用settexture()方法时，必需先上传纹理。
解决方案：同步上传纹理。注意：纹理的加载可能会异步，但是纹理的释放一般是同步的，而且引擎使用一个线程加载纹理。修改后，函数：loadFromFile，
loadFromRawData，loadFromImage 对非压缩纹理的意义不变，而对压缩纹理的意义是已经把数据加载到内存，但没有上传到显村。渲染引擎在设置纹理时必须先调用
函数：uploadTextureToGraphicMemory()
*/

#include "RenderEngine/Manager.h"
#include "TextureManager.h"

namespace xs
{
	class Texture : public ManagedItem , public ITexture
	{
	public:
		/** 释放贴图
		*/
		virtual void 			release();																	

		/** 获取贴图宽度
		@return 贴图宽度值
		*/
		virtual uint			width();																	

		/** 获取贴图高度
		@return 贴图高度值
		*/
		virtual uint			height();																	

		/** 获取贴图深度值
		@return 贴图深度值
		*/
		virtual uint			depth();																	

		/** 获取贴图Mipmaps数目，最少为1
		@return Mipmaps数目
		*/
		virtual uint			getNumMipmaps();															

		/** 获取贴图面数，cubemap为6，其他情况是1
		@return 贴图面数
		*/
		virtual uint			getNumFaces();																

		/** 是否有Alpha通道
		@return true有通道，false则没有
		*/
		virtual bool			hasAlpha();																	

		/** 获取贴图像素格式
		@return 像素格式
		*/
		virtual PixelFormat		getPixelFormat();															

		/** 贴图是否被压缩
		@return true是压缩格式，false未压缩
		*/
		virtual bool			isCompressed();																

		/** 获取贴图名称，可能是文件名或者是系统生成的唯一名称
		@return 贴图名称
		*/
		virtual const std::string&	getName();																	

		/** 从文件中创建贴图
		@param name 文件名
		@return true表示创建成功，false表示失败
		*/
		virtual bool			loadFromFile(const std::string& name);											

		/** 从Raw中创建贴图，Raw的每个单元都应该是pf格式
		@param pBuffer 内存指针
		@param width 宽度
		@param height 高度
		@param pf 像素格式PF_R8G8B8等等
		@return true表示创建成功，false表示失败
		*/
		virtual bool			loadFromRawData(uchar *pBuffer,uint width,uint height,PixelFormat pf);	

		/** 从Image中创建贴图
		@param image 图像数据
		@param pRect 子图像区域,有时候不需要图像的全部
		@return true表示创建成功，false表示失败
		*/
		virtual bool			loadFromImage(const Image& image,const Rect* pRect = 0);

		/** 设置贴图区域数据,像素格式需要用户自己保证符合
		@param level 要设置数据的图像Mipmaps等级,从0开始
		@param left 左边开始下标
		@param top 上边开始下标
		@param width 宽度
		@param height 高度
		@param pData 设置的贴图数据
		@return true表示创建成功，false表示失败
		*/
		virtual bool			setSubData(uint level,uint left,uint top,uint width,uint height,void  *pData);	

		/** 设置贴图区域数据,从图像获取数据,从image的pRect获取数据blt到贴图的(0,0)
		@param level 要设置数据的图像Mipmaps等级,从0开始
		@param image 图像数据
		@param pRect 图像的子区域
		@return true表示创建成功，false表示失败
		*/
		virtual bool			setSubData(uint level,const Image& image,const Rect* pRect);

		virtual size_t			getMemorySize();

		/** 设置用户自定义信息
		*/
		virtual void			setCustomInfo(const std::string & key, const std::string & val);

		/** 获取用户自定义信息
		@return 返回false表示没有这个自定义的信息
		*/
		virtual bool			getCustomInfo(const std::string & key, std::string & val);

	public:
		Texture(const std::string& name,ITextureManager* pTextureManager) 
			:ManagedItem(name),m_pTextureManager(pTextureManager)
			,m_ui32Width(1)
			,m_ui32Height(1)
			,m_ui32Depth(1)
			,m_gluiTextureID(0)
			,m_ui32NumMipmaps(1)
			,m_i32Flags(0)
			,m_pf(PF_UNKNOWN)
			,m_memorySize(0)
			,m_pImageBuffer(0)
			,m_lUploadFlag(0){}
		virtual ~Texture(){ if( m_pImageBuffer){delete m_pImageBuffer; m_pImageBuffer = 0;}}
	private:
		uint		m_ui32Width;
		uint		m_ui32Height;
		uint		m_ui32Depth;
		GLuint		m_gluiTextureID;
		uint		m_ui32NumMipmaps;
		int		m_i32Flags;
		PixelFormat	m_pf;
		ITextureManager *m_pTextureManager;
		size_t		m_memorySize;

		typedef stdext::hash_map<std::string, std::string> CustomInfoMap;
		CustomInfoMap m_customInfo;

	public:
		GLuint		getGLTextureID();
	public:
		void		setWidth(uint w);
		void		setHeight(uint h);
		void		setDepth(uint d);
		void		setGLTextureID(GLuint id);
		void		setNumMipmaps(uint numMipmaps);
		void		setFlags(int flags);
		void		setPixelFormat(PixelFormat pf);
	private:
		//modified by xxh ，考虑到可以存储这个image时,调用
		// _loadFromImage(image, &Image);
		//不可以存储时，则调用
		// _loadFromImage(image);
		bool		_loadFromImage(const Image& image, Image * pImage = 0);
		//added by xxh 20091029 ,用于同步上传纹理
	public:
		//上传纹理,用内联提高效率
		inline bool uploadTextureToGraphicMemory()
		{
			if( m_lUploadFlag == 1 ) return true;	
			else return realUpLoadTextureToGraphicMemory();
		};
		volatile Image * m_pImageBuffer;//图片缓存，在没有上传纹理，但已经加载纹理时，存放纹理
		volatile LONG m_lUploadFlag;//纹理已经上传的标志，1表示上传了，0表示还没有上传。
	private:
		bool realUpLoadTextureToGraphicMemory();
	};

}
#endif