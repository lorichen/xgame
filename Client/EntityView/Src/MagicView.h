//==========================================================================
/**
* @file	  : MagicView.h
* @author : 
* created : 2008-4-3   10:58
* purpose : 魔法光效对象
*/
//==========================================================================

#ifndef __MAGICVIEW_H__
#define __MAGICVIEW_H__

/// 通用魔法上下文
struct MagicContext_General
{
	ulong		magicId;/// 光效Id
	int			loops;	/// 光效循环次数
	int			nDuration;    //  光效持续时间：可以根据该值来确定光效的持续时间；
	ulong		angle;	/// 光效的角度(当脚本设定矫正光效角度时才有意义)
	handle		owner;	/// 动画拥有者(不一定是技能发起者)
	POINT		tile;	/// 动画位置（如果不是绑定到EntityView对象的话）
	bool		bSceneMagic; /// 整个游戏场景播放的特效，例如下雨、下雪 zgz
	MagicContext_General() : magicId(0), loops(0), angle(0), owner(0) , nDuration(0), bSceneMagic(false){ tile.x = tile.y = 0;}
};

class MagicView;
struct IEffectObserver
{
	virtual void onFinished(MagicView* ev) = 0;
};

/// 魔法光效
class MagicView : public EntityView, public IAnimationCallback, public TimerHandler
{
    enum
    {
        //  延迟删除光效的定时器的ID；
        DELAY_RELEASE_TIME_ID = 0,

        //  光效持续时间的定时器的ID；
        DURATION_TIME_ID = 1,

		// 预加载定时器ID;
		PRELOAD_TIME_ID = 2,
    };

protected:
	handle				mResHandle;		/// 资源句柄
	ulong				mMagicId;		/// 光效的Id(MagicView表的ID)
	ulong				mNewResId;		/// 经过Id分配器分配的新Id
	handle				mOwner;			/// 该光效拥有者
	int					mLoops;			/// 循环次数
	RECT				mShowRect;		/// 实体外围矩形
	bool				mResLoaded;		/// 标识资源已经加载
	ulong				mUpdateOption;	/// 更新选项（一般发生在位置、动画、方向、缩放、速度等改变时）
	bool				mNeedDelete;	/// 是否需要删除
	Vector3				mOffset;		/// 光效偏移
	float				mScale;			/// 缩放比率
	float				mAniSpeed;		/// 动画速度
	//IEffectObserver*	mObserver;		/// 效果动画观察者
	handle				mObserver;		/// 效果动画观察者(改为句柄更安全)
	float				mAlpha;			/// 淡入淡出时的alpha值
	bool				mTailSilence;	/// 是否最后一帧静止
	bool				mKillTimer;
	bool                m_bPetdie;      /// 宠物死亡(wan)

	Vector3				m_vStartPos;	// 出手点
	ulong				m_nSpaceAngle;	// 3D角度	
	Vector3				m_vRotNormal;	// 旋转单位向量

	// A
public:
	MagicView();
	MagicView(ulong entityType, ulong resType);
	virtual ~MagicView();

	void setUserData()
	{
		mHandle = createHandle((ulong)this);
	}
	virtual bool create(const MagicContext_General& context);
	virtual void close();

	virtual const AABB& GetAABB();
	virtual const Matrix4& GetFullTransform(); 

	// IEntityView
public:
	virtual void setTile(const POINT& ptTile);
	virtual bool onCommand(ulong cmd, ulong param1 = 0, ulong param2 = 0) {	return false; }
	virtual void onEnterViewport(int priority = 0);
	virtual void onLeaveViewport();
	virtual void drawPickObject(IRenderSystem* pRenderSystem);
	virtual void drawShadow(IRenderSystem* pRenderSystem)		{ }
	virtual void draw(IRenderSystem* pRenderSystem)				{ drawPickObject(pRenderSystem); }
	virtual void drawTopMost(IRenderSystem* pRenderSystem);
	virtual bool update(float tick, float deltaTick, IRenderSystem* pRenderSystem);
	virtual const RECT& getShowRect() const						{ return mShowRect; }
	virtual OccupantTileList* getOccupantTileList() const		{ return NULL; }
	virtual size_t onLoad(Stream* stream, size_t len);
	virtual size_t onSave(Stream* stream) const;
	virtual void onAddEntity();
	virtual void onRemoveEntity();
	virtual void release();
	virtual void OnTimer(unsigned long dwTimerID);
    virtual void SetAnchorOffset(const POINT& pointAnchorOffset);
	virtual void drawAlwayseTopMost(IRenderSystem* pRenderSystem) {};
	virtual void drawTopMostEffect(IRenderSystem* pRenderSystem) {};

public:
	void setResType(int type)				{ mResType = type; }
	void setResId(ulong id)					{ mResId = id; }
	void setArea(const RECT& rc)			{ m_rcArea = rc; }
	void setSortLeft(const POINT& pt)		{ m_ptLeft = pt; }
	void setSortRight(const POINT& pt)		{ m_ptRight = pt; }
	virtual void setAngle(long angle);
	virtual void setSpace(const Vector3& space);
	void setOccupantValue(ulong occVal)		{ mOccVal = occVal; }
	void setPosition(const Vector3& pos);
	void setTailSilence(bool b)				{ mTailSilence = b; }
	//const EffectContext_General& getContext() const	{ return mContext; }
	//void setOwner(EntityView* owner)			{ mOwner = owner; }
	void setEffectObserver(handle ob)	{ mObserver = ob; }

