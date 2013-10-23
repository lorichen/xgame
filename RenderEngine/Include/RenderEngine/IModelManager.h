#ifndef __I_ModelManager_H__
#define __I_ModelManager_H__
#include <string> //谢学辉修改
using namespace std; //谢学辉修改

namespace xs
{
	/*! \addtogroup RenderEngine
	*  渲染引擎
	*  @{
	*/

	class Camera;
	struct IRenderSystem;
	class Animation;
	struct ITexture;
	struct IVertexBuffer;
	class Skeleton;
	class CoreSkeleton;
	struct AnimationTime;
	class Material;
	struct GeosetAnimation;
	struct IShaderProgram;

#define VERTEX_BLEND_MATRICES_NUM		83 //至少是13,现在不能超过83个，同时对应的shader文件里面的混合矩阵数也要相同
#define VERTEX_BLEND_MAPPINGS_NUM		256 //模型包含的骨骼数的上限加1，最后一个矩阵作为哑元。一定是256，不要改

	/** 子模型接口
	*/
	struct ISubModel
	{
		/** 获得子模型的名称
		@return 子模型的名称
		*/
		virtual const char* getName() = 0;

		virtual ushort getIndexStart() = 0;

		virtual ushort getIndexCount() = 0;

		virtual ushort getVertexStart() = 0;

		virtual ushort getVertexEnd() = 0;

		virtual const Material&	getMaterial() = 0;

		virtual GeosetAnimation*	getGeosetAnimation() = 0;

		virtual bool	isAnimated() = 0;

		//对硬件蒙皮的支持

		/*获取绑定到的骨骼的索引，索引数组的大小必须为VERTEX_BLEND_MATRICES_NUM
		@validIndicesNum	有效的骨骼索引的数量
		@return				 绑定到的骨骼的索引
		*/
		virtual uchar *		getBindedBonesIndices(uint & validIndicesNum)=0;
	};

	/** 模型接口
	*/
	struct IModel
	{
		/**请不要使用release来释放模型，应该使用IModelManager::releaseModel，因为模型是引用计数对象
		*/
		virtual void release() = 0;
		/** 获得文件名
		@return 文件名
		*/
		virtual const std::string& getFileName() = 0;

		/** 获得包围盒数据
		@return 模型包围盒
		*/
		virtual const AABB&	getAABB() = 0;

		/** 获得包围球数据
		@return 模型包围球
		*/
		virtual const Sphere& getBoudingSphere() = 0;

		/** 获得模型动画数目
		@return 模型动画数
		*/
		virtual uint	getAnimationCount() = 0;

		/** 获得某个动画
		@param index 下标
		@return 动画指针
		*/
		virtual Animation* getAnimation(uint index) = 0;

		/** 是否拥有某个动画
		@param animation 动画名称
		@see enAnimation
		@return 如果存在animation动画则返回此动画数据，否则返回0
		*/
		virtual Animation* hasAnimation(const std::string& animation) = 0;

		//获取模型的骨架
		virtual CoreSkeleton * getCoreSkeletion()=0;
	
		/** 获得子模型的数量
		@return 子模型的数量
		*/
		virtual uint getNumSubModels() = 0;

		/** 获得子模型指针
		@param name 子模型名称
		@return 子模型指针
		*/
		virtual ISubModel*	getSubModel(const char* name) = 0;

		/** 根据下标来获得子模型指针
		@param name 子模型下标
		@return 子模型指针
		*/
		virtual ISubModel*	getSubModel(int index) = 0;

		virtual IIndexBuffer*	getIndexBuffer() = 0;

		virtual IVertexBuffer*	getTexcoordBuffer() = 0;

		/**获得面数
		*/
		virtual uint getNumFaces() = 0;

		/**获得顶点数
		*/
		virtual uint getNumVertices() = 0;

		/**获得版本号
		*/
		virtual uint getVer() = 0;

		virtual IVertexBuffer* vertexBlend(Skeleton *pSkeleton,const AnimationTime *pTime) = 0;

		virtual uint numParticleSystem() = 0;

		virtual ParticleEmitter2Data*	getParticleSystemData(uint index) = 0;

		virtual uint numRibbonSystem() = 0;

		virtual RibbonEmitterData*	getRibbonSystemData(uint index) = 0;

		virtual uint numMmParticleSystem() = 0;

		virtual MmParticleSystemData* getMmParticleSystem(uint index) = 0;

		
		//计算指定时间的纹理映射,返回值：纹理通道的数量，AnimationTime:当前的时间
		virtual int texBlend(const AnimationTime *pTime){ return 0;}
		//获取指定时间的纹理缓存,返回值：指定通道的纹理缓存，pass：指定通道，pTime：指定时间
		virtual IVertexBuffer* getTexCoordBuffer(int pass){ return 0;}
		//判断模型是否具有uv动画
		virtual bool isAnimatedTexCoord(){ return false;}

