#ifndef __ConfigCreatures_H__
#define __ConfigCreatures_H__
#include "ISchemeCenter.h"

#define INVALID_CREATURE_ID	-1

struct AnimationAssign
{
	std::vector<std::string>	attacks;
	std::string					spell;
	std::string					prepare;
	std::string					attacked;
	std::string					dizz;
	std::string					leisure;
	int							soundAttack;
	int							soundDie;
	int                         soundWound;
	int                         soundMove;
	int                         soundFallow;
	int                         soundOther;

	AnimationAssign() : soundAttack(0),soundDie(0),soundWound(0),soundMove(0),soundFallow(0),soundOther(0){}
};

struct ConfigCreature : public ConfigModelNode
{
	std::string resId;
	std::string basicCreatureName;
	ConfigRide  cRide;
	AnimationAssign aa;
};

class ConfigCreatures : public ISchemeUpdateSink
{
public:
	// ISchemeUpdateSink
	virtual bool			OnSchemeLoad(ICSVReader * pCSVReader,LPCSTR szFileName);
	virtual bool			OnSchemeLoad(TiXmlDocument * pTiXmlDocument,LPCSTR szFileName);
	virtual bool			OnSchemeUpdate(ICSVReader * pCSVReader, LPCSTR szFileName);
	virtual bool			OnSchemeUpdate(TiXmlDocument * pTiXmlDocument, LPCSTR szFileName);

public:
	bool load(const std::string& filename);

public:
	ConfigCreature* getCreature(int id);
	ConfigCreature* getFirstCreature();
	ConfigCreature* getNextCreature();

private:
	typedef stdext::hash_map<int,ConfigCreature*>	CreatureMap;
	CreatureMap m_creatures;
	CreatureMap::iterator	m_it;

public:
	static ConfigCreatures* Instance()
	{
		static ConfigCreatures cc;
		return &cc;
	}

	~ConfigCreatures();

private:
	ConfigCreatures(){}
};

#endif