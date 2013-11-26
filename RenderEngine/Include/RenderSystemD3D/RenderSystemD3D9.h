#ifndef __RenderSystem_H__
#define __RenderSystem_H__

/*
//--------------------------------------------------------------------------------------------------------------
1,渲染系统多线程的支持
	a, 渲染系统部分支持多线程，IndexBuffer， VertexBuffer， Texutre， Shader这些资源的创建和访问支持多线程。
	b，所有资源的释放和重建都是在主线程中实现
	d，渲染系统的绘制函数以及渲染系统测状态设置不支持多线程
	c, 渲染系统释放时要保证辅助线程已经不会访问渲染系统了。
	d, 不要在绘制时(beginFrame和endFrame之间)创建资源，如果一定要创建，使用后缀为NoWait字样的函数，用其它的可能会引起死锁,
		比如createEmptyTexture函数对应的函数为createEmptyTextureNoWait。
	

//------------------------------------------------------------------------------------------------------------------
2,渲染系统支持多个实例


//-------------------------------------------------------------------------------------------------------------------
*/

namespace xs
{
	class D3D9Driver;
	class D3D9Device;
	class D3D9ResourceManager;
	class RenderTargetManagerD3D9;
	class BufferManagerD3D9;
	class StreamSourceMappings;
	class BeginEndPrimitiveProxy;
	class RenderSystemD3D9;
	class TextureManagerD3D9;
	class HardwareCursorManagerD3D9;
	class ShaderProgramManagerD3D9;

	/* 用于保存渲染系统的状态，这些状态与渲染目标无关
	*/
	class RenderSystemD3D9State
	{
	private:
		RenderSystemD3D9State();
		~RenderSystemD3D9State(){};
		friend class RenderSystemD3D9;
		//处理渲染系统的状态,这些状态是与渲染目标无关的
	public: 
		//图元状态
		float	m_lineWidth;
		float	m_pointSize;
		//填充模式
		FillMode	m_fillMode;
		//beginPrimitive 和 endPrimitive 相关状态
		const static uint s_maxTexureUnit = 8;
		ColorValue	m_color;
		bool		m_edgeFlag;
		Vector3		m_normal;
		Vector2		m_texcoord[s_maxTexureUnit];
		//当前材质
		ColorValue	m_ambient;
		ColorValue	m_diffuse;
		ColorValue	 m_specular;
		ColorValue	 m_emissive;
		float		m_shininess;
		D3DMATERIAL9 m_material;//保存是为了方便设置
		TrackVertexColorEnum m_trackColor;
		//雾化
		bool m_bEnableFog;
		FogMode m_fogMode;
		ColorValue m_fogColor;
		float m_fogExpDensity;
		float m_fogLinearStart;
		float m_fogLinearEnd;
		//光照,光源的总开关放到渲染目标里面了
		const uint static s_maxLight = 8;
		Light	m_light[s_maxLight];
		D3DLIGHT9 m_d3d9light[s_maxLight];//添加这个成员，方便设置
		xs::ColorValue m_ambientLight;
		//alpha检测
		bool	m_bAlphaCheckEnabled;
		CompareFunction	m_AlphaCompareFunction;
		float	m_AlphaCompareValue;
		//模板检测
		bool	m_bEnableStencilTest;
		CompareFunction m_stencilFunc;
		uint m_stencilRefValue;
		uint m_stencilMask;
		StencilOperation m_stencilFailOp;
		StencilOperation m_depthFailOp;
		StencilOperation m_passOp;
		//剪切测试
		bool m_bScissor;
		//alpha混合
		SceneBlendFactor m_alphaBlendSrc;
		SceneBlendFactor m_alphaBlendDst;

		//采样器状态
		TextureAddressingMode m_addressU[s_maxTexureUnit];//U方向寻址模式
		TextureAddressingMode m_addressV[s_maxTexureUnit];//V方向寻址模式
		FilterOptions		  m_minFilter[s_maxTexureUnit];//缩小过滤器
		FilterOptions		  m_magFilter[s_maxTexureUnit];//放大过滤器
		FilterOptions		  m_mipFilter[s_maxTexureUnit];//MipMap过滤器
	
