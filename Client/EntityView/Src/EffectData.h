//==========================================================================
/**
* @file	  : EffectData.h
* @author : 
* created : 2008-3-13   16:45
* purpose : 光效包数据脚本管理
*/
//==========================================================================

#ifndef __EFFECTDATA_H__
#define __EFFECTDATA_H__

#include "EffectDef.h"

typedef RecordSet<MAX_SUBEFFECT_COUNT>	EffectData;

/// 光效数据管理器
class EffectDataManager : public SingletonEx<EffectDataManager>, public ISchemeUpdateSink
{
	EffectData*			mEffectDataArray[MAX_EFFECT_COUNT];

public:
	EffectDataManager();
	~EffectDataManager();

	// ISchemeUpdateSink
public:
	virtual bool OnSchemeLoad(ICSVReader * pCSVReader,const char* szFileName);
	virtual bool OnSchemeUpdate(ICSVReader * pCSVReader, const char* szFileName);
	virtual bool OnSchemeLoad(TiXmlDocument * pTiXmlDocument,const char* szFileName){return false;}
	virtual bool OnSchemeUpdate(TiXmlDocument * pTiXmlDocument, const char* szFileName){return false;}

	bool read(ICSVReader* csv);

	const EffectData* getEffectData(ulong index) const;
    bool HasRecord(ulong ulId) const;
	const XlsValue& getValue(ulong id, ulong field) const;
private:
	bool innerRead(ICSVReader* csv, EffectData** arr);
	void clearArray(EffectData** arr);
};



#endif // __EFFECTDATA_H__