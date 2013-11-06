#include "StdAfx.h"
#include "VisualComponentState.h"
#include "VisualComponentMZ.h"

////////////////////CVisualComponentState////////////////////////////////

bool CVisualComponentStateMgr::s_bInitMapCommandToState = false;//是否初始化表
CVisualComponentStateMgr::CommandToStateMap * CVisualComponentStateMgr::s_pMapCommandToState = 0;//实体命令到状态的映射

void CVisualComponentState::OnEnter()
{

}

void CVisualComponentState::OnLeave()
{

}

void CVisualComponentState::SetState( EVisualComponentState state )
{
	m_eState = state;
}


//////////////////////////////////CVisualComponentStateMgr//////////////////////////

const DWORD CVisualComponentStateMgr::s_fight2PeaceInterval = 8 * 1000; // modify by zjp；策划要求8秒中的战斗状态保持时间

CVisualComponentStateMgr::CVisualComponentStateMgr()
{
	m_StateSet[EVCS_OnPeace].SetState(EVCS_OnPeace);
	m_StateSet[EVCS_OnFight].SetState(EVCS_OnFight);
	m_CurState = EVCS_OnPeace;
	m_nCurCommand = EntityCommand_Max;
}

bool CVisualComponentStateMgr::Create(VisualComponentMZ * pComp)
{
	if( !pComp) return false;
	m_pMaster = pComp;
	return true;
}

CVisualComponentStateMgr::~CVisualComponentStateMgr()
{
	gGlobalClient->getTimerAxis()->KillTimer(0,this);
	m_nCurCommand = EntityCommand_Max;
}

void CVisualComponentStateMgr::OnTimer(unsigned long dwTimerID)
{
	if (m_nCurCommand==EntityCommand_AttackReady)
	{
		gGlobalClient->getTimerAxis()->KillTimer(0,this);
		gGlobalClient->getTimerAxis()->SetTimer(0, s_fight2PeaceInterval,this,1);		
		return;
	}
	if( m_CurState ==EVCS_OnFight) 
	{
		m_StateSet[EVCS_OnFight].OnLeave();
		m_CurState = EVCS_OnPeace;
		m_StateSet[EVCS_OnPeace].OnLeave();
		if( !m_pMaster) return;
		int actionid = m_pMaster->getCurrentAction();

		if (actionid == EntityAction_Death)
			return;
		
		//忽略动作同步
		if( actionid == EntityAction_Walk  || actionid == EntityAction_Run ) 
			m_pMaster->handleCommand(EntityCommand_Move,1,0);
		else
			m_pMaster->handleCommand(EntityCommand_Stand,1,0);
	}
}

void CVisualComponentStateMgr::initCommandToStateMap()
{
	CommandToStateMap* pMap = new CommandToStateMap();

	(*pMap)[EntityCommand_RapidMoveStart] = EVCS_OnFight;	/// 快速移动(瞬移、冲锋)开始 (ShadowManager* mgr, 0)
	(*pMap)[EntityCommand_RapidMoveEnd] = EVCS_OnFight;		/// 快速移动(瞬移、冲锋)结束
	(*pMap)[EntityCommand_AttackReady] = EVCS_OnFight;		/// 攻击准备 (const AttackContext* context, 0)
	(*pMap)[EntityCommand_Attack] = EVCS_OnFight;			/// 物理攻击 (const AttackContext* context, 0)
	(*pMap)[EntityCommand_Wound] = EVCS_OnFight;			/// 受击 (0, 0)
	(*pMap)[EntityCommand_Death] = EVCS_OnFight;			/// 死亡 (0, 0)
	(*pMap)[EntityCommand_Stun] = EVCS_OnFight;				/// 眩晕 (0, 0)	

	(*pMap)[EntityCommand_Sitdown] = EVCS_OnPeace;			// 坐下 (0, 0)
	(*pMap)[EntityCommand_Siting] = EVCS_OnPeace;			/// 坐 (0, 0)
	(*pMap)[EntityCommand_Collect] = EVCS_OnPeace;			/// 采集 (0, 0)
	(*pMap)[EntityCommand_Relive] = EVCS_OnPeace;		/// 实体复活 (0, 0)


	s_pMapCommandToState = pMap;
	s_bInitMapCommandToState = true;
}

