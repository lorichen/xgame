#ifndef __Model_H__
#define __Model_H__

#include "RenderEngine/Manager.h"
#include "SubModel.h"
#include "CollisionDetection.h"

//namespace RE
namespace xs
{
	class Model;
    class IModelCallback;
	struct IRenderSystem;

	class Model : public IModel
	{
	public:
		#define MAX_REPLACABLE_SKIN_COUNT		3
		struct CreatureSkin
		{
			std::string strFileName[MAX_REPLACABLE_SKIN_COUNT];
			uint32	ui32Base;
		};
		friend struct SubModel;
	public:
		/** 更新模型几何部件
		@param frame 帧
		@param pRenderSystem 渲染系统
		@param pVBVertexBlend 顶点混合缓冲区
		*/
		virtual void	updateGeometry(uint32 frame,IRenderSystem *pRenderSystem,IVertexBuffer *pVBVertexBlend);

		/** 更新模型效果部件
		@param pRenderSystem 渲染系统
		*/
		virtual void	updateFX(IRenderSystem *pRenderSystem);

		/** 绘制模型
		@param pRenderSystem 渲染系统接口指针
		@param pCallback 回调指针
		*/
		virtual void	render(IRenderSystem* pRenderSystem,IModelCallback *pCallback = 0);

		/** 获得文件名
		@return 文件名
		*/
		virtual const std::string& getFileName() ;

		/** 获得包围盒数据
		@return 模型包围盒
		*/
		virtual const AABB&	getAABB() ;

		/** 获得包围球数据
		@return 模型包围球
		*/
		virtual const Sphere& getBoudingSphere() ;

		/** 获得模型动画数目
		@return 模型动画数
		*/
		virtual uint32	getAnimationCount() ;

		/** 是否拥有某个动画
		@param animation 动画名称
		@see enAnimation
		@return 如果存在animation动画则返回此动画数据，否则返回0
		*/
		virtual Animation* hasAnimation(const std::string& animation) ;

		/** 获得可以换的皮肤数量
		@return 皮肤数量
		*/
		virtual uint32	getSkinCount() ;

		/** 设置当前皮肤
		@param index 皮肤数组下标
		@return 如果拥有此皮肤，则返回true，反之false
		*/
		virtual bool setSkin(uint32 index) ;

		/** 获得贴图指针
		@param index 第index个贴图
		@return 贴图指针
		*/
		virtual ITexture* getTexture(uint32 index) ;\
			virtual bool hasBone(CPChar szBoneName) ;\
			virtual const Matrix4& getBoneMatrix(const char* szBoneName) ;\
			virtual const Vector3& getBonePivot(const char* szBoneName) ;\

			/** 获得子模型的数量
			@return 子模型的数量
			*/
			virtual uint32 getNumSubModels() ;

		/** 设置某个子模型的可见性
		@param index 子模型数组的下标
		@param bVisible 是否可见
		*/
		virtual void	setSubModelVisible(uint32 index,bool bVisible) ;

		/** 获得碰撞检测模型
		@return 模型碰撞检测模型
		*/
		virtual void*	getCollisionModel() ;

		/** 获得碰撞检测包围盒数据
		@return 碰撞检测包围盒
		*/
		virtual const AABB& getCollisionAABB() ;

		/** 渲染模型
		@param pRenderSystem 渲染系统接口指针
		*/
		virtual void	render_Instancing(IRenderSystem* pRenderSystem) ;

		/** 设置几何数据
		@param tick 当前tick
		@param tickDelta 本次tick和上次tick时间差
		@param pRenderSystem 渲染系统接口指针
		@param pCamera 摄像机指针
		@param animation 动画名称
		*/
		virtual void	setGeometry_Instancing(IRenderSystem* pRenderSystem,IVertexBuffer *pVBVertexBlend) ;

		/** 清除几何数据
		@param pRenderSystem 渲染系统接口指针
		*/
		virtual void	resetGeometry_Instancing(IRenderSystem* pRenderSystem) ;

		/** 设置子模型的渲染参数
		@param index 子模型数组的下标
		@param pRenderSystem 渲染系统接口指针
		*/
		virtual void	setGeometrySubModel_Instancing(uint32 index,IRenderSystem* pRenderSystem) ;

		/** 清除子模型的渲染参数
		@param index 子模型数组的下标
		@param pRenderSystem 渲染系统接口指针
		*/
		virtual void	resetGeometrySubModel_Instancing(uint32 index,IRenderSystem* pRenderSystem) ;

		/** 渲染子模型
		@param index 子模型数组的下标
		@param pRenderSystem 渲染系统接口指针
		*/
		virtual void	renderSubModel_Instancing(uint32 index,IRenderSystem* pRenderSystem) ;

