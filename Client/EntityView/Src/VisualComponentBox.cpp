//==========================================================================
/**
* @file	  : VisualComponentBOX.cpp
* @author : 
* created : 2008-1-30   12:57
*/
//==========================================================================

#include "stdafx.h"
#include "VisualComponentBOX.h"
#include "IGlobalClient.h"
#include "IResourceManager.h"
#include "ISceneManager2.h"
#include "IEntityFactory.h"
#include "MagicView.h"

#define  PIC_SUFFIX ".png"  // ".dds"
VisualComponentBOX::VisualComponentBOX()
{
	// 放高
	mShowRect.left = 0;
	mShowRect.top = 0;
	mShowRect.right = 32;
	mShowRect.bottom = 32;

	m_bIsInit = false;
	m_pTexture = 0;
	m_pImageSet = 0;	
	m_nlastTick = 0;
	m_nEffectID = 0;

	m_bHasGuiException = false;
}

VisualComponentBOX::~VisualComponentBOX()
{
	close();
	safeRelease(m_pTexture);
}

void VisualComponentBOX::create()
{
	getOwner()->setFlag(flag2D | flagInScreen | flagFade | flagDrawName);
	//Info("create-mpw:"<<_fi("0x%08x", (ulong)getOwner())<<endl);	
}

void VisualComponentBOX::close()
{
	getOwner()->removeFlag(flagFade);
	onLeaveViewport();
}

void VisualComponentBOX::onEnterViewport(int priority)
{
	requestRes(priority);
	if (getOwner()->hasFlag(flagFade|flagInScreen))
		setFadeIn();
}

void VisualComponentBOX::onLeaveViewport()
{
	if (isValidHandle(mHandle))
	{
		if (getOwner()->hasFlag(flagFade|flagInScreen))
			setFadeOut();
	}
}

void VisualComponentBOX::requestRes(int priority)
{

}

void VisualComponentBOX::releaseRes()
{
	getOwner()->setNeedDelete();
}

void VisualComponentBOX::draw(IRenderSystem* pRenderSystem)
{
	if( 0 == m_pTexture
		|| 0 ==m_pImageSet)
		return;

	POINT ptScreen;
	gGlobalClient->getSceneManager()->world2Screen(getOwner()->getWorld(), ptScreen);
	RECT rect = getOwner()->getShowRect();
	::OffsetRect(&rect, ptScreen.x, ptScreen.y);
	
	const xsgui::Image & image = m_pImageSet->getImage(m_imageName);
	int nImageWidth = image.getWidth();
	int nImageHeigh = image.getHeight();

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

	Rect rc;
	rc.left = ptScreen.x - nImageHeigh / 2;
	rc.top = rect.top /*- m_foffset*/;
	rc.right = rc.left + nImageWidth;
	rc.bottom = rc.top + nImageHeigh;

	const xsgui::Rect & subrect = image.getSourceTextureArea();
	float width =static_cast<float> (m_pImageSet->getTexture()->getHeight());
	float height =static_cast<float>(m_pImageSet->getTexture()->getWidth());
	xs::Vector2 lefttop( subrect.d_left/ width, subrect.d_top/height);
	xs::Vector2 rightop( subrect.d_right/width, subrect.d_top/height);
	xs::Vector2 rightbottom(subrect.d_right/width, subrect.d_bottom/height);
	xs::Vector2 leftbottom(subrect.d_left/width, subrect.d_bottom/height);
	pRenderSystem->switchTo2D();
	pRenderSystem->rectangle(rc,m_pTexture,lefttop, leftbottom,rightbottom,rightop );
	pRenderSystem->switchTo3D();

	// 播特效,5秒拨一次
	IEntityFactory * pEntityFactory = gGlobalClient->getEntityFactory();
	if(pEntityFactory != NULL && m_nEffectID)
	{		
		long curTick = GetTickCount();
		if( curTick - m_nlastTick > 5 * 1000)
		{
			POINT ptTile;
			gGlobalClient->getSceneManager()->world2Tile(getOwner()->getWorld(), ptTile);		
			pEntityFactory->createEffect(m_nEffectID,ptTile);
			m_nlastTick = curTick;
		}
	}	
}

