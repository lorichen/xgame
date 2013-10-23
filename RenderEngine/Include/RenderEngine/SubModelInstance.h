#ifndef __SubModelInstance_H__
#define __SubModelInstance_H__

#include "IRenderable.h"
#include "Material.h"

namespace xs
{
	class ModelInstance;
	class ModelNode;


	class _RenderEngineExport SubModelInstance : public IRenderable
	{
	public:
		//IRenderable
		virtual void  render(IRenderSystem* pRenderSystem,bool useMtl);

	public:
		virtual const Matrix4& getWorldTransform() const;

		void setEyeAt(const Vector3& v);

		float getDistance();

		Material* getMaterial();

		Material* getOriginalMaterial();

		void setTexture(int layer,const char* szName);

		ITexture* getTexture(int layer);

		int getRenderOrder();

		void update(const AnimationTime *pAnimationTime,const ColorValue& diffuse);

		void updateColor(const ColorValue& diffuse);

		ushort numIndices();

		//更新子Mesh绑定的骨骼当前的矩阵
		void updateBonesMatrix(xs::Skeleton * pSkeleton);

	public:
		SubModelInstance(ModelInstance *pMI,ISubModel *pSubModel, uint subModelIndex);
		~SubModelInstance();

	public:
		const char* getName();

		/** 子模型是否可见
		@return 是否可见
		*/
		virtual bool isVisible();

		/** 设置子模型的可见性
		*/
		virtual void  setVisible(bool bVisible);

	private:
		void  renderCPUVertexBlend(IRenderSystem* pRenderSystem,bool useMtl);
		void  renderGPUVertexBlend(IRenderSystem* pRenderSystem,bool useMtl);	

	private:
		bool m_bVisible;
		Material	m_mtl,m_mtlOrigin;
		std::string m_name;
		ModelInstance*	m_pMI;
		ISubModel*		m_pSubModel;
		ushort indexStart,indexCount,vertexStart,vertexEnd;
		GeosetAnimation* m_pGeosetAnimation;
		float		m_GeosetAlpha;
		//用于支持骨骼的硬件混合
		//xs::Matrix4 m_BindedBonesMatrices[VERTEX_BLEND_MATRICES_NUM];//HighLevel shader使用的骨骼矩阵
		uint		m_uiSubModelIndex;
		float		 m_BindedBonesMatricesF[VERTEX_BLEND_MATRICES_NUM][3][4];//LowLevel shader 使用的骨骼矩阵
	};
}

#endif