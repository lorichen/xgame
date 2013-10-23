#ifndef __SceneNode_H__
#define __SceneNode_H__

#include <set>
#include "RenderQueue.h"

namespace xs
{

	/** 模型形成一个树状结构，比如一个模型某骨骼上绑定了个武器，就相当于武器是模型的子Node，
	而这个模型和武器形成的整体又可以作为一个Node挂到另外一个模型的骨骼上
	*/

	class ModelInstance;
	class Bone;

	class _RenderEngineExport ModelNode
	{
	public:

		/**
		*/
		virtual void		release();
		/** 获得完整变换后的AABB
		@return 包围盒
		*/
		virtual const AABB&		getWorldAABB(void) const;

		virtual const Sphere&	getWorldSphere(void) const;

		/** 获取父节点的指针
		@return 父节点指针
		*/
		virtual ModelNode* getParent(void) const;	

		/** 获取方位
		@return 方位4元数
		*/
		virtual const Quaternion &getOrientation() const;	

		/** 设置方位
		@param q 方位四元数
		*/
		virtual void setOrientation(const Quaternion& q);	

		/** 设置方位
		@param w 方位四元数w分量
		@param x 方位四元数x分量
		@param y 方位四元数y分量
		@param z 方位四元数z分量
		*/
		virtual void setOrientation( float w, float x, float y, float z);

		/** 重置方位到初始状态
		*/
		virtual void resetOrientation(void);

		/** 设置节点位置
		@param pos 节点位置
		*/
		virtual void setPosition(const Vector3& pos);

		/** 设置节点位置
		@param x 位置x分量
		@param y 位置y分量
		@param z 位置z分量
		*/
		virtual void setPosition(float x, float y, float z);	

		/** 获得节点位置
		@return 节点位置
		*/
		virtual const Vector3 & getPosition(void) const;	

		/** 设置节点缩放比例
		@param scale 缩放比例
		*/
		virtual void setScale(const Vector3& scale);

		/** 设置节点缩放比例
		@param x 缩放比例x分量
		@param y 缩放比例y分量
		@param z 缩放比例z分量
		*/
		virtual void setScale(float x, float y, float z);

		/** 获得节点缩放比例
		@return 节点缩放比例
		*/
		virtual const Vector3 & getScale(void) const;

		/** 缩放此节点
		@param scale 缩放比例
		*/
		virtual void scale(const Vector3& scale);	

		/** 缩放此节点
		@param x 缩放比例x分量
		@param y 缩放比例y分量
		@param z 缩放比例z分量
		*/
		virtual void scale(float x, float y, float z);	

		/** 移动此节点
		@param d 距离
		@see TransformSpace
		*/
		virtual void translate(const Vector3& d);	

		/** 移动此节点
		@param x 距离的x分量
		@param y 距离的y分量
		@param z 距离的z分量
		@see TransformSpace
		*/
		virtual void translate(float x, float y, float z);

		/** 在自定义的坐标系中移动此节点
		@param axes 自定义的坐标系
		@param move 距离
		@see TransformSpace
		*/
		virtual void translate(const Matrix3& axes, const Vector3& move);

		/** 在自定义的坐标系中移动此节点
		@param axes 自定义的坐标系
		@param x 距离的x分量
		@param y 距离的y分量
		@param z 距离的z分量
		@see TransformSpace
		*/
		virtual void translate(const Matrix3& axes, float x, float y, float z);

		/** 以弧度绕Z旋转
		@param angle 要旋转的弧度
		@see TransformSpace
		*/
		virtual void roll(const Radian& angle);

		/** 以角度绕Z旋转
		@param degrees 要旋转的角度
		@see TransformSpace
		*/
		virtual void roll(float degrees);

		/** 以弧度绕X旋转
		@param angle 要旋转的弧度
		@see TransformSpace
		*/
		virtual void pitch(const Radian& angle);

		/** 以角度绕X旋转
		@param degrees 要旋转的角度
		@see TransformSpace
		*/
		virtual void pitch(float angle);

		/** 以弧度绕Y旋转
		@param angle 要旋转的弧度
		@see TransformSpace
		*/
		virtual void yaw(const Radian& angle);

		/** 以角度绕Y旋转
		@param degrees 要旋转的角度
		@see TransformSpace
		*/
		virtual void yaw(float angle);	

		/** 以弧度绕某个自定义的轴旋转
		@param axis 轴
		@param angle 要旋转的弧度
		@see TransformSpace
		*/
		virtual void rotate(const Vector3& axis, const Radian& angle);

		/** 以角度绕某个自定义的轴旋转
		@param axis 轴
		@param angle 要旋转的角度
		@see TransformSpace
		*/
		virtual void rotate(const Vector3& axis, float angle);

		/** 用给定的四元数旋转
		@param q 四元数
		@see TransformSpace
		*/
		virtual void rotate(const Quaternion& q);	

		/** 获得本地坐标系
		@return 本地坐标系的矩阵
		*/
		virtual Matrix3 getLocalAxes(void) const;

		/** 创建子节点
		@param translate 子节点的偏移量
		@param rotate 子节点的方位
		@return 子节点指针
		*/
		virtual ModelNode* createChild(const char* boneName,const char* modelName,const Vector3& translate = Vector3::ZERO,const Quaternion& rotate = Quaternion::IDENTITY );

