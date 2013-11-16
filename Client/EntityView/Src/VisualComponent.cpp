//==========================================================================
/**
* @file	  : VisualComponent.cpp
* @author : 
* created : 2008-1-30   12:57
*/
//==========================================================================

#include "stdafx.h"
#include "VisualComponent.h"
#include "IChatClient.h"
#include "ITeamClient.h"
#include "IEntityFactory.h"


#if defined(RELEASEDEBUG) || defined(_DEBUG)
#define SHOW_BOUNDBOX
#endif


//ITexture* VisualComponent::mSelectedTexture = 0;

//  任务标记使用的贴图的文件名；
//crr 对于文件的查找，layout文件没有写data\\UI\\image\\的前缀，通过额外的查找data\\UI\\image\\目录可以定位到
//打包后，整个data\\UI\\image\\被打包成单个的mpk，在这个mpk内不能有data\\UI\\image\\前缀
//故这里去掉前缀
//const std::string TASK_SIGN_TEXTURE_FILE_NAME = "data\\UI\\image\\TaskListFrame.dds";
const std::string TASK_SIGN_TEXTURE_FILE_NAME = "Common.tga";

const std::string EFFECT_FINISH_TASK = "data\\Model\\Common\\Effect\\完成任务\\完成任务.TX";
const std::string EFFECT_AUTOMOVE = "data\\Model\\Common\\Effect\\自动寻路中\\自动寻路中.TX";
const std::string EFFECT_FINISH_AUTOMOVE = "data\\Model\\Common\\Effect\\已到达目的地\\已到达目的地.TX";

//  任务标记的小图标
const std::string TASK_IMAGESET_NAME = "Common";
const std::string TASK_ACCEPTABLE_IMAGENAME = "Task_HaveTaskIcon_Gold";
const std::string TASK_ACCEPTABLE_REPE_IMAGENAME = "Task_HaveCycle_Icon"; 
const std::string TASK_ACCOMPLISH_IMAGENAME = "Task_CanReportIcon_Gold";
const std::string TASK_ACCOMPLISH_REPE_IMAGENAME = "Task_ReportCycle_Icon"; 
const std::string TASK_UNFINISH_IMAGENAME = "Task_UnReportIcon_Gold";

// 回调
class VisualComponentCallBack: public IAnimationCallback
{
public:
	virtual void onOneCycle(const std::string& animation,int loopTimes)
	{
		if( loopTimes == 0 && m_pNode && m_pEntityView)
		{

			ModelInstance * inst = m_pNode->getModelInstance();
			if (inst)
				m_pNode->getModelInstance()->setCallback(0);
			if (m_pEntityView->hasFlag(flagDrawFinishTaskEffect))
					m_pEntityView->removeFlag(flagDrawFinishTaskEffect);
			if (m_pEntityView->hasFlag(flagDrawFinishAutoMoveEffect))
					m_pEntityView->removeFlag(flagDrawFinishAutoMoveEffect);
			
			delete this;
		}
	}

	VisualComponentCallBack(ModelNode * pNode,EntityView* pEntityView)
	{
		Assert( pNode);
		m_pNode = pNode;
		Assert(pEntityView);
		m_pEntityView = pEntityView;
	}
private:
	ModelNode * m_pNode;
	EntityView* m_pEntityView;
};
VisualComponent::VisualComponent() 
	: mHandle(INVALID_HANDLE), mResLoaded(false), mNameOffset(100.f), mUpdateOption(0), 
	mAlpha(1.f),m_foffset(10.f),m_bRset(false),mTextEffectTickCount(0)
{
	//SetRectEmpty(&mShowRect);
	mShowRect.left = mShowRect.right = mShowRect.top = mShowRect.bottom = 0;

	mCurHP = mMaxHP = mCurMP = mMaxMP = 100;
	mNameColor = ColorValue(1,1,1,1);

    //  这里暂时设置为走动，因为现有实现中，部分生物没有提供“Run”的动画；
    //  注意：这里对mIsRun的设置需要与对VisualComponentMZ::m_ulMoveSpeed的设置相匹配；
    mIsRun = true;

	// 走路初始设置最大速度
	m_ulInitWalkSpeed = 0;

	// 走路初始设置最大跑步速度
	m_ulInitRunSpeed = 0;

    m_CurrentTaskSign = ETS_LOWWER_BOUND;

	m_pHPTexture = NULL;
	//m_pHPImageSet = NULL;

	m_pTaskSignTexture = NULL;

	m_pTeamTexture = NULL; 
	//m_pTeamImageSet = NULL;

	m_pIconTexture = 0;
	//m_pIconImageSet = 0;

	// 创建特效模型
	m_pFinshTaskEffect = NULL;
    m_pAutoMoveEffect = NULL ;
    m_pFinshAutoMoveEffect = NULL; 
}

