#include "StdAfx.h"
#include "ChangePartManager.h"
#include "VisualComponentMZ.h"
#include "MWDLoader.h"
#include "ConfigActionMap.h"


ChangePartManager::ChangePartManager():m_pOwner(0),
m_bCanRequestRes(false),
m_ulCurResId(0),
m_ulNewResId(0),
m_handle(INVALID_HANDLE),
m_bLoaded(false),
m_pModelNode(0),
m_bShowRibbonSystem(false),//默认选择不显示
m_weaponBindPos(WeaponPos_Hands)
{
	//初始化换装部件
	for( uchar i=0; i< EEntityPart_Max; ++i)
	{
		m_CurrentPart[i].perform = false;
		m_CurrentPart[i].part = i;
		for( uchar j =0; j<EBindResNum_MainPart; ++j)
		{
			m_CurrentPart[i].resId[j] = 0;
			m_CurrentPart[i].bindType[j] = EBindType_Min;
			m_CurrentPart[i].bindPoint[j] = EBindPoint_Min;
		}
		m_CurrentPart[i].weaponclass = EWeapon_FirstClass;
	}
	//普通盔甲的特殊情况
	for( uchar i =0; i<EBindResNum_MainPart; ++i)
		m_CurrentPart[EEntityPart_Armor].bindType[i] = EBindType_Replace;
	//时装盔甲特殊情况
	for( uchar i =0; i<EBindResNum_MainPart; ++i)
		m_CurrentPart[EEntityPart_Suit_Armor].bindType[i] = EBindType_Replace;
	//普通头盔特殊情况
	for( uchar i =0; i<EBindResNum_MainPart; ++i)
	{
		m_CurrentPart[EEntityPart_Armet].bindType[i] = EBindType_SubNode;
		m_CurrentPart[EEntityPart_Armet].bindPoint[i] = EBindPoint_Head;
	}
	//时装头盔特殊情况
	for( uchar i =0; i<EBindResNum_MainPart; ++i)
	{
		m_CurrentPart[EEntityPart_Suit_Armet].bindType[i] = EBindType_SubNode;
		m_CurrentPart[EEntityPart_Suit_Armet].bindPoint[i] = EBindPoint_Head;
	}
	//武器特殊情况
	m_CurrentPart[EEntityPart_Weapon].bindType[0] = EBindType_SubNode;
	m_CurrentPart[EEntityPart_Weapon].bindPoint[0] = EBindPoint_LeftHand;
	m_CurrentPart[EEntityPart_Weapon].bindType[1] = EBindType_SubNode;
	m_CurrentPart[EEntityPart_Weapon].bindPoint[1] = EBindPoint_RightHand;


	//初始化装备显示
	for( uint i=0; i<EEntityPart_Max; ++i)
	{
		m_bShowPart[i] = true;
	}
	//默认不显示头盔，时装头盔，时装衣服
	m_bShowPart[EEntityPart_Armet] = false;
	m_bShowPart[EEntityPart_Armor] = true;
	m_bShowPart[EEntityPart_Suit_Armet] = false;
	m_bShowPart[EEntityPart_Suit_Armor] = false;


	//武器的缩放
	m_fWeaponScale = 1.0f;
	
	//初始化颜色
	m_vWeaponColor = xs::ColorValue::White;
	m_vArmetColor = xs::ColorValue::White;
	m_vSuitArmetColor = xs::ColorValue::White;	
	m_vBodyColor = xs::ColorValue::White;
}


void ChangePartManager::create(VisualComponentMZ* pOwner, IChangePartCallBack* pCallBack)
{
	if( 0 == pOwner)
		throw std::string("ChangePartManager::create:pOwner=0");
	if( 0 == pCallBack)
		throw std::string("ChangePartManager::create:pCallBack=0");
	m_pCallBack = pCallBack;
	m_pOwner = pOwner;
}

ChangePartManager::~ChangePartManager()
{
	releaeRes();
}

void ChangePartManager::setCanRequestRes(bool canRequest)
{
	m_bCanRequestRes = canRequest;
}

