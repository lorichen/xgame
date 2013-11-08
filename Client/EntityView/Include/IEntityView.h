//==========================================================================
/**
* @file	  : IEntityView.h
* @author : 
* created : 2008-1-21   9:54
* purpose : 实体视图对象接口
*/
//==========================================================================

#ifndef __IENTITYVIEW_H__
#define __IENTITYVIEW_H__

#include <string>

#include "EntityDef.h"
#include "Occupant.h"

class xs::RenderQueue;

class EntityView;

/// 用于实体的回调通知
struct IEntityViewObserver
{
	virtual void onTileChanged(EntityView* owner, const xs::Point& curTile) {}
	virtual void onMoveFinished(EntityView* owner)						{}
	virtual void onMoveBreak(EntityView* owner)							{}
};


/// 实体视图
class EntityView
{
protected:
	handle			mHandle;		/// 实体句柄
	ulong			mFlags;			/// 实体的标志位
	ulong			mLayer;			/// 对象层次，用于排序
	ulong			mOccVal;		/// 占位值
	ulong			mData;			/// 用户数据

	ulong			mResId;			/// 对象的资源ID(只读，对于生物来讲是生物Id)
	ulong			mSaveId;		/// 获取变形前保存的模型资源
	ulong			mResType;		/// 资源类型(只读)
	ulong			mEntityType;	/// 实体类型(逻辑方面，如静态物件，NPC)

	xs::Point			m_ptTile;		/// 对象在地图上的TILE位置
	xs::Rect			m_rcArea;		/// 占位区域
	xs::Point			m_ptWorld;		/// 对象锚点在地图上对应的世界坐标
	xs::Point			m_ptLeft;		/// 左边排序点
	xs::Point			m_ptRight;		/// 右边排序点
	ulong			mAngle;			/// 实体的方向[0,360)
	Vector3			mSpace;			/// 实体在3D世界中的Space坐标
	IEntityViewObserver* mObserver;	/// 实体观察者

public:
	AABB			m_AABB;
	Matrix4			m_FullTransform;

public:
	EntityView() : mHandle(INVALID_HANDLE), mData(0), mFlags(0), mLayer(lvlMidle), mOccVal(0x2000), mAngle(0), mEntityType(typeUnknow), mObserver(0)
	{
		m_ptTile.x = m_ptTile.y = 0;
        
		//SetRectEmpty(&m_rcArea);
        m_rcArea.left = m_rcArea.right = m_rcArea.top = m_rcArea.bottom = 0;
		
        m_ptWorld.x = m_ptWorld.y = 0;
		m_ptLeft.x = m_ptLeft.y = 0;
		m_ptRight.x = m_ptRight.y = 0;
        m_pointAnchorOffset.x = 0;
        m_pointAnchorOffset.y = 0;
		mSaveId = 0;
	}

	virtual ~EntityView()							
	{ if (mHandle != INVALID_HANDLE) closeHandle(mHandle); 
#ifdef MEM_LEAK_TEST
		gGlobalClient->ReleaseEntityView(this);
#endif
	}


	/// 释放
	virtual void release() = 0;

	virtual const AABB& GetAABB() = 0;

	virtual const Matrix4& GetFullTransform() = 0;

	//////////////////////////////////////////////////////////////////////////
	// 基础属性相关
	//////////////////////////////////////////////////////////////////////////
	/// 获取实体的句柄（只有逻辑对象才设置句柄）
	inline handle getHandle() const					{ return mHandle; }

	/// 获取对象的资源ID
	inline ulong getResId() const					{ return mResId; }

	// 设置变形前的ID
	//add by zjp；保存变形前的宠物
	ulong getsaveId()								{return mSaveId;}
	void saveResId(ulong id)						{ mSaveId = id; } 

