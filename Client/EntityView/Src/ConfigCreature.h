#ifndef __CreatureResource_H__
#define __CreatureResource_H__

struct IDName
{
	int id;
	std::string name;
	int soundid;
	IDName() : id(0),soundid(0){}
};

typedef std::vector<IDName> PartType;
typedef HASH_MAP_NAMESPACE::hash_map<std::string,PartType> PartTypeMap;

struct CreatureResourceEx
{
	std::string name;
	IDName idname;
	PartTypeMap partNames;

	bool operator==(const std::string& c2)
	{
		return name == c2;
	}
};

typedef std::vector<CreatureResourceEx> CreatureListEx;
typedef std::vector<IDName> CommonList;

#endif