		/** 渲染特效
		@param tick 当前tick
		@param tickDelta 本次tick和上次tick时间差
		@param pRenderSystem 渲染系统接口指针
		@param pCamera 摄像机指针
		*/
		virtual void	renderFX_Instancing(Real tick,Real tickDelta,IRenderSystem* pRenderSystem,Camera* pCamera) ;

		/**
		#return 获得VertexBlend顶点缓冲区，用于对每个模型的实例做VertexBlend
		*/
		virtual IVertexBuffer*	getVertexBlendBuffer() ;
	public:
		Model(const std::string& name,IRenderSystem* pRenderSystem);
		virtual ~Model();
		void	update(Real tick,Real tickDelta,IRenderSystem *pRenderSystem);

		IRenderSystem*		m_pRenderSystem;
	private:
		AABB	m_AABB;
		Sphere	m_BoundingSphere;

		std::string	m_strFilename;
	public:
		void	onGetBoundingVolumeData(Vector3 *p,uint32 ui32Vertices,Vector3 *pNormals,uint32 ui32Normals,uint16 *pIndices,uint32 ui32Indices);
		void	onGetName(char *szName);
		void	onGetVerticePositions(Vector3 *pVertices,uint32 ui32Vertices,uint32 ui32Stride);
		void	onGetVerticeNormals(Vector3 *pVertices,uint32 ui32Vertices,uint32 ui32Stride);
		void	onGetAnimatedVertice(AnimatedVertex *pVertices,uint32 ui32Vertices,uint32 ui32Stride);
		void	onGetVerticeTexcoords(Vector2 *pVertices,Uint32 ui32Vertices,Uint32 ui32Stride);
		void	onGetTexture(Uint32 index,Bool bReplacableTexture,Int32 type,CPChar fileName);
		void	onGetIndices(Uint16 *pIndices,Uint32 ui32Indices);
		void	onGetRenderPasses(const std::vector<SubModel>& vRenderPasses);
		void	onGetGlobalSequences(Int32 *pGlobalSequences,Uint32 ui32GlobalSequences);
		ParticleSystem*	onGetParticleSystem();
		RibbonSystem*	onGetRibbonSystem();
		Skeleton*	onGetSkeleton();
		Animation*	onGetAnimation(Uint32 start,Uint32 end,Bool bLoop,Int16 id,Uint32 rangeIndex);
		void	onGetSkin(const std::vector<CreatureSkin>& vSkin);
		void	onGetIsAnimated(Bool bAnimatedBones,Bool bAnimatedGeometry,Bool bNeedPerInstanceAnimation);
		ITexture*	getTexture(Int16 index){return m_textures[index];}
		Bone*		getBone(Int16 bone);
	private:
		IVertexBuffer	*m_pVBBoundingVertices;
		IVertexBuffer	*m_pVBBoundingNormals;
		IIndexBuffer	*m_pIBBounding;
		std::string			m_strModelName;
		IVertexBuffer	*m_pVBVertex;
		IVertexBuffer	*m_pVBNormal;
		IVertexBuffer	*m_pVBVertexNormal;			//for Animated models
		AnimatedVertex	*m_pOriginVertices;			//for Animated models
		IVertexBuffer	*m_pVBTexcoord;
		//Textures - Static Textures & Replacable Textures
		ITexture*		m_textures[32];
		Bool			m_useReplacableTextures[32];
		ITexture*		m_replacableTextures[32];
		Int32			m_replacableTextureTypes[32];
		IIndexBuffer	*m_pIB;
		std::vector<SubModel> m_vRenderPasses;
		Int32*			m_pGlobalSequences;
		std::vector<ParticleSystem*>	m_vParticleSystems;
		std::vector<RibbonSystem*>		m_vRibbonSystems;
		Skeleton*		m_pSkeleton;
		std::vector<Animation*>		m_vAnimations;
		std::hash_map<std::string,Animation*>	m_AnimationMap;
		Uint32				m_ui32VerticeNum;

		std::vector<CreatureSkin>	m_vSkin;
		Real				m_fLastCalcTime;
		Bool				m_bAnimatedGeometry,m_bAnimatedBones,m_bNeedPerInstanceAnimation;

		//Collision Detection
#ifdef SUPPORT_COLLISION_DETECTION
		CollisionDetection	m_cd;
#endif
		//Bounding Vertices for Collision Detection
		Vector3*			m_pBoundingVertices;
		Uint32*				m_pBoundingIndices;
		AABB				m_CollisionAABB;
		Sphere				m_CollisionSphere;
	private:
		void	calcAABB(Vector3 *pVertices,Uint32 ui32Vertices,Uint32 ui32Stride,AABB& aabb,Sphere& sphere);
		void	_update(Real tick,Real tickDelta,IRenderSystem *pRenderSystem,Camera* pCamera,const std::string& animation);
	};

}

#endif