#ifndef __CoreBone_H__
#define __CoreBone_H__

namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/
	//2010年12月8日：骨骼不支持billboard，不支持插值，但会缓存计算好的骨骼矩阵。


	/** 骨骼，骨骼是树状结构，大量的骨骼形成skeleton
	*/
	class _RenderEngineExport CoreBone
	{
	public:
		CoreBone() :m_pTranslation(0),
			m_pRotation(0),
			m_pScale(0),
			m_mtxInitMatrix(Matrix4::IDENTITY),
			m_vPivot(xs::Vector3::ZERO),
			m_name("uninitialized"),
			m_bPivotRotation(false),
			m_pParent(0),
			m_precomputeMtx(Matrix4::IDENTITY),
			m_bCanBufferData(false)
		{

		}

		~CoreBone();

	public:
		//设置模型名称
		void					setName(const char *name);
		//取得名称
		const char*				getName();

		//设置轴点
		void 					setPivot(const Vector3& vPivot);
		//取得轴点
		const Vector3&			getPivot();

		//设置是否沿着pivot旋转和缩放
		void					setPivotRotation(bool pivotRotation);
		bool					getPivotRotation();

		//设置父骨骼
		void 					setParent(CoreBone* pCoreBone);
		//取得父骨骼
		CoreBone*				getParent();

		//设置能否缓存数据
		void					setCanBufferData(bool canBuffer);

		//获取占用的内存,不精确
		size_t					getMemeorySize();


		//这个接口暂时没用
		void					setPrecomputeMatrix(const Matrix4& mtx);
		const xs::Matrix4 &	getPrecomputeMatrix();

		inline void	setInitMatrix(const Matrix4 & mtx) { m_mtxInitMatrix = mtx;}
		inline const Matrix4 & getInitMatrix() { return m_mtxInitMatrix;}

		//获取指定时间的骨骼数据
		void	getBoneData(
			AnimationTime* pTime, 
			int rangeIndex, 
			xs::Matrix4 & mtxTransform,
			xs::Quaternion & quatRotation
			);
		
		//根据时间计算骨骼的矩阵
		//void 					calcMatrix(IRenderSystem *pRenderSystem,AnimationTime* pTime,int rangeIndex);
		void					setInterpolationType(InterpolationType translation,InterpolationType rotation,InterpolationType scale);
		void					setKeyFramesTranslation(KeyFrames<Vector3>& keyFrames);
		void					setKeyFramesRotation(KeyFrames<Quaternion>& keyFrames);
		void					setKeyFramesScale(KeyFrames<Vector3>& keyFrames);

		std::vector<ModelKeyframeTM> *m_pMKTM;//暂时没用
		std::vector<uint>		m_keyTime;//记录所有的关键时间
		//关键帧索引表，用于加速查找,第一个uint，每个动画的开始时间，第二个int，m_keyTime里面的第一个关键时间的位置，第三个int，m_keyTime里面关键时间的个数
		HASH_MAP_NAMESPACE::hash_map<uint, std::pair<uint, uint> > m_keyTimeIndex;
	private:
		//void 					_calcMatrix_NoPivotRotation(IRenderSystem *pRenderSystem,AnimationTime* pTime,int rangeIndex);
		//void 					_calcMatrix_PivotRotation(IRenderSystem *pRenderSystem,AnimationTime* pTime,int rangeIndex);
		uint					getClosetKeyFrameTime(AnimationTime * pTime,int rangeIndex);
	private:
		
		//骨骼的初始变换矩阵
		
		KeyFrames<Vector3>		*m_pTranslation;
		KeyFrames<Quaternion>	*m_pRotation;
		KeyFrames<Vector3>		*m_pScale;
		
		Matrix4					m_mtxInitMatrix;
		Vector3					m_vPivot;
		std::string				m_name;
		CoreBone*				m_pParent;
		bool					m_bPivotRotation;
		Matrix4					m_precomputeMtx;
		bool					m_bCanBufferData; //能否缓存计算的数据

		struct					KeyFrameData
		{
			xs::Matrix4		m_mtxTransform;
			xs::Quaternion		m_quatRotation;
		};
		typedef HASH_MAP_NAMESPACE::hash_map<uint, KeyFrameData*> KeyFrameDataContainer;//关键时间和骨骼数据的映射表
		typedef KeyFrameDataContainer::iterator KeyFrameDataContainerIterator;

		KeyFrameDataContainer	m_vNoPivotData;
		KeyFrameDataContainer	m_vPivotData;

	};
	/** @} */
}

#endif