void ChangePartManager::requestRes(int priority)
{
	//能否申请资源
	if( !m_bCanRequestRes) 
		return;

	//释放旧的资源
	releaeRes();

	//申请新的资源
	ulong curBody = getCurrentBody();
	if(m_CurrentPart[curBody].perform && m_CurrentPart[curBody].resId[0] )//有初始值
	{
		m_ulCurResId = m_CurrentPart[curBody].resId[0];
		//判断是当做生物id还是资源id
		bool isCreature = false;
		if(  ConfigCreatures::Instance()->getCreature(m_ulCurResId) )
			isCreature = true;

		m_ulNewResId = MZIDMap::getInstance().add(m_ulCurResId, isCreature?0:1);//0:生物id，1:光效id
		m_handle = gGlobalClient->getResourceManager()->requestResource(m_ulNewResId, typeResourceModel, false, priority);
		m_bLoaded = false;
		m_pModelNode = 0;
	}
	else//默认值
	{
		m_ulCurResId = m_pOwner->getOwner()->getResId();
		m_ulNewResId = MZIDMap::getInstance().add(m_ulCurResId,0);//当做生物
		m_handle = gGlobalClient->getResourceManager()->requestResource(m_ulNewResId, typeResourceModel, false, priority);
		m_bLoaded = false;
		m_pModelNode = 0;
	}
}

void ChangePartManager::releaeRes()
{
	//取消外部链接
	ChangePartCallBackContext context;
	context.pNode = 0;
	m_pCallBack->onChangePart(context);


	//释放节点前，先将子节点释放
	if( m_pModelNode)
	{
		m_pModelNode->destroyAllChildren();
		m_pModelNode = 0;
	}

	//释放资源
	if( isValidHandle(m_handle) )
	{
		// hero的资源延迟回收
		bool bDelayRecycle = false;
		if (NULL != m_pOwner)
		{
			EntityViewImpl* pOwner = m_pOwner->getOwner();
			if (NULL != pOwner)
			{
				bDelayRecycle = pOwner->isMainPlayer();
			}
		}

		gGlobalClient->getResourceManager()->releaseResource(m_handle, bDelayRecycle);
		m_handle = INVALID_HANDLE;
	}

	//移除映射的资源id
	MZIDMap::getInstance().remove(m_ulNewResId);
	m_ulNewResId = 0;

	m_ulCurResId = 0;
	m_bLoaded = false;
}

ulong ChangePartManager::getCurrentBody()
{
	if( m_bShowPart[EEntityPart_Armor] )
		return EEntityPart_Armor;
	else if( m_bShowPart[EEntityPart_Suit_Armor] )
		return EEntityPart_Suit_Armor;
	else
	{
		throw std::string("ChangePartManager::getCurrentBody:no body");
	}
}



void ChangePartManager::changeArmet(SPersonMainPartChangeContext & context)
{
	ulong curBody = getCurrentBody();
	SPersonMainPartChangeContext & curContext = m_CurrentPart[EEntityPart_Armet];

	if( curBody != EEntityPart_Armor)//当前显示的不是普通装备的模型，只存储数据
	{
		curContext = context;
		return;
	}

	if( !m_bCanRequestRes)//不能申请资源了
	{
		curContext = context;
		return;
	}

	if(0 == m_pModelNode )//当前显示的节点还没有加载上来，只存储数据
	{
		curContext = context;
		return;
	}

	//首先卸载旧的模型
	if( !gGlobalClient->getEntityClient() )
	{
		return;
	}
	const char * pOldBP = gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint(curContext.bindPoint[0] );
	ModelNode * pOld = m_pModelNode->getFirstChildNodeByBone(pOldBP);
	if( pOld )  m_pModelNode->destroyChild( pOld);

	if( context.perform)//穿上装备
	{
		
		ModelNode* pNode = 0;
		ConfigCreature* pCreatureConfig = ConfigCreatures::Instance()->getCreature(context.resId[0]);
		if( pCreatureConfig )//首先从生物配置读取
		{
			pNode = ModelNodeCreater::create(pCreatureConfig);
			if( pNode) 
				pNode->setScale(xs::Vector3::UNIT_SCALE);
		}
		else//如果找不到就从资源配置中创建
		{
			const std::string & filename = ConfigCreatureRes::Instance()->getResFromId(context.resId[0]);
			pNode = ModelNodeCreater::create(filename.c_str() );
		}
			
		if( pNode )
		{
			const char * pBP = gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint(context.bindPoint[0]);
			if( !m_pModelNode->addChild(pNode, pBP) )
			{
				pNode->release();
				pNode = 0;
			}
			else
			{
				//设置头盔颜色
				pNode->setGlobalDiffuse(m_vArmetColor);
			}
			
		}
	}
	else
	{
		//已经卸载了
	}

	//保存换装环境
	curContext = context;
	//设置头盔的显示
	showPart(EEntityPart_Armet, m_bShowPart[EEntityPart_Armet]);

	return;
}