		//MMParticleSystem
		virtual  void * getMMParticleSystemParam(unsigned int index, string paramname, string & type, int & count ){return 0;}
		virtual bool setMMParticleSystemParam(unsigned int index, string desname, void * psrc, int srcnum, string srctype){ return false;}
		virtual bool addMMParticleSystem(){return false;}
		virtual bool removeMMParticleSystem(unsigned int index){return false;}

		//ParticleSystem
		virtual  void * getParticleSystemParam(unsigned int index, string paramname, string & type, int & count){return 0;}
		virtual bool setParticleSystemParam(unsigned int index, string desname, void * psrc, int srcnum, string srctype){ return false;}
		virtual bool addParticleSystem(){return false;}
		virtual bool removeParticleSystem(unsigned int index){return false;}

		//Ribbon System
		virtual void * getRibbonSystemParam( unsigned int index, string paramname, string & type, int &count){return 0;}
		virtual bool setRibbonSystemParam( unsigned int index, string desname, void * psrc, int srcnum, string srctype){ return false;}
		virtual bool addRibbonSystem(){ return false;}
		virtual bool removeRibbonSystem(unsigned int index){ return false;}

		
		//将模型写到文件
		virtual bool write(string filename,float fCopress) { return false;}

		/**获得模型占用的内存
		*/
		virtual size_t getMomorySize() = 0;

		//扩展,用于模型的参数设定
		//获取boundingbox
		virtual void getBoundingBox( Vector4 &rect){ rect.x = -1.0f;rect.y =-1.0f;rect.z = 1.0f;rect.w = 1.0f;}
		//获取模型基本扩展参数
		virtual void * getExtParam(std::string strParamName, std::string & strType, int & count)
		{
			//现在有以下参数：
			//nodeScale：node的初始缩放
			strType ="";
			count = 0;
			return 0;
		}
		//设置模型基本参数
		virtual bool setExtParam( std::string strParamName, void * psrc, int srcnum, string srctype)
		{
			return false;
		}

		//设置用户定义渲染顺序
		virtual bool enableCustomizeRenderOrder(bool enable) { return false; };
		virtual bool isEnabledCustomizeRenderOrder() { return false;};
		enum{ ECustomizeRenderOrderObject_SubModel = 0, ECustomizeRenderOrderObject_ParticleSystem, ECustomizeRenderOrderObject_RibbonSystem,
		ECustomizeRenderOrderObject_MMParticleSystem, ECustomizeRenderOrderObject_Max,};
		virtual bool setCustomizeRenderOrder( uint objectType, uint index, int order){return false;};
		virtual int	getCustomizeRenderOrder(uint objectType, uint index){return 0;}		

		//是否需要绘制阴影
		virtual bool	needDrawShadow(){ return true;}

		//对硬件蒙皮的支持

		//获取顶点混合的骨骼权重buffer
		virtual IVertexBuffer *	getBlendWeightBuffer() { return 0;};
		//获取顶点混合的索引Buffer
		virtual IVertexBuffer * getBlendIndicesBuffer(uint subModelIndex) { return 0;}
		//获取初始的顶点buffer
		virtual IVertexBuffer * getOriginVertexBuffer() { return 0;}
		//获取顶点混合的shader,也可用于判断是否用硬件加速
		virtual IShaderProgram * getBlendShaderProgram() { return 0;};
	}; 

	/** 模型管理器接口
	*/
	struct IModelManager
	{
		/** 销毁模型管理器
		*/
		virtual void 		release() = 0;				

		virtual char*		getFirstModelExt() = 0;

		virtual char*		getNextModelExt() = 0;
		/** 加载模型
		@param fileName 模型文件名
		@return 模型接口指针
		*/
		virtual IModel*		loadModel(const char* fileName) = 0;

		/** 从文件名获取模型接口指针
		@param fileName 模型文件名
		@return 模型接口指针
		*/
		virtual IModel*		getByName(const char* fileName) = 0;					

		/** 销毁模型
		@param pModel 模型接口指针
		*/
		virtual void 		releaseModel(IModel* pModel) = 0;				

		/** 根据名字销毁模型
		@param fileName 模型文件名
		*/
		virtual void 		releaseModel(const char* fileName) = 0;				

		/** 销毁所有模型
		*/
		virtual void 		releaseAll() = 0;
	};
	//add by yhc 2010.1.29
	//去掉重复和多余的keyframe 
	//位置
	void CompressPos(KeyFrames<Vector3> &inKeyFramesPos,KeyFrames<Vector3> &outKeyFramesPos,float fRange);
	void CompressQua(KeyFrames<Quaternion>  &inKeyFramesQua,KeyFrames<Quaternion>  &outKeyFramesQua,float fRange);
			

	extern "C" __declspec(dllexport) IModelManager* LoadModelManager(IRenderSystem* pRenderSystem);
	/** @} */
}

#endif