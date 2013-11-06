//==========================================================================
/**
* @file	  : SkillViewDef.h
* @author : 
* created : 2008-3-13   17:35
* purpose : 技能光效属性定义
*/
//==========================================================================

#ifndef __SKILLVIEWDEF_H__
#define __SKILLVIEWDEF_H__

enum
{ 
	MAX_SUBSKILLVIEW_COUNT	= 1000, 
	MAX_SKILLVIEW_COUNT		= 100,
};

enum ESkillViewProperty
{
	SkillViewProperty_Depth,                                         // 脚本层次    
	SkillViewProperty_Name,                                          // 名称
	SkillViewProperty_Id,                                            // 编号
	SkillViewProperty_EffectType,                                    // 光效类型
	SkillViewProperty_NotAdjustDir,                                  // 光效方向是否跟技能施放者面向一直
	SkillViewProperty_AttackReadyActId,                              // 蓄气动作
	SkillViewProperty_AttackReadyActSpeed,                           // 蓄气动作速度
	SkillViewProperty_AttackReadyMagicId,                            // 蓄气光效
    SkillViewProperty_AttackReadyMagicIdOnTarget,                    // 蓄气光效（目标）
    SkillViewProperty_AttackReadyMagicOnTargetAttachToTarget,        // 蓄气光效（目标）是否绑定在目标上      
	SkillViewProperty_RapidOrChannel,                                // 瞬发还是通道
	SkillViewProperty_AttackActId,                                   // 攻击动作
	SkillViewProperty_AttackActSpeed,                                // 攻击动作速度
	SkillViewProperty_AttackMagicId,                                 // 施法光效
	SkillViewProperty_AttackMagicAttachToSource,                     // 施法光效是否绑定在源上
	SkillViewProperty_BeHitMagicId,                                  // 目标受击光效
	SkillViewProperty_BeHitFlyMagicId,                               // 目标受击飞行光效
	SkillViewProperty_WoundMagicId,                                  // 反弹伤害光效
	SkillViewProperty_TrackMagicId,                                  // 跟踪光效
    SkillViewProperty_ExplodeMagicType,                              // 爆炸光效类型
	SkillViewProperty_ExplodeMagicID,                                // 爆炸光效
    SkillViewProperty_ExplodeMagicAttachToTarget,                    // 爆炸光效是否绑定在目标上
	SkillViewProperty_Dummy,                                         // 光效是否不受攻击祯控制
	SkillViewProperty_FlySpeed,                                      // 飞行速度
	SkillViewProperty_AttackRange,                                   // 宽度
	SkillViewProperty_Delay,                                         // 间隔
	SkillViewProperty_Times,                                         // 次数
	SkillViewProperty_LifeTime,                                      // 循环时间
	SkillViewProperty_Vibrate,                                       // 振屏参数

    SkillViewProperty_MagicID,                                       // 其他动画用到
	SkillViewProperty_SoundIDPreAttack,                              // 蓄气阶段的音效ID
	SkillViewProperty_bSoundLoopPreAttack,                           // 蓄气阶段的音效是否循环
	SkillViewProperty_SoundIDAttack,                                 // 攻击阶段的音效
	SkillViewProperty_bSoundLoopeAttack,                             // 攻击阶段的音效是否循环
	SkillViewProperty_SoundIDBeHit,                                  // 被击的音效
	SkillViewProperty_bSoundLoopeBeHit,                              // 被击的是否循环

	MaxSkillViewPropertyCount,                                       // Max
};

#endif // __SKILLVIEWDEF_H__