VisualComponent::~VisualComponent()
{
	safeRelease( m_pHPTexture);
	safeRelease(m_pTaskSignTexture);
	safeRelease(m_pTeamTexture);
	safeRelease(m_pIconTexture);
	safeRelease(m_pFinshTaskEffect);
	safeRelease(m_pAutoMoveEffect);
	safeRelease(m_pFinshAutoMoveEffect);
}

void VisualComponent::updateWorldCoord()
{
	xs::Point pt;
	gGlobalClient->getSceneManager()->tile2World(getOwner()->getTile(), pt);
	getOwner()->setWorld(pt);
}

void VisualComponent::onAddEntity()
{
	if (getOwner()->hasFlag(flagFade))
		setFadeIn();
}

void VisualComponent::onRemoveEntity()
{
	if (isValidHandle(mHandle))
	{
		if (getOwner()->hasFlag(flagFade/*|flagInScreen*/))
			setFadeOut();
	}
}

void VisualComponent::setFadeIn()
{
	if ((mUpdateOption & updateFadeIn) == 0)
	{
		if (mUpdateOption & updateFadeOut)
			mUpdateOption &= ~updateFadeOut;
		else
			mAlpha = 0.f;
		mUpdateOption |= updateFadeIn;
	}
}
void VisualComponent::setFadeOut()
{
	if ((mUpdateOption & updateFadeOut) == 0)
	{
		getOwner()->removeFlag(flagSelected);
		getOwner()->removeFlag(flagHighlight);
		if (mUpdateOption & updateFadeIn)
			mUpdateOption &= ~updateFadeIn;
		else if(mUpdateOption & updatePetdie)
			mUpdateOption &= ~updatePetdie;//wan
		else
			mAlpha = 1.f;
		mUpdateOption |= updateFadeOut;
	}
}
//设置宠物死亡
void VisualComponent::setFadeDie()
{
	if ((mUpdateOption&updatePetdie) == 0)
	{
		mUpdateOption |= updatePetdie;
	}
}

