//==========================================================================
/**
* @file	  : EffectControl.cpp
* @author : 
* created : 2008-4-6   17:06
*/
//==========================================================================

#include "stdafx.h"
#include "EffectControl.h"
#include "IEntityFactory.h"

MagicView* EffectControl::createGeneralEffect(const MagicContext_General& context)
{
	gGlobalClient->getEntityFactory()->createMagic(MagicType_General, &context, sizeof(MagicContext_General));
	MagicView* magic = new MagicView(typeEffect, typeResourceModel);
	if (!magic->create(context))
	{
		delete magic;
		return 0;
	}
	
	return magic;
}

void EffectControl_Fly::onFinished(MagicView* ev)
{
    if (ev == mFly)
    {
        mFly->release();
        mFly = 0;

        EntityView* target = (EntityView*)getHandleData(mTarget);
        if (target != NULL)
        {
            //  根据爆炸光效的类型进行处理；
            int nSkillViewID = getViewId();
			if (nSkillViewID == 0)
			{
				return;
			}

            int nExplodeMagicType = SkillViewDataManager::getInstance().getValue(nSkillViewID,
                SkillViewProperty_ExplodeMagicType).getInt();
            switch (nExplodeMagicType)
            {
            case EffectType_General:
                {
                    if (mIdExplode != 0)
                    {
                        MagicContext_General mc;
                        mc.magicId = mIdExplode;
                        mc.loops = mLoopsExplode;
                        mc.angle = 0;
                        //  注意：这里这里的owner是可以为0的；
                        mc.owner = mExplodeTarget;
                        mc.tile = target->getTile();
                        gGlobalClient->getEntityFactory()->createMagic(MagicType_General, &mc, sizeof(mc));
                    }
                }
                break;

                //  现阶段针对于“捆仙索”技能；
            case EffectType_DestRapidMove:
                {
                    //  创建光效参数；
                    EffectContext_RapidMove ec;
                    ec.magicId = mIdExplode;
                    ec.loops = -1;
                    ec.angle = mAngle;
                    ec.owner = mTarget;
                    ec.tile = mExplodeEffectContext.ptCenter;
                    AttackContext ExplodeEffectAttackContext;
                    //  这里是不需要做动作的；
                    ExplodeEffectAttackContext.actionId = 0;
                    ExplodeEffectAttackContext.loops = mExplodeEffectContext.loops;
                    ExplodeEffectAttackContext.fAnimateSpeed = 0;
                    ExplodeEffectAttackContext.isSpellAttack = mExplodeEffectContext.isSpellSkill;
                    ec.ac = ExplodeEffectAttackContext;
                    //  创建光效；
                    EffectControl_RapidMove* pRapidMoveEffectControl = new EffectControl_RapidMove();
                    if (!pRapidMoveEffectControl->create(ec))
                    {
                        delete pRapidMoveEffectControl;
                    }
                    else
                    {
                        pRapidMoveEffectControl->setSrc(mExplodeEffectContext.src);
                        pRapidMoveEffectControl->setViewId(mExplodeEffectContext.id);
                        EffectControlManager::getInstance().add(pRapidMoveEffectControl);
                    }
                }
                break;

            default:
                {
                    ASSERT(false);
                }
                break;
            }
        }

        release();
    }
}

void EffectControl_Fly::close()
{
	 if (mFly)
	 {
		 WarningLn("特效删除的顺序有异常");
		 mFly->setEffectObserver(INVALID_HANDLE);
	 }
}
