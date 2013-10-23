#ifndef __ModelData_H__
#define __ModelData_H__

namespace xs
{
	const int MODEL_NAME_NODE_SIZE             = 80;

	/// Model Animations
	struct GeosetAnimation
	{
		GeosetAnimation()
		{
			alpha.setStaticData(1.0f);
		}

		KeyFrames<float>	alpha;
	};
//////////////////////////////////////////////////////////////////////
	struct ModelKeyframeTM
	{
		float tm[4][4];
		int time;
	};
/////////////////////////////////////////////////////////////////////
	/// Bone
	struct BoneData
	{
		size_t getMemorySize()
		{
			return sizeof(*this) + translation.getMemorySize() + rotation.getMemorySize() + scale.getMemorySize();
		}

		BoneData()
		{
			name[0] = 0;

			objectId = -1;
			parentId = -1;

			translation.setStaticData(Vector3(0.0f, 0.0f, 0.0f));
			rotation.setStaticData(Quaternion(0.0f, 0.0f, 0.0f, 1.0f));
			scale.setStaticData(Vector3(1.0f, 1.0f, 1.0f));

			dontInheritTranslation = false;
			dontInheritRotation = false;
			dontInheritScaling = false;
			billboarded = false;
			billboardedLockX = false;
			billboardedLockY = false;
			billboardedLockZ = false;
			cameraAnchored = false;

			pivotPoint = Vector3(0.0f, 0.0f, 0.0f);

			pivotRotation = false;

			precomputeMtx = Matrix4::IDENTITY;

			initTrans = Vector3::ZERO;
			initQuat = Quaternion::IDENTITY;
			initScale = Vector3::UNIT_SCALE;
		}

		//骨头数据赋值函数,xxh 20090929,用于骨骼数据拷贝
		const BoneData & operator = ( const BoneData & bonedata)
		{
			strncpy( name, bonedata.name, MODEL_NAME_NODE_SIZE);
			name[MODEL_NAME_NODE_SIZE] = '\0';
			objectId = bonedata.objectId;
			parentId = bonedata.parentId;
			translation = bonedata.translation;
			rotation = bonedata.rotation;
			scale = bonedata.scale;
			m_TMVector = bonedata.m_TMVector;
			dontInheritTranslation = bonedata.dontInheritTranslation;
			dontInheritRotation = bonedata.dontInheritRotation;
			dontInheritScaling = bonedata.dontInheritScaling;
			billboarded = bonedata.billboarded;
			billboardedLockX = bonedata.billboardedLockX;
			billboardedLockY = bonedata.billboardedLockY;
			billboardedLockZ = bonedata.billboardedLockZ;
			cameraAnchored = bonedata.cameraAnchored;
			pivotPoint = bonedata.pivotPoint;
			pivotRotation = bonedata.pivotRotation;
			initTrans = bonedata.initTrans;
			initQuat = bonedata.initQuat;
			initScale = bonedata.initScale;
			precomputeMtx = bonedata.precomputeMtx;
			return *this;
		}
	
		char name[MODEL_NAME_NODE_SIZE + 1];

		int objectId;
		int parentId;

		KeyFrames<Vector3> translation;
		KeyFrames<Quaternion> rotation;
		KeyFrames<Vector3> scale;
		std::vector<ModelKeyframeTM> m_TMVector;

		bool dontInheritTranslation;
		bool dontInheritRotation;
		bool dontInheritScaling;
		bool billboarded;
		bool billboardedLockX;
		bool billboardedLockY;
		bool billboardedLockZ;
		bool cameraAnchored;

		Vector3 pivotPoint;

		bool pivotRotation;

		//模型基础变换
		Vector3 initTrans;
		Quaternion initQuat;
		Vector3 initScale;


		Matrix4	precomputeMtx;
	};