		//纹理阶段状态
		bool	m_TSSEnable[s_maxTexureUnit];//是否开启某一阶段的纹理映射
		TextureStageOperator m_TSSColorOP[s_maxTexureUnit];// 纹理操作函数
		TextureStageArgument m_TSSColorArg[s_maxTexureUnit][2];//纹理操作数
		TextureStageOperator m_TSSAlphaOP[s_maxTexureUnit];//alpha操作函数
		TextureStageArgument m_TSSAlphaArg[s_maxTexureUnit][2];//alpha操作数
		
		// 反锯齿
		bool m_bMutiSampleAntialias;
	};


	class RenderSystemD3D9 : public IRenderSystem
	{
	public:
		/** 释放渲染系统
		*/
		virtual void			release();

		/** 获取渲染系统的名称，如"OpenGL","D3D9"等
		@return 渲染系统名称的字符串
		*/
		virtual const char*		getName();							

		/** 获得渲染系统性能指针
		@return 渲染系统性能指针
		*/
		virtual RenderSystemCapabilities*	getCapabilities();	

		/** 设置视口
		@param pViewport 视口指针
		*/
		virtual void 		setViewport(int left,int top,int width,int height);			

		/** 获取视口数据
		@param left 视口距离窗口左边的距离
		@param top 视口距离窗口上边的距离
		@param width 视口的宽度
		@param height 视口的高度
		*/
		virtual void 		getViewport(int &left,int &top,int &width,int &height);		

		/** 设置清除视口的背景色
		@param color 背景色
		*/
		virtual void 		setClearColor(const ColorValue& color);
		virtual const ColorValue& getClearColor();				

		/** 设置清除深度缓存的值，当清除深度缓存时候，深度缓存的每个单位都被设置成这个值
		@param depth 深度值，默认是最大深度值
		*/
		virtual void 		setClearDepth(float depth);			

		/** 设置清除模板缓存的值
		@param stencil 清除模板缓存的值，默认是0
		*/
		virtual void 		setClearStencil(int stencil = 0);	

		/** 设置投影矩阵
		@param mtx 投影矩阵
		*/
		virtual void 		setProjectionMatrix(const Matrix4& mtx);			

		/** 设置视矩阵
		@param mtx 视矩阵
		*/
		virtual void 		setViewMatrix(const Matrix4& mtx);	

		/** 设置世界矩阵
		@param mtx 世界矩阵
		*/
		virtual void 		setWorldMatrix(const Matrix4 &mtx);	

		/** 获取投影矩阵
		@return 当前渲染系统的投影矩阵
		*/
		virtual const Matrix4&		getProjectionMatrix();		

		/** 获取视矩阵
		@return 当前渲染系统的视矩阵
		*/
		virtual const Matrix4&		getViewMatrix();			

		/** 获取世界矩阵
		@return 当前渲染系统的世界矩阵
		*/
		virtual const Matrix4&		getWorldMatrix();			

		/** 获取视图世界矩阵
		@return 视图矩阵和世界矩阵的Multiply
		*/
		virtual const Matrix4&		getModelViewMatrix();		

		/** 开始绘制一桢
		@param clearColorBuffer 是否清除颜色缓存
		@param clearDepthBuffer 是否清除深度缓存
		@param clearStencilBuffer 是否清除模板缓存
		*/
		virtual void 		beginFrame(bool clearColorBuffer = true,bool clearDepthBuffer = true,bool clearStencilBuffer = false);

		/** 结束绘制一桢,和beginFrame配套使用
		*/
		virtual void 		endFrame(int layer);							

		/** 设置是否可以写深度缓存
		@param enabled true则允许,false则不允许,默认为true
		*/
		virtual void 		setDepthBufferWriteEnabled(bool enabled);			

		/** 获取是否可以写深度缓存
		@return true表示允许,false则不允许
		*/
		virtual bool		isDepthBufferWriteEnabled();		

		/** 设置是否打开灯光
		@param enabled true则允许,false则不允许,默认为false
		*/
		virtual void 		setLightingEnabled(bool enabled);	

		/** 获取灯光是否开启
		@return true表示开启,false则关闭
		*/
		virtual bool		isLightingEnabled();				

		/** 设置填充模式
		@param sdl 填充模式
		@see FillMode
		*/
		virtual void 		setFillMode(const FillMode& sdl);	

		/** 获取填充模式
		@return 填充模式
		@see FillMode
		*/
		virtual const FillMode&	getFillMode();					

