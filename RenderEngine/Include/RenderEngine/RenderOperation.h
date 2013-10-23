#ifndef __RenderOperation_H__
#define __RenderOperation_H__

namespace xs
{
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/


	class VertexData;
	class IndexData;

	/** rendering operation using vertex buffers. */
	class _RenderEngineExport RenderOperation 
	{
	public:

		/// Vertex source data
		VertexData *vertexData;

		/// The type of operation to perform
		PrimitiveType primitiveType;

		/** Specifies whether to use indexes to determine the vertices to use as input. If false, the vertices are
		simply read in sequence to define the primitives. If true, indexes are used instead to identify vertices
		anywhere in the buffer, and allowing vertices to be used more than once.
		If true, then the indexBuffer, indexStart and numIndexes properties must be valid. */
		bool bUseIndices;

		/// Index data - only valid if bUseIndices is true
		IndexData *indexData;

		RenderOperation() :	vertexData(0),primitiveType(PT_TRIANGLES),bUseIndices(true),indexData(0){}

	};

	enum VertexFormat
	{
		VF_V2F,
		VF_V3F,
		VF_C4UB_V2F,
		VF_C4UB_V3F,
		VF_C3F_V3F,
		VF_N3F_V3F,
		VF_C4F_N3F_V3F,
		VF_T2F_V3F,
		VF_T4F_V4F,
		VF_T2F_C4UB_V3F,
		VF_T2F_C3F_V3F,
		VF_T2F_N3F_V3F,
		VF_T2F_C4F_N3F_V3F,
		VF_T4F_C4F_N3F_V4F
	};

	class _RenderEngineExport RenderDirectVertexData
	{
	public:
		uchar*			pVertexData;
		VertexFormat	vertexFormat;
		uint			count;

		RenderDirectVertexData() : vertexFormat(VF_V2F),pVertexData(0),count(0){}
	};

	class _RenderEngineExport RenderDirectIndexData
	{
	public:
		IndexType	indexType;
		uint		count;
		uchar*		pIndexData;

		RenderDirectIndexData() : indexType(IT_16BIT),count(0),pIndexData(0){}
	};

	class _RenderEngineExport RenderDirect
	{
	public:
		PrimitiveType	primitiveType;
		RenderDirectVertexData vertexData;
		RenderDirectIndexData indexData;
		bool bUseIndices;

		RenderDirect() : primitiveType(PT_TRIANGLES),bUseIndices(false){}
	};
	/** @} */
}



#endif
