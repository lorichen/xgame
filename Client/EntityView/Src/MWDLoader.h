//==========================================================================
/**
* @file	  : MWDLoader.h
* @author : 
* created : 2008-4-15   7:33
* purpose : 
*/
//==========================================================================

#ifndef __MWDLOADER_H__
#define __MWDLOADER_H__

#include "PacketFormat.h"
#include <hash_map>
/*
class MWDLoader : public SingletonEx<MWDLoader>
{
	typedef HashMap<ulong, CAniInfoHeader*>	MWDMap;
	MWDMap	mMWDMap;
public:
	MWDLoader();
	~MWDLoader();

	CAniInfoHeader* request(const std::string& filename);
};*/

class MWPFileNameMap : public SingletonEx<MWPFileNameMap>
{
	typedef HashMap<ulong, std::string>	MWPMap;
	MWPMap	mMWPMap;
public:
	MWPFileNameMap()	{ }
	~MWPFileNameMap()	{ mMWPMap.clear(); }

	ulong map(const std::string& filename);
	const std::string& find(ulong id);
};

class MZIDMap : public SingletonEx<MZIDMap>
{
	typedef stdext::hash_multimap<ulong, ulong>	MZMap;
	MZMap	mMZMap;
public:
	ulong add(ulong id, ulong flag);
	void remove(ulong id);
};

#endif // __MWDLOADER_H__