		/** 设置线条的宽度
		@param width 线条宽度，在beginPrimitive(PT_LINES)等有效
		*/
		virtual void setLineWidth(float width);

		/** 获取线条的宽度
		@return 线条宽度
		*/
		virtual float getLineWidth();

		/** 设置是否允许深度校验
		@param enabled true则允许,false则不允许,默认为false
		*/
		virtual void 		setDepthBufferCheckEnabled(bool enabled);			

		/** 获取当前系统是否允许深度校验
		@return true表示允许,false则不允许
		*/
		virtual bool		isDepthBufferCheckEnabled();		

		/** 设置深度校验函数
		@param cf 深度校验函数,默认为CMPF_LESS
		@see CompareFunction
		*/
		virtual void 		setDepthBufferFunction(const CompareFunction& cf);	

		virtual const CompareFunction& getDepthBufferFunction();

		/** 获取Alpha校验函数和参考值
		@param cf Alpha校验函数
		@param value 参考值
		@see CompareFunction
		*/
		virtual void 		getAlphaFunction(CompareFunction& cf,float& value);	

		/** 设置Alpha校验函数
		@param cf Alpha校验函数,默认为CMPF_ALWAYS
		@param value 参考值,当cf为一个需要参考值的函数时
		@see CompareFunction
		*/
		virtual void 		setAlphaFunction(const CompareFunction& cf,float value);

		/** 设置Alpha校验函数
		@param cf Alpha校验函数,默认为CMPF_ALWAYS
		@param value 参考值,value / 255和另一个重载是一样的
		@see CompareFunction
		*/
		virtual void 		setAlphaFunction(const CompareFunction& cf,uchar value);

		/** 获取是否允许Alpha校验
		@return true表示允许,false则不允许
		*/
		virtual bool		alphaCheckEnabled();				

		/** 设置是否允许Alpha校验
		@param bEnabled true表示允许,false不允许,默认为false
		@param value 参考值,当cf为一个需要参考值的函数时
		*/
		virtual void 		setAlphaCheckEnabled(bool bEnabled);

		/** 设置背面裁减方式
		@param cm 裁减方式
		@see CullingMode
		*/
		virtual void 		setCullingMode(const CullingMode& cm);

		/** 设置边标记
		@param bEdgeFlag true则打开边标记,false则关闭,默认为true
		*/
		virtual void 		setEdgeFlag(bool bEdgeFlag);	

		/** 获取边界标记
		@param bEdgeFlag true则打开边标记,false则关闭,默认为true
		*/
		bool	getEdgeFlag();

		/** 获取背面裁减方式
		@return 背面裁减方式
		@see CullingMode
		*/
		virtual const		CullingMode& getCullingMode();		

		/** 设置场景混合参数
		@param src 源混合参数,默认为SBF_ONE
		@param dst 目标混合参数,默认为SBF_ZERO
		@see SceneBlendFactor
		*/
		virtual void 		setSceneBlending(const SceneBlendFactor& src,const SceneBlendFactor& dst);

		/** 获取场景混合参数
		@param src 源混合参数
		@param dst 目标混合参数
		@see SceneBlendFactor
		*/
		virtual void 		getSceneBlending(SceneBlendFactor& src,SceneBlendFactor& dst);			

		/** 重新设置渲染参数,主要是在窗口模式和全屏模式中切换
		@param param 渲染参数
		@see RenderEngineCreationParameters
		@return true表示成功,反之失败
		*/
		virtual bool		switchDisplayMode(RenderEngineCreationParameters* param);				

		/** 获取缓冲区管理器指针
		@return 缓冲区管理器指针
		*/
		virtual IBufferManager*		getBufferManager();			

		/** 获取贴图管理器指针
		@return 贴图管理器指针
		*/
		virtual ITextureManager*	getTextureManager();		

		/** 设置当前绘图颜色,在有灯光的情况下,颜色无效
		@param color 颜色值
		@see ColorValue
		*/
		virtual void 		setColor(const ColorValue& color);	

		/** 获取当前绘图颜色
		@return 颜色值
		*/
		virtual const ColorValue& getColor();					

		/** 设置当前法线值
		@param normal 法线
		@see Vector3
		*/
		virtual void 		setNormal(const Vector3& normal);	