	/// Particle Emitter2
	struct ParticleEmitter2Data
	{
		ParticleEmitter2Data()
		{
			filterMode = BM_OPAQUE;

			speed.setStaticData(0.0f);
			variation.setStaticData(0.0f);
			latitude.setStaticData(0.0f);
			gravity.setStaticData(0.0f);
			visibility.setStaticData(1.0f);
			emissionRate.setStaticData(0.0f);
			width.setStaticData(0.0f);
			length.setStaticData(0.0f);

			segmentColor1 = Color3(1.0f, 1.0f, 1.0f);
			segmentColor2 = Color3(1.0f, 1.0f, 1.0f);
			segmentColor3 = Color3(1.0f, 1.0f, 1.0f);
			alpha = Vector3(1.0f, 1.0f, 1.0f);//将(255,255,255)改成(1,1,1)
			particleScaling = Vector3(1.0f, 1.0f, 1.0f);
			headLifeSpan = Vector3(0.0f, 0.0f, 1.0f);
			headDecay = Vector3(0.0f, 0.0f, 1.0f);
			tailLifeSpan = Vector3(0.0f, 0.0f, 1.0f);
			tailDecay = Vector3(0.0f, 0.0f, 1.0f);

			rows = 1;
			columns = 1;
			textureId = -1;
			priorityPlane = 0;
			replaceableId = 0;
			time = 0.0f;
			lifeSpan = 0.0f;
			tailLength = 0.0f;

			sortPrimitivesFarZ = false;
			lineEmitter = false;
			modelSpace = false;
			alphaKey = false;
			unshaded = true;//默认关闭关照
			unfogged = true;//默认关闭雾化
			xyQuad = false;
			squirt = false;
			head = false;
			tail = false;

			boneObjectId = -1;
			//added by xxh for version 5
			bEnhanceValue = false;
			bEnableSynScale = false;
			vInitSynScale = Vector3(1,1,1);
			memset(name,0,16);
			bEmmitTime2Space= false;
			iSpaceInterType = 1;
			fSpaceUint = 1.0f;
			attach2Bone = false;//默认为不绑定到骨骼

			nIntervalTime = 0;//默认不旋转
			nAngle = 0;

			bNodeInitMatrix = true;
		}

		BlendMode filterMode;

		KeyFrames<float> speed;
		KeyFrames<float> variation;
		KeyFrames<float> latitude;
		KeyFrames<float> gravity;
		KeyFrames<float> visibility;
		KeyFrames<float> emissionRate;
		KeyFrames<float> width;
		KeyFrames<float> length;

		Color3 segmentColor1;
		Color3 segmentColor2;
		Color3 segmentColor3;
		Vector3 alpha;
		Vector3 particleScaling;
		Vector3 headLifeSpan;
		Vector3 headDecay;
		Vector3 tailLifeSpan;
		Vector3 tailDecay;

		int rows;
		int columns;
		int textureId;
		int priorityPlane;
		int replaceableId;
		float time;
		float lifeSpan;
		float tailLength;

		bool sortPrimitivesFarZ;
		bool lineEmitter;
		bool modelSpace;
		bool alphaKey;
		bool unshaded;
		bool unfogged;
		bool xyQuad;
		bool squirt;
		bool head;
		bool tail;

		std::string textureFilename;
		int boneObjectId;

		//added by xxh for version 5
		bool bEnhanceValue ;//当启用a混合时，增强亮度
		bool bEnableSynScale;//是否启用同步放大
		Vector3 vInitSynScale;//模型的初始放大比率，用于同步扩大重力，速率，产生范围等空间参数
		char name[16];//粒子系统的名字，方便标识粒子系统
		bool bEmmitTime2Space;//时间转换为空间来计算产生率
		int iSpaceInterType;//空间均匀时的插值类型,1为直线型，2为球面型
		float fSpaceUint;//空间均匀的单位

		//added by xxhf for version 7
		bool  attach2Bone;//是否绑定到骨骼

		//added by zgz for version 8
		//支持粒子绕z轴旋转
		int    nIntervalTime; //旋转间隔时间
		int    nAngle;        //旋转角度

		//added by zgz for version 9
		// 粒子绑定到骨骼时，需要将骨骼的初始矩阵一起运算，版本12以前的粒子系统均没有考虑，这是一个bug。
		// 为了避免以前制作好的特效重做，用这个变量标识是否需要考虑初始矩阵。[5/18/2011 zgz]
		bool  bNodeInitMatrix;
	};

	/// Ribbon Emitter
	struct RibbonEmitterData
	{
		RibbonEmitterData()
		{
			heightAbove.setStaticData(0.0f);
			heightBelow.setStaticData(0.0f);
			alpha.setStaticData(1.0f);
			color.setStaticData(Color3(1.0f, 1.0f, 1.0f));
			textureSlot.setStaticData(0);
			visibility.setStaticData(1.0f);

			emissionRate = 0.0f;
			lifeSpan = 0.0f;
			gravity = 0.0f;
			rows = 1;
			columns = 1;
			materialId = -1;

			filterMode = BM_OPAQUE;
			boneObjectId = -1;
			bEnhanceValue= false;
			bEnableSynScale = false;
			vInitSynScale = Vector3(1,1,1);
			memset(name,0,16);
			attach2Bone = false;

			fadeoutmode = 0;//0普通消失模式,1指数消失模式

			//added by xxh for version 9 
			//容许指定飘带方向
			enableAssignUpDir = false;//是否容许指定向上方向，默认不容许
			vAssignedUpDir = Vector3(0.0f,1.0f,0.0f);//向上的方向
			
			fadespeed = 1.0f;
		}

