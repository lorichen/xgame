/******************************************************************
** 文件名:	PetBillboardForm.h
** 版  权:	(C)  
** 
** 日  期:	2009-11-2
** 版  本:	1.0
** 描  述:	宠物排行榜窗口			
** 应  用:  
******************************************************************/
#pragma once
#include "IGlobalClient.h"

class CPetBillboardForm : public SingletonEx<CPetBillboardForm>
{
	typedef vector<SMsgActionPetBillboardTable_SC> BillboardTop100;
public:

	void RequestSinglePetPropBillboard(LONGLONG uidPet);

	void RequestPetPropBillboard(DWORD petProp);

	// UI翻页
	void OpenBillboardRangePage(int pageIndex);

	// 缓存排行数据
	void OnBillboardReturn(DWORD dwMsgID, SGameMsgHead * pGameMsgHead, LPCSTR pszMsg, int nLen);

public:
	CPetBillboardForm();

	~CPetBillboardForm();

private:
	// 是否是成长率排行榜，成长率排行榜要显示浮点数
	bool IsGrowthBillboard();

	// 产生一行html文本
	void WriteHtmlRow(string& retStr,const SMsgActionPetBillboardTable_SC& billboardItem,string fontColor);

private:
	static const std::string	EventOnOpenPetBillboard;			//open pet properties billboard event
	LONGLONG uid_CurPet;
	BillboardTop100	m_PetBillboard_Top100;
	int lua_CurPageIndex;
	DWORD lua_CurBillboardType;
};