		/** 获取当前法线值
		@param normal 法线
		@see Vector3
		*/
		const Vector3&		getNormal();

		/** 设置当前贴图坐标
		@param texcoord 贴图坐标
		*/
		virtual void 		setTexcoord(const Vector2& texcoord);

		/** 设置当前贴图坐标
		@param texcoord 贴图坐标
		*/
		virtual void 		setMultiTexcoord(ushort unit,const Vector2& texcoord);

		/** 获取当前贴图坐标
		@param texcoord 贴图坐标
		*/
		const Vector2&			getMultiTexcoord(ushort unit);

		/** 发送顶点
		@param vertex 顶点坐标
		*/
		virtual void 		sendVertex(const Vector2& vertex);	

		/** 发送顶点
		@param vertex 顶点坐标
		*/
		virtual void 		sendVertex(const Vector3& vertex);	

		/** 发送顶点
		@param vertex 顶点坐标
		*/
		virtual void 		sendVertex(const Vector4& vertex);	

		/** 开始绘制一个形状
		@param pt 绘图类型
		@see PrimitiveType
		*/
		virtual void 		beginPrimitive(PrimitiveType pt);	

		/** 结束绘制一个形状,和beginPrimitive配套使用
		*/
		virtual void 		endPrimitive();						

		/** 切换到2D渲染状态
		@param left 视口左
		@param top 视口上
		@param width 视口宽
		@param height 视口高
		*/
		virtual void 		switchTo2D(int left,int top,int width,int height);				

		/** 以当前视口为参数切换到2D渲染状态
		*/
		virtual void 		switchTo2D();							

		/** 切换到3D渲染状态,和begin2D配套使用
		*/
		virtual void 		switchTo3D();							

		/** 当前是否处于2D状态
		@return true表示处于2D状态,false则不是
		*/
		virtual bool		is2dMode();							
		/*Slow*/																						
		virtual void 		render(const RenderOperation& ro);	
		/*For Convenience Only*/																		
		virtual void 		render(const RenderDirect& rd);		

		/** 设置贴图寻址方式
		@param unit 贴图的层索引,从0开始
		@param tamS S方向的寻址方式
		@see TextureAddressingMode
		@param tamT T方向的寻址方式
		*/
		virtual void 		setTextureWrap(ushort unit,const TextureAddressingMode& tamS,const TextureAddressingMode& tamT);			

		/** 设置贴图寻址方式
		@param unit 贴图的层索引,从0开始
		@param tam S方向的寻址方式
		*/
		virtual void 		setTextureWrapS(ushort unit,const TextureAddressingMode& tam);			
		virtual TextureAddressingMode	getTextureWrapS(ushort unit);
		virtual TextureAddressingMode	getTextureWrapT(ushort unit);

		/** 设置贴图寻址方式
		@param unit 贴图的层索引,从0开始
		@param tam T方向的寻址方式
		*/
		virtual void 		setTextureWrapT(ushort unit,const TextureAddressingMode& tam);			

		/** 设置贴图坐标计算方式
		@param unit 贴图的层索引,从0开始
		@param m 贴图坐标计算方式枚举
		@see TexCoordCalcMethod
		*/
		virtual void 		setTextureCoordCalculation(ushort uint,TexCoordCalcMethod m);			

		/** 设置第N个贴图单元的贴图
		@param unit 贴图的层索引,从0开始
		@param pTexture 贴图指针
		*/
		virtual void 		setTexture(ushort unit,ITexture* pTexture);			

		/** 设置全局环境光颜色
		@param color 环境光颜色
		*/
		virtual void 		setAmbientLight(const ColorValue& color);			

		/** 获取全局环境光颜色
		@return 环境光颜色
		*/
		virtual const ColorValue& getAmbientLight();			

		/** 设置贴图坐标顶点缓冲区
		@param unit 贴图的层索引,从0开始
		@param pVB 顶点缓冲区的地址
		@param start 开始顶点的下标,可以不从第一个开始
		*/
		virtual void 		setTexcoordVertexBuffer(ushort unit,IVertexBuffer* pVB,uint start = 0,uint stride = 0);

		/** 设置位置顶点缓冲区
		@param pVB 顶点缓冲区的地址
		@param start 开始顶点的下标,可以不从第一个开始
		*/
		virtual void 		setVertexVertexBuffer(IVertexBuffer* pVB,uint start = 0,uint stride = 0);				

