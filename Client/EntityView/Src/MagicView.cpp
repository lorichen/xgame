//==========================================================================
/**
* @file	  : MagicView.cpp
* @author : 
* created : 2008-4-3   10:58
*/
//==========================================================================

#include "stdafx.h"
#include "MagicView.h"
#include "MagicData.h"
#include "MWDLoader.h"
#include "Radial.h"
#include "IFormManager.h"
#include "IAudioEngine.h"
#include "ConfigCreatureRes.h"
#include "EntityViewImpl.h"

// 特效滞后时间
#define  LAG_TIME	200
#define  MOVE_SPEED	1.0
#define  FLY_TIME	170

MagicView::MagicView()
: mResHandle(INVALID_HANDLE), mMagicId(0), mNewResId(0), mOwner(0), mLoops(0),
mResLoaded(false), mUpdateOption(0), mNeedDelete(false), mObserver(INVALID_HANDLE), 
mAlpha(1.f), mScale(1.f), mAniSpeed(1.f), mTailSilence(false),mKillTimer(false)
{
	SetRectEmpty(&mShowRect);
	mOccVal = 0;
	mOffset.x = mOffset.y = mOffset.z = 0;
	mEntityType = typeEffect;
	mResType = typeResourceModel;
	m_bPetdie = false;//(万)
	//  ShowRect的默认大小，实际大小在加载资源时进行计算；
	mShowRect.left = -512;
	mShowRect.top = -384;
	mShowRect.right = 512;
	mShowRect.bottom = 384;
	m_vStartPos.x = m_vStartPos.y = m_vStartPos.z = 0;
	m_vRotNormal.x = m_vRotNormal.y = m_vRotNormal.z = 0;
	m_nSpaceAngle = 0;
}

MagicView::MagicView(ulong entityType, ulong resType) : EntityView(),
mResHandle(INVALID_HANDLE), mMagicId(0), mNewResId(0), mOwner(0), mLoops(0),
mResLoaded(false), mUpdateOption(0), mNeedDelete(false), mObserver(INVALID_HANDLE), mTailSilence(false),mKillTimer(false)
{
	SetRectEmpty(&mShowRect);
	mOccVal = 0;
	mOffset.x = mOffset.y = mOffset.z = 0;
	mEntityType = entityType;
	mResType = resType;
	//  ShowRect的默认大小，实际大小在加载资源时进行计算；
	mShowRect.left = -512;
	mShowRect.top = -384;
	mShowRect.right = 512;
	mShowRect.bottom = 384;
	m_vStartPos.x = m_vStartPos.y = m_vStartPos.z = 0;
	m_vRotNormal.x = m_vRotNormal.y = m_vRotNormal.z = 0;
	m_nSpaceAngle = 0;
}

MagicView::~MagicView()
{
//	Info("delete-magic:"<<_fi("0x%08x", (ulong)this)<<endl);
	close();
}

bool MagicView::create(const MagicContext_General& context)
{
	if (context.magicId == 0)
    {
        return false;
    }

	setFlag(flag3D | flagNoLogic | flagInScreen);

	mMagicId = context.magicId;
	mLoops = context.loops;
	if (mLoops <= 0)
    {
        mLoops = -1;
    }
	mAngle = context.angle;// 这个角度是施法者的面向
	mOwner = context.owner;
	if (context.bSceneMagic)
	{
		addFlag(flagGlobalSceneMagic);
	}

	//如果没有资源ID，就创建失败。
	ulong resId = MagicDataManager::getInstance().getValue(context.magicId, MagicProperty_ResId).getInt();
	if( 0 == resId) return false;
	setResId( resId);
	

	// modify by zjp；
	// 特效的绑定现在暂时只支持绑定在人物脚上，也就是Y值为0，所以特效默认的出现位置是贴近地面的，因此程序在这个地方设置一下特效的偏移值，改变特效的位置。
	// 特效需要支持任意的绑定位置，因为在程序里面设置偏移值不仅耗时而且效果也达不到！
	const IntArray& offset = MagicDataManager::getInstance().getValue(context.magicId, MagicProperty_AniOffset).getIntArray();
	if (offset.count == 3)
	{
		mOffset.x = offset.data[0];
		mOffset.y = offset.data[1];
		mOffset.z = offset.data[2];
	}

	//mScale = MagicDataManager::getInstance().getValue(mMagicId, MagicProperty_AniScale).getFloat();
	//mUpdateOption |= updateScale;
	//mAniSpeed = MagicDataManager::getInstance().getValue(mMagicId, MagicProperty_AniSpeed).getFloat();
	//mUpdateOption |= updateAniSpeed;

	bool ret = false;
	if (mOwner != NULL)
	{
		EntityView* owner = (EntityView*)getHandleData(mOwner);
		if (owner != NULL)
		{
			if (mLoops != -1 && !owner->hasFlag(flagInScreen))
            {
				return false;
            }

			//mFlags |= flagFade;
			ret = owner->onCommand(EntityCommand_AttachMagic, (ulong)this);
		}
		else
        {
			return false;
        }
	}
	else
    {
		ret = gGlobalClient->getSceneManager()->addEntity(context.tile, this);
        if (ret)
        {
            //  绑定在地面的光效，如果context中记录的nDuration不为0，则通过其来确定光效的持续时间；
            if (context.nDuration != 0)
            {
                gGlobalClient->getTimerAxis()->SetTimer(DURATION_TIME_ID, context.nDuration, this, 1, __FILE__);
            }
        }
    }

	//if (ret)
	//{
	//	Trace("create-magic:"<<_fi("0x%08x", (ulong)this)<<endl);
	//}

	return ret;
}

void MagicView::close()
{
    gGlobalClient->getTimerAxis()->KillTimer(DELAY_RELEASE_TIME_ID, this);
    gGlobalClient->getTimerAxis()->KillTimer(DURATION_TIME_ID, this);
	//if (mOwner)
		onLeaveViewport();
	//else
	//	gGlobalClient->getSceneManager()->removeEntity(getTile(), this);
}