bool VisualComponent::update(float tick, float deltaTick, IRenderSystem* pRenderSystem)
{	
	EntityView* entity = getOwner();
	ulong flags = entity->getFlag();

	if( flags & flagDrawHP)
	{
		//crr 对于文件的查找，layout文件没有写data\\UI\\image\\的前缀，通过额外的查找data\\UI\\image\\目录可以定位到
		//打包后，整个data\\UI\\image\\被打包成单个的mpk，在这个mpk内不能有data\\UI\\image\\前缀
		if( 0 == m_pHPTexture && pRenderSystem)
			m_pHPTexture = gGlobalClient->getRenderSystem()->getTextureManager()->createTextureFromFile("Common.tga");
		
		//if( 0 == m_pHPImageSet)
		//	m_pHPImageSet = xsgui::ImagesetManager::getSingleton().getImageset("Common");	
	}

	if( flags & flagDrawTextGroup)
	{
		//crr 对于文件的查找，layout文件没有写data\\UI\\image\\的前缀，通过额外的查找data\\UI\\image\\目录可以定位到
		//打包后，整个data\\UI\\image\\被打包成单个的mpk，在这个mpk内不能有data\\UI\\image\\前缀
		//故这里去掉前缀
		//ITexture * pIconTexture = gGlobalClient->getRenderSystem()->getTextureManager()->createTextureFromFile("data\\UI\\image\\main3-1.dds");
		if( 0 == m_pIconTexture && pRenderSystem)
			m_pIconTexture = pRenderSystem->getTextureManager()->createTextureFromFile("Common2.tga");
		
		//if( 0 == m_pIconImageSet)
		//	m_pIconImageSet = xsgui::ImagesetManager::getSingleton().getImageset("Common2");	
	}

	//队伍
	if (flags & flagDrawTeam)
	{
		if( 0 == m_pTeamTexture && pRenderSystem)
			m_pTeamTexture = pRenderSystem->getTextureManager()->createTextureFromFile("Common.tga");
		//if( 0 ==  m_pTeamImageSet )
		//	m_pTeamImageSet = xsgui::ImagesetManager::getSingleton().getImageset("Common");	
	}
	//  绘制任务标记；
	IEntity* pEntity = (IEntity*)getOwner()->getUserData();
	if (pEntity == NULL)
	{
		//ASSERT(false);
		return false;
	}
	ITaskClient* pTaskClient = gGlobalClient->GetTaskClient();
	if (pTaskClient == NULL)
	{
		return false;
	}
	m_CurrentTaskSign = pTaskClient->GetEntityTaskSign(pEntity->GetUID());
	if (m_CurrentTaskSign > ETS_NoTaskSign)
	{
		entity->addFlag(flagDrawTaskIcon);
	}
	else
	{
		entity->removeFlag(flagDrawTaskIcon);
	}
	//任务
	if(flags & flagDrawTaskIcon)
	{
		if( 0 == m_pTaskSignTexture && pRenderSystem)
			m_pTaskSignTexture = pRenderSystem->getTextureManager()->createTextureFromFile(TASK_SIGN_TEXTURE_FILE_NAME);
	}

	// 三个特效
	if(flags & flagDrawFinishTaskEffect)
	{
		if(NULL == m_pFinshTaskEffect && pRenderSystem)
		{
			m_pFinshTaskEffect = ModelNodeCreater::create(EFFECT_FINISH_TASK.c_str());
			ModelInstance * inst = m_pFinshTaskEffect->getModelInstance();
			if (inst)
			{
				inst->setCallback(new VisualComponentCallBack(m_pFinshTaskEffect,getOwner()));
				inst->setCurrentAnimation(inst->getCurrentAnimation(),1);
			}
		}
		else
		{
			m_pFinshTaskEffect->update(tick,deltaTick,pRenderSystem);
		}
	}
	else
	{
		if (m_pFinshTaskEffect != NULL)
		{
			safeRelease(m_pFinshTaskEffect);
		}
	}
	if (flags & flagDrawAutoMoveEffect)
	{
		if(NULL == m_pAutoMoveEffect && pRenderSystem)
		{
			m_pAutoMoveEffect = ModelNodeCreater::create(EFFECT_AUTOMOVE.c_str());
			ModelInstance * inst = m_pAutoMoveEffect->getModelInstance();
			if (inst)
			{
				inst->setCallback(new VisualComponentCallBack(m_pAutoMoveEffect,getOwner()));
				inst->setCurrentAnimation(inst->getCurrentAnimation(),-1);
			}
		}
		else
		{
			m_pAutoMoveEffect->update(tick,deltaTick,pRenderSystem);
		}
	}
	else
	{
		if (m_pAutoMoveEffect != NULL)
		{
			//m_pAutoMoveEffect->setVisible(false);
			safeRelease(m_pAutoMoveEffect);
		}
	}
	if (flags & flagDrawFinishAutoMoveEffect)
	{
		if(NULL == m_pFinshAutoMoveEffect && pRenderSystem)
		{
			m_pFinshAutoMoveEffect = ModelNodeCreater::create(EFFECT_FINISH_AUTOMOVE.c_str());
			ModelInstance * inst = m_pFinshAutoMoveEffect->getModelInstance();
			if (inst)
			{
				inst->setCallback(new VisualComponentCallBack(m_pFinshAutoMoveEffect,getOwner()));
				inst->setCurrentAnimation(inst->getCurrentAnimation(),1);
			}
		}
		else
		{
			m_pFinshAutoMoveEffect->update(tick,deltaTick,pRenderSystem);
		}
	}
	else
	{
		if (m_pFinshAutoMoveEffect != NULL)
		{
			safeRelease(m_pFinshAutoMoveEffect);
		}
	}
	return true;
}

