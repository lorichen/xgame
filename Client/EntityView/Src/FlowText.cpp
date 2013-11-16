//==========================================================================
/**
* @file	  : FlowText.cpp
* @author : 
* created : 2008-2-29   10:24
*/
//==========================================================================

#include "stdafx.h"
#include "FlowText.h"

static int getTextWidth(const wchar_t* str,DWORD font = fontDefault)
{
	AABB aabb;
	gGlobalClient->getFont(font)->getAABB(str, aabb);
	int w = aabb.getMaximum().x - aabb.getMinimum().x;
	if (w < 0) w = -w;
	return w;
}

FlowText::FlowText() 
: mTotalTime(0), mCurTime(0), mRunning(false), 
mPosX(0), mPosY(0), mAlpha(0), mSize(0),mFont(fontDefault)
{

}

FlowText::~FlowText()
{
	close();
}

bool FlowText::create(const FlowTextContext& context)
{
	if (context.align >= MaxCoordAlignCount)
		return false;
	if (context.flowType >= MaxFlowTypeCount)
		return false;

	close();

	mText = context.text;
	mPos = context.pos;
	mColor = context.color;
	mTotalTime = context.totalTime;
	mCoordAlign = context.align;
	mFont = (SysFont)context.font;

	// 文字矩形区域的中心点与传入的位置对齐
	int w = getTextWidth(mText.c_wstr(),mFont);
	mPos.x -= w >> 1;
	mPos.y -= ((int)gGlobalClient->getFont()->getLineHeight()) >> 1;

	mPosX = mPosY = mAlpha = mSize = NULL;

	if (context.flowType == FlowType_Liner)
	{
		if (context.posOffset.x != 0)
		{
			mPosX = new InterpolateLiner();
			mPosX->create(0.f, context.posOffset.x, context.totalTime);
		}
		if (context.posOffset.y != 0)
		{
			mPosY = new InterpolateLiner();
			mPosY->create(0.f, context.posOffset.y, context.totalTime);
		}
		if (!Math::RealEqual(context.lastAlpha, context.color.a))
		{
			mAlpha = new InterpolateLiner();
			mAlpha->create(context.color.a, context.lastAlpha, context.totalTime);
		}
	}
	else if (context.flowType == FlowType_Accelerative)
	{
		if (!Math::RealEqual(context.a_x, 0.f))
		{
			mPosX = new InterpolateAccelerative();
			mPosX->create(context.v0_x, context.a_x, context.totalTime);
		}
		if (!Math::RealEqual(context.a_y, 0.f))
		{
			mPosY = new InterpolateAccelerative();
			mPosY->create(context.v0_y, context.a_y, context.totalTime);
		}
		if (!Math::RealEqual(context.lastAlpha, context.color.a))
		{
			mAlpha = new InterpolateLiner();
			mAlpha->create(context.color.a, context.lastAlpha, context.totalTime);
		}
	}

	if (!Math::RealEqual(context.scale.effectTime, 0.f))
	{
		mSize = new InterpolateLiner();
		mSize->create(context.scale.firstScale,context.scale.finalScale,context.scale.effectTime);
	}

	gGlobalClient->getTimerAxis()->SetTimer(0, mTotalTime, this, 1, __FILE__);

	mRunning = true;
	return true;
}

void FlowText::close()
{
	if (mRunning)
	{
		gGlobalClient->getTimerAxis()->KillTimer(0, this);
		mText.clear_wstr();
		safeDelete(mPosX);
		safeDelete(mPosY);
		safeDelete(mAlpha);
		safeDelete(mSize);

		mRunning = false;
	}
}

void FlowText::update(ulong deltaTime)
{
	mCurTime += deltaTime;
}

