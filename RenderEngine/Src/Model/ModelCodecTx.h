#ifndef __ModelCodecTx_H__
#define __ModelCodecTx_H__

#include "RenderEngine/Manager.h"
#include "SubModel.h"
#include "RenderEngine/MzHeader.h"
//谢学辉添加，包含Mm粒子系统的参数的头文件
#include "RenderEngine/MmStructs.h" 
#define TX_TEX_VBO_STATIC_WRITE_ONLY //如果开启定义，则使用静态vbo保存纹理坐标，否则私用动态vbo保存纹理坐标



/*
2009/07/24 xiexuehui for uv animation
*/

namespace xs
{
	class ModelCodecTx : public ManagedItem,public IModel
	{
	public:
		virtual void release();

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
		virtual uint	getAnimationCount() ;

		/** 获得某个动画
		@param index 下标
		@return 动画指针
		*/
		virtual Animation* getAnimation(uint index);

		/** 是否拥有某个动画
		@param animation 动画名称
		@see enAnimation
		@return 如果存在animation动画则返回此动画数据，否则返回0
		*/
		virtual Animation* hasAnimation(const std::string& animation) ;

		//获取模型的骨架
		virtual CoreSkeleton * getCoreSkeletion();

		/** 获得子模型的数量
		@return 子模型的数量
		*/
		virtual uint getNumSubModels() ;

		/** 获得子模型指针
		@param name 子模型名称
		@return 子模型指针
		*/
		virtual ISubModel*	getSubModel(const char* name);

		/** 根据下标来获得子模型指针
		@param name 子模型下标
		@return 子模型指针
		*/
		virtual ISubModel*	getSubModel(int index);

		virtual IIndexBuffer*	getIndexBuffer();

		virtual IVertexBuffer*	getTexcoordBuffer();

		/**获得面数
		*/
		virtual uint getNumFaces();

		/**获得顶点数
		*/
		virtual uint getNumVertices();

		/**获得版本号
		*/
		virtual uint getVer();

		virtual IVertexBuffer* vertexBlend(Skeleton *pSkeleton,const AnimationTime *pTime);

		virtual uint numParticleSystem();

		virtual ParticleEmitter2Data*	getParticleSystemData(uint index);

		virtual uint numRibbonSystem();

		virtual RibbonEmitterData*	getRibbonSystemData(uint index);

		virtual uint numMmParticleSystem(); 
		
		virtual MmParticleSystemData* getMmParticleSystem(uint index);

		//是否需要绘制阴影
		virtual bool	needDrawShadow(){ return false;}

		//获取PartileSystem参数
	public:
		virtual  void * getParticleSystemParam(unsigned int index, string paramname, string & type, int & count );
		virtual bool setParticleSystemParam(unsigned int index, string desname, void * psrc, int srcnum, string srctype);
		virtual bool addParticleSystem();//增加particlesystem	
		virtual bool removeParticleSystem(unsigned int index);//删除parriclesystem
		
	private:
		void * getPSParamVector( void * pParam, string paramtype, string & type, int paramcount, int & count);
		void * getPSKeyFramesParam(void * pkf, string kftype, string & type, int & count);
		bool setPSParamVector(void * psrc, int srcnum, string srctype, void* pdes, int desnum, string destype, bool enablerange = false, float min = 0.0f, float max = 1000000.0f );
		bool setPSKeyFramesParam( void * pkf, string kftype, float * pParamVal, int iParamNum, bool enablerange = false, float min = 0.0f, float max = 1000000.0f);

		
		//用于将模型写到文件
	public:
		virtual bool write(string filename,float fCopress);

		/**获得模型占用的内存
		*/
		virtual size_t getMomorySize();

		//对硬件蒙皮的支持
		virtual IVertexBuffer *	getBlendWeightBuffer();
		virtual IVertexBuffer * getBlendIndicesBuffer(uint subModelIndex);
		virtual IVertexBuffer * getOriginVertexBuffer() ;
		virtual IShaderProgram * getBlendShaderProgram();
	private:
		//生成混合权重buffer，对每个子mesh生成混合索引buffer，可能会拆分子mesh
		void initForGPUVertexBlend(MODELFACE* pFaceBuffer);
		//创建骨架
		void createCoreSkeleton();

