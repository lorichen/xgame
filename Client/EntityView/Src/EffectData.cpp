//==========================================================================
/**
* @file	  : EffectData.cpp
* @author : 
* created : 2008-3-13   16:45
*/
//==========================================================================

#include "stdafx.h"
#include "EffectData.h"


EffectDataManager::EffectDataManager()
{
	memset(mEffectDataArray, 0, sizeof(mEffectDataArray));
}

EffectDataManager::~EffectDataManager()
{
	clearArray(mEffectDataArray);
}

void EffectDataManager::clearArray(EffectData** arr)
{
	for (int i=0; i<MAX_EFFECT_COUNT; i++)
		safeDelete(arr[i]);
}

bool EffectDataManager::OnSchemeLoad(ICSVReader* pCSVReader,const char* szFileName)
{
	return OnSchemeUpdate(pCSVReader, szFileName);
}

bool EffectDataManager::OnSchemeUpdate(ICSVReader* pCSVReader, const char* szFileName)
{
	if (pCSVReader)
	{
		if (!read(pCSVReader))
		{
			Error("EffectDataManager::read failed"<<endl);
			return false;
		}

		return true;
	}

	Assert(pCSVReader != NULL);
	return false;
}

bool EffectDataManager::read(ICSVReader* csv)
{
	Assert(csv != NULL);

	EffectData* arr[MAX_EFFECT_COUNT];
	memset(arr, 0, sizeof(arr));

	if (!innerRead(csv, arr))
	{
		clearArray(arr);
		return false;
	}

	clearArray(mEffectDataArray);

	// 更新
	memcpy(mEffectDataArray, arr, sizeof(mEffectDataArray));

	return true;
}

bool EffectDataManager::innerRead(ICSVReader* csv, EffectData** arr)
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

	EffectData* curData = 0;

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
			curData = new EffectData();

			XlsRecord* record = new XlsRecord();
			curData->setRecord(record);
			record->reserve(MaxEffectPropertyCount);

			// 下面解析大类数据
			//GetPropString(EffectProperty_Name);
			GetPropInt(EffectProperty_Id, 0);
			if (intVal == 0 || intVal >= MAX_EFFECT_COUNT) goto parse_error;

			arr[record->getInt(EffectProperty_Id)] = curData;
		}
		else if (objType == 2) // 子类
		{
			if (!curData) return false;

			XlsRecord* record = new XlsRecord();
			record->reserve(MaxEffectPropertyCount);

			// 下面解析子技能数据
			//GetPropString(EffectProperty_Name);
			GetPropInt(EffectProperty_Id, 0);
			if (intVal == 0) goto parse_error;
			intVal = intVal % MAX_SUBEFFECT_COUNT;
			if (intVal == 0 || intVal >= MAX_SUBEFFECT_COUNT) goto parse_error;
			(*record)[EffectProperty_Id].setInt(intVal);
			GetPropInt(EffectProperty_MagicId, 0);
			GetPropInt(EffectProperty_Loops, -1);
			GetPropInt(EffectProperty_TailSilence, 0);
			GetPropIntArray(EffectProperty_Color, ==, 4);
			GetPropFloat(EffectProperty_Scale, 1.0f);
			GetPropInt(EffectProperty_NeedStunAction, 0);
			GetPropInt(EffectProperty_NeedShadow, 0);
			GetPropIntArray(EffectProperty_ChangeSharp, ==, 2);

			curData->setSubRecord(record->getInt(EffectProperty_Id), record);
		}
	}

	return true;

parse_error:
	int a = (col)/26;
	int b = (col)%26;
	char cc[3] = {0};
	if (a >= 1) cc[0] =  'A' + a;
	cc[a==0?0:1] = 'A' + b;
	Error("EffectView script, Cell["<<row+4<<", "<<cc<<"]: "<<"illegal data"<<endl);
	return false;
}

const EffectData* EffectDataManager::getEffectData(ulong index) const
{
	if (index < MAX_EFFECT_COUNT)
		return mEffectDataArray[index];

	Assert(index < MAX_EFFECT_COUNT);
	return NULL;
}

#define UNSIGNED_DIV_CONST(id, num, id1, id2) {id2=id; while(id2>num){id1++,id2-=num;}}
bool EffectDataManager::HasRecord(ulong ulId) const
{
    const XlsRecord* pRecord = 0;
    ulong ulId1 = 0;
    ulong ulId2 = 0;
    UNSIGNED_DIV_CONST(ulId, MAX_SUBEFFECT_COUNT, ulId1, ulId2);
    if (ulId1 != 0 && ulId1 < MAX_EFFECT_COUNT && mEffectDataArray[ulId1])
    {
        pRecord = mEffectDataArray[ulId1]->getSubRecord(ulId2);
    }
    
    return (pRecord != NULL);
}

const XlsValue& EffectDataManager::getValue(ulong id, ulong field) const
{
    const XlsRecord* record = 0;
    ulong id1 = 0, id2 = 0;
    UNSIGNED_DIV_CONST(id, MAX_SUBEFFECT_COUNT, id1, id2);
    if (id1 > 0 && id1 < MAX_EFFECT_COUNT && mEffectDataArray[id1])
    {
        record = mEffectDataArray[id1]->getSubRecord(id2);
        if (record)
            return (*record)[field];
    }

    Assert((id1 != 0) && (id1 < MAX_EFFECT_COUNT));
    return XlsRecord::sEmptyValue;
}
