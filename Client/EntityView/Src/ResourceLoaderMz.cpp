#include "stdafx.h"
#include "ResourceLoaderMz.h"
#include "ResourceMz.h"
#include "ConfigCreatureRes.h"
#include "ConfigCreatures.h"

//derived from IResourceLoader
IResource* ResourceLoaderMz::loadResource(const ulong uResID)
{
	ulong id = uResID >> 15;
	//ConfigCreature* pConfig = ConfigCreatures::Instance()->getCreature(id);

	bool ret = false;
	ResourceMz *pMz = new ResourceMz;
	ulong flag = id >> 16;
	ulong resId = id & 0xFFFF;
	if (flag) // ¹âÐ§
	{
		ret = pMz->load(ConfigCreatureRes::Instance()->getResFromId(resId));
	}
	else
	{
		ConfigCreature* pConfig = ConfigCreatures::Instance()->getCreature(resId);
		if (pConfig)
			ret = pMz->load(pConfig);
		else
			return 0;
	}
	//if (pConfig)
	//	ret = pMz->load(pConfig);
	//else
	//	ret = pMz->load(ConfigCreatureRes::Instance()->getResFromId(id));

	if (!ret)
	{
		delete pMz;
		pMz = 0;
	}
	return pMz;
}

void  ResourceLoaderMz::release()
{
}