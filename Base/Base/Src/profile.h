#define _ROOT_ "_ROOT_"
#include "IPerfProf.h"
#include "stdio.h"
#include <queue>
#include <string>
#include <map>

#ifdef _WIN32
	#include "windows.h"
	#include "CommCtrl.h"
#endif

#define NO_STR_CPY

namespace xs
{
#ifndef _NO_PERF_PROF
	class	CProfileNode : public IProfileNode
	{
	public:
		CProfileNode( const char * name, CProfileNode * parent );
		~CProfileNode( void );

		virtual IProfileNode * GetSubNode( const char * name );

		virtual IProfileNode * GetParent( void )		{ return Parent; }
		virtual IProfileNode * GetSibling( void )		{ return Sibling; }
		virtual IProfileNode * GetChild( void )		{ return Child; }

		virtual void				Reset( void );
		virtual void				Call( void );
		virtual bool				Return( void );

		virtual const char *	GetName( void )			{ return Name; }
		virtual int				GetTotalCalls( void )		{ return TotalCalls; }
		virtual float			GetTotalTime( void )		{ return TotalTime; }

		virtual float			GetAverageTime(){if(TotalCalls)return TotalTime / TotalCalls;return 0;}
		virtual float			GetMaxTime(){return MaxTime;}
		virtual float			GetMinTime(){return MinTime;}

	protected:
#ifdef NO_STR_CPY
		char*			Name;
#else
		char			Name[256];
#endif
		int				TotalCalls;
		float			TotalTime;
		float			MaxTime,MinTime;
		int64			StartTime;
		int				RecursionCounter;

		CProfileNode *	Parent;
		CProfileNode *	Child;
		CProfileNode *	Sibling;
	};

	class	CProfileManager {
	public:
		static	void						Start_Profile( const char * name );
		static	void						Stop_Profile( void );

		static	void						Reset( void );
		static	void						Increment_Frame_Counter( void );
		static	int							Get_Frame_Count_Since_Reset( void )		{ return FrameCounter; }
		static	float						Get_Time_Since_Reset( void );

		static IProfileNode*				GetRoot(){return &Root;}
	private:
		static	CProfileNode			Root;
		static	IProfileNode *			CurrentNode;
		static	int						FrameCounter;
		static	int64					ResetTime;
	};

	class CProfiler : public IProfile
	{
	protected:
		CProfiler()
		{
			//CreateProfileWindow();
			Reset();
		}
	public:
		static CProfiler *Instance()
		{
			static CProfiler p;
			return &p;
		}
		~CProfiler()
		{
			//DestroyProfileWindow();
		}
	private:
		void CreateProfileWindow();
		void DestroyProfileWindow();
		HTREEITEM InsertItem(HWND m_hTree,HTREEITEM hItem,char *szMsg);
		void BuildTree(HWND m_hTree,IProfileNode *pNode,HTREEITEM hItem);
		void WriteLog(int iTab,IProfileNode *pNode,FILE *fp);
		void GetDesc(char szMsg[512],IProfileNode *pNode);
		void WriteIntegratedStatisticsInfo(FILE * fp);
	public:
		virtual void Start(const char *name)
		{
			CProfileManager::Start_Profile(name);
		}
		virtual void Stop(void)
		{
			CProfileManager::Stop_Profile();
		}

		virtual void	Reset( void )
		{
			CProfileManager::Reset();
			m_integratedInfo.clear();
		}
		virtual void	IncrementFrameCounter( void )
		{
			CProfileManager::Increment_Frame_Counter();
		}
		virtual int		GetFrameCountSinceReset( void )
		{
			return CProfileManager::Get_Frame_Count_Since_Reset();
		}
		virtual float	GetTimeSinceReset( void )
		{
			return CProfileManager::Get_Time_Since_Reset();
		}

		virtual IProfileNode *		GetRoot()
		{
			return CProfileManager::GetRoot();
		}
		virtual void	Profile();
		virtual void	ShowUI();
		virtual void	HideUI();
		virtual void	WriteLog2File(char szFileName[]);
	public:
		HWND m_hDlg;
		HWND m_hTree;
	private:
		struct IntegratedStatisticsInfo
		{
			std::string m_desc;
			int	m_totalCalls;
			float m_totalTime;
			float m_averageTime;
		};
		typedef std::map<std::string, IntegratedStatisticsInfo> IntegratedStatisticsInfoSet;
		IntegratedStatisticsInfoSet m_integratedInfo;
	};
#endif
}