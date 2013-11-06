//==========================================================================
/**
* @file	  : MagicData.cpp
* @author : 
* created : 2008-3-13   16:45
*/
//==========================================================================

#include "stdafx.h"
#include "MagicData.h"
#include <fstream>



MagicDataManager::MagicDataManager()
{
	memset(mMagicDataArray, 0, sizeof(mMagicDataArray));
}

MagicDataManager::~MagicDataManager()
{
	clearArray(mMagicDataArray);
}

void MagicDataManager::clearArray(MagicData** arr)
{
	for (int i=0; i<MAX_MAGIC_COUNT; i++)
		safeDelete(arr[i]);
}

bool MagicDataManager::OnSchemeLoad(ICSVReader* pCSVReader,const char* szFileName)
{
	return OnSchemeUpdate(pCSVReader, szFileName);
}

bool MagicDataManager::OnSchemeUpdate(ICSVReader* pCSVReader, const char* szFileName)
{
	if (pCSVReader)
	{
		if (!read(pCSVReader))
		{
			Error("MagicDataManager::read failed"<<endl);
			return false;
		}

		return true;
	}

	Assert(pCSVReader != NULL);
	return false;
}

bool MagicDataManager::read(ICSVReader* csv)
{
	Assert(csv != NULL);

	MagicData* arr[MAX_MAGIC_COUNT];
	memset(arr, 0, sizeof(arr));

	if (!innerRead(csv, arr))
	{
		clearArray(arr);
		return false;
	}

	clearArray(mMagicDataArray);

	// 更新
	memcpy(mMagicDataArray, arr, sizeof(mMagicDataArray));

	return true;
}

bool MagicDataManager::innerRead(ICSVReader* csv, MagicData** arr)
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
	csv->GetString(row, col=id, strVal, len);\
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


	MagicData* curData = 0;

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
			curData = new MagicData();

			XlsRecord* record = new XlsRecord();
			curData->setRecord(record);
			record->reserve(MaxMagicPropertyCount);

			// 下面解析大类数据
			GetPropInt(MagicProperty_Depth, 0);
			GetPropString(MagicProperty_Name);
			GetPropInt(MagicProperty_Id, 0);
			if (intVal == 0 || intVal >= MAX_MAGIC_COUNT) goto parse_error;

			arr[record->getInt(MagicProperty_Id)] = curData;
		}
		else if (objType == 2) // 子类
		{
			if (!curData) return false;

			XlsRecord* record = new XlsRecord();
			record->reserve(MaxMagicPropertyCount);

			// 下面解析子技能数据
			GetPropInt(MagicProperty_Depth, 0);
			GetPropString(MagicProperty_Name);
			GetPropInt(MagicProperty_Id, 0);
			if (intVal == 0) goto parse_error;
			intVal = intVal % MAX_SUBMAGIC_COUNT;
			if (intVal == 0 || intVal >= MAX_SUBMAGIC_COUNT) goto parse_error;
			(*record)[MagicProperty_Id].setInt(intVal);
			GetPropInt(MagicProperty_ResId, 0);
			GetPropInt(MagicProperty_ResType, 0);
			GetPropInt(MagicProperty_AniType, 0);
			GetPropFloat(MagicProperty_AniSpeed, 1.f);
			GetPropFloat(MagicProperty_AniScale, 1.f);
			GetPropInt(MagicProperty_AniDir, 0);
			GetPropIntArray(MagicProperty_AniColor, ==, 3);
			GetPropInt(MagicProperty_AniTrans, 255);
			GetPropIntArray(MagicProperty_AniOffset, ==, 3);

			curData->setSubRecord(record->getInt(MagicProperty_Id), record);
		}
	}

	return true;

parse_error:
	int a = (col)/26;
	int b = (col)%26;
	char cc[3] = {0};
	if (a >= 1) cc[0] =  'A' + a;
	cc[a==0?0:1] = 'A' + b;
	Error("Magic script, Cell["<<row+4<<", "<<cc<<"]: "<<"illegal data"<<endl);
	return false;
}

const MagicData* MagicDataManager::getMagicData(ulong index) const
{
	if (index < MAX_MAGIC_COUNT)
		return mMagicDataArray[index];

	Assert(index < MAX_MAGIC_COUNT);
	return NULL;
}

#define UNSIGNED_DIV_CONST(id, num, id1, id2) {id2=id; while(id2>num){id1++,id2-=num;}}
const XlsValue& MagicDataManager::getValue(ulong id, ulong field) const
{
	const XlsRecord* record = 0;
	ulong id1 = 0, id2 = 0;
	UNSIGNED_DIV_CONST(id, MAX_SUBMAGIC_COUNT, id1, id2);
	if (id1 != 0 && id1 < MAX_MAGIC_COUNT && mMagicDataArray[id1])
	{
		record = mMagicDataArray[id1]->getSubRecord(id2);
		if (record)
			return (*record)[field];
	}

	Assert((id1 != 0) && (id1 < MAX_MAGIC_COUNT));
	return XlsRecord::sEmptyValue;
}
uint MagicDataManager::getMagicDataId(uint array, uint set)
{
	if (mMagicDataArray[array])
	{
		if (set == 0)
			return 0;

		const XlsRecord* bigtype = mMagicDataArray[array]->getRecord();
		const XlsRecord* record = mMagicDataArray[array]->getSubRecord(set);
		if (record)
		{
			int k = bigtype->getInt(MagicProperty_Id);
			int id = record->getInt(MagicProperty_Id);
			char buf[32];
			sprintf_s(buf, sizeof(buf), "%d%03d", k, id);
			return atoi(buf);
		}
	}
	return 0;
}

