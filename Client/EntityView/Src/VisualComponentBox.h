//==========================================================================
/**
* @file	  : VisualComponentBOX.h
* @author : 
* created : 2008-1-30   12:56
* purpose : 可视化组件(MPW)
*/
//==========================================================================

#ifndef __VisualComponentBOX_H__
#define __VisualComponentBOX_H__

#include "VisualComponent.h"


class VisualComponentBOX : public VisualComponent
{
private:
	bool	m_bIsInit;
	string  m_imageName;
	ITexture * m_pTexture;
	//xsgui::Imageset * m_pImageSet;
	bool m_bHasGuiException;
	long m_nlastTick;
	int m_nEffectID;

public:
	VisualComponentBOX();
	virtual ~VisualComponentBOX();

	virtual void create();
	virtual void close();

	virtual void onEnterViewport(int priority = 0);
	virtual void onLeaveViewport();
	virtual void draw(IRenderSystem* pRenderSystem);

	virtual const xs::Rect& getShowRect() const						{ return mShowRect; }

	void requestRes(int priority);
	void releaseRes();
	virtual size_t onLoad(Stream* stream, size_t len);
	virtual bool update(float tick, float deltaTick, IRenderSystem* pRenderSystem);
	virtual void handleMessage(ulong msgId, ulong param1, ulong param2);
private:	
	bool InitBigImageSet();

};


#endif // __VisualComponentBOX_H__