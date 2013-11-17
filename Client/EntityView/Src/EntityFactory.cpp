//==========================================================================
/**
* @file	  : EntityFactory.cpp
* @author : 
* created : 2008-1-21   17:39
*/
//==========================================================================

#include "stdafx.h"
#include "EntityFactory.h"

#include "EntityViewImpl.h"
#include "VisualComponentMPW.h"
#include "VisualComponentMZ.h"
#include "VisualComponentBox.h"
#include "MoveComponentMZ.h"
#include "MWDLoader.h"
#include "PartManager.h"
#include "PetManager.h"

#include "FlowText.h"

#include "MagicData.h"
#include "EffectData.h"
#include "SkillViewData.h"
#include "MagicView.h"
#include "EffectControl.h"
#include "Modifier.h"

#include "ResourceLoaderMpw.h"
#include "ResourceLoaderMz.h"

#include "ConfigCreaturePart.h"
#include "ConfigCreatureRes.h"
#include "ConfigCreatures.h"
#include "MwdInfoReader.h"
#include "ConfigActionMap.h"

#include "IAudioEngine.h"
#include "IFormManager.h"

//  测试代码：修改为直接读取csv文件，以便于观察配置文件修改的效果；
#define MAGIC_CONFIG_FILENAME		"Data\\Scp\\Magic.csv"
#define EFFECTVIEW_CONFIG_FILENAME	"Data\\Scp\\EffectView.csv"
#define SKILLVIEW_CONFIG_FILENAME	"Data\\Scp\\SkillView.csv"
#define MWDINFO_CONFIG_FILENAME		"Data\\mpw.info"

static bool g_bRegistered = false;

EntityFactory::EntityFactory()
{

}

EntityFactory::~EntityFactory()
{
	close();
}

bool EntityFactory::create()
{
	new MagicDataManager();
	new EffectDataManager();
	new SkillViewDataManager();
	new FlowTextManager();
	//new MWDLoader();
	new MWPFileNameMap();
	new MZIDMap();
	new EffectControlManager();
	new MwdInfoReader();

	// 加载脚本
// 	std::string path = xs::getWorkDir();
// 	path += MAGIC_CONFIG_FILENAME;

	std::string path = MAGIC_CONFIG_FILENAME;

	if (!gGlobalClient->getSchemeEngine()->LoadScheme(path.c_str(), MagicDataManager::getInstancePtr()))
	{
		Error("LoadScheme: '"<<MAGIC_CONFIG_FILENAME<<"' failed"<<endl);
		return false;
	}

// 	path = xs::getWorkDir();
// 	path += EFFECTVIEW_CONFIG_FILENAME;

	path = EFFECTVIEW_CONFIG_FILENAME;

	if (!gGlobalClient->getSchemeEngine()->LoadScheme(path.c_str(), EffectDataManager::getInstancePtr()))
	{
		Error("LoadScheme: '"<<EFFECTVIEW_CONFIG_FILENAME<<"' failed"<<endl);
		return false;
	}

// 	path = xs::getWorkDir();
// 	path += SKILLVIEW_CONFIG_FILENAME;

	path = SKILLVIEW_CONFIG_FILENAME;

	if (!gGlobalClient->getSchemeEngine()->LoadScheme(path.c_str(), SkillViewDataManager::getInstancePtr()))
	{
		Error("LoadScheme: '"<<SKILLVIEW_CONFIG_FILENAME<<"' failed"<<endl);
		return false;
	}

// 	path = xs::getWorkDir();
// 	path += MWDINFO_CONFIG_FILENAME;

	path = MWDINFO_CONFIG_FILENAME;
	if (!MwdInfoReader::getInstance().load(path.c_str()))
	{
		Error("Load mpw info file: '"<<MWDINFO_CONFIG_FILENAME<<"' failed"<<endl);
		return false;
	}

	return true;
}

void EntityFactory::close()
{
	if (!MagicDataManager::getInstancePtr()) return;

	gGlobalClient->getResourceManager()->unregisterResourceLoader(typeResourcePic);
	gGlobalClient->getResourceManager()->unregisterResourceLoader(typeResourceModel);

	delete MwdInfoReader::getInstancePtr();
	delete EffectControlManager::getInstancePtr();
	delete MZIDMap::getInstancePtr();
	delete MWPFileNameMap::getInstancePtr();
	//delete MWDLoader::getInstancePtr();
	delete FlowTextManager::getInstancePtr();
	delete SkillViewDataManager::getInstancePtr();
	delete EffectDataManager::getInstancePtr();
	delete MagicDataManager::getInstancePtr();
}

EntityView* EntityFactory::createEntity(ulong entityType, ulong resType, Stream* stream, size_t len)
{
	if (entityType >= typeMaxCount)
	{
		Error("Unknow entity type: "<<entityType<<endl);
		return NULL;
	}

	if (resType < 0 || resType >= ResourceTypeMaxCount)
	{
		Error("ResourceType("<<resType<<") is Error!"<<endl);
		return NULL;
	}

	EntityView* view = NULL;

	switch (entityType)
	{
	case typeStatic:
	case typeWaypoint:
	case typeSpile:
		{
			EntityViewImpl* ev = new EntityViewImpl();
			ev->setEntityType(entityType);
			ev->setResType(resType);
			view = ev;

			EntityComponent* ec = new VisualComponentMPW();
			ec->setOwner(ev);
			ec->create();
			ev->setComponent(ComponentIDVisual, ec);
		}
		break;

	
	 //case typeCreature: //下面3个取代typeCreature
	case typePerson:
	case typePet:
	case typeMonster:
	case typeCage:
		{
			EntityViewImpl* ev = new EntityViewImpl();
			ev->setEntityType(entityType);
			ev->setResType(resType);
            //  生物（包括主角和怪物）设置为不占位；
            ev->setOccupantValue(0);
			view = ev;

			EntityComponent* ec = new VisualComponentMZ();
			ec->setOwner(ev);
			ec->create();
			ev->setComponent(ComponentIDVisual, ec);

			MoveComponentMZ* mc = new MoveComponentMZ();
			mc->setOwner(ev);
			mc->create();
			ev->setComponent(ComponentIDMove, mc);
		}
		break;
	case typeBox:
	case typeBubble: //add by zjp
		{
			EntityViewImpl* ev = new EntityViewImpl();
			ev->setEntityType(entityType);
			ev->setResType(resType);
			view = ev;

			ev->setOccupantValue(0);
			EntityComponent* ec = new VisualComponentBOX();
			ec->setOwner(ev);
			ec->create();
			ev->setComponent(ComponentIDVisual, ec);
		}
		break;

	case typeEffect:
		{
			MagicView* ev = new MagicView();
			ev->setEntityType(entityType);
			ev->setResType(resType);
			view = ev;
		}
		break;

	default:
		Error("No implement new object or memory error, entity type: "<<entityType<<endl);
		return NULL;
	}

	if (!view)
		return NULL;

	size_t actual = view->onLoad(stream, len);
	if (actual == 0 || actual > len)
	{
		Error("onLoad(), length error, entity type: "<<entityType<<endl);
		view->release();
		return NULL;
	}

	return view;
}

EntityView* EntityFactory::createEntity(ulong entityType, ulong resType, const uchar* buf, size_t len)
{
	MemoryStream stream((uchar*)buf, len);
	return createEntity(entityType, resType, &stream, len);
}

void EntityFactory::flowText(const FlowTextContext& context)
{
	FlowTextManager::getInstance().flowText(context);
}

void EntityFactory::flowAreaText(int area, const std::string& text, const ColorValue& color,const DWORD font)
{
	FlowTextManager::getInstance().flowAreaText(area, text, color,font);
}

const std::string EntityFactory::GetLastFlowString(FlowAreaType area)
{
	return FlowTextManager::getInstance().GetLastFlowString(area);
}

