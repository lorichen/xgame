//==========================================================================
/**
* @file	  : PetManager.h
* @author : 
* created : 
* purpose : 
*/
//==========================================================================

#ifndef __PETMANAGER_H__
#define __PETMANAGER_H__


class PetManager : public SingletonEx<PetManager>
{
	struct RidePetCommandInfo
	{
		EntityView * owner;
		EntityCommand cmd;
		ulong param1;
		ulong param2;
	};
	typedef std::list<RidePetCommandInfo>	RidePetList;	

public:
	bool add( EntityView* owner, EntityCommand cmd, ulong param1, ulong param2 )
	{
		if( !owner) return false;
		RidePetCommandInfo info;
		info.owner = owner;
		info.cmd = cmd;
		info.param1 = param1;
		info.param2 = param2;
		mList.push_back(info);
		return true;
	}

	void Apply(EntityView* owner)
	{
		for (RidePetList::iterator it=mList.begin(); it!=mList.end();)
		{
			RidePetCommandInfo & info = *it;
			if (info.owner == owner)
			{
				owner->onCommand(info.cmd, info.param1, info.param2);		
				it = mList.erase(it);
			}
			else
				++it;
		}
	}
private:
	RidePetList	mList;

};

#endif // __PETMANAGER_H__