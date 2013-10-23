#ifndef __RenderEngine_H__
#define __RenderEngine_H__

#include "IRenderEngine.h"
#include "RenderSystemHelper.h"
#include "Model\ModelManagerHelper.h"

namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/


class RenderEngine : public IRenderEngine
{
public:
	/** 渲染引擎销毁
	*/
	virtual void 				release();											

	/** 获取渲染系统接口
	@return 渲染系统接口
	*/
	virtual IRenderSystem*		getRenderSystem();												

	/** 获取模型管理接口
	@return 模型管理接口
	*/
	virtual IModelManager*		getModelManager();		

	/** 获取字体管理接口
	@return 字体管理接口
	*/
	virtual IFontManager*		getFontManager();			

	/** 创建摄像机
	@param pViewport 视口
	@return 摄像机指针
	*/
	virtual Camera*				createCamera(Viewport* pViewport);					

	/** 创建视口
	@return 视口指针
	*/
	virtual Viewport*			createViewport();

	virtual const RenderEngineCreationParameters& getCreationParams();

public:
	RenderEngine();
	~RenderEngine();

public:
	IRenderSystem*		m_pRenderSystem;
	IModelManager*		m_pModelManager;

private:
	RenderSystemHelper	m_RenderSystem;
	ModelManagerHelper	m_ModelManager;
	RenderEngineCreationParameters	m_param;

	IModelManager*		createModelManager();
	void 				closeRenderSystem();
	virtual IRenderSystem*		createRenderSystem(RenderEngineCreationParameters *param);

public:
	bool			create(RenderEngineCreationParameters* param);
};
	/** @} */

}

#endif