void EntityFactory::updateFlowText(ulong deltaTime)
{
	FlowTextManager::getInstance().updateFlowText(deltaTime);
}

void EntityFactory::updateSystemTips(ulong deltaTime)
{
	FlowTextManager::getInstance().updateSystemTips(deltaTime);
}

void EntityFactory::renderFlowText(IRenderSystem* pRenderSystem)
{
	FlowTextManager::getInstance().renderFlowText(pRenderSystem);

}

void EntityFactory::renderSystemTips(IRenderSystem* pRenderSystem)
{
	FlowTextManager::getInstance().renderSystemTips(pRenderSystem);
}



EntityView* EntityFactory::createMagic(ulong type, const void* context, size_t len) const
{
	switch (type)
	{
	case MagicType_General:
		if (len == sizeof(MagicContext_General))
		{
			MagicView* magic = new MagicView();
			if (!magic->create(*(MagicContext_General*)context))
			{
				safeDelete(magic);
				return 0;
			}
			return magic;
		}
		break;
	case MagicType_MoveRadial:
		if (len == sizeof(MagicContext_MoveRadial))
		{
			MagicViewWithMoveRadial* magic = new MagicViewWithMoveRadial();
			if (!magic->create(*(MagicContext_MoveRadial*)context))
			{
				safeDelete(magic);
				return 0;
			}
			return magic;
		}
		break;
	case MagicType_MoveTrack:
		if (len == sizeof(MagicContext_MoveTrack))
		{
			MagicViewWithMoveTrack* magic = new MagicViewWithMoveTrack();
			if (!magic->create(*(MagicContext_MoveTrack*)context))
			{
				safeDelete(magic);
				return 0;
			}
			return magic;
		}
		break;
	}

	return 0;
}

bool EntityFactory::createEffect(ulong type, const void* context, size_t len)
{
	if (!context || len == 0) return false;

	switch (type)
	{
	case EffectType_General:
		if (len == sizeof(EffectContext_General))
		{
			const EffectContext_General* ctx = (const EffectContext_General*)context;
			MagicContext_General mc;
			mc.magicId = ctx->magicId;
			mc.loops = ctx->loops;
			mc.angle = ctx->angle;
			mc.owner = ctx->owner;
			mc.tile = ctx->tile;
			MagicView* magic = (MagicView*)createMagic(MagicType_General, &mc, sizeof(mc));
			if (!magic)
				return false;

			return true;
		}
		break;
	case EffectType_Fly:
		{
			if (len == sizeof(EffectContext_Fly))
			{
				EffectControl_Fly* ec = new EffectControl_Fly();
				if (!ec->create(*(const EffectContext_Fly*)context))
				{
					delete ec;
					return false;
				}
			}
		}
		break;
	case EffectType_LifeTime:
		{
			if (len == sizeof(EffectContext_LifeTime))
			{
				EffectControl_LifeTime* ec = new EffectControl_LifeTime();
				if (!ec->create(*(const EffectContext_LifeTime*)context))
				{
					delete ec;
					return false;
				}
			}
		}
		break;
	case EffectType_MultiTimes:
		{
#if 0
			if (len == sizeof(EffectContext_MultiTimes))
			{
				EffectControl_MultiTimes* ec = new EffectControl_MultiTimes();
				if (!ec->create(*(const EffectContext_MultiTimes*)context))
				{
					delete ec;
					return false;
				}
			}
#endif
		}
		break;
	case EffectType_Area:
		{
			if (len == sizeof(EffectContext_Area))
			{
				EffectControl_Area* ec = new EffectControl_Area();
				if (!ec->create(*(const EffectContext_Area*)context))
				{
					delete ec;
					return false;
				}
			}
		}
		break;
	case EffectType_MultiDir:
		{
			if (len == sizeof(EffectContext_MultiDir))
			{
				EffectControl_MultiDir* ec = new EffectControl_MultiDir();
				if (!ec->create(*(const EffectContext_MultiDir*)context))
				{
					delete ec;
					return false;
				}
			}
		}
		break;
	default:
		return false;
	}
	return true;
}

bool EntityFactory::createEffect(ulong effId, const xs::Point& ptTile)
{	
	if (effId == 0) return false;

	ulong magicId = EffectDataManager::getInstance().getValue(effId, EffectProperty_MagicId).getInt();
	if (magicId)
	{
		int loops = EffectDataManager::getInstance().getValue(effId, EffectProperty_Loops).getInt();
		if (loops <= 0) loops = -1;

		MagicContext_General mc;
		mc.magicId = magicId;
		mc.loops = loops;
		mc.angle = 0;
		mc.owner = 0;
		mc.tile = ptTile;
		mc.nDuration = 0;

		// 非循环光效，当实体不在屏幕范围内时，不需要创建
		MagicView* mv = (MagicView*)createMagic(MagicType_General, &mc, sizeof(mc));
		if (!mv)
			return false;
	}
	return true;
}

bool EntityFactory::createBuffEffect(EntityView* view, ulong effId)
{
	if ((view == NULL) || (effId == 0) || (!EffectDataManager::getInstance().HasRecord(effId)))
    {
        ASSERT(false);
        return false;
    }

    ulong magicId = EffectDataManager::getInstance().getValue(effId, EffectProperty_MagicId).getInt();
	if (magicId)
	{
		int loops = EffectDataManager::getInstance().getValue(effId, EffectProperty_Loops).getInt();
		if (loops <= 0) loops = -1;

		MagicContext_General mc;
		mc.magicId = magicId;
		mc.loops = loops;
		mc.angle = 0;
		mc.owner = view->getHandle();
		mc.tile = view->getTile();

		// 非循环光效，当实体不在屏幕范围内时，不需要创建
		if (loops == -1 || view->hasFlag(flagInScreen))
		{
			MagicView* mv = (MagicView*)createMagic(MagicType_General, &mc, sizeof(mc));
			if (!mv)
				return false;

			int tailSilence = EffectDataManager::getInstance().getValue(effId, EffectProperty_TailSilence).getInt();
			mv->setTailSilence(tailSilence != 0);
		}
	}

	// 实体颜色修改
	const IntArray& color = EffectDataManager::getInstance().getValue(effId, EffectProperty_Color).getIntArray();
	if (color.count == 4)
	{
#define _1_div_255	0.003922f // = 1/255
		ColorValue val;
		val.r = color.data[0] * _1_div_255;
		val.g = color.data[1] * _1_div_255;
		val.b = color.data[2] * _1_div_255;
		val.a = color.data[3] * _1_div_255;
		ModifierColor* mdf = new ModifierColor(effId, val);
		view->onCommand(EntityCommand_AddModifier, (ulong)mdf);
	}

	// 实体缩放
	float scale = EffectDataManager::getInstance().getValue(effId, EffectProperty_Scale).getFloat();
	if ((scale > 0) && (!Math::RealEqual(scale, 1.0f)))
	{
		ModifierScale* mdf = new ModifierScale(effId, scale);
		view->onCommand(EntityCommand_AddModifier, (ulong)mdf);
	}

	// 眩晕动作
	bool needStunAction = EffectDataManager::getInstance().getValue(effId, EffectProperty_NeedStunAction).getInt() != 0;
	if (needStunAction)
	{
		ModifierStun* mdf = new ModifierStun(effId);
		view->onCommand(EntityCommand_AddModifier, (ulong)mdf);
	}

	// 拖影效果
	bool needShadow = EffectDataManager::getInstance().getValue(effId, EffectProperty_NeedShadow).getInt() != 0;
	if (needShadow)
	{
		// TODO 拖影效果
	}

	// 变身效果
	const IntArray& arr = EffectDataManager::getInstance().getValue(effId, EffectProperty_ChangeSharp).getIntArray();
	if (arr.count != 0)
	{
		//arr.data[0]; // 图标Id
		//arr.data[1]; // 优先级
		// TODO
		view->saveResId(view->getResId());
		view->onCommand(EntityCommand_ChangeModel,(ulong)arr.data[0]);
	}

	return true;
}