const AABB& MagicView:: GetAABB()
{
	ModelNode *pMode = getModelNode();
	if (pMode)
	{
		m_AABB = getModelNode()->getWorldAABB();
	}
	return m_AABB;
}

const Matrix4& MagicView::GetFullTransform()
{
	ModelNode *pMode = getModelNode();
	if (pMode)
	{
		m_FullTransform = pMode->getFullTransform();
	}
	return m_FullTransform;
}

void MagicView::setFadeIn()
{
	if ((mUpdateOption & updateFadeIn) == 0)
	{
		if (mUpdateOption & updateFadeOut)
			mUpdateOption &= ~updateFadeOut;
		mUpdateOption |= updateFadeIn;
		mAlpha = 0.f;
	}
}

void MagicView::setFadeOut()
{
	if ((mUpdateOption & updateFadeOut) == 0)
	{
		if (mUpdateOption & updateFadeIn) mUpdateOption &= ~updateFadeIn;
		mUpdateOption |= updateFadeOut;
		mAlpha = 1.f;
	}
}
//宠物死亡
void MagicView::setFadeDie()
{
	if ((mUpdateOption & updatePetdie) == 0)
	{
		mUpdateOption |= updatePetdie;
		m_bPetdie = true;
	}
}

void MagicView::setTile(const xs::Point& ptTile)
{
	if (ptTile.x != m_ptTile.x || ptTile.y != m_ptTile.y)
	{
		m_ptTile = ptTile;
		onTileChanged();
	}
}

void MagicView::onTileChanged()
{
	xs::Point pt;
	gGlobalClient->getSceneManager()->tile2World(getTile(), pt);
	setWorld(pt);
	setSortLeft(getWorld());
	setSortRight(getWorld());

	Vector3 space;
	gGlobalClient->getSceneManager()->tile2Space(getTile(), space);
	setSpace(space);

	mUpdateOption |= updatePosition;
}

void MagicView::onEnterViewport(int priority)
{
	requestRes(priority);
	
	//修改了地图移动后特效不可以再见的问题
	//初步认为是没有设置这个标志，导致位置不能更新
	//add by yhc & xxh
	//mUpdateOption = 1;

	//修正地图编辑器特效消失问题,进入视口的时候把数值往modenode赋一次
	//原因是没有将缩放传给modenode 里面,modenode 里面的m_vScale是1
	mUpdateOption |= updateScale;
	mUpdateOption |= updatePosition;
	mUpdateOption |= updateAngle;

	int  nResID = getResId();
    //// 播放声效
	int nMusicID = ConfigCreatureRes::Instance()->getSoundIDFromId(nResID);
	if (nMusicID > 0)
	{
		IFormManager* pFromManage = gGlobalClient->getFormManager();
		if (pFromManage)
		{
			pFromManage->PlaySound(nMusicID,0,1,SOUNDRES_TYPE_SOUND);
		}
	}
}

void MagicView::onLeaveViewport()
{
	// 停止声效
	int  nResID = getResId();
	int nMusicID = ConfigCreatureRes::Instance()->getSoundIDFromId(nResID);
	if (nMusicID > 0)
	{
		IFormManager* pFromManage = gGlobalClient->getFormManager();
		if (pFromManage)
		{
			pFromManage->StopSoundBySID(nMusicID);
		}
	}
	if (isValidHandle(mResHandle))
		releaseRes();
	
	//zgz 地图物件离开viewport需要删除
	if (hasFlag(flagMapItem))
	{
		setNeedDelete();		
	}		
}

void MagicView::requestRes(int priority)
{
	if (!isValidHandle(mResHandle))
	{
		Assert(getResId() != 0);
		mNewResId = MZIDMap::getInstance().add(getResId(), 1);
		//一定不能在主线程里面做加载和解码
		mResHandle = gGlobalClient->getResourceManager()->requestResource(mNewResId, 
			(ResourceType)getResType(), false, priority - 1); // 光效的优先级稍稍高一点点 
		getModelNode();
	}
}

void MagicView::releaseRes()
{
	if (isValidHandle(mResHandle))
	{
		removeFlag(flagSelected);
		gGlobalClient->getResourceManager()->releaseResource(mResHandle);
		MZIDMap::getInstance().remove(mNewResId);
		mResLoaded = false;
		mResHandle = INVALID_HANDLE;
	}
}

inline ModelNode* MagicView::getModelNode()
{
	IResourceNode* node = (IResourceNode*)getHandleData(mResHandle);
	if (node)
	{
		IResource* res = node->getResource();
		if (res)
		{
			ModelNode* n = (ModelNode*)res->getInnerData();
			if (n && !mResLoaded) onResLoaded(n);
			return n;
		}
	}
	return NULL;
}

void MagicView::setScale(float scale)
{
    if (mScale != scale)
    {
	    mScale = scale;
	    mUpdateOption |= updateScale;
    }
}

void MagicView::SetGenerateParticle(bool bGenerate)
{
	ModelNode* pNode = getModelNode();
	if (NULL != pNode)
	{
		pNode->SetGenerateParticle(bGenerate);
	}
}

int  MagicView::GetParticleNum()
{
	ModelNode* pNode = getModelNode();
	if (NULL != pNode)
	{
		return pNode->GetParticleNum();
	}
	return 0;
}

