//==========================================================================
/**
* @file	  : EntityFactory.h
* @author : 
* created : 2008-1-21   17:36
* purpose : 实体工厂
*/
//==========================================================================

#ifndef __ENTITYFACTORY_H__
#define __ENTITYFACTORY_H__

#include "IEntityFactory.h"

class EntityFactory : public IEntityFactory
{
public:
	EntityFactory();
	virtual ~EntityFactory();

	// IEntityFactory
public:
	virtual EntityView* createEntity(ulong entityType, ulong resType, Stream* stream, size_t len);
	virtual EntityView* createEntity(ulong entityType, ulong resType, const uchar* buf, size_t len);

	virtual void flowText(const FlowTextContext& context);
	virtual void flowAreaText(int area, const std::string& text, const ColorValue& color = ColorValue(1,1,1),const DWORD font = 0);

	virtual const std::string GetLastFlowString(FlowAreaType area);

	virtual void updateFlowText(ulong deltaTime);
	virtual void updateSystemTips(ulong deltaTime);
	virtual void renderFlowText(IRenderSystem* pRenderSystem);
	virtual void renderSystemTips(IRenderSystem* pRenderSystem);

	virtual bool createEffect(ulong type, const void* context, size_t len);
	virtual EntityView* createMagic(ulong type, const void* context, size_t len) const;

	virtual bool createBuffEffect(EntityView* view, ulong effId);
	virtual bool createEffect(ulong effId, const xs::Point& ptTile);
	virtual void closeBuffEffect(EntityView* view, ulong effId);

	virtual bool createSkillReadyEffect(const SkillEffectContext& context, bool bPlayAction, bool bHasInterval);
	virtual void closeSkillReadyEffect(EntityView* pSourceView, EntityView* pDestView, ulong id);

    virtual bool CreateSkillBootEffect(const SkillEffectContext& context);
    virtual void CloseSkillBootEffect(EntityView* pSourceView, ulong ulSkillViewID);

	// 创建游戏场景播放的特效，例如下雨、下雪，同一个ID的场景特效不能重复存在。 zgz [4/18/2011 zgz]
	virtual MagicView* CreateSceneMagic(const SceneMagicContext& context);
	// 删除游戏场景播放的特效 
	virtual bool CloseSceneMagic(MagicView* mv);

	/** 创建受击特效
	@param   
	@param   
	@return  
	*/
	virtual bool CreateBeHitEffect(const SkillEffectContext& context);

	/** 创建受击源到目标飞行特效
	@param   
	@param   
	@return  
	*/
	virtual bool CreateBeHitFlyEffect(const SkillEffectContext& context);

	/** 创建跟踪特效
	@param   
	@param   
	@return  
	*/
	virtual bool createSkillTrackEffect(const SkillEffectContext& context);

	virtual bool createSkillAttackEffect(const SkillEffectContext& context);
	virtual void closeSkillAttackEffect(EntityView* view, ulong viewId);

	virtual void release();

	virtual std::string GetFileNameByCreatureId(long id);
	virtual std::string GetFileNameByResId(long id);

	virtual int GetAnimationTime(const RequestAnimationContext& context);

	/** 创建振屏效果
	@param   
	@param   
	@return  
	*/
	virtual void CreateVibrate(ulong ulViewID);

	// 预加载技能特效
	virtual void PreLoadSkillEffect(int nViewID);

	//获取Skill相关数据 add wangtao
	virtual uint GetSkillViewDataId(uint array, uint set);
	virtual const std::string& GetSkillViewDataName(uint id);
	virtual bool GetSkillViewDataArrayIsValid(uint array);
	virtual bool GetSkillViewDataRowIsValid(uint array, uint set);
	virtual uint GetMagicDataId(uint array, uint set);
	virtual const std::string& GetMagicDataName(uint id);
	virtual bool GetMagicDataArrayIsValid(uint array);
	virtual bool GetMagicDataRowIsValid(uint array, uint set);
	virtual const uint GetConfigCreatureResMaxId();
	virtual const uint GetConfigCreatureResId(uint id);
	virtual const std::string GetConfigCreatureResName(uint id);
	virtual const std::string GetSkillViewAttackAct(uint id);
	virtual const uint GetSkillViewSwing(uint id);
	virtual const float GetSkillViewFlySpeed(uint id);
	virtual const uint GetSkillViewEmitEffect(uint id);
	virtual const uint GetSkillViewBlastEffect(uint id);
	virtual const uint GetSkillViewHitEffect(uint id);
	virtual const uint GetSkillViewFlyEffect(uint id);
	virtual const uint GetSkillViewHitFlyEffect(uint id);
	virtual const uint GetSkillViewReadyEffect(uint id);
	virtual const float GetMagicAniSpeed(uint id);
	virtual const uint GetMagicAniTrans(uint id);
	virtual const std::string GetMagicAniOffset(uint id);
	virtual const float GetMagicAniScale(uint id);
	virtual const uint GetMagicResId(uint id);
	virtual void SetSkillViewData(uint id, std::string attackAct, uint swing, float flySpeed, 
		uint emitEffect, uint blastEffect, uint hitEffect, uint flyEffect,
		uint hitFlyEffect, uint readyEffect);
	virtual void SetMagicData(uint id, float aniSpeed, uint aniTrans, std::string aniOffset,
		float aniScale, uint resId);
	virtual void SaveMagicDataToCsv(const char* szFileName);
	virtual void SaveSkillViewDataToCsv(const char* szFileName);

	bool create();
	void close();

protected:
	void	loadRes(int nMagicID);
};

#endif // __ENTITYFACTORY_H__