void EntityFactory::closeBuffEffect(EntityView* view, ulong effId)
{
	if (!view || effId == 0) return;

	ulong magicId = EffectDataManager::getInstance().getValue(effId, EffectProperty_MagicId).getInt();
	if (magicId)
	{
		int loops = EffectDataManager::getInstance().getValue(effId, EffectProperty_Loops).getInt();
		if (loops <= 0) loops = -1;
		view->onCommand(EntityCommand_DetachMagic, magicId, loops);
	}

	view->onCommand(EntityCommand_RemoveModifiers, effId);
	ulong id = view->getsaveId();
	if (view->getResId()!= id && id!=0)
	{
		view->onCommand(EntityCommand_ChangeModel,(ulong)id);
	}
}

const float EntityFactory::GetMagicAniSpeed(uint id)
{
	return MagicDataManager::getInstance().getValue(id, MagicProperty_AniSpeed).getFloat();
}

const uint EntityFactory::GetMagicAniTrans(uint id)
{
	return MagicDataManager::getInstance().getValue(id, MagicProperty_AniTrans).getInt();
}

const std::string EntityFactory::GetMagicAniOffset(uint id)
{
	const IntArray& offset = MagicDataManager::getInstance().getValue(id, MagicProperty_AniOffset).getIntArray();
	int n = offset.count;
	std::string str = "";
	for (int i=0; i<n; ++i)
	{
		char buf[16];
		sprintf_s(buf, sizeof(buf), "%d", offset.data[i]);
		if (i < n - 1)
			str = str + buf + ";";
		else
			str += buf;
	}
	return str;
}

const float EntityFactory::GetMagicAniScale(uint id)
{
	return MagicDataManager::getInstance().getValue(id, MagicProperty_AniScale).getFloat();
}

const uint EntityFactory::GetMagicResId(uint id)
{
	return MagicDataManager::getInstance().getValue(id, MagicProperty_ResId).getInt();
}



//inline int getInt(id, field) SkillViewDataManager::getInstance().getValue(id, field).getInt()
//#define getFloat(id, field) SkillViewDataManager::getInstance().getValue(id, field).getFloat()
//#define getIntArray(id, field) SkillViewDataManager::getInstance().getValue(id, field).getIntArray()
inline int getInt(int id, int field)
{
	return	SkillViewDataManager::getInstance().getValue(id, field).getInt();
}

inline float getFloat(int id, int field)
{
	return SkillViewDataManager::getInstance().getValue(id, field).getFloat();
}

inline IntArray& getIntArray(int id, int field)
{
	return (IntArray&)SkillViewDataManager::getInstance().getValue(id, field).getIntArray();
}
bool EntityFactory::createSkillReadyEffect(const SkillEffectContext& context, bool bPlayAction, bool bHasInterval)
{
	EntityView* src = (EntityView*)getHandleData(context.src);
	if (src == NULL)
	{
		return false;
	}

	// 是否需调整光效方向
	long magicAngle = 0;
	if (getInt(context.id, SkillViewProperty_NotAdjustDir) == 0)
    {
		magicAngle = src->getAngle();
    }

	if (bPlayAction)
	{
		// 生物发起攻击动作
		AttackContext ac;
		ac.actionId = getInt(context.id, SkillViewProperty_AttackReadyActId);
		ac.loops = -1;
		ac.fAnimateSpeed = getFloat(context.id, SkillViewProperty_AttackReadyActSpeed);
		ac.isSpellAttack = context.isSpellSkill;
		//bool bMounted = false;
		//src->onCommand(EntityCommand_GetMount, (ulong)&bMounted);
		//if (bMounted)
		{
			src->onCommand(EntityCommand_SetMount, 0);
		}
		src->onCommand(EntityCommand_AttackReady, (ulong)&ac);

		//播放蓄气声效
		int nSoundID = getInt(context.id,SkillViewProperty_SoundIDPreAttack);
		int nSoundLoop = getInt(context.id,SkillViewProperty_bSoundLoopPreAttack);
		if (nSoundID>1000)
		{
			IFormManager* pFormManger = gGlobalClient->getFormManager();
			if (pFormManger)
			{
				pFormManger->PlaySound(nSoundID,nSoundLoop,0.8,SOUNDRES_TYPE_SOUND);
			}
		}
	}

    //  蓄气光效；
	ulong magicId = getInt(context.id, SkillViewProperty_AttackReadyMagicId);
	if (magicId != 0)
	{
		if (context.src)
		{
			MagicContext_General mc;
			mc.magicId = magicId;
			if (bHasInterval)
			{
				mc.loops = 1;
			}
			else
			{
				mc.loops = -1;
			}
            //  这里不使用context中记录的光效持续时间，而是由光效绑定者来决定什么时候结束光效；
            mc.nDuration = 0;
			mc.angle = magicAngle;
			mc.owner = context.src;
			mc.tile = src->getTile();
			MagicView* magic = (MagicView*)createMagic(MagicType_General, &mc, sizeof(mc));
			if (magic == NULL)
            {
                ASSERT(false);
				return false;
            }
		}
	}

	if (bPlayAction)
	{
		//  蓄气时在目标身上的光效；
		ulong magicIdOnTarget = getInt(context.id, SkillViewProperty_AttackReadyMagicIdOnTarget);
		if (magicIdOnTarget != 0)
		{
			if (context.src)
			{
				//  填写光效参数；
				MagicContext_General mc;
				mc.magicId = magicIdOnTarget;
				mc.loops = -1;
				//  这里不使用context中记录的光效持续时间，而是由光效绑定者来决定什么时候结束光效；
				mc.nDuration = 0;
				mc.angle = magicAngle;
				//  读取一下光效是否绑定在目标上；
				int nAttackReadyMagicOnTargetAttachToTarget = getInt(context.id, SkillViewProperty_AttackReadyMagicOnTargetAttachToTarget);
				if (nAttackReadyMagicOnTargetAttachToTarget == 1)
				{
					mc.owner = context.target;
				}
				else
				{
					mc.owner = 0;
				}
				mc.tile = context.ptTarget;
				//  创建光效；
				MagicView* pTargetMagic = (MagicView*)createMagic(MagicType_General, &mc, sizeof(mc));
				if (pTargetMagic == NULL)
				{
					ASSERT(false);
					return false;
				}

				//  如果光效没有绑定在目标上，则将其添加到与源相关的光效中；
				if (nAttackReadyMagicOnTargetAttachToTarget != 1)
				{
					src->onCommand(EntityCommand_AddMagic, (ulong)pTargetMagic);
				}
			}
		}
	}

	return true;
}

void EntityFactory::closeSkillReadyEffect(EntityView* pSourceView, EntityView* pDestView, ulong id)
{
	if ((pSourceView == NULL) || (id == 0))
    {
        return;
    }


    //  关闭蓄气光效；
	ulong magicId = getInt(id, SkillViewProperty_AttackReadyMagicId);
	pSourceView->onCommand(EntityCommand_DetachMagic, magicId, -1);

    //  关闭目标身上的光效；
    ulong ulMagicIdOnTarget = getInt(id, SkillViewProperty_AttackReadyMagicIdOnTarget);
    //  读取一下光效是否绑定在目标上；
    int nAttackReadyMagicOnTargetAttachToTarget = getInt(id, SkillViewProperty_AttackReadyMagicOnTargetAttachToTarget);
    if (ulMagicIdOnTarget != 0)
    {
        if ((nAttackReadyMagicOnTargetAttachToTarget == 1) && (pDestView != NULL))
        {
            pDestView->onCommand(EntityCommand_DetachMagic, ulMagicIdOnTarget, -1);
        }
        else
        {
            pSourceView->onCommand(EntityCommand_RemoveMagic, ulMagicIdOnTarget, -1);
        }
    }
}

