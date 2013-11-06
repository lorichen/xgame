//==========================================================================
/**
* @file	  : MWDLoader.cpp
* @author : 
* created : 2008-4-15   7:33
*/
//==========================================================================

#include "stdafx.h"
#include "MWDLoader.h"
/*
MWDLoader::MWDLoader()
{

}

MWDLoader::~MWDLoader()
{
	for (MWDMap::iterator it=mMWDMap.begin(); it!=mMWDMap.end(); ++it)
	{
		CAniInfoHeader*& header = (*it).second;
		header->Release();
	}
	mMWDMap.clear();
}

CAniInfoHeader* MWDLoader::request(const std::string& filename)
{
	ulong id = STR2ID(filename.c_str());

	MWDMap::iterator it = mMWDMap.find(id);
	if (it != mMWDMap.end()) // found
		return (*it).second;

	CAniInfoHeader* header = new CAniInfoHeader();
	if (!header->Open(filename.c_str()))
	{
		Error("Open *.mwd failed, file: "<<filename<<endl);
		delete header;
		return NULL;
	}
		
	mMWDMap[id] = header;
	//Info("ID = "<<id<<"  Count = "<<mMWDMap.size()<<endl);
	return header;
}*/





ulong MWPFileNameMap::map(const std::string& filename)
{
	ulong id = STR2ID(filename.c_str());

	MWPMap::iterator it = mMWPMap.find(id);
	if (it != mMWPMap.end()) // found
		return (*it).first;

	mMWPMap[id] = filename;
	return id;
}

const std::string& MWPFileNameMap::find(ulong id)
{
	static const std::string sEmpty;

	MWPMap::iterator it = mMWPMap.find(id);
	if (it != mMWPMap.end()) // found
		return (*it).second;
	return sEmpty;
}




// 1位flag+16位id+15位计数
ulong MZIDMap::add(ulong id, ulong flag)
{
	int count = 0;
	ulong key = (flag << 31) | (id << 15);
	MZMap::iterator it = mMZMap.find(key);
	while (it != mMZMap.end() && (*it).first == key) // found
	{
		count++;
		if ((*it).second == 0) // free?
		{
			(*it).second = 1;
			return (key | count);
		}
		++it;
	}

	count++;
	mMZMap.insert(MZMap::value_type(key, 1));
	return (key | count);
}

void MZIDMap::remove(ulong id)
{
	ulong key = id & 0xFFFF8000;
	ulong count1 = id & 0x7FFF;
	ulong count = 0;
	MZMap::iterator it = mMZMap.find(key);
	while (it != mMZMap.end() && (*it).first == key) // found
	{
		count++;
		if (count == count1) // do free
		{
			(*it).second = 0;
			return;
		}
		++it;
	}
}