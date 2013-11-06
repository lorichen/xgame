//==========================================================================
/**
* @file	  : SkillViewData.cpp
* @author : 
* created : 2008-3-13   16:45
*/
//==========================================================================

#include "stdafx.h"
#include "SkillViewData.h"


SkillViewDataManager::SkillViewDataManager()
{
	memset(mSkillViewDataArray, 0, sizeof(mSkillViewDataArray));
}

SkillViewDataManager::~SkillViewDataManager()
{
	clearArray(mSkillViewDataArray);
}

void SkillViewDataManager::clearArray(SkillViewData** arr)
{
	for (int i=0; i<MAX_SKILLVIEW_COUNT; i++)
		safeDelete(arr[i]);
}

bool SkillViewDataManager::OnSchemeLoad(ICSVReader* pCSVReader,const char* szFileName)
{
	return OnSchemeUpdate(pCSVReader, szFileName);
}

bool SkillViewDataManager::OnSchemeUpdate(ICSVReader* pCSVReader, const char* szFileName)
{
	if (pCSVReader)
	{
		if (!read(pCSVReader))
		{
			Error("SkillViewDataManager::read failed"<<endl);
			return false;
		}

		return true;
	}

	Assert(pCSVReader != NULL);
	return false;
}

bool SkillViewDataManager::read(ICSVReader* csv)
{
	Assert(csv != NULL);

	SkillViewData* arr[MAX_SKILLVIEW_COUNT];
	memset(arr, 0, sizeof(arr));

	if (!innerRead(csv, arr))
	{
		clearArray(arr);
		return false;
	}

	clearArray(mSkillViewDataArray);

	// 更新
	memcpy(mSkillViewDataArray, arr, sizeof(mSkillViewDataArray));

	return true;
}

