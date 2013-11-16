//==========================================================================
/**
* @file	  : VisualComponent.h
* @author : 
* created : 2008-1-30   12:56
* purpose : 可视化组件
*/
//==========================================================================

#ifndef __VISUALCOMPONENT_H__
#define __VISUALCOMPONENT_H__

#include "EntityViewImpl.h"
#include "EntityComponent.h"
#include "PacketFormat.h"
//#include "TxSysGui.h"
#include "ITaskClient.h"

class VisualComponent : public EntityComponent
{
protected:
	// 资源句柄
	handle			mHandle;

	// 标识资源已经加载
	bool			mResLoaded;

	// 实体外围矩形
	xs::Rect			mShowRect;

	// 实体的名字
	autostring		mName;

	// 名字颜色
	ColorValue		mNameColor;

	// 名字相对实体的高度偏移
	float			mNameOffset;

	// 是否需要更新（一般发生在位置、动画、方向、缩放、速度等改变时）
	ulong			mUpdateOption;

	// 是否移动方式为奔跑
	bool			mIsRun;

	// 用于淡入淡出的alpha值
	float			mAlpha;

	// 当前HP和MP，当前最大HP和MP
	ulong			mCurHP, mMaxHP, mCurMP, mMaxMP;

	// 走路初始设置最大走路速度
	ulong           m_ulInitWalkSpeed;

	// 走路初始设置最大跑步速度
	ulong           m_ulInitRunSpeed;

	// 称号
	autostring		mTitleName;
	
	// 额外显示的图片文字组
	list<TextLine>	m_listText;

	// 特效字体的计时器
	long			mTextEffectTickCount;

protected:
	//  当前需要绘制的任务标记；
	EEntityTaskSign m_CurrentTaskSign;

	//  当前任务图标的位置
	float    m_foffset;

	//  是否重置位置
	bool     m_bRset;

	//crr add，任务标记贴图是否已经加载，避免VisualComponent::drawAlwayseTopMost使用static变量不可重入
	ITexture* m_pTaskSignTexture;

private:
	// 定义成成员变量，解决返回选择角色时崩溃的问题
	ITexture* m_pHPTexture;
	//xsgui::Imageset* m_pHPImageSet;

	//小图标
	ITexture * m_pIconTexture;
	//xsgui::Imageset * m_pIconImageSet;
	
	// 定义成成员变量，解决返回选择角色时崩溃的问题
	ITexture* m_pTeamTexture;
	//xsgui::Imageset* m_pTeamImageSet;

private:

	// 创建特效模型
	ModelNode * m_pFinshTaskEffect;
	// 创建特效模型
	ModelNode * m_pAutoMoveEffect;
	// 创建特效模型
	ModelNode * m_pFinshAutoMoveEffect;

public:
	//static ITexture*		mSelectedTexture;	/// 实体选择时的贴图

public:
	VisualComponent();
	virtual ~VisualComponent();

	virtual bool handleCommand(ulong cmd, ulong param1, ulong param2);
	virtual void handleMessage(ulong msgId, ulong param1, ulong param2);

	virtual void onEnterViewport(int priority = 0)				{ }
	virtual void onLeaveViewport()								{ }
	virtual void drawPickObject(IRenderSystem* pRenderSystem)	{ }
	virtual void drawShadow(IRenderSystem* pRenderSystem)		{ }
	virtual void draw(IRenderSystem* pRenderSystem)				{ }
	virtual void drawTopMost(IRenderSystem* pRenderSystem);
	//如果子类需要更新才调用更新函数，目前所有子类都调用了，防止出bug
	virtual bool update(float tick, float deltaTick, IRenderSystem* pRenderSystem);
	virtual OccupantTileList* getOccupantTileList() const		{ return NULL; }
	virtual size_t onLoad(Stream* stream, size_t len)			{ return 0; }
	virtual size_t onSave(Stream* stream) const					{ return 0; }
	virtual bool isTransparent(int x, int y)					{ return false; }
	virtual void onAddEntity();
	virtual void onRemoveEntity();
	virtual const xs::Rect& getShowRect() const						{ return mShowRect; }
	virtual void drawAlwayseTopMost(IRenderSystem* pRenderSystem);

	// 绘制顶层特效
	virtual void drawTopMostEffect(IRenderSystem* pRenderSystem);
	virtual const AABB& GetAABB(){return  getOwner()->m_AABB;};
	virtual const Matrix4& GetFullTransform(){return getOwner()->m_FullTransform;}

protected:
	void updateWorldCoord();
	void setFadeIn();
	void setFadeOut();
	void setFadeDie(); //宠物死亡(wan)
};


#endif // __VISUALCOMPONENT_H__
