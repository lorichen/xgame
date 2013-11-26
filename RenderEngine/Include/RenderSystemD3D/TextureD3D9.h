#ifndef __TextureD3D9_H__
#define __TextureD3D9_H__


#include "RenderEngine/Manager.h"
#include "D3D9ResourceManager.h"
/*
	ITexture接口的设计使得它只支持普通的纹理，不支持Cube Texture和 Volume Texture, 
	所以现在的实现暂时只支持普通的纹理
*/


namespace xs
{
	class TextureManagerD3D9;

	class TextureD3D9 : public ManagedItem , public ITexture, public D3D9Resource
	{
		// ITexture接口
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

		/** 从文件中创建贴图，不会缩放图像
		@param name 文件名
		@return true表示创建成功，false表示失败
		*/
		virtual bool			loadFromFile(const std::string& name);											

		/** 从Raw中创建贴图，Raw的每个单元都应该是pf格式，不会缩放图像
		@param pBuffer 内存指针
		@param width 宽度
		@param height 高度
		@param pf 像素格式PF_R8G8B8等等
		@return true表示创建成功，false表示失败
		*/
		virtual bool			loadFromRawData(uchar *pBuffer,uint width,uint height,PixelFormat pf);	

		/** 从Image中创建贴图，不会缩放图像
		@param image 图像数据
		@param pRect 子图像区域,有时候不需要图像的全部
		@return true表示创建成功，false表示失败
		*/
		virtual bool			loadFromImage(const Image& image,const Rect* pRect = 0);

		/** 设置贴图区域数据,像素格式需要用户自己保证符合,不会缩放图像
		@param level 要设置数据的图像Mipmaps等级,从0开始
		@param left 左边开始下标
		@param top 上边开始下标
		@param width 宽度
		@param height 高度
		@param pData 设置的贴图数据
		@return true表示创建成功，false表示失败
		*/
		virtual bool			setSubData(uint level,uint left,uint top,uint width,uint height,void  *pData);	

		/** 设置贴图区域数据,从图像获取数据,从image的pRect获取数据blt到贴图的(0,0)，不会缩放图像
		@param level 要设置数据的图像Mipmaps等级,从0开始
		@param image 图像数据
		@param pRect 图像的子区域, RECT的right 和bottom 和D3D9的不一样。
		@return true表示创建成功，false表示失败
		*/
		virtual bool			setSubData(uint level,const Image& image,const Rect* pRect);


		/**获取占用内存大小
		*/
		virtual size_t			getMemorySize();

		/** 设置用户自定义信息
		*/
		virtual void			setCustomInfo(const std::string & key, const std::string & val);

		/** 获取用户自定义信息
		@return 返回false表示没有这个自定义的信息
		*/
		virtual bool			getCustomInfo(const std::string & key, std::string & val);


		//D3D9Resource接口
	public:
		// 设备丢失，在这里释放Default Pool管理的资源
		virtual void notifyOnDeviceLost();

		//设备恢复, 在这里重建Default Pool 管理的资源
		virtual void notifyOnDeviceReset();

	public:
		enum	CreateTextureOptions
		{
			CTO_DEFAULT = 0x0,//默认
			CTO_NOWAIT = 0x1,//不需要等待渲染完成
		};
		/** 创建纹理,自定义管理池和使用方式
		*/
		bool _create(D3DPOOL pool, DWORD usage, uint width, uint height,  PixelFormat pf, CreateTextureOptions option= CTO_DEFAULT);

		/**
		*/
		bool lock(UINT level, D3DLOCKED_RECT & lockedrect, DWORD flag);

		/**
		*/
		void unlock();

		/**
		*/
		bool isLocked();

		/**
		*/

	public:
		void		setWidth(uint w);
		void		setHeight(uint h);
		void		setDepth(uint d);
		void		setNumMipmaps(uint numMipmaps);
		void		setFlags(int flags);
		void		setPixelFormat(PixelFormat pf);
		bool		_loadFromImage(const Image& image);

		void		applyFilterState(DWORD sampler);//此函数被渲染系统调用，设置过滤器状态
		inline		IDirect3DTexture9 * getD3DTexture(){ return m_pTex;};//获取D3D9纹理

		inline FilterOptions getMinFilter() const { return m_minFilter;}
		inline FilterOptions getMagFilter() const { return m_magFilter;}
		inline FilterOptions getMipFilter() const { return m_mipmapFilter;}
		inline TextureAddressingMode getAddressS() const { return m_addressS;}
		inline TextureAddressingMode getAddressT() const { return m_addressT;}

	private:
		RECT		getD3DRect(const  xs::Box & box );
		UINT		getRowPitch(PixelFormat pf, uint rowWidth);

	public:
		TextureD3D9(const std::string& name
			,TextureManagerD3D9* pTextureManager
			,FilterOptions min
			,FilterOptions mag
			,FilterOptions mip
			,TextureAddressingMode s
			,TextureAddressingMode t);
		virtual ~TextureD3D9();
	private:
		uint		m_ui32Width;
		uint		m_ui32Height;
		uint		m_ui32Depth;
		uint		m_ui32NumMipmaps;
		int		m_i32Flags;
		PixelFormat	m_pf;		
		size_t		m_memorySize;

		FilterOptions m_minFilter;
		FilterOptions m_magFilter;
		FilterOptions m_mipmapFilter;
		TextureAddressingMode m_addressS;
		TextureAddressingMode m_addressT;

		TextureManagerD3D9 *m_pTextureManager;
		D3DPOOL		m_poolType;
		DWORD		m_usage;	
		IDirect3DTexture9 * m_pTex;

		bool		m_bLocked;
		uint		m_uiLockedLevel;

		typedef stdext::hash_map<std::string, std::string> CustomInfoMap;
		CustomInfoMap m_customInfo;
	};
}
#endif