void MagicView::onResLoaded(ModelNode* node)
{
	ModelInstance* inst = node->getModelInstance();
	if (inst)
	{
		mResLoaded = true;
		inst->setCallback(static_cast<IAnimationCallback*>(this));
		if (!inst->setCurrentAnimation("Birth", mLoops))
			inst->setCurrentAnimation("Stand", mLoops);

		Animation* ani = (Animation*)inst->_getCurrentAnimation();
		if (ani) inst->setAnimationTime(ani->getTimeStart());
		inst->resetFX();

		// 缩放比例
		if (mMagicId!=0)
		{
			float scale = MagicDataManager::getInstance().getValue(mMagicId, MagicProperty_AniScale).getFloat();
			node->setScale(scale, scale, scale);
			setScale(mScale);
		}


		// 动画速度
		if (mMagicId!=0)
		{
			float speed = MagicDataManager::getInstance().getValue(mMagicId, MagicProperty_AniSpeed).getFloat();
			inst->setAnimationSpeed(speed);
		}

        //  对于场景特效来说，mMagicId的值是为0的，其表现也和设计时的表现一致，不再需要从配置中读取表现信息；
        if (mMagicId != 0)
        {
			// comment by zjp;这个暂时注释掉，看不出来有什么意义
		    // 矫正方向
			//if (MagicDataManager::getInstance().getValue(mMagicId, MagicProperty_AniDir).getInt() == 1)
			//{
			//	Quaternion q;
			//	q.FromAngleAxis(getAngle(), Vector3(0,1,0));
			//	node->setOrientation(q);
			//}

		    // 颜色
		    const IntArray& color = MagicDataManager::getInstance().getValue(mMagicId, MagicProperty_AniColor).getIntArray();
		    ulong alpha = MagicDataManager::getInstance().getValue(mMagicId, MagicProperty_AniTrans).getInt();
		    ColorValue val;
		    if (color.count == 3)
		    {
                #define _1_div_255	0.003922f // = 1/255
			    val.r = color.data[0] * _1_div_255;
			    val.g = color.data[1] * _1_div_255;
			    val.b = color.data[2] * _1_div_255;
		    }
		    val.a = alpha / 255.f;

		    node->setGlobalDiffuse(val);
        }

		//	对ShowRect进行设置；
		IModel* pModel = inst->getModel();
		ASSERT(pModel != NULL);
		if (pModel != NULL)
		{
			Vector4 rectBoundingBox;
			pModel->getBoundingBox(rectBoundingBox);
			Vector3 vScale = node->getScale();
			mShowRect.left = rectBoundingBox.x * vScale.x;
			mShowRect.top = rectBoundingBox.y * vScale.y;
			mShowRect.right = rectBoundingBox.z * vScale.x;
			mShowRect.bottom = rectBoundingBox.w * vScale.y;
			//  ＋1以避免出现0值的情况；
			::InflateRect(&mShowRect, 1, 1);
		}

		mUpdateOption |= updatePosition;
    }
}

void MagicView::drawPickObject(IRenderSystem* pRenderSystem)
{
	ModelNode* node = getModelNode();
	if (node) node->render(pRenderSystem, true);
}

void MagicView::drawTopMost(IRenderSystem* pRenderSystem)
{
	PP_BY_NAME("MagicView::drawTopMost");
    if (pRenderSystem == NULL)
    {
        ASSERT(false);
        return;
    }

    //  如果特效是在场景编辑器中并且处于选中状态，则进行边框的绘制；
    ulong flags = getFlag();
    if (flags & flagSelectedForMapEdit)
    {
        xs::Point pointtScreen;
        gGlobalClient->getSceneManager()->world2Screen(getWorld(), pointtScreen);
        xs::Rect rectBoundingBox = mShowRect;
        ::OffsetRect(&rectBoundingBox, pointtScreen.x, pointtScreen.y);
        //  微调一下，使显示更协调；
        Rect rectShow;
        rectShow.left = rectBoundingBox.left;
        rectShow.top = rectBoundingBox.top;
        rectShow.right = rectBoundingBox.right - 1;
        rectShow.bottom = rectBoundingBox.bottom - 1;
        pRenderSystem->rectangle(rectShow, ColorValue(0, 1, 0, 1));
    }
}

size_t MagicView::onLoad(Stream* stream, size_t len)
{
    if (stream == NULL)
    {
        ASSERT(false);
        return 0;
    }

    //  根据stream中的内容创建MagicView对象；
    char szResID[256];
    uchar ucResIDLength;
    stream->read(&ucResIDLength, sizeof(uchar));
    stream->read(szResID, ucResIDLength);
    szResID[ucResIDLength] = 0;
    int nResID = *(int*)szResID;
    setResId(nResID);
    setFlag(flag3D | flagNoLogic | flagInScreen);
    mLoops = -1;

#if 0
    const IntArray& offset = MagicDataManager::getInstance().getValue(context.magicId, MagicProperty_AniOffset).getIntArray();
    if (offset.count == 3)
    {
        mOffset.x = offset.data[0];
        mOffset.y = offset.data[1];
        mOffset.z = offset.data[2];
    }

    //mScale = MagicDataManager::getInstance().getValue(mMagicId, MagicProperty_AniScale).getFloat();
    mUpdateOption |= updateScale;
    mAniSpeed = MagicDataManager::getInstance().getValue(mMagicId, MagicProperty_AniSpeed).getFloat();
    mUpdateOption |= updateAniSpeed;
#endif

    return len;
}

size_t MagicView::onSave(Stream* stream) const
{
    if (stream == NULL)
    {
        ASSERT(false);
        return 0;
    }

    uchar ucResIDLength = sizeof(int);
    stream->write(&ucResIDLength, sizeof(uchar));
    int nResID = getResId();
    stream->write(&nResID, ucResIDLength);
    //  锚点偏移的处理；
    uchar uPointLength = sizeof(xs::Point);
    stream->write((const void *)&m_pointAnchorOffset, sizeof(xs::Point));
    //  缩放比的记录；
    stream->write((const void *)&mScale, sizeof(float));
    //  旋转角度的记录；
    stream->write((const void *)&mAngle, sizeof(ulong));

    return ucResIDLength + sizeof(uchar) + uPointLength + sizeof(float) + sizeof(ulong);
}

void MagicView::onAddEntity()
{
	if (hasFlag(flagFade|flagInScreen))
		setFadeIn();
}

void MagicView::onRemoveEntity()
{
	if (isValidHandle(mResHandle))
	{
		if (hasFlag(flagFade|flagInScreen))
			setFadeOut();
	}
}

