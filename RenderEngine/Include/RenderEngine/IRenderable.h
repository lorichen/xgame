#ifndef __I_Renderable_H__
#define __I_Renderable_H__

namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	struct IRenderSystem;
	class Material;

	/** 可渲染接口
	*/
	struct IRenderable
	{
		/** 渲染
		@param pRenderSystem 渲染系统接口指针
		*/
		virtual void  render(IRenderSystem* pRenderSystem,bool useMtl = true) = 0;
	};
	/** @} */
}

#endif