bool EntityFactory::CreateSkillBootEffect(const SkillEffectContext& context)
{
	EntityView* pSourceEntityView = (EntityView*)getHandleData(context.src);
	if (pSourceEntityView == NULL)
	{
		ASSERT(false);
		return false;
	}

	//  调整光效方向；
	long magicAngle = 0;
	if (getInt(context.id, SkillViewProperty_NotAdjustDir) == 0)
	{
		magicAngle = pSourceEntityView->getAngle();
	}

	//  发起攻击动作；
	AttackContext ac;
	ac.actionId = getInt(context.id, SkillViewProperty_AttackReadyActId);
	ac.loops = -1;
	ac.fAnimateSpeed = getFloat(context.id, SkillViewProperty_AttackReadyActSpeed);
	ac.isSpellAttack = context.isSpellSkill;
	bool bMounted = false;
	//pSourceEntityView->onCommand(EntityCommand_GetMount, (ulong)&bMounted);
	//if (bMounted)
	{
		pSourceEntityView->onCommand(EntityCommand_SetMount, 0);
	}
	pSourceEntityView->onCommand(EntityCommand_AttackReady, (ulong)&ac);

	//  生物攻击瞬间的光效；
	int nAttackMagicID = getInt(context.id, SkillViewProperty_AttackReadyMagicId);
	if (nAttackMagicID != 0)
	{
		EffectContext_LifeTime ec;
		ec.magicId = nAttackMagicID;
		ec.loops = -1;
		ec.angle = magicAngle;
		//  读取一下光效是否绑定在源上；
		int nAttackMagicAttachToSource = getInt(context.id, SkillViewProperty_AttackMagicAttachToSource);
		if (nAttackMagicAttachToSource == 1)
		{
			ec.owner = context.src;
		}
		else
		{
			ec.owner = 0;
		}
		ec.tile = pSourceEntityView->getTile();
		ec.life = context.nDuration;
		EffectControl_LifeTime* ctrl = new EffectControl_LifeTime();
		if (!ctrl->create(ec))
		{
			delete ctrl;
			return false;
		}
		ctrl->setSrc(context.src);
		ctrl->setViewId(context.id);
		EffectControlManager::getInstance().add(ctrl);
		return true;
	}

	return false;

	//  创建技能引导光效；
	ulong effectType = getInt(context.id, SkillViewProperty_EffectType);
	ulong magicId = getInt(context.id, SkillViewProperty_TrackMagicId);
	handle magicOwner = context.selectType == 2 ? INVALID_HANDLE : (context.selectType == 0 ? context.src : context.target);
	ulong ulInterval = getInt(context.id, SkillViewProperty_Delay);
	switch (effectType)
	{
	case EffectType_General:
		{
		}
		break;

	case EffectType_Fly:
		{
			//  填写飞行光效的参数；
			EffectContext_Fly ec;
			ec.magicId = magicId;
			ec.angle = magicAngle;
			ec.loops = -1;
			ec.moveSpeed = getFloat(context.id, SkillViewProperty_FlySpeed);
			ec.src = context.src;
			ec.target = context.target;
			ec.idExplode = getInt(context.id, SkillViewProperty_ExplodeMagicID);
			//  读取一下爆炸光效是否绑定在目标上；
			int nExplodeMagicAttachToTarget = getInt(context.id, SkillViewProperty_ExplodeMagicAttachToTarget);
			if (nExplodeMagicAttachToTarget == 1)
			{
				ec.hExplodeTarget = context.target;
			}
			else
			{
				ec.hExplodeTarget = 0;
			}
			ec.loopsExplode = 1;
			ec.ExplodeEffectContext = context;

#if 0
			EffectControl_Fly* ctrl = new EffectControl_Fly();
			if (!ctrl->create(ec))
			{
				delete ctrl;
				return false;
			}

			ctrl->setSrc(context.src);
			ctrl->setViewId(context.id);
			EffectControlManager::getInstance().add(ctrl);
#endif

			//  填写多次播放的光效的参数；
			EffectContext_MultiTimes< EffectContext_Fly > MultiTimesEffectContext;
			MultiTimesEffectContext.m_ulDuration = context.nDuration;
			MultiTimesEffectContext.m_ulInterval = ulInterval;
			MultiTimesEffectContext.m_EffectControlContext = ec;

			//  创建多次播放的光效；
			EffectControl_MultiTimes< EffectContext_Fly, EffectControl_Fly >* pMultiTimesEffect =
				new EffectControl_MultiTimes< EffectContext_Fly, EffectControl_Fly >();
			if (!pMultiTimesEffect->create(MultiTimesEffectContext))
			{
				delete pMultiTimesEffect;
				return false;
			}

			//  将创建的光效记录在光效控制管理器中，以便于后面的删除操作；
			pMultiTimesEffect->setSrc(context.src);
			pMultiTimesEffect->setViewId(context.id);
			EffectControlManager::getInstance().add(pMultiTimesEffect);

			return true;
		}
		break;

	case EffectType_LifeTime:
		{
		}
		break;

	case EffectType_MultiTimes:
		{
		}
		break;

	case EffectType_Area:
		{
		}
		break;

	case EffectType_MultiDir:
		{
		}
		break;

	case EffectType_RapidMove:
		{
		}
		break;

	case EffectType_DestRapidMove:
		{
		}
		break;

	case EffectType_SrcDestRapidMove:
		{
		}
		break;

		//	//跳跃类
	case EffectType_Jump:
		{
		}
		break;
	}

	return false;
}

void EntityFactory::CloseSkillBootEffect(EntityView* pSourceView, ulong ulSkillViewID)
{
	ASSERT(pSourceView != NULL);
	if (pSourceView != NULL)
	{
		EffectControlManager::getInstance().removeAll(pSourceView->getHandle(), ulSkillViewID);
	}
}

// 创建游戏场景播放的特效，例如下雨、下雪 zgz [4/18/2011 zgz]
MagicView* EntityFactory::CreateSceneMagic(const SceneMagicContext& context)
{
	if (0 == context.magicID)
	{
		return false;
	}

	MagicContext_General mc;
	mc.magicId = context.magicID;
	mc.loops = -1;
	mc.angle = 0;
	mc.owner = 0;
	mc.bSceneMagic = true;

	MagicView* mv =  (MagicView*)createMagic(MagicType_General, &mc, sizeof(mc));
	if (NULL != mv)
	{
		if (0 != context.nFade)
		{
			mv->addFlag(flagFade);
		}

		if (0 != context.nFall)
		{
			mv->addFlag(flagFallMagic);
		}
	}
	return mv;
}

// 删除游戏场景播放的特效 
bool EntityFactory::CloseSceneMagic(MagicView* mv)
{
	ISceneManager2* pSceneManager = gGlobalClient->getSceneManager();
	if (NULL != pSceneManager)
	{
		if (NULL != mv && mv->hasFlag(flagGlobalSceneMagic))
		{
			if (pSceneManager->removeEntity(mv->getTile(), mv))
			{
				if (mv->hasFlag(flagFade))
				{
					mv->setNeedDelete();
				}
				else if (mv->hasFlag(flagFallMagic))
				{
					mv->setNeedDelete();
					mv->SetGenerateParticle(false);
				}
				else
				{
					SAFE_DELETE(mv);
				}				
				return true;
			}
		}		
	}
	return false;
}