		KeyFrames<float> heightAbove;
		KeyFrames<float> heightBelow;
		KeyFrames<float> alpha;
		KeyFrames<Color3> color;
		KeyFrames<int> textureSlot;
		KeyFrames<float> visibility;

		float emissionRate;
		float lifeSpan;
		float gravity;
		int rows;
		int columns;
		int materialId;

		BlendMode filterMode;
		std::string textureFilename;
		int	boneObjectId;

		//added by xxh for version 5
		bool bEnhanceValue ;//当启用a混合时，增强亮度
		bool bEnableSynScale;//是否启用同步放大
		Vector3 vInitSynScale;//模型的初始放大比率，用于同步扩大重力，速率，产生范围等空间参数
		char name[16];//飘带系统的名字，方便标识粒子系统

		//added by xxhf for version 7
		bool  attach2Bone;//是否绑定到骨骼，这个参数暂时没用到

		//added by xxh for version 8
		int		fadeoutmode;//飘带消失模式，在版本10中，此参数被取消

		//added by xxh for version 9 
		//容许指定飘带方向
		bool enableAssignUpDir;//是否容许指定向上方向，默认不容许
		Vector3 vAssignedUpDir;//向上的方向

		//added by xxh for version 10
		float	fadespeed;//飘带消失速度，运动速度成反比
	};

	/// MmParticleEmitterData

	struct MmTexCoordSet
	{
		Vector2 tc[4];
	};

	struct MmParticleSystemParamData
	{
		float mid;
		Color4 colors[3];
		float sizes[3];
		short d[10];
		float unk[3];
		float scales[3];
		float slowdown;
		float rotation;
		float f2[16];
	};

	struct MmParticleSystemData
	{
		MmParticleSystemData()
		{
			bEnhanceValue = false;//当启用a混合时，增强亮度
			bEnableSynScale =false;//是否启用同步放大
			vInitSynScale = Vector3(1,1,1);//模型的初始放大比率，用于同步扩大重力，速率，产生范围等空间参数	
			memset(name,0,16);
			bEmmitTime2Space = false;
			iSpaceInterType = 1;
			fSpaceUint = 1.0f;
			attach2Bone = 0;//默认不绑定到骨骼
			nIntervalTime = 0;//默认不旋转 zgz
			nAngle  = 0;
			bNodeInitMatrix = true;
			memset(iteratorArea, 0, sizeof(iteratorArea));
		}
		int id;
		BillboardType billboardType;
		Vector3 pos;
		short bone;
		std::string texture;
		short blend;
		short emitterType;
		short type;
		int order;
		short s2;
		short cols;
		short rows;
		KeyFrames<float> speed;
		KeyFrames<float> variation;
		KeyFrames<float> spread;
		KeyFrames<float> lat;
		KeyFrames<float> gravity;
		KeyFrames<float> lifespan;
		KeyFrames<float> rate;
		KeyFrames<float> areal;
		KeyFrames<float> areaw;
		KeyFrames<float> grav2;
		MmParticleSystemParamData p;
		std::vector<MmTexCoordSet> vTexcoords;
		//added by xxh for version 5
		bool bEnhanceValue ;//当启用a混合时，增强亮度
		bool bEnableSynScale;//是否启用同步放大
		Vector3 vInitSynScale;//模型的初始放大比率，用于同步扩大重力，速率，产生范围等空间参数
		char name[16];//粒子系统的名字，方便标识粒子系统
		bool bEmmitTime2Space;//时间转换为空间来计算产生率
		int iSpaceInterType;//空间上均匀的类型，1为直线型，2为球面型
		float fSpaceUint;//空间均匀的单位

		//added by xxh for version 7
		short attach2Bone;//是否绑定到骨骼

		//added by zgz for version 8
		//支持单个粒子自身绕z轴旋转
		int    nIntervalTime; //旋转间隔时间
		int    nAngle;        //旋转角度

		//added by zgz for version 9
		// 粒子绑定到骨骼时，需要将骨骼的初始矩阵一起运算，版本12以前的粒子系统均没有考虑，这是一个bug。
		// 为了避免以前制作好的特效重做，用这个变量标识是否需要考虑初始矩阵。[5/18/2011 zgz]
		bool  bNodeInitMatrix;

		// 遍历范围
		int   iteratorArea[2];
	};
}

#endif