void ChangePartManager::changeSuitArmet(SPersonMainPartChangeContext & context)
{
	ulong curBody = getCurrentBody();
	SPersonMainPartChangeContext & curContext = m_CurrentPart[EEntityPart_Suit_Armet];

	if( curBody != EEntityPart_Suit_Armor)//当前显示的不是时装装备的模型，只存储数据
	{
		curContext = context;
		return;
	}

	if( !m_bCanRequestRes)//不能申请资源了
	{
		curContext = context;
		return;
	}

	if(0 == m_pModelNode )//当前显示的节点还没有加载上来，只存储数据
	{
		curContext = context;
		return;
	}

	//首先卸载旧的模型
	const char * pOldBP = gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint(curContext.bindPoint[0] );
	ModelNode * pOld = m_pModelNode->getFirstChildNodeByBone(pOldBP);
	if( pOld )  m_pModelNode->destroyChild( pOld);

	if( context.perform)//穿上装备
	{
		ModelNode * pNode = 0;
		ConfigCreature* pCreatureConfig = ConfigCreatures::Instance()->getCreature(context.resId[0]);
		if( pCreatureConfig )
		{
			pNode = ModelNodeCreater::create(pCreatureConfig);
			if( pNode)
				pNode->setScale(xs::Vector3::UNIT_SCALE);
		}
		else
		{
			const std::string & filename = ConfigCreatureRes::Instance()->getResFromId(context.resId[0]);
			pNode = ModelNodeCreater::create(filename.c_str() );
		}
			
		if( pNode )
		{
			const char * pBP = gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint(context.bindPoint[0]);
			if( !m_pModelNode->addChild(pNode, pBP) )
			{
				pNode->release();
				pNode = 0;
			}
			else
			{
				//设置时装头盔的颜色
				pNode->setGlobalDiffuse(m_vSuitArmetColor);
			}
			
		}
	}
	else
	{
		//已经卸载了
	}

	//保存换装环境
	curContext = context;
	//设置头盔的显示
	showPart(EEntityPart_Suit_Armet, m_bShowPart[EEntityPart_Suit_Armet]);

	return;
}


void ChangePartManager::changeArmor(SPersonMainPartChangeContext & context)
{
	ulong curBody = getCurrentBody();
	SPersonMainPartChangeContext & curContext = m_CurrentPart[EEntityPart_Armor];

	if( curBody != EEntityPart_Armor)//当前显示的不是普通装备的模型，只存储数据
	{
		curContext = context;
		return;
	}

	//释放旧的资源
	releaeRes();

	//保存环境
	curContext = context;

	//申请新的资源
	requestRes(0);

	return;
}

void ChangePartManager::changeSuitArmor(SPersonMainPartChangeContext& context)
{
	ulong curBody = getCurrentBody();
	SPersonMainPartChangeContext & curContext = m_CurrentPart[EEntityPart_Suit_Armor];

	if( curBody != EEntityPart_Suit_Armor)//当前显示的不是普通装备的模型，只存储数据
	{
		curContext = context;
		return;
	}

	//释放旧的资源
	releaeRes();

	//保存环境
	curContext = context;

	//申请新的资源
	requestRes(0);

	return;
}

void ChangePartManager::changeWeapon(SPersonMainPartChangeContext & context)
{
	SPersonMainPartChangeContext & curContext = m_CurrentPart[EEntityPart_Weapon];

	if( !m_bCanRequestRes)//不能申请资源了
	{
		curContext = context;
		return;
	}

	if(0 == m_pModelNode )//当前显示的节点还没有加载上来，只存储数据
	{
		curContext = context;
		return;
	}

	if(!gGlobalClient->getEntityClient())
		return;

	//首先卸载旧的模型
	for( uint i = 0; i < EBindResNum_MainPart; ++i)
	{
		const char * pBP = gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint(curContext.bindPoint[i] );
		ModelNode * pNode = m_pModelNode->getFirstChildNodeByBone(pBP);
		if( pNode ) m_pModelNode->destroyChild(pNode);	
	}

	//保存换装环境
	curContext = context;

	//加载或者卸载装备
	if( curContext.perform)//穿上装备
	{
		ModelNode * pNodeArr[EBindResNum_MainPart];
		memset( pNodeArr,0, sizeof(pNodeArr) );
		for( uint i =0; i < EBindResNum_MainPart; ++i)
		{
			ConfigCreature* pCreatureConfig = ConfigCreatures::Instance()->getCreature(curContext.resId[i]);
			if( pCreatureConfig)//首先从生物配置创建
			{
				pNodeArr[i] = ModelNodeCreater::create(pCreatureConfig);
				if( pNodeArr[i])
					pNodeArr[i]->setScale(xs::Vector3::UNIT_SCALE);
			}
			else//如果没有，就从资源配置创建
			{
				const std::string & filename = ConfigCreatureRes::Instance()->getResFromId(curContext.resId[i]);
				pNodeArr[i] = ModelNodeCreater::create(filename.c_str() );	
			}		
		}
		for( uint i=0; i<EBindResNum_MainPart; ++i)
		{
			if( pNodeArr[i] == 0 ) continue;
			const char * pBP = gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint(curContext.bindPoint[i]);
			bool ret = m_pModelNode->addChild(pNodeArr[i], pBP);
			if( !ret ) 
			{
				safeRelease(pNodeArr[i]);
			}
			else
			{
				//调整武器的位置
				setWeaponPos(m_weaponBindPos);

				//调整武器的大小
				pNodeArr[i]->setScale(m_fWeaponScale, m_fWeaponScale, m_fWeaponScale);

				//调整武器颜色
				pNodeArr[i]->setGlobalDiffuse(m_vWeaponColor);

			}
		}

		//显示飘带系统
		showRibbonSystem(m_bShowRibbonSystem);
	}
	else
	{
		//已经卸载了
	}

	return;
}