bool VisualComponentBOX::update(float tick, float deltaTick, IRenderSystem* pRenderSystem)
{
	if (getOwner()->isNeedDelete()/* && mUpdateOption == 0*/)
	{
		if (!getOwner()->hasFlag(flagFade) || (mUpdateOption & updateFadeOut) == 0)
		{
			getOwner()->setNeedDelete(false);
			getOwner()->removeFlag(flagVisible);
			getOwner()->removeFlag(flagFade);
			
			// by yhc安全起见,从场景里面去掉 2010.3.28
			gGlobalClient->getSceneManager()->removeEntity(getOwner()->getTile(), getOwner());
			// end
			delete getOwner();
			return false;
		}
	}	

	if(m_bHasGuiException)
	{
		Error("Gui encoutner a exception!");
		return false;
	}
	
	//更新父类
	//VisualComponent::update( tick, deltaTick, pRenderSystem);
	//更新自己的资源
	if(!InitBigImageSet())
		return false;

	if (getOwner()->hasFlag(flagInScreen))
	{
		if (mUpdateOption && getOwner()->hasFlag(flagFade))
		{

			if (mUpdateOption & updateFadeIn)
			{
				mAlpha += FADE_ALPHA * deltaTick;
				if (mAlpha > 1.0f)
				{
					mAlpha = 1.0f;
					mUpdateOption &= ~updateFadeIn;
				}
			}

			if (mUpdateOption & updateFadeOut)
			{
				mAlpha -= FADE_ALPHA * deltaTick;
				if (mAlpha < 0.f)
				{
					mAlpha = 0.f;
					mUpdateOption &= ~updateFadeOut;
					releaseRes();
				}
			}
		}
	}
	else
	{
		
		if (mUpdateOption && getOwner()->hasFlag(flagFade))
		{

			if (mUpdateOption & updateFadeIn)
			{
				mAlpha += FADE_ALPHA * deltaTick;
				if (mAlpha > 1.0f)
				{
					mAlpha = 1.0f;
					mUpdateOption &= ~updateFadeIn;
				}
			}

			if (mUpdateOption & updateFadeOut)
			{
				mAlpha -= FADE_ALPHA * deltaTick;
				if (mAlpha < 0.f)
				{
					mAlpha = 0.f;
					mUpdateOption &= ~updateFadeOut;
				}
			}
		}
		
	}

	return true;
}

void VisualComponentBOX::handleMessage(ulong msgId, ulong param1, ulong param2)
{
	switch (msgId)
	{
	case msgTileChanged:
		{
			RECT rc;
			
			RECT rcArea;
			rcArea.right = rcArea.left = getOwner()->getTile().x;
			rcArea.bottom = rcArea.top = getOwner()->getTile().y;	// 是单点对象，右上角跟左下角一样
			getOwner()->setArea(rcArea);
			CopyRect(&rc, &rcArea);
		
			updateWorldCoord();

			POINT pt;
			gGlobalClient->getSceneManager()->tile2World((POINT&)rc, pt);
			getOwner()->setSortLeft(pt);

			POINT ptRightTile = {rc.right, rc.bottom};
			gGlobalClient->getSceneManager()->tile2World(ptRightTile, pt);
			getOwner()->setSortRight(pt);

			Vector3 space;
			gGlobalClient->getSceneManager()->tile2Space(getOwner()->getTile(), space);
			getOwner()->setSpace(space);

			//mUpdateOption |= updatePosition;
		}
		break;
	}

	VisualComponent::handleMessage(msgId, param1, param2);
}

size_t VisualComponentBOX::onLoad(Stream* stream, size_t len)
{
	char name[256];
	uchar str_len;
	if (len == 0) return 0;

	// 图片
	stream->read(&str_len, sizeof(uchar));
	stream->read(name, str_len);
	name[str_len] = 0;
	int creatureId = *(int*)name;
	getOwner()->setResId(creatureId);

	// 特效
	stream->read(&str_len, sizeof(uchar));
	stream->read(name, str_len);
	name[str_len] = 0;
	m_nEffectID = *(int*)name;

	// 拉高矩形
	mShowRect.left = -32;
	mShowRect.right = 32;
	mShowRect.top = -32;
	mShowRect.bottom = 32;

	return str_len + sizeof(uchar);
}

bool VisualComponentBOX::InitBigImageSet()
{
	if( !m_bIsInit)
	{	
		ulong resId = getOwner()->getResId();	
		if (resId == 0)
		{
			return false;
		}

		int factor = 49;
		if( resId > 50000)
			factor = 16;

		int imageset = ((int)((resId - 1) / factor + 1)) * factor;
		int m = resId%factor;
		int image = 0;
		if ( m == 0)
		{
			image = factor;
		}
		else
		{
			image = resId % factor;
		}	
		char szBuf[64];

		string  imageSetName;
		string  imageName;

		sprintf(szBuf,"goodsiconset_%d%s",imageset,PIC_SUFFIX);
		imageName = string(szBuf);

		sprintf(szBuf,"%d",imageset);
		imageSetName = string(szBuf);

		sprintf(szBuf,"%d",image);
		m_imageName = string(szBuf);

		try
		{	
			//by yhc ,修正了刘勇说的掉落模糊
			if( 0 == m_pTexture)
				m_pTexture = gGlobalClient->getRenderSystem()->getTextureManager()->createTextureFromFile(imageName,FO_POINT,FO_POINT);
			
			if( 0 == m_pImageSet)
				m_pImageSet = xsgui::ImagesetManager::getSingleton().getImageset(imageSetName);		
			
			m_bIsInit = true;
		}
		catch (...)
		{
			m_bHasGuiException = true;
			m_bIsInit = false;
			Error("Can not find the image,Name"<< imageName <<"VisualComponentBOX::InitBigImageSet");
		}
	}
	return m_bIsInit;
}