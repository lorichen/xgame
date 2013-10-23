#ifndef __ParticleSystem_H__
#define __ParticleSystem_H__

#include "Particle.h"
#include <string>
using namespace std;

namespace xs
{
	class Bone;
	class ModelNode;

	class _RenderEngineExport ParticleSystem : public IRenderable
	{
	public:
		/** 渲染
		@param pRenderSystem 渲染系统接口指针
		*/
		virtual void  render(IRenderSystem* pRenderSystem,bool useMtl);

	public:
		ParticleSystem();
		~ParticleSystem();

		void update(const AnimationTime *pTime,float tickDelta,IRenderSystem* pRenderSystem);
		int getRenderOrder();
		static void set25dMode(bool b25d);
		static void setPause(bool bPause);
		void reset();
		void setColor(const ColorValue& color);
		int __getCurParticleNum();
		void SetGenerateParticle(bool bGenerate);

	public:
		ParticleEmitter2Data*	m_pData;
		Bone*					m_pBone;
		typedef std::list<Particle> ParticleContainer;
		typedef ParticleContainer::iterator ParticleContainerIterator;
		ParticleContainer	m_vParticles;
		ITexture*			m_pTexture;
		float	m_currentEmission;
		ModelNode*			m_pNode;
		static bool	m_b25d;
		static bool	s_bPause;
		ColorValue			m_color;
		void Clear();//清除粒子系统的粒子
	private:
		bool initializeResource(IRenderSystem * pRenderSystem);//初始化粒子系统资源
		void finalizeResource();//释放粒子系统资源
		void renderVBO(IRenderSystem * pRenderSystem);//使用vbo渲染
		void renderManual(IRenderSystem * pRenderSystem);//使用手工渲染
		void saveContext(IRenderSystem *pRenderSystem);//保存渲染状态
		void loadContext(IRenderSystem *pRenderSystem);//加载渲染状态
		void setContext(IRenderSystem * pRenderSystem);//设置渲染状态

	private:
		//渲染状态备份/恢复信息
		SceneBlendFactor	m_sbfSrc,m_sbfDst;
		bool m_alphaCheck;
		float m_alphaCheckRef;
		CullingMode	m_cullingMode;
		CompareFunction	m_alphaFunc;
		bool		m_bDepthCheck;
		bool		m_bDepthWrite;
		bool		m_lightingEnabled;
		bool		m_bFogEnabled;

	private://用vbo绘制
		const static uint s_MaxVertices;//最大产生粒子数
		const static bool s_UseVBO;//是否使用vbo对象
		xs::IVertexBuffer * m_pVB;//position vertx buffer
		xs::IVertexBuffer * m_pTexVB;//texcoord vertex buffer
		xs::IVertexBuffer * m_pDiffuseVB;//diffuse vertex buffer
		xs::IIndexBuffer * m_pIB;//index buffer
		uint m_uiPreviousQuads;

	private:
		bool		m_bInitializeTexture;//是否已经出师化了纹理
		bool        m_bGenerate;
	private: //把产生粒子的函数独立出来
		void generateParticleByTime(const AnimationTime * pTime,float tickDelta,IRenderSystem* pRenderSystem);
		void generateParticleBySpace(const AnimationTime * pTime, float tickDelta,IRenderSystem* pRenderSystem);
	};
}

#endif