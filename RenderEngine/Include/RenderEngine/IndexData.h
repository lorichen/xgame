#ifndef __IndexData_H__
#define __IndexData_H__

namespace xs
{
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/


	/** Summary class collecting together index data source information. */
	class _RenderEngineExport IndexData 
	{
	protected:
		/// Protected copy constructor, to prevent misuse
		IndexData(const IndexData& rhs); /* do nothing, should not use */
		/// Protected operator=, to prevent misuse
		IndexData& operator=(const IndexData& rhs); /* do not use */
	public:
		IndexData();
		~IndexData();
		/// pointer to the HardwareIndexBuffer to use, must be specified if bUseIndices = true
		IIndexBuffer* indexBuffer;

		/// index in the buffer to start from for this operation
		uint indexStart;

		/// The number of indexes to use from the buffer
		uint indexCount;
	};
	/** @} */

}
#endif