void FlowText::render(IRenderSystem* pRenderSystem)
{
	xs::Point pos;
	pos.x = mPosX ? (ulong)((float)mPos.x + mPosX->getFrame(mCurTime)) : mPos.x;
	pos.y = mPosY ? (ulong)((float)mPos.y + mPosY->getFrame(mCurTime)) : mPos.y;

	if (mCoordAlign == CoordAlign_World)
	{
		xs::Point tmp (pos.x, pos.y);
		gGlobalClient->getSceneManager()->world2Screen(tmp, pos);
	}

	if (mAlpha)
		mColor.a = mAlpha->getFrame(mCurTime);

	float scale = 1.0f;
	if( mSize)
	{
		scale = mSize->getFrame(mCurTime);
		if(scale)
		{
			//int w = getTextWidth(mText.c_wstr(),mFont);
			//pos.x += w>>1;

			//pos.x /= scale;
			//pos.y /= scale;
		}
	}

	//Matrix4 mtxScale = Matrix4::IDENTITY;
	//mtxScale.setScale(Vector3(scale,scale,scale));
	//Matrix4 mtxSave = gGlobalClient->getRenderSystem()->getWorldMatrix();
	//mtxScale = mtxSave * mtxScale;
	//pRenderSystem->setWorldMatrix(mtxScale);	
	//gGlobalClient->getFont(mFont)->render2d(pos.x,pos.y,1,mColor,mText.c_wstr());
	//pRenderSystem->setWorldMatrix(mtxSave);
	
	gGlobalClient->getFont(mFont)->render2dByScale(pos.x, pos.y, 1, mColor, mText.c_wstr(),scale);

}

void FlowText::OnTimer( unsigned long dwTimerID )
{
	close();
}




/*

void FlowTextArea::add(const std::string& text)
{
	const wchar_t* str = autostring::to_wchar(text.c_str());

	FlowTextItem* item = new FlowTextItem;

	// 解析和分拆字符串
	wchar_t c[2] = {0,0};
	int i = 0;
	int off = 0;
	int count = 0;
	int width = 0;
	size_t len = wcslen(str);

	bool isFirst = true;
	ColorValue color = ColorValue(1,1,0,1);

	while (i < len)
	{
		c[0] = str[i];
		if (str[i] == '<') // 标记
		{
			if (i+3<len && wcsnicmp(&str[i], L"<br>", 4) == 0) // 回车
			{
				i += 4;
				continue;
			}
			else if (i+7<len && wcsnicmp(&str[i], L"</color>", 8) == 0) // 颜色出栈
			{
				i += 8;
				continue;
			}
			else if (i+13<len && wcsnicmp(&str[i], L"<color:", 7) == 0) // 颜色压栈
			{
				i += 14;
				continue;
			}
		}

		int charW = getTextWidth(c);
		if (width + charW > getWidth()) // 需要分拆
		{
			FlowItem fi;
			wchar_t* s = new wchar_t[count+1];
			wcsncpy(s, &str[off], count);
			s[count] = 0;
			fi.text = s;
			fi.color = color;
			fi.offY = offY;
			if (isFirst) changePos();
			isFirst = false;
			mList.push_back(fi);

			off = i;
			count = 1;
			width = charW;
			offY -= mTextHeight;
		}
		else
		{
			width += charW;
			count++;
		}

		i++;
	}

	if (count >= 1)
	{
		FlowItem fi;
		wchar_t* s = new wchar_t[count+1];
		wcsncpy(s, &str[off], count);
		s[count] = 0;
		fi.text = s;
		fi.color = color;
		fi.offY = offY;
		if (isFirst) changePos();
		isFirst = false;
		mList.push_back(fi);
	}

	delete str;
}

void FlowTextArea::update(ulong deltaTime)
{

}

void FlowTextArea::render(const xs::Point& pos, IRenderSystem* pRenderSystem)
{

}


*/








FlowArea::FlowArea() : mSpeed(1), mScale(1),mScroll(true),mIsCover(false),mIsEffect(false)
{
}

FlowArea::~FlowArea()
{
	close();
}