void ChangePartManager::onResLoad(ModelNode* pNode)
{
	if( 0 == pNode) //不可能
	{
		throw std::string("ChangePartManager::onResLoad:: empty ModelNode");
		return;
	}

	m_bLoaded = true;
	m_pModelNode = pNode;

	//设置身体颜色
	m_pModelNode->setGlobalDiffuse(m_vBodyColor);

	//执行换装命令
	ulong curBody = getCurrentBody();
	if( curBody == EEntityPart_Armor )//普通装备
	{
		changePart(m_CurrentPart[EEntityPart_Armet]);
		changePart(m_CurrentPart[EEntityPart_Weapon]);
	}
	else if( curBody == EEntityPart_Suit_Armor)
	{
		changePart(m_CurrentPart[EEntityPart_Suit_Armet]);
		changePart(m_CurrentPart[EEntityPart_Weapon]);
	}
	else
	{
		//no thing to do
	}

	//链接外部
	ChangePartCallBackContext context;
	context.pNode = m_pModelNode;
	m_pCallBack->onChangePart(context);
}

void ChangePartManager::update()
{
	if( isValidHandle(m_handle) && !m_bLoaded ) //句柄有效，且没有加载
	{
		IResourceNode* node = (IResourceNode*)getHandleData(m_handle);
		if( node)
		{
			IResource * res = node->getResource();
			if( res)
			{
				ModelNode * n = (ModelNode *)res->getInnerData();
				if( n )
				{
					onResLoad(n);
				}
			}
		}
	}
}

EWeaponClass ChangePartManager::getWeaponClass()
{
	if( m_CurrentPart[EEntityPart_Weapon].perform )
	{
		if( m_CurrentPart[EEntityPart_Weapon].weaponclass >= EWeapon_Max)
			throw std::string("ChangePartManager::getWeaponClass:bad weapon class");
		return static_cast<EWeaponClass>( m_CurrentPart[EEntityPart_Weapon].weaponclass);
	}
	else
	{
		return EWeapon_NoWeapon;
	}
}

void ChangePartManager::setWeaponAnimation(std::string ani)
{
	if( !gGlobalClient->getEntityClient() )
		return;
	if( m_pModelNode)
	{
		const SPersonMainPartChangeContext &  context = m_CurrentPart[EEntityPart_Weapon];
		for(uint i=0; i<2; ++i)
		{
			ModelNode* pNode= m_pModelNode->getFirstChildNodeByBone(gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint(context.bindPoint[i]));
			if( pNode)
			{
				pNode->getModelInstance()->setCurrentAnimation(ani);
			}
		}
	}
}

bool ChangePartManager::getPart(uint part, SEventPersonPerform_C& c)
{
	if( part <= EEntityPart_Invalid || part >=EEntityPart_Max)
		return false;

	c.perform = m_CurrentPart[part].perform;
	c.part = m_CurrentPart[part].part;
	c.resId[0]= m_CurrentPart[part].resId[0];
	c.resId[1] = m_CurrentPart[part].resId[1];
	if( part != EEntityPart_Weapon)
	{
		sprintf_s(c.bindPoint[0], sizeof(c.bindPoint[0]),"%s",gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint(m_CurrentPart[part].bindPoint[0]) );
		sprintf_s(c.bindPoint[1], sizeof(c.bindPoint[1]),"%s",gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint(m_CurrentPart[part].bindPoint[1]) );
	}	
	else
	{
		int bind1 = m_CurrentPart[part].bindPoint[0];
		int bind2 = m_CurrentPart[part].bindPoint[1];
		sprintf_s(c.bindPoint[0], sizeof(c.bindPoint[0]), "%s", gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint(bind1));
		sprintf_s(c.bindPoint[1], sizeof(c.bindPoint[1]), "%s", gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint(bind2));
		//sprintf_s(c.bindPoint[0], sizeof(c.bindPoint[0]), "%s", gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint(EBindPoint_LeftBack));
		//sprintf_s(c.bindPoint[1], sizeof(c.bindPoint[1]), "%s", gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint(EBindPoint_RightBack));
		ActionMapContext actioncontext;
		actioncontext.iscreature = true;
		actioncontext.ismount = false;
		actioncontext.state = EVCS_OnPeace;
		actioncontext.weapon = getWeaponClass();
		sprintf_s(c.animation, sizeof(c.animation),"%s", ConfigActionMap::Instance()->getMappedAction(actioncontext, EntityAction_Stand) );	
		c.weaponsubclass = m_CurrentPart[part].weaponsubclass;
	}

	return true;
}

