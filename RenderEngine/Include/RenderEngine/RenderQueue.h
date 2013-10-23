#ifndef __RenderQueue_H__
#define __RenderQueue_H__

#include <set>
#include "IRenderable.h"

namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	struct IRenderable;

	/** 渲染队列，接受IRenderable类型对象
	*/
	class _RenderEngineExport RenderQueue
	{
	public:
		RenderQueue();
		void 	clear();
		void 	add(IRenderable* pRenderable,int renderOrder);
		void 	render(IRenderSystem* pRenderSystem,bool useMtl);
	private:
		typedef std::vector<IRenderable*> RenderableList;
		RenderableList	m_vRenderableList[RENDER_QUEUE_COUNT];
	};
	/** @} */

}
#endif