void MagicView::release()
{ 
    //  旧的实现暂时屏蔽，作为后续开发的调试和参考；
#if 0
    //	close(); delete this; 
    //if (hasFlag(flagInMap))
    //	gGlobalClient->getSceneManager()->removeEntity(getTile(), this);
    //if (hasFlag(flagFade))
    setNeedDelete();
    //Info("GetTickCount1="<<GetTickCount()<<endl);
    if (!mOwner)
        gGlobalClient->getTimerAxis()->SetTimer(DELAY_RELEASE_TIME_ID, 1, this, 1, __FILE__);
    //else
    //	delete this;
#endif

	// 地图上的物件在跳地图时没有释放，这里立即释放掉 [5/5/2011 zgz]
	if (hasFlag(flagReleaseImmediate))
	{
		delete this;
		return;
	}


    //  设置删除标志；
    setNeedDelete();

    //  终止DURATION_TIME_ID定时器；
    gGlobalClient->getTimerAxis()->KillTimer(DURATION_TIME_ID, this);

    //  启动删除光效的定时器；
    gGlobalClient->getTimerAxis()->SetTimer(DELAY_RELEASE_TIME_ID, 1, this, 1, __FILE__);
}

void MagicView::OnTimer(unsigned long dwTimerID)
{
    switch (dwTimerID)
    {
    case DELAY_RELEASE_TIME_ID:
        {
            mKillTimer = true;
            gGlobalClient->getTimerAxis()->KillTimer(DELAY_RELEASE_TIME_ID, this);
            gGlobalClient->getSceneManager()->removeEntity(getTile(), this);// zgz 这里magicview只是从场景中移除，但是没有真正释放
			
			
			if (!gGlobalClient->getSceneManager()->IsReferencedBySceneManager(this) && !hasFlag(flagMapItem) &&!mOwner)
			{
				close();
				delete this;
				// magicview 目前有4个用途：
				// 1. 用于地表上的特效显示(用flagMapItem标记)，原来是从不释放，只在玩家跳地图时释放，现在修改为离开ViewPort释放
				// 2. 用于玩家点击地面这种独立的特效(没有flagMapItem标记，mOwner为空)，这里释放的就是这种
				// 3. 依附于其他EntityView身上的，比如 CEffectObjHolder(没有flagMapItem标记，mOwner不为空)，也没有正常释放
				// 4. 全场景的特性，已有删除
			}
        }
        break;

    case DURATION_TIME_ID:
        {
            release();
        }
        break;

	case PRELOAD_TIME_ID:
		{
			if (getModelNode())
			{
				gGlobalClient->getTimerAxis()->KillTimer(PRELOAD_TIME_ID, this);
				releaseRes();
				delete this;
			}
		}
		break;

    default:
        {
            ASSERT(false);
        }
        break;
    }
}

void MagicView::SetAnchorOffset(const xs::Point& pointAnchorOffset)
{
    //  更新锚点偏移信息；
    m_pointAnchorOffset.x = pointAnchorOffset.x;
    m_pointAnchorOffset.y = pointAnchorOffset.y;

    //  更新特效位置；
    xs::Point pointWorld;
    gGlobalClient->getSceneManager()->tile2World(getTile(), pointWorld);
    pointWorld.x -= m_pointAnchorOffset.x;
    pointWorld.y -= m_pointAnchorOffset.y;
    setWorld(pointWorld);
    setSortLeft(getWorld());
    setSortRight(getWorld());

    xs::Point ptScreenCenter;
    gGlobalClient->getSceneManager()->tile2Screen(getTile(),ptScreenCenter);
    ptScreenCenter.x -= pointAnchorOffset.x;
    ptScreenCenter.y -= pointAnchorOffset.y;
    xs::Point ptWorld;
    gGlobalClient->getSceneManager()->screen2World(ptScreenCenter,ptWorld);
    Vector3 vSpace;
    gGlobalClient->getSceneManager()->world2Space(ptWorld,vSpace);
    setSpace(vSpace);

    mUpdateOption |= updatePosition;
}

