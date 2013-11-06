#ifndef __VISUALCOMPONENTSTATE_H__
#define __VISUALCOMPONENTSTATE_H__
#include <map>

class VisualComponentMZ;

class CVisualComponentState //组件状态接口
{
	friend class  CVisualComponentStateMgr;
private:
	void OnEnter();
	void OnLeave();
	void SetState(EVisualComponentState state);
private:
	EVisualComponentState m_eState;//状态
};

class CVisualComponentStateMgr: public TimerHandler
{
private:
	CVisualComponentState m_StateSet[EVCS_Max];
	VisualComponentMZ * m_pMaster;
	EVisualComponentState m_CurState;
	const static DWORD s_fight2PeaceInterval;

public:
	////////////////////TimerHandler////////////////////
	void OnTimer(unsigned long dwTimerID);
public:
	CVisualComponentStateMgr();
	bool Create(VisualComponentMZ * pComp);
	~CVisualComponentStateMgr();
	void OnCommand(EntityCommand cmd);//不同的命令会改变当前状态
	inline EVisualComponentState GetCurrentState() { return m_CurState; };//获取当前状态
	void initCommandToStateMap();
private:
	static bool		s_bInitMapCommandToState;//是否初始化表
	typedef std::map<uint, EVisualComponentState> CommandToStateMap;
	static CommandToStateMap* s_pMapCommandToState;//实体命令到状态的映射
	int				m_nCurCommand;
};

#endif