bool ChangePartManager::getPartShow(uint part, SEventPersonPerformShow_C& c)
{
	if( part <= EEntityPart_Invalid || part >=EEntityPart_Max)
		return false;

	c.part = static_cast<EntityParts>(part);
	c.show = m_bShowPart[part];
	return true;

}

int ChangePartManager::getCurrentCreatureId()
{
	ulong curBody = getCurrentBody();
	if( !m_CurrentPart[curBody].perform 
		|| 0 == m_CurrentPart[curBody].resId[0]
		|| ConfigCreatures::Instance()->getCreature( m_CurrentPart[curBody].resId[0]) )
	{
		return m_pOwner->getOwner()->getResId();
	}
	else
	{
		return m_CurrentPart[curBody].resId[0];
	}
}

void ChangePartManager::setWeaponPos(WeaponPosition pos)
{
	SPersonMainPartChangeContext &  context = m_CurrentPart[EEntityPart_Weapon];
	IEntityClient* pEntityClient = gGlobalClient->getEntityClient();
	if (NULL == pEntityClient)
	{
		return;
	}
	ISchemeCenter* pSchemeCenter = pEntityClient->GetSchemeCenter();
	if (!pSchemeCenter)
	{
		return;
	}
	m_weaponBindPos = pos;
	if( !context.perform)//没有装备武器
	{
		if( m_weaponBindPos == WeaponPos_Back)
		{
			//对于卷轴类武器，特殊处理
			if( context.weaponsubclass == EQUIPMENT_SUBCLASS_SORCERER_WEAPON_1_CLASS)
			{
				for( uint i=0; i<2;++i)
				{
					if( context.bindPoint[i] == EBindPoint_LeftHand)
						context.bindPoint[i] = EBindPoint_RightBack ;
					else if( context.bindPoint[i] == EBindPoint_RightHand)
						context.bindPoint[i] = EBindPoint_LeftBack;
				}
			}
			else
			{
				for( uint i=0; i<2;++i)
				{
					if( context.bindPoint[i] == EBindPoint_LeftHand)
						context.bindPoint[i] = EBindPoint_LeftBack;
					else if( context.bindPoint[i] == EBindPoint_RightHand)
						context.bindPoint[i] = EBindPoint_RightBack;
				}
			}

		}
		else
		{
			//对于卷轴类武器，特殊处理
			if( context.weaponsubclass == EQUIPMENT_SUBCLASS_SORCERER_WEAPON_1_CLASS)
			{
				for( uint i =0; i<2; ++i)
				{
					if( context.bindPoint[i] == EBindPoint_LeftBack)
						context.bindPoint[i] = EBindPoint_RightHand ;
					else if( context.bindPoint[i] == EBindPoint_RightBack)
						context.bindPoint[i] = EBindPoint_LeftHand;
				}
			}
			else
			{
				for( uint i =0; i<2; ++i)
				{
					if( context.bindPoint[i] == EBindPoint_LeftBack)
						context.bindPoint[i] = EBindPoint_LeftHand;
					else if( context.bindPoint[i] == EBindPoint_RightBack)
						context.bindPoint[i] = EBindPoint_RightHand;
				}
			}		
		}
		return;
	}
	else
	{
		ModelNode* pNode[2];
		memset( &pNode[0], 0, sizeof(pNode));
		if(m_pModelNode)//移除
		{		
			for( uint i=0; i<2;++i)
			{
				const char* pchar = pSchemeCenter->getBindPoint(context.bindPoint[i]);
				pNode[i] = m_pModelNode->getFirstChildNodeByBone(pchar);
				if( pNode[i])
				{
					m_pModelNode->removeChild(pNode[i]);
					pNode[i]->setPosition(xs::Vector3::ZERO);
					pNode[i]->setOrientation(xs::Quaternion::IDENTITY);
				}
			}
		}

		//绑定
		if( m_weaponBindPos == WeaponPos_Back)
		{
			for( uint i=0; i<2;++i)
			{
				if( context.bindPoint[i] == EBindPoint_LeftHand)
					context.bindPoint[i] = EBindPoint_RightBack;
				else if( context.bindPoint[i] == EBindPoint_RightHand)
					context.bindPoint[i] = EBindPoint_LeftBack;
			}

			////对于卷轴类武器，特殊处理
			//if( context.weaponsubclass == EQUIPMENT_SUBCLASS_SORCERER_WEAPON_1_CLASS)
			//{
			//	for( uint i=0; i<2;++i)
			//	{
			//		if( context.bindPoint[i] == EBindPoint_LeftHand)
			//			context.bindPoint[i] = EBindPoint_RightBack ;
			//		else if( context.bindPoint[i] == EBindPoint_RightHand)
			//			context.bindPoint[i] = EBindPoint_LeftBack;
			//	}
			//}
			//else
			//{
			//	for( uint i=0; i<2;++i)
			//	{
			//		if( context.bindPoint[i] == EBindPoint_LeftHand)
			//			context.bindPoint[i] = EBindPoint_LeftBack;
			//		else if( context.bindPoint[i] == EBindPoint_RightHand)
			//			context.bindPoint[i] = EBindPoint_RightBack;
			//	}
			//}		
		}
		else
		{
			for( uint i =0; i<2; ++i)
			{
				if( context.bindPoint[i] == EBindPoint_LeftBack)
					context.bindPoint[i] = EBindPoint_RightHand;
				else if( context.bindPoint[i] == EBindPoint_RightBack)
					context.bindPoint[i] = EBindPoint_LeftHand;
			}
			//对于卷轴类武器，特殊处理
			//if( context.weaponsubclass == EQUIPMENT_SUBCLASS_SORCERER_WEAPON_1_CLASS)
			//{
			//	for( uint i =0; i<2; ++i)
			//	{
			//		if( context.bindPoint[i] == EBindPoint_LeftBack)
			//			context.bindPoint[i] = EBindPoint_RightHand;
			//		else if( context.bindPoint[i] == EBindPoint_RightBack)
			//			context.bindPoint[i] =  EBindPoint_LeftHand;
			//	}
			//}
			//else
			//{
			//	for( uint i =0; i<2; ++i)
			//	{
			//		if( context.bindPoint[i] == EBindPoint_LeftBack)
			//			context.bindPoint[i] = EBindPoint_LeftHand;
			//		else if( context.bindPoint[i] == EBindPoint_RightBack)
			//			context.bindPoint[i] = EBindPoint_RightHand;
			//	}
			//}	
		}

		if(m_pModelNode)
		{
			for(uint i=0; i<2;++i)
			{
				if( !pNode[i]) continue;
				bool ret = m_pModelNode->addChild(pNode[i], gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint(context.bindPoint[i]));
				if(!ret)
				{
					safeRelease(pNode[i]);
				}
				else
				{
					if( m_weaponBindPos == WeaponPos_Back)
					{
						//设置武器朝向
						if(context.weaponsubclass == EQUIPMENT_SUBCLASS_WARRIOR_WEAPON_2_CLASS)
						{
							xs::Quaternion rotx;
							rotx.FromAngleAxis(180.0f, Vector3(1.0f, 0.0f, 0.0f));
							xs::Quaternion roty;
							roty.FromAngleAxis(180.0f, Vector3(0.0f,1.0f,0.0f));
							xs::Quaternion rot = roty * rotx;
							pNode[i]->setOrientation(rot);
							xs::Vector3 origin = xs::Vector3::ZERO;
							origin.y += pNode[i]->getModelInstance()->getBoundingSphere().getCenter().y;
							pNode[i]->setPosition(origin);
						}
						else if (context.weaponsubclass == EQUIPMENT_SUBCLASS_ARCHER_WEAPON_1_CLASS)
						{
							// 修改弓手武器的朝向，现在的弓手武器是按弓口朝Z轴方向，以前默认是朝X轴方向
							xs::Quaternion roty;
							roty.FromAngleAxis(90.0f, Vector3(0.0f,1.0f,0.0f));
							pNode[i]->setOrientation(roty);

							xs::Vector3 origin = xs::Vector3::ZERO;
							origin.y -= pNode[i]->getModelInstance()->getBoundingSphere().getCenter().y;
							pNode[i]->setPosition(origin);
						}
						else
						{
							xs::Vector3 origin = xs::Vector3::ZERO;
							origin.y -= pNode[i]->getModelInstance()->getBoundingSphere().getCenter().y;
							pNode[i]->setPosition(origin);
						}
						
					}
					else
					{
						// 修改弓手武器的朝向，现在的弓手武器是按弓口朝Z轴方向，以前默认是朝X轴方向
						if (context.weaponsubclass == EQUIPMENT_SUBCLASS_ARCHER_WEAPON_1_CLASS)
						{						
							xs::Quaternion roty;
							roty.FromAngleAxis(90.0f, Vector3(0.0f,1.0f,0.0f));
							pNode[i]->setOrientation(roty);
						}
						else
						{
							pNode[i]->setOrientation(xs::Quaternion::IDENTITY);
						}
						pNode[i]->setPosition(xs::Vector3::ZERO);
					}
				}
			}	
		}
	}
}