		/** 设置法线顶点缓冲区
		@param pVB 顶点缓冲区的地址
		@param start 开始顶点的下标,可以不从第一个开始
		*/
		virtual void 		setNormalVertexBuffer(IVertexBuffer* pVB,uint start = 0,uint stride = 0);				

		/** 设置颜色顶点缓冲区
		@param pVB 顶点缓冲区的地址
		@param start 开始顶点的下标,可以不从第一个开始
		*/
		virtual void 		setDiffuseVertexBuffer(IVertexBuffer* pVB,uint start = 0,uint stride = 0);			

		/** 设置镜面光顶点缓冲区
		@param pVB 顶点缓冲区的地址
		@param start 开始顶点的下标,可以不从第一个开始
		*/
		virtual void 		setSpecularVertexBuffer(IVertexBuffer* pVB,uint start = 0,uint stride = 0);		

		/** 设置索引缓冲区
		@param pIB 索引缓冲区的地址
		*/
		virtual void 		setIndexBuffer(IIndexBuffer* pIB);	

		/** 绘制带索引的几何形状
		@param pt 几何形状的类型
		@see PrimitiveType
		@param ui32IndexCount 索引数目
		@param it 索引类型
		@see IndexType
		*/
		virtual void 		drawIndexedPrimitive(PrimitiveType pt,uint ui32IndexCount,IndexType it,void* pdata=0);

		/** 绘制带索引的带范围的几何形状
		@param pt 几何形状的类型
		@see PrimitiveType
		@param ui32IndexStart 开始的索引下标
		@param ui32IndexCount 索引数目
		@param ui32VertexStart 开始的顶点下标
		@param ui32VertexEnd 结束的顶点下标
		@param it 索引类型
		@see IndexType
		*/
		virtual void 		drawRangeIndexedPrimitive(													
			PrimitiveType pt,														
			IndexType it,															
			uint ui32IndexStart,													
			uint ui32IndexCount,													
			uint ui32VertexStart,													
			uint ui32VertexEnd);			

		/** 绘制不带索引的几何形状
		@param pt 几何形状的类型
		@param ui32VertexStart 开始的顶点下标
		@param ui32VertexCount 顶点数目
		@see PrimitiveType
		*/
		virtual void 		drawPrimitive(PrimitiveType pt,uint ui32VertexStart,uint ui32VertexCount);

		/** 设置雾参数
		@param mode 雾的类型
		@see FogMode
		@param color 颜色
		@param expDensity EXP类型的雾参数
		@param linearStart 线性雾的开始距离
		@param linearEnd 线性雾的结束距离
		@see ColorValue
		*/
		virtual void 		setFog(FogMode mode,														
			const ColorValue& color = ColorValue::Black,						
			float expDensity = 0.001,											
			float linearStart = 0.0,												
			float linearEnd = 1.0);

		virtual void	enableFog(bool enable);

		virtual bool	isFogEnabled();

		/** 设置灯光
		@param unit 灯光的单元,表示要设置第几盏灯
		@param pLight 灯光数据
		@see Light
		*/
		virtual void 		setLight(ushort unit,Light* pLight);

		const ColorValue&	getSurfaceDiffuse();

		virtual void setSurfaceDiffuse(const ColorValue &diffuse);

		/** 设置材质参数颜色镜面光参数
		@param ambient 环境光颜色
		@param diffuse 漫反射光颜色
		@param specular 镜面光颜色
		@param emissive 自身发射光颜色
		@param shininess Specular exponent of material
		@see ColorValue
		*/
		virtual void 		setSurfaceParams(															
			const ColorValue &ambient,											
			const ColorValue &diffuse,											
			const ColorValue &specular,											
			const ColorValue &emissive,											
			float shininess);			

		/** 设置setColor函数影响的颜色类型,Ambient,Diffuse等
		@param tracking 颜色类型枚举
		@see TrackVertexColorEnum
		*/
		virtual void 		setTrackingColorEnum(TrackVertexColorEnum tracking);

		/** 设置当前点的大小
		@param size 点大小
		*/
		virtual void 		setPointSize(float size);			

		/** 设置附加裁减面
		@param index 下标,第几个裁减面
		@param pPlane 面数据
		*/
		virtual void 		setClipPlane(uint index,const Plane* pPlane);				