/** 创建受击特效
@param   
@param   
@return  
*/
bool EntityFactory::CreateBeHitEffect(const SkillEffectContext& context)
{
	EntityView* pSourceEntityView = (EntityView*)getHandleData(context.src);
	if (pSourceEntityView == NULL)
	{
		//ASSERT(false);
		return false;
	}

	//  调整光效方向；
	long magicAngle = pSourceEntityView->getAngle();
    
	//  目标受击音效；
	int nSoundID = getInt(context.id,SkillViewProperty_SoundIDBeHit);
	int nSoundLoop = getInt(context.id,SkillViewProperty_bSoundLoopeBeHit);
	if (nSoundID > 1000)
	{
		IFormManager* pFormManger = gGlobalClient->getFormManager();
		if (pFormManger)
		{
			pFormManger->PlaySound(nSoundID,nSoundLoop,0.8,SOUNDRES_TYPE_SOUND);
		}
	}

	// 目标受击光效
	int nAttackMagicID = getInt(context.id, SkillViewProperty_BeHitMagicId);
	if (nAttackMagicID != 0)
	{
		EffectContext_General ec;
		ec.magicId = nAttackMagicID;
		ec.loops = 1;
		ec.angle = magicAngle;
		ec.owner = context.target;
		ec.tile = context.ptCenter;
		EffectControl_General* ctrl = new EffectControl_General();
		if (!ctrl->create(ec))
		{
			delete ctrl;
			return false;
		}
		ctrl->setSrc(context.src);
		ctrl->setViewId(context.id);
		EffectControlManager::getInstance().add(ctrl);

		return true;
	}

	return true;
}

/** 创建受击源到目标飞行特效
@param   
@param   
@return  
*/
bool EntityFactory::CreateBeHitFlyEffect(const SkillEffectContext& context)
{
	EntityView* pSourceEntityView = (EntityView*)getHandleData(context.src);
	if (pSourceEntityView == NULL)
	{
		ASSERT(false);
		return false;
	}

	//  调整光效方向；
	long magicAngle = pSourceEntityView->getAngle();
	//  创建飞行光效；
	handle magicOwner = context.src;

	int nFlyMagicID = getInt(context.id, SkillViewProperty_BeHitFlyMagicId);
	if (nFlyMagicID != 0)
	{
		EffectContext_Fly ec;
		ec.magicId = nFlyMagicID;
		ec.angle = magicAngle;
		ec.loops = -1;
		ec.moveSpeed = getFloat(context.id, SkillViewProperty_FlySpeed);
		ec.src = context.src;
		ec.target = context.target;
		ec.idExplode = getInt(context.id, SkillViewProperty_ExplodeMagicID);
		//  读取一下爆炸光效是否绑定在目标上；
		int nExplodeMagicAttachToTarget = getInt(context.id, SkillViewProperty_ExplodeMagicAttachToTarget);
		if (nExplodeMagicAttachToTarget == 1)
		{
			ec.hExplodeTarget = context.target;
		}
		else
		{
			ec.hExplodeTarget = 0;
		}
		ec.loopsExplode = 1;
		ec.ExplodeEffectContext = context;
		//return createEffect(EffectType_Fly, &ec, sizeof(EffectContext_Fly));

		EffectControl_Fly* ctrl = new EffectControl_Fly();
		if (!ctrl->create(ec))
		{
			delete ctrl;
			return false;
		}

		ctrl->setSrc(context.src);
		ctrl->setViewId(context.id);
		EffectControlManager::getInstance().add(ctrl);
		return true;
	}

	return true;
}

bool EntityFactory::createSkillAttackEffect(const SkillEffectContext& context)
{
	EntityView* pSourceEntityView = (EntityView*)getHandleData(context.src);
	if ((pSourceEntityView == NULL) || (context.id == 0))
	{
		ASSERT(false);
		return false;
	}

	// 是否需调整光效方向
	long magicAngle = 0;
	if (getInt(context.id, SkillViewProperty_NotAdjustDir) == 1)
    {
       magicAngle = pSourceEntityView->getAngle();
    }

	// 生物发起攻击动作
	AttackContext ac;

	// 取得配置的多个攻击动作中随机的一个
	int nActionId;
	const IntArray & AttackActId = getIntArray(context.id, SkillViewProperty_AttackActId);
	int nCount = AttackActId.count;

	if (nCount == 1)
	{
		nActionId = AttackActId.data[0];
	}
	else if (nCount > 1)
	{
		int nIndex = getRand(1, nCount);
		nActionId = AttackActId.data[nIndex - 1];
	}
	else
	{
		nActionId = 0;
	}

	ac.actionId = nActionId;

	ac.loops = context.loops;
	ac.fAnimateSpeed = getFloat(context.id, SkillViewProperty_AttackActSpeed);
	ac.isSpellAttack = context.isSpellSkill;
    //  冲锋技能是在瞬移效果完成后再做攻击动作；
	ulong effectType = getInt(context.id, SkillViewProperty_EffectType);
	if ( effectType != EffectType_Jump
		/*&& effectType != EffectType_RapidMove*/) // modify by zjp;冲锋的过程中做动作
	{
		if (ac.actionId!=0)		
		{
			//bool bMounted = false;
			//pSourceEntityView->onCommand(EntityCommand_GetMount, (ulong)&bMounted);
			//if (bMounted)
			{
				pSourceEntityView->onCommand(EntityCommand_SetMount, 0);
			}
			pSourceEntityView->onCommand(EntityCommand_Attack, (ulong)&ac);
		}
	}
    //  捆仙索在完成后也不需要做动作  
	if (effectType == EffectType_DestRapidMove || effectType == EffectType_SrcDestRapidMove)
    {
        ac.actionId = 0;
    }

    //  施法光效；
    int nAttackMagicID = getInt(context.id, SkillViewProperty_AttackMagicId);
    if (nAttackMagicID != 0)
    {
        EffectContext_General ec;
        ec.magicId = nAttackMagicID;
        ec.loops = 1;
        ec.angle = magicAngle;
		//  读取一下光效是否绑定在源上；
		int nAttackMagicAttachToSource = getInt(context.id, SkillViewProperty_AttackMagicAttachToSource);
		if (nAttackMagicAttachToSource == 1)
		{
			ec.owner = context.src;
		}
		else
		{
			ec.owner = 0;
		}
		// modify by zjp.修改特效释放的位置
		//ec.tile = pSourceEntityView->getTile();
		ec.tile = context.ptCenter;

        EffectControl_General* ctrl = new EffectControl_General();
        if (!ctrl->create(ec))
        {
            delete ctrl;
            return false;
        }
        ctrl->setSrc(context.src);
        ctrl->setViewId(context.id);
        EffectControlManager::getInstance().add(ctrl);
    }

	//播放攻击声效
	const IntArray & nSoundIDArray = getIntArray(context.id,SkillViewProperty_SoundIDAttack);
	int nSoundCount = nSoundIDArray.count;
	int nSoundID = 0;
	if (nSoundCount == 1)
	{
		nSoundID = nSoundIDArray.data[0];
	}
	else if (nSoundCount > 1)
	{
		IEntity* pEntity = (IEntity*)pSourceEntityView->getUserData();
		if (pEntity && pEntity->GetEntityClass()->IsPerson())
		{
			int PersonSex = pEntity->GetNumProp(CREATURE_PROP_SEX);
			if (PersonSex == PERSON_SEX_MALE)
			{
				nSoundID = nSoundIDArray.data[0];
			}
			else if(PersonSex == PERSON_SEX_FEMALE)
			{
				nSoundID = nSoundIDArray.data[1];
			}
		}
	}
	int nSoundLoop = getInt(context.id,SkillViewProperty_bSoundLoopeAttack);
	if (nSoundID>1000)
	{
		IFormManager* pFormManger = gGlobalClient->getFormManager();
		if (pFormManger)
		{
			pFormManger->PlaySound(nSoundID,nSoundLoop,0.8,SOUNDRES_TYPE_SOUND);
		}
	}

	return true;
}

