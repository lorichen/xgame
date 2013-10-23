#ifndef __RibbonSystem_H__
#define __RibbonSystem_H__


namespace xs
{
	class Bone;
	class ModelNode;

	class RibbonSystem : public IRenderable
	{
	public:
		/** 渲染
		@param pRenderSystem 渲染系统接口指针
		*/
		virtual void  render(IRenderSystem* pRenderSystem,bool useMtl);

	public:
		RibbonSystem();
		~RibbonSystem();

		void update(const AnimationTime *pTime,float tickDelta,IRenderSystem* pRenderSystem);
		int getRenderOrder();

		void reset();
		void setColor(const ColorValue& color);

		void Clear();//清空参数

		int __getRibbonSegNum();

		void show( bool bShow);//显示飘带系统，默认显示

	public:
		RibbonEmitterData*	m_pData;
		Bone*				m_pBone;
		ModelNode*			m_pNode;
		ITexture*			m_pTexture;

		float	m_currentEmission;
		uint	m_originTime;
		Color4	m_diffuse;

	private:
		struct Segment
		{
			Vector3 v1,v2;
			Vector2 t1,t2;
			Color4 c;
			Color4 fullColor;
			float lifeSpan;
			Vector3 v;//速度变化
			Segment()
			{
				
			}
		};

		typedef std::list<Segment> SegmentList;
		SegmentList m_vSegments;


	private:
		void saveContext(IRenderSystem *pRenderSystem);
		void loadContext(IRenderSystem *pRenderSystem);
		//渲染状态备份/恢复信息
		SceneBlendFactor	m_sbfSrc,m_sbfDst;
		bool m_alphaCheck;
		float m_alphaCheckRef;
		CullingMode	m_cullingMode;
		CompareFunction	m_alphaFunc;
		bool		m_bDepthCheck;
		bool		m_bDepthWrite;
		bool		m_lightingEnabled;
		Color4		m_color;
		bool		m_bShowFlag;//飘带系统显示标志
	private:
		/*
		如果动作的速度不均匀，会使得飘带不平滑，且有部分被挤压的感觉，所以，在插入点的时候，按角度，来均匀插值
		*/
		Vector3 m_v3PrePrePviot;//前驱的前驱
		Vector3 m_v3PreviousPivot;//前驱
		Vector3 m_vPreviousUp;
		float m_fPreviousHeightAbove;
		float m_fPreviousHeighBelow;
	};
}

#endif