		/** 将一个节点作为子节点挂接
		@param child 未来的子节点
		*/
		virtual bool addChild(ModelNode* child,const char* boneName);	

		/** 获得子节点的数量
		@return 子节点的数量
		*/
		virtual ushort numChildren(void) const;	

		/** 获得第index个子节点
		@param index 子节点的下标
		@return 子节点的指针
		*/
		virtual ModelNode* getChild(ushort index) const;	

		/** 删除一个子节点
		@param child 子节点的指针
		@return 此子节点的指针
		*/
		virtual ModelNode* removeChild(ModelNode* child);

		virtual ModelNode* getFirstChildNodeByBone(const char* boneName);
		virtual ModelNode* getNextChildNodeByBone();

		/** 摧毁一个子节点
		*/
		virtual void destroyChild(ModelNode* child);

		/** 摧毁所有子节点
		*/
		virtual void destroyAllChildren(void);

		/** 获得从父节点继承的变换矩阵，这是一个从根节点传递到本节点的变换矩阵
		@return 变换矩阵
		*/
		virtual const Matrix4& getFullTransform(void);

		Bone*	getBoneAttachedTo();

		/** 更新此节点
		@param tick 时间点
		@param tickDelta 时间点Delta
		@param pRenderSystem 渲染系统
		*/
		virtual void update(float tick,float tickDelta,IRenderSystem* pRenderSystem,const ColorValue& diffuse = ColorValue::White);

		/** 更新此节点的色彩数据
		@param pRenderSystem 渲染系统
		@remarks 有时候仅仅需要改变模型的颜色
		*/
		virtual void updateColor(IRenderSystem* pRenderSystem,const ColorValue& diffuse = ColorValue::White);

		virtual void render(IRenderSystem* pRenderSystem,bool useMtl);

		//add by yhc
		//for 影子渲染
		virtual void renderByTime(IRenderSystem* pRenderSystem,bool useMtl,float tickCurrent);

		/** 设置节点是否可见
		*/
		void setVisible(bool bVisible);
	

		/** 通知此节点需要Update,可能是外界环境已经发生了变化
		*/
		virtual void needUpdate();

		/**更新矩阵
		*/
		void updateTransform();

		ModelInstance*	getModelInstance();

		//模型相关

		void setGlobalDiffuse(const ColorValue& color);

		const ColorValue& getSurfaceDiffuse();

		size_t getMemorySize();

		const std::string& getName() const;

		void setName(const std::string& name);

		void setStepLength(int length);

		int getStepLength();

		void setOnlyInheritTranslation(bool bInherit){m_bOnlyInheritTranslation = bInherit;}

		void setBoundingBox(const Rect& rect);
		const Rect& getBoundingBox();
		
		void SetGenerateParticle(bool bGenerate);
		int  GetParticleNum();
	public:
		typedef std::vector<ModelNode*> ChildNodeList;

	protected:
		/// Pointer to parent node
		ModelNode* m_pParent;
		/// Collection of pointers to direct children; hashmap for efficiency
		ChildNodeList m_vChildren;

		/// Stores the orientation of the node relative to it's parent.
		Quaternion m_quatOrientation;

		/// Stores the position/translation of the node relative to its parent.
		Vector3 m_vPosition;

		/// Stores the scaling factor applied to this node
		Vector3 m_vScale;

		/** Internal method for building a Matrix4 from orientation / scale / position. 
		@remarks
		Transform is performed in the order rotate, scale, translation, i.e. translation is independent
		of orientation axes, scale does not affect size of translation, rotation and scaling are always
		centered on the origin.
		*/
		void  makeTransform(
			const Vector3& position, 
			const Vector3& scale, 
			const Quaternion& orientation, 
			Matrix4& destMatrix ) const;

		/** 更新渲染队列，实现需要根据情况是否把自己加到渲染队列里去
		*/
		virtual void		updateRenderQueue(RenderQueue *pQueue);

		/// Cached derived transform as a 4x4 matrix
		mutable Matrix4 mTransform;
		mutable Matrix4 mDerivedTransform;

		ChildNodeList::iterator	m_itBone;
		std::string m_boneNameIt;

		std::string m_strName;
		int m_stepLength;

		RenderQueue		m_renderQueue;

		bool m_bOnlyInheritTranslation;

		Rect			m_boundingBox;

		bool m_bVisible;//节点是否可见，如果不可见，则不显示；

		enum ModelType
		{
			MT_MM,
			MT_MDX,
			MT_MZ,
			MT_TX,
			MT_MX
		};
		ModelType		m_mt;

	public:
		ModelNode();
		ModelNode::~ModelNode();
	public:
		virtual bool		setModel(const char* modelName);
	private:
		AABB	m_AABB;

		Sphere	m_sphere;
		/** Tells the ModelNode to update the world bound info it stores.
		*/
		virtual void  _updateAABB(void );

		ModelInstance*		m_pModelInstance;
		Bone*				m_pBone;

		bool				m_bNeedUpdate;
	private:
		ColorValue			m_surfaceDiffuse;
	};

}

#endif