bool EntityFactory::createSkillTrackEffect(const SkillEffectContext& context)
{
	EntityView* pSourceEntityView = (EntityView*)getHandleData(context.src);
	if ((pSourceEntityView == NULL) || (context.id == 0))
	{
		ASSERT(false);
		return false;
	}

	// 是否需调整光效方向
	long magicAngle = 0;
	//if (getInt(context.id, SkillViewProperty_NotAdjustDir) == 0)
	//{
		magicAngle = pSourceEntityView->getAngle();
	//}

	//// 生物发起攻击动作
	AttackContext ac;

	// 取得配置的多个攻击动作中随机的一个
	int nActionId;
	const IntArray & AttackActId = getIntArray(context.id, SkillViewProperty_AttackActId);
	int nCount = AttackActId.count;

	if (nCount == 1)
	{
		nActionId = AttackActId.data[0];
	}
	else if (nCount > 1)
	{
		int nIndex = getRand(1, nCount);
		nActionId = AttackActId.data[nIndex - 1];
	}
	else
	{
		nActionId = 0;
	}

	ac.actionId = nActionId;

	ac.loops = context.loops;
	ac.fAnimateSpeed = getFloat(context.id, SkillViewProperty_AttackActSpeed);
	ac.isSpellAttack = context.isSpellSkill;
	//  冲锋技能是在瞬移效果完成后再做攻击动作；
	ulong effectType = getInt(context.id, SkillViewProperty_EffectType);
	//if (effectType != EffectType_RapidMove && effectType != EffectType_Jump)
	//{
	//	pSourceEntityView->onCommand(EntityCommand_Attack, (ulong)&ac);
	//}
	//  捆仙索技能在瞬移效果完成后也不需要做攻击动作；
	if (effectType == EffectType_DestRapidMove || effectType == EffectType_SrcDestRapidMove)
	{
		ac.actionId = 0;
	}

	ulong magicId = getInt(context.id, SkillViewProperty_TrackMagicId);
	if (magicId == 0 && (effectType != EffectType_RapidMove && effectType != EffectType_DestRapidMove && effectType != EffectType_SrcDestRapidMove && effectType != EffectType_Jump))
	{
		return true;//(context.id == 1001) ? true : false;
	}

	//  创建技能光效；
	handle magicOwner = context.selectType == 2 ? INVALID_HANDLE : (context.selectType == 0 ? context.src : context.target);
	switch (effectType)
	{
	case EffectType_General:
		{
			EffectContext_General ec;
			ec.magicId = magicId;
			ec.loops = 1;
			ec.angle = magicAngle;

			//  读取一下光效是否绑定在目标上；
			//  对于通过光效，也使用了SkillViewProperty_ExplodeMagicAttachToTarget来指定光效是否绑定在目标上；
			int nExplodeMagicAttachToTarget = getInt(context.id, SkillViewProperty_ExplodeMagicAttachToTarget);
			if (nExplodeMagicAttachToTarget == 1)
			{
				ec.owner = magicOwner;
			}
			else
			{
				ec.owner = 0;
			}
			ec.tile = context.ptCenter;
			//return createEffect(EffectType_General, &ec, sizeof(ec));

			EffectControl_General* ctrl = new EffectControl_General();
			if (!ctrl->create(ec))
			{
				delete ctrl;
				return false;
			}

			ctrl->setSrc(context.src);
			ctrl->setViewId(context.id);
			EffectControlManager::getInstance().add(ctrl);
			return true;
		}
		break;

	case EffectType_Fly:
		{
			EffectContext_Fly ec;
			ec.magicId = magicId;
			ec.angle = magicAngle;
			ec.loops = -1;
			ec.moveSpeed = getFloat(context.id, SkillViewProperty_FlySpeed);
			ec.src = context.src;
			ec.target = context.target;
			ec.idExplode = getInt(context.id, SkillViewProperty_ExplodeMagicID);
			//  读取一下爆炸光效是否绑定在目标上；
			int nExplodeMagicAttachToTarget = getInt(context.id, SkillViewProperty_ExplodeMagicAttachToTarget);
			if (nExplodeMagicAttachToTarget == 1)
			{
				ec.hExplodeTarget = context.target;
			}
			else
			{
				ec.hExplodeTarget = 0;
			}
			ec.loopsExplode = 1;
			ec.ExplodeEffectContext = context;
			//return createEffect(EffectType_Fly, &ec, sizeof(EffectContext_Fly));

			EffectControl_Fly* ctrl = new EffectControl_Fly();
			if (!ctrl->create(ec))
			{
				delete ctrl;
				return false;
			}

			ctrl->setSrc(context.src);
			ctrl->setViewId(context.id);
			EffectControlManager::getInstance().add(ctrl);
			return true;
		}
		break;

	case EffectType_LifeTime:
		{
			EffectContext_LifeTime ec;
			ec.magicId = magicId;
			ec.loops = -1;
			ec.angle = magicAngle;
			ec.owner = magicOwner;//context.target;
			ec.tile = context.ptCenter;
			ec.life = getInt(context.id, SkillViewProperty_LifeTime);
			//return createEffect(EffectType_LifeTime, &ec, sizeof(ec));

			EffectControl_LifeTime* ctrl = new EffectControl_LifeTime();
			if (!ctrl->create(ec))
			{
				delete ctrl;
				return false;
			}

			ctrl->setSrc(context.src);
			ctrl->setViewId(context.id);
			EffectControlManager::getInstance().add(ctrl);
			return true;
		}
		break;

	case EffectType_MultiTimes:
		{
#if 0
			EffectContext_MultiTimes ec;
			ec.magicId = magicId;
			ec.loops = 1;
			ec.angle = magicAngle;
			ec.tile = context.ptCenter;
			ec.interval = getInt(context.id, SkillViewProperty_Delay);
			ec.times = getInt(context.id, SkillViewProperty_Times);
			ec.owner = magicOwner;//context.target;
			//return createEffect(EffectType_MultiTimes, &ec, sizeof(EffectContext_MultiTimes));

			EffectControl_MultiTimes* ctrl = new EffectControl_MultiTimes();
			if (!ctrl->create(ec))
			{
				delete ctrl;
				return false;
			}

			ctrl->setSrc(context.src);
			ctrl->setViewId(context.id);
			EffectControlManager::getInstance().add(ctrl);
			return true;
#endif
		}
		break;

	case EffectType_Area:
		{
			EffectContext_Area ec;
			ec.magicId = magicId;
			ec.loops = 1;
			ec.angle = magicAngle;
			ec.owner = magicOwner;//0;
			ec.tile = context.ptCenter;
			ec.interval = getInt(context.id, SkillViewProperty_Delay);
			ec.life = getInt(context.id, SkillViewProperty_LifeTime);
            
            IntArray& intArray = getIntArray(context.id, SkillViewProperty_AttackRange);
			ec.arr = &intArray;
			//return createEffect(EffectType_Area, &ec, sizeof(EffectContext_Area));

			EffectControl_Area* ctrl = new EffectControl_Area();
			if (!ctrl->create(ec))
			{
				delete ctrl;
				return false;
			}

			ctrl->setSrc(context.src);
			ctrl->setViewId(context.id);
			EffectControlManager::getInstance().add(ctrl);
			return true;
		}
		break;

	case EffectType_MultiDir:
		{
			const IntArray& arr = getIntArray(context.id, SkillViewProperty_AttackRange);
			if (arr.count != 3)
				return false;

			EffectContext_MultiDir ec;
			ec.magicId = magicId;
			ec.loops = -1;
			ec.angle = magicAngle;
			ec.owner = context.src;
			ec.tile = context.ptCenter;
			ec.moveSpeed = getFloat(context.id, SkillViewProperty_FlySpeed);
			ec.dirs = arr.data[2];
			ec.blockType = 0;
			ec.width = arr.data[0];
			ec.maxDist = arr.data[1];
			ec.pt = pSourceEntityView->getTile();
			//return createEffect(EffectType_MultiDir, &ec, sizeof(EffectContext_MultiDir));

			EffectControl_MultiDir* ctrl = new EffectControl_MultiDir();
			if (!ctrl->create(ec))
			{
				delete ctrl;
				return false;
			}

			ctrl->setSrc(context.src);
			ctrl->setViewId(context.id);
			EffectControlManager::getInstance().add(ctrl);
			return true;
		}
		break;

	case EffectType_RapidMove:
		{
			//  创建爆炸光效；
			int nExplodeMagicID = getInt(context.id, SkillViewProperty_ExplodeMagicID);
			if (nExplodeMagicID != 0)
			{
				EffectContext_General ec;
				ec.magicId = nExplodeMagicID;
				ec.loops = 1;
				ec.angle = magicAngle;
				//  读取一下爆炸光效是否绑定在目标上；
				int nExplodeMagicAttachToTarget = getInt(context.id, SkillViewProperty_ExplodeMagicAttachToTarget);
				if (nExplodeMagicAttachToTarget == 1)
				{
					ec.owner = context.target;
				}
				else
				{
					ec.owner = 0;
				}
				ec.tile = context.ptTarget;
				EffectControl_General* ctrl = new EffectControl_General();
				if (!ctrl->create(ec))
				{
					delete ctrl;
					return false;
				}
				ctrl->setSrc(context.src);
				ctrl->setViewId(context.id);
				EffectControlManager::getInstance().add(ctrl);
			}

			//  创建瞬移特效；
			EffectContext_RapidMove ec;
			ec.magicId = magicId;
			ec.loops = -1;
			ec.angle = magicAngle;
			ec.owner = context.src;
			ec.tile = context.ptCenter;
			ec.ac = ac;

			EffectControl_RapidMove* ctrl = new EffectControl_RapidMove();
			if (!ctrl->create(ec))
			{
				delete ctrl;
				return false;
			}

			ctrl->setSrc(context.src);
			ctrl->setViewId(context.id);
			EffectControlManager::getInstance().add(ctrl);

			return true;
		}
		break;

	case EffectType_DestRapidMove:
		{
			EffectContext_RapidMove ec;
			ec.magicId = magicId;
			ec.loops = -1;
			ec.angle = magicAngle;
			ec.owner = context.target;
			ec.tile = context.ptCenter;
			ec.ac = ac;

			EffectControl_RapidMove* ctrl = new EffectControl_RapidMove();
			if (!ctrl->create(ec))
			{
				delete ctrl;
				return false;
			}

			ctrl->setSrc(context.src);
			ctrl->setViewId(context.id);
			EffectControlManager::getInstance().add(ctrl);
			return true;
		}
		break;

	case EffectType_SrcDestRapidMove:
		{
			EffectContext_RapidMove ecSrc;
			ecSrc.magicId = magicId;
			ecSrc.loops = -1;
			ecSrc.angle = magicAngle;
			ecSrc.owner = context.src;
			ecSrc.tile = context.ptTarget;
			ecSrc.ac = ac;

			EffectControl_RapidMove* ctrlSrc = new EffectControl_RapidMove();
			if (!ctrlSrc->create(ecSrc))
			{
				delete ctrlSrc;
				return false;
			}

			ctrlSrc->setSrc(context.src);
			ctrlSrc->setViewId(context.id);
			EffectControlManager::getInstance().add(ctrlSrc);
			//return true;
			//  创建爆炸光效；
			int nExplodeMagicID = getInt(context.id, SkillViewProperty_ExplodeMagicID);
			if (nExplodeMagicID != 0)
			{
				EffectContext_General ec;
				ec.magicId = nExplodeMagicID;
				ec.loops = 1;
				ec.angle = magicAngle;
				//  读取一下爆炸光效是否绑定在目标上；
				int nExplodeMagicAttachToTarget = getInt(context.id, SkillViewProperty_ExplodeMagicAttachToTarget);
				if (nExplodeMagicAttachToTarget == 1)
				{
					ec.owner = context.target;
				}
				else
				{
					ec.owner = 0;
				}
				ec.tile = context.ptTarget;
				EffectControl_General* ctrl = new EffectControl_General();
				if (!ctrl->create(ec))
				{
					delete ctrl;
					return false;
				}
				ctrl->setSrc(context.src);
				ctrl->setViewId(context.id);
				EffectControlManager::getInstance().add(ctrl);
			}

			EffectContext_RapidMove ecDest;
			ecDest.magicId = magicId;
			ecDest.loops = -1;
			ecDest.angle = magicAngle;
			ecDest.owner = context.target;
			ecDest.tile = context.ptCenter;
			ecDest.ac = ac;

			EffectControl_RapidMove* ctrlDest = new EffectControl_RapidMove();
			if (!ctrlDest->create(ecDest))
			{
				delete ctrlDest;
				return false;
			}

			ctrlDest->setSrc(context.src);
			ctrlDest->setViewId(context.id);
			EffectControlManager::getInstance().add(ctrlDest);
			return true;
		}
		break;

		//跳跃类
	case EffectType_Jump:
		{
			EffectContext_RapidMove ec;
			ec.magicId = magicId;
			ec.loops = -1;
			ec.angle = magicAngle;
			ec.owner = context.src;
			ec.tile = context.ptCenter;
			ec.ac = ac;

			EffectControl_Jump* ctrl = new EffectControl_Jump();
			if (!ctrl->create(ec))
			{
				delete ctrl;
				return false;
			}

			ctrl->setSrc(context.src);
			ctrl->setViewId(context.id);
			EffectControlManager::getInstance().add(ctrl);
			return true;
		}
		break;
	}

	return true;
}