bool SkillViewDataManager::innerRead(ICSVReader* csv, SkillViewData** arr)
{
#define GetPropString(id) \
	strVal[0] = 0;len = sizeof(strVal);\
	if (!csv->GetString(row, col=id, strVal, len)) goto parse_error;\
	(*record)[id].setString(strVal)

#define GetPropInt(id, def) \
	intVal = csv->GetInt(row, col=id, def); (*record)[id].setInt(intVal)

#define GetPropFloat(id, def) \
	fVal = csv->GetFloat(row, col=id, def);	(*record)[id].setFloat(fVal)

#define GetPropIntArray(id, op, val)\
	strVal[0] = 0; len = sizeof(strVal);\
	if (!csv->GetString(row, col=id, strVal, len)) goto parse_error;\
	if (len > 1){\
		int ids[val];\
		int counts = parseIntArray<int>(strVal, ids, val);\
		if (counts op val) (*record)[id].setIntArray(ids, counts);\
			else goto parse_error;\
	}else{\
		(*record)[id].setIntArray(0, 0);\
	}

#define GetPropIntArrayPair(id, op, val)\
	strVal[0] = 0; len = sizeof(strVal);\
	if (!csv->GetString(row, col=id, strVal, len)) goto parse_error;\
	if (len > 1){\
		int ids[val];\
		int counts = parseIntArray<int>(strVal, ids, val);\
		if (((counts & 1) == 0) && (counts op val)) (*record)[id].setIntArray(ids, counts);\
			else goto parse_error;\
	}



	ulong rows = csv->GetRecordCount();
	mCsvFiledName = csv->GetFiledDescVector(); 
	mCsvFiledType = csv->GetFiledTypeVector();

	SkillViewData* curData = 0;

	ulong row = 0;
	ulong col = 0;

	char strVal[512] = {0};
	int len = sizeof(strVal);
	int intVal = 0;
	float fVal = 0.f;

	for (; row<rows; row++)
	{
		int objType = csv->GetInt(row, col = 0);
		if (objType == 1) // 大类
		{
			curData = new SkillViewData();

			XlsRecord* record = new XlsRecord();
			curData->setRecord(record);
			record->reserve(MaxSkillViewPropertyCount);

			// 下面解析大类数据
			GetPropInt(SkillViewProperty_Depth, 0);
			GetPropString(SkillViewProperty_Name);
			GetPropInt(SkillViewProperty_Id, 0);
			if (intVal == 0 || intVal >= MAX_SKILLVIEW_COUNT) goto parse_error;

			arr[record->getInt(SkillViewProperty_Id)] = curData;
		}
		else if (objType == 2) // 子类
		{
			if (!curData) return false;

			XlsRecord* record = new XlsRecord();
			record->reserve(MaxSkillViewPropertyCount);

			// 下面解析子技能数据
			GetPropInt(SkillViewProperty_Depth, 0);
			GetPropString(SkillViewProperty_Name);
			GetPropInt(SkillViewProperty_Id, 0);
			if (intVal == 0) goto parse_error;
			intVal = intVal % MAX_SUBSKILLVIEW_COUNT;
			if (intVal == 0 || intVal >= MAX_SUBSKILLVIEW_COUNT) goto parse_error;
			(*record)[SkillViewProperty_Id].setInt(intVal);
			GetPropInt(SkillViewProperty_EffectType, 0);
			GetPropInt(SkillViewProperty_NotAdjustDir, 0);
			GetPropInt(SkillViewProperty_AttackReadyActId, 0);
			GetPropFloat(SkillViewProperty_AttackReadyActSpeed, 1.f);
			GetPropInt(SkillViewProperty_AttackReadyMagicId, 0);
            GetPropInt(SkillViewProperty_AttackReadyMagicIdOnTarget, 0);
            GetPropInt(SkillViewProperty_AttackReadyMagicOnTargetAttachToTarget, 1);
			GetPropInt(SkillViewProperty_RapidOrChannel, 0);
			GetPropIntArray(SkillViewProperty_AttackActId,<=,3);
			GetPropFloat(SkillViewProperty_AttackActSpeed, 1.f);
			GetPropInt(SkillViewProperty_AttackMagicId, 0);
			GetPropInt(SkillViewProperty_AttackMagicAttachToSource, 1);
			GetPropInt(SkillViewProperty_BeHitMagicId, 0);
			GetPropInt(SkillViewProperty_BeHitFlyMagicId, 0);
			GetPropInt(SkillViewProperty_WoundMagicId, 0);
			GetPropInt(SkillViewProperty_TrackMagicId, 0);
            GetPropInt(SkillViewProperty_ExplodeMagicType, 0);
			GetPropInt(SkillViewProperty_ExplodeMagicID, 0);
            GetPropInt(SkillViewProperty_ExplodeMagicAttachToTarget, 1);
			GetPropInt(SkillViewProperty_Dummy, 0);
			GetPropFloat(SkillViewProperty_FlySpeed, 0.f);
			GetPropIntArray(SkillViewProperty_AttackRange,<=,3);
			GetPropInt(SkillViewProperty_Delay, 0);
			GetPropInt(SkillViewProperty_Times, 0);
			GetPropInt(SkillViewProperty_LifeTime, 0);
			GetPropInt(SkillViewProperty_Vibrate, 0);
			GetPropInt(SkillViewProperty_MagicID,0);
			GetPropInt(SkillViewProperty_SoundIDPreAttack,0);
			GetPropInt(SkillViewProperty_bSoundLoopPreAttack,0);
			GetPropIntArray(SkillViewProperty_SoundIDAttack,<=,3);
			GetPropInt(SkillViewProperty_bSoundLoopeAttack,0);
			GetPropInt(SkillViewProperty_SoundIDBeHit,0);
			GetPropInt(SkillViewProperty_bSoundLoopeBeHit,0);

			curData->setSubRecord(record->getInt(SkillViewProperty_Id), record);
		}
	}

	return true;

parse_error:
	int a = (col)/26;
	int b = (col)%26;
	char cc[3] = {0};
	if (a >= 1) cc[0] =  'A' + a;
	cc[a==0?0:1] = 'A' + b;
	Error("SkillView script, Cell["<<row+4<<", "<<cc<<"]: "<<"illegal data"<<endl);
	return false;
}

const XlsRecord* SkillViewDataManager::getRecord(ulong id) const
{
	ulong id1 = id / MAX_SUBSKILLVIEW_COUNT;
	if (id1 < MAX_SKILLVIEW_COUNT && mSkillViewDataArray[id1])
		return mSkillViewDataArray[id1]->getSubRecord(id % MAX_SUBSKILLVIEW_COUNT);

	Assert(id / MAX_SUBSKILLVIEW_COUNT < MAX_SKILLVIEW_COUNT);
	return NULL;
}

