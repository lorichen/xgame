#include "stdafx.h"
#include "ConfigCreaturePart.h"


ConfigCreaturePart*	ConfigCreaturePart::Instance()
{
    static ConfigCreaturePart c;
    return &c;
}

CreaturePart *ConfigCreaturePart::getByType(const std::string& partType)
{
	CreaturePartMap::iterator it = m_cp.find(partType);
	if(it == m_cp.end())return 0;

	return &(*it).second;
}

bool ConfigCreaturePart::load(const char* fileName)
{
	ISchemeEngine *pSchemeEngine = CreateSchemeEngineProc();
	if(pSchemeEngine)
	{
		return pSchemeEngine->LoadScheme(fileName,this);
	}

	return false;
}

bool ConfigCreaturePart::OnSchemeLoad(ICSVReader * pCSVReader,const char* szFileName)
{
	DWORD dwRecordNum = pCSVReader->GetRecordCount();
	DWORD dwFieldNum = pCSVReader->GetFieldCount();
	for(DWORD i = 0;i < dwRecordNum;i++)
	{
		char sz[256];
		int len = 256;

		CreaturePart cp;
		pCSVReader->GetString(i,0,sz,len);
		cp.partType = sz;
		cp.attachType = pCSVReader->GetInt(i,1);
		len = 256;
		pCSVReader->GetString(i,2,sz,len);
		cp.name = sz;

		m_cp[cp.partType] = cp;
	}
	return true;
}

bool ConfigCreaturePart::OnSchemeLoad(TiXmlDocument * pTiXmlDocument,const char* szFileName)
{
	return false;
}

bool ConfigCreaturePart::OnSchemeUpdate(ICSVReader * pCSVReader, const char* szFileName)
{
	return false;
}

bool ConfigCreaturePart::OnSchemeUpdate(TiXmlDocument * pTiXmlDocument, const char* szFileName)
{
	return false;
}