bool FlowArea::create(const xs::Rect& rc, float speed, float scale, bool scroll /* = true */,bool isCover /* = false */,bool isEffect /* = false */,bool isChangeA /* = true */)
{
	mRect = rc;
	mSpeed = speed;
	mScale = scale;
	mIsCover = isCover;
	mRect.left /= mScale;
	mRect.right /= mScale;
	mRect.top /= mScale;
	mRect.bottom /= mScale;
	mScroll = scroll;
	mIsEffect = isEffect;
	mIsChangeA = isChangeA;
	return true;
}

void FlowArea::close()
{
	for (FlowItemList::iterator it=mList.begin(); it!=mList.end(); ++it)
	{
		FlowItem& item = (*it);
		delete item.text;
	}
	mList.clear();
}

void FlowArea::add(const std::string& text, const ColorValue& color,const DWORD font)
{
	const wchar_t* str = autostring::to_wchar(text.c_str());

	FlowItem item;
	item.text = str;
	item.color = color;
	item.scale = 1.0f;
	item.font = (SysFont)font;
	item.offY = 0;//mList.empty() ? 0 : mList.back().offY;

	// 分拆长字符串
	int w = getTextWidth(str);
	if (w > getWidth()) // 太长
	{
		wchar_t c[2] = {0,0};
		int i = 0;
		int off = 0;
		int count = 0;
		int width = 0;
		float offY = item.offY;
		bool isFirst = true;
		while (str[i])
		{
			c[0] = str[i];
			int charW = getTextWidth(c);
			if (width + charW > getWidth()) // 需要分拆
			{
				FlowItem fi;
				wchar_t* s = new wchar_t[count+1];
				wcsncpy(s, &str[off], count);
				s[count] = 0;
				fi.text = s;
				fi.color = color;
				fi.scale = 1.0f;
				fi.offY = offY;
				fi.font = (SysFont)font;
				if (isFirst) changePos();
				isFirst = false;
				mList.push_back(fi);

				off = i;
				count = 1;
				width = charW;
				offY -= gGlobalClient->getFont(font)->getLineHeight();
			}
			else
			{
				width += charW;
				count++;
			}

			i++;
		}

		if (count >= 1)
		{
			FlowItem fi;
			wchar_t* s = new wchar_t[count+1];
			wcsncpy(s, &str[off], count);
			s[count] = 0;
			fi.text = s;
			fi.color = color;
			fi.offY = offY;
			fi.font = (SysFont)font;
			if (isFirst) changePos();
			isFirst = false;
			mList.push_back(fi);
		}

		delete str;
	}
	else
	{
		changePos();
		mList.push_back(item);
	}
}

void FlowArea::update(ulong deltaTime)
{
	if (mList.empty())
		return;

	// update
	update(deltaTime * mSpeed);
}

void FlowArea::render(const xs::Point& pt, IRenderSystem* pRenderSystem)
{
	if (mList.empty())
		return;

	// render
	float x = pt.x;
	float y = (float)(pt.y + mRect.bottom);
	for (FlowItemList::iterator it=mList.begin(); it!=mList.end(); ++it)
	{
		FlowItem& item = (*it);
		float xx = x;
		float yy = (y - item.offY);

		// 动画效果居中
		if(mIsEffect)
		{
			int w = getTextWidth(item.text,item.font);
			xx -=(w*item.scale/2);
		}

		ColorValue color = item.color;
		if( !mIsChangeA)
			color.a = 1.0f;
	
		if(mIsEffect)
			gGlobalClient->getFont(item.font)->render2dByScale(xx, yy, 1, color, item.text,item.scale);
		else
			gGlobalClient->getFont()->render2d(xx, yy, 1, color, item.text);	
	}
}