#define UNSIGNED_DIV_CONST(id, num, id1, id2) {id2=id; while(id2>num){id1++,id2-=num;}}
const XlsValue& SkillViewDataManager::getValue(ulong id, ulong field) const
{
	const XlsRecord* record = 0;
	ulong id1 = 0, id2 = 0;
	UNSIGNED_DIV_CONST(id, MAX_SUBSKILLVIEW_COUNT, id1, id2);
	if (id1 != 0 && id1 < MAX_SKILLVIEW_COUNT && mSkillViewDataArray[id1])
	{
		record = mSkillViewDataArray[id1]->getSubRecord(id2);
		if (record)
			return (*record)[field];
	}

	Assert((id1 != 0) && (id1 < MAX_SKILLVIEW_COUNT));
	return XlsRecord::sEmptyValue;
}

const std::string& SkillViewDataManager::getSkillViewDataName(uint id)
{
	return getValue(id, SkillViewProperty_Name).getString();
}

uint SkillViewDataManager::getSkillViewDataId(uint array, uint set)
{
	if (mSkillViewDataArray[array])
	{
		if (set == 0)
			return 0;

		const XlsRecord* bigtype = mSkillViewDataArray[array]->getRecord();
		const XlsRecord* record = mSkillViewDataArray[array]->getSubRecord(set);
		if (record)
		{
			int k = bigtype->getInt(SkillViewProperty_Id);
			int id = record->getInt(SkillViewProperty_Id);
			char buf[32];
			sprintf_s(buf, sizeof(buf), "%d%03d", k, id);
			return atoi(buf);
		}
	}
	return 0;
}

bool SkillViewDataManager::getSkillViewDataArrayIsValid(uint array)
{
	if (mSkillViewDataArray[array])
		return true;
	return false;
}

bool SkillViewDataManager::getSkillViewDataRowIsValid(uint array, uint set)
{
	if (getSkillViewDataArrayIsValid(array))
		if (mSkillViewDataArray[array]->getSubRecord(set))
			return true;
	return false;
}

void SkillViewDataManager::SetSkillViewData(uint id, std::string attackAct, uint swing, float flySpeed, 
											uint emitEffect, uint blastEffect, uint hitEffect, uint flyEffect,
											uint hitFlyEffect, uint readyEffect)
{
	XlsRecord* record = const_cast<XlsRecord*>(getRecord(id));
	IntArray arr;
	int buf[32];
	memset(buf, 0, sizeof(buf));
	arr.data = buf;
	if (record)
	{
		stringToIntArray(attackAct, arr);
	}
	record->setIntArray(SkillViewProperty_AttackActId, arr.data, arr.count);
	record->setInt(SkillViewProperty_Vibrate, swing);
	record->setFloat(SkillViewProperty_FlySpeed, flySpeed);
	record->setInt(SkillViewProperty_AttackMagicId, emitEffect);
	record->setInt(SkillViewProperty_ExplodeMagicID, blastEffect);
	record->setInt(SkillViewProperty_BeHitMagicId, hitEffect);
	record->setInt(SkillViewProperty_TrackMagicId, flyEffect);
	record->setInt(SkillViewProperty_BeHitFlyMagicId, hitFlyEffect);
	record->setInt(SkillViewProperty_AttackReadyMagicId, readyEffect);
}