	/// 设置用户数据(目前仅仅用于记录逻辑对象的指针)
	inline void setUserData(ulong data)
	{
		if (mHandle == INVALID_HANDLE && data) mHandle = createHandle((ulong)this);
		mData = data;
		//if (mData && mHandle) { closeHandle(mHandle); mHandle=INVALID_HANDLE;}
		//if (mData = data) mHandle = createHandle((ulong)this);
	}

	/// 获得用户数据
	inline ulong getUserData() const				{ return mData; }

	/// 设置实体类型
	inline void setEntityType(ulong entityType)		{ mEntityType = entityType; }

	/// 获取实体类型
	inline ulong getEntityType() const				{ return mEntityType; }

	/// 获取资源类型
	inline ulong getResType() const					{ return mResType; }

	/// 设置实体观察者
	inline void setEntityViewObserver(IEntityViewObserver* ob)	{ mObserver = ob; }

	/// 获取实体观察者
	inline IEntityViewObserver* getEntityViewObserver() const	{ return mObserver; }




	//////////////////////////////////////////////////////////////////////////
	// 坐标/位置相关
	//////////////////////////////////////////////////////////////////////////
	/// 设置当前Tile(只能由场景管理器调用)
	virtual void setTile(const xs::Point& ptTile) = 0;

	/// 获得当前Tile坐标
	inline const xs::Point& getTile() const				
	{ 
		return m_ptTile; 
	}

	/// 设置对象的世界坐标
	inline void setWorld(const xs::Point& ptWorld)		{ m_ptWorld = ptWorld; };

	/// 获取实体的世界坐标
	inline const xs::Point& getWorld() const			{ return m_ptWorld; }

	/// 获取实体的占位块区域
	inline const xs::Rect& getArea() const				{ return m_rcArea; }

	/// 获取左排序点
	inline const xs::Point& getSortLeft() const			{ return m_ptLeft; }

	/// 获取右排序点
	inline const xs::Point& getSortRight() const		{ return m_ptRight; }

	/// 设置实体面向的角度
	virtual void setAngle(long angle) = 0;

	/// 获取实体面向的角度
	inline long getAngle() const					{ return mAngle; }

	/// 设置Space坐标
	virtual void setSpace(const Vector3& space) = 0;

	/// 获取Space坐标
	inline const Vector3& getSpace() const			{ return mSpace; }

    //  设置缩放比：在MagicView中重写，用于设置特效缩放比；
    inline virtual void setScale(float /*scale*/)
    {
    }

	// add by zjp；获取受击点
	virtual xs::Point getAttackPos()
	{
		xs::Point pt(0,0);
		return pt;
	}


	//////////////////////////////////////////////////////////////////////////
	// 视图相关
	//////////////////////////////////////////////////////////////////////////
	/// 获得实体的矩形框(2D包围框)，主要用于决策对象是否在屏幕范围之内
	virtual const xs::Rect& getShowRect() const = 0;

	/// 绘制实体
	virtual void draw(IRenderSystem* pRenderSystem) = 0;

	/// 绘制可选取的对象(一个实体可能有多个部分，但只有某些部分是可以被选取的)
	virtual void drawPickObject(IRenderSystem* pRenderSystem) = 0;

	/// 绘制影子
	virtual void drawShadow(IRenderSystem* pRenderSystem) = 0;

	/// 绘制顶层对象，比如血条，名称等等
	virtual void drawTopMost(IRenderSystem* pRenderSystem) = 0;

    /// 绘制永久显示的顶层对象，比如任务图标等等
	virtual void drawAlwayseTopMost(IRenderSystem* pRenderSystem) = 0;

	/// 绘制顶层特效
	virtual void drawTopMostEffect(IRenderSystem* pRenderSystem) = 0;
	/** 更新实体
	@param tick				当前的时间片
	@param deltaTick		2次update间隔的时间差
	@param pRenderSystem	渲染系统
	@param pCamera			摄像机
	@param retval			返回false表示该对象已释放，否则返回true
	*/
	virtual bool update(float tick, float deltaTick, IRenderSystem* pRenderSystem) = 0;