void EntityFactory::closeSkillAttackEffect(EntityView* view, ulong viewId)
{
	if (view)
		EffectControlManager::getInstance().remove(view->getHandle(), viewId);
}

void EntityFactory::release()
{
	close();
	delete this;

	//静态变量也是魔鬼!!!
	//把静态鬼变成全局鬼，在Release的时候让它投胎 >_<
	g_bRegistered = false;
}



API_EXPORT IEntityFactory* LoadCEV()
{
	if (!gGlobalClient)
		Error("Parameter error: IGlobalClient object pointer is NULL"<<endl);

	if (!gGlobalClient->getResourceManager() || !gGlobalClient->getSchemeEngine())
		Error("CreateEntityFactory, create order error"<<endl);

	EntityFactory* ef = new EntityFactory();
	if (!ef || !ef->create())
	{
		Error("EntityFactory::create() failed"<<endl);
		return NULL;
	}

	//静态变量也是魔鬼!!!
	//把静态鬼变成全局鬼，在Release的时候让它投胎 >_<
	//static bool registered = false;
	if(!g_bRegistered)
	{
		gGlobalClient->getResourceManager()->registerResourceLoader(typeResourcePic,ResourceLoaderMpw::Instance());
		gGlobalClient->getResourceManager()->registerResourceLoader(typeResourceModel,ResourceLoaderMz::Instance());

        //  测试代码：修改为直接读取csv文件，以便于观察配置文件修改的效果；
		if(!ConfigCreaturePart::Instance()->load("data\\scp\\CreaturePart.csv"))
		{
			Error("load(scp\\CreaturePart.csv) failed"<<endl);
			return NULL;
		}
        //  测试代码：修改为直接读取xml文件，以便于观察配置文件修改的效果；
		//if(!ConfigCreatureRes::Instance()->load("scp\\ConfigCreature.xmc"))
        if(!ConfigCreatureRes::Instance()->load("data\\scp\\ConfigCreature.xml"))
		{
			Error("load(scp\\ConfigCreature.xml) failed"<<endl);
			return NULL;
		}
        //  测试代码：修改为直接读取xml文件，以便于观察配置文件修改的效果；
        //if(!ConfigCreatures::Instance()->load("scp\\Creature.xmc"))
        if(!ConfigCreatures::Instance()->load("data\\scp\\Creature.xml"))
		{
			Error("load(scp\\Creatures.xml) failed"<<endl);
			return NULL;
		}

		
		if( !ConfigActionMap::Instance()->load("data\\scp\\ActionMap.csv"))
		{
			Error("load(scp\\ActionMap.csv) failed"<<endl);
			return NULL;	
		}

		g_bRegistered = true;
	}

	//VisualComponent::mSelectedTexture = gGlobalClient->getRenderSystem()->getTextureManager()->createTextureFromFile("Selected.dds");
	
	return static_cast<IEntityFactory*>(ef);
}