bool MagicView::update(float tick, float deltaTick, IRenderSystem* pRenderSystem)
{
	if (mNeedDelete)
	{
		if (hasFlag(flagMapItem))
		{
			ISceneManager2* pSceneManager = gGlobalClient->getSceneManager();
			if (NULL != pSceneManager)
			{							
				if (pSceneManager->getRunType() == RUN_TYPE_GAME)
				{
					this->removeFlag(flagFade);
					pSceneManager->removeEntity(getTile(), this);
					delete this;
					return false;
				}
			}			
		}

		if (hasFlag(flagFallMagic))
		{
			if (GetParticleNum() == 0)
			{
				ModelNode* pNode = getModelNode();
				if (NULL != pNode)
				{
					pNode->SetGenerateParticle(true);
				}
				delete this;
				return false;
			}
		}

		if (hasFlag(flagFade))
		{
			if (mUpdateOption == 0)
			{
				delete this;
				return false;
			}
		}
		else
		{
		//Info("GetTickCount2="<<GetTickCount()<<endl);
			if (!mOwner)
			{
				if (mKillTimer)
				{
					//if (hasFlag(flagInMap))
					//	gGlobalClient->getSceneManager()->removeEntity(getTile(), this);
					delete this;
					return false;
				}
			}
			else
			{
				delete this;
				return false;

			}
		}

		
	}

	if (hasFlag(flagInScreen))
	{
		ModelNode* node = getModelNode();
		if (node)
		{
			if (mUpdateOption)
			{
				if (mUpdateOption & updatePosition)
				{
					const Vector3& pos = getSpace();
					//node->setPosition(pos.x + mOffset.x, pos.y + mOffset.y, pos.z + mOffset.z);
					node->setPosition(pos.x , pos.y , pos.z);
					if (pos.y!=0)
					{
						breakable;
					}
					mUpdateOption &= ~updatePosition;
				}

				if (mUpdateOption & updateScale)
				{
                    //  非技能特效的话，这里读取到的值可能会是无效的，此时将1作为固定缩放比；
					float stockScale = 0;
					if (mMagicId > 0)
					{
						stockScale = MagicDataManager::getInstance().getValue(mMagicId, MagicProperty_AniScale).getFloat();
					}

                    if (stockScale <= 0)
                    {
                        stockScale = 1;
                    }
					node->setScale(stockScale*mScale, stockScale*mScale, stockScale*mScale);

					//  更新一下ShowRect；
					ModelInstance* pModelInstance = node->getModelInstance();
					ASSERT(pModelInstance != NULL);
					if (pModelInstance != NULL)
					{
						IModel* pModel = pModelInstance->getModel();
						ASSERT(pModel != NULL);
						if (pModel != NULL)
						{
							Vector4 rectBoundingBox;
							pModel->getBoundingBox(rectBoundingBox);
							Vector3 vScale = node->getScale();
							mShowRect.left = rectBoundingBox.x * vScale.x;
							mShowRect.top = rectBoundingBox.y * vScale.y;
							mShowRect.right = rectBoundingBox.z * vScale.x;
							mShowRect.bottom = rectBoundingBox.w * vScale.y;
							//  ＋1以避免出现0值的情况；
							::InflateRect(&mShowRect, 1, 1);
						}
					}

					mUpdateOption &= ~updateScale;
				}

				if (mUpdateOption & updateAniSpeed)
				{
					node->getModelInstance()->setAnimationSpeed(mAniSpeed);
					mUpdateOption &= ~updateAniSpeed;
				}

				if (mUpdateOption & updateAngle)
				{
					if (m_nSpaceAngle)
					{
						Quaternion r;
						r.FromAngleAxis(m_nSpaceAngle, m_vRotNormal);
						node->setOrientation(r);
						m_nSpaceAngle = 0;
					}
					else
					{
	 					Quaternion q;
	 					q.FromAngleAxis(mAngle, Vector3(0,1,0));
	 					node->setOrientation(q);
					}

					mUpdateOption &= ~updateAngle;
				}

				if (mUpdateOption & updateColor)
				{
					//node->setGlobalDiffuse(mColor);
					mUpdateOption &= ~updateColor;
				}

				// 特效加速的原因,暂时这样修改，modify by zjp
				//if (mUpdateOption & updateFadeOut)
				//	node->update(tick, 0, pRenderSystem);
				//else
				//	node->update(tick, deltaTick, pRenderSystem);

				if (mUpdateOption & updateFadeIn)
				{
					ColorValue clr(1.f, 1.f, 1.f, mAlpha);
					node->updateColor(pRenderSystem, clr);
					mAlpha += FADE_ALPHA * deltaTick;
					if (mAlpha > 1.0f)
					{
						mAlpha = 1.0f;
						mUpdateOption &= ~updateFadeIn;
					}
				}

				if (mUpdateOption & updateFadeOut)
				{
					ColorValue clr(1.f, 1.f, 1.f, mAlpha);
					node->updateColor(pRenderSystem, clr);
					mAlpha -= FADE_ALPHA * deltaTick;
					if (mAlpha < 0.f)
					{
						mAlpha = 0.f;
						mUpdateOption &= ~updateFadeOut;
						releaseRes();
						return true;
					}
				}
				if (mUpdateOption & updatePetdie)
				{
					ColorValue clr(1.f, 1.f, 1.f, mAlpha);
					mAlpha = 0.3f;
					node->updateColor(pRenderSystem, clr);
					if (!m_bPetdie)
					{
						mUpdateOption &= ~updatePetdie;
					}
				}
			}

			if (mUpdateOption == 0)
				node->update(tick, deltaTick, pRenderSystem);
		}
	}
	else
	{
		ModelNode* node = getModelNode();
		if (node)
		{
			ModelInstance* inst = node->getModelInstance();
			inst->advanceTime(deltaTick);
		}
	}

	return true;
}

void MagicView::setPosition(const Vector3& pos)
{
	setSpace(pos);
	xs::Point pt;
	gGlobalClient->getSceneManager()->space2World(pos, pt);
	setWorld(pt);
	mUpdateOption |= updatePosition;
}

void MagicView::setAngle(long angle)
{
	if (angle != mAngle)
	{
		mAngle = angle;
		while (mAngle < 0) mAngle += 360;
		while (mAngle >= 360) mAngle -= 360;
		mUpdateOption |= updateAngle;
	}
}

void MagicView::setSpace(const Vector3& space)
{
	mSpace = space;
	mUpdateOption |= updatePosition;
}

void MagicView::onOneCycle(const std::string& animation,int loopTimes)
{
	if (loopTimes == 0 && !mTailSilence)
	{
		if (isValidHandle(mObserver))
		{
			IEffectObserver *pObserver = (IEffectObserver *)getHandleData(mObserver);
			pObserver->onFinished(this);
		}
		else
			release();
	}
}

void MagicView::preLoadRes()
{
	requestRes(0);
	// 这个定时器用来检测资源是否加载完毕，如果加载完了，就删除自己，将自己放入垃圾回收池
	gGlobalClient->getTimerAxis()->SetTimer(PRELOAD_TIME_ID, 500, this);
}


MagicView_Move::MagicView_Move()
: mMoveSpeed(MOVE_SPEED), mDirTime(0.f), mTicks(0)
{
}

MagicView_Move::MagicView_Move(ulong entityType, ulong resType)
: MagicView(entityType, resType),
mMoveSpeed(MOVE_SPEED), mDirTime(0.f), mTicks(0)
{
}

MagicView_Move::~MagicView_Move()
{
}


void MagicView_Move::close()
{
	gGlobalClient->getTimerAxis()->KillTimer(1, this);
	MagicView::close();
}

void MagicView_Move::stopMove()
{
	gGlobalClient->getTimerAxis()->KillTimer(1, this);
	if (isValidHandle(mObserver))
	{
		IEffectObserver *pObserver = (IEffectObserver *)getHandleData(mObserver);
		pObserver->onFinished(this);
	}
	else
		release();
}

void MagicView_Move::onPosChanged(const Vector3& pos)
{
	setSpace(pos);
	xs::Point pt;
	gGlobalClient->getSceneManager()->space2World(pos, pt);
	setWorld(pt);

	mUpdateOption |= updatePosition;
}








MagicViewWithMoveRadial::MagicViewWithMoveRadial()
: mBlockType(0)
{
}

MagicViewWithMoveRadial::MagicViewWithMoveRadial(ulong entityType, ulong resType)
: MagicView_Move(entityType, resType), mBlockType(0)
{
}