//EVisualComponentState CVisualComponentStateMgr::MapCommandToState(EntityCommand cmd)
//{
//	static EVisualComponentState s_CommandToState[EntityCommand_Max] = 
//	{
//		//// 外观类
//		EVCS_Max,			//EntityCommand_SetPart,			/// 设置部件，换装 (ulong part, ulong resId)，如果resId为0表示仅仅取下部件
//		EVCS_Max,			//EntityCommand_SetPartScale,		/// 设置部件的缩放比例，只对模型有效 (ulong part, float scale)
//		EVCS_Max,			//EntityCommand_SetPartColor,		/// 设置部件的颜色 (ulong part, const ColorValue* color)
//
//		//
//		EVCS_Max,			//EntityCommand_ShowPart,	////added by xxh 20090923, 用于控制是否显示装备
//
//		EVCS_Max,			//EntityCommand_PartDeformation,//装备变形命令
//		EVCS_Max,			//EntityCommand_PartAttachEffect,//装备附加特效
//
//		EVCS_Max,			//EntityCommand_AttachMagic,		/// 在实体(主要指生物)上吸附一个光效 (const MagicView* magic, 0)
//		////  增加一个与实体相关的光效，注意：该类光效并没有吸附在实体上；
//		EVCS_Max,			//EntityCommand_AddMagic,
//		EVCS_Max,			//EntityCommand_DetachMagic,		/// 取消实体(主要指生物)身上指定的光效 (ulong id, int loops)
//		////  移除一个与实体相关的光效，注意：该类光效并没有吸附在实体上；
//		EVCS_Max,			//EntityCommand_RemoveMagic,
//		EVCS_Max,			//EntityCommand_ClearMagic,		/// 清除实体身上的所有光效(一般发生在实体死亡时) (0, 0)
//		EVCS_Max,			//EntityCommand_AddModifier,		/// 添加修改器 (const Modifier* modifier, 0)
//		EVCS_Max,			//EntityCommand_RemoveModifiers,	/// 移除修改器 (ulong key, 0)
//
//		//// 动作类
//		EVCS_Max,			// EntityCommand_ForceStop		/// add by zjp强制停止站立，停止引导动作
//		EVCS_Max,			//EntityCommand_Stand,			/// 站立 (const POINT* tile=0, 0)
//		EVCS_Max,			//EntityCommand_Fallow,			/// 休闲 (ulong fallowId, 0)
//		EVCS_Max,			//EntityCommand_Move,				/// 移动，不关心是行走还是奔跑 (const POINT* pathList, size_t count)
//		EVCS_OnFight,			//EntityCommand_RapidMoveStart,	/// 快速移动(瞬移、冲锋)开始 (ShadowManager* mgr, 0)
//		EVCS_OnFight,			//EntityCommand_RapidMoveEnd,		/// 快速移动(瞬移、冲锋)结束
//		EVCS_OnFight,			//EntityCommand_AttackReady,		/// 攻击准备 (const AttackContext* context, 0)
//		EVCS_OnFight,			//EntityCommand_Attack,			/// 物理攻击 (const AttackContext* context, 0)
//
//		EVCS_OnFight,			//EntityCommand_Wound,			/// 受击 (0, 0)
//		EVCS_OnFight,			//EntityCommand_Death,			/// 死亡 (0, 0)
//		EVCS_OnFight,			//EntityCommand_Stun,				/// 眩晕 (0, 0)
//
//		EVCS_OnPeace,			//EntityCommand_Sitdown,			/// 坐下 (0, 0)
//		EVCS_OnPeace,           //EntityCommand_Siting              /// 坐 (0, 0)
//
//		EVCS_Max,           //EntityCommand_GetSitState         /// 获取坐的状态
//		EVCS_OnPeace,           //EntityCommand_Collect             /// 采集 (0, 0)
//		EVCS_OnPeace,           //EntityCommand_ShowRibbon          /// 是否显示武器飘带
//		EVCS_Max,			//EntityCommand_SetMount,			/// 设置骑（下）马 (ulong mount, ulong actionIndex) mount为1即上马，为0即下马,actionIndex为使用第几套动作，以0为基数
//		EVCS_Max,			//EntityCommand_GetMount,			/// 获取是否是骑马姿态 (ulong* mount, 0) mount为1即在马上，为0即没有骑马
//		EVCS_Max,			//EntityCommand_SetDefaultSpeed,  /// 设置默认速度（跑步，走路）
//		EVCS_Max,			//EntityCommand_SetMoveStyle,		/// 设置移动类型 (ulong style, 0) style为0即行走，为1即奔跑
//		EVCS_Max,			//EntityCommand_GetMoveStyle,		/// 获取移动类型 (ulong* style, 0) style为0即行走，为1即奔跑
//		EVCS_OnPeace,		//EntityCommand_Relive,			/// 实体复活 (0, 0)
//		EVCS_Max,			//EntityCommand_IsMoving,			/// 实体是否正在移动 (bool* isMoving, 0)
//		EVCS_Max,			//EntityCommand_ChangeModel,		/// 换模型
//
//		//// 主模型控制
//		EVCS_Max,			//EntityCommand_SetScale,			/// 设置主模型的缩放比例(默认为1) (const float* scale, 0) 第二个参数为0表示不相对当前缩放比进行缩放，为1表示相对当前缩放比进行缩放
//		EVCS_Max,			//EntityCommand_GetScale,			/// 获取主模型的缩放比例 (float* scale, 0)
//		EVCS_Max,			//EntityCommand_SetMoveSpeed,		/// 设置主模型的移动速度 (ulong tileMoveTime, 0)，这里的移动速度是移动档次对应的两个Tile中心点的移动时间（毫秒）
//		EVCS_Max,			//EntityCommand_GetMoveSpeed,		/// 获取主模型的移动速度 (ulong* tileMoveTime, 0)
//		//EVCS_Max,			////EntityCommand_SetAniSpeed,		/// 设置主模型动画的速度(默认为1) (const float* speed, 0)
//		//EVCS_Max,			////EntityCommand_GetAniSpeed,		/// 获取主模型动画的速度 (float* speed, 0)
//		EVCS_Max,			//EntityCommand_SetColor,			/// 设置主模型的颜色 (const ColorValue* color, 0)
//		EVCS_Max,			//EntityCommand_GetColor,			/// 获取主模型的颜色 (ColorValue* color, 0)
//
//		//// Topmost类
//		EVCS_Max,			//EntityCommand_SetName,			/// 设置名称 (const char* name, size_t len)
//		EVCS_Max,			//EntityCommand_GetName,			/// 获取名字 (std::string* name, 0)
//		EVCS_Max,			//EntityCommand_SetNameColor,		/// 设置名称的颜色 (const ColorValue* color, 0)
//		EVCS_Max,			//EntityCommand_GetNameColor,		/// 获取名称的颜色 (ColorValue* color, 0)
//		EVCS_Max,			//EntityCommand_SetHPInfo,		/// 设置HP信息 (ulong cur, ulong max)
//		EVCS_Max,			//EntityCommand_SetMPInfo,		/// 设置MP信息 (ulong cur, ulong max)
//		EVCS_Max,			//EntityCommand_ChangeLayout,		/// 改变topmost信息中的显示和不显示的标识(ulong hideFlags, ulong showFlags)，参考TopmostFlags枚举
//		// add by zjp；
//		EVCS_Max,			//EntityCommand_SetTitle,			/// 设置称号
//		EVCS_Max,			//EntityCommand_AddGroupText,		/// 添加文字显示组
//		EVCS_Max,			//EntityCommand_ClearGroupText,	/// 清空文字显示组
//
//		////  设置任务标记；
//		EVCS_Max,			//EntityCommand_SetTaskSign,
//
//		//// 光效类
//		EVCS_Max,			//EntityCommand_MoveRadial,		/// 射线移动，用于光效 (const POINT* ptBegin, const POINT* ptEnd)
//		EVCS_Max,			//EntityCommand_MoveTrack,		/// 移动并跟踪，用于光效，如火球 (IEntity* source, IEntity* target)
//		EVCS_Max,			//EntityCommand_Jump,				/// 跳跃
//		EVCS_Max,			//EntityCommand_NextMovingTile,
//	};
//
//	return s_CommandToState[cmd];
//}