std::string EntityFactory::GetFileNameByCreatureId(long id)
{
	ConfigCreature * pConfigCreature = ConfigCreatures::Instance()->getCreature(id);
	if(pConfigCreature)
		return pConfigCreature->modelName;
	else
		return "";
	//return ConfigCreatureRes::Instance()->getModelFilename(pConfigCreature->modelName);
}

std::string EntityFactory::GetFileNameByResId(long id)
{
	return   ConfigCreatureRes::Instance()->getResFromId(id);
}

void EntityFactory::CreateVibrate(ulong ulViewID)
{
	ulong ulVibrate = getInt(ulViewID, SkillViewProperty_Vibrate);
	if (ulVibrate > 0)
	{
		gGlobalClient->getSceneManager()->vibrate(ulVibrate);
	}
}

void EntityFactory::PreLoadSkillEffect(int nViewID)
{
	 int nAttackMagicID = getInt(nViewID, SkillViewProperty_AttackMagicId);
	 if (nAttackMagicID)
	 {
		loadRes(nAttackMagicID);
	 }
	 int nPrepareMagicID = getInt(nViewID, SkillViewProperty_AttackReadyMagicId);
	 if (nPrepareMagicID)
	 {
		 loadRes(nPrepareMagicID);
	 }
	 int nTrackMagicID = getInt(nViewID, SkillViewProperty_TrackMagicId);
	 if (nTrackMagicID)
	 {
		 loadRes(nTrackMagicID);
	 }
	 int nHitMagicID = getInt(nViewID, SkillViewProperty_BeHitMagicId);
	 if (nHitMagicID)
	 {
		 loadRes(nHitMagicID);
	 }
}

void EntityFactory::loadRes(int nMagicID)
{
	int resID =  MagicDataManager::getInstance().getValue(nMagicID, MagicProperty_ResId).getInt();
	MagicView *pMagic = new MagicView();
	pMagic->setResId(resID);
	pMagic->preLoadRes();
}

int EntityFactory::GetAnimationTime(const RequestAnimationContext& context)
{
	EntityView* pSourceEntityView = (EntityView*)getHandleData(context.src);
	if ((pSourceEntityView == NULL) || (context.id == 0))
	{
		ASSERT(false);
		return false;
	}

	// 取得配置的多个攻击动作中随机的一个
	int nActionId;
	const IntArray & AttackActId = getIntArray(context.id, SkillViewProperty_AttackActId);
	int nCount = AttackActId.count;

	if (nCount == 1)
	{
		nActionId = AttackActId.data[0];
	}
	else if (nCount > 1)
	{
		int nIndex = getRand(1, nCount);
		nActionId = AttackActId.data[nIndex - 1];
	}
	else
	{
		nActionId = 0;
	}

	//if (nActionId == )
	//{
	//}

	int nTicks = 0;
	pSourceEntityView->onCommand(EntityCommand_GetAnimationTime, (ulong)&nTicks, nActionId);

	return nTicks;
}
uint EntityFactory::GetSkillViewDataId(uint array, uint set)
{
	return SkillViewDataManager::getInstancePtr()->getSkillViewDataId(array, set);
}

const std::string& EntityFactory::GetSkillViewDataName(uint id)
{
	return SkillViewDataManager::getInstancePtr()->getSkillViewDataName(id);
}

bool EntityFactory::GetSkillViewDataArrayIsValid(uint array)
{
	return SkillViewDataManager::getInstancePtr()->getSkillViewDataArrayIsValid(array);
}

bool EntityFactory::GetSkillViewDataRowIsValid(uint array, uint set)
{
	return SkillViewDataManager::getInstancePtr()->getSkillViewDataRowIsValid(array, set);
}

uint EntityFactory::GetMagicDataId(uint array, uint set)
{
	return MagicDataManager::getInstancePtr()->getMagicDataId(array, set);
}

const std::string& EntityFactory::GetMagicDataName(uint id)
{
	return MagicDataManager::getInstancePtr()->getMagicDataName(id);
}

bool EntityFactory::GetMagicDataArrayIsValid(uint array)
{
	return MagicDataManager::getInstancePtr()->getMagicDataArrayIsValid(array);
}

bool EntityFactory::GetMagicDataRowIsValid(uint array, uint set)
{
	return MagicDataManager::getInstancePtr()->getMagicDataRowIsValid(array, set);
}

const uint EntityFactory::GetConfigCreatureResMaxId()
{
	return ConfigCreatureRes::Instance()->getMaxId();
}

const uint EntityFactory::GetConfigCreatureResId(uint id)
{
	return ConfigCreatureRes::Instance()->getResId(id);
}

const std::string EntityFactory::GetConfigCreatureResName(uint id)
{
	return ConfigCreatureRes::Instance()->getEffectName(id);
}

const std::string EntityFactory::GetSkillViewAttackAct(uint id)
{
	IntArray AttackActId = getIntArray(id, SkillViewProperty_AttackActId);
	std::string str = AttackActId.intArrayToString();
	return str;
}

const uint EntityFactory::GetSkillViewSwing(uint id)
{
	return getInt(id, SkillViewProperty_Vibrate);
}

const float EntityFactory::GetSkillViewFlySpeed(uint id)
{
	return getFloat(id, SkillViewProperty_FlySpeed);
}

const uint EntityFactory::GetSkillViewEmitEffect(uint id)
{
	return getInt(id, SkillViewProperty_AttackMagicId);
}

const uint EntityFactory::GetSkillViewBlastEffect(uint id)
{
	return getInt(id, SkillViewProperty_ExplodeMagicID);
}

const uint EntityFactory::GetSkillViewHitEffect(uint id)
{
	return getInt(id, SkillViewProperty_BeHitMagicId);
}

const uint EntityFactory::GetSkillViewFlyEffect(uint id)
{
	return getInt(id, SkillViewProperty_TrackMagicId);
}

const uint EntityFactory::GetSkillViewHitFlyEffect(uint id)
{
	return getInt(id, SkillViewProperty_BeHitFlyMagicId);
}

const uint EntityFactory::GetSkillViewReadyEffect(uint id)
{
	return getInt(id, SkillViewProperty_AttackReadyMagicId);
}

void EntityFactory::SetSkillViewData(uint id, std::string attackAct, uint swing, float flySpeed, 
					  uint emitEffect, uint blastEffect, uint hitEffect, uint flyEffect,
					  uint hitFlyEffect, uint readyEffect)
{
	SkillViewDataManager::getInstance().SetSkillViewData(id, attackAct, swing, 
		flySpeed, emitEffect, blastEffect, hitEffect, flyEffect, hitFlyEffect, readyEffect);
}

void EntityFactory::SetMagicData(uint id, float aniSpeed, uint aniTrans, std::string aniOffset,
				  float aniScale, uint resId)
{
	MagicDataManager::getInstance().SetMagicData(id, aniSpeed, aniTrans, aniOffset, aniScale, resId);
}

void EntityFactory::SaveMagicDataToCsv(const char* szFileName)
{
	MagicDataManager::getInstance().SaveMagicDataToCsv(szFileName);
}

void EntityFactory::SaveSkillViewDataToCsv(const char* szFileName)
{
	SkillViewDataManager::getInstance().SaveSkillViewDataToCsv(szFileName);
}