MagicViewWithMoveRadial::~MagicViewWithMoveRadial()
{
	close();
}

bool MagicViewWithMoveRadial::create(const MagicContext_MoveRadial& context)
{
	if (!MagicView::create(context))
		return false;

	setSpeed(context.moveSpeed);
	mBlockType = context.blockType;
	return true;
}

bool MagicViewWithMoveRadial::onCommand(ulong cmd, ulong param1, ulong param2)
{
	switch (cmd)
	{
	case EntityCommand_MoveRadial:
		{
			xs::Point* start = (xs::Point*)param1;
			xs::Point* end = (xs::Point*)param2;
			gGlobalClient->getSceneManager()->tile2Space(*start, mStartPos);
			gGlobalClient->getSceneManager()->tile2Space(*end, mNextPos);
			long angle = calcAngle_space(mStartPos, mNextPos);
			setAngle(angle);
			mTicks = 0;
			mOldTicks = gGlobalClient->getTimeStamp();
			mDirDistance = mNextPos - mStartPos;
			mDirTime = mDirDistance.length() / mMoveSpeed;
			mDirDistance.normalize();
			//gGlobalClient->getTimerAxis()->SetTimer(1, 1, this);
		}
		break;
	default:
		return false;
	}
	return true;
}

bool MagicViewWithMoveRadial::update(float tick, float deltaTick, IRenderSystem* pRenderSystem)
{
	if (!mNeedDelete)
		moveStep((ulong)deltaTick);
	return MagicView::update(tick, deltaTick, pRenderSystem);
}

void MagicViewWithMoveRadial::OnTimer(unsigned long dwTimerID)
{
	if (dwTimerID == 0)
		MagicView::OnTimer(dwTimerID);
	else if (dwTimerID == 1)
	{
		ulong newTicks = gGlobalClient->getTimeStamp();
		moveStep(newTicks - mOldTicks);
		mOldTicks = newTicks;
	}
}

void MagicViewWithMoveRadial::moveStep(ulong period)
{
	mTicks += period;
	if (mTicks >= mDirTime) // 到达终点
	{
		onPosChanged(mNextPos);
		stopMove();
	}
	else
	{
		Vector3 curPos = mStartPos + mDirDistance * (mTicks * mMoveSpeed);
		xs::Point tile;
		gGlobalClient->getSceneManager()->space2Tile(curPos, tile);
		if (getTile().x != tile.x || getTile().y != tile.y)
		{
			bool block = false;
			if (mBlockType == 0) // 射线阻挡，只判断地表占位就停止
			{
				Tile* tileobj = gGlobalClient->getSceneManager()->getTile(tile);
				block = (!tileobj || !tileobj->isValid() || tileobj->isOccupant_Surface() != 0);
			}
			else // 线段阻挡
			{
				Tile* tileobj = gGlobalClient->getSceneManager()->getTile(tile);
				block = (!tileobj || !tileobj->isValid() || tileobj->isBlock());
			}
			/*
			if (mBlockType == 0)
			{
				block = false;
			}
			else if (mBlockType == 1) // 飞行阻挡
			{
				Tile* tileobj = gGlobalClient->getSceneManager()->getTile(tile);
				block = (!tileobj || !tileobj->isValid() || tileobj->isFlyBlock() != 0);
			}
			else if (mBlockType == 2) // 蔓延阻挡
			{
				Tile* tileobj = gGlobalClient->getSceneManager()->getTile(tile);
				block = (!tileobj || !tileobj->isValid() || tileobj->isSpreadBlock() != 0);
			}
			else if (mBlockType == 3) // 行走阻挡
			{
				Tile* tileobj = gGlobalClient->getSceneManager()->getTile(tile);
				block = (!tileobj || !tileobj->isValid() || tileobj->isBlock() != 0);
			}*/

			if (block)
			{
				stopMove();
				return;
			}
			else
			{
				gGlobalClient->getSceneManager()->moveEntity(getTile(), tile, this);
				onPosChanged(curPos);
			}
		}
	}
}







MagicViewWithMoveTrack::MagicViewWithMoveTrack()
: mSource(INVALID_HANDLE), mTarget(INVALID_HANDLE), m_TargetAABB()
{
	mTargetTile.x = mTargetTile.y = 0;
}

MagicViewWithMoveTrack::MagicViewWithMoveTrack(ulong entityType, ulong resType)
: MagicView_Move(entityType, resType), mSource(INVALID_HANDLE), mTarget(INVALID_HANDLE)
{
	mTargetTile.x = mTargetTile.y = 0;
}

MagicViewWithMoveTrack::~MagicViewWithMoveTrack()
{
	close();
}

bool MagicViewWithMoveTrack::create(const MagicContext_MoveTrack& context)
{
	if (!MagicView::create(context))
		return false;
	//setSpeed(context.moveSpeed);
	mSource = context.src;
	mTarget = context.target;
	_getStartPos();
	onPosChanged(m_vStartPos);
	return true;
}

bool MagicViewWithMoveTrack::onCommand(ulong cmd, ulong param1, ulong param2)
{
	switch (cmd)
	{
	case EntityCommand_MoveTrack:
		{
			EntityView* source = (EntityView*)(getHandleData(mSource));
			if (!source) return false;

			//mStartPos = source->getSpace();
			resetMoveInfo();
			//adjustStartInfo();
			//gGlobalClient->getTimerAxis()->SetTimer(1, 1, this);
		}
		break;
	default:
		return false;
	}
	return true;
}


bool MagicViewWithMoveTrack::update(float tick, float deltaTick, IRenderSystem* pRenderSystem)
{
	if (!mNeedDelete)
		moveStep((ulong)deltaTick);
	return MagicView::update(tick, deltaTick, pRenderSystem);
}

void MagicViewWithMoveTrack::OnTimer(unsigned long dwTimerID)
{
	if (dwTimerID == 0)
		MagicView::OnTimer(dwTimerID);
	else if (dwTimerID == 1)
	{
		resetMoveInfo();
		ulong newTicks = gGlobalClient->getTimeStamp();
		moveStep(newTicks - mOldTicks);
		mOldTicks = newTicks;
		//moveStep(30);
	}
}

