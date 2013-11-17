#ifndef __ConfigCreatureRes_H__
#define __ConfigCreatureRes_H__

#include "ConfigCreature.h"

struct lessString
{
	bool operator()(const std::string& v1,const std::string& v2) const
	{
		return StringHelper::casecmp(v1,v2) != 0;
	}
};

class ConfigCreatureRes : public ISchemeUpdateSink
{
public:
	// ISchemeUpdateSink
	virtual bool			OnSchemeLoad(ICSVReader * pCSVReader,const char* szFileName);
	virtual bool			OnSchemeLoad(TiXmlDocument * pTiXmlDocument,const char* szFileName);
	virtual bool			OnSchemeUpdate(ICSVReader * pCSVReader, const char* szFileName);
	virtual bool			OnSchemeUpdate(TiXmlDocument * pTiXmlDocument, const char* szFileName);

public:
	bool load(const char* fileName);
	void save();

public:
	CreatureResourceEx* getFirstCreature();
	CreatureResourceEx* getNextCreature();

	std::string getFirstCreaturePart(const std::string& name);
	std::string getNextCreaturePart();

	IDName* getFirstCreaturePartName(const std::string& creature,const std::string& part);
	IDName* getNextCreaturePartName();

	std::string getModelFilename(const std::string& name);

	const std::string& getResFromId(int id);

	//skill编辑器用，
	const int getResId(int id);
	const int getMaxId();
	const std::string getEffectName(int id);
	bool isEffect(std::string& name);
	const int getSoundIDFromId(int id);

protected:
	ConfigCreatureRes()
	{
		m_maxId = 1;
	}

public:
	static ConfigCreatureRes*	Instance();

public:
	CreatureListEx m_creatures;
	CommonList m_commonList;

public:
	int m_maxId;
	std::string	m_filename;
	typedef HASH_MAP_NAMESPACE::hash_map<std::string,int > IdMap;
	IdMap m_idMaps;
	typedef HASH_MAP_NAMESPACE::hash_map<int,std::string> ResMap;
	ResMap m_resMap;

	CreatureListEx::iterator m_itCreature,m_itCreature__;
	PartTypeMap::iterator m_itCreaturePart,m_itCreaturePart__;
	PartType::iterator m_itCreaturePartName;

    // 通过资源ID来查找对应的声音ID；
	typedef HASH_MAP_NAMESPACE::hash_map<int,IDName> IdSoundMap;
	IdSoundMap  m_soundMap;
};

#endif