void ChangePartManager::showRibbonSystem(bool show)
{
	m_bShowRibbonSystem = show;
	if( !m_pModelNode)
		return;

	const char* pBP[2];
	pBP[0] = gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint( m_CurrentPart[EEntityPart_Weapon].bindPoint[0]);
	pBP[1] = gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint(m_CurrentPart[EEntityPart_Weapon].bindPoint[1] );
	for( uint i=0; i<2; ++i)
	{
		ModelNode* pWeapon = m_pModelNode->getFirstChildNodeByBone(pBP[i]);
		if( pWeapon)
			pWeapon->getModelInstance()->showRibbonSystem(m_bShowRibbonSystem);
	}
}

void ChangePartManager::changePart(SPersonMainPartChangeContext & context)
{
	//人物的主节点时异步加载的，其它的是同步加载的。

	//验证是否有效
	if( context.part <= EEntityPart_Invalid && context.part >= EEntityPart_Max) 
		return;

	switch( context.part)
	{
	case EEntityPart_Armet:
		changeArmet( context);
		break;
	case EEntityPart_Suit_Armet:
		changeSuitArmet( context);
		break;
	case EEntityPart_Armor:
		changeArmor( context);
		break;
	case EEntityPart_Suit_Armor:
		changeSuitArmet(context);
		break;
	case EEntityPart_Weapon:
		changeWeapon(context);
		break;
	default:
		m_CurrentPart[context.part] = context;
		break;
	}

	return;
}