	public:
		//获取MMParticleSystem参数
		virtual  void * getMMParticleSystemParam(unsigned int index, string paramname, string & type, int & count );
		virtual bool setMMParticleSystemParam(unsigned int index, string desname, void * psrc, int srcnum, string srctype);
		virtual bool addMMParticleSystem();
		virtual bool removeMMParticleSystem(unsigned int index);
		//设置MMParticleSystem参数
	private:
		void * getMMPSParamVector( void * pParam, string paramtype, string & type, int paramcount, int & count);
		void * getMMPSKeyFramesParam(void * pkf, string kftype, string & type, int & count);
		bool setMMPSParamVector(void * psrc, int srcnum, string srctype, void* pdes, int desnum, string destype, bool enablerange = false, float min = 0.0f, float max = 1000000.0f );
		bool setMMPSKeyFramesParam( void * pkf, string kftype, float * pParamVal, int iParamNum, bool enablerange = false, float min = 0.0f, float max = 1000000.0f);
	private:
			void initTile(Vector2 *tc,int num,int cols,int rows,int order);//用于解码时初始化纹理坐标


	public:
		//设置用户定义渲染顺序
		virtual bool enableCustomizeRenderOrder(bool enable) ;
		virtual bool isEnabledCustomizeRenderOrder();
		virtual bool setCustomizeRenderOrder( uint objectType, uint index, int order);
		virtual int	getCustomizeRenderOrder(uint objectType, uint index);	
	private:
		int getInitRenderOrder();
		bool	m_bIsEnableCustomizeRenderOrder;
		std::vector<int> * m_pCustomizeRenderOrderQueue;

		/*Ribbon System 编辑函数
		*/
	public:
		virtual void * getRibbonSystemParam( unsigned int index, string paramname, string & type, int &count);
		virtual bool setRibbonSystemParam( unsigned int index, string desname, void * psrc, int srcnum, string srctype);
		virtual bool addRibbonSystem();
		virtual bool removeRibbonSystem(unsigned int index);
	private:
		void * getRSParamVector( void * pParam, string paramtype, string & type, int paramcount, int & count);//谢学辉修改
		void * getRSKeyFramesParam( void * pkf, string keyframetype,string & type, int & count);//谢学辉修改
		bool setRSParamVector(void * psrc, int srcnum, string srctype, void* pdes, int desnum, string destype, bool enablerange = false, float min = 0.0f, float max = 1000000.0f );//谢学辉修改
		bool setRSKeyFramesParam( void * pkf, string keyframetype, float * pParamVal, int iParamNum );//谢学辉修改，设置帧参数


	public:
		virtual ~ModelCodecTx();
		ModelCodecTx(const std::string& name,IRenderSystem* pRenderSystem);

	public:
		const char*	getType() const;
		bool decode(xs::Stream *pStream);

		void loadFeiBian(xs::Stream *pStream,const std::string& animationName);

	private:
		IRenderSystem*	m_pRenderSystem;
		std::vector<SubModel> m_vRenderPasses;
		std::vector<bool>	m_vShouldSavePasses;
		//Textures
		std::vector<Animation*>		m_vAnimations;
		stdext::hash_map<std::string,Animation*>	m_AnimationMap;
		//VertexBuffer
		IVertexBuffer	*m_pVBTexcoord;
		uint				m_ui32VerticeNum;
		//IndexBuffer
		IIndexBuffer	*m_pIB;
		IVertexBuffer	*m_pVBPosNormal;

		IVertexBuffer*	m_pBlendWeightBuffer;
		std::vector<IVertexBuffer*> m_vBlendIndices;
		IShaderProgram* m_pBlendProgram;
		//BoundingVolume
		AABB	m_AABB;
		Sphere	m_BoundingSphere;

