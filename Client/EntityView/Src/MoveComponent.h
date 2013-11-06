//==========================================================================
/**
* @file	  : MoveComponent.h
* @author : 
* created : 2008-1-31   12:53
* purpose : ÒÆ¶¯×é¼þ
*/
//==========================================================================

#ifndef __MOVECOMPONENT_H__
#define __MOVECOMPONENT_H__

#include "EntityComponent.h"

class MoveComponent : public EntityComponent
{
public:
	MoveComponent();
	virtual ~MoveComponent();

	//virtual void MoveStep(ulong dwPeriod){}
	virtual bool update(ulong curTicks, ulong deltaTicks) { return true; }
};

#endif // __MOVECOMPONENT_H__