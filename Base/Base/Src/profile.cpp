#include "stdafx.h"
#include "Common.h"
#include "profile.h"
#include "Limits.h"
#include "Resource.h"

#define new RKT_NEW

namespace xs
{
#ifndef _NO_PERF_PROF
	inline void Profile_Get_Ticks(__int64 * ticks)
	{
		LARGE_INTEGER *p = (LARGE_INTEGER *)ticks;
		QueryPerformanceCounter(p);
		/*
		__asm
		{
			push edx;
			push ecx;
			mov ecx,ticks;
			_emit 0Fh
			_emit 31h
			mov [ecx],eax;
			mov [ecx+4],edx;
			pop ecx;
			pop edx;
		}
		*/
	}

	inline float Profile_Get_Tick_Rate(void)
	{
		static float _CPUFrequency = -1.0f;
		
		if (_CPUFrequency == -1.0f) {
			__int64 curr_rate = 0;
			::QueryPerformanceFrequency ((LARGE_INTEGER *)&curr_rate);
			_CPUFrequency = (float)curr_rate;
		} 
		
		return _CPUFrequency;
	}

	CProfileNode::CProfileNode( const char * name, CProfileNode * parent ) :
		TotalCalls( 0 ),
		TotalTime( 0 ),
		MaxTime(0),
		MinTime((float)_I64_MAX),
		StartTime( 0 ),
		RecursionCounter( 0 ),
		Parent( parent ),
		Child( NULL ),
		Sibling( NULL )
	{
#ifdef NO_STR_CPY
		this->Name = (char*)name;
#else
		strcpy(this->Name,name);
#endif
		Reset();
	}


	CProfileNode::~CProfileNode( void )
	{
		safeDelete(Child);
		safeDelete(Sibling);
	}