void VisualComponent::drawTopMost(IRenderSystem* pRenderSystem)
{
	EntityView* entity = getOwner();
	ulong flags = entity->getFlag();

	xs::Point ptScreen;
	gGlobalClient->getSceneManager()->world2Screen(getOwner()->getWorld(), ptScreen);
	xs::Rect rc = entity->getShowRect();
	::OffsetRect(&rc, ptScreen.x, ptScreen.y);
	Rect rect;
	rect.left = rc.left;
	rect.top = rc.top;
	rect.right = rc.right - 1;
	rect.bottom = rc.bottom - 1;

#ifdef SHOW_BOUNDBOX
	//pRenderSystem->rectangle(rect, ColorValue(0, 1, 0, 1));
#endif

	int yOffset = rect.top;

	
	// 从下往上绘制
	if (flags & flagSelectedForMapEdit)
	{
		PP_BY_NAME("VisualComponent::drawTopMost::drawFlagSelectedForMapEdit");
		pRenderSystem->rectangle(rect, ColorValue(0,1,0,1));
	}

	// 资源id	
	if ((flags & flagDrawResId))
	{
		PP_BY_NAME("VisualComponent::drawTopMost::drawFlagDrawResId");
		yOffset -= (int)gGlobalClient->getFont()->getLineHeight();
		std::string str;
		StringHelper::fromInt(str, entity->getResId());
		AABB aabb;
		gGlobalClient->getFont()->getAABB(str.c_str(), aabb);
		int w = aabb.getMaximum().x - aabb.getMinimum().x;
		if (w < 0) w = -w;
		gGlobalClient->getFont()->render2d(ptScreen.x-(w/2),yOffset,1,ColorValue(0,1,0,mAlpha),str.c_str());
	}

	// 称号
	if ((flags & flagDrawTitle) && !mTitleName.empty())
	{
		PP_BY_NAME("VisualComponent::drawTopMost::drawFlagDrawTitle");
		yOffset -= (int)gGlobalClient->getFont()->getLineHeight();
		AABB aabb;
		gGlobalClient->getFont()->getAABB(mTitleName.c_wstr(), aabb);
		int w = aabb.getMaximum().x - aabb.getMinimum().x;
		if (w < 0) w = -w;
		mNameColor.a = mAlpha;
		gGlobalClient->getFont()->render2d(ptScreen.x-(w/2),yOffset,1,mNameColor,mTitleName.c_wstr());
	}

	const ulong HPMPINFO_W = 120;
	const ulong HPMPINFO_H = 6;

	// 魔法值
	if (flags & flagDrawMP)
	{
		PP_BY_NAME("VisualComponent::drawTopMost::drawFlagDrawMP");
		yOffset -= HPMPINFO_H;
		Rect rc;
		rc.left = ptScreen.x - HPMPINFO_W/2;
		rc.top = yOffset;
		rc.right = rc.left + HPMPINFO_W - 1;
		rc.bottom = rc.top + HPMPINFO_H - 1;

		pRenderSystem->rectangle(rc, ColorValue(1,1,1,mAlpha));

		Rect rcLeft;
		rcLeft.left = rc.left + 1;
		rcLeft.top = rc.top + 1;
		rcLeft.right = rcLeft.left + (int)(mCurMP * (float)(HPMPINFO_W - 2) / mMaxMP) - 1;
		rcLeft.bottom = rc.bottom - 1;

		pRenderSystem->box(rcLeft, ColorValue(0,0,1,mAlpha));
	}

	// 血条	
	if ( (flags & flagDrawHP)
		/*&& m_pHPImageSet
		&& m_pHPImageSet*/ )
	{
		PP_BY_NAME("VisualComponent::drawTopMost::drawFlagDrawHP");

		/*
		const xsgui::Image &pHPImage = m_pHPImageSet->getImage("HpBg");

		int nImageWidth = pHPImage.getWidth();
		int nImageHeigh = pHPImage.getHeight();

		yOffset -= nImageHeigh;

		// 血条
		Rect rcHpLeft;		
		rcHpLeft.left = ptScreen.x - (nImageWidth - 14)/2;
		rcHpLeft.top = yOffset + 5;
		rcHpLeft.right = rcHpLeft.left + (int)(mCurHP * (float)(nImageWidth - 14) / mMaxHP);
		rcHpLeft.bottom = rcHpLeft.top + nImageHeigh - 9; 
		rcHpLeft.left--; 
		if (rcHpLeft.right <= rcHpLeft.left)
		{
			rcHpLeft.right = rcHpLeft.left;
		}
		pRenderSystem->box(rcHpLeft, ColorValue(0.6,0.2,0,mAlpha));

		//rcHpLeft.right = rcHpLeft.left + nImageWidth - 11;
		//rcHpLeft.top -= 30;
		//rcHpLeft.bottom = rcHpLeft.top + nImageHeigh - 10;
		//pRenderSystem->rectangle(rcHpLeft,ColorValue(0,0,1,mAlpha));

		// 血条外框
		Rect rcLeft;
		rcLeft.left = ptScreen.x - nImageWidth/2;
		rcLeft.top = yOffset;
		rcLeft.right = rcLeft.left + nImageWidth ;
		rcLeft.bottom =  rcLeft.top + nImageHeigh; 

		const xsgui::Rect & subrect = pHPImage.getSourceTextureArea();
		float height =static_cast<float> (m_pHPImageSet->getTexture()->getHeight());
		float width =static_cast<float>( m_pHPImageSet->getTexture()->getWidth());
		xs::Vector2 lefttop( subrect.d_left/ width, subrect.d_top/height);
		xs::Vector2 rightop( subrect.d_right/width, subrect.d_top/height);
		xs::Vector2 rightbottom(subrect.d_right/width, subrect.d_bottom/height);
		xs::Vector2 leftbottom(subrect.d_left/width, subrect.d_bottom/height);

		pRenderSystem->rectangle(rcLeft, m_pHPTexture,lefttop, leftbottom,rightbottom,rightop);
		*/
	}

	// 摊位标识
	if (flags & flagDrawStall)
	{
		yOffset -= (int)gGlobalClient->getFont()->getLineHeight();
		AABB aabb;
		gGlobalClient->getFont()->getAABB("自己的摊位", aabb);

		ColorValue color(0.82f,0.49f,0.05f,1); // 橙色
		int w = aabb.getMaximum().x - aabb.getMinimum().x;
		if (w < 0) w = -w;
		color.a = mAlpha;

		gGlobalClient->getFont()->render2d(ptScreen.x-(w/2),yOffset,1,color,"自己的摊位");
	}

	// 名字	
	if ((flags & flagDrawName) && !mName.empty())
	{
		PP_BY_NAME("VisualComponent::drawTopMost::drawFlagDrawName");
		yOffset -= (int)gGlobalClient->getFont()->getLineHeight();
		AABB aabb;
		gGlobalClient->getFont()->getAABB(mName.c_wstr(), aabb);
		int w = aabb.getMaximum().x - aabb.getMinimum().x;
		if (w < 0) w = -w;
		mNameColor.a = mAlpha;
		gGlobalClient->getFont()->render2d(ptScreen.x-(w/2),yOffset,1,mNameColor,mName.c_wstr());
	}

	// 文字组
	if ((flags & flagDrawTextGroup)
		&& !m_listText.empty()
		&& m_pIconTexture 
		/*&& m_pIconImageSet*/ )
	{
		PP_BY_NAME("VisualComponent::drawTopMost::drawFlagDrawTextGroup");
		list<TextLine>::iterator it = m_listText.begin();
		for(;it != m_listText.end(); ++ it)
		{
			// 小图标 
			if( !(*it).txPicture.empty() )
			{
				/*
				const xsgui::Image  pImage = m_pIconImageSet->getImage((*it).txPicture);
				const xsgui::Rect & subrect = pImage.getSourceTextureArea();
				float width = static_cast<float> (m_pIconImageSet->getTexture()->getHeight());
				float height = static_cast<float>( m_pIconImageSet->getTexture()->getWidth());

				xs::Vector2 lefttop( subrect.d_left/ width, subrect.d_top/height);
				xs::Vector2 rightop( subrect.d_right/width, subrect.d_top/height);
				xs::Vector2 rightbottom(subrect.d_right/width, subrect.d_bottom/height);
				xs::Vector2 leftbottom(subrect.d_left/width, subrect.d_bottom/height);

				Rect rectIcon;
				rectIcon.left = ptScreen.x - HPMPINFO_W/2 - pImage.getWidth()/2;
				rectIcon.top  = yOffset;
				rectIcon.right = rectIcon.left + pImage.getWidth();
				rectIcon.bottom = rectIcon.top + pImage.getHeight();

				pRenderSystem->rectangle(rectIcon, m_pIconTexture,lefttop, leftbottom,rightbottom,rightop );
				*/
			}

			if(!(*it).txStr.empty())
			{
				yOffset -= (int)gGlobalClient->getFont((*it).txFont)->getLineHeight();
				AABB aabb;
				gGlobalClient->getFont((*it).txFont)->getAABB((*it).txStr.c_wstr(), aabb);
				int w = aabb.getMaximum().x - aabb.getMinimum().x;
				if (w < 0) w = -w;	
				(*it).txColor.a = mAlpha;

				gGlobalClient->getFont((*it).txFont)->render2d(ptScreen.x-(w/2),yOffset,1,(*it).txColor,(*it).txStr.c_wstr());

				// 特效，测试代码
				if( (*it).txEffectID)
				{
					IEntityFactory * pEntityFactory = gGlobalClient->getEntityFactory();

					if(pEntityFactory != NULL)
					{		
						long curTick = GetTickCount();
						if( curTick - mTextEffectTickCount > 5 * 1000)
						{
							xs::Point ptTile,ptScreenEffect;
							ptScreenEffect.x = 	ptScreen.x;
							ptScreenEffect.y = yOffset;
							gGlobalClient->getSceneManager()->screen2Tile(ptScreenEffect,ptTile);		
							pEntityFactory->createEffect((*it).txEffectID,ptTile);
							mTextEffectTickCount = curTick;
						}
					}	
				}
			}
		}
	}

    // 绘制个BOX 
    IEntity* pEntity = (IEntity*)getOwner()->getUserData();
    if (pEntity == NULL)
    {
        //ASSERT(false);
        return;
    }

	// 绘制队伍图标
	/*
    ITeamClient* pTeamClient = gGlobalClient->getTeamClient();
	//ASSERT(pTeamClient != NULL);
	if (pTeamClient && m_pTeamTexture && m_pTeamImageSet)
	{
		if (pEntity && pEntity->GetEntityView()&& pEntity->GetEntityClass()->IsPerson())
		{
			DWORD dwPDBID = pEntity->GetNumProp(CREATURE_PROP_PDBID);
			if(pTeamClient->IsTeammate(dwPDBID))
			{
				PP_BY_NAME("VisualComponent::drawTopMost::drawFlagTeam");
				if (pTeamClient->GetTeamCaptainUID() == dwPDBID)
				{
					const xsgui::Image &pTeamCaptainImage = m_pTeamImageSet->getImage("MoveEndPoint");
					int nImageWidth = pTeamCaptainImage.getWidth();
					int nImageHeigh = pTeamCaptainImage.getHeight();

					yOffset -= nImageHeigh;

					Rect rcLeft;
					rcLeft.left = ptScreen.x - nImageWidth/2;
					rcLeft.top = yOffset;
					rcLeft.right = rcLeft.left + nImageWidth;
					rcLeft.bottom =  rcLeft.top + nImageHeigh; 

					const xsgui::Rect & subrect = pTeamCaptainImage.getSourceTextureArea();
					float height =static_cast<float> (m_pTeamImageSet->getTexture()->getHeight());
					float width =static_cast<float>( m_pTeamImageSet->getTexture()->getWidth());
					xs::Vector2 lefttop( subrect.d_left/ width, subrect.d_top/height);
					xs::Vector2 rightop( subrect.d_right/width, subrect.d_top/height);
					xs::Vector2 rightbottom(subrect.d_right/width, subrect.d_bottom/height);
					xs::Vector2 leftbottom(subrect.d_left/width, subrect.d_bottom/height);

					pRenderSystem->rectangle(rcLeft, m_pTeamTexture,lefttop, leftbottom,rightbottom,rightop);
				}
				else
				{
					const xsgui::Image &pTeamImage = m_pTeamImageSet->getImage("TeamMember_Icon");
					int nImageWidth = pTeamImage.getWidth();
					int nImageHeigh = pTeamImage.getHeight();

					yOffset -= nImageHeigh;

					Rect rcLeft;
					rcLeft.left = ptScreen.x - nImageWidth/2;
					rcLeft.top = yOffset;
					rcLeft.right = rcLeft.left + nImageWidth;
					rcLeft.bottom =  rcLeft.top + nImageHeigh; 

					const xsgui::Rect & subrect = pTeamImage.getSourceTextureArea();
					float height =static_cast<float> (m_pTeamImageSet->getTexture()->getHeight());
					float width =static_cast<float>( m_pTeamImageSet->getTexture()->getWidth());
					xs::Vector2 lefttop( subrect.d_left/ width, subrect.d_top/height);
					xs::Vector2 rightop( subrect.d_right/width, subrect.d_top/height);
					xs::Vector2 rightbottom(subrect.d_right/width, subrect.d_bottom/height);
					xs::Vector2 leftbottom(subrect.d_left/width, subrect.d_bottom/height);

					pRenderSystem->rectangle(rcLeft, m_pTeamTexture,lefttop, leftbottom,rightbottom,rightop);
				}
			}
		}
	}
	*/

	// 绘制聊天泡泡
	IChatClient* pChatClient = gGlobalClient->getChatClient();
	//ASSERT(pChatClient != NULL);
	if (pChatClient)
	{	
		if (pEntity && pEntity->GetEntityView())
		{
			PP_BY_NAME("VisualComponent::drawTopMost::drawPaoPao");

			xs::Point pos = pEntity->GetEntityView()->getWorld();
			yOffset -= 5;
			pos.y = yOffset;
			// 得到相对于实体视图的 屏幕位子
			xs::Point ptPlayerPos;
			gGlobalClient->getSceneManager()->world2Screen(pos,ptPlayerPos);
			ptPlayerPos.x -= 60;
			//ptPlayerPos.y -= 100;
			pChatClient->UpdatePaoPaoPlace(ptPlayerPos.x,yOffset,pEntity->GetUID());
		}
	}
}