	ulong getMagicId() const				{ return mMagicId; }
	int getLoops() const					{ return mLoops; }
	bool needDelete() const					{ return mNeedDelete; }
	bool isNeedDelete() const				{ return mNeedDelete; }
	void setNeedDelete()					{ mNeedDelete = true; }
	void addUpdateOption(ulong option)		{ mUpdateOption |= option; }
	void setFadeIn();
	void setFadeOut();
	void setFadeDie();//宠物死亡(wan)
	void setScale(float scale);
	void SetGenerateParticle(bool bGenerate);
	int  GetParticleNum();
protected:
	void requestRes(int priority);
	void releaseRes();
	inline ModelNode* getModelNode();
	void onResLoaded(ModelNode* node);
	void onTileChanged();
	virtual void onOneCycle(const std::string& animation,int loopTimes);

protected:
	// 设置3围空间下的夹角
	void setSpaceAngle(long angle)
	{
		m_nSpaceAngle = angle;
		mUpdateOption |= updateAngle;
	}
	void setRotNormal(Vector3 v)
	{
		m_vRotNormal = v;
	}
public:
	void preLoadRes();
};


/// 能移动的光效
class MagicView_Move : public MagicView//, public TimerHandler
{
protected:
	// 移动相关
	float			mMoveSpeed;				/// 速度

	float			mDirTime;				/// 该方向的所有时间
	ulong			mTicks;					/// 逝去的时间
	ulong			mOldTicks;				/// 上次的tick数
	Vector3			mStartPos;				/// 开始处的位置(方向改变后重置)
	Vector3			mNextPos;				/// 下一个位置
	Vector3			mDirDistance;			/// 同一方向上开始点和下一点的距离

public:
	MagicView_Move();
	MagicView_Move(ulong entityType, ulong resType);
	virtual ~MagicView_Move();

	virtual void close();

	// 移动
public:
	virtual void OnTimer(unsigned long dwTimerID)	{ }
	void stopMove();
	void setSpeed(float speed)	{ mMoveSpeed = speed; }

protected:
	void onPosChanged(const Vector3& pos);
};



/// 射线移动类魔法上下文
struct MagicContext_MoveRadial : public MagicContext_General
{
	float	moveSpeed;	/// 移动速度
	ulong	blockType;	/// 阻挡类型

	MagicContext_MoveRadial() : moveSpeed(1.f), blockType(0) {}
};

/// 射线移动
class MagicViewWithMoveRadial : public MagicView_Move
{
protected:
	ulong			mBlockType;				/// 阻挡类型 0-射线型阻挡方式 1-线段型阻挡方式

public:
	MagicViewWithMoveRadial();
	MagicViewWithMoveRadial(ulong entityType, ulong resType);
	virtual ~MagicViewWithMoveRadial();

	virtual bool create(const MagicContext_MoveRadial& context);
	virtual bool onCommand(ulong cmd, ulong param1 = 0, ulong param2 = 0);
	virtual void OnTimer(unsigned long dwTimerID);
	virtual bool update(float tick, float deltaTick, IRenderSystem* pRenderSystem);
private:
	void moveStep(ulong period);
};




/// 跟踪移动类魔法上下文
struct MagicContext_MoveTrack : public MagicContext_General
{
	float		moveSpeed;	/// 移动速度
	handle		src;		/// 源实体
	handle		target;		/// 目标实体

	MagicContext_MoveTrack() : moveSpeed(1.f), src(0), target(0) { }
};

/// 跟踪移动
class MagicViewWithMoveTrack : public MagicView_Move
{
protected:
	handle			mSource;				/// 源UID
	handle			mTarget;				/// 目标UID
	POINT			mTargetTile;			/// 目标的TILE坐标，当变化时，会触发一些参数的计算
	
	// add by zjp
	AABB			m_TargetAABB;			// 记录目标的AABB
	AABB			m_SourceAABB;			// 自身的AABB
	Matrix4			m_SourceMatrix;
	Matrix4			m_TargetMatrix;

public:
	MagicViewWithMoveTrack();
	MagicViewWithMoveTrack(ulong entityType, ulong resType);
	virtual ~MagicViewWithMoveTrack();

	virtual bool create(const MagicContext_MoveTrack& context);
	virtual bool onCommand(ulong cmd, ulong param1 = 0, ulong param2 = 0);
	virtual void OnTimer(unsigned long dwTimerID);
	virtual bool update(float tick, float deltaTick, IRenderSystem* pRenderSystem);
private:
	void moveStep(ulong period);
	void resetMoveInfo();
	void adjustStartInfo();

private:
	void _getStartPos();
};

//  光效对象持有者；
class CEffectObjHolder : public IEffectObserver
{
	handle			mHandle;
	typedef std::list< MagicView* > MagicViewList;
    //  依附于持有者的光效的列表；
	MagicViewList m_AttachedEffectObjList;
    //  与持有者相关但没有依附于持有者的光效的列表；
    MagicViewList m_RelativedEffectObjList;

public:
	CEffectObjHolder();
	~CEffectObjHolder();

	void addAttachObj(MagicView* obj);
	bool hasAttackObj(ulong magicID);
    void AddRelativedObj(MagicView* obj);	
	void removeAttachObj(ulong magicId, int loops);
    void RemoveRelativedObj(ulong magicId, int loops);
	void close();

	void draw(IRenderSystem* pRenderSystem);
	bool update(float tick, float deltaTick, IRenderSystem* pRenderSystem);
	void setPosition(const Vector3& pos);
	void onEnterViewport();
	void onLeaveViewport();
	void onAddEntity();
	void onRemoveEntity();

	void addUpdateOption(ulong option);
	void setScale(float scale);

	virtual void onFinished(MagicView* ev);

	handle	getHandle()
	{
		return mHandle; 
	}

private:
	void attach(MagicView* obj);
	void detach(const MagicView* obj);
};


#endif // __MAGICVIEW_H__