void MagicViewWithMoveTrack::adjustStartInfo()
{
	if (!isValidHandle(mSource))
	{
		return;
	}
	EntityView* source = (EntityView*)(getHandleData(mSource));
	if (source==NULL)
	{
		return;
	}

	xs::Rect rt = source->getShowRect();
	mOffset.y  = 3*(rt.bottom-rt.top)/4;
}

void MagicViewWithMoveTrack::resetMoveInfo()
{
	EntityView* target = (EntityView*)(getHandleData(mTarget));
	if (target) // 目标还在
	{
		const xs::Point& tile = target->getTile();
		if (tile.x != mTargetTile.x || tile.y != mTargetTile.y) // 位置改变了，需要重新计算
		{
			// add by zjp
			if (target)
			{
				m_TargetAABB = target->GetAABB();
				m_TargetMatrix = target->GetFullTransform();
			}

			mStartPos = getSpace();
			string s("Bip01 Spine1"); // 受击点骨骼名称
			target->onCommand(EntityCommand_GetBoneSpace, (ulong)&mNextPos, (ulong)&s);
			if (mNextPos.x==0 && mNextPos.y==0 && mNextPos.z==0)
			{
				mNextPos = target->getSpace();
			}

			mTargetTile = tile;
			//long angle = calcAngle_space(mStartPos, mNextPos);
			//if (angle>165 && angle<180)
			//{
			//	// 在3D空间下，处于这个角度的特效不可见，因此改用2D计算
			//	setAngle(angle);
			//}
			//else
			{
				// 计算3D空间下的偏转
				Vector3 start,end;
				start = Vector3(0,0,1);// 特效默认是沿Z轴正方向
				end = mNextPos-mStartPos;
				start.normalize();
				end.normalize();
				float fCosA = start.dotProduct(end);//旋转角  
				Radian rdAngle = Math::ACos(fCosA);
				m_vRotNormal = start.crossProduct(end);
				m_vRotNormal.normalize();
				ulong spaceAngle = rdAngle.valueDegrees();
				setSpaceAngle(spaceAngle);
			}

			mTicks = 0;
			mOldTicks = gGlobalClient->getTimeStamp();
			mDirDistance = mNextPos - mStartPos;

			//xs::Point sTile,nTile;
			//Vector3 sSpace,nSpace;
			//sTile.x = 800;
			//sTile.y = 600;
			//nTile.x = 800;
			//nTile.y = 599;
			//gGlobalClient->getSceneManager()->tile2Space(sTile, sSpace);
			//gGlobalClient->getSceneManager()->tile2Space(nTile, nSpace);
			//Vector3 nDistance = sSpace-nSpace;
			//int nLenth =  nDistance.length();
			//int nTileLenth = 45;


			//mDirTime = mDirDistance.length() / mMoveSpeed;
			mDirTime = FLY_TIME; //  固定飞行时间
			mMoveSpeed = float(mDirDistance.length()/mDirTime);
		}
	}
}

// 忽略Y值的碰撞检测
inline bool intersectsWithoutY(const AABB& b1, const AABB& b2)
{
	// Early-fail for nulls
	if (b1.isNull() || b1.isNull())
		return false;
	Vector3 mMaximum = b1.getMaximum(),
			mMinimum = b1.getMinimum(),
			b2mMinimum = b2.getMinimum(),
			b2mMaximum = b2.getMaximum();

	// Use up to 6 separating planes
	if (mMaximum.x < b2mMinimum.x)
		return false;
	if (mMaximum.z < b2mMinimum.z)
		return false;

	if (mMinimum.x > b2mMaximum.x)
		return false;
	if (mMinimum.z > b2mMaximum.z)
		return false;

	// otherwise, must be intersecting
	return true;
}

void MagicViewWithMoveTrack::moveStep(ulong period)
{
#if 0
	if (!mResLoaded)
	{
		return;
	}
	Vector3 savePos = getSpace();
	mTicks += period;
	if (mTicks >= mDirTime) // 到达终点
	{
		WarningLn("追踪特效失去目标超时消失");
		stopMove();
		return;
	}
	resetMoveInfo();
	Vector3 v = mDirDistance;
	v.normalize();
	Vector3 VMove = v*(mTicks*mMoveSpeed);
	Vector3 curPos = mStartPos + VMove;
	// modify by zjp;
	// 获取AABB进行碰撞检测,不使用时间来判断，用时间判断特效表现不怎么好
	m_SourceMatrix = getModelNode()->getFullTransform();
	m_SourceAABB = GetAABB();
	AABB sourcTestAABB = m_SourceAABB;
	sourcTestAABB.transform(m_SourceMatrix);
	AABB targetTestAABB = m_TargetAABB;
	targetTestAABB.transform(m_TargetMatrix);
	if (intersectsWithoutY(sourcTestAABB,targetTestAABB))
	{
		// 这个地方最好的做法是停止在碰撞发生时的碰撞点位置，
		// 因为现在没有提供上述的接口，现在暂时用碰撞发生前的一个位置点。
		onPosChanged(savePos);
		stopMove();
		return;
	}
	else
	{
		onPosChanged(curPos);
	}
#endif

#if 1
	mTicks += period;
	if (mTicks >= mDirTime) // 到达终点
	{
		onPosChanged(mNextPos);
		stopMove();
	}
	else
	{
		resetMoveInfo();
		Vector3 v = mDirDistance;
		v.normalize();
		
		Vector3 VMove = v*(mTicks*mMoveSpeed);
		Vector3 curPos = mStartPos + VMove;
		if (VMove.length() >= mDirDistance.length())
		{
			onPosChanged(mNextPos);
			stopMove();
		}
		else
		{
			xs::Point tile;
			gGlobalClient->getSceneManager()->space2Tile(curPos, tile);
			if (getTile().x != tile.x || getTile().y != tile.y)
				gGlobalClient->getSceneManager()->moveEntity(getTile(), tile, this);
			onPosChanged(curPos);
		}
	}
#endif
}


