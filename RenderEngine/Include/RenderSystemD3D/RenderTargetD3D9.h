#ifndef __RenderTargetD3D9_H__
#define __RenderTargetD3D9_H__
namespace xs
{
	class RenderSystemD3D9;
	class RenderTargetManagerD3D9;

	class RenderTargetD3D9
	{
		//接口
	public:
		virtual void 		release()=0;
		virtual const ITexture*	getTexture()=0;
		virtual bool		getTextureData(void * pData)=0;
		virtual void 		onAttach(RenderTargetD3D9 * pPrevious);
		virtual void 		onDetach();

		virtual void		onBeginFrame(bool clearColorBuffer,bool clearDepthBuffer,bool clearStencilBuffer)=0;
		virtual void 		onEndFrame(int layer)=0;

		virtual void		clearFrameBuffer(bool clearColorBuffer,bool clearDepthBuffer ,bool clearStencilBuffer) =0;

		virtual bool		isDefaultRenderTarget() {return false; };//判断渲染目标是不是设备的一个属性
		
		virtual IDirect3DSurface9 * getDepthStencilBuffer()=0;

		virtual void onDeviceLost() = 0;
		virtual void onDeviceReset() = 0;
		virtual RECT getFrameBufferRect() = 0;

		virtual bool switchDisplayMode(RenderEngineCreationParameters* param)=0;//改变显示模式

		virtual bool getDisplayMode(RenderEngineCreationParameters & createParam)=0;//获取当前的显示模式

	public:	
		//清除帧缓存函数
		void				setClearColor(const ColorValue& color);
		const ColorValue&	getClearColor();
		void				setClearDepth(float depth);
		float				getClearDepth();
		void				setClearStencil(int stencil);
		int					getClearStencil();

		//2d 3d切换
		void				switchTo2D();
		void 				switchTo2D(int left,int top,int width,int height);
		void				switchTo3D();
		bool				is2dMode();

		//背面拣选
		void				setCullingMode(const CullingMode& cm);
		const CullingMode&	getCullingMode();

		//光照参数
		void				setLightingEnabled(bool enabled);
		bool				isLightingEnabled();

		//矩阵变换
		void				setWorldMatrix(const Matrix4 & mtx);
		const Matrix4&		getWorldMatrix();
		const Matrix4&		getModelViewMatrix();
		void				setViewMatrix(const Matrix4& mtx);
		const Matrix4&		getViewMatrix();
		void				setProjectionMatrix(const Matrix4& mtx);
		const Matrix4&		getProjectionMatrix();

		void				getWorldMatrix(D3DXMATRIX & mtx);
		void				getViewMatrix(D3DXMATRIX & mtx);
		void				getProjectionMatrix(D3DXMATRIX & mtx);

		//深度检测
		bool					isDepthBufferCheckEnabled();
		void					setDepthBufferCheckEnabled(bool enabled);
		void					setDepthBufferFunction(const CompareFunction& cf);
		void					setDepthBufferWriteEnabled(bool enabled);
		bool					isDepthBufferWriteEnabled();
		const CompareFunction&	getDepthBufferFunction();

		//颜色缓存mask
		void					setColorBufferWriteEnabled(bool red, bool green, bool blue, bool alpha);

		//视口
		void					setViewport(int left,int top,int width,int height);
		void					getViewport(int &left,int &top,int &width,int &height);

		//投影和逆投影
		Vector3					unproject( const Vector3 & v);
		Vector3					project(const Vector3 & v);

	public:
		struct RenderState
		{
			//与渲染目标相关且与2D和3D模式相关的状态

			//矩阵状态
			Matrix4	m_mtxView;
			Matrix4 m_mtxWorld;
			Matrix4 m_mtxModelView;
			Matrix4	m_mtxProjection;
			//背面拣选
			CullingMode	m_cullingMode;
			//光照参数
			bool	m_bLightingEnabled;
			//深度检测
			bool	m_bDepthWriteEnabled;
			bool	m_bDepthCheckEnabled;
			CompareFunction	m_DepthBufferFunction;

		public:
			RenderState();//构造函数
		};

	private:
		void setRenderState( const RenderState & rs);

	public:
		void resetRenderState();//重设渲染状态

	protected:
		inline DWORD		F2DW(float f) { return *((DWORD*)&f);}
		void				convertProjectionMatrix(const Matrix4& matrix,
							Matrix4& dest, bool forGpuProgram);
		
	protected:
		//构造和析构函数
		RenderTargetD3D9(RenderSystemD3D9 * pRenderSystem, bool b2d, RenderTargetManagerD3D9 * pRenderTargetMgr);
		virtual ~RenderTargetD3D9();	
	public:
		//与2D和3D模式相关的渲染状态
		RenderState		m_RenderState2D,m_RenderState3D;
		RenderState		m_RenderState;
		bool			m_b2D;

		//与渲染目标相关的状态，但是与2D和3D模式无关的状态
	public:
		//帧缓存清除状态
		ColorValue	m_clearColor;
		float	m_clearDepth;
		int	m_clearStencil;

		//视口参数
		D3DVIEWPORT9 m_viewport;

		//颜色缓存Mask
		bool m_redChannel;
		bool m_greenChannel;
		bool m_blueChannel;
		bool m_alphaChannel;

	public:
		RenderSystemD3D9 * m_pRenderSystem;//渲染系统
		RenderTargetManagerD3D9 * m_pRenderTargetMgr;//渲染目标管理器
	};
}
#endif