bool VisualComponent::handleCommand(ulong cmd, ulong param1, ulong param2)
{
	switch (cmd)
	{
	case EntityCommand_SetName:
		{
			const char* name = (const char*)param1;
			size_t len = (size_t)param2;
			Assert(name != NULL);
			if (name && len > 0)
			{
				mName = std::string(name, len);
				return true;
			}
			return false;	
		}
		break;

	case EntityCommand_GetName:
		*(std::string*)param1 = mName;
		break;

	case EntityCommand_SetNameColor:
		mNameColor = *(const ColorValue*)param1;
		break;

	case EntityCommand_GetNameColor:
		*(ColorValue*)param1 = mNameColor;
		break;

	case EntityCommand_SetHPInfo:
		mCurHP = param1;
		mMaxHP = param2;
		Assert(mCurHP <= mMaxHP);
		break;
	case EntityCommand_SetMPInfo:
		mCurMP = param1;
		mMaxMP = param2;
		Assert(mCurMP <= mMaxMP);
		break;
	case EntityCommand_SetMoveStyle:
		{
			bool old = mIsRun;
			mIsRun = (param1 == EMoveStyle_Run);
			if (mIsRun != old)
			{
				getOwner()->onMessage(msgMoveStyleChanged);
			}
		}
		break;
	case EntityCommand_SetDefaultSpeed:
		{
			m_ulInitRunSpeed = param1;
			m_ulInitWalkSpeed = param2;
		}
		break;
	case EntityCommand_GetMoveStyle:
		*(ulong*)param1 = mIsRun ? 1 : 0;
		break;

    case EntityCommand_SetTaskSign:
        {
            EEntityTaskSign eNewTaskSign = (EEntityTaskSign)param1;
            if (m_CurrentTaskSign != eNewTaskSign)
            {
                m_CurrentTaskSign = eNewTaskSign;
            }
        }
        break;

	case EntityCommand_SetTitle:
		{
			const char* name = (const char*)param1;
			size_t len = (size_t)param2;
			Assert(name != NULL);
			if (name && len > 0)
			{
				mTitleName = std::string(name, len);
				return true;
			}
			return false;	
		}
		break;

	case EntityCommand_AddGroupText:
		{
			TextLine * text = (TextLine *)param1;

			Assert(text != NULL);
			if (text)
			{
				m_listText.push_back(*text);
				return true;
			}
			return false;	
		}
		break;

	case EntityCommand_ClearGroupText:
		{
			m_listText.clear();			
			return true;	
		}
		break;

	default:
		return false;
	}

	return true;
}

