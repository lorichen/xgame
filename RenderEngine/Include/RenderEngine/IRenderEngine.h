#ifndef __I_RenderEngine_H__
#define __I_RenderEngine_H__

struct ISceneManager2;
namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	struct IRenderSystem;
	class Camera;
	class Viewport;
	class TextureManager;
	class BufferManager;
	struct ISceneEnv;
	struct IModelManager;
	struct ICynthiaScene;
	struct IFontManager;
	struct IModelInstanceManager;

	/**渲染系统类型
	*/
	enum RenderSystemType
	{
		/// OpenGL
		RS_OPENGL,

		/// D3D(尚未实现)
		RS_D3D9,
		RS_COUNT
	};


	/** RenderEngine Creation Flags
	*/

	struct RenderEngineCreationParameters
	{
		/// 渲染系统类型RenderSystemType
		RenderSystemType	rst;

		/// 是否全屏
		bool				fullscreen;	

		/// 是否开启StencilBuffer
		bool				stencilBuffer;

		/// 窗口句柄，不能为空
		void *				hwnd;

		/// 窗口宽度
		uint				w;

		/// 窗口高度
		uint				h;

		/// 刷新率
		uint				refreshRate;

		/// 颜色深度
		uint				colorDepth;

		/// 垂直同步
		bool				vertcialSync;

		RenderEngineCreationParameters() : hwnd(0),rst(RS_OPENGL),w(800),h(600),fullscreen(false),refreshRate(85),colorDepth(32),vertcialSync(true)
		{
		}
	};
	

	struct IResourceManager;
/** 渲染引擎接口
*/
struct IRenderEngine
{
	/** 渲染引擎销毁
	*/
	virtual void 				release() = 0;											

	/** 获取渲染系统接口
	@return 渲染系统接口
	*/
	virtual IRenderSystem*		getRenderSystem() = 0;									

	/** 获取模型管理接口
	@return 模型管理接口
	*/
	virtual IModelManager*		getModelManager() = 0;

	/** 获取字体管理接口
	@return 字体管理接口
	*/
	virtual IFontManager*		getFontManager() = 0;									

	/** 创建摄像机
	@param pViewport 视口
	@return 摄像机指针
	*/
	virtual Camera*				createCamera(Viewport* pViewport) = 0;					

	/** 创建视口
	@return 视口指针
	*/
	virtual Viewport*			createViewport() = 0;

	virtual const RenderEngineCreationParameters& getCreationParams() = 0;
};

/** 创建渲染引擎
@param param 引擎的创建参数
@return 渲染引擎指针
*/
extern "C" __declspec(dllexport) IRenderEngine* LoadRenderEngine(RenderEngineCreationParameters *param);

/** 获取渲染引擎接口
@return 渲染引擎指针
*/
extern "C" __declspec(dllexport) IRenderEngine* getRenderEngine();


extern "C" __declspec(dllexport) void registerImageCodec();

	/** @} */
}

#endif