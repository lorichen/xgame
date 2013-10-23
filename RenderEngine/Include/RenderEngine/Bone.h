#ifndef __Bone_H__
#define __Bone_H__

namespace xs
{
	class CoreBone;

	class _RenderEngineExport Bone
	{
	public:
		Bone(CoreBone * pCoreBone):m_pCoreBone(pCoreBone),
			m_bPivotRotation(false),
			m_pParent(0),
			m_vPivot(xs::Vector3::ZERO),
			m_vTransformedPivot(xs::Vector3::ZERO),
			m_mtxTransform(Matrix4::IDENTITY),
			m_quatRotation(Quaternion::IDENTITY),
			m_mtxInitMatrix(Matrix4::IDENTITY),
			m_precomputeMtx(Matrix4::IDENTITY),
			m_bNeedUpdate(true)
		{
		}
	public:
		//设置模型名称
		void					setName(const char *name);
		//设置是否沿着pivot旋转和缩放
		void					setPivotRotation(bool pivotRotation);
		//设置轴点
		void 					setPivot(const Vector3& vPivot);
		//设置父骨骼
		void 					setParent(Bone * pBone);
		//设置预计算矩阵
		void					setPrecomputeMatrix(const Matrix4& mtx);
		//取得名称
		const char*				getName();
		//取得父骨骼
		Bone*					getParent();
		//取得轴点
		const Vector3&			getPivot();
		//更新当前的矩阵
		void					calcMatrix(AnimationTime * pTime, int rangeIndex);
		//是否需要更新骨骼
		bool					getNeedUpdate();
		//设置是否需要更新骨骼
		void					setNeedUpdate(bool needUpdate);
		//取得变换后的轴点
		inline const Vector3&			getTransformedPivot()
		{
			return m_vTransformedPivot;
		}
		//取得最终的变换矩阵
		inline const Matrix4&	getFullTransform()
		{
			return m_mtxTransform;
		}
		//取得四元数版本的最终旋转分量
		inline const Quaternion& getFullRotation()
		{
			return m_quatRotation;
		}
		//设置初始矩阵
		inline void	setInitMatrix(const Matrix4 & mtx) 
		{ 
			m_mtxInitMatrix = mtx;
		}
		//获取初始矩阵
		inline const Matrix4 & getInitMatrix() 
		{
			return m_mtxInitMatrix;
		}
	private:	
		Matrix4					m_mtxInitMatrix;		//骨骼的初始变换矩阵
		Vector3					m_vPivot;				//初始轴点
		Vector3					m_vTransformedPivot;	//当前轴点
		Matrix4					m_mtxTransform;			//当前变换矩阵
		Quaternion				m_quatRotation;			//当前旋转矩阵
		std::string				m_name;					//骨骼影子的名字
		Bone*					m_pParent;				//父骨骼影子
		bool					m_bPivotRotation;		//计算骨骼的标志
		Matrix4					m_precomputeMtx;		//预计算矩阵
		CoreBone*				m_pCoreBone;			//关联的真正的骨骼
		bool					m_bNeedUpdate;			//骨骼数据是否需要更新
	};

}
#endif