		/** 根据当前的矩阵数据,反向投影一个投影坐标系中的点到世界坐标系
		@param v 投影坐标系中的点
		@return 经过反向投影后的点坐标,世界坐标系
		*/
		virtual Vector3		unproject(const Vector3& v);		

		/** 根据当前的矩阵数据,投影一个局部坐标系中的点
		@param v 局部坐标系中的点
		@return 经过投影后的点坐标
		*/
		virtual Vector3		project(const Vector3& v);			

		/** 设置是否打开垂直同步
		@param vsync true则打开垂直同步,false则关闭,默认为false
		*/
		virtual void 		setVerticalSync(bool vsync);		

		/** Turns stencil buffer checking on or off.
		@remarks
		Stencilling (masking off areas of the rendering target based on the stencil 
		buffer) canbe turned on or off using this method. By default, stencilling is
		disabled.
		*/
		virtual void  setStencilCheckEnabled(bool enabled);		

		/** This method allows you to set all the stencil buffer parameters in one call.
		@remarks
		The stencil buffer is used to mask out pixels in the render target, allowing
		you to do effects like mirrors, cut-outs, stencil shadows and more. Each of
		your batches of rendering is likely to ignore the stencil buffer, 
		update it with new values, or apply it to mask the output of the render.
		The stencil test is:<PRE>
		(Reference Value & Mask) CompareFunction (Stencil Buffer Value & Mask)</PRE>
		The result of this will cause one of 3 actions depending on whether the test fails,
		succeeds but with the depth buffer check still failing, or succeeds with the
		depth buffer check passing too.
		@par
		Unlike other render states, stencilling is left for the application to turn
		on and off when it requires. This is because you are likely to want to change
		parameters between batches of arbitrary objects and control the ordering yourself.
		In order to batch things this way, you'll want to use OGRE's separate render queue
		groups (see RenderQueue) and register a RenderQueueListener to get notifications
		between batches.
		@par
		There are individual state change methods for each of the parameters set using 
		this method. 
		Note that the default values in this method represent the defaults at system 
		start up too.
		@param func The comparison function applied.
		@param refValue The reference value used in the comparison
		@param mask The bitmask applied to both the stencil value and the reference value 
		before comparison
		@param stencilFailOp The action to perform when the stencil check fails
		@param depthFailOp The action to perform when the stencil check passes, but the
		depth buffer check still fails
		@param passOp The action to take when both the stencil and depth check pass.
		@param twoSidedOperation If set to true, then if you render both back and front faces 
		(you'll have to turn off culling) then these parameters will apply for front faces, 
		and the inverse of them will happen for back faces (keep remains the same).
		*/
		virtual void  setStencilBufferParams(CompareFunction func = CMPF_ALWAYS_PASS,					
			uint refValue = 0, uint mask = 0xFFFFFFFF,												
			StencilOperation stencilFailOp = SOP_KEEP,													
			StencilOperation depthFailOp = SOP_KEEP,													
			StencilOperation passOp = SOP_KEEP);				

		/** 设置颜色缓存的Red,Green,Blud,Alpha位是否可以写
		@param red true则可以写颜色缓存red位
		@param green true则可以写颜色缓存green位
		@param blue true则可以写颜色缓存blue位
		@param alpha true则可以写颜色缓存alpha位
		*/
		virtual void setColorBufferWriteEnabled(bool red, bool green, bool blue, bool alpha);

		/**开始进行场景选取
		@param ptX 屏幕坐标x
		@param ptY 屏幕坐标y
		*/
		virtual void beginSelection(int ptX,int ptY,uint variance);

		/**结束场景选取，将选取的结果放入pBuffer，num带回选中的数目，不超过64个
		@param num[IN,OUT] 带入pBuffer的大小，返回选中的数目
		@param pBuffer 返回选中的结果
		*/
		virtual void endSelection(int& num,int* pBuffer);

		/**设置选择索引
		@param index 索引
		*/
		virtual void setSelectionIndex(int index);

		/**添加RenderTarget
		@param hwnd 窗口句柄
		@return 是否成功
		*/
		virtual bool addRenderTarget(uint hwnd);

		/**删除渲染目标
		@param hwnd 窗口句柄
		@return 是否成功
		*/
		virtual bool removeRenderTarget(uint hwnd);

