#ifndef __SubModel_H__
#define __SubModel_H__

namespace xs
{
	struct IRenderSystem;
	class Model;

	class SubModel : public ISubModel
	{
	public:
		/** 获得子模型的名称
		@return 子模型的名称
		*/
		virtual const char* getName();

		virtual ushort getIndexStart();

		virtual ushort getIndexCount();

		virtual ushort getVertexStart();

		virtual ushort getVertexEnd();

		virtual const  Material&	getMaterial();

		virtual GeosetAnimation*	getGeosetAnimation();

		virtual bool	isAnimated();

		//对硬件蒙皮的支持

		virtual uchar *	 getBindedBonesIndices(uint & validIndicesNum);

	public:
		void  setMaterial(const Material& mtl);
	private:
		Material	m_mtl;

	public:
		SubModel();

	public:
		std::string	m_name;
		int		m_matIndex;
		ushort indexStart,indexCount,vertexStart,vertexEnd;
		bool	m_bAnimated;
		GeosetAnimation	m_GeosetAnimation;
		uchar	m_ValidIndices;//有效的骨骼索引数
		uchar   m_BindedBonesIndices[VERTEX_BLEND_MATRICES_NUM];//新的骨骼索引
		uchar	m_BonesMappings[VERTEX_BLEND_MAPPINGS_NUM];//骨骼索引映射表
	};
}

#endif