const std::string& MagicDataManager::getMagicDataName(uint id)
{
	return getValue(id, 1).getString();
}

bool MagicDataManager::getMagicDataArrayIsValid(uint array)
{
	if (mMagicDataArray[array])
		return true;
	return false;
}

bool MagicDataManager::getMagicDataRowIsValid(uint array, uint set)
{
	if (getMagicDataArrayIsValid(array))
		if (mMagicDataArray[array]->getSubRecord(set))
			return true;
	return false;
}

void MagicDataManager::SetMagicData(uint id, float aniSpeed, uint aniTrans, 
									std::string aniOffset, float aniScale, uint resId)
{
	XlsRecord* record = const_cast<XlsRecord*>(getRecord(id));
	IntArray arr;
	int buf[32];
	memset(buf, 0, sizeof(buf));
	arr.data = buf;
	if (record)
	{
		stringToIntArray(aniOffset, arr);
	}
	record->setFloat(MagicProperty_AniSpeed, aniSpeed);
	record->setInt(MagicProperty_AniTrans, aniTrans);
	record->setIntArray(MagicProperty_AniOffset, arr.data, arr.count);
	record->setFloat(MagicProperty_AniScale, aniScale);
	record->setInt(MagicProperty_ResId, resId);
}

const XlsRecord* MagicDataManager::getRecord(ulong id) const
{
	ulong id1 = id / MAX_SUBMAGIC_COUNT;
	if (id1 < MAX_MAGIC_COUNT && mMagicDataArray[id1])
		return mMagicDataArray[id1]->getSubRecord(id % MAX_SUBMAGIC_COUNT);

	Assert(id / MAX_SUBMAGIC_COUNT < MAX_MAGIC_COUNT);
	return NULL;
}

void MagicDataManager::SaveMagicDataToCsv(const char* szFileName)
{
 	ofstream of(szFileName, std::ios_base::out);
 	if (!of.is_open())
 		return;

	for (int m=0; m<MaxMagicPropertyCount; ++m)
	{
		of <<mCsvFiledName[m].c_str() << ",";
	}
	of << "\n";

	for (int n=0; n<MaxMagicPropertyCount; ++n)
	{
		int type = mCsvFiledType[n];
		CinCsvFiledTypeString(of, type);
	}
	of << "\n";
	
	for (int i=0; i<MAX_MAGIC_COUNT; ++i)
	{
		if (mMagicDataArray[i])
		{
			for (int j=0; j<MAX_SUBMAGIC_COUNT; ++j)
			{
				const XlsRecord* record = mMagicDataArray[i]->getSubRecord(j);
				if (record)
				{
					int bigtype = mMagicDataArray[i]->getRecord()->getInt(MagicProperty_Id);
					int smalltype = mMagicDataArray[i]->getSubRecord(j)->getInt(MagicProperty_Id);
					char buf[32];
					if (j == 0)
					{
						sprintf_s(buf, sizeof(buf), "%d", bigtype);
						of << record->getInt(MagicProperty_Depth) << ","
							<< record->getString(MagicProperty_Name) << ","
							<< buf << ","
							<< "" << ","
							<< "" << ","
							<< "" << ","
							<< "" << ","
							<< "" << ","
							<< "" << ","
							<< "" << ","
							<< "" << ","
							<< "" << "\n";
					}
					else
					{
						sprintf_s(buf, sizeof(buf), "%d%03d", bigtype, smalltype);
						IntArray color = record->getIntArray(MagicProperty_AniColor);
						std::string colorStr = color.intArrayToString();
						IntArray offset = record->getIntArray(MagicProperty_AniOffset);
						std::string offsetStr = offset.intArrayToString();
						of << record->getInt(MagicProperty_Depth) << ","
							<< record->getString(MagicProperty_Name) << ","
							<< buf << ","
							<< record->getInt(MagicProperty_ResId) << ","
							<< record->getInt(MagicProperty_ResType) << ","
							<< record->getInt(MagicProperty_AniType) << ","
							<< record->getFloat(MagicProperty_AniSpeed) << ","
							<< record->getFloat(MagicProperty_AniScale) << ","
							<< record->getInt(MagicProperty_AniDir) << ","
							<< colorStr.c_str() << ","
							<< record->getInt(MagicProperty_AniTrans) << ","
							<< offsetStr.c_str() << "\n";
					}
				}
			}
		}
	}
	of.close();
}

