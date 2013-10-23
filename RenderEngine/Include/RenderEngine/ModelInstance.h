#ifndef __ModelInstance_H__
#define __ModelInstance_H__

#include <set>
#include "SubModelInstance.h"
#include "Skeleton.h"

namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	class ModelNode;
	class Material;
	class Bone;
	class ParticleSystem;
	class RibbonSystem;
	class MmParticleSystem;

	/// 动画播放完一遍后的回调
	struct IAnimationCallback
	{
		virtual void onOneCycle(const std::string& animation,int loopTimes) = 0;
	};

	/** 模型实例，一个模型可能在场景中有多个实例
	*/
	class _RenderEngineExport ModelInstance
	{
	public:
		//ModelNode
		virtual void updateRenderQueue(RenderQueue* pRenderQueue);
	public:

		void updateColor(IRenderSystem* pRenderSystem,const ColorValue& diffuse);

		/** 更新这个可渲染对象，使其有机会更新自己的状态
		@param tick 当前tick
		@param tickDelta 本次tick和上次tick 时间差
		@param pRenderSystem 渲染系统接口指针
		@param pCamera 摄像机指针
		*/
		virtual void  update(float tick,float tickDelta,IRenderSystem* pRenderSystem,const ColorValue& diffuse);

		/**取得Model对象
		*/
		inline IModel* getModel()const{return m_pModel;}

		/**获得包围盒
		*/
		inline const AABB&	getAABB();

		/**获得包围球
		*/
		inline const Sphere& getBoundingSphere();

		/**设置当前的动画
		@param animation 动画的名称
		@param loopTimes 循环次数，-1表示一直循环
		*/
		inline bool			setCurrentAnimation(const std::string& animation,int loopTimes = -1);

		// add by zjp；获取当前动作的耗时
		inline int			getAnimationTicks(const std::string& animation);

		inline void			advanceTime(float tickDelta);

		inline void setAnimationTime(uint time);

		inline AnimationTime*	getAnimationTime();
		/**获得当前动画名称
		*/
		inline const std::string& getCurrentAnimation();

		inline Animation* _getCurrentAnimation();

		/**设置动画速度
		@param speed 速度
		*/
		inline void	setAnimationSpeed(float speed);
		float getAnimationSpeed() { return m_speed; }

		ModelNode* getModelNode();

		bool setModel(const char* modelName,ModelNode *pNode);

		Bone*	getBone(const char*boneName);

		uint getAnimationCount();

		Animation* getAnimation(uint index);

		uint getBoneCount();



		Bone* getBone(int index);

		const std::string& getFileName();

		uint getNumFaces();

		uint getNumVertices();

		uint getNumSubModels();

		SubModelInstance*	getSubModel(const char* subModelName);

		SubModelInstance*	getFirstSubModel();
		SubModelInstance*	getNextSubModel();

		uint getVer();

		void showAllSubModels(bool show);

		void setCallback(IAnimationCallback *pCallback){m_pCallback = pCallback;}

		IAnimationCallback* getCallback(){return m_pCallback;}

		void resetFX();

		size_t getMemorySize();

		void showRibbonSystem(bool bShow);

	public:
		IIndexBuffer*	getIndexBuffer();
		IVertexBuffer*	getVertexBuffer();
		IVertexBuffer*	getVertexBufferTexcoord();
		IVertexBuffer*	getBlendWeightBuffer();
		IVertexBuffer*  getBlendIndicesBuffer(uint subModelIndex);
		IShaderProgram* getBlendShaderProgram();
		//与动画相关函数 added by xxh 20090930
		inline ulong _getLastUpdateTime(){ return m_lastUpdateTime;};//获取最后更新时间，此函数不是很精确，一般不建议使用
		inline int _getCurrentLoops() {return m_loopTimes;};//获取当前剩余的循环次数。

		//是否需要绘制阴影
		bool			needDrawShadow();



			//add by yhc
		//设置对应时间的顶点数组指针
		void SetVBOByTime(float tickCurrent);
	public:

		ModelInstance();

		~ModelInstance();

	private:
		IModel*		m_pModel;

		//Animation
		std::string	m_currentAnimation;
		Animation*	m_pCurrentAnimation;

		//VertexBlend
		IVertexBuffer*	m_pVBVertexBlend;
		IIndexBuffer*	m_pIB;
		IVertexBuffer*	m_pVBTexcoord;
		IVertexBuffer*  m_pVBBlendWeight;
		std::vector<IVertexBuffer*> m_vBlendIndices;
		IShaderProgram* m_pBlendShaderProgram;

		//是否需要绘制阴影
		bool			m_needDrawShadow;

		//Loop
		int			m_loopTimes;
		AnimationTime		m_currentTime;
		float		m_speed;

		//SubModels
		typedef std::vector<SubModelInstance*> SubModelList;
		SubModelList	m_subModels;
		SubModelList::iterator m_iteratorSubModel;

		ModelNode*	m_pNode;

		//Skeleton
		Skeleton	m_skeleton;

		//ParticleSystem
		typedef std::vector<ParticleSystem*> ParticleSystemList;
		ParticleSystemList m_vParticleSystem;

		//RibbonSystem
		typedef std::vector<RibbonSystem*> RibbonSystemList;
		RibbonSystemList m_vRibbonSystem;

		//MmParticleSystem
		typedef std::vector<MmParticleSystem*> MmParticleSystemList;
		MmParticleSystemList m_vMmParticleSystem;

		//Animation Callback
		IAnimationCallback*		m_pCallback;

		ulong	m_lastUpdateTime;
	private:

		//void loadBone(Bone *pBone,BoneData *pData);//取消此函数

		
		//谢学辉添加，用于处理粒子系统
		//
		
		//谢学辉添加，用于设置不同动画序列的动画时间
		private:
		bool setAllPSCurAnimationTime( uint aniStart, uint aniEnd);

		// 停止产生新的粒子 [4/26/2011 zgz]
		public:
		void SetGenerateParticle(bool bGenerate);
		int  GetAllParticleNum();

		//对MMParticleSystem for modelviewer
		public:	
		int GetMMParticleSystemCount();
		void * getMMParticleSystemParam(unsigned int index, string paramname, string & type, int & count );
		bool setMMParticleSystemParam(unsigned int index, string desname, void * psrc, int srcnum, string srctype);
		bool addMMParticleSystem();
		bool removeMMParticleSystem(unsigned int index);
		
		//对ParticleSystem for modelviewer
		public:
		int GetParticleSystemCount();
		void * getParticleSystemParam(unsigned int index, string paramname, string & type, int & count );
		bool setParticleSystemParam(unsigned int index, string desname, void * psrc, int srcnum, string srctype);
		bool addParticleSystem();
		bool removeParticleSystem(unsigned int index);

		//对RibbonSystem for modelviewer
		public:
		int getRibbonSystemCount();
		void * getRibbonSystemParam( unsigned int index, string paramname, string & type, int &count);
		bool setRibbonSystemParam( unsigned int index, string desname, void * psrc, int srcnum, string srctype);
		bool addRibbonSystem();
		bool removeRibbonSystem(unsigned int index);

		//设置模型基本参数
		bool setExtParam( std::string strParamName, void * psrc, int srcnum, string srctype);

		//用户自定义模型的渲染顺序
		bool isEnableCustomizeRenderOrder();
		bool enableCustomizeRenderOrder(bool enable);
		bool setCustomizeRenderOrder( uint objectType, uint index, int order);
		void getCustomizeRenderOrder(std::vector<int> & listRenderOrder, std::vector<int> & listObjectType, std::vector<int> & listIndex, std::vector<std::string> & listname);
		
		//谢学辉编写，用于将模型写到文件
		public:		
		bool writeModel(string filename,float fCopress/*压缩参数*/ = 0.0f);

		//added by xiexuehui for uv animation 2009/07/24
		//判断模型是否具有uv动画
		bool isAnimatedTexCoord();
		//获取指定时间的纹理缓存,返回值：指定通道的纹理缓存，pass：指定通道，pTime：指定时间
		IVertexBuffer* getTexCoordBuffer(int pass);
		//added end 
	};
	/** @} */
}

#endif