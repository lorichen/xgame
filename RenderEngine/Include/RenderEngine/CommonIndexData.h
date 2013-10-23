#ifndef __CommonIndexData_H__
#define __CommonIndexData_H__

namespace xs
{
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/

	/**
	*/
	class _RenderEngineExport SceneCommonData
	{
	public:
		struct SceneIndexData
		{
			IIndexBuffer*	indexBuffer;
			PrimitiveType	primitiveType;
		};
	public:
		/**
			RenderOperations,4 Type,2 lod per
		*/
		SceneIndexData	m_sIndexData[8];
		IVertexBuffer*	m_spDetailMapTexcoordVB;
		IVertexBuffer*	m_spAlphaMapTexcoordVB;
		IVertexBuffer*	m_spAlphaMapTexcoordVBWow;

		void	initialize(IRenderSystem *pRenderSystem);
		void finalize();

		IShader*			m_pFragmentShaders[4];
		IShader*			m_pVertexShader;
		IShaderProgram*	m_pShaderPrograms[4];
		IRenderSystem*	m_pRenderSystem;

		ITexture*		m_pDefaultTexture;
	protected:
		SceneCommonData(){}
	public:
		static SceneCommonData& getInstance()
		{
			static SceneCommonData scd;
			return scd;
		}
	};

	_RenderEngineExport SceneCommonData&  getSceneCommonData();
	/** @} */
}
#endif