bool FlowArea::changePos()
{
	if (mList.size() > 0 && !mIsCover)
	{
		FlowItem& back = mList.back();
		int fontHeight = gGlobalClient->getFont(back.font)->getLineHeight();
		if (back.offY < fontHeight)
		{
			float delta = fontHeight - back.offY;
			float last = fontHeight;
			for (FlowItemList::reverse_iterator it=mList.rbegin(); it!=mList.rend();)
			{
				FlowItem& item = (*it);

				if (item.offY > last)
					break;

				last += fontHeight;
				item.offY += delta;/*
				if (item.offY > getHeight())
				{
					delete item.text;
					it = mList.erase(it);
				}
				else*/
				{
					++it;
				}
			}

			for (FlowItemList::iterator it=mList.begin(); it!=mList.end();)
			{
				FlowItem& item = (*it);
				//item.offY += mTextHeight - back.offY;
				if (item.offY > getHeight())
				{
					delete item.text;
					it = mList.erase(it);
				}
				else
					++it;
			}
			return true;
		}
	}
	return false;
}

void FlowArea::update(float offY)
{
	if (mScroll)
	{
		for (FlowItemList::iterator it=mList.begin(); it!=mList.end();)
		{
			FlowItem& item = (*it);
			item.offY += offY;

			if( mIsChangeA)
				item.color.a = ((float)getHeight() - item.offY) / (float)getHeight() + 0.3f;

			if( mIsEffect && item.offY > getHeight()/50 && item.scale > mScale )
			{
				item.scale -= (offY/3);
				if(item.scale < mScale)
					item.scale = mScale;
			}

			if (item.offY > getHeight())
			{
				delete item.text;
				it = mList.erase(it);
			}
			else
				++it;
		}
	}
	else
	{
		for (FlowItemList::iterator it=mList.begin(); it!=mList.end();)
		{
			FlowItem& item = (*it);
			item.color.a -= offY/1000;

			if (item.color.a <= 0)
			{
				delete item.text;
				it = mList.erase(it);
			}
			else
				++it;
		}
	}
}





FlowTextManager::FlowTextManager()
{
	xs::Rect rc1 (0,-200,100,-150);
	mFlowArea[FlowArea_Top].create(rc1, 0.03, 0.7,true,true,true);

	xs::Rect rc2 (-50,0,100,200);
	mFlowArea[FlowArea_Bottom].create(rc2, 0.05, 0.8);

	xs::Rect rc3 (100,100,400,200);
	mFlowArea[FlowArea_Right].create(rc3, 0.05, 1,true,false,false,false);

	//xs::Rect rc4 = {-100,-220,100,-20};
	//mFlowArea[FlowArea_Mouse].create(rc4, 0.0005, 0.8, false);
}

FlowTextManager::~FlowTextManager()
{
	for (int i=0; i<MaxFlowAreaCount-1; i++)
		mFlowArea[i].close();

	for (FlowTextList::iterator it=mList.begin(); it!=mList.end(); ++it)
		delete (*it);

	mList.clear();
}

void FlowTextManager::flowText(const FlowTextContext& context)
{
	FlowText* ft = new FlowText();
	if (ft->create(context))
	{
		mList.push_back(ft);
	}
}

void FlowTextManager::flowAreaText(int area, const std::string& text, const ColorValue& color,const DWORD font)
{
	if (area == FlowArea_Mouse)
	{
		// 获取鼠标位置
	
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient((HWND)gGlobalClient->getHWND(), &pt);

		FlowTextContext ftc;
		memset(&ftc, 0, sizeof(ftc));
		ftc.flowType = FlowType_Liner;
		ftc.align = CoordAlign_Screen;
		ftc.color = color;
		ftc.lastAlpha = 0.1f;
		ftc.pos.x = pt.x;
		ftc.pos.y = pt.y;
		ftc.pos.y -= 15;
		ftc.text = text;
		ftc.totalTime = 2000;

		FlowText* ft = new FlowText();
		if (ft->create(ftc))
			mMouseTipsList.push_back(ft);
		return;
	}

	if (area >= 0 && area < MaxFlowAreaCount-1)
	{
		mFlowArea[area].add(text, color, font);
	}
}