		/**多线程添加RenderTarget
		@param hwnd 窗口句柄
		@return 是否成功
		*/
		virtual uint MTaddRenderTarget(int layer);

		/**多线程删除渲染目标
		@param hwnd 窗口句柄
		@return 是否成功
		*/
		virtual bool MTremoveRenderTarget(uint key);

		/**添加Overlay RenderTarget
		@note 只能创建一个
		*/
		virtual bool addOverlayRenderTarget();

		/**删除Overlay RenderTarget
		*/
		virtual void removeOverlayRenderTarget();

		/**把Overlay RenderTarget设置为当前RenderTarget
		*/
		virtual bool setOverlayRenderTarget();

		/**获得当前的渲染目标
		@return 当前的渲染目标的窗口句柄，或者是RTT的句柄
		*/
		virtual uint getCurrentRenderTarget();

		/**设置当前的渲染目标
		@param hwnd 窗口句柄或者RTT句柄
		@return 是否成功
		*/
		virtual bool setCurrentRenderTarget(uint hwnd);

		/**增加一个RTT
		@param width 宽度
		@param height 高度
		@param min 最小滤波器
		@param mag 最大滤波器
		@param mip mipmap滤波方式
		@param s 纹理s寻址方式
		@param t 纹理t寻址方式
		@return 生成的RTT的句柄
		*/
		virtual uint addRTT(int width,int height, bool alpha = false,
			FilterOptions min = FO_LINEAR, 
			FilterOptions mag = FO_LINEAR, 
			FilterOptions mip = FO_NONE,
			TextureAddressingMode s = TAM_WRAP,											
			TextureAddressingMode t = TAM_WRAP);

		/**获得RTT贴图指针
		@param hwnd RTT句柄，对于窗口句柄总是返回0
		@return 贴图指针
		*/
		virtual const ITexture*	getRenderTargetTexture(uint hwnd);

		/**得到RTT贴图数据
		@param hwnd RTT句柄，对于窗口句柄无效
		@param pData 返回的缓冲区
		@return 是否成功
		@remarks 此函数用于地图编辑器绘制小地图
		*/
		virtual bool getRenderTargetTextureData(uint hwnd,void  *pData);

		virtual bool isScissorEnabled();

		virtual void setScissorEnabled(bool enabled);


		/**构造一个视频对象
		@param pszFilename 视频文件名字
		@param type 视频文件类型，见enumVideoObjectType
		@param imgWidth 生成的纹理的宽度，只有提示作用
		@param imgHeight 生成的纹理的高度，只有提示作用
		@return 视频对象
		*/
		virtual IVideoObject * createVideoObject(const char * pszFilename, uint type, uint imgWidth, uint imgHeight );

		/** 以size为大小在当前位置画一个茶壶
		@param size 大小
		*/
		virtual void 		teapot(float size,const ColorValue& color);

		virtual void		line(const Point& ptFirst,const Point& ptSecond,const ColorValue& color);

		virtual void		rectangle(const Rect& rc,const ColorValue& color);

		virtual void		rectangle(const Rect& rc,const ColorValue& cLeftTop,const ColorValue& cLeftBottom,const ColorValue& cRightTop,const ColorValue& cRightBottom);

		virtual void		point(const Point& pt,const ColorValue& color);

		virtual void		rectangle(const Rect& rc,ITexture* pTexture);

		virtual void		rectangle(const Rect& rc,ITexture* pTexture, const Vector2 & vLetfTop, const Vector2 & vLeftBottom, const Vector2 & vRightBottom, const Vector2 & RightTop);

		virtual void		box(const Rect& rc,const ColorValue& color);

		virtual const RenderMode&	getRenderMode();

		virtual  RenderSystemType getRenderSystemType();

		virtual bool		setTextureStageStatus(	uint stage,
													TextureStageOperator colorOP,
													TextureStageArgument colorArg0,
													TextureStageArgument colorArg1,
													TextureStageOperator alphaOP,
													TextureStageArgument alphaArg0,
													TextureStageArgument alphaArg1);

		virtual void		getTextureStageStatus(	uint stage,
													TextureStageOperator & colorOP,
													TextureStageArgument & colorArg0,
													TextureStageArgument & colorArg1,
													TextureStageOperator & alphaOP,
													TextureStageArgument & alphaArg0,
													TextureStageArgument & alphaArg1);

