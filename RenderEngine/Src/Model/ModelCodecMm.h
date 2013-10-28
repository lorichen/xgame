#ifndef __ModelCodecMm_H__
#define __ModelCodecMm_H__

#include "RenderEngine/Manager.h"
#include "SubModel.h"
#include "RenderEngine/MmStructs.h"

namespace xs
{
	class ModelCodecMm : public ManagedItem,public IModel
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

		//对硬件蒙皮的支持
		virtual IVertexBuffer *	getBlendWeightBuffer();
		virtual IVertexBuffer * getBlendIndicesBuffer(uint subModelIndex);
		virtual IVertexBuffer * getOriginVertexBuffer() ;
		virtual IShaderProgram * getBlendShaderProgram();
	private:
		//生成混合权重buffer，对每个子mesh生成混合索引buffer，可能会拆分子mesh
		void initForGPUVertexBlend(MmFace* pFaceBuffer);
		//创建骨架
		void createCoreSkeleton();

	public:

		//////获取MMPartileSystem参数
		////virtual  void * getMMParticleSystemParam(unsigned int index, string paramname, string & type, int & count );
		////void * aidGetMMPSParamVector( void * pParam, string paramtype, string & type, int paramcount, int & count);
		////void * aidGetMMPSParamsKeyFrames( MmParticleSystemData & mmpsdata, string paraname,string & type, int & count);
		//////设置MMParticleSystem参数
		////virtual bool setMMParticleSystemParam(unsigned int index, string desname, void * psrc, int srcnum, string srctype);
		////bool aidSetMMPSParamVector(void * psrc, int srcnum, string srctype, void* pdes, int desnum, string destype, bool enablerange = false, float min = 0.0f, float max = 0.0f );
		////bool setMMPSKeyFramesParam(KeyFrames<float> * pkf, float * pParamVal, int iParamNum, string szParamType);
		//////增加mmparticlesystem
		////virtual bool addMMParticleSystem();
		//////删除mmparriclesystem
		////virtual bool removeMMParticleSystem(unsigned int index);
		////
		//////将模型写入到文件
		////bool write(string filename);

		/**获得模型占用的内存
		*/
		virtual size_t getMomorySize();

	public:
		virtual ~ModelCodecMm();
		ModelCodecMm(const std::string& name,IRenderSystem* pRenderSystem);

	public:
		const char*	getType() const;
		bool decode(xs::Stream *pStream);

	private:
		IRenderSystem*	m_pRenderSystem;

		uint	m_ver;

		//Global Sequences
		std::vector<uint>	m_vGlobalSequences;

		//isAnimated
		bool	m_bAnimatedGeometry;
		bool	m_bAnimatedBones;
		bool	m_bNeedPerInstanceAnimation;

		//Textures
		std::vector<MmTexture>	m_vTextures;

		//Materials
		std::vector<Material>	m_vMaterials;
		std::vector<int> m_vRecordMaterialstextureIndex;//记录m_vMaterials的layer的纹理信息

		//VertexBuffer
		IVertexBuffer	*m_pVBTexcoord;

		//IndexBuffer
		IIndexBuffer	*m_pIB;

		//Vertices
		AnimatedVertex*	m_pOriginVertices;
		uint			m_numVertices;

		//用于硬件顶点混合
		IVertexBuffer	*m_pVBPosNormal;;
		IVertexBuffer*	m_pBlendWeightBuffer;
		std::vector<IVertexBuffer*> m_vBlendIndices;
		IShaderProgram* m_pBlendProgram;

		//Bounding Volume
		AABB	m_aabb;
		Sphere	m_sphere;

		//Faces
		uint	m_numFaces;

		//Skeleton
		std::vector<BoneData*>	m_vBones;
		CoreSkeleton			m_skeleton;

		//Animations
		std::vector<Animation*>	m_vAnimations;
        HASH_MAP_NAMESPACE::hash_map<std::string,Animation*> m_AniamtionMap;

		std::vector<SubModel> m_vRenderPasses;
		std::vector<int> m_vRecordMaterialIndex;//谢学辉修改，记录SubModel的MaterialIndex
		//precomputed vertexbuffer
#define MM_PRECOMPUTED_VBO_COUNT_MAX (30 * 3 * 60 ) // 帧率* 动画平均时间 * 动画数量
		IVertexBuffer*	m_pPrecomputedVBs[MM_PRECOMPUTED_VBO_COUNT_MAX  /*30720*/];

		//Momory size
		size_t		m_memorySize;

		//
		std::vector<MmParticleSystemData>	m_vMmParticleSystems;

	private:
		void	calcAABB(Vector3* pVertices,uint ui32Stride,uint numVertices);
	};
}

#endif