const std::string FlowTextManager::GetLastFlowString(FlowAreaType area)
{
	// 对右边窗口，与窗口最后一条提示重复的将不被显示
	FlowItem* pFlowItem = mFlowArea[area].GetLastFlowItem();
	if (pFlowItem == NULL)
	{
		return "";
	}

	int nBufSize = ::WideCharToMultiByte(GetACP(), 0, pFlowItem->text, -1, NULL, 0, 0, FALSE);
	char *szBuf = new char[nBufSize];
	::WideCharToMultiByte(GetACP(), 0, pFlowItem->text, -1, szBuf, nBufSize, 0, FALSE);
	string strRet(szBuf);
	delete []szBuf;
	szBuf = NULL;
	return strRet;
}

void FlowTextManager::updateFlowText(ulong deltaTime)
{
	for (FlowTextList::iterator it=mList.begin(); it!=mList.end();)
	{
		FlowText*& ft = *it;
		if (ft->isRunning())
		{
			ft->update(deltaTime);
			++it;
		}
		else
		{
			delete ft;
			it = mList.erase(it);
		}
	}
}

void FlowTextManager::renderFlowText(IRenderSystem* pRenderSystem)
{
	for (FlowTextList::iterator it=mList.begin(); it!=mList.end();++it)
	{
		FlowText*& ft = *it;
		ft->render(pRenderSystem);
	}
}

void FlowTextManager::updateSystemTips(ulong deltaTime)
{
	mFlowArea[FlowArea_Top].update(deltaTime);
	mFlowArea[FlowArea_Bottom].update(deltaTime);
	mFlowArea[FlowArea_Right].update(deltaTime);

	for (FlowTextList::iterator it=mMouseTipsList.begin(); it!=mMouseTipsList.end();)
	{
		FlowText*& ft = *it;
		if (ft->isRunning())
		{
			ft->update(deltaTime);
			++it;
		}
		else
		{
			delete ft;
			it = mMouseTipsList.erase(it);
		}
	}
}


void FlowTextManager::renderSystemTips(IRenderSystem* pRenderSystem)
{
	EntityView* player = (EntityView*)getHandleData(gGlobalClient->getPlayer());
	xs::Point ptScreen;
	if (player)	
	{
		 //获取玩家位置
		gGlobalClient->getSceneManager()->world2Screen(player->getWorld(), ptScreen);

		// 头顶脚底的区域必须跟着玩家移动，尤其是在地图边界玩家位置不居中的时候
		mFlowArea[FlowArea_Top].render(ptScreen, pRenderSystem);
		mFlowArea[FlowArea_Bottom].render(ptScreen, pRenderSystem);
	}

	const xs::Rect& rc = gGlobalClient->getSceneManager()->getViewportRect();
	ptScreen.x = (rc.left + rc.right) / 2 - gGlobalClient->getSceneManager()->getViewTopLeftX();
	ptScreen.y = (rc.top + rc.bottom) / 2 - gGlobalClient->getSceneManager()->getViewTopLeftY();
	mFlowArea[FlowArea_Right].render(ptScreen, pRenderSystem);

	/*
	if (mFlowArea[FlowArea_Mouse].needRender())
	{
	// 获取鼠标位置
	xs::Point ptScreen;
	GetCursorPos(&ptScreen);
	ScreenToClient((HWND)gGlobalClient->getHWND(), &ptScreen);

	mFlowArea[FlowArea_Mouse].render(ptScreen, deltaTime);
	}*/

	for (FlowTextList::iterator it=mMouseTipsList.begin(); it!=mMouseTipsList.end();++it)
	{
		FlowText*& ft = *it;
		ft->render(pRenderSystem);
	}
}

