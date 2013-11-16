//==========================================================================
/**
* @file	  : FlowText.h
* @author : 
* created : 2008-2-29   10:24
* purpose : 浮动文字及其管理
*/
//==========================================================================

#ifndef __FLOWTEXT_H__
#define __FLOWTEXT_H__

#include "IEntityFactory.h"

struct Interpolate
{
	virtual void create(float first, float last, ulong total_time) = 0;
	virtual float getFrame(ulong time) const = 0;
};

// 匀速运动
class InterpolateLiner : public Interpolate
{
	float _first;
	float _last;
	ulong _time;
	float _v;
public:
	virtual void create(float first, float last, ulong total_time)
	{
		_first = first;
		_last = last;
		_time = total_time;
		_v = (total_time == 0) ? 0.f :(last - first) / (float)total_time;
	}
	virtual float getFrame(ulong time) const
	{
		if (time < _time)
			return _first + _v * (float)time;
		return _last;
	}
};

// 匀加速运动
//vt = v + a*t
//s = v0*t + a*t*t/2
class InterpolateAccelerative : public Interpolate
{
	float _v0;
	float _a;
	ulong _time;
	float _last;
public:
	virtual void create(float first, float last, ulong total_time)
	{
		_v0 = first;
		_a = last;
		_time = total_time;
		_last = _v0*_time + 0.5f*_a*_time*_time;
	}
	virtual float getFrame(ulong time) const
	{
		if (time < _time)
			return  _v0*time + 0.5f*_a*time*time;
		return _last;
	}
};


/// 浮动文字
class FlowText : public TimerHandler
{
public:	
	autostring		mText;		/// 字符串
	ColorValue		mColor;		/// 文字颜色
	xs::Point			mPos;		/// 屏幕位置
	ulong			mTotalTime;	/// 总时间
	CoordAlign		mCoordAlign;/// 坐标对齐方式

	bool			mRunning;	/// 是否还在运行中
	ulong			mCurTime;	/// 当前时间
	SysFont			mFont;		/// 字体


	Interpolate*	mPosX;		/// 位置X
	Interpolate*	mPosY;		/// 位置Y
	Interpolate*	mAlpha;		/// 颜色的alpha值
	Interpolate*	mSize;		/// 文字大小

	FlowText();
	~FlowText();

	bool create(const FlowTextContext& context);
	void close();

	bool isRunning() const			{ return mRunning; }
	void update(ulong deltaTime);
	void render(IRenderSystem* pRenderSystem);

	virtual void OnTimer( unsigned long dwTimerID );
};
/*
struct ColorItem
{
	const wchar_t*	mText;
	ColorValue		mColor;
};

// e.g: <color:FFFF00>FlowTextItem<br><color:FF0000>我们都是中国人</color>haha</color>
struct FlowTextItem
{
	typedef std::list<ColorItem>	ColorItemList;
	ColorItemList	mList;
	float			mScale;
	float			mPosX, mPosY;
	float			mAlpha;

	void update(ulong deltaTime)
	{
	}

	void render(IRenderSystem* pRenderSystem)
	{

	}
};

class FlowTextArea
{
	typedef std::list<FlowTextItem*>	FlowTextItemList;
	FlowTextItemList	mList;
	ulong				mAlign;			/// 区域中的条目对其方式,0-左对齐,1-居中对齐,2-右对齐
	xs::Rect				mRect;			/// 显示区域(相对于主角屏幕坐标的偏移)
public:
	void add(const std::string& text);
	void update(ulong deltaTime);
	void render(const xs::Point& pos, IRenderSystem* pRenderSystem);
private:
	inline int getWidth() const		{ return mRect.right - mRect.left; }
	inline int getHeight() const	{ return mRect.bottom - mRect.top; }
};*/

struct FlowItem
{
	const wchar_t*	text;
	ColorValue	color;
	float offY;
	float scale;
	SysFont font;
};
typedef std::list<FlowItem>	FlowItemList;

/// 浮动区域（里面有多个浮动文字，用于角色旁边的信息提示）
class FlowArea
{
	xs::Rect	mRect;			/// 显示区域(相对于主角屏幕坐标的偏移)
	float	mSpeed;			/// 滚动速度或者alpha淡出的速度
	float	mScale;			/// 字体缩放比例
	bool	mScroll;		/// 是否文字自动滚动
	bool    mIsCover;		/// 是否允许文字覆盖
	bool	mIsEffect;		/// 是否需要动画效果，控制字体的缩放
	bool	mIsChangeA;		/// 是否改变alpha
	FlowItemList mList;
	
public:
	FlowArea();
	~FlowArea();

	bool create(const xs::Rect& rc, float speed, float scale, bool scroll = true,bool isCover = false,bool isEffect = false,bool isChangeA = true);
	void close();
	void add(const std::string& text, const ColorValue& color,const DWORD font = 0);
	void update(ulong deltaTime);
	void render(const xs::Point& pt, IRenderSystem* pRenderSystem);
	bool needRender() const			{ return !mList.empty(); }

	FlowItem* GetLastFlowItem(void)
	{
		if (mList.empty())
		{
			return NULL;
		}
		else
		{
			return &(mList.back());
		}
	}
private:
	bool changePos();
	void update(float offY);
	inline int getWidth() const		{ return mRect.right - mRect.left; }
	inline int getHeight() const	{ return mRect.bottom - mRect.top; }
};



class FlowTextManager : public SingletonEx<FlowTextManager>
{
	typedef std::list<FlowText*>	FlowTextList;
	FlowTextList	mList;				/// 屏幕中的浮动文字列表，主要用于飘血
	FlowArea		mFlowArea[MaxFlowAreaCount-1];
	FlowTextList	mMouseTipsList;		/// 鼠标错误提示列表

public:
	FlowTextManager();
	~FlowTextManager();

	void flowText(const FlowTextContext& context);
	void flowAreaText(int area, const std::string& text, const ColorValue& color,const DWORD font = 0);

	const std::string GetLastFlowString(FlowAreaType area);

	void updateFlowText(ulong deltaTime);
	void updateSystemTips(ulong deltaTime);

	void renderFlowText(IRenderSystem* pRenderSystem);
	void renderSystemTips(IRenderSystem* pRenderSystem);
};

#endif // __FLOWTEXT_H__