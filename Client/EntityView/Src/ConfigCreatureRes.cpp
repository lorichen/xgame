#include "stdafx.h"
#include "ConfigCreatureRes.h"

bool ConfigCreatureRes::load(const char* fileName)
{
	ISchemeEngine *pSchemeEngine = CreateSchemeEngineProc();
	if(pSchemeEngine)
	{
		m_filename = fileName;
		return pSchemeEngine->LoadScheme(fileName,this);
	}

	return false;
}

void ConfigCreatureRes::save()
{
}

bool ConfigCreatureRes::OnSchemeLoad(ICSVReader * pCSVReader,LPCSTR szFileName)
{
	return false;
}

bool ConfigCreatureRes::OnSchemeLoad(TiXmlDocument * pTiXmlDocument,LPCSTR szFileName)
{
	m_creatures.clear();

	const TiXmlElement* root = pTiXmlDocument->RootElement();
	for(const TiXmlElement* child = root->FirstChildElement();child;child=child->NextSiblingElement())
	{
		CreatureResourceEx c;
		child->Attribute("id",&c.idname.id);
		c.name = child->Attribute("name");
		c.idname.name = child->Attribute("model");

		m_idMaps[c.idname.name] = c.idname.id;
		m_resMap[c.idname.id] = c.idname.name;

		for(const TiXmlElement* cc = child->FirstChildElement();cc;cc=cc->NextSiblingElement())
		{
			std::string part = cc->Attribute("name");

			PartType pt;
			for(const TiXmlElement* ccc = cc->FirstChildElement();ccc;ccc=ccc->NextSiblingElement())
			{
				IDName i;
				ccc->Attribute("id",&i.id);
				i.name = ccc->Attribute("name");

				m_idMaps[i.name] = i.id;
				m_resMap[i.id] = i.name;

				pt.push_back(i);
			}

			c.partNames[part] = pt;
		}

		m_creatures.push_back(c);
	}

	const TiXmlElement *pCommon = root->NextSiblingElement();

	for(const TiXmlElement* child = pCommon->FirstChildElement();child;child=child->NextSiblingElement())
	{
		IDName i;
		child->Attribute("id",&i.id);
		i.name = child->Attribute("name");
		child->Attribute("soundid",&i.soundid);
		m_commonList.push_back(i);

		m_idMaps[i.name] = i.id;
		m_resMap[i.id] = i.name;
		m_soundMap[i.id] = i;
	}

	const TiXmlElement *pMaxId = pCommon->NextSiblingElement();
	if(pMaxId)
	{
		pMaxId->Attribute("value",&m_maxId);
	}

	return true;
}

bool ConfigCreatureRes::OnSchemeUpdate(ICSVReader * pCSVReader, LPCSTR szFileName)
{
	return false;
}

bool ConfigCreatureRes::OnSchemeUpdate(TiXmlDocument * pTiXmlDocument, LPCSTR szFileName)
{
	return false;
}

CreatureResourceEx* ConfigCreatureRes::getFirstCreature()
{
	m_itCreature = m_creatures.begin();
	return getNextCreature();
}

CreatureResourceEx* ConfigCreatureRes::getNextCreature()
{
	if(m_itCreature == m_creatures.end())return 0;

	return &(*m_itCreature++);
}

std::string ConfigCreatureRes::getFirstCreaturePart(const std::string& name)
{
	m_itCreature__ = find(m_creatures.begin(),m_creatures.end(),name);
	if(m_itCreature__ == m_creatures.end())return "";
	m_itCreaturePart = (*m_itCreature__).partNames.begin();
	return getNextCreaturePart();
}

std::string ConfigCreatureRes::getNextCreaturePart()
{
	if(m_itCreaturePart == (*m_itCreature__).partNames.end())return "";

	return (*m_itCreaturePart++).first;
}

IDName* ConfigCreatureRes::getFirstCreaturePartName(const std::string& creature,const std::string& part)
{
	CreatureListEx::iterator it = find(m_creatures.begin(),m_creatures.end(),creature);
	if(it == m_creatures.end())return 0;
	m_itCreaturePart__ = (*it).partNames.find(part);
	if(m_itCreaturePart__ == (*it).partNames.end())return 0;
	m_itCreaturePartName = (*m_itCreaturePart__).second.begin();
	return getNextCreaturePartName();
}

IDName* ConfigCreatureRes::getNextCreaturePartName()
{
	if(m_itCreaturePartName == (*m_itCreaturePart__).second.end())return 0;

	return &(*m_itCreaturePartName++);
}

std::string ConfigCreatureRes::getModelFilename(const std::string& name)
{
	CreatureListEx::iterator it = find(m_creatures.begin(),m_creatures.end(),name);
	if(it == m_creatures.end())return "";

	return (*it).idname.name;
}

const std::string& ConfigCreatureRes::getResFromId(int id)
{
	static std::string str;


	ResMap::iterator it = m_resMap.find(id);
	if(it == m_resMap.end())return str;

	return (*it).second;
}
const int ConfigCreatureRes::getSoundIDFromId(int id)
{
	IdSoundMap::iterator it = m_soundMap.find(id);
	if(it == m_soundMap.end())
		return 0;
	IDName i = (*it).second;
	return i.soundid;
}
const std::string c_effectResPath = "data\\Model\\Common\\Effect\\";
const int ConfigCreatureRes::getResId(int id)
{
	ResMap::iterator it = m_resMap.find(id);
	if (it == m_resMap.end())
		return 0;

	if (isEffect((*it).second))
		return (*it).first;
	return 0;
}

const int ConfigCreatureRes::getMaxId()
{
	return m_maxId;
}

const std::string ConfigCreatureRes::getEffectName(int id)
{
	static std::string str = "";
	if (isEffect(m_resMap[id]))
	{
		std::string path = m_resMap[id];
		int start = c_effectResPath.size();
		int end = path.size();
		std::string temp = path.substr(start, end);
		return temp;
	}
	return str;
}

bool ConfigCreatureRes::isEffect(std::string& name)
{
	std::string temp = name;
	std::string s = temp.substr(0, c_effectResPath.size());
	if (c_effectResPath == s)
		return true;
	return false;
}