		virtual IHardwareCursorManager * getHardwareCursorManager();

		/** 设置顶点混合的权重顶点缓冲区
		@param pVB 顶点缓冲区的地址
		@param start 开始顶点的下标,可以不从第一个开始
		*/
		virtual void		setBlendWeightVertexBuffer(IVertexBuffer* pVB, uint start=0);

		/** 设置顶点混合的矩阵索引顶点缓冲区
		@param pVB 顶点缓冲区的地址
		@param start 开始顶点的下标,可以不从第一个开始
		*/
		virtual void		setBlendIndicesVertexBuffer(IVertexBuffer* pVB, uint start=0);

		/** 获取着色器程序管理器指针
		@return 着色器程序管理器指针
		*/
		virtual IShaderProgramManager* getShaderProgramManager();

		/** 清除帧缓存
		@param clearColorBuffer 是否清除颜色缓存
		@param clearDepthBuffer 是否清除深度缓存
		@param clearStencilBuffer 是否清除模板缓存
		*/
		virtual void 		clearFrameBuffer(bool clearColorBuffer = true,bool clearDepthBuffer = true,bool clearStencilBuffer = false);

		/** 获取当前的显示模式
		@param createParam 显示模式参数
		*/
		virtual bool		getDisplayMode(RenderEngineCreationParameters & createParam);

		virtual IShaderProgram* getShaderProgram(int id){return 0;}

		virtual void		bindCurrentShaderProgram(IShaderProgram* pShaderProgram,bool setShaderConst = false);
	public:
		RenderSystemD3D9();
		bool	create(RenderEngineCreationParameters *);
		void	getTransformMatrix(TransformMatrixType type, D3DXMATRIX & mtx);
	private:
		void	close();
		bool resetRenderState();
		inline DWORD  F2DW(float f) { return *((DWORD*)&f);}

	public:
		inline IDirect3D9 * getDirect3D9() {return m_pcsDirect3D9;}
		inline D3D9Driver * getD3D9Driver(){return m_pcsD3D9DDriver;}
		inline D3D9Device * getD3D9Device(){return m_pcsD3D9Device;}
		inline D3D9ResourceManager * getD3D9ResourceMgr(){ return m_pcsD3D9ResourceMgr;}
		inline RenderTargetManagerD3D9 * getRenderTargetMgr(){return m_pcsRenderTargetMgr;}
		inline TextureManagerD3D9	* getTextureMgr() { return m_pcsTextureMgr; }
		inline ShaderProgramManagerD3D9* getShaderProgramMgr() { return m_pcsShaderProgramMgr;}
		inline RenderSystemD3D9State * getRenderSystemState() { return &m_csRenderState; }
	private: //渲染系统的管理器
		IDirect3D9 * m_pcsDirect3D9;	//D3D9对象
		D3D9Driver * m_pcsD3D9DDriver;		//D3D9驱动程序
		D3D9Device * m_pcsD3D9Device;		//D3D9设备
		D3D9ResourceManager * m_pcsD3D9ResourceMgr; //D3D9资源管理器
		RenderTargetManagerD3D9 * m_pcsRenderTargetMgr;//渲染目标管理器
		BufferManagerD3D9 * m_pcsBufferMgr;	//Buffer管理器
		StreamSourceMappings * m_pcsStreamSourceMappings;//用于将VertexBuffer映射到指定的数据流端口
		BeginEndPrimitiveProxy * m_pcsBeginEndPrimitiveProxy;//带来opengl方式的beginPrimitive/endPrimitive绘制方式
		RenderSystemD3D9State	m_csRenderState;//保存渲染系统的状态
		TextureManagerD3D9		* m_pcsTextureMgr;//纹理管理器
		ShaderProgramManagerD3D9*	m_pcsShaderProgramMgr;//着色器程序管理器
		HardwareCursorManagerD3D9 * m_pcsHardwareCursorMgr;//硬件鼠标管理器

	private: //Misc
		ID3DXMesh				* m_pTeapot;//茶壶


	private: //渲染系统指针
		typedef std::list<RenderSystemD3D9 * > RenderSystemContainer;
		typedef RenderSystemContainer::iterator RenderSystemContainerIterator;
		static RenderSystemContainer m_vRenderSystem;//渲染系统
	};

}

#endif