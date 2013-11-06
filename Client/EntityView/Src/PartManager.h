//==========================================================================
/**
* @file	  : PartManager.h
* @author : 
* created : 2008-5-27   14:06
* purpose : 生物部件管理器
*/
//==========================================================================

#ifndef __PARTMANAGER_H__
#define __PARTMANAGER_H__

struct PartInformation
{
	virtual EntityCommand getCommandType()=0;
	virtual void apply() = 0;
	virtual ulong getPart()=0;
	virtual ~PartInformation(){};
};

struct SetPartInformation: public PartInformation
{
	virtual EntityCommand getCommandType() { return EntityCommand_SetPart; }
	virtual void apply() { owner->onCommand(EntityCommand_SetPart, (ulong)(&context), 0) ;}
	virtual ~SetPartInformation(){};
	virtual ulong getPart(){ return context.part;}
	SetPartInformation( EntityView * o, const SPersonMainPartChangeContext & c):owner(o),context(c){}
private:
	EntityView * owner;
	SPersonMainPartChangeContext context;
};

struct ShowPartInformation: public PartInformation
{
	virtual EntityCommand getCommandType() { return EntityCommand_ShowPart;}
	virtual void apply(){ owner->onCommand(EntityCommand_ShowPart, part, show) ;}
	virtual ~ShowPartInformation(){};
	virtual ulong getPart(){ return part;}
	ShowPartInformation(EntityView * o, ulong p, bool s):owner(o),part(p),show(s){ }
private:
	EntityView * owner;
	ulong part;
	bool show;
};

struct PartAttachEffectInfo: public PartInformation
{
	virtual EntityCommand getCommandType() { return EntityCommand_PartAttachEffect;}
	virtual void apply() { owner->onCommand(EntityCommand_PartAttachEffect, (ulong)&context,0); };
	virtual ~PartAttachEffectInfo(){};
	PartAttachEffectInfo(EntityView * o, const SPersonAttachPartChangeContext & c): owner(o),context(c){}
	virtual ulong getPart(){return context.part;}
private:
	EntityView * owner;
	SPersonAttachPartChangeContext context;
};

struct DeformPartInfo: public PartInformation
{
	virtual EntityCommand getCommandType() { return EntityCommand_PartDeformation;}
	virtual void apply() { owner->onCommand(EntityCommand_PartDeformation, (ulong)&context,0); };
	virtual ~DeformPartInfo(){};
	DeformPartInfo(EntityView * o, const SPersonPartDeformationContext & c): owner(0),context(c){}
	inline ulong getPart(){ return context.part;}
private:
	EntityView * owner;
	SPersonPartDeformationContext context;
};

class PartManager : public SingletonEx<PartManager>
{
	typedef std::list<PartInformation *> EntityPartInfoList;
	typedef std::map<EntityView*, EntityPartInfoList> EntityPartInfoMap;
	EntityPartInfoMap m_map;

public:
	void add(EntityView * owner,  PartInformation  * info)
	{
		if( !owner || !info ) return;
		EntityPartInfoMap::iterator it = m_map.find(owner);
		if( it == m_map.end() )
		{
			EntityPartInfoList lst;
			lst.push_back( info );
			m_map[owner] = lst;
			return;
		}
		else
		{
			it->second.push_back(info);
			return;
		}
	} 

	void apply(  EntityView * owner, EntityCommand cmd, ulong part)
	{
		if( !owner) return;
		EntityPartInfoMap::iterator it = m_map.find(owner);
		if( it == m_map.end() ) return;
		EntityPartInfoList::iterator it2 = it->second.begin();
		for( ; it2 != it->second.end();  )
		{
			if( (*it2)->getCommandType() == cmd  && (*it2)->getPart() == part )
			{
				(*it2)->apply();
				delete (*it2);
				it2 = it->second.erase(it2);
			}
			else ++it2;
		}
		if( it->second.empty() ) m_map.erase(it);
		return;	
	}

	void apply( EntityView * owner, EntityCommand cmd )
	{
		if( !owner) return;
		EntityPartInfoMap::iterator it = m_map.find(owner);
		if( it == m_map.end() ) return;
		EntityPartInfoList::iterator it2 = it->second.begin();
		for( ; it2 != it->second.end();  )
		{
			if( (*it2)->getCommandType() == cmd )
			{
				(*it2)->apply();
				delete (*it2);
				it2 = it->second.erase(it2);
			}
			else ++it2;
		}
		if( it->second.empty() ) m_map.erase(it);
		return;
	}
};
#endif // __PARTMANAGER_H__