void ChangePartManager::showArmet(bool bShow)
{
	m_bShowPart[EEntityPart_Armet] = bShow;

	//检测
	if( 0 == m_pModelNode )//节点没有加载
		return;
	ulong curBody = getCurrentBody();
	if( curBody != EEntityPart_Armor)//不是普通装备
		return;

	const char * pBP = gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint( m_CurrentPart[EEntityPart_Armet].bindPoint[0] );
	ModelNode * pSubNode = m_pModelNode->getFirstChildNodeByBone(pBP);
	xs::SubModelInstance * pSubModel = m_pModelNode->getModelInstance()->getSubModel( getModelMeshName(MSFCP_HAIR) );

	if( bShow )
	{
		if( pSubNode )
		{
			pSubNode->setVisible(true);
			if( pSubModel)
				pSubModel->setVisible(false);
		}
		else
		{
			if( pSubModel)
				pSubModel->setVisible(true);
		}
	}
	else
	{
		if( pSubNode)
			pSubNode->setVisible(false);
		if( pSubModel)
			pSubModel->setVisible(true);
	}

	return;
}

void ChangePartManager::showSuitArmet(bool bShow)
{
	m_bShowPart[EEntityPart_Suit_Armet] = bShow;

	//检测
	if( 0 == m_pModelNode )//节点没有加载
		return;
	ulong curBody = getCurrentBody();
	if( curBody != EEntityPart_Suit_Armor)//不是时装装备
		return;

	

	const char * pBP = gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint( m_CurrentPart[EEntityPart_Suit_Armet].bindPoint[0] );
	ModelNode * pSubNode = m_pModelNode->getFirstChildNodeByBone(pBP);
	xs::SubModelInstance * pSubModel = m_pModelNode->getModelInstance()->getSubModel( getModelMeshName(MSFCP_HAIR) );

	if( bShow )
	{
		if( pSubNode )
		{
			pSubNode->setVisible(true);
			if( pSubModel)
				pSubModel->setVisible(false);
		}
		else
		{
			if( pSubModel)
				pSubModel->setVisible(true);
		}
	}
	else
	{
		if( pSubNode)
			pSubNode->setVisible(false);
		if( pSubModel)
			pSubModel->setVisible(true);
	}

	return;
}