	IProfileNode * CProfileNode::GetSubNode( const char * name )
	{
		CProfileNode * child = Child;
		while ( child )
		{
#ifdef NO_STR_CPY
			if(child->Name == name){
#else
			if ( strcmp(child->Name , name ) == 0) {
#endif
				return child;
			}
			child = child->Sibling;
		}
		CProfileNode * node = new CProfileNode( name, this );
		CProfileNode *pTemp = Child;
		CProfileNode *pPre = 0;
		while(pTemp)
		{
			pPre = pTemp;
			pTemp = (CProfileNode*)pTemp->GetSibling();
		}
		if(pPre)
		{
			pPre->Sibling = node;
		}
		else
		{
			Child = node;
		}
		return node;
	}


	void	CProfileNode::Reset( void )
	{
		TotalCalls = 0;
		TotalTime = 0.0f;
		MinTime = (float)_I64_MAX;
		MaxTime = 0;

		if ( Child ) {
			Child->Reset();
		}
		if ( Sibling ) {
			Sibling->Reset();
		}
	}


	void	CProfileNode::Call( void )
	{
		TotalCalls++;
		if (RecursionCounter++ == 0) {
			Profile_Get_Ticks(&StartTime);
		}
	}


	bool	CProfileNode::Return( void )
	{
		if ( --RecursionCounter == 0 && TotalCalls != 0 ) { 
			__int64 time;
			Profile_Get_Ticks(&time);
			time-=StartTime;

			float fTime = (float)time / Profile_Get_Tick_Rate(); 
			TotalTime += fTime;

			if(fTime > MaxTime)MaxTime = fTime;
			if(fTime < MinTime)
			{
				MinTime = fTime;

				//char sz[256];
				//sprintf(sz,"%f\n",MinTime);
				//OutputDebugString(sz);
			}
		}
		return ( RecursionCounter == 0 );
	}

	CProfileNode	CProfileManager::Root(_ROOT_, NULL );
	IProfileNode *	CProfileManager::CurrentNode = &CProfileManager::Root;
	int				CProfileManager::FrameCounter = 0;
	__int64			CProfileManager::ResetTime = 0;

	void	CProfileManager::Start_Profile( const char * name )
	{
#ifdef NO_STR_CPY
		if(name != CurrentNode->GetName())
#else
		if (strcmp(name,CurrentNode->GetName())) 
#endif
		{
			CurrentNode = CurrentNode->GetSubNode( name );
		} 
		
		CurrentNode->Call();
	}

	void	CProfileManager::Stop_Profile( void )
	{
		if (CurrentNode->Return()) {
			CurrentNode = CurrentNode->GetParent();
		}
	}

	void	CProfileManager::Reset( void )
	{ 
		Root.Reset(); 
		FrameCounter = 0;
		Profile_Get_Ticks(&ResetTime);
	}

	void CProfileManager::Increment_Frame_Counter( void )
	{
		FrameCounter++;
	}

	float CProfileManager::Get_Time_Since_Reset( void )
	{
		__int64 time;
		Profile_Get_Ticks(&time);
		time -= ResetTime;
		return (float)time / Profile_Get_Tick_Rate();
	}

	LRESULT CALLBACK DlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
	{
		switch(message)
		{
		case WM_INITDIALOG:
			{
				RECT rect;
				GetClientRect(hWnd,&rect);

				CProfiler *pProfiler = (CProfiler*)lParam;
				
				pProfiler->m_hTree = CreateWindow(WC_TREEVIEW, "", 
					WS_CHILD|WS_VISIBLE|
					TVS_HASLINES|TVS_HASBUTTONS|TVS_LINESATROOT|TVS_SHOWSELALWAYS|TVS_INFOTIP, 
					rect.left,rect.top,rect.right - rect.left,rect.bottom - rect.top,hWnd, (HMENU)0, GetModuleHandle(MAKE_DLL_NAME(Base)), NULL);
				if(!pProfiler->m_hTree)
				{
					MessageBox(0,"Can't Create TreeView",0,0);
				}

				DWORD dw = GetLastError();
				
				DWORD dwStyle = GetWindowLong(pProfiler->m_hTree,GWL_EXSTYLE);
				dwStyle |= WS_EX_CLIENTEDGE;
				SetWindowLong(pProfiler->m_hTree,GWL_EXSTYLE,dwStyle);
			}
			return FALSE;
		case WM_CLOSE:
			ShowWindow(hWnd,SW_HIDE);
			return TRUE;
		case WM_SIZE:
			{
				RECT rect;
				GetClientRect(hWnd,&rect);
				MoveWindow(CProfiler::Instance()->m_hTree,rect.left,rect.top,rect.right - rect.left,rect.bottom - rect.top,TRUE);
			}
			break;
		}
		return FALSE;
	}

	void CProfiler::CreateProfileWindow()
	{
		::InitCommonControls();
		HINSTANCE hInstance = GetModuleHandle(MAKE_DLL_NAME(Base));
		m_hDlg = CreateDialogParamA(hInstance,"IDD_PROFILE",GetDesktopWindow(),(DLGPROC)DlgProc,(DWORD)this);
		ShowWindow(m_hDlg,SW_SHOWMAXIMIZED);
		ShowWindow(m_hDlg,SW_HIDE);
	}
	void CProfiler::DestroyProfileWindow()
	{
		if(m_hDlg)DestroyWindow(m_hDlg);
	}
	HTREEITEM CProfiler::InsertItem(HWND m_hTree,HTREEITEM hItem,char *szMsg)
	{
		TVITEM ti;
		TVINSERTSTRUCT tis;
		ti.mask=TVIF_TEXT;
		ti.pszText=szMsg;
		tis.item=ti;
		tis.hParent=hItem;
		tis.hInsertAfter=0;
		return (HTREEITEM)SendMessage(m_hTree,TVM_INSERTITEM,0,(LPARAM)&tis);
	}

	void CProfiler::GetDesc(char szMsg[512],IProfileNode *pNode)
	{
		if(stricmp(pNode->GetName(),_ROOT_) == 0)
			sprintf(szMsg,"总时间 = %f秒",GetTimeSinceReset());
		else
			sprintf(szMsg,"调用次数 = %8d 总时间 = %6.4f秒 平均 = %6.4f秒 最小 = %6.4f秒 最大 = %6.4f秒 fps = %6.4f",pNode->GetTotalCalls(),pNode->GetTotalTime(),pNode->GetAverageTime(),pNode->GetMinTime(),pNode->GetMaxTime(),1.0f / pNode->GetAverageTime());		
	}
	void CProfiler::BuildTree(HWND m_hTree,IProfileNode *pNode,HTREEITEM hItem)
	{
		if(!pNode)return;

		char szMsg[256];
		GetDesc(szMsg,pNode);

		char szItem[512];
		char *pName = (char*)pNode->GetName();
		if(!pName)return;

		int iSize = strlen(pName);
		strcpy(szItem,pNode->GetName());
		for(int k = 0;k < 32 - iSize;k++)
		{
			strcat(szItem," ");
		}
		strcat(szItem,szMsg);
		
		HTREEITEM hNow = InsertItem(m_hTree,hItem,szItem);

		TreeView_Expand(m_hTree,hItem,TVE_EXPAND);

		IProfileNode *pChild = pNode->GetChild();
		if(pChild)
		{
			while(pChild)
			{
				BuildTree(m_hTree,pChild,hNow);
				pChild = pChild->GetSibling();
			}
		}
	}

	void CProfiler::WriteLog(int iTab,IProfileNode *pNode,FILE *fp)
	{
		if(!pNode || !fp)return;

		char szMsg[512];
		GetDesc(szMsg,pNode);

		char *pName = (char*)pNode->GetName();
		if(!pName)return;

		int iSize = strlen(pName);
		fprintf(fp,pNode->GetName());
		for(int k = 0;k < 32 - iSize;k++)
		{
			fprintf(fp," ");
		}

		for(int i = 0;i < iTab;i++)fprintf(fp,"    ");
		fprintf(fp,szMsg);
		fprintf(fp,"\r\n");

		//综合统计信息
		std::string strName(pNode->GetName());
		IntegratedStatisticsInfoSet::iterator it = m_integratedInfo.find(strName);
		if( it == m_integratedInfo.end() )
		{
			IntegratedStatisticsInfo info;
			info.m_desc = strName;
			info.m_totalCalls = pNode->GetTotalCalls();
			info.m_totalTime = pNode->GetTotalTime();
			info.m_averageTime = pNode->GetAverageTime();
			m_integratedInfo[strName] = info;
		}
		else
		{
			it->second.m_totalCalls += pNode->GetTotalCalls();
			it->second.m_totalTime += pNode->GetTotalTime();
			it->second.m_averageTime = it->second.m_totalTime / (float)(it->second.m_totalCalls);
		}

		IProfileNode *pChild = pNode->GetChild();
		if(pChild)
		{
			while(pChild)
			{
				WriteLog(iTab + 1,pChild,fp);
				pChild = pChild->GetSibling();
			}
		}
	}

	void CProfiler::WriteIntegratedStatisticsInfo(FILE * fp)
	{
		if( 0 == fp) return;

		char szMsg[512];
		uint rankLen = 0;
		struct StatisticsInfo
		{
			std::string m_desc;
			float		m_info;
			bool	operator()( StatisticsInfo & p1, StatisticsInfo &p2){ return p1.m_info < p2.m_info;}
		};
		typedef std::priority_queue<StatisticsInfo, std::vector<StatisticsInfo>, StatisticsInfo > ProfileRankInfo;
		ProfileRankInfo totalCallsRank;
		ProfileRankInfo totalTimeRank;
		ProfileRankInfo averageTimeRank;

		//整合后的统计数据
		fprintf(fp,"\r\n");
		fprintf(fp,"\r\n");
		sprintf(szMsg, "////////////////////////////////////////////////////////////////////////\r\n");
		fprintf(fp, szMsg);
		sprintf(szMsg, "//////////////////////////合并后的统计数据//////////////////////////////\r\n");
		fprintf(fp,szMsg);
		sprintf(szMsg, "////////////////////////////////////////////////////////////////////////\r\n");
		fprintf(fp,szMsg);
		IntegratedStatisticsInfoSet::iterator it = m_integratedInfo.begin();
		for( ; it != m_integratedInfo.end(); ++it)
		{
			StatisticsInfo info;
			info.m_desc = it->second.m_desc;

			info.m_info = (float)it->second.m_totalCalls;
			totalCallsRank.push(info);

			info.m_info = it->second.m_totalTime;
			totalTimeRank.push(info);

			info.m_info = it->second.m_averageTime;
			averageTimeRank.push(info);

			sprintf(szMsg,"%s\t调用次数=%8d 总时间=%6.4f秒 平均=%6.6f秒\r\n",
				it->second.m_desc.c_str(),
				it->second.m_totalCalls, 
				it->second.m_totalTime, 
				it->second.m_averageTime);
			fprintf(fp, szMsg);
		}

		//最大调用次数
		fprintf(fp,"\r\n");
		fprintf(fp,"\r\n");
		sprintf(szMsg, "////////////////////////////////////////////////////////////////////////\r\n");
		fprintf(fp, szMsg);
		sprintf(szMsg, "//////////////////////////合并后最大调用次数//////////////////////////////\r\n");
		fprintf(fp,szMsg);
		sprintf(szMsg, "////////////////////////////////////////////////////////////////////////\r\n");
		fprintf(fp,szMsg);
		rankLen = totalCallsRank.size();
		for( uint i=0; i< rankLen; ++i)
		{
			sprintf(szMsg,"%s\t%.6f\r\n", totalCallsRank.top().m_desc.c_str(), totalCallsRank.top().m_info);
			fprintf(fp, szMsg);
			totalCallsRank.pop();
		}

		//最长调用时间
		fprintf(fp,"\r\n");
		fprintf(fp,"\r\n");
		sprintf(szMsg, "////////////////////////////////////////////////////////////////////////\r\n");
		fprintf(fp, szMsg);
		sprintf(szMsg, "//////////////////////////合并后最长调用时间//////////////////////////////\r\n");
		fprintf(fp,szMsg);
		sprintf(szMsg, "////////////////////////////////////////////////////////////////////////\r\n");
		fprintf(fp,szMsg);
		rankLen = totalTimeRank.size();
		for( uint i =0; i <rankLen; ++i)
		{
			sprintf(szMsg,"%s\t%.6f\r\n", totalTimeRank.top().m_desc.c_str(), totalTimeRank.top().m_info);
			fprintf(fp, szMsg);
			totalTimeRank.pop();	
		}


		//平均最长调用时间
		fprintf(fp,"\r\n");
		fprintf(fp,"\r\n");
		sprintf(szMsg, "////////////////////////////////////////////////////////////////////////\r\n");
		fprintf(fp, szMsg);
		sprintf(szMsg, "//////////////////////////合并后平均最长调用时间//////////////////////////////\r\n");
		fprintf(fp,szMsg);
		sprintf(szMsg, "////////////////////////////////////////////////////////////////////////\r\n");
		fprintf(fp,szMsg);
		rankLen = averageTimeRank.size() ;
		for( uint i =0; i <rankLen; ++i)
		{
			sprintf(szMsg,"%s\t%.6f\r\n", averageTimeRank.top().m_desc.c_str(), averageTimeRank.top().m_info);
			fprintf(fp, szMsg);
			averageTimeRank.pop();	
		}
	}

	void CProfiler::WriteLog2File(char szFileName[])
	{
		FILE *fp = fopen(szFileName,"wb+");
		if(!fp)return;
		IProfileNode *pNode = GetRoot();
		if(pNode)
		{
			WriteLog(0,pNode,fp);
		}

		WriteIntegratedStatisticsInfo(fp);

		fclose(fp);
	}
#endif
}