#ifndef __CONFIGACTIONMAP_H__
#define __CONFIGACTIONMAP_H__


struct SConfigActionMapEntry
{
	enum{ePersonActionMax = 6,};
	std::string mountaction;
	std::string personunmountaction[ePersonActionMax];
	std::string MNPunmountaction[EVCS_Max];
};

class ConfigActionMap : public ISchemeUpdateSink
{
public:
	// ISchemeUpdateSink
	virtual bool			OnSchemeLoad(ICSVReader * pCSVReader,LPCSTR szFileName);
	virtual bool			OnSchemeLoad(TiXmlDocument * pTiXmlDocument,LPCSTR szFileName);
	virtual bool			OnSchemeUpdate(ICSVReader * pCSVReader, LPCSTR szFileName);
	virtual bool			OnSchemeUpdate(TiXmlDocument * pTiXmlDocument, LPCSTR szFileName);

public:
	bool load(const char * filename);
	const char * getMappedAction(const ActionMapContext & context, int actionid);//这个指针不要保存
	bool getMappedAction(const ActionMapContext & context, int actionid, std::string & action);//可以保存

private:
	typedef stdext::hash_map<int,SConfigActionMapEntry>	ActionMap;
	ActionMap m_actionMap;

public:
	static ConfigActionMap* Instance()
	{
		static ConfigActionMap cam;
		return &cam;
	}

private:
	ConfigActionMap();
};

#endif