//==========================================================================
/**
* @file	  : StateDef.h
* @author : pk <pk163@163.com>
* created : 2008-3-5   15:01
* purpose : 状态定义(主要针对模型对象而言)
*/
//==========================================================================

#ifndef __STATEDEF_H__
#define __STATEDEF_H__

/**
	考虑到我们的动画没有wow那么灵活，没有上下身独立运动，
	所以去掉先前的并行状态机，进行简化处理
*/

enum
{
	stateStand,
	stateMove,
	stateSitdown,
	stateDeath,
	stateFallow,
	stateWound,
	stateStun,
	stateAttackReady,
	stateAttack,
	//stateSpellReady,
	//stateSpell,
	MaxStateCount,
	// 如果修改该枚举，请修改StateComponentMZ::setState中的loopFlag
};


/*
角色动作(支持1到多套)：
站立 Stand
行走 Walk
攻击准备 AttackReady
物理攻击 Attack
施法准备 SpellReady
施法 Spell
通道施法 SpellChannel
被击 Hit / Wound
死亡 Death
尸体 DecayFlesh

可选:
休闲 Fallow / Emote
骑乘 Ride
起立 Rise / SitGroundUp
坐下 Sitting  / SitGroundDown
坐着 SitGround
躺下 Lie
眩晕 Stun / Dizzy
躲避 Parry


怪物附加：
尸骨 DecayBone
出生 Birth
*/

#endif // __STATEDEF_H__