		//Skeleton
		std::vector<BoneData*>	m_vBones;
		CoreSkeleton			m_skeleton;

		//Materials
		std::vector<Material> m_vMaterials;

		//Ribbon
		std::vector<RibbonEmitterData*> m_vRibbonEmitters;

		//Particle
		std::vector<ParticleEmitter2Data*> m_vParticleEmitters;

		std::vector<MmParticleSystemData *> m_vMmParticleSystemData;//谢学辉添加，处理MM粒子系统

		uint		m_ver;

		//precomputed vertexbuffer
		//IVertexBuffer*	m_pPrecomputedVBs[8192];
#define TX_PRECOMPUTED_TEX_FPS 30 //定义帧速度
#define TX_PRECOMPUTED_TEX_TIME 34 //定义动画持续时间，定义为34，支持1000帧
#define TX_PRECOMPUTED_VBO_COUNT_MAX ( TX_PRECOMPUTED_TEX_FPS * TX_PRECOMPUTED_TEX_TIME * 1) //支持1000帧的动画
		IVertexBuffer*	m_pPrecomputedVBs[TX_PRECOMPUTED_VBO_COUNT_MAX];//动画每秒30帧，最多持续20秒，不需要很大的缓存

#if defined(TX_TEX_VBO_STATIC_WRITE_ONLY)
#define TX_PRECOMPUTED_TEX_PASS_COUNT 1 //导出插件只是导出1重纹理，所以现在只支持1重纹理通道，如果想支持多重，请作相应的设定
#define TX_PRECOMPUTED_TEX_VBO_COUNT ( TX_PRECOMPUTED_TEX_FPS * TX_PRECOMPUTED_TEX_TIME ) //定义为每秒30帧，纹理变化持续20秒，一般的特效不会超出这个时间
		IVertexBuffer * m_pPrecomputedTexVBs[TX_PRECOMPUTED_TEX_PASS_COUNT*TX_PRECOMPUTED_TEX_VBO_COUNT];
		IVertexBuffer * m_pPrecomputedTexVBsPass[TX_PRECOMPUTED_TEX_PASS_COUNT];
#endif

		//Momory size
		size_t		m_memorySize;

	private:
		_VertexData*	m_pVertexData;

		bool			m_bAnimatedGeometry;

	private:
		void  calcAABB(_VertexData *pVtx,uint num,AABB& aabb,Sphere& sphere);

	//added by xiexuehui for uv animation 2009/07/24
	private:
		bool m_bAnimatedTexCoord;
		int m_iMaxPassNum;

#if !defined(TX_TEX_VBO_STATIC_WRITE_ONLY)
		std::map< int , IVertexBuffer *> m_mapPrecomputedTexBuffers;//预计算的纹理映射
		uint m_uiPreAniTime;
#endif
		///关于unwrap uvw修改器的uv坐标
		std::vector< KeyFrames<Vector2> > m_vUVKFs;//保存不同时间的顶点的uv坐标。
		
	public:
		//判断模型是否具有uv动画
		virtual bool isAnimatedTexCoord();
		//计算指定时间的纹理映射,返回值：纹理通道的数量，AnimationTime:当前的时间
		virtual int texBlend(const AnimationTime *pTime);
		//获取指定时间的纹理缓存,返回值：指定通道的纹理缓存，pass：指定通道，pTime：指定时间
		virtual IVertexBuffer* getTexCoordBuffer(int pass);
	//added end 2009/07/24
		public: //模型的扩展
			virtual void * getExtParam(std::string strParamName, std::string & strType, int & count);
			virtual bool setExtParam( std::string strParamName, void * psrc, int srcnum, string srctype);
	private:
		bool m_bShouldBoneInterKeyFrame;
		Vector4 m_boundingBox;
		bool m_bCalculatedAABB;
	public:
		//获取boundingbox
		virtual void getBoundingBox( Vector4 &rect);

	private:
		bool m_bMergeSubModel;//子模型融合
		bool m_bLoadSkinInfoFromFile;//从文件读取了蒙皮信息


	};
}

#endif