	/** 当实体进入视口
	@param priority			资源加载的优先级参数
	*/
	virtual void onEnterViewport(int priority = 0) = 0;

	/// 当实体离开视口
	virtual void onLeaveViewport() = 0;

	/// 判断某个相对于显示矩形的位置是否透明（主要用于物件）
	virtual bool isTransparent(int x, int y)	{ return false; }

	/// 当实体添加到地图上面的时候的回调
	virtual void onAddEntity()					{ }

	/// 当实体从地图上面移除的时候的回调
	virtual void onRemoveEntity()				{ }




	//////////////////////////////////////////////////////////////////////////
	// 序列化相关
	//////////////////////////////////////////////////////////////////////////
	/** 从流中加载数据
	@param stream 用于读取的流对象
	@param len 限制读取的最大长度
	@retval 成功返回实际读取的长度，失败返回0
	*/
	virtual size_t onLoad(Stream* stream, size_t len) = 0;

	/** 写数据到指定的流对象
	@param stream 用于保存数据的流对象
	@retval 成功返回实际写入的数据长度，失败返回0
	*/
	virtual size_t onSave(Stream* stream) const = 0;




	//////////////////////////////////////////////////////////////////////////
	// 命令相关
	//////////////////////////////////////////////////////////////////////////
	/** 给实体发送命令
	@param cmd 命令码
	@param param1 命令参数1
	@param param2 命令参数2
	*/
	virtual bool onCommand(ulong cmd, ulong param1 = 0, ulong param2 = 0) = 0;



	//////////////////////////////////////////////////////////////////////////
	// 标志相关
	//////////////////////////////////////////////////////////////////////////
	/** 设置标志
	@param flag 标志值
	*/
	inline void setFlag(ulong flag = 0)					{ mFlags = flag; }

	/// 取得标志位
	inline ulong getFlag() const						{ return mFlags; }

	/** 增加标志
	@param flag 标志位类型，参考 EntityFlags 枚举
	*/
	inline void addFlag(ulong flag)						{ mFlags |= flag; }

	/**删除标志
	@param flag 标志位类型，参考 EntityFlags 枚举
	*/
	inline void removeFlag(ulong flag)					{ mFlags &= ~flag; }

	/// 判断是否具有某些标志
	inline bool hasFlag(ulong flag) const				{ return (mFlags & flag) == flag; }




	//////////////////////////////////////////////////////////////////////////
	// 排序相关
	//////////////////////////////////////////////////////////////////////////
	/// 获得实体层级别（参考 SortLevel 枚举）
	inline ulong getLayer() const						{ return mLayer; }

	/// 设置层级别
	inline void setLayer(ulong layer)					{ mLayer = layer; }

	/// 获取占位值（参考占位类型定义，逻辑对象，我们返回指定的值）
	inline ulong getOccupantValue() const				{ return hasFlag(flagNoLogic) ? mOccVal : mOccVal | 0x70000; }

	/// 设置占位值
	inline void setOccupantValue(ulong occ)				{ mOccVal = occ; }

	/// 获取占位列表
	virtual OccupantTileList* getOccupantTileList() const { return NULL; }

    //  获取锚点偏移；
    inline xs::Point GetAnchorOffset() const
    {
        return m_pointAnchorOffset;
    }

    //  设置锚点偏移；
    inline virtual void SetAnchorOffset(const xs::Point& pointAnchorOffset)
    {
        m_pointAnchorOffset.x = pointAnchorOffset.x;
        m_pointAnchorOffset.y = pointAnchorOffset.y;
    }

protected:
    //  锚点偏移的记录：锚点偏移是指在场景设置中放置物件时，鼠标在物件锚点所在tile中，相对于tile的中心位置的偏移；
    //  记录该值用于使物件可以以像素为单位进行移动和放置；
    xs::Point m_pointAnchorOffset;
};

#endif // __IENTITYVIEW_H__