void VisualComponent::handleMessage(ulong msgId, ulong param1, ulong param2)
{
	switch (msgId)
	{
	case msgPosChanged:
		{
			mUpdateOption |= updatePosition;

			xs::Point pt;
			gGlobalClient->getSceneManager()->space2World(getOwner()->getSpace(), pt);
			getOwner()->setWorld(pt);
		}
		break;
	}
}

void VisualComponent::drawAlwayseTopMost(IRenderSystem* pRenderSystem)
{
	EntityView* entity = getOwner();
	ulong flags = entity->getFlag();

	xs::Point ptScreen;
	gGlobalClient->getSceneManager()->world2Screen(getOwner()->getWorld(), ptScreen);
	xs::Rect rc = entity->getShowRect();
	::OffsetRect(&rc, ptScreen.x, ptScreen.y);

	int yOffset = rc.top;

	// 从下往上绘制
	if ((flags & flagDrawName) && !mName.empty())
	{
		yOffset -= (int)gGlobalClient->getFont()->getLineHeight();
	}

	if ((flags & flagDrawResId))
	{
		yOffset -= (int)gGlobalClient->getFont()->getLineHeight();
	}

	const ulong HPMPINFO_H = 6;
	if (flags & flagDrawMP)
	{
		yOffset -= HPMPINFO_H;
	}

	if (flags & flagDrawHP)
	{
		yOffset -= HPMPINFO_H;	
	}

	// 文字组
	if (flags & flagDrawTextGroup)
	{
		yOffset -= (int)gGlobalClient->getFont()->getLineHeight() * m_listText.size();
	}

	////  绘制任务标记；
	//IEntity* pEntity = (IEntity*)getOwner()->getUserData();
	//if (pEntity == NULL)
	//{
	//	//ASSERT(false);
	//	return;
	//}
	//ITaskClient* pTaskClient = gGlobalClient->GetTaskClient();
	//if (pTaskClient == NULL)
	//{
	//	ASSERT(false);
	//	return;
	//}
	//m_CurrentTaskSign = pTaskClient->GetEntityTaskSign(pEntity->GetUID());
	if ((m_CurrentTaskSign > ETS_LOWWER_BOUND) && (m_CurrentTaskSign < ETS_UPPER_BOUND))
	{
		if (m_CurrentTaskSign > ETS_NoTaskSign)
		{
			//  获得图标
			 /*
			 xsgui::Imageset * pImageSet = xsgui::ImagesetManager::getSingleton().getImageset(TASK_IMAGESET_NAME);
			 const xsgui::Image &pImageTaskAcceptable = pImageSet->getImage(TASK_ACCEPTABLE_IMAGENAME);
			 const xsgui::Image &pImageTaskAcceptableRepe = pImageSet->getImage(TASK_ACCEPTABLE_REPE_IMAGENAME);
			 const xsgui::Image &pImageTaskAccomplish = pImageSet->getImage(TASK_ACCOMPLISH_IMAGENAME);
			 const xsgui::Image &pImageTaskAccomplishRepe = pImageSet->getImage(TASK_ACCOMPLISH_REPE_IMAGENAME);
			 const xsgui::Image &pImageTaskUnfinish = pImageSet->getImage(TASK_UNFINISH_IMAGENAME);

			if( pImageSet == NULL)
				return;

			const xsgui::Image imageArray[ETS_HasUnfinishedTask] = 
			{
				pImageTaskAcceptable,
				pImageTaskAcceptableRepe,
				pImageTaskAccomplish,
				pImageTaskAccomplishRepe,
				pImageTaskUnfinish
			};

			const xsgui::Image pImage = imageArray[m_CurrentTaskSign - 1]; 
			int nImageWidth = pImage.getWidth();
			int nImageHeigh = pImage.getHeight();

			if ( 0 != m_pTaskSignTexture)
			{
				//  计算绘制任务标记的矩形；
				yOffset -= (nImageHeigh-20);
				Rect rectTaskSign;
				rectTaskSign.left = ptScreen.x - nImageWidth / 2;
				if (!m_bRset)
				{
					m_foffset = m_foffset -0.3f;
					if (m_foffset <= 0.0f)
					{
						m_bRset = true;
					}
				}
				else
				{
					m_foffset = m_foffset + 0.3f;
					if (m_foffset >= 10.0f)
					{
						m_bRset = false;
					}
				}
				rectTaskSign.top = yOffset - m_foffset;
				rectTaskSign.right = rectTaskSign.left + nImageWidth;
				rectTaskSign.bottom = rectTaskSign.top + nImageHeigh;

				const xsgui::Rect & subrect = pImage.getSourceTextureArea();
				float height =static_cast<float> (pImageSet->getTexture()->getHeight());
				float width =static_cast<float>( pImageSet->getTexture()->getWidth());
				xs::Vector2 lefttop( subrect.d_left/ width, subrect.d_top/height);
				xs::Vector2 rightop( subrect.d_right/width, subrect.d_top/height);
				xs::Vector2 rightbottom(subrect.d_right/width, subrect.d_bottom/height);
				xs::Vector2 leftbottom(subrect.d_left/width, subrect.d_bottom/height);
				pRenderSystem->rectangle(rectTaskSign, m_pTaskSignTexture,lefttop, leftbottom,rightbottom,rightop );

			}
			*/
		}
	}
}
// 绘制顶层特效
void VisualComponent::drawTopMostEffect(IRenderSystem* pRenderSystem)
{
	EntityView* entity = getOwner();
	ulong flags = entity->getFlag();
	Vector3 space = getOwner()->getSpace();
	Vector3 nTemp ;
	nTemp.x = space.x;
	nTemp.y = space.y + 280;
	nTemp.z = space.z;
	pRenderSystem->switchTo3D();
	if (flags & flagDrawFinishTaskEffect)
	{
		if (m_pFinshTaskEffect)
		{
			m_pFinshTaskEffect->setVisible(true);
			m_pFinshTaskEffect->setPosition(nTemp);
			m_pFinshTaskEffect->setScale(85,85,85);
			m_pFinshTaskEffect->render(pRenderSystem,true);
		}
	}
	if (flags & flagDrawAutoMoveEffect)
	{
		if (m_pAutoMoveEffect)
		{
			m_pAutoMoveEffect->setVisible(true);
			m_pAutoMoveEffect->setPosition(nTemp);
			m_pAutoMoveEffect->setScale(85,85,85);
			m_pAutoMoveEffect->render(pRenderSystem,true);
		}
	}
	if(flags & flagDrawFinishAutoMoveEffect)
	{
		if (m_pFinshAutoMoveEffect)
		{
			m_pFinshAutoMoveEffect->setVisible(true);
			m_pFinshAutoMoveEffect->setPosition(nTemp);
			m_pFinshAutoMoveEffect->setScale(85,85,85);
			m_pFinshAutoMoveEffect->render(pRenderSystem,true);
		}
	}
	pRenderSystem->switchTo2D();
}