void ChangePartManager::showBody( bool normal)
{
	//释放旧的资源
	releaeRes();

	//设定当前显示的节点
	if( normal )
	{
		m_bShowPart[EEntityPart_Armor] = true;
		m_bShowPart[EEntityPart_Suit_Armor] = false;
	}
	else
	{
		m_bShowPart[EEntityPart_Armor] = false;
		m_bShowPart[EEntityPart_Suit_Armor] = true;
	}

	//申请新的资源
	requestRes(0);	
}

void ChangePartManager::showWeapon( bool bShow)//变身时候需要屏蔽武器，by cjl
{
	m_bShowPart[EEntityPart_Weapon] = bShow;

	SPersonMainPartChangeContext & curContext = m_CurrentPart[EEntityPart_Weapon];
	curContext.perform = bShow;

}//end cjl


void ChangePartManager::showPart( ulong ulPart, bool bShow)
{
	//验证
	if (ulPart <= static_cast<ulong>(EEntityPart_Invalid) ||  ulPart >= static_cast<ulong>(EEntityPart_Max) )
		return;

	switch( ulPart)
	{
	case EEntityPart_Armet:
		showArmet(bShow);
		break;
	case EEntityPart_Armor:
		showBody(bShow);
		break;
	case EEntityPart_Suit_Armet:
		showSuitArmet(bShow);
		break;
	case EEntityPart_Suit_Armor:
		showBody(!bShow);
		break;
	case EEntityPart_Weapon://by cjl
		showWeapon(bShow);
		break;
	default:
		break;
	}

	return;
}

void ChangePartManager::setPartScale(ulong part, float scale)
{
	if( part <= EEntityPart_Invalid || part >= EEntityPart_Max )
		return;

	//只考虑武器的缩放
	if( part != EEntityPart_Weapon )
		return ;

	m_fWeaponScale = scale;

	if( m_pModelNode && m_CurrentPart[EEntityPart_Weapon].perform )
	{
		for( uint i=0; i<EBindResNum_MainPart; ++i)
		{
			const char * pBP = gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint(m_CurrentPart[EEntityPart_Weapon].bindPoint[i]);
			xs::ModelNode* pSubNode = m_pModelNode->getFirstChildNodeByBone(pBP);
			if( pSubNode )
				pSubNode->setScale(m_fWeaponScale,m_fWeaponScale,m_fWeaponScale);
		}
	}
}

void ChangePartManager::setPartColor(ulong	part, const ColorValue& color)
{
	//只考虑头盔和武器的颜色
	if( part <= EEntityPart_Invalid || part >= EEntityPart_Max)
		return;

	switch( part )
	{
	case EEntityPart_Weapon:
		{
			m_vWeaponColor = color;
			if( m_pModelNode && m_CurrentPart[EEntityPart_Weapon].perform )
			{
				for( uint i=0; i<EBindResNum_MainPart; ++i)
				{
					const char * pBP = gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint(m_CurrentPart[EEntityPart_Weapon].bindPoint[i]);
					xs::ModelNode* pSubNode = m_pModelNode->getFirstChildNodeByBone(pBP);
					if( pSubNode )
						pSubNode->setGlobalDiffuse(m_vWeaponColor);
				}
			}
		}
		break;
	case EEntityPart_Armet:
		{
			m_vArmetColor = color;
			ulong curBody = getCurrentBody();
			if( curBody == EEntityPart_Armor
				&& m_pModelNode
				&& m_CurrentPart[EEntityPart_Armet].perform )
			{
				const char* pBP = gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint( m_CurrentPart[EEntityPart_Armet].bindPoint[0]);
				xs::ModelNode* pArmet = m_pModelNode->getFirstChildNodeByBone(pBP);
				if( pArmet)
					pArmet->setGlobalDiffuse(m_vArmetColor);
			}
		}
		break;
	case EEntityPart_Suit_Armet:
		{
			m_vSuitArmetColor = color;
			ulong curBody = getCurrentBody();
			if( curBody == EEntityPart_Suit_Armor
				&& m_pModelNode
				&& m_CurrentPart[EEntityPart_Suit_Armet].perform )
			{
				const char* pBP = gGlobalClient->getEntityClient()->GetSchemeCenter()->getBindPoint( m_CurrentPart[EEntityPart_Suit_Armet].bindPoint[0]);
				xs::ModelNode* pSuitArmet = m_pModelNode->getFirstChildNodeByBone(pBP);
				if( pSuitArmet)
					pSuitArmet->setGlobalDiffuse(m_vSuitArmetColor);
			}	
		}
		break;
	}

}

void ChangePartManager::setBodyColor(const ColorValue & color)
{
	m_vBodyColor = color;
	if( m_pModelNode )
		m_pModelNode->setGlobalDiffuse(m_vBodyColor);
}

const ColorValue & ChangePartManager::getBodyColor()
{
	return m_vBodyColor;
}