void MagicViewWithMoveTrack::_getStartPos()
{
	EntityView *pView = (EntityView *)getHandleData(mSource);
	m_vStartPos.x = m_vStartPos.y = m_vStartPos.z = 0;
	if (pView)
	{
		EntityViewImpl *pImp = (EntityViewImpl *)pView;
		string s("bip01 L Finger0");
		m_vStartPos.x = m_vStartPos.y = m_vStartPos.z = 0;
		pImp->onCommand(EntityCommand_GetBoneSpace, (ulong)&m_vStartPos, (ulong)&s);
	}
}





CEffectObjHolder::CEffectObjHolder()
{
	mHandle = createHandle((ulong)this);
}

CEffectObjHolder::~CEffectObjHolder()
{
	if (mHandle != INVALID_HANDLE) 
		closeHandle(mHandle); 

	close();
}

void CEffectObjHolder::addAttachObj(MagicView* obj)
{
	attach(obj);
}

bool CEffectObjHolder::hasAttackObj(ulong magicID)
{
	for (MagicViewList::iterator it=m_AttachedEffectObjList.begin(); it!=m_AttachedEffectObjList.end(); ++it)
	{
		if ((*it)->getMagicId() == magicID)
		{
			return true;
		}
	}
	return false;
}

void CEffectObjHolder::AddRelativedObj(MagicView* obj)
{
    m_RelativedEffectObjList.push_back(obj);
}

void CEffectObjHolder::removeAttachObj(ulong magicId, int loops)
{
	for (MagicViewList::iterator it=m_AttachedEffectObjList.begin(); it!=m_AttachedEffectObjList.end(); ++it)
	{
		if ((*it)->getMagicId() == magicId && (*it)->getLoops() == loops)
		{
			(*it)->release();
			//zgz 这里有bug magicview release方法只是设置了needDelete, 从此之后此magicview既不在场景管理中被管理，也不被CEffectObjHolder被管理，造成内存泄露
			// 这里不从m_AttachedEffectObjList中删除，让它在update中自己释放
			//m_AttachedEffectObjList.erase(it);
		}
	}
}

void CEffectObjHolder::RemoveRelativedObj(ulong magicId, int loops)
{
    for (MagicViewList::iterator it = m_RelativedEffectObjList.begin(); it != m_RelativedEffectObjList.end(); )
    {
        if ((*it)->getMagicId() == magicId && (*it)->getLoops() == loops)
        {
            (*it)->release();
			//zgz 此处可能有内存泄露， 原因见removeAttachObj方法
           it = m_RelativedEffectObjList.erase(it);
        }
		else
		{
			++it;
		}
    }
}

void CEffectObjHolder::close()
{
	for (MagicViewList::iterator it=m_AttachedEffectObjList.begin(); it!=m_AttachedEffectObjList.end(); ++it)
		delete *it;
	m_AttachedEffectObjList.clear();

    for (MagicViewList::iterator it = m_RelativedEffectObjList.begin(); it != m_RelativedEffectObjList.end(); ++it)
    {
        delete *it;
    }
    m_RelativedEffectObjList.clear();
}

void CEffectObjHolder::draw(IRenderSystem* pRenderSystem)
{
	for (MagicViewList::iterator it=m_AttachedEffectObjList.begin(); it!=m_AttachedEffectObjList.end(); ++it)
		(*it)->draw(pRenderSystem);
}

bool CEffectObjHolder::update(float tick, float deltaTick, IRenderSystem* pRenderSystem)
{
	for (MagicViewList::iterator it=m_AttachedEffectObjList.begin(); it!=m_AttachedEffectObjList.end();)
	{
		MagicView* obj = *it;

		if (obj->update(tick, deltaTick, pRenderSystem))
			++it;
		else
			it = m_AttachedEffectObjList.erase(it);
	}
	return true;
}

void CEffectObjHolder::setPosition(const Vector3& pos)
{
	for (MagicViewList::iterator it=m_AttachedEffectObjList.begin(); it!=m_AttachedEffectObjList.end(); ++it)
		(*it)->setPosition(pos);
}

void CEffectObjHolder::onEnterViewport()
{
	for (MagicViewList::iterator it=m_AttachedEffectObjList.begin(); it!=m_AttachedEffectObjList.end(); ++it)
	{
		(*it)->addFlag(flagVisible);
		(*it)->onEnterViewport();
	}
}

void CEffectObjHolder::onLeaveViewport()
{
	for (MagicViewList::iterator it=m_AttachedEffectObjList.begin(); it!=m_AttachedEffectObjList.end(); ++it)
	{
		(*it)->removeFlag(flagVisible);
		(*it)->onLeaveViewport();
	}
}

void CEffectObjHolder::onAddEntity()
{
	for (MagicViewList::iterator it=m_AttachedEffectObjList.begin(); it!=m_AttachedEffectObjList.end(); ++it)
		(*it)->onAddEntity();
}

void CEffectObjHolder::onRemoveEntity()
{
	for (MagicViewList::iterator it=m_AttachedEffectObjList.begin(); it!=m_AttachedEffectObjList.end(); ++it)
		(*it)->onRemoveEntity();
}

void CEffectObjHolder::attach(MagicView* obj)
{
	m_AttachedEffectObjList.push_back(obj);
}

void CEffectObjHolder::detach(const MagicView* obj)
{
	for (MagicViewList::iterator it=m_AttachedEffectObjList.begin(); it!=m_AttachedEffectObjList.end(); ++it)
	{
		if (*it == obj)
		{
			m_AttachedEffectObjList.erase(it);
			break;
		}
	}
}

void CEffectObjHolder::addUpdateOption(ulong option)
{
	for (MagicViewList::iterator it=m_AttachedEffectObjList.begin(); it!=m_AttachedEffectObjList.end(); ++it)
		(*it)->addUpdateOption(option);
}

void CEffectObjHolder::setScale(float scale)
{
	for (MagicViewList::iterator it=m_AttachedEffectObjList.begin(); it!=m_AttachedEffectObjList.end(); ++it)
		(*it)->setScale(scale);
}

void CEffectObjHolder::onFinished(MagicView* ev)
{
	if (ev) ev->setNeedDelete();
}