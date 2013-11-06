#ifndef __ConfigCreaturePart_H__
#define __ConfigCreaturePart_H__

enum
{
	PT_TEXTURE = 1,
	PT_MODEL = 2
};

struct CreaturePart
{
	std::string partType;	//上半身
	int attachType;			//1=贴图,2=模型
	std::string name;		//骨骼名或者部件名称
};

class ConfigCreaturePart : public ISchemeUpdateSink
{
public:
	// ISchemeUpdateSink
	virtual bool			OnSchemeLoad(ICSVReader * pCSVReader,LPCSTR szFileName);
	virtual bool			OnSchemeLoad(TiXmlDocument * pTiXmlDocument,LPCSTR szFileName);
	virtual bool			OnSchemeUpdate(ICSVReader * pCSVReader, LPCSTR szFileName);
	virtual bool			OnSchemeUpdate(TiXmlDocument * pTiXmlDocument, LPCSTR szFileName);
public:
	bool load(const char* fileName);
	CreaturePart *getByType(const std::string& partType);
protected:
	ConfigCreaturePart()
	{
	}
public:
	static ConfigCreaturePart*	Instance()
	{
		static ConfigCreaturePart c;
		return &c;
	}
private:
	typedef stdext::hash_map<std::string,CreaturePart> CreaturePartMap;
	CreaturePartMap	m_cp;
};

#endif