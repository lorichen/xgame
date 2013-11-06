#include "StdAfx.h"
#include "ConfigActionMap.h"

bool ConfigActionMap::load(const char* fileName)
{
	ISchemeEngine *pSchemeEngine = CreateSchemeEngineProc();
	if(pSchemeEngine)
	{
		return pSchemeEngine->LoadScheme(fileName,this);
	}

	return false;
}

const char * ConfigActionMap::getMappedAction(const ActionMapContext & context,  int actionid)
{
	ActionMap::iterator it = m_actionMap.find(actionid);
	if( it == m_actionMap.end() ) return "";//不返回空指针

	if( context.ismount) return it->second.mountaction.c_str();
	else
	{
		if( context.iscreature )
		{
			int index = context.state * 3 + context.weapon;
			if( index >= SConfigActionMapEntry::ePersonActionMax ) return it->second.personunmountaction[0].c_str();
			else return it->second.personunmountaction[index].c_str();	
		}
		else
		{
			return it->second.MNPunmountaction[context.state].c_str();
		}	
	}
}
bool ConfigActionMap::getMappedAction(const ActionMapContext & context, int actionid, std::string & action)
{
	ActionMap::iterator it = m_actionMap.find(actionid);
	if( it == m_actionMap.end() ) return false;

	action = getMappedAction(context,actionid);
	return true;
}

ConfigActionMap::ConfigActionMap()
{
	
}

// ISchemeUpdateSink
bool ConfigActionMap::OnSchemeLoad(ICSVReader * pCSVReader,LPCSTR szFileName)
{
	if( pCSVReader == 0) return false;
	m_actionMap.clear();
	DWORD count = pCSVReader->GetRecordCount();
	std::string szDefaultAction[9] =
	{
		"MountStand",
		"Stand",
		"Stand",
		"Stand",
		"Stand1",
		"Stand3",
		"Stand2",
		"Stand",
		"Stand",
	};

	SConfigActionMapEntry entrybuff;
	char  szBuffer[64];
	int nBufferLen;
	for( DWORD i=0; i <count; ++i)
	{
		int actionid = pCSVReader->GetInt(i,0,-1);
		if( actionid < 0 ) continue;

		//骑乘动作
		nBufferLen = 64;
		pCSVReader->GetString(i,3,szBuffer,nBufferLen);
		if( nBufferLen == 1 ) entrybuff.mountaction = szDefaultAction[0];
		else entrybuff.mountaction.assign(szBuffer,nBufferLen);

		//人物非骑乘动作
		for( DWORD j = 0; j <SConfigActionMapEntry::ePersonActionMax; ++j)
		{
			nBufferLen = 64;
			pCSVReader->GetString(i,4+j,szBuffer,nBufferLen);
			if( nBufferLen == 1 ) entrybuff.personunmountaction[j].assign(szDefaultAction[1+j]);
			else entrybuff.personunmountaction[j].assign(szBuffer,nBufferLen);	
		}

		//怪物，npc，宠物动作
		for( DWORD j = 0; j <EVCS_Max; ++j)
		{
			nBufferLen = 64;
			pCSVReader->GetString(i,10+j,szBuffer,nBufferLen);
			if( nBufferLen == 1 ) entrybuff.MNPunmountaction[j].assign(szDefaultAction[7+j]);
			else entrybuff.MNPunmountaction[j].assign(szBuffer,nBufferLen);		
		}

		m_actionMap[actionid] = entrybuff;
	}
	return true;
}
bool ConfigActionMap::OnSchemeLoad(TiXmlDocument * pTiXmlDocument,LPCSTR szFileName)
{
	return false;
}
bool ConfigActionMap::OnSchemeUpdate(ICSVReader * pCSVReader, LPCSTR szFileName)
{
	return false;
}
bool ConfigActionMap::OnSchemeUpdate(TiXmlDocument * pTiXmlDocument, LPCSTR szFileName)
{
	return false;
}
