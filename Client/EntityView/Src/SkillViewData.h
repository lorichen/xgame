//==========================================================================
/**
* @file	  : SkillViewData.h
* @author : 
* created : 2008-3-13   16:45
* purpose : 技能光效数据脚本管理
*/
//==========================================================================

#ifndef __SKILLVIEWDATA_H__
#define __SKILLVIEWDATA_H__

#include "SkillViewDef.h"

typedef RecordSet<MAX_SUBSKILLVIEW_COUNT>	SkillViewData;

/// 光效数据管理器
class SkillViewDataManager : public SingletonEx<SkillViewDataManager>, public ISchemeUpdateSink
{
	SkillViewData*			mSkillViewDataArray[MAX_SKILLVIEW_COUNT];

	//技能编辑使用,add wangtao
	std::vector<std::string> mCsvFiledName;
	std::vector<int>         mCsvFiledType;
public:
	SkillViewDataManager();
	virtual ~SkillViewDataManager();


	bool read(ICSVReader* csv);

	const XlsRecord* getRecord(ulong id) const;
	const XlsValue& getValue(ulong id, ulong field) const;

	//Skill编辑, add wangtao
	uint getSkillViewDataId(uint array, uint set);
	const std::string& getSkillViewDataName(uint id);
	bool getSkillViewDataArrayIsValid(uint array);
	bool getSkillViewDataRowIsValid(uint array, uint set);
	void SetSkillViewData(uint id, std::string attackAct, uint swing, float flySpeed, 
		uint emitEffect, uint blastEffect, uint hitEffect, uint flyEffect,
		uint hitFlyEffect, uint readyEffect);
	void SaveSkillViewDataToCsv(const char* szFileName);
private:
	bool innerRead(ICSVReader* csv, SkillViewData** arr);
	void clearArray(SkillViewData** arr);

	// ISchemeUpdateSink
private:
	virtual bool OnSchemeLoad(ICSVReader * pCSVReader,const char* szFileName);
	virtual bool OnSchemeUpdate(ICSVReader * pCSVReader, const char* szFileName);
	virtual bool OnSchemeLoad(TiXmlDocument * pTiXmlDocument,const char* szFileName){return false;}
	virtual bool OnSchemeUpdate(TiXmlDocument * pTiXmlDocument, const char* szFileName){return false;}
};

#endif // __SKILLVIEWDATA_H__