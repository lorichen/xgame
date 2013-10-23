#include "StdAfx.h"
#include "RenderQueue.h"

namespace xs
{

	RenderQueue::RenderQueue()
	{
	}

	void 	RenderQueue::clear()
	{
		for(size_t i = 0;i < RENDER_QUEUE_COUNT;i++)
		{
			m_vRenderableList[i].clear();
		}
	}

	void 	RenderQueue::add(IRenderable* pRenderable,int renderOrder)
	{
		m_vRenderableList[renderOrder].push_back(pRenderable);
	}

	void 	RenderQueue::render(IRenderSystem* pRenderSystem,bool useMtl)
	{
		PP_BY_NAME( "RenderQueue::render" );
		for(size_t i = 0;i < RENDER_QUEUE_COUNT;i++)
		{
			RenderableList::iterator end = m_vRenderableList[i].end();
			for(RenderableList::iterator it = m_vRenderableList[i].begin();it != end;++it)
			{
				IRenderable *pRenderable = (*it);
				pRenderable->render(pRenderSystem,useMtl);
			}
		}
	}
}