void SkillViewDataManager::SaveSkillViewDataToCsv(const char* szFileName)
{
	ofstream of(szFileName, std::ios_base::out);
	if (!of.is_open())
		return;

	for (int m=0; m<MaxSkillViewPropertyCount; ++m)
	{
		of <<mCsvFiledName[m].c_str() << ",";
	}
	of << "\n";

	for (int n=0; n<MaxSkillViewPropertyCount; ++n)
	{
		int type = mCsvFiledType[n];
		CinCsvFiledTypeString(of, type);
	}
	of << "\n";

	for (int i=0; i<MAX_SKILLVIEW_COUNT; ++i)
	{
		if (mSkillViewDataArray[i])
		{
			for (int j=0; j<MAX_SUBSKILLVIEW_COUNT; ++j)
			{
				const XlsRecord* record = mSkillViewDataArray[i]->getSubRecord(j);
				if (record)
				{
					int bigtype = mSkillViewDataArray[i]->getRecord()->getInt(SkillViewProperty_Id);
					int smalltype = mSkillViewDataArray[i]->getSubRecord(j)->getInt(SkillViewProperty_Id);
					char buf[32];
					if (j == 0)
					{
						sprintf_s(buf, sizeof(buf), "%d", bigtype);
						of << record->getInt(SkillViewProperty_Depth) << ","
							<< record->getString(SkillViewProperty_Name) << ","
							<< buf << ",";
						CinCsvFillFiled(of, mCsvFiledName.size()-3);
						of << "\n";
					}
					else
					{
						sprintf_s(buf, sizeof(buf), "%d%03d", bigtype, smalltype);
						IntArray attackActId = record->getIntArray(SkillViewProperty_AttackActId);
						std::string attackActIdStr = attackActId.intArrayToString();
						IntArray attackRange = record->getIntArray(SkillViewProperty_AttackRange);
						IntArray attackSoundID = record->getIntArray(SkillViewProperty_SoundIDAttack);
						std::string attackRangeStr = attackRange.intArrayToString();
						std::string attackSoundStr = attackSoundID.intArrayToString();
						of << record->getInt(SkillViewProperty_Depth) << ","
							<< record->getString(SkillViewProperty_Name) << ","
							<< buf << ","
							<< record->getInt(SkillViewProperty_EffectType) << ","
							<< record->getInt(SkillViewProperty_NotAdjustDir) << ","
							<< record->getInt(SkillViewProperty_AttackReadyActId) << ","
							<< record->getFloat(SkillViewProperty_AttackReadyActSpeed) << ","
							<< record->getInt(SkillViewProperty_AttackReadyMagicId) << ","
							<< record->getInt(SkillViewProperty_AttackReadyMagicIdOnTarget) << ","
							<< record->getInt(SkillViewProperty_AttackReadyMagicOnTargetAttachToTarget) << ","
							<< record->getInt(SkillViewProperty_RapidOrChannel) << ","
							<< attackActIdStr.c_str() << ","
							<< record->getFloat(SkillViewProperty_AttackActSpeed) << ","
							<< record->getInt(SkillViewProperty_AttackMagicId) << ","
							<< record->getInt(SkillViewProperty_AttackMagicAttachToSource) << ","
							<< record->getInt(SkillViewProperty_BeHitMagicId) << ","
							<< record->getInt(SkillViewProperty_BeHitFlyMagicId) << ","
							<< record->getInt(SkillViewProperty_WoundMagicId) << ","
							<< record->getInt(SkillViewProperty_TrackMagicId) << ","
							<< record->getInt(SkillViewProperty_ExplodeMagicType) << ","
							<< record->getInt(SkillViewProperty_ExplodeMagicID) << ","
							<< record->getInt(SkillViewProperty_ExplodeMagicAttachToTarget) << ","
							<< record->getInt(SkillViewProperty_Dummy) << ","
							<< record->getFloat(SkillViewProperty_FlySpeed) << ","
							<< attackRangeStr.c_str() << ","
							<< record->getInt(SkillViewProperty_Delay) << ","
							<< record->getInt(SkillViewProperty_Times) << ","
							<< record->getInt(SkillViewProperty_LifeTime) << ","
							<< record->getInt(SkillViewProperty_Vibrate) << ","
							<< record->getInt(SkillViewProperty_MagicID) << ","
							<< record->getInt(SkillViewProperty_SoundIDPreAttack) << ","
							<< record->getInt(SkillViewProperty_bSoundLoopPreAttack) << ","
							<< attackSoundStr.c_str()<< ","
							<< record->getInt(SkillViewProperty_bSoundLoopeAttack) << ","
							<< record->getInt(SkillViewProperty_SoundIDBeHit) << ","
							<< record->getInt(SkillViewProperty_bSoundLoopeBeHit) << "\n";
					}
				}
			}
		}
	}
	of.close();
}