void CVisualComponentStateMgr::OnCommand(EntityCommand cmd)
{
	if( !s_bInitMapCommandToState)
	{
		initCommandToStateMap();
	}
	CommandToStateMap::iterator it = (*s_pMapCommandToState).find(cmd);
	if( it == (*s_pMapCommandToState).end() )
	{
		if (m_nCurCommand==EntityCommand_AttackReady && cmd==EntityCommand_Stand)
		{
			m_nCurCommand = EntityCommand_Stand;
		}
		return;	 
	}
	EVisualComponentState newstate = it->second;

	m_nCurCommand = cmd;
	switch( newstate)
	{
	case EVCS_OnFight:
		{
			switch(m_CurState)
			{
			case EVCS_OnFight:
				{
					gGlobalClient->getTimerAxis()->KillTimer(0,this);
					gGlobalClient->getTimerAxis()->SetTimer(0, s_fight2PeaceInterval,this,1);		
				}
				break;
			case EVCS_OnPeace:
				{
					gGlobalClient->getTimerAxis()->SetTimer(0, s_fight2PeaceInterval,this,1);
					m_CurState = EVCS_OnFight;				
				}
				break;
			default:
				break;			
			}
		}
		break;
	case EVCS_OnPeace:
		{
			switch(m_CurState)
			{
			case EVCS_OnPeace:
				{
					//donothing
				}
				break;
			case EVCS_OnFight:
				{
					gGlobalClient->getTimerAxis()->KillTimer(0,this);
					m_CurState = EVCS_OnPeace;			
				}
				break;
			default:
				break;
				
			}
		
		}
		break;
	default:
		break;
	}

	return;
}


