//==========================================================================
/**
* @file	  : VisualComponentMPW.h
* @author : 
* created : 2008-1-30   12:56
* purpose : 可视化组件(MPW)
*/
//==========================================================================

#ifndef __VISUALCOMPONENTMPW_H__
#define __VISUALCOMPONENTMPW_H__

#include "VisualComponent.h"
#include "Occupant.h"
#include "MwdInfo.h"
#include "MwdInfoReader.h"


class VisualComponentMPW : public VisualComponent, public Occupant
{
	//CAniInfoHeader*	mAniInfo;	/// mpw数据脚本
	const MwdInfo*		mMwdInfo;
public:
	VisualComponentMPW();
	virtual ~VisualComponentMPW();

	virtual void create();
	virtual void close();

	virtual void onEnterViewport(int priority = 0);
	virtual void onLeaveViewport();
	virtual void drawPickObject(IRenderSystem* pRenderSystem);
	virtual void draw(IRenderSystem* pRenderSystem);
	virtual bool update(float tick, float deltaTick, IRenderSystem* pRenderSystem);
	void requestRes(int priority);
	void releaseRes();
	virtual OccupantTileList* getOccupantTileList() const { return Occupant::getOccupantTileList(); }
	virtual size_t onLoad(Stream* stream, size_t len);
	virtual size_t onSave(Stream* stream) const;
	virtual bool isTransparent(int x, int y);
	//virtual void onAddEntity();
	//virtual void onRemoveEntity();

	virtual void handleMessage(ulong msgId, ulong param1, ulong param2);
protected:
	bool readMWD();
	void setOccupantInfo();
	inline Entity2DAniPack* getRes();
	void onResLoaded(Entity2DAniPack* node);
};


#endif // __VISUALCOMPONENTMPW_H__